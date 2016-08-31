/************************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename bhd_nb_api.c 
  *
  * @purpose BroadView BHD JSON north bound API functions 
  *                    to post the request message to BHD application 
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


#include "json.h"
#include "bhd_include.h"
#include "bhd_debug.h"
#include "cancel_black_hole.h"
#include "configure_black_hole_detection.h"
#include "openapps_log_api.h"
#include "get_black_hole_detection.h"

/*********************************************************************
* @brief : REST API handler to clear the bhd black hole 
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to bhd queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to bhd.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note : This api posts the request to bhd application to clear black hole. 
*
*********************************************************************/
BVIEW_STATUS bhdjson_cancel_black_hole_impl (void *cookie, int asicId,
                                                int id,
                                                BHDJSON_CANCEL_BLACK_HOLE_t *
                                                pCommand)
{
  BVIEW_BHD_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  /* copy the params from the request */

  memset (&msg_data, 0, sizeof (BVIEW_BHD_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.id = id;
  msg_data.msg_type = BVIEW_BHD_CMD_API_CANCEL_BLACK_HOLE;
  /* send message to bhd application */
  rv = bhd_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
   _BHD_LOG(_BHD_DEBUG_ERROR, "%s: failed to post cancel_black_hole request to bhd queue. err = %d.\r\n", __func__, rv);
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post cancel_black_hole request to bhd queue. err = %d.\r\n",rv);
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
BVIEW_STATUS bhdjson_configure_black_hole_detection_impl (void *cookie, int asicId,
                                                 int id,
                                                 BHDJSON_CONFIGURE_BLACK_HOLE_DETECTION_t *pCommand)
{
  BVIEW_BHD_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  if (NULL == pCommand)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }


  memset (&msg_data, 0, sizeof (BVIEW_BHD_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.msg_type = BVIEW_BHD_CMD_API_BLACK_HOLE_DETECTION_ENABLE;
  msg_data.id = id;
  msg_data.cmd.feature = *pCommand;


  /* send message to bhd application */
  rv = bhd_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
   _BHD_LOG(_BHD_DEBUG_ERROR, "%s: failed to post configure_black_hole_detection request to bhd queue. err = %d.\r\n", __func__, rv);
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post configure_black_hole_detection request to bhd queue. err = %d.\r\n",rv);
  }

  return rv;
}


/*********************************************************************
* @brief : REST API handler to configure the black hole params
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to bhd queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to bhd.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note    : This api posts the request to bhd application to configures bhd feature params.
*
* @end
*********************************************************************/
BVIEW_STATUS bhdjson_configure_black_hole_impl (void *cookie, int asicId,
                                                 int id,
                                                 BHDJSON_CONFIGURE_BLACK_HOLE_t
                                                 * pCommand)
{
  BVIEW_BHD_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  if (NULL == pCommand)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }


  memset (&msg_data, 0, sizeof (BVIEW_BHD_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.msg_type = BVIEW_BHD_CMD_API_CONFIGURE_BLACK_HOLE;
  msg_data.id = id;
  msg_data.cmd.black_hole_config_params = *pCommand;


  /* send message to bhd application */
  rv = bhd_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
   _BHD_LOG(_BHD_DEBUG_ERROR, "%s: failed to post configure_black_hole request to bhd queue. err = %d.\r\n", __func__, rv);
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post configure_black_hole request to bhd queue. err = %d.\r\n",rv);
  }

  return rv;
}


/*********************************************************************
* @brief : REST API handler to get the black hole params
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to bhd queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to bhd.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note    : This api posts the request to bhd application to get bhd black hole params.
*
* @end
*********************************************************************/
BVIEW_STATUS bhdjson_get_black_hole_impl (void *cookie, int asicId,
                                                 int id,
                                                 BHDJSON_GET_BLACK_HOLE_t
                                                 * pCommand)
{
  BVIEW_BHD_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  if (NULL == pCommand)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }


  memset (&msg_data, 0, sizeof (BVIEW_BHD_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.msg_type = BVIEW_BHD_CMD_API_GET_BLACK_HOLE;
  msg_data.id = id;


  /* send message to bhd application */
  rv = bhd_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
   _BHD_LOG(_BHD_DEBUG_ERROR, "%s: failed to post get_black_hole request to bhd queue. err = %d.\r\n", __func__, rv);
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post get_black_hole request to bhd queue. err = %d.\r\n",rv);
  }

  return rv;
}


/*********************************************************************
* @brief : REST API handler to get the black hole feature params
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to bhd queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to bhd.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note    : This api posts the request to bhd application to get bhd black hole feature params.
*
* @end
*********************************************************************/
BVIEW_STATUS bhdjson_get_black_hole_detection_impl (void *cookie, int asicId,
                                                 int id,
                                                 BHDJSON_GET_BLACK_HOLE_DETECTION_t
                                                 * pCommand)
{
  BVIEW_BHD_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  if (NULL == pCommand)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }


  memset (&msg_data, 0, sizeof (BVIEW_BHD_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.msg_type = BVIEW_BHD_CMD_API_GET_BLACK_HOLE_DETECTION_ENABLE;
  msg_data.id = id;


  /* send message to bhd application */
  rv = bhd_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
   _BHD_LOG(_BHD_DEBUG_ERROR, "%s: failed to post black_hole_detection request to bhd queue. err = %d.\r\n", __func__, rv);
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post black_hole_detection request to bhd queue. err = %d.\r\n",rv);
  }

  return rv;
}


/*********************************************************************
* @brief : REST API handler to get the sampling status 
*
* @param[in] cookie : pointer to the cookie
* @param[in] asicId : asic id 
* @param[in] id     : unit id
* @param[in] pCommand : pointer to the input command structure
*
* @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to bhd queue.
* @retval  : BVIEW_STATUS_FAILURE : failed to post the message to bhd.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note    : This api posts the request to bhd application to get sampling status.
*
* @end
*********************************************************************/
BVIEW_STATUS bhdjson_get_sflow_sampling_status_impl (void *cookie, int asicId,
                                                 int id,
                                                 BHDJSON_GET_SFLOW_SAMPLING_STATUS_t
                                                 * pCommand)
{
  BVIEW_BHD_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;

  if (NULL == pCommand)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }


  memset (&msg_data, 0, sizeof (BVIEW_BHD_REQUEST_MSG_t));
  msg_data.unit = asicId;
  msg_data.cookie = cookie;
  msg_data.msg_type = BVIEW_BHD_CMD_API_GET_SFLOW_SAMPLING_STATUS;
  msg_data.id = id;
  msg_data.cmd.bhd_sflow_sampling_req = *pCommand;


  /* send message to bhd application */
  rv = bhd_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
   _BHD_LOG(_BHD_DEBUG_ERROR, "%s: failed to post get_sflow_sampling_status request to bhd queue. err = %d.\r\n", __func__, rv);
    LOG_POST (BVIEW_LOG_ERROR,
        "failed to post get_sflow_sampling_status request to bhd queue. err = %d.\r\n",rv);
  }

  return rv;
}


