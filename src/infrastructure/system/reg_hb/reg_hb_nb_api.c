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

#include "json.h"
#include "system_utils_json_memory.h"
#include "configure_reg_hb_feature.h"
#include "get_reg_hb_feature.h"
#include "get_switch_properties.h"
#include "cancel_request.h"
#include "system.h"
#include "broadview.h"
#include "openapps_log_api.h"
#include "system.h"
#include "system_utils.h"
#include "reg_hb.h"
#include "reg_hb_json_encoder.h"

/*********************************************************************
  * @brief : REST API handler to get switch properties params
  *
  * @param[in] cookie : pointer to the cookie
  * @param[in] id     : unit id
  * @param[in] pCommand : pointer to the input command structure
  *
  * @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted
  *            to sys utility queue.
  * @retval  : BVIEW_STATUS_FAILURE : failed to post the message to application.
  * @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
  *
  * @note    : This api posts the request to application to get
  *            switch properties params.
  *
  *********************************************************************/
BVIEW_STATUS reg_hb_json_get_switch_properties_impl (void *cookie, 
                                                     int id,
                                                     REG_HB_JSON_GET_SWITCH_PROPERTIES_t *pCommand)
{
  BVIEW_STATUS rv;
  BVIEW_SYSTEM_UTILS_REQUEST_MSG_t msg_data;

  memset (&msg_data, 0, sizeof (BVIEW_SYSTEM_UTILS_REQUEST_MSG_t));
  msg_data.cookie = cookie;
  msg_data.id = id;
  msg_data.msg_type = BVIEW_SYSTEM_UTILS_CMD_API_GET_SWITCH_PROPERTIES;
  /* send message to system utils application */
  rv = system_utils_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post get switch properties to system utils queue. err = %d.\r\n",rv);
  }
  return rv;
}


/*********************************************************************
 * @brief : REST API handler to configure the system_utils feature params
 *
 * @param[in] cookie : pointer to the cookie
 * @param[in] id     : unit id
 * @param[in] pCommand : pointer to the input command structure
 *
 * @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to system_utils queue.
 * @retval  : BVIEW_STATUS_FAILURE : failed to post the message to system_utils.
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
 *
 * @note    : This api posts the request to system_utils application to configures system_utils feature params.
 *
 * @end
 *********************************************************************/
BVIEW_STATUS reg_hb_json_configure_feature_impl (void *cookie,
    int id,
    REG_HB_JSON_CONFIGURE_FEATURE_t
    * pCommand)
{
  BVIEW_SYSTEM_UTILS_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  if (NULL == pCommand)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }


  memset (&msg_data, 0, sizeof (BVIEW_SYSTEM_UTILS_REQUEST_MSG_t));
  msg_data.cookie = cookie;
  msg_data.msg_type = BVIEW_SYSTEM_UTILS_CMD_API_SET_FEATURE;
  msg_data.id = id;
  msg_data.request.config = *pCommand;

  /* send message to system_utils application */
  rv = system_utils_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    _REG_HB_LOG(_REG_HB_DEBUG_ERROR, "reg_hb_json_configure_feature_impl: failed to post confiigure reg_hb feature to system_utils queue. err = %d.\r\n",rv);
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post configure system_utils feature to system_utils queue. err = %d.\r\n",rv);
  }
  return rv;
}


/*********************************************************************
 * @brief : REST API handler to get the system utils feature params
 *
 * @param[in] cookie : pointer to the cookie
 * @param[in] id     : unit id
 * @param[in] pCommand : pointer to the input command structure
 *
 * @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to system utils queue.
 * @retval  : BVIEW_STATUS_FAILURE : failed to post the message to system utils.
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
 *
 * @note    : This api posts the request to system utils application to get the system utils feature params.
 *
 *********************************************************************/
BVIEW_STATUS reg_hb_json_get_feature_impl (void *cookie, int id,
    REG_HB_JSON_GET_FEATURE_t * pCommand)
{
  BVIEW_SYSTEM_UTILS_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  memset (&msg_data, 0, sizeof (BVIEW_SYSTEM_UTILS_REQUEST_MSG_t));
  msg_data.cookie = cookie;
  msg_data.msg_type = BVIEW_SYSTEM_UTILS_CMD_API_GET_FEATURE;
  msg_data.id = id;
  /* send message to system utils application */
  rv = system_utils_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post get system utils feature to system utils queue. err = %d.\r\n",rv);
  }
  return rv;
}

/*********************************************************************
  * @brief : REST API handler to cancel request 
  *
  * @param[in] cookie : pointer to the cookie
  * @param[in] id     : unit id
  * @param[in] pCommand : pointer to the input command structure
  *
  * @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted
  *            to sys utility queue.
  * @retval  : BVIEW_STATUS_FAILURE : failed to post the message to application.
  * @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
  *
  * @note    : This api posts the request to application to get
  *            switch properties params.
  *
  *********************************************************************/
BVIEW_STATUS reg_hb_json_cancel_request_impl (void *cookie,
                                                     int asicId, 
                                                     int id,
                                                     REG_HB_JSON_CANCEL_REQUEST_t *pCommand)
{
  BVIEW_STATUS rv;
  BVIEW_SYSTEM_UTILS_REQUEST_MSG_t msg_data;

  memset (&msg_data, 0, sizeof (BVIEW_SYSTEM_UTILS_REQUEST_MSG_t));
  msg_data.cookie = cookie;
  msg_data.unit = asicId;
  msg_data.id = id;
  msg_data.msg_type = BVIEW_SYSTEM_UTILS_CMD_API_CANCEL_REQUEST;
  msg_data.request.cancel = *pCommand;
  /* send message to system utils application */
  rv = system_utils_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post get system utils feature to system utils queue. err = %d.\r\n",rv);
  }
  return rv;


  return BVIEW_STATUS_SUCCESS;
}


