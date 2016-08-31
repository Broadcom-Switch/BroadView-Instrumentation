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

#ifndef INCLUDE_SYSTEM_UTILS_SOCK_H
#define	INCLUDE_SYSTEM_UTILS_SOCK_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include "broadview.h"



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
                                    const struct sockaddr *servAddr);


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
BVIEW_STATUS system_utils_non_block_send(int sockFd, const void *buf, int length, int sendFlags);

#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_SYSTEM_UTILS_SOCK_H */

