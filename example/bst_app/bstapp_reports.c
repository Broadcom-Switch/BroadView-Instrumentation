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

#include "bstapp.h"
#include "bstapp_debug.h"
extern int bstapp_read_from_agent (int fd );
static int numReports = 0;

/******************************************************************
 * @brief  This function starts a web server and never returns (unless an error).
 *
 * @param[in]   bstapp      BSTAPP context for operation
 *                           
 * @retval   -1 Error creating web server
 *
 * @note     IPv4 only, non-multi-threaded.
 *********************************************************************/
int bstapp_http_server_run(BSTAPP_CONFIG_t *config)
{
    int listenFd, connectionFd;
    int temp;
    struct sockaddr_in serverAddr;
    struct sockaddr_in peerAddr;
    socklen_t peerLen;

    _BSTAPP_ASSERT(config != NULL);

    _BSTAPP_LOG(_BSTAPP_DEBUG_INFO, "Starting HTTP server on port %d \n", config->localPort);

    /* setup listening socket */
    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    _BSTAPP_ASSERT_NET_ERROR((listenFd != -1), "Error Creating server socket");

    /* Initialize the server address and bind to the required port */
    memset(&serverAddr, 0, sizeof (struct sockaddr_in));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(config->localPort);

    /* bind to the socket, */
    temp = bind(listenFd, (struct sockaddr*) &serverAddr, sizeof (serverAddr));
    _BSTAPP_ASSERT_NET_SOCKET_ERROR((temp != -1), "Error binding to the port",listenFd);

    /* Listen for connections */
    temp = listen(listenFd, 1);
    _BSTAPP_ASSERT_NET_SOCKET_ERROR((temp != -1), "Error listening (making socket as passive) ",listenFd);

    /* Every thing set, start accepting connections */
    while (numReports <= config->maxReports)
    {
        _BSTAPP_LOG(_BSTAPP_DEBUG_TRACE, "Waiting for HTTP connections on port %d \n", config->localPort);

        peerLen = sizeof (peerAddr);

        /* wait for an incoming connection */
        temp = accept(listenFd, (struct sockaddr*) &peerAddr, &peerLen);
        if (temp == -1)
        {
          _BSTAPP_LOG(_BSTAPP_DEBUG_TRACE, "Accept Failed \n");
          continue;
        }
        _BSTAPP_LOG(_BSTAPP_DEBUG_TRACE, "Received connection \n");

        /* initialize the connection socket */
        connectionFd = temp;

        /* process the request */
        if (bstapp_read_from_agent(connectionFd) == 0)
            numReports++;
    }

    /* execution  shouldn't reach here */
    _BSTAPP_LOG(_BSTAPP_DEBUG_TRACE, "HTTP Server , exiting [%d: %s] \n", errno, strerror(errno));
    close(listenFd);
    return 0;

}
