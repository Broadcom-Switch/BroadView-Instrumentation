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
#include <mqueue.h>
#include <errno.h>
#include "bst_json_memory.h"
#include "clear_bst_statistics.h"
#include "clear_bst_thresholds.h"
#include "configure_bst_thresholds.h"
#include "configure_bst_feature.h"
#include "configure_bst_tracking.h"
#include "get_bst_tracking.h"
#include "get_bst_feature.h"
#include "get_bst_thresholds.h"
#include "get_bst_cgsn_drop_counters.h"
#include "get_bst_report.h"
#include "bst_json_encoder.h"
#include "bst.h"
#include "broadview.h"
#include "bst_app.h"
#include "system.h"
#include "rest_api.h"
#include "openapps_log_api.h"
#include "sbplugin_redirect_bst.h"
#include "sbplugin_redirect_system.h"
#include "system_utils_api.h"

/* BST Context Info*/
extern BVIEW_BST_CXT_t bst_info;
/* BST Mutex*/
pthread_mutex_t *bst_mutex;

/*********************************************************************
 * @brief : function to return the api handler for the bst command type 
 *
 * @param[in] int : request type 
 *
 * @retval    : function pointer to the corresponding api 
 *
 * @note : 
 *
 *********************************************************************/
BVIEW_STATUS bst_type_api_get (int type, BVIEW_BST_API_HANDLER_t *handler)
{
  unsigned int i = 0;

  static const BVIEW_BST_API_t bst_api_list[] = {
    {BVIEW_BST_CMD_API_GET_FEATURE, bst_config_feature_get},
    {BVIEW_BST_CMD_API_GET_TRACK, bst_config_track_get},
    {BVIEW_BST_CMD_API_GET_REPORT, bst_get_report},
    {BVIEW_BST_CMD_API_GET_THRESHOLD, bst_get_report},
    {BVIEW_BST_CMD_API_TRIGGER_REPORT, bst_get_report},
    {BVIEW_BST_CMD_API_TRIGGER_COLLECT, bst_process_trigger},
    {BVIEW_BST_CMD_API_SET_FEATURE, bst_config_feature_set},
    {BVIEW_BST_CMD_API_SET_TRACK, bst_config_track_set},
    {BVIEW_BST_CMD_API_SET_THRESHOLD, bst_config_threshold_set},
    {BVIEW_BST_CMD_API_CLEAR_THRESHOLD, bst_clear_threshold_set},
    {BVIEW_BST_CMD_API_CLEAR_STATS, bst_clear_stats_set},
    {BVIEW_BST_CMD_API_CLEAR_TRIGGER_COUNT, bst_clear_trigger_count},
    {BVIEW_BST_CMD_API_ENABLE_BST_ON_TRIGGER, bst_enable_on_trigger_timer_expiry},
    {BVIEW_BST_CMD_API_GET_CGSN_DRP_CTRS, bst_get_cgsn_drp_ctrs}
  };

  for (i = 0; i < BVIEW_BST_CMD_API_MAX-1; i++)
  {
    if (type == bst_api_list[i].bst_command)
    {
      *handler = bst_api_list[i].handler;
      return BVIEW_STATUS_SUCCESS;
    }
  }
  return BVIEW_STATUS_INVALID_PARAMETER;
}


/*********************************************************************
* @brief : clear the trigger counters 
*
* @param[in] msg_data : pointer to the bst message request.
*
* @retval  : BVIEW_STATUS_SUCCESS : successfully cleared the trigger report count 
* @retval  : BVIEW_STATUS_FAILURE : The clearing of count has failed.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note
*
*********************************************************************/
BVIEW_STATUS bst_clear_trigger_count (BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BST_UNIT_CXT_t *ptr;
  bool clearTimer = true;
  unsigned int i;
  BVIEW_BST_REQUEST_MSG_t bst_msg = {0};

  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  ptr = BST_UNIT_PTR_GET (msg_data->unit);

  if (NULL == ptr)
    return BVIEW_STATUS_INVALID_PARAMETER;

  /* before clearing, check if there is still a need to 
     run this timer . If no triggers are there, 
     the timer can be cleared and restarted as and when 
     required */

  BST_RWLOCK_WR_LOCK (msg_data->unit);
  for (i = 0; i < BST_ID_MAX; i++)
  {
    if (0 != ptr->bst_trigger_count[i])
    {
      clearTimer = false;
      break;
    }
  }

  memset (ptr->bst_trigger_count, 0, sizeof(ptr->bst_trigger_count)); 
  BST_RWLOCK_UNLOCK (msg_data->unit);

  if (true == clearTimer)
  {
    rv = bst_trigger_timer_delete (msg_data->unit);
  }

  /* post message to bst queue */
  bst_msg.unit = msg_data->unit;
  bst_msg.msg_type = BVIEW_BST_CMD_API_ENABLE_BST_ON_TRIGGER;

  /* Send the message to the bst application */
  rv = bst_send_request (&bst_msg);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "Failed to send trigger collection message to bst application. err = %d\r\n", rv);
    return BVIEW_STATUS_FAILURE;
  }

  return rv;
}

/*********************************************************************
* @brief :  re-enable bst on trigger timer expiry 
*
* @param[in] msg_data : pointer to the bst message request.
*
* @retval  : BVIEW_STATUS_SUCCESS : successfully cleared the trigger report count 
* @retval  : BVIEW_STATUS_FAILURE : The re-enable has failed.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note
*
*********************************************************************/
BVIEW_STATUS bst_enable_on_trigger_timer_expiry (BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BST_CONFIG_PARAMS_t *ptr;

  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  ptr = BST_CONFIG_FEATURE_PTR_GET (msg_data->unit);

  if (NULL == ptr)
    return BVIEW_STATUS_INVALID_PARAMETER;

  /* Take lock */
  BST_RWLOCK_RD_LOCK (msg_data->unit);

  if (true == ptr->bstEnable)
  {
    rv = bst_enable_on_trigger(msg_data, true);
  }

  /* release lock */
  BST_RWLOCK_UNLOCK (msg_data->unit);
  return rv;
}

/*********************************************************************
* @brief : bst main application function which does processing of messages
*
* @param[in] : none
*
* @retval  : BVIEW_STATUS_SUCCESS: Fails to register with module manager 
*             or some error happened due to which the bst application is
*             unable to process incoming messages.
* @retval  : BVIEW_STATUS_FAILURE: Fails to register with module manager 
*
* @note  : This api is the processing thread of the bst application. 
*          All the incoming requests are processed and the responses 
*          are sent in the bst context. Currently the assumption
*          is made that if the thread fails to read continously 10 or 
*          more messages, then there is some error and the thread exits.
*
*********************************************************************/
BVIEW_STATUS bst_app_main (void)
{
  BVIEW_BST_REQUEST_MSG_t msg_data;
  BVIEW_BST_RESPONSE_MSG_t reply_data;
  BVIEW_STATUS rv = BVIEW_STATUS_FAILURE;
  unsigned int rcvd_err = 0;
  unsigned int id = 0, num_units = 0;
  BVIEW_BST_API_HANDLER_t handler;

  if (BVIEW_STATUS_SUCCESS != bst_module_register ())
  {
    /* registration with module mgr has failed.
       return failure. so that the caller can clean the resources */
    LOG_POST (BVIEW_LOG_EMERGENCY,
              "Registration with module mgr failed \r\n");

    return BVIEW_STATUS_FAILURE;
  }

  if (BVIEW_STATUS_SUCCESS != 
	    system_utils_cancel_api_request_register(BVIEW_FEATURE_BST, bst_cancel_request))
  {
    /* registration with system utils for cancel request has failed.
       return failure. so that the caller can clean the resources */
    LOG_POST (BVIEW_LOG_ERROR,
              "Registration with system utils for cancel request failed \r\n");

    return BVIEW_STATUS_FAILURE;
  }

  if (BVIEW_STATUS_SUCCESS != sbapi_system_num_units_get ((int *) &num_units))
  {
    LOG_POST (BVIEW_LOG_EMERGENCY, "Failed to get num of units\r\n");
    return BVIEW_STATUS_FAILURE;
  }

  for (id = 0; id < num_units; id++)
  {
    /* register the trigger callback for every unit */
    /* register for triggers from asic */
    rv = sbapi_bst_register_trigger (id, (bst_trigger_cb), &bst_info.unit[id].cb_cookie);
    if (BVIEW_STATUS_SUCCESS != rv)
    {
      LOG_POST (BVIEW_LOG_ERROR,
          "Failed to register with asic for callbacks for  unit %d\r\n", id);
      return BVIEW_STATUS_FAILURE;
    }

      /* get the asic capabilities of the system 
      * save the same so that the same can be reused 
      */
    if (BVIEW_STATUS_SUCCESS != sbapi_system_asic_capabilities_get(id, 
                                          &bst_info.unit[id].asic_capabilities))
    {
        /* unable to get the asic capabilities
           log error and return */
       LOG_POST (BVIEW_LOG_ERROR, 
               "Failed to Get Asic capabilities for unit %d. \r\n", id);
       return BVIEW_STATUS_FAILURE;
    }
    else
    {
             
      _BST_LOG(_BST_DEBUG_INFO, "supported capabilities are\n"
          "num ports = %d, numUnicastQueues = %d\n" 
          "numUnicastQueueGroups = %d, numMulticastQueues = %d\n" 
          "numServicePools = %d, numCommonPools = %d\n" 
          "numCpuQueues = %d, numRqeQueues = %d\n" 
          "numRqeQueuePools = %d, numPriorityGroups = %d\n",
           bst_info.unit[id].asic_capabilities.numPorts, 
           bst_info.unit[id].asic_capabilities.numUnicastQueues, 
           bst_info.unit[id].asic_capabilities.numUnicastQueueGroups, 
           bst_info.unit[id].asic_capabilities.numMulticastQueues, 
           bst_info.unit[id].asic_capabilities.numServicePools, 
           bst_info.unit[id].asic_capabilities.numCommonPools, 
           bst_info.unit[id].asic_capabilities.numCpuQueues, 
           bst_info.unit[id].asic_capabilities.numRqeQueues, 
           bst_info.unit[id].asic_capabilities.numRqeQueuePools, 
           bst_info.unit[id].asic_capabilities.numPriorityGroups 
          ); 
    }
  }


  while (1)
  {
    if (-1 != (msgrcv (bst_info.recvMsgQid, &msg_data, sizeof (msg_data), 0, 0))) 
    {
      _BST_LOG(_BST_DEBUG_INFO, "msg_data info\n"
          "msg_data.msg_type = %ld\n"
          "msg_data.unit = %d\n"
          "msg_data.cookie = %d\n",
          msg_data.msg_type, msg_data.unit, (NULL == msg_data.cookie)? true: false); 

      if (msg_data.unit >= num_units)
      {
        /* requested id is greater than the number of units.
           send failure message and continue */
        memset (&reply_data, 0, sizeof (BVIEW_BST_RESPONSE_MSG_t));
        reply_data.rv = BVIEW_STATUS_INVALID_PARAMETER;

        /* copy the request type */
        reply_data.msg_type = msg_data.msg_type;
        /* copy the unit */
        reply_data.unit = msg_data.unit;
        /* copy the asic type */
        reply_data.id = msg_data.id;
        /* copy the cookie ..  */
        reply_data.cookie = msg_data.cookie;

        rv = bst_send_response(&reply_data);
        if (BVIEW_STATUS_SUCCESS != rv)
        {
          LOG_POST (BVIEW_LOG_ERROR,
              "failed to send response for command %ld , err = %d. \r\n", msg_data.msg_type, rv);
        }
        continue;
      }
      /* Counter to check for read errors.. 
         successfully read the message. clear this to 0*/
      rcvd_err = 0;
      /* Memset the response message */
      memset (&reply_data, 0, sizeof (BVIEW_BST_RESPONSE_MSG_t));

      /* get the api function for the method type */
      if (BVIEW_STATUS_SUCCESS != bst_type_api_get (msg_data.msg_type, &handler))
      {
        continue;
      }

      rv = handler(&msg_data);

      reply_data.rv = rv;

      rv = bst_copy_reply_params (&msg_data, &reply_data);
      if (BVIEW_STATUS_SUCCESS != rv)
      {
        _BST_LOG(_BST_DEBUG_ERROR, 
                "bst_main.c failed to send response for command %ld , err = %d. \r\n", msg_data.msg_type, rv);
        LOG_POST (BVIEW_LOG_ERROR,
            "failed to send response for command %ld , err = %d. \r\n", msg_data.msg_type, rv);
      }
    }
    else
    {
      LOG_POST (BVIEW_LOG_ERROR,
          "Failed to read message from the bst application queue. err = %d\r\n", errno);
      /* increment the error counter. 
         If this counter increments continously exit and return failure
         so that corrective action can be taken */
       rcvd_err++;

        /* wait till 10 consective messages */
       if(BVIEW_BST_MAX_QUEUE_SEND_FAILS < rcvd_err)
       {
         break;
       }
       continue;
    }
  }                             /* while (1) */
  LOG_POST (BVIEW_LOG_EMERGENCY,
            "exiting from bst pthread ... \r\n");
  pthread_exit(&bst_info.bst_thread);
  return BVIEW_STATUS_FAILURE;
}


/*********************************************************************
* @brief : function to initializes bst structures to default values 
*
* @param[in] : number of units on the agent
*
* @retval  : BVIEW_STATUS_SUCCESS : configuration is successfully initialized.
* @retval  : BVIEW_STATUS_FAILURE : Fail to initialize the BST appliation to default.
*
* @note  : Initialization functon to make sure both software and asic 
*          are configured with same default values. We are not setting 
*          the default values for threshold, but we are using 
*          the same as that of asic.
*
*********************************************************************/
BVIEW_STATUS bst_app_config_init (unsigned int num_units)
{
  BVIEW_BST_CFG_PARAMS_t *ptr;
  BVIEW_BST_DATA_t *bst_data_ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_TIME_t  tv;   

  BVIEW_BST_CONFIG_t bstMode;
  int unit_id = 0;

  if (0 == num_units)
  {
    /* get the number of units on this device */
    LOG_POST (BVIEW_LOG_ERROR, "Failed to Init BST:Number of units are zero\r\n");
    return BVIEW_STATUS_FAILURE;
  }

  for (unit_id = 0; unit_id < num_units; unit_id++)
  {
    /* all the bst info we maintain is per unit. so get each unit 
       and configure the same.. */
    ptr = BST_CONFIG_PTR_GET (unit_id);

    if (NULL == ptr)
    {
      /* fatal error. cannot access the bst info.. */
      LOG_POST (BVIEW_LOG_ERROR, "Error!! bst info is not accessible !!\r\n");
      return BVIEW_STATUS_FAILURE;
    }

    bst_data_ptr = BST_UNIT_DATA_PTR_GET (unit_id);
    if (NULL == ptr)
    {
      LOG_POST (BVIEW_LOG_ERROR, "Error!! bst memory is not accessible !!\r\n");
      return BVIEW_STATUS_FAILURE;
    }
    bstjson_memory_init();

    /* bst enable */
    ptr->config.bstEnable = BVIEW_BST_DEFAULT_ENABLE;
    /* send async reports  */
    ptr->config.sendAsyncReports = BVIEW_BST_PERIODIC_REPORT_DEFAULT;
    /* collection interval  */
    ptr->config.collectionInterval = BVIEW_BST_DEFAULT_INTERVAL;
    /* stats in cells or bytes.  */
    ptr->config.statUnitsInCells = BVIEW_BST_DEFAULT_STATS_UNITS;
    ptr->config.statsInPercentage = BVIEW_BST_DEFAULT_STATS_PERCENTAGE;
    ptr->config.bstMaxTriggers = BVIEW_BST_DEFAULT_MAX_TRIGGERS;
    ptr->config.sendSnapshotOnTrigger = BVIEW_BST_DEFAULT_SNAPSHOT_TRIGGER;
    ptr->config.triggerTransmitInterval = BVIEW_BST_DEFAULT_TRIGGER_INTERVAL;
    ptr->config.sendIncrementalReport = BVIEW_BST_DEFAULT_SEND_INCR_REPORT;


    /* enable device tracking   */
    ptr->track.trackDevice = BVIEW_BST_DEFAULT_TRACK_DEVICE;
    /* enable ingress tracking p + pg   */
    ptr->track.trackIngressPortPriorityGroup = BVIEW_BST_DEFAULT_TRACK_IN_P_PG;
    /* enable ingress tracking p + sp  */
    ptr->track.trackIngressPortServicePool = BVIEW_BST_DEFAULT_TRACK_IN_P_SP;
    /* enable ingress tracking  sp  */
    ptr->track.trackIngressServicePool = BVIEW_BST_DEFAULT_TRACK_IN_SP;
    /* enable egress tracking P+ sp  */
    ptr->track.trackEgressPortServicePool = BVIEW_BST_DEFAULT_TRACK_E_P_SP;
    /* enable egress tracking sp  */
    ptr->track.trackEgressServicePool = BVIEW_BST_DEFAULT_TRACK_E_SP;
    /* enable egress tracking uc queues  */
    ptr->track.trackEgressUcQueue = BVIEW_BST_DEFAULT_TRACK_E_UC_Q;
    /* enable egress tracking uc queue grp */
    ptr->track.trackEgressUcQueueGroup = BVIEW_BST_DEFAULT_TRACK_E_UC_QG;
    /* enable egress tracking mc queue grp */
    ptr->track.trackEgressMcQueue = BVIEW_BST_DEFAULT_TRACK_E_MC_Q;
    /* enable egress tracking cpu queue grp */
    ptr->track.trackEgressCpuQueue = BVIEW_BST_DEFAULT_TRACK_E_CPU_Q;
    /* enable egress tracking rqe queue grp */
    ptr->track.trackEgressRqeQueue = BVIEW_BST_DEFAULT_TRACK_MODE;
    /* enable  tracking mode to current */

    ptr->track.trackPeakStats = false;

    /* Initialize the bst timer array */
    bst_data_ptr->bst_collection_timer.context.unit = unit_id;
    bst_data_ptr->bst_collection_timer.in_use = false;
    bst_data_ptr->bst_trigger_timer.in_use = false;
    bst_data_ptr->bst_trigger_timer.context.unit = unit_id;

    /* push default values to asic */

    rv =  sbapi_bst_clear_thresholds(unit_id);
    if (BVIEW_STATUS_SUCCESS != rv)
    {
      LOG_POST (BVIEW_LOG_ERROR, 
          "threshold clear not successful for the unit. %d \r\n", 
          unit_id);
    }

    /* populate the default thereshold cache */

    memset(&bst_info.unit[unit_id].bst_thresholds_cache, 0, sizeof(BVIEW_BST_ASIC_SNAPSHOT_DATA_t));
    rv = sbapi_bst_threshold_get (unit_id,
               &bst_info.unit[unit_id].bst_thresholds_cache,
               &tv);

    memset (&bstMode, 0, sizeof (BVIEW_BST_CONFIG_t));
    bstMode.enableStatsMonitoring = BVIEW_BST_DEFAULT_ENABLE;
    bstMode.enableDeviceStatsMonitoring = BVIEW_BST_DEFAULT_TRACK_DEVICE;
    bstMode.enableIngressStatsMonitoring = BVIEW_BST_DEFAULT_TRACK_INGRESS;
    bstMode.enableEgressStatsMonitoring = BVIEW_BST_DEFAULT_TRACK_EGRESS;
    bstMode.mode = BVIEW_BST_MODE_CURRENT;
  
    if (BVIEW_STATUS_SUCCESS != sbapi_bst_config_set (unit_id, &bstMode))
    {
      LOG_POST (BVIEW_LOG_ERROR,
                "Failed to set bst config params for unit %d\r\n", unit_id);
      return BVIEW_STATUS_FAILURE;
    }

    if ((false != ptr->config.sendAsyncReports) && (ptr->config.bstEnable != false))
    {
      /* register for timer callback only if reports need
         to be sent asyncronously */
      rv = bst_periodic_collection_timer_add (unit_id,
                                              bst_periodic_collection_cb,
                                              BVIEW_BST_CMD_API_GET_REPORT, 0);
      if (BVIEW_STATUS_SUCCESS != rv)
      {
        LOG_POST (BVIEW_LOG_ERROR,
            "Failed to register with timer  for callbacks for  unit %d\r\n", unit_id);
        return BVIEW_STATUS_FAILURE;
      }
    }
  }

  LOG_POST (BVIEW_LOG_INFO, 
      "bst default initialization successful\r\n"); 
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
* @note   : This function is invoked by the bst to send the report and 
*           requested get configure params. This function internally
*           calls the encoding api to encode the data, and the memory
*           for the data is allocated. In case of both successful and 
*           unsuccessful send of the data, the memory must be freed.
*           
*********************************************************************/
BVIEW_STATUS bst_send_response (BVIEW_BST_RESPONSE_MSG_t * reply_data)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  uint8_t *pJsonBuffer = NULL;

  if (NULL == reply_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

        _BST_LOG(_BST_DEBUG_INFO, "Reply_data\n"
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
    if ((BVIEW_BST_CMD_API_SET_TRACK == reply_data->msg_type) ||
        (BVIEW_BST_CMD_API_SET_FEATURE == reply_data->msg_type) ||
        (BVIEW_BST_CMD_API_SET_THRESHOLD == reply_data->msg_type) ||
        (BVIEW_BST_CMD_API_CLEAR_STATS == reply_data->msg_type) ||
        (BVIEW_BST_CMD_API_CLEAR_THRESHOLD == reply_data->msg_type))
    {
      rest_response_send_ok (reply_data->cookie);
      return BVIEW_STATUS_SUCCESS;
    }

  } 

  /* Take lock*/
  BST_LOCK_TAKE (reply_data->unit);
  switch (reply_data->msg_type)
  {
    case BVIEW_BST_CMD_API_GET_TRACK:

      /* call json encoder api for tracking  */
      rv = bstjson_encode_get_bst_tracking (reply_data->unit, reply_data->id,
          reply_data->response.track,
          &pJsonBuffer);
      break;

    case BVIEW_BST_CMD_API_GET_FEATURE:
      /* call json encoder api for feature  */

      rv = bstjson_encode_get_bst_feature (reply_data->unit, reply_data->id,
          reply_data->response.config,
          &pJsonBuffer);
      break;

   
     case BVIEW_BST_CMD_API_GET_REPORT:
     case BVIEW_BST_CMD_API_TRIGGER_REPORT:
     case BVIEW_BST_CMD_API_GET_THRESHOLD:
      /*  call json encoder api for report  */

      /* if this is a periodic report, the back up pointer is
         a non null poiner, other wise , the backup 
         pointer would be NULL pointer. so call 
         the encoder function accordingly */

      if (NULL == reply_data->response.report.backup)
      {
      rv = bstjson_encode_get_bst_report (reply_data->unit, reply_data->msg_type,
                                          NULL, 
                                          &reply_data->response.report.active->snapshot_data,
                                          &reply_data->options,
                                          reply_data->asic_capabilities,
                                          &reply_data->response.report.active->tv,
                                          &pJsonBuffer); 
      }
      else
      {
      rv = bstjson_encode_get_bst_report (reply_data->unit, reply_data->msg_type,
                                          &reply_data->response.report.backup->snapshot_data, 
                                          &reply_data->response.report.active->snapshot_data,
                                          &reply_data->options,
                                          reply_data->asic_capabilities,
                                          &reply_data->response.report.active->tv,
                                          &pJsonBuffer); 
      }

        break;


    case BVIEW_BST_CMD_API_GET_CGSN_DRP_CTRS:
      /* call json encoder api for feature  */

      rv = bstjson_encode_get_cgsn_drop_ctrs (reply_data->unit, 
                                              (void *)reply_data->response.drp_ctr_report, 
                                              reply_data->asic_capabilities, 
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
      bstjson_memory_free(pJsonBuffer);
    }
    BST_LOCK_GIVE(reply_data->unit);
    return rv;
  }
  if (NULL != pJsonBuffer)
  {
    #ifdef BST_DEBUG_METRICS
    if (reply_data->msg_type == BVIEW_BST_CMD_API_TRIGGER_REPORT)
    {
      char buf[BVIEW_TIME_BUFFER_SIZE];
      system_dispaly_local_time_get (buf);
      printf ("\r\n%s: Sending trigger report for realm (%s) counter (%s) index1 (%d) index2 (%d)\r\n",
             buf,reply_data->options.triggerInfo.realm,
             reply_data->options.triggerInfo.counter,
             reply_data->options.triggerInfo.port,
             reply_data->options.triggerInfo.queue);
    }
    #endif
    rv = rest_response_send(reply_data->cookie, (char *)pJsonBuffer, strlen((char *)pJsonBuffer));

    if (BVIEW_STATUS_SUCCESS != rv)
    {
      _BST_LOG(_BST_DEBUG_ERROR, "sending response failed due to error = %d\r\n",rv);
      LOG_POST (BVIEW_LOG_ERROR,
          " sending response failed due to error = %d\r\n",rv);
    }
    /* free the json buffer */
    bstjson_memory_free(pJsonBuffer);
  }
  else
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "encoding of bst response failed due failure of memory allocation\r\n");
  }
  /* release the lock for success and failed cases */
  BST_LOCK_GIVE(reply_data->unit);
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
*          message to response message. In case of report, it copies the 
*          pointers into the structure The data need to be protected till 
*          the memory is allocated for the report to be copied.
*          If the report is periodic, then we need the references of both
*          active and back up records. If the report is non periodic, i.e 
*          trigger or get report, then only the active record is required. 
*          incse of periodic report, the report contains 
*          incremental/differential changes.. Wher as for get report 
*          the report is complete. Ensure that the lock is taken at this function
*          is released when the buffer for the data is allocated.
*
*********************************************************************/
BVIEW_STATUS bst_copy_reply_params (BVIEW_BST_REQUEST_MSG_t * msg_data,
                            BVIEW_BST_RESPONSE_MSG_t * reply_data)
{
  BVIEW_BST_UNIT_CXT_t *ptr;
  BVIEW_BST_STAT_COLLECT_CONFIG_t *pCollect = &msg_data->request.collect;
  BVIEW_BST_REPORT_OPTIONS_t  *pResp;
  BVIEW_BST_CGSN_DROPS_t *tmp = NULL;

  if ((NULL == msg_data) || (NULL == reply_data))
    return BVIEW_STATUS_INVALID_PARAMETER;

  ptr = BST_UNIT_PTR_GET (msg_data->unit);

  pCollect = &msg_data->request.collect;
  pResp = &reply_data->options;

  /* copy the request type */
  reply_data->msg_type = msg_data->msg_type;
  /* copy the unit */
  reply_data->unit = msg_data->unit;
  /* copy the asic type */
  reply_data->id = msg_data->id;
  /* copy the cookie ..  */
  reply_data->cookie = msg_data->cookie;
        /* copy the asic capabilities into the response */

  reply_data->asic_capabilities = &ptr->asic_capabilities;

  reply_data->options.statUnitsInCells = 
  ptr->bst_data->bst_config.config.statUnitsInCells;

  reply_data->options.statsInPercentage = false;

  /* copy the address pointer of the default values */
  reply_data->options.bst_max_buffers_ptr = &ptr->bst_max_buffers;
        /* copy the collect params into options fields of the request */
  BST_COPY_COLLECT_TO_RESP (pCollect, pResp);


  switch (msg_data->msg_type)
  {
    case BVIEW_BST_CMD_API_GET_REPORT:
    case BVIEW_BST_CMD_API_TRIGGER_REPORT:
      {
        /* copy stats_units and trigger into  options */
        if (BVIEW_BST_STATS_TRIGGER == msg_data->report_type)
        {
          /* this report is of type trigger */
          reply_data->options.reportTrigger = true;
          reply_data->options.reportThreshold = false;
          reply_data->cookie = NULL;
          reply_data->options.triggerInfo = msg_data->triggerInfo;
          reply_data->options.sendSnapShotOnTrigger = ptr->bst_data->bst_config.config.sendSnapshotOnTrigger;
          if(false == reply_data->options.sendSnapShotOnTrigger)
          {
            BST_COPY_TO_RESP(pResp, false);
            /* Set the only the desired realm to true */
            bst_set_realm_to_collect(msg_data->triggerInfo.realm, pResp);
          }
        }

        /* update the data, i.e make the active record as new backup
           and current record as new active */
        bst_update_data (BVIEW_BST_STATS, msg_data->unit);

        /* assign the active records */
        reply_data->response.report.active = ptr->stats_active_record_ptr;

        /* copy the backup record ptr if and only if the report is periodic */

        if (BVIEW_BST_PERIODIC == msg_data->report_type)
        {
            reply_data->options.sendIncrementalReport = 
                 ptr->bst_data->bst_config.config.sendIncrementalReport;

          /* check if complete report is configured */
          if (false == ptr->bst_data->bst_config.config.sendIncrementalReport)
          {
            reply_data->response.report.backup = NULL;
          }
          else
          {
            reply_data->response.report.backup = ptr->stats_backup_record_ptr;
          }
          reply_data->cookie = NULL;
        }
        else
        {
          /* copy null as the encoder function expects the null for non-periodic cases */
          reply_data->response.report.backup = NULL;

          /* no need to send huge data when get-bst-report is requested.
             Send only the counters which contain non zero values */
            reply_data->options.sendIncrementalReport = 
                 true;
        }


        reply_data->options.statsInPercentage = 
          ptr->bst_data->bst_config.config.statsInPercentage;
      }
      break;

    case BVIEW_BST_CMD_API_GET_THRESHOLD:
      {
        reply_data->options.reportThreshold = true;
        reply_data->response.report.active = ptr->threshold_record_ptr;
        /* copy null as the encoder function expects the null  */
        reply_data->response.report.backup = NULL;
         reply_data->options.sendIncrementalReport = 
                 false;
        reply_data->options.statsInPercentage = 
          ptr->bst_data->bst_config.config.statsInPercentage;
      }
      break;

    case BVIEW_BST_CMD_API_GET_FEATURE:
      reply_data->response.config = &ptr->bst_data->bst_config.config;
      break;

    case BVIEW_BST_CMD_API_GET_TRACK:
      reply_data->response.track = &ptr->bst_data->bst_config.track;
      break;

    case BVIEW_BST_CMD_API_GET_CGSN_DRP_CTRS:
    BST_LOCK_TAKE (msg_data->unit);
    tmp = ptr->cgsn_drp_curr;
    ptr->cgsn_drp_curr = ptr->cgsn_drp_active;
    ptr->cgsn_drp_active = tmp;
    reply_data->response.drp_ctr_report = ptr->cgsn_drp_active;
    if (BVIEW_BST_PERIODIC == msg_data->report_type)
    {
      reply_data->cookie = NULL;
    }
    BST_LOCK_GIVE (msg_data->unit);
      break;

    default:
      break;
  }
  /* release the lock for success and failed cases */

  return bst_send_response(reply_data);
}

/*********************************************************************
* @brief   :  function to post message to the bst application  
*
* @param[in]  msg_data : pointer to the message request
*
* @retval  : BVIEW_STATUS_SUCCESS : if the message is successfully posted to BST queue.
* @retval  : BVIEW_STATUS_FAILURE : if the message is failed to send to bst
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameters to function.
*            
* @note  : all the apis and call back functions should use this api
*          to post the message to bst application.
*
*********************************************************************/
BVIEW_STATUS bst_send_request (BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  int rv = BVIEW_STATUS_SUCCESS;
  struct mq_attr obuf; /* output attr struct for getattr */

  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  if (-1 == msgsnd (bst_info.recvMsgQid, msg_data, sizeof (BVIEW_BST_REQUEST_MSG_t), IPC_NOWAIT))
  {
    if ( ! mq_getattr(bst_info.recvMsgQid,&obuf) )
    {
      _BST_LOG(_BST_DEBUG_TRACE, "flags: %ld  maxmsg: %ld  msgsize: %ld  curmsgs: %ld\n",
          obuf.mq_flags, obuf.mq_maxmsg, obuf.mq_msgsize, obuf.mq_curmsgs);
    }

    LOG_POST (BVIEW_LOG_ERROR,
              "Failed to send message to bst application,  msg_type  %ld, err = %d\r\n",
              msg_data->msg_type, errno);
    rv = BVIEW_STATUS_FAILURE;
  }

  return rv;
}

/*********************************************************************
*  @brief:  callback function to send periodic reports  
*
* @param[in]   sigval : Data passed with notification after timer expires
*
* @retval  : BVIEW_STATUS_SUCCESS : message is successfully posted to bst.
* @retval  : BVIEW_STATUS_FAILURE : failed to post message to bst.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameters to function.
*
* @note : when the periodic collection timer expires, this api is invoked in
*         the timer context. We just post the request for the report, while 
*        posting we mark the report type as periodic. The parameter returned 
*       from the callback is the reference to unit id. 
*
*********************************************************************/
BVIEW_STATUS bst_periodic_collection_cb (union sigval sigval)
{
  BVIEW_BST_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;
  BVIEW_BST_TIMER_CONTEXT_t context;

  memset(&context, 0, sizeof(BVIEW_BST_TIMER_CONTEXT_t));

  msg_data.report_type = BVIEW_BST_PERIODIC;
  context = (*(BVIEW_BST_TIMER_CONTEXT_t *)sigval.sival_ptr);
  msg_data.msg_type = context.cmd;
  msg_data.unit = context.unit;
  msg_data.id = context.index;
  /* Send the message to the bst application */
  rv = bst_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "Failed to send periodic collection message to bst application. err = %d\r\n", rv);
       return BVIEW_STATUS_FAILURE;
  }  
    LOG_POST (BVIEW_LOG_INFO,
        "timer call back received for unit = %d\r\n", *(int *)sigval.sival_ptr);
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
*  @brief:  function to clean up  bst application  
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
void bst_app_uninit ()
{
  int id = 0, num_units;
  pthread_mutex_t *bst_mutex;
  pthread_rwlock_t *bst_configRWLock;

  bst_info.key1 = MSG_QUEUE_ID_TO_BST;
  bst_info.trigger_key = MSG_QUEUE_ID_TO_BST_TRIGGER;

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
    bst_periodic_collection_timer_delete (id, BVIEW_BST_CMD_API_GET_REPORT, 0);
    /* Destroy mutex */
    bst_mutex = &bst_info.unit[id].bst_mutex;
    pthread_mutex_destroy (bst_mutex);

    bst_configRWLock = &bst_info.unit[id].bst_configRWLock;
    pthread_rwlock_destroy (bst_configRWLock);

    /* free all memeory */
    if (NULL != bst_info.unit[id].bst_data)
    {
      free (bst_info.unit[id].bst_data);
    }

    if (NULL != bst_info.unit[id].stats_active_record_ptr)
    {
      free (bst_info.unit[id].stats_active_record_ptr);
    }

    if (NULL != bst_info.unit[id].stats_backup_record_ptr)
    {
      free (bst_info.unit[id].stats_backup_record_ptr);
    }

    if (NULL != bst_info.unit[id].stats_current_record_ptr)
    {
      free (bst_info.unit[id].stats_current_record_ptr);
    }

    if (NULL != bst_info.unit[id].threshold_record_ptr)
    {
      free (bst_info.unit[id].threshold_record_ptr);
    }

    if (NULL != bst_info.unit[id].cgsn_drp_curr)
    {
      free (bst_info.unit[id].cgsn_drp_curr);
    }

    if (NULL != bst_info.unit[id].cgsn_drp_active)
    {
      free (bst_info.unit[id].cgsn_drp_active);
    }
  }
  
  /* check if the message queue already exists.
     If yes, we should delete the same */
  if (-1 == (bst_info.recvMsgQid = msgget(bst_info.key1, IPC_EXCL | IPC_CREAT | 0777)))
  {
    if (EEXIST == errno)
    {
      /* delete the message queue  */
      if (msgctl(bst_info.recvMsgQid, IPC_RMID, NULL) == -1) 
      {
        LOG_POST (BVIEW_LOG_ERROR,
            "Failed to destroy bst msgQ id, err = %d\n", errno);
      }
    }
  }
}

/*********************************************************************
* @brief : function to create the bst application thread.
*
* @param[in]  none 
*
* @retval  : BVIEW_STATUS_RESOURCE_NOT_AVAILABLE : Returns when memory is not available
* @retval  : BVIEW_STATUS_FAILURE : failed to create the message queue or thread. 
* @retval  : BVIEW_STATUS_SUCCESS : successful completion of task creation. 
* @retval  : BVIEW_STATUS_INIT_FAILED : BST initialization failed. 
*
* @note : Main function to invoke the bst thread. This function allocates 
*         the required memory for bst application. 
*         Creates the mutexes for the bst data. Creates the message queue
*         It assumes that the system is initialized before the bst 
* @retval  : none
*         thread is invoked. This api depends on the system variables 
*         such as number of units per device. Incase of no such 
*         need, the number of units can be #defined.
*
*********************************************************************/
BVIEW_STATUS bst_main ()
{
  unsigned int id = 0, num_units = 0;
  int rv = BVIEW_STATUS_SUCCESS;
  int recvMsgQid;
  pthread_rwlock_t *bst_configRWLock;


  bst_info.key1 = MSG_QUEUE_ID_TO_BST;
  bst_info.trigger_key = MSG_QUEUE_ID_TO_BST_TRIGGER;


  /* create mutex for all units */
  for (id = 0; id < BVIEW_BST_MAX_UNITS; id++)
  {
    bst_mutex = &bst_info.unit[id].bst_mutex;
    pthread_mutex_init (bst_mutex, NULL);

    bst_configRWLock = &bst_info.unit[id].bst_configRWLock;
    /* Initialize Read Write lock with default attributes */
    pthread_rwlock_init (bst_configRWLock, NULL);
  }

  /* get the number of units */
  if (BVIEW_STATUS_SUCCESS != sbapi_system_num_units_get ((int *) &num_units))
  {
    /* Free the resources allocated so far */
    bst_app_uninit ();

    LOG_POST (BVIEW_LOG_EMERGENCY,
              "Failed to number of units, Unable to start bst application\r\n");
    return BVIEW_STATUS_RESOURCE_NOT_AVAILABLE;
  }

  /* allocate memory for all units */
  for (id = 0; id < num_units; id++)
  {
    /* config variables */
    bst_info.unit[id].bst_data =
      (BVIEW_BST_DATA_t *) malloc (sizeof (BVIEW_BST_DATA_t));

    /* stats records */
    bst_info.unit[id].stats_active_record_ptr =
      (BVIEW_BST_REPORT_SNAPSHOT_t *)
      malloc (sizeof (BVIEW_BST_REPORT_SNAPSHOT_t));
    bst_info.unit[id].stats_backup_record_ptr =
      (BVIEW_BST_REPORT_SNAPSHOT_t *)
      malloc (sizeof (BVIEW_BST_REPORT_SNAPSHOT_t));
    bst_info.unit[id].stats_current_record_ptr =
      (BVIEW_BST_REPORT_SNAPSHOT_t *)
      malloc (sizeof (BVIEW_BST_REPORT_SNAPSHOT_t));

    /* threshold records */
    bst_info.unit[id].threshold_record_ptr =
      (BVIEW_BST_REPORT_SNAPSHOT_t *)
      malloc (sizeof (BVIEW_BST_REPORT_SNAPSHOT_t));

    /* cgsn records */
    bst_info.unit[id].cgsn_drp_curr =
      (BVIEW_BST_CGSN_DROPS_t *)
      malloc (sizeof (BVIEW_BST_CGSN_DROPS_t));

    /* threshold records */
    bst_info.unit[id].cgsn_drp_active =
      (BVIEW_BST_CGSN_DROPS_t *)
      malloc (sizeof (BVIEW_BST_CGSN_DROPS_t));

    if ((NULL == bst_info.unit[id].bst_data) ||
        (NULL == bst_info.unit[id].stats_active_record_ptr) ||
        (NULL == bst_info.unit[id].stats_backup_record_ptr) ||
        (NULL == bst_info.unit[id].stats_current_record_ptr) ||
        (NULL == bst_info.unit[id].cgsn_drp_active) ||
        (NULL == bst_info.unit[id].cgsn_drp_curr) ||
        (NULL == bst_info.unit[id].threshold_record_ptr))
    {
      /* Free the resources allocated so far */
      bst_app_uninit ();

      LOG_POST (BVIEW_LOG_EMERGENCY,
                "Failed to allocate memory for bst application\r\n");
      return BVIEW_STATUS_RESOURCE_NOT_AVAILABLE;
    }
  }

  for (id = 0; id < num_units; id++)
  {
    memset (bst_info.unit[id].bst_data, 0, sizeof (BVIEW_BST_DATA_t));

    memset (bst_info.unit[id].stats_active_record_ptr, 0,
            sizeof (BVIEW_BST_REPORT_SNAPSHOT_t));
    memset (bst_info.unit[id].stats_backup_record_ptr, 0,
            sizeof (BVIEW_BST_REPORT_SNAPSHOT_t));
    memset (bst_info.unit[id].stats_current_record_ptr, 0,
            sizeof (BVIEW_BST_REPORT_SNAPSHOT_t));


    memset (bst_info.unit[id].threshold_record_ptr, 0,
            sizeof (BVIEW_BST_REPORT_SNAPSHOT_t));

    memset (bst_info.unit[id].cgsn_drp_active, 0,
            sizeof (BVIEW_BST_CGSN_DROPS_t));

    memset (bst_info.unit[id].cgsn_drp_curr, 0,
            sizeof (BVIEW_BST_CGSN_DROPS_t));
  }

  LOG_POST (BVIEW_LOG_INFO,
              "bst application: bst memory allocated successfully\r\n");

  if (BVIEW_STATUS_SUCCESS != bst_app_config_init (num_units))
  {
    /* Free the resources allocated so far */
     /*bst_app_uninit (); */
    return BVIEW_STATUS_INIT_FAILED;
  }




   /* Check if message queue is already exitsing */
  if (0 > (recvMsgQid = msgget(bst_info.key1, IPC_EXCL | IPC_CREAT | 0777)))
  {
    if (EEXIST == errno)
    {

     /* get the already existing  message queue id for bst */
      if ((recvMsgQid = msgget (bst_info.key1, IPC_CREAT | 0777)) < 0)
      {
        /* Free the resources allocated so far */
        //bst_app_uninit ();

        LOG_POST (BVIEW_LOG_EMERGENCY,
            "Failed to create  msgQ for bst application. err = %d\n", errno);
        return  BVIEW_STATUS_FAILURE;
      }

      /* message queue exits.. Delete the message queue */
      if  (msgctl(recvMsgQid, IPC_RMID, (struct msqid_ds *)NULL) <0) 
      {
        LOG_POST (BVIEW_LOG_EMERGENCY,
            "Failed to destroy bst msgQ id, err = %d\n", errno);
      }


      /* create the message queue for bst */
      if ((recvMsgQid = msgget (bst_info.key1, IPC_CREAT | 0777)) < 0)
      {
        /* Free the resources allocated so far */
        bst_app_uninit ();

        LOG_POST (BVIEW_LOG_EMERGENCY,
              "Failed to create  msgQ for bst application. err = %d\n", errno);
        return  BVIEW_STATUS_FAILURE;
      }
    }
  }
  bst_info.recvMsgQid = recvMsgQid;

   /* create pthread for bst application */
  if (0 != pthread_create (&bst_info.bst_thread, NULL, (void *) &bst_app_main, NULL))
  {
    LOG_POST (BVIEW_LOG_EMERGENCY, "BST  thread creation failed %d\r\n", errno);
    return  BVIEW_STATUS_FAILURE;
  }
    LOG_POST (BVIEW_LOG_INFO,
              "bst application: bst pthread created\r\n");


   /* Check if message queue is already exitsing */
  if (0 > (recvMsgQid = msgget(bst_info.trigger_key, IPC_EXCL | IPC_CREAT | 0777)))
  {
    if (EEXIST == errno)
    {

     /* get the already existing  message queue id for bst */
      if ((recvMsgQid = msgget (bst_info.trigger_key, IPC_CREAT | 0777)) < 0)
      {
        /* Free the resources allocated so far */
        //bst_app_uninit ();

        LOG_POST (BVIEW_LOG_EMERGENCY,
            "Failed to create  msgQ for bst trigger. err = %d\n", errno);
        return  BVIEW_STATUS_FAILURE;
      }

      /* message queue exits..
         Delete the message queue */
      if  (msgctl(recvMsgQid, IPC_RMID, (struct msqid_ds *)NULL) <0) 
      {
        LOG_POST (BVIEW_LOG_EMERGENCY,
            "Failed to destroy bst msgQ id, err = %d\n", errno);
      }


  /* create the message queue for bst */
      if ((recvMsgQid = msgget (bst_info.trigger_key, IPC_CREAT | 0777)) < 0)
  {
    /* Free the resources allocated so far */
    bst_app_uninit ();

    LOG_POST (BVIEW_LOG_EMERGENCY,
              "Failed to create  msgQ for bst application. err = %d\n", errno);
    return  BVIEW_STATUS_FAILURE;
  }

    }
  }
  bst_info.recvTriggerMsgQid = recvMsgQid;

   /* create pthread for bst application */
  if (0 != pthread_create (&bst_info.bst_trigger_thread, NULL, (void *) &bst_trigger_main, NULL))
  {
    LOG_POST (BVIEW_LOG_EMERGENCY, "BST  trigger processing thread creation failed %d\r\n", errno);
    return  BVIEW_STATUS_FAILURE;
  }
    LOG_POST (BVIEW_LOG_INFO,
              "bst application: bst trigger process pthread created\r\n");



  return rv;
}


/*********************************************************************
*  @brief:  function to set the given realm in the include trigger report.  
*
* @param[in]   *realm : pointer to realm 
* @param[in]   *realm : pointer to json encode options 
*
* @retval  : none : 
*
* @note :
*
*********************************************************************/

void bst_set_realm_to_collect(char *realm, BVIEW_BST_REPORT_OPTIONS_t *options)
{
  if (0 == strcmp("device", realm))
  {
    options->includeDevice = true;
    return;
  }
  
  if (0 == strcmp("ingress-service-pool", realm))
  {
    options->includeIngressServicePool = true;
    return;
  }

  if (0 == strcmp("ingress-port-service-pool", realm))
  {
    options->includeIngressPortServicePool = true;
    return;
  }
  
  if (0 == strcmp("ingress-port-priority-group", realm))
  {
    options->includeIngressPortPriorityGroup = true;
    return;
  }

  if (0 == strcmp("egress-port-service-pool", realm))
  {
    options->includeEgressPortServicePool = true;
    return;
  }

  if (0 == strcmp("egress-service-pool", realm))
  {
    options->includeEgressServicePool = true;
    return;
  }

  if (0 == strcmp("egress-uc-queue", realm))
  {
    options->includeEgressUcQueue = true;
    return;
  }

  if (0 == strcmp("egress-uc-queue-group", realm))
  {
    options->includeEgressUcQueueGroup = true;
    return;
  }

  if (0 == strcmp("egress-mc-queue", realm))
  {
    options->includeEgressMcQueue = true;
    return;
  }

  if (0 == strcmp("egress-cpu-queue", realm))
  {
    options->includeEgressCpuQueue = true;
    return;
  }

  if (0 == strcmp("egress-rqe-queue", realm))
  {
    options->includeEgressRqeQueue = true;
    return;
  }

  return;
}

