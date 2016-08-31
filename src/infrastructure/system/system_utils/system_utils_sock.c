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

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#include <arpa/inet.h>

#include "broadview.h"
#include "openapps_log_api.h"
#include "system.h"
#include "system_utils_app.h"
#include "system_utils.h"
#include "system_utils_sock.h"
#include "openapps_feature.h"


#define  SYSTEM_UTILS_CONNECT_WAIT_TIME_SEC    0
#define  SYSTEM_UTILS_CONNECT_WAIT_TIME_USEC   (5*100*1000)
#define  SYSTEM_UTILS_SEND_WAIT_TIME_SEC       0
#define  SYSTEM_UTILS_SEND_WAIT_TIME_USEC      (5*100*1000)

/*************************************************************************
 * @brief      Performs a TCP connection operation in a non-blocking mode
 *
 * @param[in]  sockfd     Connecting socket identifier.
 * @param[in]  servAddr   Remote peer address
 *
 * @retval     BVIEW_STATUS_SUCCESS  Connection established successfully.
 *             BVIEW_STATUS_FAILURE  Failed to establish connection
 * @note     
 **************************************************************************/
BVIEW_STATUS system_utils_non_block_connect(int sockFd,
                                    const struct sockaddr *servAddr)
{
  fd_set  clientSet;
  struct  timeval timeout;
  int     flags, result, temp;

  flags = fcntl(sockFd, F_GETFL, 0);
  if (flags < 0)
  {
    _SYSTEM_UTILS_LOG(_SYSTEM_UTILS_DEBUG_ERROR, "Error Reading client socket\r \n");
    return BVIEW_STATUS_FAILURE;
  }

  if (fcntl(sockFd, F_SETFL, flags | O_NONBLOCK) <0) 
  {   
    _SYSTEM_UTILS_LOG(_SYSTEM_UTILS_DEBUG_ERROR, "Error Setting client socket to non-blocking \r \n");
    return BVIEW_STATUS_FAILURE;
  }   

  FD_ZERO(&clientSet);
  FD_SET(sockFd, &clientSet);
  timeout.tv_sec = SYSTEM_UTILS_CONNECT_WAIT_TIME_SEC;
  timeout.tv_usec = SYSTEM_UTILS_CONNECT_WAIT_TIME_USEC;

  /* connect to the peer */
  temp = connect(sockFd, (struct sockaddr *) servAddr, sizeof (struct sockaddr));

  if(temp < 0)
  {
    if (errno == EINPROGRESS)
    {
      result = select(sockFd + 1, NULL, &clientSet, NULL, &timeout);

      if (result <= 0)
      {
        _SYSTEM_UTILS_LOG(_SYSTEM_UTILS_DEBUG_ERROR, "Error connecting to client \r \n");
        return BVIEW_STATUS_FAILURE;
      }
    }
    else
    {
      return BVIEW_STATUS_FAILURE;
    } 
  }

  if (fcntl(sockFd, F_SETFL, (flags & ~(O_NONBLOCK)) < 0))
  {
    _SYSTEM_UTILS_LOG(_SYSTEM_UTILS_DEBUG_ERROR, "Error Setting client socket to blocking \r \n");
  }
  return BVIEW_STATUS_SUCCESS;
}

/*************************************************************************
 * @brief      Performs a socket send operation in a non-blocking mode 
 *
 * @param[in]  sockfd     Connecting socket identifier.
 * @param[in]  buf        Data buffer pointer
 * @param[in]  length     Data buffer length. 
 * &param[in]  sendFlags  Send operation flags.
 *
 * @retval     BVIEW_STATUS_SUCCESS  Connection established successfully.
 *             BVIEW_STATUS_FAILURE  Failed to establish connection
 * @note     
 **************************************************************************/
BVIEW_STATUS system_utils_non_block_send(int sockFd, const void *buf, int length, int sendFlags)
{
  fd_set  clientSet;
  struct  timeval timeout;
  int     flags, result, send_len, total_sent;

  total_sent = 0;

  flags = fcntl(sockFd, F_GETFL, 0);
  if (flags < 0)
  {
    _SYSTEM_UTILS_LOG(_SYSTEM_UTILS_DEBUG_ERROR, "Error Reading client socket\r\n");
    return BVIEW_STATUS_FAILURE;
  }

  if (fcntl(sockFd, F_SETFL, flags | O_NONBLOCK) < 0) 
  {   
    _SYSTEM_UTILS_LOG(_SYSTEM_UTILS_DEBUG_ERROR, "Error Setting client socket to non-blocking\r\n");
    return BVIEW_STATUS_FAILURE;
  }   

  FD_ZERO(&clientSet);
  FD_SET(sockFd, &clientSet);
  timeout.tv_sec = SYSTEM_UTILS_SEND_WAIT_TIME_SEC;
  timeout.tv_usec = SYSTEM_UTILS_SEND_WAIT_TIME_USEC;

  /* send to the peer */
  do  
  {   
    send_len = send(sockFd, (buf + total_sent), length, 0); 
    if (send_len < 0)
    {   
      /* buffer is full, we may block */
      if (errno == EAGAIN || errno == EWOULDBLOCK)
      {   
        /* wait for some time, or get notified when sent */
        result = select(sockFd + 1, NULL, &clientSet, NULL, &timeout);
        if (result <= 0)  
        {
          _SYSTEM_UTILS_LOG(_SYSTEM_UTILS_DEBUG_INFO, "Sending failed on socket=%d result=%d\r\n",
                    sockFd, result);
          return BVIEW_STATUS_FAILURE;
        }
        continue;
      }   
      else
      {   
        return BVIEW_STATUS_FAILURE;
      }   
    }   
    total_sent = send_len + total_sent;
    length = length - send_len;
  } while(length > 0);


  if (fcntl(sockFd, F_SETFL, (flags & ~(O_NONBLOCK)) < 0))
  {
    _SYSTEM_UTILS_LOG(_SYSTEM_UTILS_DEBUG_ERROR, "Error Setting client socket to blocking\r\n");
  }

  return BVIEW_STATUS_SUCCESS;
}

