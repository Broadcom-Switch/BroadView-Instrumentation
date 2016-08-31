
/*********************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename bhdapp_http.c 
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
#include <pthread.h>

#include "bhdapp.h"
#include "bhdapp_menu.h"
#include "bhdapp_debug.h"

extern BHDAPP_REST_MSG_t bhdRestMessages[];

/******************************************************************
 * @brief  This function processes incoming http request .
 *
 * @param[in]   fd      socket to read request data from
 * 
 * @note     All errors are processed internally. Caller ignores the RV
 *********************************************************************/
int bhdapp_read_from_agent (int fd )
{
    char *buf;
    int length = 0;
    int temp = 0;
    char report[BHDAPP_MAX_REPORT_LENGTH];

    _BHDAPP_LOG(_BHDAPP_DEBUG_TRACE, "Extracting data from incoming report  \n");

    buf = &report[0];

    /* Read data from incoming request into the buffer */
    do
    {
        temp = read(fd, (buf + length), (BHDAPP_MAX_REPORT_LENGTH - length));
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
        _BHDAPP_LOG(_BHDAPP_DEBUG_ERROR,
                    "BHDAPP : Error reading data on incoming report, closing socket [%u : %s] \n",
                    errno, strerror(errno));
        return -1;
    }

    bhdapp_message_log(buf, length, true);

    return 0;
}


/******************************************************************
 * @brief  sends an commands to agent and logs the received responses 
 *
 * 
 * @note     
 *********************************************************************/
int  bhdapp_communicate_with_agent(void *param)
{
    BHDAPP_CONFIG_t *config = (BHDAPP_CONFIG_t *) param;
    unsigned int restMsgIndex = 0; 
    char *header = "%s /broadview/black-hole-detection/%s HTTP/1.1\r\n"
            "Host: BroadViewAgent \r\n"
            "User-Agent: BroadView Black Hole Detection App\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml\r\n"
            "Content-Length: %u\r\n"
            "\r\n";

    char sendBuf[BHDAPP_MAX_HTTP_BUFFER_LENGTH] = { 0 };
    int clientFd;
    struct sockaddr_in clientAddr;
    int temp = 0;
    BHDAPP_REST_MSG_t *restMsg;

    _BHDAPP_ASSERT(config != NULL);

    _BHDAPP_LOG(_BHDAPP_DEBUG_TRACE, "Starting communication with agent ");

    do {
        /* Get user input and update the JSON*/        
        bhdapp_get_user_input(bhdRestMessages, &restMsgIndex);
        restMsg = &bhdRestMessages[restMsgIndex];

        memset(sendBuf, 0, sizeof (sendBuf));
        snprintf(sendBuf, BHDAPP_MAX_HTTP_BUFFER_LENGTH, header,
                 restMsg->httpMethod, restMsg->method, strlen(restMsg->json));

        _BHDAPP_LOG(_BHDAPP_DEBUG_DUMPJSON, "Json is %s\n", restMsg->json);    
        /* create socket to send data to */
        clientFd = socket(AF_INET, SOCK_STREAM, 0);
        if (clientFd == -1) {
            _BHDAPP_LOG(_BHDAPP_DEBUG_ERROR, "Error Creating server socket : %s \n", strerror(errno));
            continue;
        }
        /* setup the socket */
        memset(&clientAddr, 0, sizeof (struct sockaddr_in));
        clientAddr.sin_family = AF_INET;
        clientAddr.sin_port = htons(config->agentPort);
        temp = inet_pton(AF_INET, &config->agentIp[0], &clientAddr.sin_addr);
        if (temp <= 0) {
            _BHDAPP_LOG(_BHDAPP_DEBUG_ERROR, "Error Creating server socket %s \n", strerror(errno));
            close(clientFd);
            continue;
        }

        /* connect to the peer */
        temp = connect(clientFd, (struct sockaddr *) &clientAddr, sizeof (clientAddr));
        if (temp == -1) {
            _BHDAPP_LOG(_BHDAPP_DEBUG_ERROR, "Error connecting to client for sending async reports %s \n", strerror(errno));
            close(clientFd);
            continue;
        }

        /* log what is being sent */
        bhdapp_message_log(sendBuf, strlen(sendBuf), false);
        bhdapp_message_log(restMsg->json, strlen(restMsg->json), false);

        /* send data */
        temp = send(clientFd, sendBuf, strlen(sendBuf), MSG_MORE);
        if (temp == -1) {
            _BHDAPP_LOG(_BHDAPP_DEBUG_ERROR, "Error sending data %s \n", strerror(errno));
            close(clientFd);
            continue;
        }
        temp = send(clientFd, restMsg->json, strlen(restMsg->json), 0);
        if (temp == -1) {
            _BHDAPP_LOG(_BHDAPP_DEBUG_ERROR, "Error sending data %s \n", strerror(errno));
            close(clientFd);
            continue;
        }
         
        /*Now read from the socket for the response */
        bhdapp_read_from_agent(clientFd);
        sleep(2);
    }while(true);

    _BHDAPP_LOG(_BHDAPP_DEBUG_TRACE, "Completed communication with agent, exiting ");

}
