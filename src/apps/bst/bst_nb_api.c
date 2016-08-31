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
#include "bst_json_memory.h"
#include "clear_bst_statistics.h"
#include "clear_bst_thresholds.h"
#include "configure_bst_thresholds.h"
#include "configure_bst_feature.h"
#include "configure_bst_tracking.h"
#include "get_bst_tracking.h"
#include "get_bst_feature.h"
#include "get_bst_thresholds.h"
#include "get_bst_report.h"
#include "get_bst_cgsn_drop_counters.h"
#include "bst_json_encoder.h"
#include "system.h"
#include "bst.h"
#include "broadview.h"
#include "openapps_log_api.h"
#include "bst_app.h"
#include "common/platform_spec.h"

/*********************************************************************
  * @brief : function to return the threshold type for the given realm
  *
  * @param[in] str : realm passed in the input request
  *
  * @retval    : the threshold type value
  *
  * @note : The minumum value of enum type is starts from 1.
  *         if 0 is returned then the given realm didn't match
  *         with any of the supported realm types 
  *
  *********************************************************************/
static BVIEW_STATUS bst_realm_type_get (char *str, unsigned int *type, BVIEW_BST_THRESHOLD_API_HANDLER_t *handler)
{
  unsigned int i = 0;

  const BVIEW_BST_REALM_THRESHOLD_t realm_threshold_map[] = {
    {"device", BVIEW_BST_DEVICE_THRESHOLD, bst_device_threshold_input_validate},
    {"ingress-port-priority-group", BVIEW_BST_INGRESS_PORT_PG_THRESHOLD, bst_ippg_threshold_input_validate},
    {"ingress-port-service-pool", BVIEW_BST_INGRESS_PORT_SP_THRESHOLD, bst_ipsp_threshold_input_validate},
    {"ingress-service-pool", BVIEW_BST_INGRESS_SP_THRESHOLD, bst_isp_threshold_input_validate},
    {"egress-port-service-pool", BVIEW_BST_EGRESS_PORT_SP_THRESHOLD, bst_epsp_threshold_input_validate},
    {"egress-service-pool", BVIEW_BST_EGRESS_SP_THRESHOLD, bst_esp_threshold_input_validate},
    {"egress-uc-queue", BVIEW_BST_EGRESS_UC_QUEUE_THRESHOLD, bst_euq_threshold_input_validate},
    {"egress-uc-queue-group", BVIEW_BST_EGRESS_UC_QUEUEGROUPS_THRESHOLD, bst_euqg_threshold_input_validate},
    {"egress-mc-queue", BVIEW_BST_EGRESS_MC_QUEUE_THRESHOLD, bst_emc_threshold_input_validate},
    {"egress-cpu-queue", BVIEW_BST_EGRESS_CPU_QUEUE_THRESHOLD, bst_ecpu_threshold_input_validate},
    {"egress-rqe-queue", BVIEW_BST_EGRESS_RQE_QUEUE_THRESHOLD, bst_erqe_threshold_input_validate}
  };
  for (i = BVIEW_BST_MAX_THRESHOLD_TYPE_MIN; i <= BVIEW_BST_MAX_THRESHOLD_TYPE_MAX; i++)
  {
    if (0 == strcmp (str, realm_threshold_map[i-1].realm))
    {
       _BST_LOG(_BST_DEBUG_TRACE, "requested realm %s found match for the realm type %d\n", str, realm_threshold_map[i-1].threshold);
       *handler = realm_threshold_map[i-1].handler;
       *type = realm_threshold_map[i-1].threshold;
      return BVIEW_STATUS_SUCCESS;
    }
  }
  _BST_LOG(_BST_DEBUG_ERROR, "requested realm %s not found match for the realm type \n ", str);
  return BVIEW_STATUS_FAILURE;
}

/*********************************************************************
* @brief : REST API handler to clear the bst stats
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to bst queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to bst.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note : This api posts the request to bst application to clear stats. 
*
*********************************************************************/
BVIEW_STATUS bstjson_clear_bst_statistics_impl (void *cookie, int asicId,
                                                int id,
                                                BSTJSON_CLEAR_BST_STATISTICS_t *
                                                pCommand)
{
  BVIEW_BST_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  /* copy the params from the request */

  memset (&msg_data, 0, sizeof (BVIEW_BST_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.id = id;
  msg_data.msg_type = BVIEW_BST_CMD_API_CLEAR_STATS;
  /* send message to bst application */
  rv = bst_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
   _BST_LOG(_BST_DEBUG_ERROR, "bstjson_clear_bst_statistics_impl: failed to post clear bst stats request to bst queue. err = %d.\r\n",rv);
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post clear bst stats request to bst queue. err = %d.\r\n",rv);
  }
  return rv;
}
/*********************************************************************
* @brief : REST API handler to clear the bst thresholds
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to bst queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to bst.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note    : This api posts the request to bst application to clear stats.
*
*********************************************************************/
BVIEW_STATUS bstjson_clear_bst_thresholds_impl (void *cookie, int asicId,
                                                int id,
                                                BSTJSON_CLEAR_BST_THRESHOLDS_t *
                                                pCommand)
{
  BVIEW_BST_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  /* copy the params from the decoded request */
  memset (&msg_data, 0, sizeof (BVIEW_BST_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.msg_type = BVIEW_BST_CMD_API_CLEAR_THRESHOLD;
  msg_data.id = id;
  /* send message to bst application */
  rv = bst_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
   _BST_LOG(_BST_DEBUG_ERROR, "bstjson_clear_bst_thresholds_impl: failed to post clear bst thresholds request to bst queue. err = %d.\r\n",rv);
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post clear bst thresholds request to bst queue. err = %d.\r\n",rv);
  }
  return rv;
}

/*********************************************************************
* @brief : REST API handler to configure the bst feature params
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to bst queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to bst.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note    : This api posts the request to bst application to configures bst feature params.
*
* @end
*********************************************************************/
BVIEW_STATUS bstjson_configure_bst_feature_impl (void *cookie, int asicId,
                                                 int id,
                                                 BSTJSON_CONFIGURE_BST_FEATURE_t
                                                 * pCommand)
{
  BVIEW_BST_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  if (NULL == pCommand)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }


  memset (&msg_data, 0, sizeof (BVIEW_BST_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.msg_type = BVIEW_BST_CMD_API_SET_FEATURE;
  msg_data.id = id;
  msg_data.request.config = *pCommand;

  /* send message to bst application */
  rv = bst_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
   _BST_LOG(_BST_DEBUG_ERROR, "bstjson_configure_bst_feature_impl: failed to post confiigure bst feature to bst queue. err = %d.\r\n",rv);
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post confiigure bst feature to bst queue. err = %d.\r\n",rv);
  }
  return rv;
}

/*********************************************************************
* @brief : REST API handler to configure the bst thresholds 
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to bst queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to bst.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note    : This api posts the request to bst application to configures bst thresholds.
*            The validation for the input params for thresholds is done in this api.
*            If the input is invalid, then error is returned
*
* @end
*********************************************************************/
BVIEW_STATUS bstjson_configure_bst_thresholds_impl (void *cookie, int asicId,
                                                    int id,
                                                    BSTJSON_CONFIGURE_BST_THRESHOLDS_t
                                                    * pCommand)
{
  BVIEW_BST_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  unsigned int threshold_type;
  BVIEW_ASIC_CAPABILITIES_t capabilities;
  BVIEW_BST_THRESHOLD_API_HANDLER_t handler_fn;

  if (NULL == pCommand)
    return BVIEW_STATUS_INVALID_PARAMETER;

  memset (&msg_data, 0, sizeof (BVIEW_BST_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.msg_type = BVIEW_BST_CMD_API_SET_THRESHOLD;
  msg_data.id = id;

  memcpy (msg_data.realm, pCommand->realm, JSON_MAX_NODE_LENGTH);

  if (BVIEW_STATUS_SUCCESS != bst_realm_type_get (pCommand->realm,
                                &threshold_type, &handler_fn))   
  {
      LOG_POST (BVIEW_LOG_ERROR,
          "Invalid threshold type in request. \r\n");
   _BST_LOG(_BST_DEBUG_ERROR, "bstjson_configure_bst_thresholds_impl: Invalid threshold type in request. \r\n");
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  msg_data.threshold_type = threshold_type;

  memset(&capabilities, 0, sizeof(BVIEW_ASIC_CAPABILITIES_t));


  if (BVIEW_STATUS_SUCCESS != sbapi_system_asic_capabilities_get(asicId,
        &capabilities))
  {
    /* unable to get the asic capabilities
       log error and return */
    LOG_POST (BVIEW_LOG_ERROR,
        "Failed to Get Asic capabilities for unit %d. \r\n", asicId);
    return BVIEW_STATUS_FAILURE;
  }

  _BST_INPUT_PARAMS_CHECK(threshold_type, capabilities, pCommand);

  if (BVIEW_STATUS_SUCCESS != handler_fn(pCommand, &msg_data))
  {
   _BST_LOG(_BST_DEBUG_ERROR, "validation for  bst threshold config has failed for threshold type %s\r\n",pCommand->realm);
    LOG_POST (BVIEW_LOG_ERROR,
        "validation for  bst threshold config has failed for threshold type %s\r\n",pCommand->realm);
    return BVIEW_STATUS_INVALID_PARAMETER;
  }


  msg_data.threshold_config_mask = pCommand->mask;
  /* send message to bst application */
    rv = bst_send_request (&msg_data);
    return rv;
}

/*********************************************************************
* @brief : REST API handler to configure the bst track params 
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to bst queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to bst.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note    : This api posts the request to bst application to set the track params.
*
* @end
*********************************************************************/
BVIEW_STATUS bstjson_configure_bst_tracking_impl (void *cookie, int asicId,
                                                  int id,
                                                  BSTJSON_CONFIGURE_BST_TRACKING_t
                                                  * pCommand)
{
  BVIEW_BST_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  if (NULL == pCommand)
    return BVIEW_STATUS_INVALID_PARAMETER;

  memset (&msg_data, 0, sizeof (BVIEW_BST_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.msg_type = BVIEW_BST_CMD_API_SET_TRACK;
  msg_data.id = id;
 msg_data.request.track = *pCommand;
  /* send message to bst application */
  rv = bst_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post configure bst tracking to bst queue. err = %d.\r\n",rv);
  }
  return rv;
}

/*********************************************************************
* @brief : REST API handler to get the bst feature params 
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to bst queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to bst.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note    : This api posts the request to bst application to get the bst feature params.
*
*********************************************************************/
BVIEW_STATUS bstjson_get_bst_feature_impl (void *cookie, int asicId, int id, 
                                           BSTJSON_GET_BST_FEATURE_t * pCommand)
{
  BVIEW_BST_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  memset (&msg_data, 0, sizeof (BVIEW_BST_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.msg_type = BVIEW_BST_CMD_API_GET_FEATURE;
  msg_data.id = id;
  /* send message to bst application */
  rv = bst_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post get bst feature to bst queue. err = %d.\r\n",rv);
  }
  return rv;
}

/*********************************************************************
* @brief : REST API handler to get the bst report params 
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to bst queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to bst.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note    : This api posts the request to bst application to get the bst report.
*             As a response to this request the report is sent.
*
*********************************************************************/
BVIEW_STATUS bstjson_get_bst_report_impl (void *cookie, int asicId, int id,
                                          BSTJSON_GET_BST_REPORT_t * pCommand)
{
  BVIEW_BST_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  if (NULL == pCommand)
    return BVIEW_STATUS_INVALID_PARAMETER;

  memset (&msg_data, 0, sizeof (BVIEW_BST_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.msg_type = BVIEW_BST_CMD_API_GET_REPORT;
  msg_data.id = id;
  msg_data.request.collect = *pCommand;

  /* send message to bst application */
  rv = bst_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post get bst report to bst queue. err = %d.\r\n",rv);
  }
  return rv;
}

/*********************************************************************
* @brief : REST API handler to get the bst threshold 
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to bst queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to bst.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note    : This api posts the request to bst application to get the bst thresholds.
*
*********************************************************************/
BVIEW_STATUS bstjson_get_bst_thresholds_impl (void *cookie, int asicId, int id,
                                              BSTJSON_GET_BST_THRESHOLDS_t *
                                              pCommand)
{
  BVIEW_BST_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  if (NULL == pCommand)
    return BVIEW_STATUS_INVALID_PARAMETER;

  memset (&msg_data, 0, sizeof (BVIEW_BST_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.msg_type = BVIEW_BST_CMD_API_GET_THRESHOLD;
  msg_data.id = id;

  memcpy (&msg_data.request.collect, pCommand, sizeof(BSTJSON_GET_BST_THRESHOLDS_t));

  /* send message to bst application */
  rv = bst_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post get bst thresholds to bst queue. err = %d.\r\n",rv);
  }
  return rv;
}

/*********************************************************************
* @brief : REST API handler to get the bst tracking params
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to bst queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to bst.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note    : This api posts the request to bst application to get the bst thresholds.
*
*********************************************************************/
BVIEW_STATUS bstjson_get_bst_tracking_impl (void *cookie, int asicId, int id,
                                            BSTJSON_GET_BST_TRACKING_t *
                                            pCommand)
{
  BVIEW_BST_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  if (NULL == pCommand)
    return BVIEW_STATUS_INVALID_PARAMETER;

  memset (&msg_data, 0, sizeof (BVIEW_BST_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.msg_type = BVIEW_BST_CMD_API_GET_TRACK;
  msg_data.id = id;
  /* send message to bst application */
  rv = bst_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post get bst tracking to bst queue. err = %d.\r\n",rv);
  }
  return rv;
}

/*********************************************************************
* @brief : REST API handler to get the bst congestion drop counters 
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to bst queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to bst.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note    :
*
*********************************************************************/
BVIEW_STATUS bstjson_get_bst_cgsn_drop_counters_impl (void *cookie, 
                                                      int asicId, int id,
                                          BSTJSON_GET_BST_CGSN_DROP_CTRS_t
                                             *pCommand)
{
  BVIEW_BST_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  if (NULL == pCommand)
    return BVIEW_STATUS_INVALID_PARAMETER;

  memset (&msg_data, 0, sizeof (BVIEW_BST_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.msg_type = BVIEW_BST_CMD_API_GET_CGSN_DRP_CTRS;
  msg_data.id = id;
  msg_data.request.drp_ctrs = *pCommand;
  /* send message to bst application */
  rv = bst_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post get bst-congestion-drop-counters to bst queue. err = %d.\r\n",rv);
  }
  return rv;
}

BVIEW_STATUS bst_device_threshold_input_validate(BSTJSON_CONFIGURE_BST_THRESHOLDS_t *pCommand,
                                                 BVIEW_BST_REQUEST_MSG_t *msg_data)
{
  if ((!(pCommand->mask & BVIEW_BST_DEVICE_MASK)) || 
      (BVIEW_BST_DEVICE_THRESHOLD_CHECK (pCommand)))
  {
    return  BVIEW_STATUS_INVALID_PARAMETER;
  }
 
  msg_data->request.device_threshold.threshold = pCommand->threshold;
  return BVIEW_STATUS_SUCCESS;
}

BVIEW_STATUS bst_ippg_threshold_input_validate(BSTJSON_CONFIGURE_BST_THRESHOLDS_t *pCommand,
                                                 BVIEW_BST_REQUEST_MSG_t *msg_data)
{

  if (!((pCommand->mask & BVIEW_BST_UMSHARE_MASK) || 
     (pCommand->mask & BVIEW_BST_UMHEADROOM_MASK)))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  if (!((pCommand->mask & BVIEW_BST_PORT_MASK) &&
      (pCommand->mask & BVIEW_BST_PG_MASK)))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->threshold.port = pCommand->port;
    msg_data->threshold.priorityGroup = pCommand->priorityGroup;
  }

  if ((pCommand->mask & BVIEW_BST_UMSHARE_MASK) &&
      (BVIEW_BST_IPPG_SHRD_THRESHOLD_CHECK (pCommand)))
  {
    return  BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->request.i_p_pg_threshold.umShareThreshold =
      pCommand->umShareThreshold;
  }

  if ((pCommand->mask & BVIEW_BST_UMHEADROOM_MASK) &&
      (BVIEW_BST_IPPG_HDRM_THRESHOLD_CHECK (pCommand)))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->request.i_p_pg_threshold.umHeadroomThreshold =
      pCommand->umHeadroomThreshold;
  }

  return BVIEW_STATUS_SUCCESS;
}

BVIEW_STATUS bst_ipsp_threshold_input_validate(BSTJSON_CONFIGURE_BST_THRESHOLDS_t *pCommand,
                                                 BVIEW_BST_REQUEST_MSG_t *msg_data)
{
  if (!(pCommand->mask & BVIEW_BST_UMSHARE_MASK))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  if (!((pCommand->mask & BVIEW_BST_PORT_MASK) &&
      (pCommand->mask & BVIEW_BST_SP_MASK)))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->threshold.port = pCommand->port;
    msg_data->threshold.servicePool = pCommand->servicePool;
  }

  if ((pCommand->mask & BVIEW_BST_UMSHARE_MASK) &&
      (BVIEW_BST_IPSP_THRESHOLD_CHECK (pCommand)))
  {
    return  BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->request.i_p_sp_threshold.umShareThreshold =
      pCommand->umShareThreshold;
  }

  return BVIEW_STATUS_SUCCESS;
}

BVIEW_STATUS bst_isp_threshold_input_validate(BSTJSON_CONFIGURE_BST_THRESHOLDS_t *pCommand,
                                                 BVIEW_BST_REQUEST_MSG_t *msg_data)
{
  if (!(pCommand->mask & BVIEW_BST_UMSHARE_MASK))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  if (!(pCommand->mask & BVIEW_BST_SP_MASK))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->threshold.servicePool = pCommand->servicePool;
  }

  if ((pCommand->mask & BVIEW_BST_UMSHARE_MASK) &&
      (BVIEW_BST_ISP_THRESHOLD_CHECK (pCommand)))
  {
    return  BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->request.i_sp_threshold.umShareThreshold =
      pCommand->umShareThreshold;
  }

  return BVIEW_STATUS_SUCCESS;
}


BVIEW_STATUS bst_epsp_threshold_input_validate(BSTJSON_CONFIGURE_BST_THRESHOLDS_t *pCommand,
                                                 BVIEW_BST_REQUEST_MSG_t *msg_data)
{
  if (!((pCommand->mask & BVIEW_BST_UMSHARE_MASK) || 
        (pCommand->mask & BVIEW_BST_UCSHARE_MASK) || 
        (pCommand->mask & BVIEW_BST_MCSHARE_MASK) || 
        (pCommand->mask & BVIEW_BST_MCSHARE_QUEUE_MASK)))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  if (!((pCommand->mask & BVIEW_BST_PORT_MASK)&& 
      (pCommand->mask & BVIEW_BST_SP_MASK)))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->threshold.port = pCommand->port;
    msg_data->threshold.servicePool = pCommand->servicePool;
  }

  if ((pCommand->mask & BVIEW_BST_UCSHARE_MASK) &&
      (BVIEW_BST_EPSP_UC_THRESHOLD_CHECK (pCommand)))
  {
    return  BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->request.ep_sp_threshold.ucShareThreshold =
      pCommand->ucShareThreshold;
  }

  if ((pCommand->mask & BVIEW_BST_UMSHARE_MASK) &&
      (BVIEW_BST_EPSP_UM_THRESHOLD_CHECK (pCommand)))
  {
    return  BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->request.ep_sp_threshold.umShareThreshold =
      pCommand->umShareThreshold;
  }

  if ((pCommand->mask & BVIEW_BST_MCSHARE_MASK) &&
      (BVIEW_BST_EPSP_MC_THRESHOLD_CHECK (pCommand)))
  {
    return  BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->request.ep_sp_threshold.mcShareThreshold =
      pCommand->mcShareThreshold;
  }

  if ((pCommand->mask & BVIEW_BST_MCSHARE_QUEUE_MASK) &&
      (BVIEW_BST_EPSP_MC_SQ_THRESHOLD_CHECK (pCommand)))
  {
    return  BVIEW_STATUS_INVALID_PARAMETER;
  }

  return BVIEW_STATUS_SUCCESS;
}

BVIEW_STATUS bst_esp_threshold_input_validate(BSTJSON_CONFIGURE_BST_THRESHOLDS_t *pCommand,
                                                 BVIEW_BST_REQUEST_MSG_t *msg_data)
{
  if (!((pCommand->mask & BVIEW_BST_UMSHARE_MASK) ||
        (pCommand->mask & BVIEW_BST_MCSHARE_MASK)))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
  if (!(pCommand->mask & BVIEW_BST_SP_MASK))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->threshold.servicePool = pCommand->servicePool;
  }

  if ((pCommand->mask & BVIEW_BST_UMSHARE_MASK) &&
      (BVIEW_BST_E_SP_UM_THRESHOLD_CHECK (pCommand)))
  {
    return  BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->request.e_sp_threshold.umShareThreshold =
      pCommand->umShareThreshold;
  }

  if ((pCommand->mask & BVIEW_BST_MCSHARE_MASK) &&
      (BVIEW_BST_E_SP_MC_THRESHOLD_CHECK (pCommand)))
  {
    return  BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->request.e_sp_threshold.mcShareThreshold =
      pCommand->mcShareThreshold;
  }

  return BVIEW_STATUS_SUCCESS;
}

BVIEW_STATUS bst_euq_threshold_input_validate(BSTJSON_CONFIGURE_BST_THRESHOLDS_t *pCommand,
                                                 BVIEW_BST_REQUEST_MSG_t *msg_data)
{
  if (!(pCommand->mask & BVIEW_BST_UC_MASK))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  if (!(pCommand->mask & BVIEW_BST_QUEUE_MASK))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->threshold.queue = pCommand->queue;
  }
  if ((pCommand->mask & BVIEW_BST_UC_MASK) &&
      (BVIEW_BST_EGRESS_UC_THRESHOLD_CHECK (pCommand)))
  {
    return  BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->request.e_ucq_threshold.ucBufferThreshold =
      pCommand->ucThreshold;
  }

  return BVIEW_STATUS_SUCCESS;
}

BVIEW_STATUS bst_euqg_threshold_input_validate(BSTJSON_CONFIGURE_BST_THRESHOLDS_t *pCommand,
                                                 BVIEW_BST_REQUEST_MSG_t *msg_data)
{
  if (!(pCommand->mask & BVIEW_BST_UC_MASK))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  if (!(pCommand->mask & BVIEW_BST_QUEUE_MASK))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->threshold.queueGroup = pCommand->queueGroup;
  }
  if ((pCommand->mask & BVIEW_BST_UC_MASK) &&
      (BVIEW_BST_EGRESS_UC_GRP_THRESHOLD_CHECK (pCommand)))
  {
    return  BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->request.e_ucqg_threshold.ucBufferThreshold =
      pCommand->ucThreshold;
  }

  return BVIEW_STATUS_SUCCESS;
}

BVIEW_STATUS bst_emc_threshold_input_validate(BSTJSON_CONFIGURE_BST_THRESHOLDS_t *pCommand,
                                                 BVIEW_BST_REQUEST_MSG_t *msg_data)
{
  if (!(pCommand->mask & BVIEW_BST_MC_MASK))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  if (!(pCommand->mask & BVIEW_BST_QUEUE_MASK))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->threshold.queue = pCommand->queue;
  }

  if ((pCommand->mask & BVIEW_BST_MC_MASK) &&
      (BVIEW_BST_E_MC_QG_THRESHOLD_CHECK (pCommand)))
  {
    return  BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->request.e_mcq_threshold.mcBufferThreshold =
      pCommand->mcThreshold;
  }

  if ((pCommand->mask & BVIEW_BST_MCQUEUE_MASK) &&
      (BVIEW_BST_E_MC_SQG_THRESHOLD_CHECK (pCommand)))
  {
    return  BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->request.e_mcq_threshold.mcQueueThreshold =
      pCommand->mcQueueEntriesThreshold;
  }

  return BVIEW_STATUS_SUCCESS;
}

BVIEW_STATUS bst_ecpu_threshold_input_validate(BSTJSON_CONFIGURE_BST_THRESHOLDS_t *pCommand,
                                                 BVIEW_BST_REQUEST_MSG_t *msg_data)
{
  if (!(pCommand->mask & BVIEW_BST_CPU_MASK))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  if (!(pCommand->mask & BVIEW_BST_QUEUE_MASK))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->threshold.queue = pCommand->queue;
  }

  if ((pCommand->mask & BVIEW_BST_CPU_MASK) &&
      (BVIEW_BST_EGRESS_CPU_THRESHOLD_CHECK (pCommand)))
  {
    return  BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->request.cpu_q_threshold.cpuBufferThreshold =
      pCommand->cpuThreshold;
  }

  return BVIEW_STATUS_SUCCESS;
}

BVIEW_STATUS bst_erqe_threshold_input_validate(BSTJSON_CONFIGURE_BST_THRESHOLDS_t *pCommand,
                                                 BVIEW_BST_REQUEST_MSG_t *msg_data)
{
  if (!(pCommand->mask & BVIEW_BST_RQE_MASK))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  if (!(pCommand->mask & BVIEW_BST_QUEUE_MASK))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->threshold.queue = pCommand->queue;
  }
  if ((pCommand->mask & BVIEW_BST_RQE_MASK) &&
      (BVIEW_BST_EGRESS_RQE_QUEUE_THRESHOLD_CHECK (pCommand)))
  {
    return  BVIEW_STATUS_INVALID_PARAMETER;
  }
  else
  {
    msg_data->request.rqe_q_threshold.rqeBufferThreshold =
      pCommand->rqeThreshold;
  }
  return BVIEW_STATUS_SUCCESS;
}

