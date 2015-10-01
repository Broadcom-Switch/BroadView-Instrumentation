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

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <errno.h>

#include "broadview.h"

#include "json.h"
#include "cJSON.h"

#include "rest.h"
#include "rest_http.h"

#include "openapps_feature.h"
#include "modulemgr.h"

#define BVIEW_REST_MAX_SUPPORTED_METHODS 4

/******************************************************************
 * @brief  validates a string , whether its a proper HTTP method or not 
 *
 * @param[in]   method    string for validation
 *
 * @retval   BVIEW_STATUS_SUCCESS if the string is a valid HTTP method
 * @retval   BVIEW_STATUS_FAILURE otherwise
 * 
 * @note     
 *********************************************************************/
static BVIEW_STATUS rest_validate_http_method(char *method)
{
    /* Not exhaustive, but minimal methods for functionality */
    static char *supportedMethods[] = { "GET" , "POST", "PUT", "DELETE" };
    int i = 0;
    for (i = 0; i < BVIEW_REST_MAX_SUPPORTED_METHODS; i++)
    {
        if (strcmp(method, supportedMethods[i]) == 0)
            return BVIEW_STATUS_SUCCESS;
    }

    return BVIEW_STATUS_FAILURE;
}

/******************************************************************
 * @brief  validates a string , whether its a supported URL or not 
 *
 * @param[in]   method    string for validation
 *
 * @retval   BVIEW_STATUS_SUCCESS if the string is a valid URL
 * @retval   BVIEW_STATUS_FAILURE otherwise
 * 
 * @note     
 *********************************************************************/
static BVIEW_STATUS rest_validate_url(char *url)
{
    /* currently hard-coded for BST */
    if ((strstr(url, "broadview/bst/") != NULL) ||
        (strstr(url, "broadview/packettrace/") != NULL) || 
        (strstr(url, "broadview/")))
    {
        return BVIEW_STATUS_SUCCESS;
    }

    return BVIEW_STATUS_FAILURE;
}

/******************************************************************
 * @brief  This function parses http request to extract relevant fields.
 *
 * @param[in]   rest    REST context for operation
 * @param[in]   fd      socket to read request data from
 * @param[in]   peer    The connecting peer details
 * 
 * @note     
 *********************************************************************/
static BVIEW_STATUS rest_parse_http_request_to_session (REST_SESSION_t *session)
{
    char *buf = &session->buffer[0];
    char *httpMethod, *url, *json, *restMethod;
    BVIEW_STATUS status;
    int temp = 0, urlLength = 0;

    /* raw http data is available @ session->buffer */
    /* This needs to be parsed into httpMethod, URL and the JSON body */


    /* HTTP method will be the first word of the request */
    httpMethod = strtok(buf, REST_HTTP_SPACE);
    _REST_ASSERT_NET_ERROR((httpMethod != NULL), "REST : Invalid HTTP Request \n");

    /* validate the method */
    status = rest_validate_http_method(httpMethod);
    _REST_ASSERT_NET_ERROR((status == BVIEW_STATUS_SUCCESS), "REST : Unsupported HTTP Method \n");

    /* advance the buf, now it should point to the URL */
    buf += strlen(httpMethod) + 1;

    url = strtok(buf, REST_HTTP_SPACE);
    _REST_ASSERT_NET_ERROR((url != NULL), "REST : Invalid HTTP Request \n");

    /* validate the URL */
    status = rest_validate_url(url);
    _REST_ASSERT_NET_ERROR((status == BVIEW_STATUS_SUCCESS),
                           "REST : Unsupported URL \n");

    /* obtain the REST method */
    restMethod = url;
    urlLength = strlen(url);
    for (temp = 0; temp < urlLength; temp++)
    {
        /* method is the string after the last / */
        if (url[temp] == '/')
        {
            if (temp < (urlLength - 1))
                restMethod = &url[temp + 1];
        }
    }

    /* move buf, past the URL, this should now point to HTTP header, */
    buf += urlLength + 1;

    /* look for the twin carriage-return-linefeed sequence, which signals end of header */
    json = strstr(buf, REST_HTTP_TWIN_CRLF);
    _REST_ASSERT_NET_ERROR((json != NULL), "REST : Invalid HTTP Request \n");

    /* move past the end of header, after which, json points to body */
    json += strlen(REST_HTTP_TWIN_CRLF);

    /* update the session */
    session->json = json;
    strncpy(session->httpMethod, httpMethod, REST_MAX_STRING_LENGTH);
    strncpy(session->url, url, REST_MAX_STRING_LENGTH);
    memcpy(session->restMethod, restMethod, REST_MAX_STRING_LENGTH);
    session->length -= (json - session->buffer);

    return BVIEW_STATUS_SUCCESS;
}

/******************************************************************
 * @brief  This function processes incoming http request .
 *
 * @param[in]   rest    REST context for operation
 * @param[in]   fd      socket to read request data from
 * @param[in]   peer    The connecting peer details
 * 
 * @note     All errors are processed internally. Caller ignores the RV
 *********************************************************************/
static BVIEW_STATUS rest_process_http_request (REST_CONTEXT_t *rest,
                                               int fd,
                                               struct sockaddr_in peer,
                                               socklen_t peerLen)
{
    int sessionId;
    BVIEW_STATUS status, ret;
    REST_SESSION_t *session;
    char *buf;
    int length = 0;
    int temp = 0;
    int id =0;
    struct timeval timeout;
    fd_set cset;
    int retval = 0;
    

    BVIEW_REST_API_HANDLER_t handler;

    _REST_LOG(_REST_DEBUG_TRACE, "Extracting data from incoming request  \n");

    /* find an available session buffer for this request */
    status = rest_allocate_session(rest, &sessionId);
    _REST_ASSERT_NET_ERROR((status == BVIEW_STATUS_SUCCESS),
                           "REST : No available session for incoming request \n");
    session = &rest->sessions[sessionId];

    buf = &session->buffer[0];

   for (;;)
   {
     FD_ZERO (&cset);
     FD_SET (fd, &cset);

     timeout.tv_sec = 2;
     timeout.tv_usec = 0;

     retval = select (fd+1, &cset, NULL, NULL, &timeout);
     if (retval < 0) 
     {
       /* error; log/die/whatever and close() socket */
       _REST_LOG(_REST_DEBUG_ERROR,
           "REST : Error select of socket failed, closing socket [%d : %s] \n",
           errno, strerror(errno));
       close(fd);
       return BVIEW_STATUS_SUCCESS;
     }
     else if (retval == 0) 
     {
       /* timed out without receiving any data; log/die/whatever and close() */
       _REST_LOG(_REST_DEBUG_ERROR,
           "REST : Socket timed out no data, closing socket [%d : %s] \n",
           errno, strerror(errno));
       break;
     }

     temp = read(fd, (buf + length), (REST_MAX_HTTP_BUFFER_LENGTH - length));
     if (temp < 0)
     {
       if (errno == EINTR)
       {
         continue; /* perfectly normal; try again */
       }
       else if (errno == EAGAIN) 
       {
         continue;
       }
       else
       {
         /* some real error; log/die/whatever and close() socket */
         _REST_LOG(_REST_DEBUG_ERROR,
             "REST : Socket read error, closing socket [%d : %s] \n",
             errno, strerror(errno));
         close(fd);
         return BVIEW_STATUS_SUCCESS;
       }
     } 
     else if (temp == 0)
     {
       /* the connection has been closed by your peer; clean-up and close() */
       _REST_LOG(_REST_DEBUG_ERROR,
           "REST : Socket closed by peer, closing socket [%d : %s] \n",
           errno, strerror(errno));
       break;
     }
     else
     {
       /* you got some data; do whatever with it... */
       length += temp;
       continue;
     }
   }


    /* update the session */
    session->connectionFd = fd;
    session->peerAddr = peer;
    time(&session->creationTime);
    session->length = length;

    status = rest_parse_http_request_to_session(session);

    ret =  rest_get_id_from_request (session->json, session->length, &id);

    if ((BVIEW_STATUS_SUCCESS == ret) && (BVIEW_STATUS_SUCCESS != status))
    {
      status = BVIEW_STATUS_UNSUPPORTED;
      rest_json_error_fn_invoke(fd, status, id);
      close(fd);
      session->inUse = false;
      return BVIEW_STATUS_SUCCESS;
    }
    else
    {
      if (status != BVIEW_STATUS_SUCCESS)
      {
        /* send a 404 unsupported back to client */
        rest_send_404(fd);
        close(fd);
        return BVIEW_STATUS_SUCCESS;
      }
    }

    _REST_LOG(_REST_DEBUG_TRACE, "Data extraction from incoming request complete  \n");

    rest_session_dump(session);

    /* mark the session as 'used' */
    session->inUse = true;

    /* talk to module manager and get the handler for this request */
    status = modulemgr_rest_api_handler_get(session->json, session->length, &handler);
   
    if (BVIEW_STATUS_SUCCESS == status)
    {
      _REST_LOG(_REST_DEBUG_TRACE, "successfully found the api handler for the inputted json\n");
    }
 
    if ((BVIEW_STATUS_SUCCESS == ret) && (BVIEW_STATUS_SUCCESS != status))
    {
      status = BVIEW_STATUS_UNSUPPORTED;
      rest_json_error_fn_invoke(fd, status, id);
      close(fd);
      session->inUse = false;
      return BVIEW_STATUS_SUCCESS;
    }
    else
    {
      /* if there is no registered handler for this request, send a 404 */
      if (status != BVIEW_STATUS_SUCCESS)
      {
        /* send a 404 unsupported back to client */
        rest_send_404(fd);
        close(fd);
        session->inUse = false;
        return BVIEW_STATUS_SUCCESS;
      }
    }

    /* invoke the handler */
    status = handler(session, session->json, session->length);

    if ((BVIEW_STATUS_SUCCESS == ret) && (BVIEW_STATUS_SUCCESS != status))
    {
      rest_json_error_fn_invoke(fd, status, id);
      close(fd);
      session->inUse = false;
      return BVIEW_STATUS_SUCCESS;
    }
    else
    {
      /* if not successful processing the request, send appropriate error code */
      if (status != BVIEW_STATUS_SUCCESS)
      {
        if (status == BVIEW_STATUS_INVALID_JSON)
        {
          rest_send_500(fd);
        }
        else
        {
          rest_send_400(fd);
        }

        close(fd);
        session->inUse = false;
        return BVIEW_STATUS_SUCCESS;
      }
    }

    /* we keep the session, and keep the fd open. */
    return BVIEW_STATUS_SUCCESS;
}

/******************************************************************
 * @brief  This function starts a web server and never returns (unless an error).
 *
 * @param[in]   rest      REST context for operation
 *                           
 * @retval   BVIEW_STATUS_FAILURE Error creating web server
 *
 * @note     IPv4 only, non-multi-threaded.
 *********************************************************************/
BVIEW_STATUS rest_http_server_run(REST_CONTEXT_t *rest)
{
    int listenFd, connectionFd;
    int temp;
    struct sockaddr_in serverAddr;
    struct sockaddr_in peerAddr;
    socklen_t peerLen;

    _REST_ASSERT(rest != NULL);

    _REST_LOG(_REST_DEBUG_INFO, "Starting HTTP server on port %d \n", rest->config.localPort);

    /* setup listening socket */
    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    _REST_ASSERT_NET_ERROR((listenFd != -1), "Error Creating server socket");

    /* Initialize the server address and bind to the required port */
    memset(&serverAddr, 0, sizeof (struct sockaddr_in));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(rest->config.localPort);

    /* bind to the socket, */
    temp = bind(listenFd, (struct sockaddr*) &serverAddr, sizeof (serverAddr));
    _REST_ASSERT_NET_SOCKET_ERROR((temp != -1), "Error binding to the port",listenFd);

    /* Listen for connections */
    temp = listen(listenFd, REST_MAX_SESSIONS);
    _REST_ASSERT_NET_SOCKET_ERROR((temp != -1), "Error listening (making socket as passive) ",listenFd);

    /* Every thing set, start accepting connections */
    while (true)
    {
        _REST_LOG(_REST_DEBUG_TRACE, "Waiting for HTTP connections on port %d \n", rest->config.localPort);

        peerLen = sizeof (peerAddr);

        /* wait for an incoming connection */
        temp = accept(listenFd, (struct sockaddr*) &peerAddr, &peerLen);
        if (temp == -1)
        {
          _REST_LOG(_REST_DEBUG_TRACE, "Accept Failed \n");
          continue;
        }

        _REST_LOG(_REST_DEBUG_TRACE, "Received connection \n");

        /* initialize the connection socket */
        connectionFd = temp;

        /* process the request */
        rest_process_http_request(rest, connectionFd, peerAddr, peerLen);
    }

    /* execution  shouldn't reach here */
    _REST_LOG(_REST_DEBUG_TRACE, "HTTP Server , Unknown error, exiting [%d: %s] \n", errno, strerror(errno));
    close(listenFd);
    return BVIEW_STATUS_SUCCESS;

}


BVIEW_STATUS rest_get_id_from_request(char * jsonBuffer, int bufLength, int *id)
{
  cJSON *root, *json_id;
  int temp_id =0;

  /* Validating 'jsonBuffer' */
  JSON_VALIDATE_POINTER(jsonBuffer,"jsonBuffer",BVIEW_STATUS_INVALID_PARAMETER);


  /* Parse JSON to a C-JSON root */
  root = cJSON_Parse(jsonBuffer);
  JSON_VALIDATE_JSON_POINTER(root,"root",BVIEW_STATUS_INVALID_JSON);


  json_id = cJSON_GetObjectItem(root, "id");
  JSON_VALIDATE_JSON_POINTER(json_id, "id", BVIEW_STATUS_INVALID_JSON);
  JSON_VALIDATE_JSON_AS_NUMBER(json_id, "id");
  temp_id = json_id->valueint;
  /* Ensure  that the number 'id' is within range of [1,100000] */
  JSON_CHECK_VALUE_AND_CLEANUP (temp_id, 1, 100000);

  *id = temp_id;
  return BVIEW_STATUS_SUCCESS;

}

