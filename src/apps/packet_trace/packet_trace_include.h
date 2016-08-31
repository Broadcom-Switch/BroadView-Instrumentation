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

#ifndef INCLUDE_PACKET_TRACE_INCLUDE_H
#define INCLUDE_PACKET_TRACE_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include "modulemgr.h"
#include "packet_trace.h"
#include "configure_packet_trace_feature.h"
#include "get_packet_trace_profile.h"
#include "cancel_packet_trace_profile.h"

#define MSG_QUEUE_ID_TO_PT  0x101

/* Default values for PT configurations */
  /* default  packet trace settings */
#define BVIEW_PT_DEFAULT_ENABLE    true  

#define BVIEW_PT_MAX_UNITS  3


  /* Maximum number of failed Receive messages */
#define BVIEW_PT_MAX_QUEUE_SEND_FAILS      10
  /* packet trace command enums */
  typedef enum _pt_cmd_ {
    /* Set group */
    BVIEW_PT_CMD_API_SET_FEATURE = 1,
    BVIEW_PT_CMD_API_CANCEL_TRACE_PROFILE,
    BVIEW_PT_CMD_API_CANCEL_LAG_RESOLUTION,
    BVIEW_PT_CMD_API_CANCEL_ECMP_RESOLUTION,
    /* get group */
    BVIEW_PT_CMD_API_GET_FEATURE,
    BVIEW_PT_CMD_API_GET_TRACE_PROFILE,
    BVIEW_PT_CMD_API_GET_LAG_RESOLUTION,
    BVIEW_PT_CMD_API_GET_ECMP_RESOLUTION,
    BVIEW_PT_CMD_API_MATCH_PKT_RCVD,
    BVIEW_PT_CMD_API_MAX
  }BVIEW_FEATURE_PT_CMD_API_t;


  typedef struct _pt_trace_pkt_profile_s
  {
    BVIEW_PT_PACKET_t packet;
  }PT_PKT_PROFILE_t;

  typedef struct _pt_trace_5_tuple_s
  {
    PT_5_TUPLE_PARAMS_t tpl;
    int rcvd_pkts;
    int pkt_limit;
  }PT_5_TUPLE_t;
  typedef struct _pt_trace_profile_s
  {
    bool in_use;
    int id;
    int collection_interval;
    BVIEW_PORT_MASK_t port_list;
    bool drop_packet;
    long type;
    unsigned int req_method;
    union
    {
      PT_PKT_PROFILE_t pcap;
      PT_5_TUPLE_t tuple;
    }prfl;
  }PT_PROFILE_t;

  typedef struct _pt_cfg_data_s
  {
    PTJSON_CONFIGURE_PT_FEATURE_t feature;

    /* structure to hold the requested contect info for the trace profile */
    PT_PROFILE_t profile_list[BVIEW_PT_MAX_PROFILE_REQUESTS];
  }BVIEW_PT_CFG_t;


  typedef struct _pt_profile_record_s
  {
    int id;
    BVIEW_TIME_t tv;
    BVIEW_TIME_t cpu_tv;
    BVIEW_TIME_t ingress_tv;
    BVIEW_TIME_t egress_tv;
    PT_PACKET_PARAMS_t rcvd_pkt;
    BVIEW_PORT_MASK_t port_list;
    BVIEW_PT_TRACE_PROFILE_t profile[BVIEW_ASIC_MAX_PORTS];
  } BVIEW_PT_PROFILE_RECORD_t;


 typedef struct _pt_timer_context_s
 {
   int unit;
   int index;
   long msg_type;
   int req_method;
 }BVIEW_PT_TIMER_CONTEXT_t; 

  typedef struct _pt_timer_s_ {
    bool in_use;
    BVIEW_PT_TIMER_CONTEXT_t context;
    int interval;
    timer_t pt_collection_timer;
  }BVIEW_PT_TIMER_t;



  typedef struct _pt_info_s
  {
    BVIEW_PT_TIMER_t pt_timer[BVIEW_PT_MAX_PROFILE_REQUESTS];
    BVIEW_PT_PROFILE_RECORD_t *active;
    BVIEW_PT_PROFILE_RECORD_t *current;
  }BVIEW_PT_INFO_t;

  /* request message to the packet trace */
  typedef struct  _pt_request_msg_s_
  {
    long msg_type;
    int unit;
    void *cookie;
    int id;
    bool report_type_periodic;
    union
    {
      PTJSON_CONFIGURE_PT_FEATURE_t feature;
      PTJSON_GET_PT_PROFILE_t profile;
      PTJSON_CANCEL_PT_PROFILE_t cancel_profile;
    }cmd;
  }BVIEW_PT_REQUEST_MSG_t;

#define BVIEW_PT_MAX_QUEUE_SIZE  (sizeof(BVIEW_PT_REQUEST_MSG_t));


  /* reporting options */
  typedef struct _pt_reporting_options_
  {
      bool report_lag_ecmp;
      bool report_lag;
      bool ig_tv_present;
      bool eg_tv_present;
      PT_PROFILE_INPUT_t req_method;
  } PTJSON_REPORT_OPTIONS_t;


  /* response message to the packet trace */
  typedef struct  _pt_response_msg_s_
  {
    long msg_type;
    int unit;
    void *cookie;
    int id;
    BVIEW_STATUS rv;
    PTJSON_REPORT_OPTIONS_t options;
    union
    {
      PTJSON_CONFIGURE_PT_FEATURE_t *feature;
      BVIEW_PT_PROFILE_RECORD_t *profile;
    }response;
  }BVIEW_PT_RESPONSE_MSG_t;



  typedef struct _pt_context_unit_info_
  {
    /* config data */
    BVIEW_PT_CFG_t *pt_config_data;

    /* oper data */
    BVIEW_PT_INFO_t *pt_oper_data;

    BVIEW_PT_PROFILE_RECORD_t *current_profile;
    BVIEW_PT_PROFILE_RECORD_t *active_profile;
    /* lock for this unit */
    pthread_mutex_t pt_mutex;

  } BVIEW_PT_UNIT_CXT_t;



  typedef struct _pt_context_info_
  {
    BVIEW_PT_UNIT_CXT_t unit[BVIEW_PT_MAX_UNITS];
    /* PT Key to Queue Message*/
    key_t key1;
    /* message queue id for pt */
    int recvMsgQid;
    /* pthread ID*/
    pthread_t pt_thread;

    /* PT packet multiplexer hook */
    pktMuxRegister_t pt_pkt_mux_hook;
  } BVIEW_PT_CXT_t;



  typedef BVIEW_STATUS(*BVIEW_PT_API_HANDLER_t) (BVIEW_PT_REQUEST_MSG_t * msg_data);


  /** Definition of an Packet trace API */
  typedef struct _feature_pt_api_
  {
    /** command */
    int pt_command;
    /** Handler associated with the corresponding packet trace command */
    BVIEW_PT_API_HANDLER_t    handler;
  }BVIEW_PT_API_t;





#define PT_CONFIG_PTR_GET(_id_)  pt_info.unit[_id_].pt_config_data
#define PT_OPER_PTR_GET(_id_)    pt_info.unit[_id_].pt_oper_data
#define PT_UNIT_PTR_GET(_id_)    &pt_info.unit[_id_]

  /* Macro to acquire lock */
#define PT_LOCK_TAKE(_unit)                                                        \
  {                                                                           \
    BVIEW_PT_UNIT_CXT_t *_ptr;                                              \
    _ptr = PT_UNIT_PTR_GET (_unit);                                         \
    if (0 != pthread_mutex_lock (&_ptr->pt_mutex))                          \
    {                                                                        \
      LOG_POST (BVIEW_LOG_ERROR,                                            \
          "Failed to take the lock for unit %d.\r\n",_unit);                \
      return BVIEW_STATUS_FAILURE;                                          \
    }                                                                        \
  }
  /* Macro to release lock*/
#define PT_LOCK_GIVE(_unit)                                                        \
  {                                                                          \
    BVIEW_PT_UNIT_CXT_t *_ptr;                                              \
    _ptr = PT_UNIT_PTR_GET (_unit);                                         \
    if (0 != pthread_mutex_unlock(&_ptr->pt_mutex))                         \
    {                                                                        \
      LOG_POST (BVIEW_LOG_ERROR,                                            \
          "Failed to Release the lock for unit %d.\r\n",_unit);                 \
      return BVIEW_STATUS_FAILURE;                                         \
    }                                                                       \
  }


/* prototypes */


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
BVIEW_STATUS pt_send_response (BVIEW_PT_RESPONSE_MSG_t * reply_data);


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
                                BVIEW_PT_RESPONSE_MSG_t * reply_data);


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
BVIEW_STATUS pt_periodic_collection_cb (union sigval sigval);

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
BVIEW_STATUS pt_send_request (BVIEW_PT_REQUEST_MSG_t * msg_data);
/*********************************************************************
* @brief : function to collect trace profile for 5 tuple 
*
* @param[in] msg_data : pointer to the pt message request.
*
* @retval  : BVIEW_STATUS_SUCCESS : successfully collected the trace profile
* @retval  : BVIEW_STATUS_FAILURE : The trace profile collection has failed.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note : 
*
*********************************************************************/
BVIEW_STATUS pt_trace_pkt_rcv_process(BVIEW_PT_REQUEST_MSG_t * msg_data);

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
BVIEW_STATUS pt_pkt_mux_cb(BVIEW_PACKET_MSG_t *pkt_info_ptr);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_PACKET_TRACE_INCLUDE_H */

