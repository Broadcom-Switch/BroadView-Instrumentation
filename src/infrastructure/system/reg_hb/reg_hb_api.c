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
#include "json.h"
#include "feature.h"
#include "configure_reg_hb_feature.h"
#include "get_switch_properties.h"
#include "broadview.h"
#include "openapps_feature.h"
#include "cancel_request.h"
#include "system.h"
#include "system_utils.h"
#include "reg_hb.h"
#include "openapps_log_api.h"
#include "sbplugin_redirect_system.h"

extern BVIEW_SYSTEM_UTILS_CXT_t system_utils_info;

/*********************************************************************
  * @brief : application function to configure the system utility features
  *
  * @param[in] msg_data : pointer to the bst message request.
  *
  * @retval  : BVIEW_STATUS_SUCCESS : when the asic successfully programmed
  * @retval  : BVIEW_STATUS_FAILURE : when the asic is failed to programme.
  * @retval  : BVIEW_STATUS_INVALID_PARAMETER : Inpput paramerts are invalid.
  *
  * @note : This function is invoked in the system utility context and used to
  *         configure the parameters like
  *         -- enable or disable registartion and heartbeat messages 
  *         -- configuring the heartbeat interval
  *         In case of the underlying  api returns failure, the same error message
  *         is received  and sent to the invoking function.
  *
  *********************************************************************/
BVIEW_STATUS reg_hb_config_feature_set (BVIEW_SYSTEM_UTILS_REQUEST_MSG_t * msg_data)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_REG_HB_CONFIG_PARAMS_t *ptr = NULL;
  int tmpMask = 0;
  bool modifyTimer = false;

  /* check for the null of the input pointer */
  if (NULL == msg_data)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  /* get the configuration structure pointer  for the desired unit */
  ptr = &system_utils_info.data->config;
  if (NULL == ptr)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  /* collection interval is maintained in seconds in application.
     while adding the timer, the same should be converted into
     milli seconds and added as the timer api expects
     interval in milli seconds. */
  tmpMask = msg_data->request.config.configMask;

  if ((tmpMask & (1 << REG_HB_CONFIG_PARAMS_ENABLE)) &&
      (ptr->enable != msg_data->request.config.enable))
  {
    ptr->enable = msg_data->request.config.enable;
     modifyTimer = true;
  }

  if ((tmpMask & (1 << REG_HB_CONFIG_PARAMS_INTRVL)) &&
      (ptr->interval != msg_data->request.config.interval))
  {
    ptr->interval = msg_data->request.config.interval;

    if (0 != ptr->interval)
    {
      modifyTimer = true;
    }
  }

  /* stop timer when the feature is disabled or when the timer is modified */

  if ((false == ptr->enable) || (0 == ptr->interval))
  {
    reg_hb_heartbeat_timer_stop();
  }

  /* restart only if the timer is modified and the feature is enabled */
   if ((true == modifyTimer ) && ((true == ptr->enable) &&  (0 != ptr->interval)))
   {
     reg_hb_heartbeat_timer_start();
   }

  return rv;
}


/*********************************************************************
 * @brief : application function to get the system utility features
 *
 * @param[in] msg_data : pointer to the message request.
 *
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER : Inpput paramerts are invalid.
 * @retval  : BVIEW_STATUS_SUCCESS : when the feature params are 
 *                                   retrieved successfully.
 *
 * @note
 *
 *********************************************************************/
BVIEW_STATUS reg_hb_config_feature_get (BVIEW_SYSTEM_UTILS_REQUEST_MSG_t * msg_data)
{
  BVIEW_REG_HB_CONFIG_PARAMS_t *ptr;


  if (NULL == msg_data)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  ptr = &system_utils_info.data->config;
  if (NULL == ptr)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
  return  BVIEW_STATUS_SUCCESS;
}


/*********************************************************************
  * @brief : application function to get switch properties
  *
  * @param[in] msg_data : pointer to the message request.
  *
  * @retval  : BVIEW_STATUS_INVALID_PARAMETER : Inpput paramerts are invalid.
  * @retval  : BVIEW_STATUS_SUCCESS  : successfully retrieved the switch
  *                                    properties.
  * @note
  *
  *********************************************************************/
BVIEW_STATUS reg_hb_switch_properties_get (BVIEW_SYSTEM_UTILS_REQUEST_MSG_t * msg_data)
{
  time_t *value;
  if (NULL == msg_data)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  value = &system_utils_info.switchProperties->timeStamp;
  memset(value, 0, sizeof(time_t));
  sbapi_system_time_get(value);
  return BVIEW_STATUS_SUCCESS;
}


/*********************************************************************
  * @brief : function to add timer for the periodic heartbeat 
  *
  * @param[in] unit : unit for which the heart beat need to be advertized.
  *
  * @retval  : BVIEW_STATUS_INVALID_PARAMETER -- Inpput paramerts are invalid.
  * @retval  : BVIEW_STATUS_FAILURE -- failed to add the timer
  * @retval  : BVIEW_STATUS_SUCCESS -- timer is successfully added
  *
  * @note : this api adds the timer to the linux timer thread, so when the timer
  *         expires, we receive the callback and post message to the application.
  *
  *********************************************************************/
BVIEW_STATUS reg_hb_heartbeat_timer_start (void)
{
  BVIEW_SYSTEM_UTILS_DATA_t *ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;


  ptr = system_utils_info.data;

  if (NULL == ptr)
    return BVIEW_STATUS_INVALID_PARAMETER;
  /* check if the timer node is already in use.
   */
  if (true == ptr->heart_beat_timer.in_use)
  {
    /* the timer is in use. The requester has asked
       to add the timer again.. Remove the old entru
       and add it again.. Reasosn could be that config
       interval would have been changed, In such case,
       delete the one with previous collection timer
       interval and add the new one */
    rv =  reg_hb_heartbeat_timer_stop();
    if (BVIEW_STATUS_SUCCESS != rv)
    {
      /* timer node add has failed. log the same */
      LOG_POST (BVIEW_LOG_ERROR,
          "%s Failed to delete heart beat timer err %d \r\n", __func__, rv);
    }
  }

  /* The timer add function expects the time in milli seconds..
     so convert the time into milli seconds. , before adding
     the timer node */
  rv =  system_timer_add (reg_hb_timer_cb,
      &ptr->heart_beat_timer.timer,
      ptr->config.interval*BVIEW_SYSTEM_TIME_CONVERSION_FACTOR,
      PERIODIC_MODE, 0);

  if (BVIEW_STATUS_SUCCESS == rv)
  {
    ptr->heart_beat_timer.in_use = true;
    LOG_POST (BVIEW_LOG_INFO,
        "registration and heart beat: heart beat timer is successfully started.\r\n");
  }
  else
  {
    /* timer node add has failed. log the same */
    LOG_POST (BVIEW_LOG_ERROR,
        "Failed to add heart beat timer. err %d \r\n", rv);
  }
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
  * @note  : The heart beat timer is deleted when send feature is turned off 
  *
  *********************************************************************/
BVIEW_STATUS reg_hb_heartbeat_timer_stop (void)
{
  BVIEW_SYSTEM_UTILS_DATA_t *ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

  ptr = system_utils_info.data;

  if (NULL == ptr)
    return BVIEW_STATUS_INVALID_PARAMETER;

  if (true == ptr->heart_beat_timer.in_use)
  {
    rv = system_timer_delete (ptr->heart_beat_timer.timer);
    if (BVIEW_STATUS_SUCCESS == rv)
    {
      ptr->heart_beat_timer.in_use = false;
      LOG_POST (BVIEW_LOG_INFO,
          "Registration and heart beat: successfully deleted heart  beat timer id %d.\r\n", ptr->heart_beat_timer.timer);
    }
    else
    {
      /* timer node add has failed. log the same */
      LOG_POST (BVIEW_LOG_ERROR,
          "Failed to delete heart beat timer for err %d \r\n", rv);
    }
  }

  return rv;
}


/*********************************************************************
  *  @brief:  timer callback function to send heart beat messages 
  *
  * @param[in]   sigval : Data passed with notification after timer expires
  *
  * @retval  : BVIEW_STATUS_SUCCESS : message is successfully posted to system_utils.
  * @retval  : BVIEW_STATUS_FAILURE : failed to post message to system_utils.
  * @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameters to function.
  *
  * @note : when the periodic collection timer expires, this api is invoked in
  *         the timer context. We just post the request for the report, while
  *        posting we mark the report type as periodic. The parameter returned
  *       from the callback is the reference to unit id.
  *
  *********************************************************************/
BVIEW_STATUS reg_hb_timer_cb (union sigval sigval)
{
  BVIEW_SYSTEM_UTILS_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  memset(&msg_data, 0, sizeof(BVIEW_SYSTEM_UTILS_REQUEST_MSG_t));
  msg_data.report_type = BVIEW_SYSTEM_UTILS_PERIODIC;
  msg_data.msg_type = BVIEW_SYSTEM_UTILS_CMD_API_GET_SWITCH_PROPERTIES;
  /* Send the message to the system utility application */
  rv = system_utils_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "Failed to send heart beat timer expiry message to application. err = %d\r\n", rv);
    return BVIEW_STATUS_FAILURE;
  }
  return BVIEW_STATUS_SUCCESS;
}

