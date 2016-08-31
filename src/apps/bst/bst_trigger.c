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
#include "openapps_log_api.h"
#include "sbplugin_redirect_bst.h"
#include "sbplugin_redirect_system.h"


/* BST Context Info*/
extern BVIEW_BST_CXT_t bst_info;
/* BST rwlock for config data*/

bool bst_trigger_index_get(char *realm, char *counter, unsigned int *val)
{
  static BST_REALM_COUNTER_INDEX_t bst_realm_index_map [] = {
    {BST_ID_DEVICE, "device" ,"data"},
    {BST_ID_ING_POOL, "ingress-service-pool", "um-share-buffer-count"},
    {BST_ID_PORT_POOL, "ingress-port-service-pool", "um-share-buffer-count" },
    {BST_ID_PRI_GROUP_SHARED, "ingress-port-priority-group", "um-share-buffer-count"},
    {BST_ID_PRI_GROUP_HEADROOM, "ingress-port-priority-group", "um-headroom-buffer-count"},
    {BST_ID_EGR_POOL, "egress-service-pool", "um-share-buffer-count"},
    {BST_ID_EGR_MCAST_POOL, "egress-service-pool", "mc-share-buffer-count"},
    {BST_ID_UCAST, "egress-uc-queue", "uc-buffer-count"},
    {BST_ID_MCAST, "egress-mc-queue", "mc-buffer-count"},
    {BST_ID_EGR_UCAST_PORT_SHARED, "egress-port-service-pool", "uc-share-buffer-count"},
    {BST_ID_EGR_PORT_SHARED, "egress-port-service-pool", "um-share-buffer-count"},
    {BST_ID_RQE_QUEUE, "egress-rqe-queue", "rqe-buffer-count"},
    {BST_ID_UCAST_GROUP, "egress-uc-queue-group", "uc-buffer-count"}
  };

  unsigned int i;

  if ((NULL == realm) ||
      (NULL == counter) || 
      (NULL == val))
  {
    return false;
  }

  for (i = 0; i <BST_ID_MAX; i++)
  {
    if ((0 == strcmp(realm, bst_realm_index_map[i].realm)) && 
        (0 == strcmp (counter, bst_realm_index_map[i].counter)))
    {
      *val = bst_realm_index_map[i].index;
      return true;
    }
  }

  return false;
}

/*********************************************************************
* @brief : enable bst on trigger 
*
* @param[in] msg_data : pointer to the bst message request.
* @param[in] bstEnable : bst enable or disable.
*
* @retval  : BVIEW_STATUS_SUCCESS : successfully re-enabled bst 
* @retval  : BVIEW_STATUS_FAILURE : failed to re-enable bst.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note
*
*********************************************************************/
BVIEW_STATUS bst_enable_on_trigger(BVIEW_BST_REQUEST_MSG_t *msg_data, int bstEnable)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BST_CONFIG_t bstMode;

  if ((BVIEW_BST_CMD_API_TRIGGER_REPORT != msg_data->msg_type) &&
      (BVIEW_BST_CMD_API_ENABLE_BST_ON_TRIGGER != msg_data->msg_type))
  {
    /* ignore */
    return BVIEW_STATUS_SUCCESS;
  }

  memset (&bstMode, 0, sizeof (BVIEW_BST_CONFIG_t));
  bstMode.enableDeviceStatsMonitoring = BVIEW_BST_DEFAULT_TRACK_DEVICE;
  bstMode.enableIngressStatsMonitoring = BVIEW_BST_DEFAULT_TRACK_INGRESS;
  bstMode.enableEgressStatsMonitoring = BVIEW_BST_DEFAULT_TRACK_EGRESS;
  bstMode.mode = BVIEW_BST_MODE_CURRENT;
  bstMode.enableStatsMonitoring = true;
  /* Set the asic with the desired config to control bst */
  rv = sbapi_bst_config_set (msg_data->unit, &bstMode);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    _BST_LOG(_BST_DEBUG_TRACE,"Failed to enable the bst on unit %d\r\n", msg_data->unit);
  }

  return rv;

}



/*********************************************************************
* @brief : bst trigger main application function which does processing of 
*          trigger messages
*
* @param[in] : none
*
* @retval  : BVIEW_STATUS_SUCCESS: 
* @retval  : BVIEW_STATUS_FAILURE: Fails to process the trigger messages 
*
* @note  : This api is the processing thread of the bst trigger application. 
*          All the incoming requests are processed. 
*          Currently the assumption is made that if the 
*          thread fails to read continously 10 or more messages,
*          then there is some error and the thread exits.
*
*********************************************************************/

BVIEW_STATUS bst_trigger_main(void)
{
  BVIEW_BST_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv = BVIEW_STATUS_FAILURE;
  unsigned int rcvd_err = 0;
  unsigned int num_units = 0;
  BVIEW_BST_API_HANDLER_t handler;

  while (1)
  {
    if (-1 != (msgrcv (bst_info.recvTriggerMsgQid, &msg_data, sizeof (msg_data), 0, 0))) 
    {
      /* get num units */
       num_units = 0;
      if (BVIEW_STATUS_SUCCESS != sbapi_system_num_units_get ((int *) &num_units))
      {
        LOG_POST (BVIEW_LOG_ERROR, "Failed to get num of units\r\n");
      }

      _BST_LOG(_BST_DEBUG_INFO, "trigger : msg_data info\n"
            "msg_data.msg_type = %ld\n"
            "msg_data.unit = %d\n"
            "msg_data.cookie = %d\n"
            "num_units = %d\n",
            msg_data.msg_type, msg_data.unit, (NULL == msg_data.cookie)? true: false, num_units); 

      if (msg_data.unit >= num_units)
      {
        /* requested id is greater than the number of units.
           send failure message and continue */
          LOG_POST (BVIEW_LOG_ERROR,
              "failed to send response for command %ld , err = %d. \r\n", msg_data.msg_type, rv);
        continue;
      }
      /* Counter to check for read errors.. 
        successfully read the message. clear this to 0*/
      rcvd_err = 0;
      /* get the api function for the method type */
      if (BVIEW_STATUS_SUCCESS != bst_type_api_get(msg_data.msg_type, &handler))
      {
        continue;
      }

      handler(&msg_data);
    }
    else
    {
      LOG_POST (BVIEW_LOG_ERROR,
          "Failed to read message from the bst trigger application queue. err = %d\r\n", errno);
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
                 "exiting from bst trigger pthread ... \r\n");
  pthread_exit(&bst_info.bst_trigger_thread);
  return BVIEW_STATUS_FAILURE;
}

/*********************************************************************
* @brief : application function to process trigger messages 
*
* @param[in] msg_data : pointer to the bst message request.
*
* @retval  : BVIEW_STATUS_SUCCESS : when the request is successfully processed 
* @retval  : BVIEW_STATUS_FAILURE : when the processing of the request failed. 
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : Inpput paramerts are invalid. 
*
* @note : This function is invoked in the bst thread context and used to 
*         -- bst enable
*         -- send the message to bst thread if the sending of trigger report is allowed. 
*
*********************************************************************/

BVIEW_STATUS bst_process_trigger(BVIEW_BST_REQUEST_MSG_t *msg_data)
{
  bool send_trigger = true;
  int bstMaxTriggers = 0;
  BVIEW_BST_REQUEST_MSG_t bst_msg = {0};
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BST_CONFIG_PARAMS_t *ptr = NULL;
  unsigned int index = 0;

  /* check if the trigger report needs to be collected */
  send_trigger = bst_trigger_index_get(msg_data->triggerInfo.realm, 
                                       msg_data->triggerInfo.counter,
                                       &index);
  if (true == send_trigger)
  {
    /* get the configuration structure pointer  for the desired unit */
    ptr = BST_CONFIG_FEATURE_PTR_GET (msg_data->unit);
    if (NULL == ptr)
    {
      return BVIEW_STATUS_INVALID_PARAMETER;
    }
    /* take lock */
    BST_RWLOCK_RD_LOCK (msg_data->unit);
    bstMaxTriggers = ptr->bstMaxTriggers;
    /* release lock */

    if ((bst_info.unit[msg_data->unit].bst_trigger_count[index] >= bstMaxTriggers) && (0 != bstMaxTriggers))
    {
      send_trigger = false;
    }
    BST_RWLOCK_UNLOCK (msg_data->unit);
  }

  if (true == send_trigger)
  {
    /* post the trigger request to the bst thread */
    bst_msg.unit = msg_data->unit;
    bst_msg.msg_type = BVIEW_BST_CMD_API_TRIGGER_REPORT;
    bst_msg.report_type = BVIEW_BST_STATS_TRIGGER;
    memcpy(&bst_msg.triggerInfo, &msg_data->triggerInfo, sizeof(BVIEW_BST_TRIGGER_INFO_t));

    /* Send the message to the bst application */
    rv = bst_send_request (&bst_msg);
    if (BVIEW_STATUS_SUCCESS != rv)
    {
      LOG_POST (BVIEW_LOG_ERROR,
          "Failed to send trigger collection message to bst application. err = %d\r\n", rv);
      return BVIEW_STATUS_FAILURE;
    }
    LOG_POST (BVIEW_LOG_INFO,
        "trigger collet request sent to bst application successfully for unit %d\r\n", msg_data->unit);


    if ((0 == bst_info.unit[msg_data->unit].bst_trigger_count[index]) && (0 != bstMaxTriggers))
    {
      /* start the timer */
      bst_trigger_timer_add(msg_data->unit);
    }
  }
    BST_RWLOCK_WR_LOCK (msg_data->unit);
    bst_info.unit[msg_data->unit].bst_trigger_count[index]++;
    BST_RWLOCK_UNLOCK (msg_data->unit);

  return rv;
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
BVIEW_STATUS bst_trigger_send_request (BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  int rv = BVIEW_STATUS_SUCCESS;

  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  if (-1 == msgsnd (bst_info.recvTriggerMsgQid, msg_data, sizeof (BVIEW_BST_REQUEST_MSG_t), IPC_NOWAIT))
  {
    LOG_POST (BVIEW_LOG_ERROR,
              "Failed to send message to bst application,  %ld, errno  %d\r\n",
              msg_data->msg_type, errno);
    rv = BVIEW_STATUS_FAILURE;
  }

  return rv;
}


/*************************************************************
*@brief:  Callback function to send the trigger to bst application
*         to send periodic collection
*
* @param[in] unit : unit for which the trigger is generated 
* @param[in] cookie : cookie
* @param[in] type : trigger type
*
* @retval  : BVIEW_STATUS_SUCCESS
* @retval  : BVIEW_STATUS_FAILURE
*
* @note  : callback function from ASIC to bst application 
*          to send the triggers. This function is invoked 
*          in the asic context, when the congigured thresholds 
*          exceeds the values. In such case, 
*          the trigger is generated by the asic and bst 
*          application take a trigger report and sends the same to collector.
*
*************************************************************/
BVIEW_STATUS bst_trigger_cb (int unit, void *cookie,
                             BVIEW_BST_TRIGGER_INFO_t *triggerInfo)
{
  BVIEW_BST_REQUEST_MSG_t msg_data = {0};
  BVIEW_STATUS rv;

  msg_data.unit = unit;
  msg_data.msg_type = BVIEW_BST_CMD_API_TRIGGER_COLLECT;
  memcpy(&msg_data.triggerInfo, triggerInfo, sizeof(BVIEW_BST_TRIGGER_INFO_t));

  /* Send the message to the bst application */
  rv = bst_trigger_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
      "Failed to send trigger collection message to application. err = %d\r\n", rv);
       return BVIEW_STATUS_FAILURE;
  }
    LOG_POST (BVIEW_LOG_INFO,
      "trigger call back sent to application successfully for unit %d\r\n", unit);
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
*  @brief:  callback function to clear the trigger count  
*
* @param[in]   sigval : Data passed with notification after timer expires
*
* @retval  : BVIEW_STATUS_SUCCESS : message is successfully posted to bst.
* @retval  : BVIEW_STATUS_FAILURE : failed to post message to bst.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameters to function.
*
* @note : when the trigger timer expires, this api is invoked in
*         the timer context.
*
*********************************************************************/
BVIEW_STATUS bst_trigger_timer_cb (union sigval sigval)
{
  BVIEW_BST_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv; 

  msg_data.msg_type = BVIEW_BST_CMD_API_CLEAR_TRIGGER_COUNT;
  msg_data.unit = (*(int *)sigval.sival_ptr);
  /* Send the message to the bst application */
  rv = bst_trigger_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "Failed to send trigger count clear message to application. err = %d\r\n", rv);
    return BVIEW_STATUS_FAILURE;
  }

    LOG_POST (BVIEW_LOG_INFO,
        "timer call back received for unit = %d\r\n", *(int *)sigval.sival_ptr);
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief : function to add timer for the periodic stats collection 
*
* @param[in] unit : unit for which the periodic stats need to be collected.
*
* @retval  : BVIEW_STATUS_INVALID_PARAMETER -- Inpput paramerts are invalid. 
* @retval  : BVIEW_STATUS_FAILURE -- failed to add the timer 
* @retval  : BVIEW_STATUS_SUCCESS -- timer is successfully added 
*
* @note : this api adds the timer to the linux timer thread, so when the timer 
*         expires, we receive the callback and post message to the bst application
*         to collect the stats.. this is a periodic timer , whose interval
*         is equal to the collection interval. Note that collection is per
*         unit and hence we need per timer per unit.
*
*********************************************************************/
BVIEW_STATUS bst_trigger_timer_add (unsigned int  unit)
{
  BVIEW_BST_CONFIG_PARAMS_t *ptr;
  BVIEW_BST_DATA_t *bst_data_ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  int interval = 0;

  bst_data_ptr = BST_UNIT_DATA_PTR_GET (unit);
  ptr = BST_CONFIG_FEATURE_PTR_GET (unit);

  if ((NULL == bst_data_ptr) || (NULL == ptr)) 
    return BVIEW_STATUS_INVALID_PARAMETER;

  /* check if the timer node is already in use.
  */
    /* take lock */
    BST_RWLOCK_WR_LOCK (unit);

  if (true != bst_data_ptr->bst_trigger_timer.in_use)
  {
    interval = ptr->triggerTransmitInterval;


    /* The timer add function expects the time in milli seconds..
       so convert the time into milli seconds. , before adding
       the timer node */
    
    rv =  system_timer_add (bst_trigger_timer_cb,
        &bst_data_ptr->bst_trigger_timer.bstTimer,
        interval*BVIEW_BST_TIME_CONVERSION_FACTOR,
        PERIODIC_MODE, &bst_data_ptr->bst_trigger_timer.context.unit);

    if (BVIEW_STATUS_SUCCESS == rv)
    {
      bst_data_ptr->bst_trigger_timer.in_use = true;
      LOG_POST (BVIEW_LOG_INFO,
          "bst application: trigger timer is successfully started for unit %d.\r\n", unit);
    }
    else
    {
      /* timer node add has failed. log the same */
      LOG_POST (BVIEW_LOG_ERROR,
          "Failed to add trigger timer for unit %d, err %d \r\n", unit, rv);
    }
  }
    /* release lock */
    BST_RWLOCK_UNLOCK (unit);
  return rv;
}


/*********************************************************************
* @brief : Deletes the timer node for the given unit
*
* @param[in] unit : unit id for which  the timer needs to be deleted.
*
* @retval  : BVIEW_STATUS_INVALID_PARAMETER -- Inpput paramerts are invalid. 
* @retval  : BVIEW_STATUS_FAILURE -- timer is successfully deleted 
* @retval  : BVIEW_STATUS_SUCCESS -- failed to delete the timer 
*
* @note  : The periodic timer is deleted when send asyncronous reporting
*          is turned off. This timer is per unit.
*
*********************************************************************/
BVIEW_STATUS bst_trigger_timer_delete (int unit)
{
  BVIEW_BST_DATA_t *bst_data_ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

  bst_data_ptr = BST_UNIT_DATA_PTR_GET (unit);

  if (NULL == bst_data_ptr)
    return BVIEW_STATUS_INVALID_PARAMETER;

    /* take lock */
    BST_RWLOCK_WR_LOCK (unit);
  if (true == bst_data_ptr->bst_trigger_timer.in_use)
  {
    rv = system_timer_delete (bst_data_ptr->bst_trigger_timer.bstTimer);
    if (BVIEW_STATUS_SUCCESS == rv)
    {
      bst_data_ptr->bst_trigger_timer.in_use = false;
        LOG_POST (BVIEW_LOG_INFO,
              "bst application: successfully deleted trigger timer for unit %d , timer id %d.\r\n", unit, bst_data_ptr->bst_trigger_timer.bstTimer);
    }
    else
    {
      /* timer node add has failed. log the same */
      LOG_POST (BVIEW_LOG_ERROR, 
           "Failed to delete trigger timer for unit %d, err %d \r\n", unit, rv);
    }
  }
    /* release lock */
    BST_RWLOCK_UNLOCK (unit);
  
  return rv;
}


