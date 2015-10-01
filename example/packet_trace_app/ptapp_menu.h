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
#ifndef INCLUDE_PTAPP_MENU_H
#define INCLUDE_PTAPP_MENU_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "ptapp.h"
#include "ptapp_base64.h"
#include "ptapp_pcap.h"
#include "ptapp_json.h"
#include "ptapp_pktlib.h"



typedef enum _ptapp_user_input_status_ {
  USER_INPUT_OK = 0,
  USER_INPUT_NONE,
  USER_INPUT_ERROR
} PTAPP_USER_INPUT_STATUS_t;

typedef struct _ptapp_pkt_menu_data_ {
   PTAPP_PKT_TYPE_t pktType;
   char menuString[1024];  
   int (*get_pkt_data_fn) (char *pktData, unsigned int maxLen, int *pktLen);
} PTAPP_PKT_MENU_DATA_t;

/******************************************************************
 * @brief      Function to provide (main)features menu to user  
 *
 * @param[in]   ptRestMessages  Buffer that holds the rest message for 
 *                              different methods. And the JSON buffer of
 *                              user selected method would get updated with
 *                              JSON string
 *
 *              restMesgIndex   User selected method  
 *
 * @retval   none   
 *
 * @note     
 *********************************************************************/
void ptapp_get_user_input(PTAPP_REST_MSG_t *ptRestMessages, unsigned int *restMesgIndex);

extern int get_l2ether_pkt_data(char *pktData, unsigned int maxLen, int *pktLen);
extern int get_ipv4_pkt_data(char *pktData, unsigned int maxLen, int *pktLen);
extern int get_ipv4_multicast_pkt_data(char *pktData, unsigned int maxLen, int *pktLen);
extern int get_tcp_pkt_data(char *pktData, unsigned int maxLen, int *pktLen);
extern int get_udp_pkt_data(char *pktData, unsigned int maxLen, int *pktLen);
extern int get_ipv6_pkt_data(char *pktData, unsigned int maxLen, int *pktLen);
extern int get_tcpv6_pkt_data(char *pktData, unsigned int maxLen, int *pktLen);
extern int get_udpv6_pkt_data(char *pktData, unsigned int maxLen, int *pktLen);
extern int get_ipv4_gre_pkt_data(char *pktData, unsigned int maxLen, int *pktLen);
extern int get_ipv6_gre_pkt_data(char *pktData, unsigned int maxLen, int *pktLen);
extern int get_ipv4_vxlan_pkt_data(char *pktData, unsigned int maxLen, int *pktLen);
extern int get_ipv6_vxlan_pkt_data(char *pktData, unsigned int maxLen, int *pktLen);

#ifdef	__cplusplus
}
#endif

#endif

