/************************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename bhd_include.h 
  *
  * @purpose BroadView BHD Application internal definitions 
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

#ifndef INCLUDE_BHD_INCLUDE_H
#define INCLUDE_BHD_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include "modulemgr.h"
#include "bhd.h"
#include "configure_black_hole.h"
#include "get_sflow_sampling_status.h"
#include "get_black_hole.h"
#include "system_utils_pcap.h"
#include "system_utils_base64.h"
#include "packet_multiplexer.h"

#define     MSG_QUEUE_ID_TO_BHD  0x401

  /* Default values for BHD configurations */
  /* default  BHD settings       */
#define     BVIEW_BHD_DEFAULT_AGENT_SAMPLE_PERIODICITY    10
#define     BVIEW_BHD_DEFAULT_AGENT_SAMPLE_COUNT          2

  /* sFlow default sample pool size Represents the packet pool size for sampling. 
     One packet is sampled from each pool of packets.       */
#define     BVIEW_BHD_DEFAULT_ENABLE                      false  
#define     BVIEW_BHD_DEFAULT_SFLOW_SAMPLE_POOL_SIZE      1024
  /* Maximum number of failed Receive messages */
#define     BVIEW_BHD_MAX_QUEUE_SEND_FAILS                10
  /* MAX PACKET SIZES  */
#define     BVIEW_BHD_MAX_PCAP_PKT_SIZE                   (BVIEW_BHD_MAX_PKT_SIZE + SYSTEM_PCAP_ADD_LEN + 4)
#define     BHD_JSON_MAX_PKT_LEN                          SYSTEM_BASE64_ENCODE_LEN(BVIEW_BHD_MAX_PCAP_PKT_SIZE)

  typedef  BHDJSON_CONFIGURE_BLACK_HOLE_t           BHD_BLACK_HOLE_CONFIG_PARAMS_t;
  typedef  BHDJSON_GET_BLACK_HOLE_t                 BHD_GET_BLACK_HOLE_t;
  typedef  BHDJSON_GET_SFLOW_SAMPLING_STATUS_t      BHD_GET_SFLOW_SAMPLING_STATUS_t;

  /* BHD command enums */
  typedef enum _bhd_cmd_ {
    /* Set Group */
    BVIEW_BHD_CMD_API_BLACK_HOLE_DETECTION_ENABLE = 1,
    BVIEW_BHD_CMD_API_CONFIGURE_BLACK_HOLE,
    BVIEW_BHD_CMD_API_CANCEL_BLACK_HOLE,
    /* get group */
    BVIEW_BHD_CMD_API_GET_BLACK_HOLE_DETECTION_ENABLE,
    BVIEW_BHD_CMD_API_GET_BLACK_HOLE,
    BVIEW_BHD_CMD_API_GET_BLACK_HOLE_EVENT_REPORT,
    BVIEW_BHD_CMD_API_GET_SFLOW_SAMPLING_STATUS,
    BVIEW_BHD_CMD_API_EVENT_TIME_PERIOD_START,
    BVIEW_BHD_CMD_API_MAX
  }BVIEW_FEATURE_BHD_CMD_API_t;

  /* Black hole event report with JSON and PCAP formatted pkt */
  typedef struct _bhd_black_hole_event_report_
  {
    /* Ingress port  */ 
    unsigned int      ing_port;
    /* Bitmap of egress ports */ 
    BVIEW_PORT_MASK_t egr_pbmp; 

    /* Sampled packet data */
    unsigned char     json_pkt[BHD_JSON_MAX_PKT_LEN];

    /* Total number of Black Holed packets */ 
    uint64_t          total_black_holed_pkts;
    /* Time stamp */ 
    BVIEW_TIME_t      report_time;

  } BHD_BLACK_HOLE_EVENT_REPORT_t;

  /* BHD timer context- is used to pass it to timer call back */ 
  typedef struct _bhd_timer_context_s
  {
    int unit;
    long msg_type;
  }BVIEW_BHD_TIMER_CONTEXT_t;

  typedef struct _bhd_timer_s_ {
    bool in_use;
    BVIEW_BHD_TIMER_CONTEXT_t context;
    int interval;
    timer_t bhd_sampling_timer;
  }BVIEW_BHD_TIMER_t;

  typedef struct _bhd_info_s
  {
    /* BHD timer information */
    BVIEW_BHD_TIMER_t bhd_timer;

    /* BHD packet multiplexer hook */
    pktMuxRegister_t bhd_pkt_mux_hook;

    /* Sampling capabilties */
    int sampling_capabilities;

    /* Represents whether packet needs to be considered for sampling or not */                 
    bool  start_sampling;

    /* Information of the sampled packets */ 
    BVIEW_BHD_SAMPLED_PKT_INFO_t *current_sample_pkt_info;
    BHD_BLACK_HOLE_EVENT_REPORT_t *latest_bhd_report;
    BVIEW_BHD_SFLOW_SAMPLING_STATUS_t  *sflow_sampling_status;
    /* Sample count in the current period */ 
    int current_sample_count; 

    /* Total number of packets sampled after the black hole is enabled */
    int total_sample_count;

  }BVIEW_BHD_INFO_t;

  /* request message to the BHD */
  typedef struct  _bhd_request_msg_s_
  {
    long msg_type;
    int unit;
    void *cookie;
    int id;
    union
    {

      BVIEW_CONFIGURE_BHD_FEATURE_t                feature;
      BVIEW_BLACK_HOLE_CONFIG_t                    black_hole_config_params;
      BHD_GET_SFLOW_SAMPLING_STATUS_t              bhd_sflow_sampling_req;
      BVIEW_BHD_TIMER_CONTEXT_t                    timer_trigger_req;
      BVIEW_BHD_SAMPLED_PKT_INFO_t                 sample_packet;
    }cmd;

  }BVIEW_BHD_REQUEST_MSG_t;

#define BVIEW_BHD_MAX_QUEUE_SIZE  (sizeof(BVIEW_BHD_REQUEST_MSG_t));


  /* response message */
  typedef struct  _bhd_response_msg_s_
  {
    long msg_type;
    int unit;
    void *cookie;
    int id;
    int version;
    BVIEW_STATUS rv;
    union
    {
      BVIEW_CONFIGURE_BHD_FEATURE_t                *feature;
      BVIEW_BLACK_HOLE_CONFIG_t                    *black_hole_config_status;
      BVIEW_BHD_SFLOW_SAMPLING_STATUS_t            *bhd_sflow_sampling;
      BHD_BLACK_HOLE_EVENT_REPORT_t                *bhd_event_report;   
    }response;

  }BVIEW_BHD_RESPONSE_MSG_t;



  typedef struct _bhd_context_unit_info_
  {
    /* config data */
    BVIEW_BHD_CONFIG_t *bhd_config_data;

    /* oper data */
    BVIEW_BHD_INFO_t *bhd_oper_data;


    /* lock for this unit */
    pthread_mutex_t bhd_mutex;

  } BVIEW_BHD_UNIT_CXT_t;



  typedef struct _bhd_context_info_
  {
    BVIEW_BHD_UNIT_CXT_t unit[BVIEW_BHD_MAX_UNITS];

    /* BHD Key to Queue Message*/
    key_t key1;

    /* message queue id for BHD */
    int recvMsgQid;

    /* pthread ID*/
    pthread_t bhd_thread;

    /* pthread ID TESTING TBD*/
    pthread_t bhd_test_thread;
  } BVIEW_BHD_CXT_t;



  typedef BVIEW_STATUS(*BVIEW_BHD_API_HANDLER_t) (BVIEW_BHD_REQUEST_MSG_t * msg_data);


  /** Definition of an BHD API */
  typedef struct _feature_bhd_api_
  {
    /** command */
    int bhd_command;
    /** Handler associated with the corresponding bhd command */
    BVIEW_BHD_API_HANDLER_t    handler;
  }BVIEW_BHD_API_t;



#define BHD_CONFIG_PTR_GET(_id_)  bhd_info.unit[_id_].bhd_config_data
#define BHD_OPER_PTR_GET(_id_)    bhd_info.unit[_id_].bhd_oper_data
#define BHD_UNIT_PTR_GET(_id_)    &bhd_info.unit[_id_]

  /* Macro to acquire lock */
#define BHD_LOCK_TAKE(_unit)                                                        \
  {                                                                           \
    BVIEW_BHD_UNIT_CXT_t *_ptr;                                              \
    _ptr = BHD_UNIT_PTR_GET (_unit);                                         \
    if (0 != pthread_mutex_lock (&_ptr->bhd_mutex))                          \
    {                                                                        \
      LOG_POST (BVIEW_LOG_ERROR,                                            \
          "Failed to take the lock for unit %d.\r\n",_unit);                \
      return BVIEW_STATUS_FAILURE;                                          \
    }                                                                        \
  }
  /* Macro to release lock*/
#define BHD_LOCK_GIVE(_unit)                                                        \
  {                                                                          \
    BVIEW_BHD_UNIT_CXT_t *_ptr;                                              \
    _ptr = BHD_UNIT_PTR_GET (_unit);                                         \
    if (0 != pthread_mutex_unlock(&_ptr->bhd_mutex))                         \
    {                                                                        \
      LOG_POST (BVIEW_LOG_ERROR,                                            \
          "Failed to Release the lock for unit %d.\r\n",_unit);                 \
      return BVIEW_STATUS_FAILURE;                                         \
    }                                                                       \
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
   * @note   : This function is invoked by the BHD to send the black hole 
   *           event report and requested get configure params. This function 
   *           internally calls the encoding api to encode the data, and the memory
   *           for the data is allocated. In case of both successful and
   *           unsuccessful send of the data, the memory must be freed.
   *
   *********************************************************************/
  BVIEW_STATUS bhd_send_response (BVIEW_BHD_RESPONSE_MSG_t * reply_data);


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
      BVIEW_BHD_RESPONSE_MSG_t * reply_data);


  /*********************************************************************
   *  @brief:  callback function to send periodic event reports
   *
   * @param[in]   Data passed with notification after timer expires
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
  BVIEW_STATUS bhd_periodic_collection_cb ( );

  /*********************************************************************
   * @brief   :  function to post message to the bhd application
   *
   * @param[in]  msg_data : pointer to the message request
   *
   * @retval  : BVIEW_STATUS_SUCCESS : if the message is successfully posted to BHD queue.
   * @retval  : BVIEW_STATUS_FAILURE : if the message is failed to send to bhd
   * @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameters to function.
   *
   * @note  : all the apis and call back functions should use this api
   *          to post the message to BHD application.
   *
   *********************************************************************/
  BVIEW_STATUS bhd_send_request (BVIEW_BHD_REQUEST_MSG_t * msg_data);

  BVIEW_STATUS bhd_feature_set(BVIEW_BHD_REQUEST_MSG_t * msg_data);
  BVIEW_STATUS bhd_configure_black_hole(BVIEW_BHD_REQUEST_MSG_t * msg_data);
  BVIEW_STATUS bhd_cancel_black_hole(BVIEW_BHD_REQUEST_MSG_t * msg_data);
  BVIEW_STATUS bhd_feature_get(BVIEW_BHD_REQUEST_MSG_t * msg_data);
  BVIEW_STATUS bhd_get_black_hole(BVIEW_BHD_REQUEST_MSG_t * msg_data);
  BVIEW_STATUS bhd_process_black_hole_event(BVIEW_BHD_REQUEST_MSG_t * msg_data);
  BVIEW_STATUS bhd_sflow_sample_status_get(BVIEW_BHD_REQUEST_MSG_t * msg_data);
  BVIEW_STATUS bhd_start_time_period(BVIEW_BHD_REQUEST_MSG_t * msg_data);

  /*********************************************************************
   * @brief :  function to register with module mgr
   *
   * @param[in] : none 
   * 
   * @retval  : BVIEW_STATUS_SUCCESS : registration of BHD with module manager is successful.
   * @retval  : BVIEW_STATUS_FAILURE : BHD failed to register with module manager.
   *
   * @note : BHD need to register with module manager for the below purpose.
   *         When the REST API is invoked, rest queries the module manager for
   *         the suitable function api  for the corresponding request. Once the
   *         api is retieved , posts the request using the retrieved api.
   *
   * @end
   *********************************************************************/
  BVIEW_STATUS bhd_module_register ();

  /*********************************************************************
   * @brief : Deletes the timer node for the given unit
   *
   * @param[in] unit : unit id for which  the timer needs to be deleted.
   *
   * @retval  : BVIEW_STATUS_INVALID_PARAMETER -- input parameters are invalid. 
   * @retval  : BVIEW_STATUS_FAILURE -- timer is successfully deleted 
   * @retval  : BVIEW_STATUS_SUCCESS -- failed to delete the timer 
   *
   * @note  : The periodic timer is deleted when send asyncronous reporting
   *          is turned off. This timer is per unit.
   *
   *********************************************************************/
  BVIEW_STATUS bhd_periodic_sampling_timer_delete (int unit);

  /*********************************************************************
   * @brief : function to add timer for the periodic profile collection 
   *
   * @param[in] unit : unit for which the periodic profile need to be collected.
   * @param[in] index : Index of the table which maps to the id.
   *
   * @retval  : BVIEW_STATUS_INVALID_PARAMETER -- input parameters are invalid. 
   * @retval  : BVIEW_STATUS_FAILURE -- failed to add the timer 
   * @retval  : BVIEW_STATUS_SUCCESS -- timer is successfully added 
   *
   * @note : this api adds the timer to the linux timer thread, so when the timer 
   *         expires, we receive the callback and post message to the BHD application
   *         to collect the profile. this is a periodic timer , whose interval
   *         is equal to the collection interval. Note that collection is per
   *         unit and per request.
   *
   *********************************************************************/
  BVIEW_STATUS bhd_periodic_sampling_timer_add (int  unit, 
      long msg_type);

  /*********************************************************************
   *  @brief:  callback function to send periodic sampling info  
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
  BVIEW_STATUS bhd_periodic_sampling_cb(union sigval sigval);


  /*********************************************************************
   * @brief   BHD packet multiplexer callback function
   *
   * @param    pkt_info_ptr @b{(input)} Pointer to packet info
   *
   * @returns  BVIEW_STATUS_SUCCESS    if packet data is valid and posted
   *                                   pkt info to BHD app
   * @returns  BVIEW_STATUS_FAILURE    if packet data is invalid and failed
   *                                   to post to 
   *                                  
   *
   * @notes    This function sends the packet and its info to BHD application 
   *                   through message queue
   *
   * @end
   *********************************************************************/
  BVIEW_STATUS bhd_pkt_mux_cb(BVIEW_PACKET_MSG_t *pkt_info_ptr);
#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_BHD_INCLUDE_H */

