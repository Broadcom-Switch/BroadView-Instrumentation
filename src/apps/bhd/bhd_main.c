/************************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename bhd_main.c 
  *
  * @purpose BroadView BHD Application functions 
  *
  * @component Black hole detection 
  *
  * @comments
  *
  * @create 3/7/2016
  *
  * @author 
  * @end
  *
  **********************************************************************/


#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include "broadview.h"
#include "bhd.h"
#include "bhd_include.h"
#include "bhd_debug.h"  
#include "system.h"
#include "rest_api.h"
#include "configure_black_hole_detection.h"
#include "bhd_json_memory.h"
#include "bhd_json_encoder.h"
#include "openapps_log_api.h"
#include "sbplugin_redirect_system.h"
#include "sbplugin_redirect_bhd.h"
#include "system_utils_base64.h"
#include "system_utils_pcap.h"

/* BHD Context Info*/
extern BVIEW_BHD_CXT_t bhd_info;
/* BHD Mutex*/
pthread_mutex_t *bhd_mutex;

static const BVIEW_BHD_API_t bhd_api_list[] = {
  {BVIEW_BHD_CMD_API_BLACK_HOLE_DETECTION_ENABLE, bhd_feature_set},
  {BVIEW_BHD_CMD_API_CONFIGURE_BLACK_HOLE, bhd_configure_black_hole},
  {BVIEW_BHD_CMD_API_CANCEL_BLACK_HOLE, bhd_cancel_black_hole},
  {BVIEW_BHD_CMD_API_GET_BLACK_HOLE_DETECTION_ENABLE, bhd_feature_get},
  {BVIEW_BHD_CMD_API_GET_BLACK_HOLE, bhd_get_black_hole},
  {BVIEW_BHD_CMD_API_GET_BLACK_HOLE_EVENT_REPORT, bhd_process_black_hole_event},
  {BVIEW_BHD_CMD_API_GET_SFLOW_SAMPLING_STATUS, bhd_sflow_sample_status_get},
  {BVIEW_BHD_CMD_API_EVENT_TIME_PERIOD_START, bhd_start_time_period}
};

/*********************************************************************
 * @brief : function to return the api handler for the bhd command type 
 *
 * @param[in] int : request type 
 *
 * @retval    : function pointer to the corresponding api 
 *
 * @note : 
 *
 *********************************************************************/
BVIEW_STATUS bhd_type_api_get (int type, BVIEW_BHD_API_HANDLER_t *handler)
{
  unsigned int i = 0;

  for (i = 0; i < BVIEW_BHD_CMD_API_MAX-1; i++)
  {
    if (type == bhd_api_list[i].bhd_command)
    {
      *handler = bhd_api_list[i].handler;
      return BVIEW_STATUS_SUCCESS;
    }
  }
  return BVIEW_STATUS_INVALID_PARAMETER;
}

/*********************************************************************
 * @brief : BHD main application function which does processing of messages
 *
 * @param[in] : none
 *
 * @retval  : BVIEW_STATUS_SUCCESS: Fails to register with module manager 
 *             or some error happened due to which the application is
 *             unable to process incoming messages.
 * @retval  : BVIEW_STATUS_FAILURE: Fails to register with module manager 
 *
 * @note  : This api is the processing thread of the BHD application. 
 *          All the incoming requests are processed and the responses 
 *          are sent in the BHD context. Currently the assumption
 *          is made that if the thread fails to read continously 10 or 
 *          more messages, then there is some error and the thread exits.
 *
 *********************************************************************/
BVIEW_STATUS bhd_app_main (void)
{
  BVIEW_BHD_REQUEST_MSG_t msg_data;
  BVIEW_BHD_RESPONSE_MSG_t reply_data;
  BVIEW_STATUS rv = BVIEW_STATUS_FAILURE;
  unsigned int rcvd_err = 0;
  unsigned int num_units = 0;
  BVIEW_BHD_API_HANDLER_t handler;

  if (BVIEW_STATUS_SUCCESS != bhd_module_register ())
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
    if (-1 != (msgrcv (bhd_info.recvMsgQid, &msg_data, sizeof (msg_data), 0, 0))) 
    {
      _BHD_LOG(_BHD_DEBUG_INFO, "msg_data info\n"
          "msg_data.msg_type = %ld\n"
          "msg_data.unit = %d\n"
          "msg_data.cookie = %d\n",
          msg_data.msg_type, msg_data.unit, (NULL == msg_data.cookie)? true: false); 

      if (msg_data.unit >= num_units)
      {
        /* requested id is greater than the number of units.
           send failure message and continue */
        memset (&reply_data, 0, sizeof (BVIEW_BHD_RESPONSE_MSG_t));
        reply_data.rv = BVIEW_STATUS_INVALID_PARAMETER;

        /* copy the request type */
        reply_data.msg_type = msg_data.msg_type;
        /* copy the unit */
        reply_data.unit = msg_data.unit;
        /* copy the asic type */
        reply_data.id = msg_data.id;
        /* copy the cookie ..  */
        reply_data.cookie = msg_data.cookie;

        rv = bhd_send_response(&reply_data);
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
      memset (&reply_data, 0, sizeof (BVIEW_BHD_RESPONSE_MSG_t));

      /* get the api function for the method type */
      if (BVIEW_STATUS_SUCCESS != bhd_type_api_get(msg_data.msg_type, &handler))
      {
        continue;
      }

      rv = handler(&msg_data);

      if ((rv != BVIEW_STATUS_SUCCESS) && 
          (msg_data.msg_type == BVIEW_BHD_CMD_API_GET_BLACK_HOLE_EVENT_REPORT))
      {
        continue;
      } 
      if (msg_data.msg_type == BVIEW_BHD_CMD_API_EVENT_TIME_PERIOD_START)
      {      
        continue;
      } 
      reply_data.rv = rv;
      rv = bhd_copy_reply_params (&msg_data, &reply_data);
      if (BVIEW_STATUS_SUCCESS != rv)
      {
        _BHD_LOG(_BHD_DEBUG_ERROR, "BHD: failed to send response for command %ld , err = %d. \r\n", msg_data.msg_type, rv);
      }
      continue;
    }
    else
    {
      LOG_POST (BVIEW_LOG_ERROR,
          "Failed to read message from the BHD application queue. err = %d\r\n", errno);
      /* increment the error counter. 
         If this counter increments continously exit and return failure
         so that corrective action can be taken */
      rcvd_err++;

      /* wait till 10 consective messages */
      if(BVIEW_BHD_MAX_QUEUE_SEND_FAILS < rcvd_err)
      {
        break;
      }
      continue;
    }
  }                             /* while (1) */
  LOG_POST (BVIEW_LOG_EMERGENCY,
      "exiting from BHD pthread ... \r\n");
  pthread_exit(&bhd_info.bhd_thread);
  return BVIEW_STATUS_FAILURE;
}


/*********************************************************************
 * @brief : function to initializes BHD structures to default values 
 *
 * @param[in] : number of units on the agent
 *
 * @retval  : BVIEW_STATUS_SUCCESS : configuration is successfully initialized.
 * @retval  : BVIEW_STATUS_FAILURE : Fail to initialize the appliation to default.
 *
 *
 *********************************************************************/
BVIEW_STATUS bhd_app_config_init (unsigned int num_units)
{
  BVIEW_BHD_CONFIG_t  *config_ptr;
  BVIEW_BHD_INFO_t *data_ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  int sampling_capabilities = 0;

  int unit_id = 0;

  if (0 == num_units)
  {
    /* get the number of units on this device */
    LOG_POST (BVIEW_LOG_ERROR, "Failed to Init BHD:Number of units are zero\r\n");
    return BVIEW_STATUS_FAILURE;
  }

  for (unit_id = 0; unit_id < num_units; unit_id++)
  {
    /* all the bhd info we maintain is per unit. so get each unit 
       and configure the same.. */
    config_ptr = BHD_CONFIG_PTR_GET (unit_id);

    if (NULL == config_ptr)
    {
      /* fatal error. cannot access the BHD info.. */
      LOG_POST (BVIEW_LOG_ERROR, "Error!! BHD config info memory is not accessible !!\r\n");
      return BVIEW_STATUS_FAILURE;
    }

    data_ptr = BHD_OPER_PTR_GET (unit_id);
    if (NULL == data_ptr)
    {
      LOG_POST (BVIEW_LOG_ERROR, "Error!! BHD oper memory is not accessible !!\r\n");
      return BVIEW_STATUS_FAILURE;
    }
    bhdjson_memory_init();

    /* build defaults for config info */
    memset(config_ptr, 0, sizeof(BVIEW_BHD_CONFIG_t));

    /* Build defaults for operating info */ 
    if (NULL == data_ptr->current_sample_pkt_info)
    {
      LOG_POST (BVIEW_LOG_ERROR, "Error!! BHD operating sample pkt info memory is not accessible !!\r\n");
      return BVIEW_STATUS_FAILURE;
    }
    memset(data_ptr->current_sample_pkt_info, 0, sizeof(BVIEW_BHD_SAMPLED_PKT_INFO_t));

    if (NULL == data_ptr->latest_bhd_report)
    {
      LOG_POST (BVIEW_LOG_ERROR, "Error!! BHD operating latest bhd report memory is not accessible !!\r\n");
      return BVIEW_STATUS_FAILURE;
    }
    memset(data_ptr->latest_bhd_report, 0, sizeof(BHD_BLACK_HOLE_EVENT_REPORT_t));

    if (NULL == data_ptr->sflow_sampling_status)
    {
      LOG_POST (BVIEW_LOG_ERROR, "Error!! BHD operating sflow sampling status memory is not accessible !!\r\n");
      return BVIEW_STATUS_FAILURE;
    }
    memset(data_ptr->sflow_sampling_status, 0, sizeof(BVIEW_BHD_SFLOW_SAMPLING_STATUS_t));
    /* Update sampling capabilities */
    if (sbapi_bhd_asic_sampling_capabilties_get(unit_id, &sampling_capabilities) == BVIEW_STATUS_SUCCESS)
    {
      data_ptr->sampling_capabilities = sampling_capabilities;
      LOG_POST (BVIEW_LOG_INFO, 
          "BHD default initialization: sampling capabilities are %d\r\n", sampling_capabilities); 
    }
    else
    {
      LOG_POST (BVIEW_LOG_ERROR, 
          "BHD default initialization: Failed to get sampling capabilities \r\n"); 
      return BVIEW_STATUS_FAILURE;
    }
    data_ptr->start_sampling = false;
    /* Init packet mux call back related information */
    data_ptr->bhd_pkt_mux_hook.appID = BVIEW_FEATURE_BHD;
    strcpy(data_ptr->bhd_pkt_mux_hook.appFuncName, "bhd_pkt_mux_cb");  
    data_ptr->bhd_pkt_mux_hook.callbackFunc = bhd_pkt_mux_cb;
  }

  LOG_POST (BVIEW_LOG_INFO, 
      "BHD default initialization successful\r\n"); 
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
 * @note   : This function is invoked by the BHD to send the BHD event report and 
 *           requested get configure params. This function internally
 *           calls the encoding api to encode the data, and the memory
 *           for the data is allocated. In case of both successful and 
 *           unsuccessful send of the data, the memory must be freed.
 *           
 *********************************************************************/
BVIEW_STATUS bhd_send_response (BVIEW_BHD_RESPONSE_MSG_t *reply_data)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  uint8_t *pJsonBuffer = NULL;

  if (NULL == reply_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  _BHD_LOG(_BHD_DEBUG_INFO, "Reply_data\n"
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
    if ((BVIEW_BHD_CMD_API_BLACK_HOLE_DETECTION_ENABLE == reply_data->msg_type) ||
        (BVIEW_BHD_CMD_API_CONFIGURE_BLACK_HOLE == reply_data->msg_type) ||
        (BVIEW_BHD_CMD_API_CANCEL_BLACK_HOLE == reply_data->msg_type)) 
    {
      rest_response_send_ok (reply_data->cookie);
      return BVIEW_STATUS_SUCCESS;
    }

  } 

  /* Take lock*/
  BHD_LOCK_TAKE (reply_data->unit);
  switch (reply_data->msg_type)
  {
    case BVIEW_BHD_CMD_API_GET_BLACK_HOLE_DETECTION_ENABLE:

      /* call json encoder api for feature  */
      rv = bhdjson_encode_get_black_hole_detection (reply_data->unit, reply_data->id,
          reply_data->response.feature, 
          &pJsonBuffer);
      break;


    case BVIEW_BHD_CMD_API_GET_BLACK_HOLE:
      /*  call json encoder api for get black hole */
      rv = bhdjson_encode_get_black_hole(reply_data->unit, reply_data->id,
          reply_data->response.black_hole_config_status,
          &pJsonBuffer); 

      break;

    case BVIEW_BHD_CMD_API_GET_BLACK_HOLE_EVENT_REPORT:
      /*  call json encoder api for get black hole event report */
      rv = bhdjson_encode_get_black_hole_event_report(reply_data->unit, reply_data->id,
          reply_data->response.bhd_event_report,
          &(reply_data->response.bhd_event_report->report_time),  
          &pJsonBuffer); 
      break; 

    case BVIEW_BHD_CMD_API_GET_SFLOW_SAMPLING_STATUS:
      /*  call json encoder api for get sflow sampling status */
      rv = bhdjson_encode_get_sampling_status(reply_data->unit, reply_data->id,
          reply_data->response.bhd_sflow_sampling,
          &(reply_data->response.bhd_sflow_sampling->report_time),
          &pJsonBuffer); 
      break; 

    default:
      break;
  }

  if (NULL != pJsonBuffer && BVIEW_STATUS_SUCCESS == rv)
  {
    _BHD_LOG(_BHD_DEBUG_TRACE,"sent response to rest, pJsonBuffer = %s, len = %d\r\n", pJsonBuffer, (int)strlen((char *)pJsonBuffer)); 

    rv = rest_response_send(reply_data->cookie, (char *)pJsonBuffer, strlen((char *)pJsonBuffer));
    if (BVIEW_STATUS_SUCCESS != rv)
    {
      _BHD_LOG(_BHD_DEBUG_ERROR, "sending response failed due to error = %d\r\n",rv);
      LOG_POST (BVIEW_LOG_ERROR,
          " sending response failed due to error = %d\r\n",rv);
    }
    else
    {
      _BHD_LOG(_BHD_DEBUG_TRACE,"sent response to rest, pJsonBuffer = %s, len = %d\r\n", pJsonBuffer, (int)strlen((char *)pJsonBuffer)); 
    }
    /* free the json buffer */
    if (NULL != pJsonBuffer)
    {
      bhdjson_memory_free(pJsonBuffer);
    }
  }
  else
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "encoding of BHD response failed due to error = %d\r\n", rv);
    /* Can happen that memory is allocated,
       but the encoding failed.. in that case also 
       free the json buffer.
     */
    if (NULL != pJsonBuffer)
    {
      bhdjson_memory_free(pJsonBuffer);
    }
  }
  /* release the lock for success and failed cases */
  BHD_LOCK_GIVE(reply_data->unit);
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
BVIEW_STATUS bhd_copy_reply_params (BVIEW_BHD_REQUEST_MSG_t * msg_data,
    BVIEW_BHD_RESPONSE_MSG_t * reply_data)
{
  BVIEW_BHD_INFO_t *ptr;
  BVIEW_BHD_CONFIG_t *config_ptr;
  char *pkt_data = NULL;
  unsigned int pkt_len = 0; 
  unsigned int json_pkt_len = 0;
  char pcap_file_data[BVIEW_BHD_MAX_PCAP_PKT_SIZE] = {0};
  unsigned int pcap_file_len = 0;
  BHD_BLACK_HOLE_EVENT_REPORT_t *bhd_event_report_ptr = NULL;
  BVIEW_BHD_SAMPLED_PKT_INFO_t  *sample_pkt_info_ptr = NULL;

  if ((NULL == msg_data) || (NULL == reply_data))
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "%s (%d): Invalid request/reply pointers\r\n", __FILE__, __LINE__);
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  ptr = BHD_OPER_PTR_GET (msg_data->unit);
  config_ptr = BHD_CONFIG_PTR_GET (msg_data->unit);

  if ((NULL == ptr) || (NULL == config_ptr))
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "%s (%d): Invalid config/oper pointers\r\n", __FILE__, __LINE__);
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

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
    case BVIEW_BHD_CMD_API_GET_BLACK_HOLE_DETECTION_ENABLE:
      reply_data->response.feature = &config_ptr->feature;
      break;

    case BVIEW_BHD_CMD_API_GET_BLACK_HOLE:
      reply_data->response.black_hole_config_status = &config_ptr->bh_config;
      break;

    case BVIEW_BHD_CMD_API_GET_BLACK_HOLE_EVENT_REPORT:
      pkt_data = (char *)msg_data->cmd.sample_packet.pkt_data;
      pkt_len  = msg_data->cmd.sample_packet.pkt_len;

      sample_pkt_info_ptr =  &(msg_data->cmd.sample_packet);
      reply_data->response.bhd_event_report = ptr->latest_bhd_report; 
      bhd_event_report_ptr = (reply_data->response.bhd_event_report);

      if (bhd_event_report_ptr == NULL)
      {
        LOG_POST (BVIEW_LOG_ERROR,
            "%s(%d): latest bhd report memory in oper is inaccessible\r\n");
        return BVIEW_STATUS_FAILURE; 
      }

      memset(bhd_event_report_ptr, 0x00, sizeof(BHD_BLACK_HOLE_EVENT_REPORT_t)); 
      /*Convert to pcap packet */ 
      if (system_convert_raw_pkt_to_pcap_format(pkt_data, pkt_len,
            pcap_file_data, sizeof(pcap_file_data),
            &pcap_file_len) != BVIEW_STATUS_SUCCESS)
      {
        LOG_POST (BVIEW_LOG_ERROR,
            "BHD:Failed to convert raw packet to pcap format\r\n");
        return BVIEW_STATUS_FAILURE; 
      }
      memset(bhd_event_report_ptr->json_pkt, 0x00, BHD_JSON_MAX_PKT_LEN);
      /* Do base64 encoding */
      if (system_convert_pcap_to_b64_data(pcap_file_data, pcap_file_len,
            (char *)bhd_event_report_ptr->json_pkt, 
            BHD_JSON_MAX_PKT_LEN, &json_pkt_len) != BVIEW_STATUS_SUCCESS)
      {
        LOG_POST (BVIEW_LOG_ERROR,
            "BHD:Failed to convert pcap formatted packet to base64\r\n");
        return BVIEW_STATUS_FAILURE; 
      }
      bhd_event_report_ptr->ing_port = sample_pkt_info_ptr->ing_port; 
      bhd_event_report_ptr->egr_pbmp = sample_pkt_info_ptr->egr_pbmp;
      sbapi_bhd_black_holed_pkt_count_get(msg_data->unit, &bhd_event_report_ptr->total_black_holed_pkts);
      sbapi_system_time_get(&bhd_event_report_ptr->report_time);
      /* Increment the sample count in the current period*/
      ptr->current_sample_count++; 
      break;

    case BVIEW_BHD_CMD_API_GET_SFLOW_SAMPLING_STATUS:
      reply_data->response.bhd_sflow_sampling = ptr->sflow_sampling_status;
      break;

    case BVIEW_BHD_CMD_API_BLACK_HOLE_DETECTION_ENABLE:
    case BVIEW_BHD_CMD_API_CONFIGURE_BLACK_HOLE:
    case BVIEW_BHD_CMD_API_CANCEL_BLACK_HOLE:
      break;    

    default:
      break;
  }
  /* release the lock for success and failed cases */

  return  bhd_send_response(reply_data);
}

/*********************************************************************
 * @brief   :  function to post message to the BHD application  
 *
 * @param[in]  msg_data : pointer to the message request
 *
 * @retval  : BVIEW_STATUS_SUCCESS : if the message is successfully posted to BHD queue.
 * @retval  : BVIEW_STATUS_FAILURE : if the message is failed to send to pt
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameters to function.
 *            
 * @note  : all the apis and call back functions should use this api
 *          to post the message to BHD application.
 *
 *********************************************************************/
BVIEW_STATUS bhd_send_request (BVIEW_BHD_REQUEST_MSG_t * msg_data)
{
  int rv = BVIEW_STATUS_SUCCESS;

  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  if (-1 == msgsnd (bhd_info.recvMsgQid, msg_data, sizeof (BVIEW_BHD_REQUEST_MSG_t), IPC_NOWAIT))
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "Failed to send message to BHD application,  err = %d, %d\r\n",
        msg_data->msg_type, errno);
    rv = BVIEW_STATUS_FAILURE;
  }

  return rv;
}


/*********************************************************************
 *  @brief:  function to clean up  BHD application  
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
void bhd_app_uninit ()
{
  int id = 0,  num_units;
  pthread_mutex_t *bhd_mutex;

  bhd_info.key1 = MSG_QUEUE_ID_TO_BHD;

  if (BVIEW_STATUS_SUCCESS != sbapi_system_num_units_get (&num_units))
  {
    LOG_POST (BVIEW_LOG_ERROR, "Failed to get num of units\r\n");
  }

  for (id = 0; id < num_units; id++)
  {
    /* Clear blackhole, if any is configured */
    sbapi_bhd_cancel_black_hole(id);
    /* Deregister with packet multiplexer */
    pkt_mux_deregister(&(bhd_info.unit[id].bhd_oper_data->bhd_pkt_mux_hook));
    /* if periodic collection is enabled
       delete the timer.
       loop through all the units and close
     */
    bhd_periodic_sampling_timer_delete (id);
    /* Destroy mutex */
    bhd_mutex = &bhd_info.unit[id].bhd_mutex;
    pthread_mutex_destroy (bhd_mutex);

    /* free all memeory */
    if (NULL != bhd_info.unit[id].bhd_config_data)
    {
      free (bhd_info.unit[id].bhd_config_data);
    }

    if (NULL != bhd_info.unit[id].bhd_oper_data)
    {
      if (NULL != bhd_info.unit[id].bhd_oper_data->current_sample_pkt_info)
      {
        free(bhd_info.unit[id].bhd_oper_data->current_sample_pkt_info);   
      }
      if (NULL != bhd_info.unit[id].bhd_oper_data->latest_bhd_report)
      {
        free(bhd_info.unit[id].bhd_oper_data->latest_bhd_report);   
      }
      if (NULL != bhd_info.unit[id].bhd_oper_data->sflow_sampling_status)
      {
        free(bhd_info.unit[id].bhd_oper_data->sflow_sampling_status);   
      }
      free (bhd_info.unit[id].bhd_oper_data);
    }
  }


  /* check if the message queue already exists.
     If yes, we should delete the same */
  if (-1 == (bhd_info.recvMsgQid = msgget(bhd_info.key1, IPC_EXCL | IPC_CREAT | 0777)))
  {
    if (EEXIST == errno)
    {
      /* delete the message queue  */
      if (msgctl(bhd_info.recvMsgQid, IPC_RMID, NULL) == -1) 
      {
        LOG_POST (BVIEW_LOG_ERROR,
            "Failed to destroy BHD msgQ id, err = %d\n", errno);
      }
    }
  }
}

/*********************************************************************
 * @brief : function to create the bhd application thread.
 *
 * @param[in]  none 
 *
 * @retval  : BVIEW_STATUS_RESOURCE_NOT_AVAILABLE : Returns when memory is not available
 * @retval  : BVIEW_STATUS_FAILURE : failed to create the message queue or thread. 
 * @retval  : BVIEW_STATUS_SUCCESS : successful completion of task creation. 
 * @retval  : BVIEW_STATUS_INIT_FAILED : BHD initialization failed. 
 *
 * @note : Main function to invoke the bhd thread. This function allocates 
 *         the required memory for bhd application. 
 *         Creates the mutexes for the bhd data. Creates the message queue
 *         It assumes that the system is initialized before the bhd 
 * @retval  : none
 *         thread is invoked. This api depends on the system variables 
 *         such as number of units per device. Incase of no such 
 *         need, the number of units can be #defined.
 *
 *********************************************************************/
BVIEW_STATUS bhd_main ()
{
  unsigned int id = 0, num_units = 0;
  int rv = BVIEW_STATUS_SUCCESS;
  int recvMsgQid;
  struct msqid_ds ds;
  pthread_mutex_t  *bhd_mutex;
  BVIEW_BHD_INFO_t *oper_ptr = NULL;

  bhd_info.key1 = MSG_QUEUE_ID_TO_BHD;

  /* create mutex for all units */
  for (id = 0; id < BVIEW_BHD_MAX_UNITS; id++)
  {
    /* locks */
    bhd_mutex = &bhd_info.unit[id].bhd_mutex;
    pthread_mutex_init (bhd_mutex, NULL);

  }

  /* get the number of units */
  if (BVIEW_STATUS_SUCCESS != sbapi_system_num_units_get ((int *) &num_units))
  {
    /* Free the resources allocated so far */
    bhd_app_uninit ();

    LOG_POST (BVIEW_LOG_EMERGENCY,
        "Failed to get number of units, Unable to start bhd application\r\n");
    return BVIEW_STATUS_RESOURCE_NOT_AVAILABLE;
  }
  /* allocate memory for all units */
  for (id = 0; id < num_units; id++)
  {
    /* config data */
    bhd_info.unit[id].bhd_config_data =
      (BVIEW_BHD_CONFIG_t *) malloc (sizeof (BVIEW_BHD_CONFIG_t));

    /* oper data */
    bhd_info.unit[id].bhd_oper_data =
      (BVIEW_BHD_INFO_t *) malloc (sizeof (BVIEW_BHD_INFO_t));

    if ((NULL == bhd_info.unit[id].bhd_config_data) ||
        (NULL == bhd_info.unit[id].bhd_oper_data) )
    {
      /* Free the resources allocated so far */
      bhd_app_uninit ();

      LOG_POST (BVIEW_LOG_EMERGENCY,
          "%s  %d: Failed to allocate memory for bhd application\r\n", __FILE__, __LINE__);
      return BVIEW_STATUS_RESOURCE_NOT_AVAILABLE;
    }

    oper_ptr = bhd_info.unit[id].bhd_oper_data;

    if (oper_ptr != NULL)
    {
      memset(oper_ptr, 0, sizeof(BVIEW_BHD_INFO_t));
      oper_ptr->current_sample_pkt_info = (BVIEW_BHD_SAMPLED_PKT_INFO_t *) 
        malloc (sizeof(BVIEW_BHD_SAMPLED_PKT_INFO_t));
      oper_ptr->latest_bhd_report = (BHD_BLACK_HOLE_EVENT_REPORT_t *) 
        malloc (sizeof(BHD_BLACK_HOLE_EVENT_REPORT_t));
      oper_ptr->sflow_sampling_status = (BVIEW_BHD_SFLOW_SAMPLING_STATUS_t *) 
        malloc (sizeof(BVIEW_BHD_SFLOW_SAMPLING_STATUS_t));
      if ((NULL == oper_ptr->current_sample_pkt_info) ||
          (NULL == oper_ptr->latest_bhd_report)        ||
          (NULL == oper_ptr->sflow_sampling_status))
      {
        /* Free the resources allocated so far */
        bhd_app_uninit ();

        LOG_POST (BVIEW_LOG_EMERGENCY,
            "%s  %d: Failed to allocate memory for bhd application\r\n", __FILE__, __LINE__);
        return BVIEW_STATUS_RESOURCE_NOT_AVAILABLE;
      }
    }
    else
    {
      /* Free the resources allocated so far */
      bhd_app_uninit ();

      LOG_POST (BVIEW_LOG_EMERGENCY,
          "%s  %d: Failed to allocate memory for bhd application\r\n", __FILE__, __LINE__);
      return BVIEW_STATUS_RESOURCE_NOT_AVAILABLE;
    }
  }

  LOG_POST (BVIEW_LOG_INFO,
      "bhd application: bhd memory allocated successfully\r\n");

  if (BVIEW_STATUS_SUCCESS != bhd_app_config_init (num_units))
  {
    return BVIEW_STATUS_INIT_FAILED;
  }


  /* Check if message queue is already exitsing */
  if (0 > (recvMsgQid = msgget(bhd_info.key1, IPC_EXCL | IPC_CREAT | 0777)))
  {
    if (EEXIST == errno)
    {
      /* get the already existing  message queue id for BHD */
      if ((recvMsgQid = msgget (bhd_info.key1, IPC_CREAT | 0777)) < 0)
      {

        LOG_POST (BVIEW_LOG_EMERGENCY,
            "Failed to create  msgQ for bhd application. err = %d\n", errno);
        return  BVIEW_STATUS_FAILURE;
      }

      /* message queue exits..
         Delete the message queue */
      if  (msgctl(recvMsgQid, IPC_RMID, (struct msqid_ds *)NULL) <0) 
      {
        LOG_POST (BVIEW_LOG_EMERGENCY,
            "Failed to destroy BHD msgQ id, err = %d\n", errno);
      }


      /* create the message queue for bhd */
      if ((recvMsgQid = msgget (bhd_info.key1, IPC_CREAT | 0777)) < 0)
      {
        /* Free the resources allocated so far */
        bhd_app_uninit ();

        LOG_POST (BVIEW_LOG_EMERGENCY,
            "Failed to create  msgQ for bhd application. err = %d\n", errno);
        return  BVIEW_STATUS_FAILURE;
      }

      memset(&ds, 0, sizeof(struct msqid_ds));
      if (msgctl(recvMsgQid, IPC_STAT, &ds))
      {
        LOG_POST (BVIEW_LOG_EMERGENCY, "msgctl(recvMsgQid=%d, IPC_STAT failed: "
            "%s (errno=%d)\n", recvMsgQid, strerror(errno),errno);

        return BVIEW_STATUS_FAILURE;
      }

      ds.msg_qbytes = BVIEW_BHD_MAX_QUEUE_SIZE;

      if (msgctl(recvMsgQid, IPC_SET,&ds))
      {
        LOG_POST (BVIEW_LOG_EMERGENCY,
            "msgctl(recvMsgQid=%d, IPC_SET, ...) failed "
            "(msg_perm.uid=%u," "msg_perm.cuid=%u): " "%s (errno=%d)\n",
            recvMsgQid, ds.msg_perm.uid, ds.msg_perm.cuid,  strerror(errno), errno);
      }
    }
  }
  bhd_info.recvMsgQid = recvMsgQid;

  /* create pthread for bhd application */
  if (0 != pthread_create (&bhd_info.bhd_thread, NULL, (void *) &bhd_app_main, NULL))
  {
    LOG_POST (BVIEW_LOG_EMERGENCY, "bhd  thread creation failed %d\r\n", errno);
    return  BVIEW_STATUS_FAILURE;
  }
  LOG_POST (BVIEW_LOG_INFO,
      "bhd application: bhd pthread created\r\n");

  return rv;
}



