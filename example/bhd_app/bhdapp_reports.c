
/*********************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename bhdapp_reports.c 
  *
  * @purpose BroadView BHD reference application 
  *
  * @component Black hole detection 
  *
  * @comments
  *
  * @create 3/17/2016
  *
  * @author 
  * @end
  *
  **********************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "bhdapp.h"
#include "bhdapp_debug.h"
extern int bhdapp_read_from_agent (int fd );
static int numReports = 0;

/******************************************************************
 * @brief  This function starts a web server and never returns (unless an error).
 *
 * @param[in]   bhdapp      BHDAPP context for operation
 *                           
 * @retval   -1 Error creating web server
 *
 * @note     IPv4 only, non-multi-threaded.
 *********************************************************************/
int bhdapp_http_server_run(BHDAPP_CONFIG_t *config)
{
    int listenFd, connectionFd;
    int temp;
    struct sockaddr_in serverAddr;
    struct sockaddr_in peerAddr;
    socklen_t peerLen;

    _BHDAPP_ASSERT(config != NULL);

    _BHDAPP_LOG(_BHDAPP_DEBUG_INFO, "Starting HTTP server on port %d \n", config->localPort);

    /* setup listening socket */
    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    _BHDAPP_ASSERT_NET_ERROR((listenFd != -1), "Error Creating server socket");

    /* Initialize the server address and bind to the required port */
    memset(&serverAddr, 0, sizeof (struct sockaddr_in));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(config->localPort);

    /* bind to the socket, */
    temp = bind(listenFd, (struct sockaddr*) &serverAddr, sizeof (serverAddr));
    _BHDAPP_ASSERT_NET_SOCKET_ERROR((temp != -1), "Error binding to the port",listenFd);

    /* Listen for connections */
    temp = listen(listenFd, 1);
    _BHDAPP_ASSERT_NET_SOCKET_ERROR((temp != -1), "Error listening (making socket as passive) ",listenFd);

    /* Every thing set, start accepting connections */
    while (1)
    {
        _BHDAPP_LOG(_BHDAPP_DEBUG_TRACE, "Waiting for HTTP connections on port %d \n", config->localPort);

        peerLen = sizeof (peerAddr);

        /* wait for an incoming connection */
        temp = accept(listenFd, (struct sockaddr*) &peerAddr, &peerLen);
        if (temp == -1)
        {
          _BHDAPP_LOG(_BHDAPP_DEBUG_TRACE, "Accept Failed \n");
          continue;
        }
        _BHDAPP_LOG(_BHDAPP_DEBUG_TRACE, "Received connection \n");

        /* initialize the connection socket */
        connectionFd = temp;

        /* process the request */
        if (bhdapp_read_from_agent(connectionFd) == 0)
            numReports++;
    }

    /* execution  shouldn't reach here */
    _BHDAPP_LOG(_BHDAPP_DEBUG_TRACE, "HTTP Server , exiting [%d: %s] \n", errno, strerror(errno));
    close(listenFd);
    return 0;

}
