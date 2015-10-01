/*****************************************************************************
  *
  * (C) Copyright Broadcom Corporation 2015
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
#include "broadview.h"
#include "packet_trace.h"
#include "get_packet_trace_feature.h"
#include "system.h"
#include "openapps_log_api.h"
#include "packet_trace_debug.h"
#include "packet_trace_include.h"
#include "packet_trace_json_encoder.h"
#include "packet_trace_json_memory.h"
#include "common/platform_spec.h"

/*********************************************************************
* @brief : REST API handler to cancel the pt profile 
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to pt queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to pt.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note : This api posts the request to pt application to clear stats. 
*
*********************************************************************/
BVIEW_STATUS ptjson_cancel_pt_profile_impl (void *cookie, int asicId,
                                                int id,
                                                PTJSON_CANCEL_PT_PROFILE_t *
                                                pCommand)
{
  BVIEW_PT_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  /* copy the params from the request */

  memset (&msg_data, 0, sizeof (BVIEW_PT_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.id = id;
  msg_data.msg_type = BVIEW_PT_CMD_API_CANCEL_TRACE_PROFILE;
  msg_data.cmd.cancel_profile.id = pCommand->id;
  /* send message to pt application */
  rv = pt_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
   _PT_LOG(_PT_DEBUG_ERROR, "ptjson_clear_pt_statistics_impl: failed to post clear pt stats request to pt queue. err = %d.\r\n",rv);
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post clear pt stats request to pt queue. err = %d.\r\n",rv);
  }
  return rv;
}

/*********************************************************************
* @brief : REST API handler to cancel the pt profile 
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to pt queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to pt.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note : This api posts the request to pt application to clear stats. 
*
*********************************************************************/
BVIEW_STATUS ptjson_cancel_pt_lag_resolution_impl (void *cookie, int asicId,
                                                int id,
                                                PTJSON_CANCEL_PT_PROFILE_t *
                                                pCommand)
{
  BVIEW_PT_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  /* copy the params from the request */

  memset (&msg_data, 0, sizeof (BVIEW_PT_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.id = id;
  msg_data.msg_type = BVIEW_PT_CMD_API_CANCEL_LAG_RESOLUTION;
  msg_data.cmd.cancel_profile.id = pCommand->id;
  /* send message to pt application */
  rv = pt_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
   _PT_LOG(_PT_DEBUG_ERROR, "ptjson_cancel_pt_lag_resolution_impl: failed to post cancel ptlag resolution to pt queue. err = %d.\r\n",rv);
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post cancel ptlag resolution to pt queue. err = %d.\r\n",rv);
  }
  return rv;
}

/*********************************************************************
* @brief : REST API handler to cancel the pt profile 
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to pt queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to pt.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note : This api posts the request to pt application to clear stats. 
*
*********************************************************************/
BVIEW_STATUS ptjson_cancel_pt_ecmp_resolution_impl (void *cookie, int asicId,
                                                int id,
                                                PTJSON_CANCEL_PT_PROFILE_t *
                                                pCommand)
{
  BVIEW_PT_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  /* copy the params from the request */

  memset (&msg_data, 0, sizeof (BVIEW_PT_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.id = id;
  msg_data.msg_type = BVIEW_PT_CMD_API_CANCEL_ECMP_RESOLUTION;
  msg_data.cmd.cancel_profile.id = pCommand->id;
  /* send message to pt application */
  rv = pt_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
   _PT_LOG(_PT_DEBUG_ERROR, "ptjson_cancel_pt_ecmp_resolution_impl: failed to post cancel pt ecmp resolution to pt queue. err = %d.\r\n",rv);
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post cancel pt ecmp resolution to pt queue. err = %d.\r\n",rv);
  }
  return rv;
}


/*********************************************************************
* @brief : REST API handler to configure the pt feature params
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to pt queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to pt.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note    : This api posts the request to pt application to configures pt feature params.
*
* @end
*********************************************************************/
BVIEW_STATUS ptjson_configure_pt_feature_impl (void *cookie, int asicId,
                                                 int id,
                                                 PTJSON_CONFIGURE_PT_FEATURE_t
                                                 * pCommand)
{
  BVIEW_PT_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  if (NULL == pCommand)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }


  memset (&msg_data, 0, sizeof (BVIEW_PT_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.msg_type = BVIEW_PT_CMD_API_SET_FEATURE;
  msg_data.id = id;
  msg_data.cmd.feature = *pCommand;

  /* send message to pt application */
  rv = pt_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
   _PT_LOG(_PT_DEBUG_ERROR, "ptjson_configure_pt_feature_impl: failed to post confiigure pt feature to pt queue. err = %d.\r\n",rv);
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post confiigure pt feature to pt queue. err = %d.\r\n",rv);
  }
  return rv;
}
/*********************************************************************
* @brief : REST API handler to get the pt feature params 
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to pt queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to pt.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note    : This api posts the request to pt application to get the pt feature params.
*
*********************************************************************/
BVIEW_STATUS ptjson_get_pt_feature_impl (void *cookie, int asicId, int id,
                                           PTJSON_GET_PT_FEATURE_t * pCommand)
{
  BVIEW_PT_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  memset (&msg_data, 0, sizeof (BVIEW_PT_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.msg_type = BVIEW_PT_CMD_API_GET_FEATURE;
  msg_data.id = id;
  /* send message to pt application */
  rv = pt_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post get pt feature to pt queue. err = %d.\r\n",rv);
  }
  return rv;
}

/*********************************************************************
* @brief : REST API handler to get the pt profile report 
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to pt queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to pt.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note    : This api posts the request to pt application to get the pt profile report.
*             As a response to this request the report is sent.
*
*********************************************************************/
BVIEW_STATUS ptjson_get_pt_profile_impl (void *cookie, int asicId, int id,
                                          PTJSON_GET_PT_PROFILE_t * pCommand)
{
  BVIEW_PT_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  if (NULL == pCommand)
    return BVIEW_STATUS_INVALID_PARAMETER;

  memset (&msg_data, 0, sizeof (BVIEW_PT_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.msg_type = BVIEW_PT_CMD_API_GET_TRACE_PROFILE;
  msg_data.id = id;
  msg_data.cmd.profile = *pCommand;

  /* send message to pt application */
  rv = pt_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post get pt profile to pt queue. err = %d.\r\n",rv);
  }
  return rv;
}

/*********************************************************************
* @brief : REST API handler to get the pt profile report 
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to pt queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to pt.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note    : This api posts the request to pt application to get the pt profile report.
*             As a response to this request the report is sent.
*
*********************************************************************/
BVIEW_STATUS ptjson_get_pt_lag_resolution_impl (void *cookie, int asicId, int id,
                                          PTJSON_GET_PT_PROFILE_t * pCommand)
{
  BVIEW_PT_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  if (NULL == pCommand)
    return BVIEW_STATUS_INVALID_PARAMETER;

  memset (&msg_data, 0, sizeof (BVIEW_PT_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.msg_type = BVIEW_PT_CMD_API_GET_LAG_RESOLUTION;
  msg_data.id = id;
  msg_data.cmd.profile = *pCommand;

  /* send message to pt application */
  rv = pt_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post get pt report to pt queue. err = %d.\r\n",rv);
  }
  return rv;
}


/*********************************************************************
* @brief : REST API handler to get the pt profile report 
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to pt queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to pt.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note    : This api posts the request to pt application to get the pt profile report.
*             As a response to this request the report is sent.
*
*********************************************************************/
BVIEW_STATUS ptjson_get_pt_ecmp_resolution_impl (void *cookie, int asicId, int id,
                                          PTJSON_GET_PT_PROFILE_t * pCommand)
{
  BVIEW_PT_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  if (NULL == pCommand)
    return BVIEW_STATUS_INVALID_PARAMETER;

  memset (&msg_data, 0, sizeof (BVIEW_PT_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.msg_type = BVIEW_PT_CMD_API_GET_ECMP_RESOLUTION;
  msg_data.id = id;
  msg_data.cmd.profile = *pCommand;

  /* send message to pt application */
  rv = pt_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post get pt report to pt queue. err = %d.\r\n",rv);
  }
  return rv;
}


