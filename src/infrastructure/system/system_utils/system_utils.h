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

#ifndef INCLUDE_SYSTEM_UTILS_H
#define INCLUDE_SYSTEM_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include "modulemgr.h"
#include "system_utils_api.h"
#include "configure_reg_hb_feature.h"
#include "cancel_request.h"


#define MSG_QUEUE_ID_TO_SYSTEM_UTILS  0x300

#define _SYSTEM_UTILS_DEBUG
#define _SYSTEM_UTILS_DEBUG_LEVEL        0x00

#define _SYSTEM_UTILS_DEBUG_TRACE        (0x1)
#define _SYSTEM_UTILS_DEBUG_INFO         (0x01 << 1)
#define _SYSTEM_UTILS_DEBUG_ERROR        (0x01 << 2)
#define _SYSTEM_UTILS_DEBUG_ALL          (0xFF)

#ifdef _SYSTEM_UTILS_DEBUG
#define _SYSTEM_UTILS_LOG(level, format,args...)   do { \
                if ((level) & _SYSTEM_UTILS_DEBUG_LEVEL) { \
                                                  printf(format, ##args); \
                                              } \
            }while(0)
#else
#define _SYSTEM_UTILS_LOG(level, format,args...)
#endif



  /* Maximum number of failed Receive messages */
#define BVIEW_SYSTEM_UTILS_MAX_QUEUE_SEND_FAILS      10

#define BVIEW_IPADDR_LEN_MAX 20
#define BVIEW_UID_LEN_MAX 128
#define BVIEW_VERSION_LEN_MAX 32


typedef REG_HB_JSON_CONFIGURE_FEATURE_t BVIEW_SYSTEM_UTILS_CONFIG_PARAMS_t; 
typedef REG_HB_JSON_CANCEL_REQUEST_t BVIEW_SYSTEM_UTILS_CANCEL_REQUEST_t; 

  typedef enum _system_utils_report_type_ {
    BVIEW_SYSTEM_UTILS_NON_PERIODIC = 1,
    BVIEW_SYSTEM_UTILS_PERIODIC
  }BVIEW_SYSTEM_UTILS_REPORT_TYPE_t;

  typedef enum _system_utils_cmd_ {
    /* Set group */
    BVIEW_SYSTEM_UTILS_CMD_API_SET_FEATURE = 1,
    /* get group */
    BVIEW_SYSTEM_UTILS_CMD_API_GET_FEATURE,
    BVIEW_SYSTEM_UTILS_CMD_API_GET_SWITCH_PROPERTIES,
    BVIEW_SYSTEM_UTILS_CMD_API_CANCEL_REQUEST
  }BVIEW_FEATURE_SYSTEM_UTILS_CMD_API_t;

#define BVIEW_SYSTEM_UTILS_CMD_API_MAX BVIEW_SYSTEM_UTILS_CMD_API_GET_SWITCH_PROPERTIES

  /* Structure for switch properties parameters */
  typedef struct _switch_properties_
  {
    time_t timeStamp;
    uint8_t networkOs[BVIEW_NETWORK_OS_LEN_MAX];
    int     numAsics;
    int     featureMask;
    BVIEW_SWITCH_ASIC_INFO_t   asicInfo[BVIEW_MAX_ASICS_ON_A_PLATFORM];
    uint8_t uid[BVIEW_UID_LEN_MAX];
    uint8_t agent_ipaddr[BVIEW_IPADDR_LEN_MAX];
    int     agent_port;
    uint8_t agent_sw_version[BVIEW_VERSION_LEN_MAX];
  } BVIEW_SWITCH_PROPERTIES_t;


  typedef struct _system_utils_request_msg_ {
    long msg_type; /* message type */
    int unit; /* variable to hold the asic type */
    void *cookie;
    int id; /* id passed from the request */
    int version; /* json version */
    BVIEW_SYSTEM_UTILS_REPORT_TYPE_t report_type;
    union
    {
      /* feature params */
      BVIEW_SYSTEM_UTILS_CONFIG_PARAMS_t config;
      BVIEW_SYSTEM_UTILS_CANCEL_REQUEST_t cancel;
    }request;
  }BVIEW_SYSTEM_UTILS_REQUEST_MSG_t;


  typedef struct _system_utils_response_msg_ {
    long msg_type;
    int unit;
    void *cookie;
    int id;
    BVIEW_SYSTEM_UTILS_REPORT_TYPE_t report_type;
    BVIEW_SWITCH_PROPERTIES_t  *switchProperties;
    BVIEW_STATUS rv; /* return value for set request */
    union
    {
      BVIEW_SYSTEM_UTILS_CONFIG_PARAMS_t *config;
    }response;
  }BVIEW_SYSTEM_UTILS_RESPONSE_MSG_t;

  typedef struct _system_util_timer_s_ {
    unsigned int unit;
    bool in_use;
    timer_t timer;
  }BVIEW_SYSTEM_UTILS_TIMER_t;

  typedef struct _system_util_data_ {
    BVIEW_SYSTEM_UTILS_TIMER_t heart_beat_timer;
    BVIEW_SYSTEM_UTILS_CONFIG_PARAMS_t config;
  } BVIEW_SYSTEM_UTILS_DATA_t;

  /** Definition of an SYSTEM_UTILS API */
  typedef struct _feature_system_utils_cancel_api_
  {
    /* this element is free or not */ 
    bool present;
    /** system_utils command */
    int module;
    /** Handler associated with the corresponding bst command */
    BVIEW_SYSTEM_UTILS_CANCEL_API_HANDLER_t    handler;
  }BVIEW_SYSTEM_UTILS_CANCEL_API_t;


  typedef struct _system_utils_context_info__
  {
    pthread_mutex_t system_utils_mutex;
    /* Switch Properties*/
    BVIEW_SWITCH_PROPERTIES_t  *switchProperties;
    BVIEW_SYSTEM_UTILS_DATA_t *data;
    /*  Key to Queue Message*/
    key_t key1;
    /* message queue id  */
    int recvMsgQid;
    /* Cancel api list of different applications */
    BVIEW_SYSTEM_UTILS_CANCEL_API_t system_utils_cancel_api_list[BVIEW_MAX_FEATURES];
    /* pthread ID*/
    pthread_t system_utils_thread;
  } BVIEW_SYSTEM_UTILS_CXT_t;


  typedef BVIEW_STATUS(*BVIEW_SYSTEM_UTILS_API_HANDLER_t) (BVIEW_SYSTEM_UTILS_REQUEST_MSG_t * msg_data);



  /** Definition of an SYSTEM_UTILS API */
  typedef struct _feature_system_utils_api_
  {
    /** system_utils command */
    int command;
    /** Handler associated with the corresponding bst command */
    BVIEW_SYSTEM_UTILS_API_HANDLER_t    handler;
  }BVIEW_SYSTEM_UTILS_API_t;





  /* Macro to acquire lock */
#define SYSTEM_UTILS_LOCK_TAKE(_ptr)                                                        \
  {                                                                           \
    if (0 != pthread_mutex_lock (_ptr))                          \
    {                                                                        \
      LOG_POST (BVIEW_LOG_ERROR,                                            \
          "Failed to take the lock for system utils \r\n");                \
      return BVIEW_STATUS_FAILURE;                                          \
    }                                                                        \
  }
  /*  to release lock*/
#define SYSTEM_UTILS_LOCK_GIVE(_ptr)                                                        \
  {                                                                          \
    if (0 != pthread_mutex_unlock(_ptr))                         \
    {                                                                        \
      LOG_POST (BVIEW_LOG_ERROR,                                            \
          "Failed to Release the lock for system utils \r\n");                 \
      return BVIEW_STATUS_FAILURE;                                         \
    }                                                                       \
  }


/*********************************************************************
 * @brief :  function to register with module mgr
 *
 * @param[in] : none
 *
 * @retval  : BVIEW_STATUS_SUCCESS : registration of application with module manager is successful.
 * @retval  : BVIEW_STATUS_FAILURE : Application failed to register with module manager.
 *
 * @note : The utility application need to register with module manager for the below purpose.
 *         When the REST API is invoked, rest queries the module manager for
 *         the suitable function api  for the corresponding request. Once the
 *         api is retieved , posts the request using the retrieved api.
 *
 * @end
 *********************************************************************/
BVIEW_STATUS system_utils_module_register (void);

BVIEW_STATUS system_utils_copy_reply_params (BVIEW_SYSTEM_UTILS_REQUEST_MSG_t * msg_data,
                                BVIEW_SYSTEM_UTILS_RESPONSE_MSG_t * reply_data);

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
  * @note   : This function is invoked by the system_utils to send the report and
  *           requested get configure params. This function internally
  *           calls the encoding api to encode the data, and the memory
  *           for the data is allocated. In case of both successful and
  *           unsuccessful send of the data, the memory must be freed.
  *
  *********************************************************************/
BVIEW_STATUS system_utils_send_response (BVIEW_SYSTEM_UTILS_RESPONSE_MSG_t * reply_data);

/*********************************************************************
  * @brief   :  function to post message to the system utility application
  *
  * @param[in]  msg_data : pointer to the message request
  *
  * @retval  : BVIEW_STATUS_SUCCESS : if the message is successfully posted to queue.
  * @retval  : BVIEW_STATUS_FAILURE : if the message is failed to send to application 
  * @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameters to function.
  *
  * @note  : all the apis and call back functions should use this api
  *          to post the message to application.
  *
  *********************************************************************/
BVIEW_STATUS system_utils_send_request (BVIEW_SYSTEM_UTILS_REQUEST_MSG_t * msg_data);
/*********************************************************************
  * @brief : system utilities application main function which does processing of messages
  *
  * @param[in] : none
  *
  * @retval  : BVIEW_STATUS_SUCCESS: Fails to register with module manager
  *             or some error happened due to which the application is
  *             unable to process incoming messages.
  * @retval  : BVIEW_STATUS_FAILURE: Fails to register with module manager
  *
  * @note  : This api is the processing thread of the system util application.
  *          All the incoming requests are processed and the responses
  *          are sent in the application context. Currently the assumption
  *          is made that if the thread fails to read continously 10 or
  *          more messages, then there is some error and the thread exits.
  *
  *********************************************************************/
BVIEW_STATUS system_utils_app_main (void);

/*********************************************************************
 * @brief : function to handle system command cancel request
 *
 * @param[in] BVIEW_SYSTEM_UTILS_REQUEST_MSG_t :  Msg data
 *
 * @retval  BVIEW_STATUS_INVALID_PARAMETER : If the msg data is invalid
 *          BVIEW_STATUS_SUCCESS : if the cancel request is successfull
 * @note :
 *
 *********************************************************************/
BVIEW_STATUS system_utils_switch_cancel_request(BVIEW_SYSTEM_UTILS_REQUEST_MSG_t * msg_data);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SYSTEM_UTILS_H */

