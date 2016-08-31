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

#ifndef INCLUDE_PACKET_MULTIPLEXER_DB_H
#define INCLUDE_PACKET_MULTIPLEXER_DB_H

#ifdef __cplusplus
extern "C"
{
#endif

#include  "broadview.h"
#include  "packet_multiplexer.h"

#define        BVIEW_PKT_MUX_MAX_HOOKS            5

typedef struct _pktMuxHooks_
{
  unsigned int appID;         
  char         appFuncName[BVIEW_PKT_MUX_MAX_FUNC_NAME]; /* application function name */  
  BVIEW_PKT_MUX_CALLBACK_t callbackFunc;        /* Function to be called at specified hook */
  unsigned int packetCount;
} pktMuxHooks_t;


typedef struct _pktMuxInfo_
{
  key_t key;
  /* message queue id for packet multiplexer */
  int recvMsgQid;
  /* pthread ID*/
  pthread_t pt_thread;
  /* Mutex for Packet Multiplexer*/
  pthread_mutex_t  pkt_mux_mutex;
   
  /* Database to Hold Application Registration Info*/
  pktMuxHooks_t   pktMuxHooks[BVIEW_PKT_MUX_MAX_HOOKS];
} pktMuxInfo_t;

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_PACKET_MULTIPLEXER_DB_H */

