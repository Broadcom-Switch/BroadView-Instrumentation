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
#include <arpa/inet.h>
#include <pthread.h>

#include "bstapp.h"
#include "bstapp_debug.h"

extern BSTAPP_REST_MSG_t bstRestMessages[];

/******************************************************************
 * @brief  This function processes incoming http request .
 *
 * @param[in]   fd      socket to read request data from
 * 
 * @note     All errors are processed internally. Caller ignores the RV
 *********************************************************************/
int bstapp_read_from_agent (int fd )
{
    char *buf;
    int length = 0;
    int temp = 0;
    char report[BSTAPP_MAX_REPORT_LENGTH];

    _BSTAPP_LOG(_BSTAPP_DEBUG_TRACE, "Extracting data from incoming report  \n");

    buf = &report[0];

    /* Read data from incoming request into the buffer */
    do
    {
        temp = read(fd, (buf + length), (BSTAPP_MAX_REPORT_LENGTH - length));
        if (temp > 0)
        {
            length += temp;
        }
        /* Make socket as non-blocking, so that it doesn't block for connection */
/*        sopts = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, sopts | O_NONBLOCK); */

    } while (temp > 0);

    close(fd);

    /* if there is any error reading */
    if ((temp < 0) && (length <= 0))
    {
        _BSTAPP_LOG(_BSTAPP_DEBUG_ERROR,
                    "BSTAPP : Error reading data on incoming report, closing socket [%d : %s] \n",
                    errno, strerror(errno));
        return -1;
    }

    bstapp_message_log(buf, length, true);

    return 0;
}

/******************************************************************
 * @brief  sends an commands to agent and logs the received responses 
 *
 * 
 * @note     
 *********************************************************************/
int  bstapp_communicate_with_agent(void *param)
{
    BSTAPP_CONFIG_t *config = (BSTAPP_CONFIG_t *) param;
    char *header = "%s /broadview/bst/%s HTTP/1.1\r\n"
            "Host: BroadViewAgent \r\n"
            "User-Agent: BroadView BST App\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml\r\n"
            "Content-Length: %d\r\n"
            "\r\n";

    char sendBuf[BSTAPP_MAX_HTTP_BUFFER_LENGTH] = { 0 };
    int clientFd;
    struct sockaddr_in clientAddr;
    int temp = 0;
    int i = 0;
    BSTAPP_REST_MSG_t *restMsg;

    _BSTAPP_ASSERT(config != NULL);

    _BSTAPP_LOG(_BSTAPP_DEBUG_TRACE, "Starting communication with agent ");

    for (i=0; i < 13; i++)
    {
        restMsg = &bstRestMessages[i];

        _BSTAPP_LOG(_BSTAPP_DEBUG_TRACE, "Now attempting : %s ", restMsg->descr);


        memset(sendBuf, 0, sizeof (sendBuf));
        snprintf(sendBuf, BSTAPP_MAX_HTTP_BUFFER_LENGTH, header,
                 restMsg->httpMethod, restMsg->method, strlen(restMsg->json));

        /* create socket to send data to */
        clientFd = socket(AF_INET, SOCK_STREAM, 0);
        if (clientFd == -1) {
            _BSTAPP_LOG(_BSTAPP_DEBUG_ERROR, "Error Creating server socket : %s \n", strerror(errno));
            continue;
        }
        /* setup the socket */
        memset(&clientAddr, 0, sizeof (struct sockaddr_in));
        clientAddr.sin_family = AF_INET;
        clientAddr.sin_port = htons(config->agentPort);
        temp = inet_pton(AF_INET, &config->agentIp[0], &clientAddr.sin_addr);
        if (temp <= 0) {
            _BSTAPP_LOG(_BSTAPP_DEBUG_ERROR, "Error Creating server socket %s \n", strerror(errno));
            close(clientFd);
            continue;
        }

        /* connect to the peer */
        temp = connect(clientFd, (struct sockaddr *) &clientAddr, sizeof (clientAddr));
        if (temp == -1) {
            _BSTAPP_LOG(_BSTAPP_DEBUG_ERROR, "Error connecting to client for sending async reports %s \n", strerror(errno));
            close(clientFd);
            continue;
        }

        /* log what is being sent */
        bstapp_message_log(sendBuf, strlen(sendBuf), false);
        bstapp_message_log(restMsg->json, strlen(restMsg->json), false);

        /* send data */
        temp = send(clientFd, sendBuf, strlen(sendBuf), MSG_MORE);
        if (temp == -1) {
            _BSTAPP_LOG(_BSTAPP_DEBUG_ERROR, "Error sending data %s \n", strerror(errno));
            close(clientFd);
            continue;
        }
        temp = send(clientFd, restMsg->json, strlen(restMsg->json), 0);
        if (temp == -1) {
            _BSTAPP_LOG(_BSTAPP_DEBUG_ERROR, "Error sending data %s \n", strerror(errno));
            close(clientFd);
            continue;
        }
         
        /*Now read from the socket for the response */
        bstapp_read_from_agent(clientFd);
        sleep(2);
    }

    _BSTAPP_LOG(_BSTAPP_DEBUG_TRACE, "Completed communication with agent, exiting ");

    pthread_exit(NULL);

}
