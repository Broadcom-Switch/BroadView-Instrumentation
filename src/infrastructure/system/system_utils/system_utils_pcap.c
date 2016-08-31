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
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "broadview.h"
#include "openapps_log_api.h"
#include "system_utils_pcap.h" 
#include "system_utils_base64.h"

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
                            unsigned int max_len, unsigned int *fileLength)
{
  int fd = 0;
  int tempLen = 0;
  char tempBuf[1024] = {0}; 
  
  if ((fd = open(pcapFileName, O_RDONLY)) < 0)
  {
    LOG_POST (BVIEW_LOG_ERROR, "Failed to open file %s\n", pcapFileName);
    return BVIEW_STATUS_FAILURE;
  }
  
  *fileLength = 0;
  while ((tempLen = read(fd, tempBuf, 1024)) > 0)
  {
    if ((*fileLength + tempLen) > max_len)
    {
      LOG_POST (BVIEW_LOG_ERROR,"PCAP file is larger than the max supported file length %u\n", max_len);
      close(fd);
      return -1;
    }
    memcpy((pcapFileData + *(fileLength)), tempBuf, tempLen);
    *fileLength += tempLen;  
  }
  close(fd);
  return BVIEW_STATUS_SUCCESS; 
}

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
                                   unsigned int *b64DataLength)
{
  return system_base64_encode((unsigned char *)pcapFileData, fileLength, pcapFileB64Data, 
                             max_len, b64DataLength); 
}

#if SYSTEM_WRITE_PCAP_FILE 
/******************************************************************
 * @brief     Prepare a PCAP file from PCAP buffer.
 *
 * @param[in]   pkt_offset  Buffer of PCAP data
 *              out_len     Size of PCAP data  
 *
 * @retval   0   When encoding is succesfull
 *           -1  When fails to encode
 * @note     
 *********************************************************************/
int system_writetopcap(char *pkt_offset, int out_len)
{
	int name = rand();
	char filename[64] = {0};
	FILE *fp;
	char *buffPtr = NULL;

	sprintf(filename, "%d.pcap",name);
	printf("FILE NAME is %s\n", filename);
	fp = fopen(filename, "w+");
	buffPtr = pkt_offset;
	if (fp)
	{
		int len = out_len;
		int offset = 0;
		while(1)
		{
			offset += fwrite(buffPtr+offset, 1, len, fp);
			len -= offset;
			if (len <= 0)
				break;
		}
		fclose (fp);
	}
    return 0;
}
#endif
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
                                   unsigned int *fileLength)
{
   SYSTEM_PCAP_HDR_t      pcapHdr;  
   SYSTEM_PCAPREC_HDR_t   pcapPktHdr;
   struct timespec       currentTime;
   unsigned int          offset = 0;

   
   /* Check if there is enough space to hold the pcap file */
   if ((pktLen + sizeof(SYSTEM_PCAP_HDR_t) + sizeof(SYSTEM_PCAPREC_HDR_t)) 
	                                         > maxLen) 
   {
     LOG_POST (BVIEW_LOG_ERROR, "Buffer space is not enough to hold PCAP file \n");  
     return BVIEW_STATUS_FAILURE;
   }

   pcapHdr.magic_number = SYSTEM_PCAP_MAGIC_NUMBER;
   pcapHdr.version_major = SYSTEM_PCAP_VERSION_NUM_MAJOR;
   pcapHdr.version_minor = SYSTEM_PCAP_VERSION_NUM_MINOR;
   pcapHdr.thiszone = SYSTEM_PCAP_THIS_ZONE;
   pcapHdr.sigfigs  = SYSTEM_PCAP_SIGFIGS;
   pcapHdr.snaplen = SYSTEM_PCAP_SNAPLEN;
   pcapHdr.network = SYSTEM_PCAP_NETWORK;

   clock_gettime(CLOCK_REALTIME, &currentTime);

   pcapPktHdr.ts_sec = currentTime.tv_sec;
   pcapPktHdr.ts_usec = (currentTime.tv_nsec/1000);
   pcapPktHdr.incl_len = pktLen;
   pcapPktHdr.orig_len = pktLen;


   offset = 0;
   memcpy((pcapFileData+offset), &pcapHdr, sizeof(pcapHdr));

   offset += sizeof(pcapHdr);
   memcpy((pcapFileData+offset), &pcapPktHdr, sizeof(pcapPktHdr));

   offset += sizeof(pcapPktHdr);
   memcpy((pcapFileData+offset), pktData, pktLen);

   *fileLength = (pktLen + sizeof(SYSTEM_PCAP_HDR_t) + sizeof(SYSTEM_PCAPREC_HDR_t));
#if SYSTEM_WRITE_PCAP_FILE 
   system_writetopcap(pcapFileData, *fileLength);
#endif
   return BVIEW_STATUS_SUCCESS;
}

