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

#ifndef INCLUDE_REG_HB_H
#define INCLUDE_REG_HB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include "modulemgr.h"



#define _REG_HB_DEBUG
#define _REG_HB_DEBUG_LEVEL        0xFF

#define _REG_HB_DEBUG_TRACE        (0x1)
#define _REG_HB_DEBUG_INFO         (0x01 << 1)
#define _REG_HB_DEBUG_ERROR        (0x01 << 2)
#define _REG_HB_DEBUG_ALL          (0xFF)

#ifdef _REG_HB_DEBUG
#define _REG_HB_LOG(level, format,args...)   do { \
                if ((level) & _REG_HB_DEBUG_LEVEL) { \
                                                  printf(format, ##args); \
                                              } \
            }while(0)
#else
#define _REG_HB_LOG(level, format,args...)
#endif


  /* Default values for system utility configurations */
    /* heart beat enable */
#define BVIEW_REG_HB_DEFAULT_FEATURE_ENABLE   true 

    /* default heart beat timer interval.
            */
#define BVIEW_REG_HB_DEFAULT_HEARTBEAT_INTERVAL 5 

#define BVIEW_REG_HB_CMD_API_MAX BVIEW_REG_HB_CMD_API_GET_SWITCH_PROPERTIES

typedef REG_HB_JSON_CONFIGURE_FEATURE_t     BVIEW_REG_HB_CONFIG_PARAMS_t;


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
BVIEW_STATUS reg_hb_config_feature_set (BVIEW_SYSTEM_UTILS_REQUEST_MSG_t * msg_data);

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
BVIEW_STATUS reg_hb_config_feature_get (BVIEW_SYSTEM_UTILS_REQUEST_MSG_t * msg_data);
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
BVIEW_STATUS reg_hb_switch_properties_get (BVIEW_SYSTEM_UTILS_REQUEST_MSG_t * msg_data);

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
BVIEW_STATUS reg_hb_heartbeat_timer_start (void);

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
BVIEW_STATUS reg_hb_heartbeat_timer_stop (void);

/*********************************************************************
 *  @brief:  timer callback function to send heart beat messages
 *
 * @param[in]   sigval : Data passed with notification after timer expires
 *
 * @retval  : BVIEW_STATUS_SUCCESS : message is successfully posted to reg_hb.
 * @retval  : BVIEW_STATUS_FAILURE : failed to post message to reg_hb.
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameters to function.
 *
 * @note : when the periodic collection timer expires, this api is invoked in
 *         the timer context. We just post the request for the report, while
 *        posting we mark the report type as periodic. The parameter returned
 *       from the callback is the reference to unit id.
 *
 *********************************************************************/
BVIEW_STATUS reg_hb_timer_cb (union sigval sigval);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_REG_HB_H */

