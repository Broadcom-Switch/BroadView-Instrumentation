/*****************************************************************************
  *
  * Copyright � 2016 Broadcom.  The term "Broadcom" refers
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

#ifndef  INCLUDE_SYSTEM_UTIL_PCAP_H
#define  INCLUDE_SYSTEM_UTIL_PCAP_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "system_utils_base64.h"

#define SYSTEM_PCAP_ETHERNET              0x1

#define SYSTEM_PCAP_MAGIC_NUMBER           0xa1b2c3d4
#define SYSTEM_PCAP_VERSION_NUM_MAJOR      0x2
#define SYSTEM_PCAP_VERSION_NUM_MINOR      0x4
#define SYSTEM_PCAP_THIS_ZONE              0x0
#define SYSTEM_PCAP_SIGFIGS                0x0
#define SYSTEM_PCAP_SNAPLEN                0xffff
#define SYSTEM_PCAP_NETWORK                SYSTEM_PCAP_ETHERNET  /* Ethernet */

typedef struct _system_pcap_hdr_ {
  unsigned int   magic_number;   /* magic number */
  unsigned short version_major;  /* major version number */
  unsigned short version_minor;  /* minor version number */
  int            thiszone;       /* GMT to local correction */
  unsigned int   sigfigs;        /* accuracy of timestamps */
  unsigned int   snaplen;        /* max length of captured packets, in octets */
  unsigned int   network;        /* data link type */
} SYSTEM_PCAP_HDR_t;

typedef struct _system_pcaprec_hdr_ {
  unsigned int ts_sec;         /* timestamp seconds */
  unsigned int ts_usec;        /* timestamp microseconds */
  unsigned int incl_len;       /* number of octets of packet saved in file */
  unsigned int orig_len;       /* actual length of packet */
} SYSTEM_PCAPREC_HDR_t;

#define SYSTEM_PCAP_ADD_LEN   (sizeof(SYSTEM_PCAP_HDR_t) + sizeof(SYSTEM_PCAPREC_HDR_t))

/******************************************************************
 * @brief     Reads a pcap file to a buffer.
 *
 * @param[in]   pcapFileName   PCAP file name
 *              pcapFileData   Buffer to hold the PCAP file data
 *              max_len        size of the pcapFileData  
 *              fileLength     actual size of the data in pcapFileData 
 *
 * @retval   BVIEW_STATUS_SUCCESS  When conversion is succesfull
 *           BVIEW_STATUS_FAILURE  When fails to convert
 * @note     
 *********************************************************************/
BVIEW_STATUS system_read_pcap_file(char *pcapFileName, char *pcapFileData, 
                            unsigned int max_len, unsigned int *fileLength);

/******************************************************************
 * @brief     Convert PCAP data to base64 encoded data.
 *
 * @param[in]   pcapFileData   Buffer of PCAP data
 *              fileLength     actual size of the data in pcapFileData 
 *              pcapFileB64Data   Buffer to hold encoded PCAP data
 *              max_len        size of the pcapFileB64Data  
 *              b64DataLength  size of the encoded data 
 *
 * @retval   BVIEW_STATUS_SUCCESS  When conversion is succesfull
 *           BVIEW_STATUS_FAILURE  When fails to convert
 * @note     
 *********************************************************************/
BVIEW_STATUS system_convert_pcap_to_b64_data(char *pcapFileData, unsigned int fileLength,
                                   char *pcapFileB64Data, unsigned int max_len,  
                                   unsigned int *b64DataLength);

/******************************************************************
 * @brief     Convert given packet to PCAP format.
 *
 * @param[in]   pktData       Packet data
 *              pktLen        Length of the Packet
                pcapFileData  Buffer to hold PCAP data
 *              max_len        size of the pcapFileData  
 *              fileLength     actual size of the data in pcapFileData 
 *
 * @retval   BVIEW_STATUS_SUCCESS  When conversion is succesfull
 *           BVIEW_STATUS_FAILURE  When fails to convert
 * @note     
 *********************************************************************/
BVIEW_STATUS system_convert_raw_pkt_to_pcap_format(char *pktData, unsigned int pktLen, 
                                   char *pcapFileData, unsigned int maxLen, 
                                   unsigned int *fileLength);
#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_SYSTEM_UTIL_PCAP_H */

