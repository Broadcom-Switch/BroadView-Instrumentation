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
#include <mqueue.h>
#include <errno.h>
#include "configure_reg_hb_feature.h"
#include "get_reg_hb_feature.h"
#include "get_switch_properties.h"
#include "cancel_request.h"
#include "system.h"
#include "system_utils_app.h"
#include "system_utils.h"
#include "reg_hb.h"
#include "rest_api.h"
#include "openapps_log_api.h"
#include "sbplugin_redirect_system.h"
#include "reg_hb_json_encoder.h"
#include "system_utils_json_memory.h"
#include "system_utils_api.h"
#include "version.h"

/* System application Context Info*/
BVIEW_SYSTEM_UTILS_CXT_t system_utils_info;

static BVIEW_REST_API_t system_utils_api_list[] = {

  {"configure-system-feature", reg_hb_json_configure_feature},
  {"get-system-feature", reg_hb_json_get_feature},
  {"get-switch-properties", reg_hb_json_get_switch_properties},
  {"cancel-request", system_utils_json_cancel_request}
};


/*********************************************************************
 * @brief : function to return the api handler for the system command type
 *
 * @param[in] int : request type
 *
 * @retval    : function pointer to the corresponding api
 *
 * @note :
 *
 *********************************************************************/
BVIEW_STATUS system_utils_type_api_get (int type, BVIEW_SYSTEM_UTILS_API_HANDLER_t *handler)
{
  unsigned int i = 0;

  static const BVIEW_SYSTEM_UTILS_API_t system_utils_cmd_api_list[] = {
    {BVIEW_SYSTEM_UTILS_CMD_API_GET_FEATURE, reg_hb_config_feature_get},
    {BVIEW_SYSTEM_UTILS_CMD_API_SET_FEATURE, reg_hb_config_feature_set},
    {BVIEW_SYSTEM_UTILS_CMD_API_GET_SWITCH_PROPERTIES, reg_hb_switch_properties_get},
    {BVIEW_SYSTEM_UTILS_CMD_API_CANCEL_REQUEST, system_utils_switch_cancel_request}
  };

  for (i = 0; i < (sizeof(system_utils_cmd_api_list)/sizeof(BVIEW_SYSTEM_UTILS_API_t)); i++)
  {
    if (type == system_utils_cmd_api_list[i].command)
    {
      *handler = system_utils_cmd_api_list[i].handler;
      return BVIEW_STATUS_SUCCESS;
    }
  }
  return BVIEW_STATUS_INVALID_PARAMETER;
}


/* utility function to convert ip address to string */
char *get_ip_str(const struct sockaddr_in *sa, char *ip_str, int maxlen)
{
      inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr),
          ip_str, (size_t) maxlen);
  return ip_str;
}

/*********************************************************************
* @brief  Get the version of the system
*
* @param[out] buffer                         - buffer
* @param[in]  length                         - length of the buffer
*
* @retval  BVIEW_STATUS_SUCCESS            if version get is success.
* @retval  BVIEW_STATUS_FAILURE            if version get is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes   get the version of the system 
*
*
*********************************************************************/
static BVIEW_STATUS system_version_get (char *buffer, 
                                           int length)
{

  if (buffer == NULL) 
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  if (length < (strlen(RELEASE_STRING)))
    return BVIEW_STATUS_FAILURE;

  strncpy(buffer, RELEASE_STRING, strlen(RELEASE_STRING));
  return BVIEW_STATUS_SUCCESS;
}

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
BVIEW_STATUS system_utils_app_main (void)
{
  BVIEW_SYSTEM_UTILS_REQUEST_MSG_t msg_data;
  BVIEW_SYSTEM_UTILS_RESPONSE_MSG_t reply_data;
  BVIEW_STATUS rv = BVIEW_STATUS_FAILURE;
  unsigned int rcvd_err = 0;
  unsigned int id = 0, num_units = 0;
  BVIEW_SYSTEM_UTILS_API_HANDLER_t handler;
  BVIEW_SWITCH_PROPERTIES_t  *pswitchProp = system_utils_info.switchProperties;
  struct sockaddr_in agent_ip;

  if (BVIEW_STATUS_SUCCESS != system_utils_module_register ())
  {
    /* registration with module mgr has failed.
       return failure. so that the caller can clean the resources */
    LOG_POST (BVIEW_LOG_EMERGENCY,
        "Registration with module mgr failed \r\n");

    return BVIEW_STATUS_FAILURE;
  }

  if (BVIEW_STATUS_SUCCESS != sbapi_system_num_units_get ((int *) &num_units))
  {
    LOG_POST (BVIEW_LOG_ERROR, "Failed to get num of units\r\n");
    return BVIEW_STATUS_FAILURE;
  }

  /* NULLPTR check*/
  if (pswitchProp == NULL)
  {
    return BVIEW_STATUS_FAILURE;
  }

  pswitchProp->numAsics = num_units;

  /* Get Supported feature mask*/
  if (BVIEW_STATUS_SUCCESS !=
      sbapi_system_feature_mask_get (&pswitchProp->featureMask))
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* get the network OS or Plugin*/
  if (BVIEW_STATUS_SUCCESS !=
      sbapi_system_network_os_get (&pswitchProp->networkOs[0],
        BVIEW_NETWORK_OS_LEN_MAX))
  {
    return BVIEW_STATUS_FAILURE;
  }

  for (id = 0; id < num_units; id++)
  {
    /* Get asic notation*/
    if (BVIEW_STATUS_SUCCESS !=
        sbapi_system_asic_translate_to_notation (id,
          pswitchProp->asicInfo[id].asic_notation))
    {
      return BVIEW_STATUS_FAILURE;
    }

    /* Get ASIC type*/
    if (BVIEW_STATUS_SUCCESS !=
        sbapi_system_unit_to_asic_type_get (id,
          &pswitchProp->asicInfo[id].asicType))
    {
      return BVIEW_STATUS_FAILURE;
    }

    /* Get ASIC type*/
    if (BVIEW_STATUS_SUCCESS !=
        sbapi_system_asic_num_ports_get (id,
          &pswitchProp->asicInfo[id].numPorts))
    {
      return BVIEW_STATUS_FAILURE;
    }

  }

  
  /* get the system UID */
  sbapi_system_uid_get(&pswitchProp->uid[0], BVIEW_UID_LEN_MAX);
  sbapi_system_ip4_get((uint8_t *)&agent_ip.sin_addr, sizeof(agent_ip.sin_addr));
  get_ip_str(&agent_ip, (char *)&pswitchProp->agent_ipaddr, BVIEW_IPADDR_LEN_MAX);
  system_agent_port_get (&pswitchProp->agent_port);
  system_version_get ((char *)&pswitchProp->agent_sw_version[0], BVIEW_VERSION_LEN_MAX);

   _SYSTEM_UTILS_LOG(_SYSTEM_UTILS_DEBUG_INFO, "received Uid is %s \n"
                            " agent ip %s \n"
                            "agent port %d \n",
                            &pswitchProp->uid[0], &pswitchProp->agent_ipaddr[0], pswitchProp->agent_port);

   /* start the timer, if the feature is enabled by default */

   if ((true == system_utils_info.data->config.enable) && (0 != system_utils_info.data->config.interval))
   {
     /* start the heart beat timer */
     reg_hb_heartbeat_timer_start();
   }

  while (1)
  {
    if (-1 != (msgrcv (system_utils_info.recvMsgQid, &msg_data, sizeof (msg_data), 0, 0)))
    {
      _SYSTEM_UTILS_LOG(_SYSTEM_UTILS_DEBUG_INFO, "msg_data info\n"
          "msg_data.msg_type = %ld\n"
          "msg_data.unit = %d\n"
          "msg_data.cookie = %d\n",
          msg_data.msg_type, msg_data.unit, (NULL == msg_data.cookie)? true: false);

      if (msg_data.unit >= num_units)
      {
        /* requested id is greater than the number of units.
           send failure message and continue */
        memset (&reply_data, 0, sizeof (BVIEW_SYSTEM_UTILS_RESPONSE_MSG_t));
        reply_data.rv = BVIEW_STATUS_INVALID_PARAMETER;

        /* copy the request type */
        reply_data.msg_type = msg_data.msg_type;
        /* copy the unit */
        reply_data.unit = msg_data.unit;
        /* copy the asic type */
        reply_data.id = msg_data.id;
        /* copy the cookie ..  */
        reply_data.cookie = msg_data.cookie;

        rv = system_utils_send_response(&reply_data);
        if (BVIEW_STATUS_SUCCESS != rv)
        {
          LOG_POST (BVIEW_LOG_ERROR,
              "failed to send response for command %ld , err = %d. \r\n", msg_data.msg_type, rv);
        }
        continue;
      }
      /* Counter to check for read errors..
         successfully read the message. clear this to 0*/
      rcvd_err = 0;
      /* Memset the response message */
      memset (&reply_data, 0, sizeof (BVIEW_SYSTEM_UTILS_RESPONSE_MSG_t));

      /* get the api function for the method type */
      if (BVIEW_STATUS_SUCCESS != system_utils_type_api_get (msg_data.msg_type, &handler))
      {
        continue;
      }
      rv = handler(&msg_data);

      reply_data.rv = rv;

      rv = system_utils_copy_reply_params (&msg_data, &reply_data);
      if (BVIEW_STATUS_SUCCESS != rv)
      {
        _SYSTEM_UTILS_LOG(_SYSTEM_UTILS_DEBUG_ERROR,
            "system_utils_main.c failed to send response for command %ld , err = %d. \r\n", msg_data.msg_type, rv);
        LOG_POST (BVIEW_LOG_ERROR,
            "failed to send response for command %ld , err = %d. \r\n", msg_data.msg_type, rv);
      }
    }
    else
    {
      LOG_POST (BVIEW_LOG_ERROR,
          "Failed to read message from the system utility application queue. err = %d\r\n", errno);
      /* increment the error counter.
         If this counter increments continously exit and return failure
         so that corrective action can be taken */
      rcvd_err++;

      /* wait till 10 consective messages */
      if(BVIEW_SYSTEM_UTILS_MAX_QUEUE_SEND_FAILS < rcvd_err)
      {
        break;
      }
      continue;
    }
  }                             /* while (1) */
  LOG_POST (BVIEW_LOG_EMERGENCY,
      "exiting from system utility pthread ... \r\n");
  pthread_exit(&system_utils_info.system_utils_thread);
  return BVIEW_STATUS_FAILURE;
}


/*********************************************************************
  * @brief : function to create the system utility application thread.
  *
  * @param[in]  none
  *
  * @retval  : BVIEW_STATUS_RESOURCE_NOT_AVAILABLE : Returns when memory is not available
  * @retval  : BVIEW_STATUS_FAILURE : failed to create the message queue or thread.
  * @retval  : BVIEW_STATUS_SUCCESS : successful completion of task creation.
  * @retval  : BVIEW_STATUS_INIT_FAILED : application initialization failed.
  *
  * @note : Main function to invoke the  application thread. This function allocates
  *         the required memory for application.
  *         Creates the mutexes to protect data. Creates the message queue
  *         It assumes that the system is initialized before the application 
  *         thread is invoked. This api depends on the system variables
  *         such as number of units per device. Incase of no such
  *         need, the number of units can be #defined.
  *
  *********************************************************************/
BVIEW_STATUS system_utils_main ()
{
  int rv = BVIEW_STATUS_SUCCESS;
  int recvMsgQid;
  pthread_mutex_t *system_utils_mutex;
  int num_units = 0;

  system_utils_info.key1 = MSG_QUEUE_ID_TO_SYSTEM_UTILS;

  system_utils_mutex = &system_utils_info.system_utils_mutex;
  pthread_mutex_init (system_utils_mutex, NULL);


  /* get the number of units */
  if (BVIEW_STATUS_SUCCESS != sbapi_system_num_units_get ((int *) &num_units))
  {
    /* Free the resources allocated so far */
    system_utils_app_uninit ();

    LOG_POST (BVIEW_LOG_EMERGENCY,
        "Failed to number of units, Unable to start system utility application\r\n");
    return BVIEW_STATUS_RESOURCE_NOT_AVAILABLE;
  }
  /* allocate memory for switch properties*/
  system_utils_info.switchProperties =
    (BVIEW_SWITCH_PROPERTIES_t *) malloc (sizeof (BVIEW_SWITCH_PROPERTIES_t));
  if (system_utils_info.switchProperties == NULL)
  {
    LOG_POST (BVIEW_LOG_EMERGENCY,
        "Failed to allocate memory for system utility application\r\n");
    return BVIEW_STATUS_RESOURCE_NOT_AVAILABLE;
  }

  memset (system_utils_info.switchProperties, 0x00,
      sizeof (BVIEW_SWITCH_PROPERTIES_t));

  /* allocate memory for config and data*/
  system_utils_info.data =
    (BVIEW_SYSTEM_UTILS_DATA_t *) malloc (sizeof (BVIEW_SYSTEM_UTILS_DATA_t));
  if (system_utils_info.data == NULL)
  {
    LOG_POST (BVIEW_LOG_EMERGENCY,
        "Failed to allocate memory for system utility application\r\n");
    return BVIEW_STATUS_RESOURCE_NOT_AVAILABLE;
  }

  memset (system_utils_info.data, 0x00,
      sizeof (BVIEW_SYSTEM_UTILS_DATA_t));

  /* initialize the default values to the config */

  system_utils_info.data->config.enable = BVIEW_REG_HB_DEFAULT_FEATURE_ENABLE;
  system_utils_info.data->config.interval = BVIEW_REG_HB_DEFAULT_HEARTBEAT_INTERVAL;

  /* initialize the JSON memory */
  system_utils_json_memory_init();

  /* Check if message queue is already exitsing */
  if (0 > (recvMsgQid = msgget(system_utils_info.key1, IPC_EXCL | IPC_CREAT | 0777)))
  {
    if (EEXIST == errno)
    {

      /* get the already existing  message queue id for application */
      if ((recvMsgQid = msgget (system_utils_info.key1, IPC_CREAT | 0777)) < 0)
      {
        /* Free the resources allocated so far */

        LOG_POST (BVIEW_LOG_EMERGENCY,
            "Failed to create  msgQ for system utility application. err = %d\n", errno);
        return  BVIEW_STATUS_FAILURE;
      }

      /* message queue exits.. Delete the message queue */
      if  (msgctl(recvMsgQid, IPC_RMID, (struct msqid_ds *)NULL) <0)
      {
        LOG_POST (BVIEW_LOG_EMERGENCY,
            "Failed to destroy system_utils msgQ id, err = %d\n", errno);
      }


      /* create the message queue for application */
      if ((recvMsgQid = msgget (system_utils_info.key1, IPC_CREAT | 0777)) < 0)
      {
        /* Free the resources allocated so far */
        system_utils_app_uninit ();

        LOG_POST (BVIEW_LOG_EMERGENCY,
            "Failed to create  msgQ for system_utils application. err = %d\n", errno);
        return  BVIEW_STATUS_FAILURE;
      }
    }
  }
  system_utils_info.recvMsgQid = recvMsgQid;


  /* create pthread for system_utils application */
  if (0 != pthread_create (&system_utils_info.system_utils_thread, NULL, (void *) &system_utils_app_main, NULL))
  {
    LOG_POST (BVIEW_LOG_EMERGENCY, "SYSTEM_UTILS  thread creation failed %d\r\n", errno);
    return  BVIEW_STATUS_FAILURE;
  }
  LOG_POST (BVIEW_LOG_INFO,
      "system_utils application: system_utils pthread created\r\n");

  return rv;
}


/*********************************************************************
  *  @brief:  function to clean up  system utility application
  *
  * @param[in]  none
  *
  * @retval  : none
  *
  * @note  When in case of any error event, this api cleans up all the resources.
  *         i.e it deletes the timer notes, if any are present.
  *         destroy the mutexes. clears the memory and destroys the message queue.
  *
  *********************************************************************/
void system_utils_app_uninit ()
{
  system_utils_info.key1 = MSG_QUEUE_ID_TO_SYSTEM_UTILS;
  pthread_mutex_t *system_utils_mutex;

  /* free all memeory */
  if (NULL != system_utils_info.switchProperties)
  {
    free (system_utils_info.switchProperties);
  }

  /* 
     delete the timer.
   */
  reg_hb_heartbeat_timer_stop ();

  /* Destroy mutex */
  system_utils_mutex = &system_utils_info.system_utils_mutex;
  pthread_mutex_destroy (system_utils_mutex);

  if (NULL != system_utils_info.data)
  {
    free (system_utils_info.data);
  }

  /* check if the message queue already exists.
     If yes, we should delete the same */
  if (-1 == (system_utils_info.recvMsgQid = msgget(system_utils_info.key1, IPC_EXCL | IPC_CREAT | 0777)))
  {
    if (EEXIST == errno)
    {
      /* delete the message queue  */
      if (msgctl(system_utils_info.recvMsgQid, IPC_RMID, NULL) == -1)
      {
        LOG_POST (BVIEW_LOG_ERROR,
            "Failed to destroy system utility msgQ id, err = %d\n", errno);
      }
    }
  }
}

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
BVIEW_STATUS system_utils_send_request (BVIEW_SYSTEM_UTILS_REQUEST_MSG_t * msg_data)
{
  int rv = BVIEW_STATUS_SUCCESS;
  struct mq_attr obuf; /* output attr struct for getattr */

  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  if (-1 == msgsnd (system_utils_info.recvMsgQid, msg_data, sizeof (BVIEW_SYSTEM_UTILS_REQUEST_MSG_t), IPC_NOWAIT))
  {
    if ( ! mq_getattr(system_utils_info.recvMsgQid,&obuf) )
    {
      _SYSTEM_UTILS_LOG(_SYSTEM_UTILS_DEBUG_TRACE, "flags: %ld  maxmsg: %ld  msgsize: %ld  curmsgs: %ld\n",
          obuf.mq_flags, obuf.mq_maxmsg, obuf.mq_msgsize, obuf.mq_curmsgs);
    }

    LOG_POST (BVIEW_LOG_ERROR,
        "Failed to send message to system utility application,  msg_type  %ld, err = %d\r\n",
        msg_data->msg_type, errno);
    rv = BVIEW_STATUS_FAILURE;
  }

  return rv;
}

BVIEW_STATUS system_utils_copy_reply_params (BVIEW_SYSTEM_UTILS_REQUEST_MSG_t * msg_data,
                                BVIEW_SYSTEM_UTILS_RESPONSE_MSG_t * reply_data)
{

  if ((NULL == msg_data) || (NULL == reply_data))
    return BVIEW_STATUS_INVALID_PARAMETER;

  /* copy the request type */
  reply_data->msg_type = msg_data->msg_type;
  /* copy the unit */
  reply_data->unit = msg_data->unit;
  /* copy the asic type */
  reply_data->id = msg_data->id;
  /* copy the cookie ..  */
  reply_data->cookie = msg_data->cookie;

  switch (msg_data->msg_type)
  {
    case BVIEW_SYSTEM_UTILS_CMD_API_GET_FEATURE:
      reply_data->response.config = &system_utils_info.data->config;
      break;

    case BVIEW_SYSTEM_UTILS_CMD_API_GET_SWITCH_PROPERTIES:
      reply_data->switchProperties = system_utils_info.switchProperties;
      break;
    default:
      break;
  }
  /* release the lock for success and failed cases */

  return system_utils_send_response(reply_data);
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
  * @note   : This function is invoked by the system_utils to send the report and
  *           requested get configure params. This function internally
  *           calls the encoding api to encode the data, and the memory
  *           for the data is allocated. In case of both successful and
  *           unsuccessful send of the data, the memory must be freed.
  *
  *********************************************************************/
BVIEW_STATUS system_utils_send_response (BVIEW_SYSTEM_UTILS_RESPONSE_MSG_t * reply_data)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  uint8_t *pJsonBuffer = NULL;

  if (NULL == reply_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  _SYSTEM_UTILS_LOG(_SYSTEM_UTILS_DEBUG_INFO, "Reply_data\n"
      "reply_data->msg_type = %ld\n"
      "reply_data->rv = %d\n"
      "reply_data->unit = %d\n"
      "reply_data->cookie = %d\n",
      reply_data->msg_type,reply_data->rv, reply_data->unit, (NULL == reply_data->cookie)? true : false);

  if (reply_data->rv != BVIEW_STATUS_SUCCESS)
  {
    rest_response_send_error(reply_data->cookie, reply_data->rv, reply_data->id);
    return BVIEW_STATUS_SUCCESS;
  }
  else
  {
    if ((BVIEW_SYSTEM_UTILS_CMD_API_SET_FEATURE == reply_data->msg_type) ||
        (BVIEW_SYSTEM_UTILS_CMD_API_CANCEL_REQUEST == reply_data->msg_type))
    {
      rest_response_send_ok (reply_data->cookie);
      return BVIEW_STATUS_SUCCESS;
    }

  }

  /* Take lock*/
  SYSTEM_UTILS_LOCK_TAKE (&system_utils_info.system_utils_mutex);
  switch (reply_data->msg_type)
  {
    case BVIEW_SYSTEM_UTILS_CMD_API_GET_FEATURE:
      /* call json encoder api for feature  */

      rv = reg_hb_json_encode_get_feature (reply_data->id,
          reply_data->response.config,
          &pJsonBuffer);
      break;
    case  BVIEW_SYSTEM_UTILS_CMD_API_GET_SWITCH_PROPERTIES:
      /* call json encoder api for featute*/
      rv = reg_hb_json_encode_get_switch_properties (reply_data->id,
          reply_data->switchProperties,
          &pJsonBuffer);
      break;

    default:
      break;
  }

  if (NULL != pJsonBuffer && BVIEW_STATUS_SUCCESS == rv)
  {
    rv = rest_response_send(reply_data->cookie, (char *)pJsonBuffer, strlen((char *)pJsonBuffer));
    if (BVIEW_STATUS_SUCCESS != rv)
    {
      _SYSTEM_UTILS_LOG(_SYSTEM_UTILS_DEBUG_ERROR, "sending response failed due to error = %d\r\n",rv);
      LOG_POST (BVIEW_LOG_ERROR,
          " sending response failed due to error = %d\r\n",rv);
    }
    else
    {
      _SYSTEM_UTILS_LOG(_SYSTEM_UTILS_DEBUG_TRACE,"sent response to rest, pJsonBuffer = %s, len = %d\r\n", pJsonBuffer, (int)strlen((char *)pJsonBuffer));
    }
    /* free the json buffer */
    if (NULL != pJsonBuffer)
    {
      system_utils_json_memory_free(pJsonBuffer);
    }
  }
  else
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "encoding of system_utils response failed due to error = %d\r\n", rv);
    /* Can happen that memory is allocated,
       but the encoding failed.. in that case also
       free the json buffer.
     */
    if (NULL != pJsonBuffer)
    {
      system_utils_json_memory_free(pJsonBuffer);
    }
  }
  /* release the lock for success and failed cases */
  SYSTEM_UTILS_LOCK_GIVE(&system_utils_info.system_utils_mutex);
  return rv;
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
BVIEW_STATUS system_utils_module_register ()
{
  BVIEW_MODULE_FETAURE_INFO_t systemUtilsInfo;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

  memset (&systemUtilsInfo, 0, sizeof (BVIEW_MODULE_FETAURE_INFO_t));

  systemUtilsInfo.featureName[0] = '\0'; 
  systemUtilsInfo.featureId = BVIEW_FEATURE_SYSTEM;
  memcpy (systemUtilsInfo.restApiList, system_utils_api_list,
      sizeof(system_utils_api_list));

  /* Register with module manager. */
  rv = modulemgr_register (&systemUtilsInfo);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "system utility application failed to register with module mgr\r\n");
  }
  else
  {
    LOG_POST (BVIEW_LOG_INFO,
        "module mgr registration for system utility successful \r\n");
  }
  return rv;
}

/*********************************************************************
 * @brief : Register a feature's cancel api handler for the system 
 *                   cancel command type
 *
 * @param[in] int : feature id
 * @param[in] BVIEW_SYSTEM_UTILS_CANCEL_API_HANDLER_t : Feature's Handler 
 *                                        function for cancel command
 *
 * @retval  : BVIEW_STATUS_SUCCESS : when the function is successfully registered
 * @retval  : BVIEW_STATUS_ALREADY_CONFIGURED: when the hanlder is already configured 
 *                    for the feature
 * @retval  : BVIEW_STATUS_FAILURE : when failed to register the handler.
 *
 * @note :
 *
 *********************************************************************/
BVIEW_STATUS system_utils_cancel_api_request_register(unsigned int featId,
                                                BVIEW_SYSTEM_UTILS_CANCEL_API_HANDLER_t handler)
{
   unsigned int numOfEle, i = 0;
   BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
   unsigned int freeIndex = 0;
   bool freeIndexFound =  false;
   
   numOfEle = 
         sizeof(system_utils_info.system_utils_cancel_api_list)/sizeof(BVIEW_SYSTEM_UTILS_CANCEL_API_t);
   
   SYSTEM_UTILS_LOCK_TAKE (&system_utils_info.system_utils_mutex);
   for(i = 0; i < numOfEle; i++)
   {
     if ((system_utils_info.system_utils_cancel_api_list[i].present == false) && 
         (false == freeIndexFound))
     {
        freeIndex = i;
        freeIndexFound = true;
     }
     if (system_utils_info.system_utils_cancel_api_list[i].module == featId)
     {
        rv = BVIEW_STATUS_ALREADY_CONFIGURED;
        break;
     }
   }
   if (rv != BVIEW_STATUS_ALREADY_CONFIGURED)
   {
      if (freeIndexFound == true)
      {
        system_utils_info.system_utils_cancel_api_list[freeIndex].module = featId;
        system_utils_info.system_utils_cancel_api_list[freeIndex].handler = handler;
        system_utils_info.system_utils_cancel_api_list[freeIndex].present = true;
        rv = BVIEW_STATUS_SUCCESS;
      }
      else
      {
        rv = BVIEW_STATUS_TABLE_FULL; 
      } 
   }
   SYSTEM_UTILS_LOCK_GIVE (&system_utils_info.system_utils_mutex); 
   return rv;
}
    
/*********************************************************************
 * @brief : function to return the api handler for the system command type
 *
 * @param[in] int : request type
 *
 * @retval    : function pointer to the corresponding api
 *
 * @note :
 *
 *********************************************************************/
BVIEW_STATUS system_utils_cancel_request(unsigned int unit,
                                                unsigned int id)
{
  unsigned int i = 0;
  BVIEW_STATUS rv = BVIEW_STATUS_NOT_CONFIGURED;
  unsigned int numOfEle;

  numOfEle = sizeof(system_utils_info.system_utils_cancel_api_list)/sizeof(BVIEW_SYSTEM_UTILS_CANCEL_API_t);

  SYSTEM_UTILS_LOCK_TAKE (&system_utils_info.system_utils_mutex);
  for (i = 0; i < numOfEle; i++)
  {
    if ((system_utils_info.system_utils_cancel_api_list[i].present == true) &&
        (NULL != system_utils_info.system_utils_cancel_api_list[i].handler))  
    {
      rv = system_utils_info.system_utils_cancel_api_list[i].handler(unit, id);
      if (rv != BVIEW_STATUS_NOT_CONFIGURED)
      {
        /* Id cancel is handled by handler, return value is in rv*/
        break;
      }
    }
  }
  SYSTEM_UTILS_LOCK_GIVE (&system_utils_info.system_utils_mutex); 
  return rv;
}

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
BVIEW_STATUS system_utils_switch_cancel_request(BVIEW_SYSTEM_UTILS_REQUEST_MSG_t * msg_data)
{
  if (NULL == msg_data)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  return system_utils_cancel_request(msg_data->unit,
                                     msg_data->request.cancel.id);

}
