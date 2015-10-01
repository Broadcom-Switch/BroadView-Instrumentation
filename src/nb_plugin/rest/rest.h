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

#ifndef INCLUDE_REST_H
#define	INCLUDE_REST_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "broadview.h"
#include "rest_debug.h"

#define REST_MAX_STRING_LENGTH      128
#define REST_MAX_HTTP_BUFFER_LENGTH 2048

#define REST_MAX_SESSIONS    5

#define REST_MAX_IP_ADDR_LENGTH    20

/* file from where the configuration properties are read. */
#define REST_CONFIG_FILE    "agent_config.cfg"

#define REST_CONFIG_PROPERTY_CLIENT_IP   "bview_client_ip"
#define REST_CONFIG_PROPERTY_CLIENT_IP_DEFAULT   "127.0.0.1"    

#define REST_CONFIG_PROPERTY_CLIENT_PORT   "bview_client_port"
#define REST_CONFIG_PROPERTY_CLIENT_PORT_DEFAULT   9070

#define REST_CONFIG_PROPERTY_LOCAL_PORT "agent_port"
#define REST_CONFIG_PROPERTY_LOCAL_PORT_DEFAULT 8080


/* Macro to acquire lock */
#define REST_LOCK_TAKE(_ptr)                                                        \
        {                                                                           \
           if (0 != pthread_mutex_lock (&_ptr->config_mutex))                          \
           {                                                                        \
              LOG_POST (BVIEW_LOG_ERROR,                                            \
                  "Failed to take the lock for rest config \r\n");                \
              return BVIEW_STATUS_FAILURE;                                          \
           }                                                                        \
         }
/*  to release lock*/
#define REST_LOCK_GIVE(_ptr)                                                        \
         {                                                                          \
           if (0 != pthread_mutex_unlock(&_ptr->config_mutex))                         \
           {                                                                        \
              LOG_POST (BVIEW_LOG_ERROR,                                            \
              "Failed to Release the lock for rest config \r\n");                 \
               return BVIEW_STATUS_FAILURE;                                         \
            }                                                                       \
          }


typedef struct _rest_config_
{
    char clientIp[REST_MAX_IP_ADDR_LENGTH];

    int clientPort;

    int localPort;
} REST_CONFIG_t;

/* REST session */
typedef struct _rest_session_
{
    /* is this session in use ? */
    bool inUse;

    /* http method */
    char httpMethod[REST_MAX_STRING_LENGTH+1];

    /* incoming URL */
    char url[REST_MAX_STRING_LENGTH+1];

    /* incoming REST method */
    char restMethod[REST_MAX_STRING_LENGTH];

    /* socket on which the response message, if any, is to be sent */
    int connectionFd;

    /* buffer to store the HTTP request message */
    char buffer[REST_MAX_HTTP_BUFFER_LENGTH + 1];

    /* data length */
    int length;

    /* JSON content start, filled while parsing */
    char *json;

    /* peer address */
    struct sockaddr_in peerAddr;

    /* time the session is created */
    time_t creationTime;

} REST_SESSION_t;

typedef struct _rest_context_
{
    REST_CONFIG_t config;

    REST_SESSION_t sessions[REST_MAX_SESSIONS];

    pthread_mutex_t config_mutex;

} REST_CONTEXT_t;

typedef BVIEW_STATUS(*BVIEW_REST_ERROR_HANDLER_t) (int fd,
                                                 char *jsonBuffer,
                                                 int bufLength);


typedef struct _rest_json_error_map_
{
 BVIEW_STATUS return_code;
 int   json_code;
 char *errorstr;
 BVIEW_REST_ERROR_HANDLER_t handler;
}REST_JSON_ERROR_t;



/* initialize configurations */
BVIEW_STATUS rest_config_init(REST_CONTEXT_t *rest);

/* dumps existing sessions */
void rest_session_dump(REST_SESSION_t *session);

/* allocations an available session (returns the index) */
BVIEW_STATUS rest_allocate_session(REST_CONTEXT_t *context, int *sessionId);

/* initialize sessions */
BVIEW_STATUS rest_sessions_init(REST_CONTEXT_t *context);

/* sends a HTTP 200 OK message to the client  */
BVIEW_STATUS rest_send_200(int fd);

/* sends a HTTP 404 message to the client  */
BVIEW_STATUS rest_send_404(int fd);

/* sends a HTTP 400 message to the client  */
BVIEW_STATUS rest_send_400(int fd);

/* sends a HTTP 500 message to the client  */
BVIEW_STATUS rest_send_500(int fd);

/* sends asynchronous report to client */
BVIEW_STATUS rest_send_async_report(REST_CONTEXT_t *rest, char *buffer, int length);

BVIEW_STATUS rest_http_server_run(REST_CONTEXT_t *rest);
BVIEW_STATUS rest_session_validate(REST_CONTEXT_t *context, REST_SESSION_t *session);
BVIEW_STATUS rest_send_200_with_data(int fd, char *buffer, int length);

/******************************************************************
 * @brief  sends a HTTP 404 message to the client 
 *
 * @param[in]   fd    socket for sending message
 * @param[in]   buffer  Buffer containing data to be sent
 * @param[in]   length  number of bytes to be sent 
 *
 * @retval   BVIEW_STATUS_SUCCESS if send is successful
 * 
 * @note     
 *********************************************************************/
BVIEW_STATUS rest_send_404_with_data(int fd, char *buffer, int length);


/******************************************************************
 * @brief  sends a HTTP 400 message to the client 
 *
 * @param[in]   fd    socket for sending message
 * @param[in]   buffer  Buffer containing data to be sent
 * @param[in]   length  number of bytes to be sent 
 *
 * @retval   BVIEW_STATUS_SUCCESS if send is successful
 * 
 * @note     
 *********************************************************************/
BVIEW_STATUS rest_send_400_with_data(int fd, char *buffer, int length);

/******************************************************************
 * @brief  sends a HTTP 500 message to the client 
 *
 * @param[in]   fd    socket for sending message
 * @param[in]   buffer  Buffer containing data to be sent
 * @param[in]   length  number of bytes to be sent 
 *
 * @retval   BVIEW_STATUS_SUCCESS if send is successful
 * 
 * @note     
 *********************************************************************/
BVIEW_STATUS rest_send_500_with_data(int fd, char *buffer, int length);
 
BVIEW_STATUS rest_send_json_error_async(BVIEW_STATUS rv);

BVIEW_STATUS rest_send_json_error(void *cookie,  BVIEW_STATUS rv, int id);

BVIEW_STATUS rest_session_fd_get (void *cookie, int *fd);

BVIEW_STATUS rest_json_error_fn_invoke(int fd, BVIEW_STATUS rv, int id);

BVIEW_STATUS rest_get_json_error_data(BVIEW_STATUS rv, int *json_val, 
                                      char *ptr, BVIEW_REST_ERROR_HANDLER_t *handler);
BVIEW_STATUS rest_get_id_from_request(char * jsonBuffer, int bufLength, int *id);


int rest_agent_config_params_modify(char *ipaddr, unsigned int clientPort);

#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_REST_H */

