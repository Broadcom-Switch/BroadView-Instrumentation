/*****************************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  *
  * You may obtain a copy of the License at
  * http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ***************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include "broadview.h"
#include "packet_trace.h"
#include "packet_trace_debug.h"
#include "packet_trace_include.h"
#include "packet_trace_api.h"
#include "system.h"
#include "rest_api.h"
#include "system_utils_pcap.h"
#include "packet_trace_json_memory.h"
#include "packet_trace_json_encoder.h"
#include "openapps_log_api.h"
#include "sbplugin_redirect_system.h"

/* PT Context Info*/
extern BVIEW_PT_CXT_t pt_info;
/* PT Mutex*/
pthread_mutex_t *pt_mutex;



/*********************************************************************
 * @brief : function to return the api handler for the pt command type 
 *
 * @param[in] int : request type 
 *
 * @retval    : function pointer to the corresponding api 
 *
 * @note : 
 *
 *********************************************************************/
BVIEW_STATUS pt_type_api_get (int type, BVIEW_PT_API_HANDLER_t *handler)
{
  unsigned int i = 0;

  static const BVIEW_PT_API_t pt_api_list[] = {
    {BVIEW_PT_CMD_API_SET_FEATURE, pt_feature_set},
    {BVIEW_PT_CMD_API_CANCEL_TRACE_PROFILE, pt_trace_profile_cancel},
    {BVIEW_PT_CMD_API_CANCEL_LAG_RESOLUTION, pt_trace_profile_cancel},
    {BVIEW_PT_CMD_API_CANCEL_ECMP_RESOLUTION, pt_trace_profile_cancel},
    {BVIEW_PT_CMD_API_GET_FEATURE, pt_feature_get},
    {BVIEW_PT_CMD_API_GET_LAG_RESOLUTION, pt_trace_profile_get},
    {BVIEW_PT_CMD_API_GET_ECMP_RESOLUTION, pt_trace_profile_get},
    {BVIEW_PT_CMD_API_MATCH_PKT_RCVD, pt_trace_pkt_rcv_process},
    {BVIEW_PT_CMD_API_GET_TRACE_PROFILE, pt_trace_profile_get}
  };

  for (i = 0; i < BVIEW_PT_CMD_API_MAX-1; i++)
  {
    if (type == pt_api_list[i].pt_command)
    {
      *handler = pt_api_list[i].handler;
      return BVIEW_STATUS_SUCCESS;
    }
  }
  return BVIEW_STATUS_INVALID_PARAMETER;
}


/*********************************************************************
* @brief : packet trace main application function which does processing of messages
*
* @param[in] : none
*
* @retval  : BVIEW_STATUS_SUCCESS: Fails to register with module manager 
*             or some error happened due to which the application is
*             unable to process incoming messages.
* @retval  : BVIEW_STATUS_FAILURE: Fails to register with module manager 
*
* @note  : This api is the processing thread of the pt application. 
*          All the incoming requests are processed and the responses 
*          are sent in the packet trace context. Currently the assumption
*          is made that if the thread fails to read continously 10 or 
*          more messages, then there is some error and the thread exits.
*
*********************************************************************/
BVIEW_STATUS pt_app_main (void)
{
  BVIEW_PT_REQUEST_MSG_t msg_data;
  BVIEW_PT_RESPONSE_MSG_t reply_data;
  BVIEW_STATUS rv = BVIEW_STATUS_FAILURE;
  unsigned int rcvd_err = 0;
  unsigned int num_units = 0;
  BVIEW_PT_API_HANDLER_t handler;

  if (BVIEW_STATUS_SUCCESS != pt_module_register ())
  {
    /* registration with module mgr has failed.
       return failure. so that the caller can clean the resources */
             LOG_POST (BVIEW_LOG_EMERGENCY,
                 "Registration with module mgr failed \r\n");

    return BVIEW_STATUS_FAILURE;
  }

  if (BVIEW_STATUS_SUCCESS != sbapi_system_num_units_get ((int *) &num_units))
   {
    LOG_POST (BVIEW_LOG_ERROR, "Failed to get num of units\r\n");
    return BVIEW_STATUS_FAILURE;
   }


  while (1)
  {
    if (-1 != (msgrcv (pt_info.recvMsgQid, &msg_data, sizeof (msg_data), 0, 0))) 
    {
        _PT_LOG(_PT_DEBUG_INFO, "msg_data info\n"
            "msg_data.msg_type = %ld\n"
            "msg_data.unit = %d\n"
            "msg_data.cookie = %d\n",
            msg_data.msg_type, msg_data.unit, (NULL == msg_data.cookie)? true: false); 

      if (msg_data.unit >= num_units)
      {
        /* requested id is greater than the number of units.
           send failure message and continue */
        memset (&reply_data, 0, sizeof (BVIEW_PT_RESPONSE_MSG_t));
        reply_data.rv = BVIEW_STATUS_INVALID_PARAMETER;

        /* copy the request type */
        reply_data.msg_type = msg_data.msg_type;
        /* copy the unit */
        reply_data.unit = msg_data.unit;
        /* copy the asic type */
        reply_data.id = msg_data.id;
        /* copy the cookie ..  */
        reply_data.cookie = msg_data.cookie;

        rv = pt_send_response(&reply_data);
        if (BVIEW_STATUS_SUCCESS != rv)
        {
          LOG_POST (BVIEW_LOG_ERROR,
              "failed to send response for command %d , err = %d. \r\n", msg_data.msg_type, rv);
        }
        continue;
      }
      /* Counter to check for read errors.. 
        successfully read the message. clear this to 0*/
      rcvd_err = 0;
      /* Memset the response message */
      memset (&reply_data, 0, sizeof (BVIEW_PT_RESPONSE_MSG_t));

      /* get the api function for the method type */
      if (BVIEW_STATUS_SUCCESS != pt_type_api_get(msg_data.msg_type, &handler))
      {
        continue;
      }

      rv = handler(&msg_data);

      if ((true == msg_data.report_type_periodic) &&
          ((BVIEW_PT_CMD_API_GET_LAG_RESOLUTION == msg_data.msg_type)||
          (BVIEW_PT_CMD_API_GET_TRACE_PROFILE == msg_data.msg_type)||
          (BVIEW_PT_CMD_API_GET_ECMP_RESOLUTION == msg_data.msg_type)) &&
          (PT_5_TUPLE == msg_data.cmd.profile.req_method))
      {
        continue;
      }

      reply_data.rv = rv;

      rv = pt_copy_reply_params (&msg_data, &reply_data);
      if (BVIEW_STATUS_SUCCESS != rv)
      {
        _PT_LOG(_PT_DEBUG_ERROR, "pt_main.c failed to send response for command %ld , err = %d. \r\n", msg_data.msg_type, rv);
         LOG_POST (BVIEW_LOG_ERROR,
            "failed to send response for command %d , err = %d. \r\n", msg_data.msg_type, rv);
      }
      continue;
    }
    else
    {
      LOG_POST (BVIEW_LOG_ERROR,
          "Failed to read message from the pt application queue. err = %d\r\n", errno);
      /* increment the error counter. 
         If this counter increments continously exit and return failure
         so that corrective action can be taken */
         rcvd_err++;

         /* wait till 10 consective messages */
         if(BVIEW_PT_MAX_QUEUE_SEND_FAILS < rcvd_err)
         {
           break;
         }
        continue;
    }
  }                             /* while (1) */
             LOG_POST (BVIEW_LOG_EMERGENCY,
                 "exiting from pt pthread ... \r\n");
  pthread_exit(&pt_info.pt_thread);
  return BVIEW_STATUS_FAILURE;
}


/*********************************************************************
* @brief : function to initializes pt structures to default values 
*
* @param[in] : number of units on the agent
*
* @retval  : BVIEW_STATUS_SUCCESS : configuration is successfully initialized.
* @retval  : BVIEW_STATUS_FAILURE : Fail to initialize the PT appliation to default.
*
* @note  : Initialization functon to make sure both software and asic 
*          are configured with same default values.  
*
*********************************************************************/
BVIEW_STATUS pt_app_config_init (unsigned int num_units)
{
  BVIEW_PT_CFG_t *config_ptr;
  BVIEW_PT_INFO_t *data_ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

  int unit_id = 0;

  if (0 == num_units)
  {
    /* get the number of units on this device */
    LOG_POST (BVIEW_LOG_ERROR, "Failed to Init PT:Number of units are zero\r\n");
    return BVIEW_STATUS_FAILURE;
  }

  for (unit_id = 0; unit_id < num_units; unit_id++)
  {
    /* all the pt info we maintain is per unit. so get each unit 
       and configure the same.. */
    config_ptr = PT_CONFIG_PTR_GET (unit_id);

    if (NULL == config_ptr)
    {
      /* fatal error. cannot access the pt info.. */
      LOG_POST (BVIEW_LOG_ERROR, "Error!! pt config info memory is not accessible !!\r\n");
      return BVIEW_STATUS_FAILURE;
    }

    data_ptr = PT_OPER_PTR_GET (unit_id);
    if (NULL == data_ptr)
    {
      LOG_POST (BVIEW_LOG_ERROR, "Error!! pt memory is not accessible !!\r\n");
      return BVIEW_STATUS_FAILURE;
    }
    ptjson_memory_init();

    /* build defaults for config info */
    memset(config_ptr, 0, sizeof(BVIEW_PT_CFG_t));

    /* build defaults for oper info */
    memset(data_ptr, 0, sizeof(BVIEW_PT_INFO_t));

    data_ptr->current = pt_info.unit[unit_id].current_profile;
    data_ptr->active = pt_info.unit[unit_id].active_profile;

    memset(data_ptr->current, 0, sizeof(BVIEW_PT_PROFILE_RECORD_t));
    memset(data_ptr->active, 0, sizeof(BVIEW_PT_PROFILE_RECORD_t));

    /* Init packet mux call back related information */
    pt_info.pt_pkt_mux_hook.appID = BVIEW_FEATURE_LIVE_PT;
    strcpy(pt_info.pt_pkt_mux_hook.appFuncName, "pt_pkt_mux_cb");
    pt_info.pt_pkt_mux_hook.callbackFunc = pt_pkt_mux_cb;
    if (pkt_mux_register(&(pt_info.pt_pkt_mux_hook)) != BVIEW_STATUS_SUCCESS)
    {
      LOG_POST (BVIEW_LOG_ERROR, "Error!! Packet trace Failed to register with packet mux !!\r\n");
      return BVIEW_STATUS_FAILURE;

    }
  }

  LOG_POST (BVIEW_LOG_INFO, 
      "pt default initialization successful\r\n"); 
  return rv;
}


/*********************************************************************
* @brief : function to send reponse for encoding to cjson and sending 
*          using rest API 
*
* @param[in] reply_data : pointer to the response message
*
* @retval  : BVIEW_STATUS_SUCCESS : message is successfully using rest API. 
* @retval  : BVIEW_STATUS_FAILURE : message is unable to deliver using rest API.
* @retval  : BVIEW_STATUS_OUTOFMEMORY : Unable to allocate json buffer.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter to function.
*
* @note   : This function is invoked by the pt to send the trace profile and 
*           requested get configure params. This function internally
*           calls the encoding api to encode the data, and the memory
*           for the data is allocated. In case of both successful and 
*           unsuccessful send of the data, the memory must be freed.
*           
*********************************************************************/
BVIEW_STATUS pt_send_response (BVIEW_PT_RESPONSE_MSG_t * reply_data)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  uint8_t *pJsonBuffer = NULL;

  if (NULL == reply_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

        _PT_LOG(_PT_DEBUG_INFO, "Reply_data\n"
            "reply_data->msg_type = %ld\n"
            "reply_data->rv = %d\n"
            "reply_data->unit = %d\n"
            "reply_data->cookie = %d\n",
            reply_data->msg_type,reply_data->rv, reply_data->unit, (NULL == reply_data->cookie)? true : false); 

  if (reply_data->rv != BVIEW_STATUS_SUCCESS)
  {
    rest_response_send_error(reply_data->cookie, reply_data->rv, reply_data->id);
    return BVIEW_STATUS_SUCCESS;
  }
  else 
  {
    if ((BVIEW_PT_CMD_API_SET_FEATURE == reply_data->msg_type) ||
        (BVIEW_PT_CMD_API_CANCEL_TRACE_PROFILE == reply_data->msg_type) ||
        (BVIEW_PT_CMD_API_CANCEL_LAG_RESOLUTION == reply_data->msg_type) ||
        (BVIEW_PT_CMD_API_CANCEL_ECMP_RESOLUTION == reply_data->msg_type) ||
        ((PT_5_TUPLE == reply_data->options.req_method) &&
         (NULL != reply_data->cookie) &&
         ((BVIEW_PT_CMD_API_GET_TRACE_PROFILE == reply_data->msg_type) ||
          (BVIEW_PT_CMD_API_GET_LAG_RESOLUTION == reply_data->msg_type) ||
          (BVIEW_PT_CMD_API_GET_ECMP_RESOLUTION == reply_data->msg_type)))
        )
    {
      rest_response_send_ok (reply_data->cookie);
      return BVIEW_STATUS_SUCCESS;
    }

  } 

  /* Take lock*/
  PT_LOCK_TAKE (reply_data->unit);
  switch (reply_data->msg_type)
  {
    case BVIEW_PT_CMD_API_GET_FEATURE:

      /* call json encoder api for feature  */
      rv = ptjson_encode_get_pt_feature (reply_data->unit, reply_data->id,
                                         reply_data->response.feature, 
                                         &pJsonBuffer);
      break;


    case BVIEW_PT_CMD_API_GET_TRACE_PROFILE:
    case BVIEW_PT_CMD_API_GET_LAG_RESOLUTION:
    case BVIEW_PT_CMD_API_GET_ECMP_RESOLUTION:
      /*  call json encoder api for trace profile */

      rv = ptjson_encode_get_pt_profile (reply_data->unit,
                                          reply_data->response.profile,
                                          &reply_data->options,
                                          &pJsonBuffer); 

        break;
    default:
      break;
  }
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    /* free the allocated memory */
    if (NULL != pJsonBuffer)
    {
      ptjson_memory_free(pJsonBuffer);
    }
    PT_LOCK_GIVE(reply_data->unit);
    return rv;
  }
  if (NULL != pJsonBuffer)
  {
     _PT_LOG(_PT_DEBUG_TRACE,"sent response to rest, pJsonBuffer = %s, len = %d\r\n", pJsonBuffer, (int)strlen((char *)pJsonBuffer)); 

    rv = rest_response_send(reply_data->cookie, (char *)pJsonBuffer, strlen((char *)pJsonBuffer));
    if (BVIEW_STATUS_SUCCESS != rv)
    {
      _PT_LOG(_PT_DEBUG_ERROR, "sending response failed due to error = %d\r\n",rv);
      LOG_POST (BVIEW_LOG_ERROR,
          " sending response failed due to error = %d\r\n",rv);
    }
    else
    {
      _PT_LOG(_PT_DEBUG_TRACE,"sent response to rest, pJsonBuffer = %s, len = %d\r\n", pJsonBuffer, (int)strlen((char *)pJsonBuffer)); 
    }
    /* free the json buffer */
     ptjson_memory_free(pJsonBuffer);
  }
  else
  {
    /* Can happen that memory is not allocated
     */
    LOG_POST (BVIEW_LOG_ERROR,
        "encoding of pt response failed. due to failure of memory allocation\r\n");
  }
  /* release the lock for success and failed cases */
  PT_LOCK_GIVE(reply_data->unit);
  return rv;
}

/*********************************************************************
* @brief : function to prepare the response to the request message  
*
* @param[in] msg_data : pointer to the request message
* @param[out] reply_data : pointer to the response message
*
* @retval  : BVIEW_STATUS_FAILURE : if the copy to response message fails 
* @retval  : BVIEW_STATUS_SUCCESS :  response message is successfully prepred. 
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameters to function.
*                                           or null pointers to function
*
* @note  : This api copies the required information from input request 
*          message to response message.  
*
*********************************************************************/
BVIEW_STATUS pt_copy_reply_params (BVIEW_PT_REQUEST_MSG_t * msg_data,
                            BVIEW_PT_RESPONSE_MSG_t * reply_data)
{
  BVIEW_PT_INFO_t *ptr;
  BVIEW_PT_CFG_t *config_ptr;
  char pcap_file_data[PT_JSON_MAX_PKT_LEN] = {0};
  unsigned int pcap_file_len = 0;
  char *pkt_data = NULL;
  unsigned int pkt_len = 0; 

  if ((NULL == msg_data) || (NULL == reply_data))
    return BVIEW_STATUS_INVALID_PARAMETER;

  ptr = PT_OPER_PTR_GET (msg_data->unit);
  config_ptr = PT_CONFIG_PTR_GET (msg_data->unit);

  if ((NULL == ptr) || (NULL == config_ptr))
    return BVIEW_STATUS_INVALID_PARAMETER;

  /* copy the request type */
  reply_data->msg_type = msg_data->msg_type;
  /* copy the unit */
  reply_data->unit = msg_data->unit;
  /* copy the asic type */
  reply_data->id = msg_data->id;
  /* copy the cookie ..  */
  reply_data->cookie = msg_data->cookie;


  switch (msg_data->msg_type)
  {
    case BVIEW_PT_CMD_API_GET_TRACE_PROFILE:
    case BVIEW_PT_CMD_API_GET_LAG_RESOLUTION:
    case BVIEW_PT_CMD_API_GET_ECMP_RESOLUTION:
      {
        PT_LOCK_TAKE (msg_data->unit);
        reply_data->response.profile = ptr->current;
        ptr->current = ptr->active;
        ptr->active = reply_data->response.profile;
        PT_LOCK_GIVE (msg_data->unit);
        /* copy the options */
        reply_data->options.ig_tv_present = false;
        reply_data->options.eg_tv_present = false;
        reply_data->options.req_method = msg_data->cmd.profile.req_method;

        if (0 != msg_data->cmd.profile.req_prfl.pkt.packet_len)
        {
          /* if the packet is raw packet,
             convert the same to pcap format */
          if (PT_5_TUPLE == msg_data->cmd.profile.req_method)
          {
            if (system_convert_raw_pkt_to_pcap_format(&msg_data->cmd.profile.
                  req_prfl.pkt.packet[0],
                  msg_data->cmd.profile.req_prfl.pkt.packet_len, 
                  pcap_file_data, sizeof(pcap_file_data),
                  &pcap_file_len) != BVIEW_STATUS_SUCCESS)
            {
              LOG_POST (BVIEW_LOG_ERROR,
                  "PT:Failed to convert raw packet to pcap format\r\n");
              return BVIEW_STATUS_FAILURE;
            }

            pkt_data = &pcap_file_data[0];
            pkt_len = pcap_file_len;
          }
          else if (PT_PKT == msg_data->cmd.profile.req_method)
          {
            pkt_data = &msg_data->cmd.profile.req_prfl.pkt.packet[0];
            pkt_len = msg_data->cmd.profile.req_prfl.pkt.packet_len;
          }
          /* copy the packet and its length */
          memcpy (&reply_data->response.profile->rcvd_pkt.packet[0], 
              pkt_data, pkt_len);

          reply_data->response.profile->rcvd_pkt.packet_len = pkt_len;
        }


        if (BVIEW_PT_CMD_API_GET_TRACE_PROFILE == reply_data->msg_type)
        {
          reply_data->options.report_lag_ecmp = false;
          reply_data->options.report_lag = false;
        }
        if (BVIEW_PT_CMD_API_GET_LAG_RESOLUTION == reply_data->msg_type)
        {
          reply_data->options.report_lag_ecmp = true;
          reply_data->options.report_lag = true;
        }
        if (BVIEW_PT_CMD_API_GET_ECMP_RESOLUTION == reply_data->msg_type) 
        {
          reply_data->options.report_lag_ecmp = true;
          reply_data->options.report_lag = false;
        }
        /* check if the request is a periodic one */
        if (true == msg_data->report_type_periodic)
        {
          /* this report is of periodic type */
          reply_data->cookie = NULL;
        }
      }

      break;


    case BVIEW_PT_CMD_API_GET_FEATURE:
      reply_data->response.feature = &config_ptr->feature;
      break;

    default:
      break;
  }
  /* release the lock for success and failed cases */

  return pt_send_response(reply_data);
}

/*********************************************************************
* @brief   :  function to post message to the pt application  
*
* @param[in]  msg_data : pointer to the message request
*
* @retval  : BVIEW_STATUS_SUCCESS : if the message is successfully posted to PT queue.
* @retval  : BVIEW_STATUS_FAILURE : if the message is failed to send to pt
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameters to function.
*            
* @note  : all the apis and call back functions should use this api
*          to post the message to pt application.
*
*********************************************************************/
BVIEW_STATUS pt_send_request (BVIEW_PT_REQUEST_MSG_t * msg_data)
{
  int rv = BVIEW_STATUS_SUCCESS;

  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  if (-1 == msgsnd (pt_info.recvMsgQid, msg_data, sizeof (BVIEW_PT_REQUEST_MSG_t), IPC_NOWAIT))
  {
    LOG_POST (BVIEW_LOG_ERROR,
              "Failed to send message to pt application,  err = %d, %d\r\n",
              msg_data->msg_type, errno);
    rv = BVIEW_STATUS_FAILURE;
  }

  return rv;
}

/*********************************************************************
*  @brief:  callback function to send periodic trace profiles  
*
* @param[in]   sigval : Data passed with notification after timer expires
*
* @retval  : BVIEW_STATUS_SUCCESS : message is successfully posted to pt.
* @retval  : BVIEW_STATUS_FAILURE : failed to post message to pt.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameters to function.
*
* @note : when the periodic timer expires, this api is invoked in
*         the timer context. We just post the request for the report, while 
*        posting we mark the report type as periodic. The parameter returned 
*       from the callback are the reference to unit id, and the context id. 
*
*********************************************************************/
BVIEW_STATUS pt_periodic_collection_cb (union sigval sigval)
{
  BVIEW_PT_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;
  BVIEW_PT_TIMER_CONTEXT_t timer_context;

  memset(&timer_context, 0, sizeof(BVIEW_PT_TIMER_CONTEXT_t));
  memset(&msg_data, 0, sizeof(BVIEW_PT_REQUEST_MSG_t));
  msg_data.report_type_periodic = true;
  timer_context = (*(BVIEW_PT_TIMER_CONTEXT_t *)sigval.sival_ptr);
  msg_data.unit = timer_context.unit;
  msg_data.id = timer_context.index;
  msg_data.msg_type = timer_context.msg_type;
  msg_data.cmd.profile.req_method = timer_context.req_method;
  /* Send the message to the pt application */
  rv = pt_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "Failed to send periodic collection message to pt application. err = %d\r\n", rv);
       return BVIEW_STATUS_FAILURE;
  } 

  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
*  @brief:  function to clean up  pt application  
*
* @param[in]  none 
*
* @retval  : none
*
* @note  When in case of any error event, this api cleans up all the resources.
*         i.e it deletes the timer notes, if any are present.
*         destroy the mutexes. clears the memory and destroys the message queue.
*
*********************************************************************/
void pt_app_uninit ()
{
  int id = 0, req = 0, num_units;
  pthread_mutex_t *pt_mutex;

  pt_info.key1 = MSG_QUEUE_ID_TO_PT;

  pkt_mux_deregister(&(pt_info.pt_pkt_mux_hook));

  if (BVIEW_STATUS_SUCCESS != sbapi_system_num_units_get (&num_units))
  {
    LOG_POST (BVIEW_LOG_ERROR, "Failed to get num of units\r\n");
  }
  for (id = 0; id < num_units; id++)
  {
    /* if periodic collection is enabled
       delete the timer.
       loop through all the units and close
     */
    for (req = 0; req < BVIEW_PT_MAX_PROFILE_REQUESTS; req++)
    {
      pt_periodic_collection_timer_delete (id, req);
    }
    /* Destroy mutex */
    pt_mutex = &pt_info.unit[id].pt_mutex;
    pthread_mutex_destroy (pt_mutex);

    /* free all memeory */
    if (NULL != pt_info.unit[id].pt_config_data)
    {
      free (pt_info.unit[id].pt_config_data);
    }

    if (NULL != pt_info.unit[id].pt_oper_data)
    {
      free (pt_info.unit[id].pt_oper_data);
    }
  }
  
  /* check if the message queue already exists.
     If yes, we should delete the same */
  if (-1 == (pt_info.recvMsgQid = msgget(pt_info.key1, IPC_EXCL | IPC_CREAT | 0777)))
  {
    if (EEXIST == errno)
    {
      /* delete the message queue  */
      if (msgctl(pt_info.recvMsgQid, IPC_RMID, NULL) == -1) 
      {
        LOG_POST (BVIEW_LOG_ERROR,
            "Failed to destroy pt msgQ id, err = %d\n", errno);
      }
    }
  }
}

/*********************************************************************
* @brief : function to create the pt application thread.
*
* @param[in]  none 
*
* @retval  : BVIEW_STATUS_RESOURCE_NOT_AVAILABLE : Returns when memory is not available
* @retval  : BVIEW_STATUS_FAILURE : failed to create the message queue or thread. 
* @retval  : BVIEW_STATUS_SUCCESS : successful completion of task creation. 
* @retval  : BVIEW_STATUS_INIT_FAILED : PT initialization failed. 
*
* @note : Main function to invoke the pt thread. This function allocates 
*         the required memory for pt application. 
*         Creates the mutexes for the pt data. Creates the message queue
*         It assumes that the system is initialized before the pt 
* @retval  : none
*         thread is invoked. This api depends on the system variables 
*         such as number of units per device. Incase of no such 
*         need, the number of units can be #defined.
*
*********************************************************************/
BVIEW_STATUS pt_main ()
{
  unsigned int id = 0, num_units = 0;
  int rv = BVIEW_STATUS_SUCCESS;
  int recvMsgQid;
  struct msqid_ds ds;


  pt_info.key1 = MSG_QUEUE_ID_TO_PT;



  /* create mutex for all units */
  for (id = 0; id < BVIEW_PT_MAX_UNITS; id++)
  {
     /* locks */
    pt_mutex = &pt_info.unit[id].pt_mutex;
    pthread_mutex_init (pt_mutex, NULL);
    
  }

  /* get the number of units */
  if (BVIEW_STATUS_SUCCESS != sbapi_system_num_units_get ((int *) &num_units))
  {
    /* Free the resources allocated so far */
    pt_app_uninit ();

    LOG_POST (BVIEW_LOG_EMERGENCY,
              "Failed to number of units, Unable to start pt application\r\n");
    return BVIEW_STATUS_RESOURCE_NOT_AVAILABLE;
  }
  /* allocate memory for all units */
  for (id = 0; id < num_units; id++)
  {
    /* config data */
    pt_info.unit[id].pt_config_data =
      (BVIEW_PT_CFG_t *) malloc (sizeof (BVIEW_PT_CFG_t));

    /* oper data */
    pt_info.unit[id].pt_oper_data =
      (BVIEW_PT_INFO_t *) malloc (sizeof (BVIEW_PT_INFO_t));

    pt_info.unit[id].current_profile = (BVIEW_PT_PROFILE_RECORD_t *) malloc (sizeof(BVIEW_PT_PROFILE_RECORD_t));
    pt_info.unit[id].active_profile = (BVIEW_PT_PROFILE_RECORD_t *) malloc (sizeof(BVIEW_PT_PROFILE_RECORD_t));

    if ((NULL == pt_info.unit[id].pt_config_data) ||
        (NULL == pt_info.unit[id].pt_oper_data) ||
        (NULL == pt_info.unit[id].current_profile) ||
        (NULL == pt_info.unit[id].active_profile))
    {
      /* Free the resources allocated so far */
      pt_app_uninit ();

      LOG_POST (BVIEW_LOG_EMERGENCY,
                "Failed to allocate memory for pt application\r\n");
      return BVIEW_STATUS_RESOURCE_NOT_AVAILABLE;
    }
  }

    LOG_POST (BVIEW_LOG_INFO,
              "pt application: pt memory allocated successfully\r\n");

  if (BVIEW_STATUS_SUCCESS != pt_app_config_init (num_units))
  {
    return BVIEW_STATUS_INIT_FAILED;
  }


   /* Check if message queue is already exitsing */
  if (0 > (recvMsgQid = msgget(pt_info.key1, IPC_EXCL | IPC_CREAT | 0777)))
  {
    if (EEXIST == errno)
    {
      /* get the already existing  message queue id for pt */
      if ((recvMsgQid = msgget (pt_info.key1, IPC_CREAT | 0777)) < 0)
      {

	LOG_POST (BVIEW_LOG_EMERGENCY,
	    "Failed to create  msgQ for pt application. err = %d\n", errno);
	return  BVIEW_STATUS_FAILURE;
      }

      /* message queue exits..
	 Delete the message queue */
      if  (msgctl(recvMsgQid, IPC_RMID, (struct msqid_ds *)NULL) <0) 
      {
	LOG_POST (BVIEW_LOG_EMERGENCY,
	    "Failed to destroy pt msgQ id, err = %d\n", errno);
      }


      /* create the message queue for pt */
      if ((recvMsgQid = msgget (pt_info.key1, IPC_CREAT | 0777)) < 0)
      {
	/* Free the resources allocated so far */
	pt_app_uninit ();

	LOG_POST (BVIEW_LOG_EMERGENCY,
	    "Failed to create  msgQ for pt application. err = %d\n", errno);
	return  BVIEW_STATUS_FAILURE;
      }

      memset(&ds, 0, sizeof(struct msqid_ds));
      if (msgctl(recvMsgQid, IPC_STAT, &ds))
      {
        LOG_POST (BVIEW_LOG_EMERGENCY, "msgctl(recvMsgQid=%d, IPC_STAT failed: "
            "%s (errno=%d)\n", recvMsgQid, strerror(errno),errno);

        return BVIEW_STATUS_FAILURE;
      }

      ds.msg_qbytes = BVIEW_PT_MAX_QUEUE_SIZE;

      if (msgctl(recvMsgQid, IPC_SET,&ds))
      {
        LOG_POST (BVIEW_LOG_EMERGENCY,
            "msgctl(recvMsgQid=%d, IPC_SET, ...) failed "
            "(msg_perm.uid=%u," "msg_perm.cuid=%u): " "%s (errno=%d)\n",
            recvMsgQid, ds.msg_perm.uid, ds.msg_perm.cuid,  strerror(errno), errno);
      }
    }
  }
  pt_info.recvMsgQid = recvMsgQid;

   /* create pthread for pt application */
  if (0 != pthread_create (&pt_info.pt_thread, NULL, (void *) &pt_app_main, NULL))
  {
    LOG_POST (BVIEW_LOG_EMERGENCY, "pt  thread creation failed %d\r\n", errno);
    return  BVIEW_STATUS_FAILURE;
  }
    LOG_POST (BVIEW_LOG_INFO,
              "pt application: pt pthread created\r\n");


  return rv;
}



/*********************************************************************
 * @brief   Packet trace packet multiplexer callback function
 *
 * @param    pkt_info_ptr @b{(input)} Pointer to packet info
 *
 * @returns  BVIEW_STATUS_SUCCESS    if packet data is valid and posted
 *                                   pkt info to packet trace app
 * @returns  BVIEW_STATUS_FAILURE    if packet data is invalid and failed
 *                                   to post to 
 *                                  
 *
 * @notes    This function sends the packet and its info to packet trace 
 *                 application through message queue
 *
 * @end
 *********************************************************************/

BVIEW_STATUS pt_pkt_mux_cb(BVIEW_PACKET_MSG_t *pkt_info_ptr)
{
  BVIEW_PT_REQUEST_MSG_t   msg_data;
  BVIEW_STATUS              rv = BVIEW_STATUS_SUCCESS;
  struct msqid_ds pt_msgq_attr;

  /* Check the number of messages in msg queue */
  if (msgctl(pt_info.recvMsgQid, IPC_STAT, &pt_msgq_attr) == 0)
  {
    if (pt_msgq_attr.msg_qnum > (SYSTEM_MSGQ_MAX_MSG/2))
    {
      return BVIEW_STATUS_FAILURE;
    }
  }
  memset(&msg_data, 0, sizeof(BVIEW_PT_REQUEST_MSG_t));
 
 _PT_LOG(_PT_DEBUG_TRACE,"packet received from mux.\r\n"
    "received asic = %d, msg_type = %d, src port %d id %d\r\n"
   "packet len %d \r\n", pkt_info_ptr->packet.asic, 
    BVIEW_PT_CMD_API_MATCH_PKT_RCVD,
    pkt_info_ptr->packet.source_port,
    pkt_info_ptr->ltcRequestId,
    pkt_info_ptr->packet.pkt_len);

  /* Copy packet related information */  
  msg_data.unit = pkt_info_ptr->packet.asic;
  msg_data.msg_type = BVIEW_PT_CMD_API_MATCH_PKT_RCVD;
  msg_data.id = pkt_info_ptr->ltcRequestId;
  /* set the ingress port in the port mask */
  BVIEW_SETMASKBIT(msg_data.cmd.profile.pbmp, pkt_info_ptr->packet.source_port);
  /* Check packet length */
  if (pkt_info_ptr->packet.pkt_len > BVIEW_MAX_PACKET_SIZE)
  {
    LOG_POST (BVIEW_LOG_ERROR, 
        "Packet length fiven by packet multiplexer is greater than" 
        "the MAX supported length\r\n"); 

    return BVIEW_STATUS_FAILURE;
  }
  msg_data.cmd.profile.req_prfl.pkt.packet_len = pkt_info_ptr->packet.pkt_len;
  memcpy(msg_data.cmd.profile.req_prfl.pkt.packet, pkt_info_ptr->packet.data, pkt_info_ptr->packet.pkt_len);


 _PT_LOG(_PT_DEBUG_TRACE,"packet received is posted to packet trace application.\r\n");
  rv = pt_send_request (&msg_data);
  return rv;
}



