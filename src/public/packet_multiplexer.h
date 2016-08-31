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

#ifndef INCLUDE_PACKET_MULTIPLEXER_H
#define INCLUDE_PACKET_MULTIPLEXER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "broadview.h"
#include "asic.h"
#include "port_utils.h"

#define     BVIEW_MAX_PACKET_SIZE                1558 
#define     BVIEW_PKT_MUX_MAX_FUNC_NAME          256
#define     BVIEW_PT_MAX_PROFILE_REQUESTS          5

typedef struct _bview_packet_
{
  unsigned char     data[BVIEW_MAX_PACKET_SIZE];
  unsigned int      asic;
  unsigned int      pkt_len;
  unsigned int      source_port;
  BVIEW_PORT_MASK_t dst_port_mask;
} BVIEW_PACKET_t;


typedef struct _bview_packet_msg_
{
  unsigned int      appMask;
  BVIEW_PACKET_t    packet;
  int               ltcRequestId;
} BVIEW_PACKET_MSG_t;


/* The callback for invoking when a configured trigger goes off */
typedef BVIEW_STATUS(*BVIEW_PKT_MUX_CALLBACK_t) (
        BVIEW_PACKET_MSG_t *packet);

typedef struct pktMuxRegister_s
{
  unsigned int appID;                             /* application ID*/
  char         appFuncName[BVIEW_PKT_MUX_MAX_FUNC_NAME]; /* application function name */  
  BVIEW_PKT_MUX_CALLBACK_t callbackFunc;           /* Function to be called at specified hook */
} pktMuxRegister_t;



/*********************************************************************
* @brief  Initialize the Packet Multiplexer hook structure
*
* @param    none
*
* @returns  BVIEW_STATUS_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
BVIEW_STATUS pkt_mux_init();

/*********************************************************************
* @brief  Register for a Packet
*
* @param    pkt_mux_hook @b{(input)} Pointer to application registration info
*
* @returns  BVIEW_STATUS_SUCCESS    if registration is successful
* @returns  BVIEW_STATUS_DUPLICATE  if application is already registered
* @returns  BVIEW_STATUS_FAILURE    if packet multiplexer is not initialized
*
*
* @notes    none
*
* @end
*********************************************************************/
BVIEW_STATUS pkt_mux_register(pktMuxRegister_t *pkt_mux_hook);

/*********************************************************************
* @brief  De-Register with packet multiplexer
*
* @param    pkt_mux_hook @b{(input)} Pointer to application registration info
*
* @returns  BVIEW_STATUS_SUCCESS  if successful de-registation
* @returns  BVIEW_STATUS_FAILURE  if 1) hooks uninitialized,
*                                    2) or no registation present
*
* @notes    none
*
* @end
*********************************************************************/
BVIEW_STATUS pkt_mux_deregister (pktMuxRegister_t *pkt_mux_hook);

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_PACKET_MULTIPLEXER_H */

