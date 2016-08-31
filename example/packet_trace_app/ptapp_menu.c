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

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "ptapp.h"
#include "ptapp_base64.h"
#include "ptapp_pcap.h"
#include "ptapp_json.h"
#include "ptapp_menu.h"

#define MPLS_LABEL_DEF_NUMBER   1 
#define PTAPP_VXLAN_MAX_VNID          0xffffff
#define PTAPP_DEFAULT_VXLAN_L2_ENCAP_STR "no"
#define PTAPP_VXLAN_L2_ENCAP_DEFAULT   1


/* Structure that holds menu strings and target input (get) functions for 
 * different packet types                                                */
PTAPP_PKT_MENU_DATA_t pktMenuInfo[PKT_TYPE_MAX] = 
                                      {  {
                                           .pktType = PKT_TYPE_ETHERNET,
                                           .menuString = "L2 packet",
                                           .get_pkt_data_fn = get_l2ether_pkt_data,
                                         },
                                         {
                                           .pktType = PKT_TYPE_IP,
                                           .menuString = "IPv4 packet",
                                           .get_pkt_data_fn = get_ipv4_pkt_data,
                                         },
                                         {
                                           .pktType = PKT_TYPE_TCP,
                                           .menuString = "TCP packet",
                                           .get_pkt_data_fn = get_tcp_pkt_data,
                                         },
                                         {
                                           .pktType = PKT_TYPE_UDP,
                                           .menuString = "UDP packet",
                                           .get_pkt_data_fn = get_udp_pkt_data,
                                         },
                                         {
                                           .pktType = PKT_TYPE_IPv6,
                                           .menuString = "IPv6 packet",
                                           .get_pkt_data_fn = get_ipv6_pkt_data,
                                         },
                                         {
                                           .pktType = PKT_TYPE_TCPv6,
                                           .menuString = "TCPv6 packet",
                                           .get_pkt_data_fn = get_tcpv6_pkt_data,
                                         },
                                         {
                                           .pktType = PKT_TYPE_UDPv6,
                                           .menuString = "UDPv6 packet",
                                           .get_pkt_data_fn = get_udpv6_pkt_data,
                                         },
                                         {
                                           .pktType = PKT_TYPE_IP_MULTICAST,
                                           .menuString = "IP Multicast packet",
                                           .get_pkt_data_fn = get_ipv4_multicast_pkt_data,
                                         },
                                         {
                                           .pktType = PKT_TYPE_IPv4_GRE,
                                           .menuString = "IPv4 GRE packet",
                                           .get_pkt_data_fn = get_ipv4_gre_pkt_data,
                                         },
                                         {
                                           .pktType = PKT_TYPE_IPv6_GRE,
                                           .menuString = "IPv6 GRE packet",
                                           .get_pkt_data_fn = get_ipv6_gre_pkt_data,
                                         },
                                         {
                                           .pktType = PKT_TYPE_IPv4_VXLAN,
                                           .menuString = "IPv4 VXLAN packet",
                                           .get_pkt_data_fn = get_ipv4_vxlan_pkt_data,
                                         },
                                         {
                                           .pktType = PKT_TYPE_IPv6_VXLAN,
                                           .menuString = "IPv6 VXLAN packet",
                                           .get_pkt_data_fn = get_ipv6_vxlan_pkt_data,
                                         },
                                      };


/******************************************************************
 * @brief     Function to convert string of hexadecimal characters to 
 *              hexadecimal data.
 *
 * @param[in]   hexstring      String of hexadecimal characters
 *              data           Buffer to hold hexa decimal data
 *              len            length of the hexadecimal data  
 *
 * @retval   0   When conversion is succesfull
 *           -1  When fails to convert
 * @note     
 *********************************************************************/
static int ptapp_hex_to_data_convert(const unsigned char *hexstring, 
                                     unsigned char *data, unsigned int len)
{
  unsigned const char *pos = hexstring;
  char *endptr;
  size_t count = 0;

  _PTAPP_LOG(_PTAPP_DEBUG_TRACE,
             "PTAPP : ptapp_hex_to_data_convert processing string is %s  \n", hexstring);
  if ((hexstring[0] == '\0') || (strlen((char *)hexstring) % 2)) 
  {
    //hexstring contains no data
    //or hexstring has an odd length
    printf("\t\t Input hex string is null (or) Of invalid length  \n");
    return -1;
  }

  for(count = 0; count < len; count++) 
  {
    char buf[5] = {'0', 'x', pos[0], pos[1], 0};

    data[count] = strtol(buf, &endptr, 0);
    pos += 2 * sizeof(char);
   
    if (endptr[0] != '\0') 
	{
       //non-hexadecimal character encountered
      _PTAPP_LOG(_PTAPP_DEBUG_ERROR,
                 "PTAPP : Non hexadecimal character enocuntered  \n");
	  return -1;
    }
  }
  return 0;
}

/******************************************************************
 * @brief      Read input string from user and convert it to integer
 *
 * @param[in]   dest           Buffer to hold the integer
 *
 * @retval   USER_INPUT_NONE    There is not input from user
 *           USER_INPUT_ERROR   Input is incorrect
 *           USER_INPUT_OK      Input is successfully read and converted to integer
 * @note     
 *********************************************************************/

static PTAPP_USER_INPUT_STATUS_t get_uint_from_user(unsigned int *dest)
{
  char inputData[1024] = {0};
  int originalLen = 0;
  
  while (1)
  {
    if (fgets(inputData, sizeof(inputData), stdin) != NULL)
    {
      /* fgets reads next line char(\n) as part of string */
      /* only \n is present */ 
      if (strlen(inputData) == 1)
      {
        _PTAPP_LOG(_PTAPP_DEBUG_TRACE,
                   "PTAPP : no integer input from user \n");
        return USER_INPUT_NONE;
      }
      originalLen = strlen(inputData);
      inputData[originalLen-1] = 0;

      if (1 == sscanf(inputData, "%u", dest))
      {
        fflush(stdin); 
        break;
      }
      else
      {
        fflush(stdin); 
        _PTAPP_LOG(_PTAPP_DEBUG_ERROR,
                   "PTAPP : invalid integer input from user \n");
        return USER_INPUT_ERROR;
      } 
    }
    return USER_INPUT_NONE;
  }
  
  _PTAPP_LOG(_PTAPP_DEBUG_TRACE,
             "PTAPP : user input is %u \n", *dest);
  return USER_INPUT_OK;
}

/******************************************************************
 * @brief      Read input string from user
 *
 * @param[in]   dest           Buffer to hold the string
 *              sizeOfDest     Max size of the buffer 
 *
 * @retval   USER_INPUT_NONE    There is not input from user
 *           USER_INPUT_ERROR   Input is incorrect
 *           USER_INPUT_OK      Input is successfully read. 
 * @note     
 *********************************************************************/
PTAPP_USER_INPUT_STATUS_t get_string_from_user(char *dest, int sizeOfDest)
{
  char inputData[PTAPP_MAX_USER_INPUT_STRING_SIZE] = {0};
  int originalLen = 0;
  
  if (sizeOfDest > sizeof(inputData))
  {
    return USER_INPUT_ERROR; 
  }

  while (1)
  {
    if (fgets(inputData, sizeof(inputData), stdin) != NULL)
    {
      /* fgets returns string with End of line and null character */
      if (strlen(inputData) == 1)
      {
        _PTAPP_LOG(_PTAPP_DEBUG_TRACE,
                   "PTAPP : no string input from user \n");
        return USER_INPUT_NONE;
      }
      originalLen = strlen(inputData);
      /* Replace EOL character with null character */
      inputData[originalLen-1] = 0;

      if (strlen(inputData) > (sizeOfDest-1))
      {
        printf("\t\t Maximum allowed is %u characters!\n", (sizeOfDest-1)); 
        fflush(stdin);
        return USER_INPUT_ERROR;
	  }
      strcpy(dest, inputData);
      fflush(stdin); 
      break;
    }
    _PTAPP_LOG(_PTAPP_DEBUG_TRACE,
               "PTAPP : no  string input from user \n");
    return USER_INPUT_NONE;
  }
  _PTAPP_LOG(_PTAPP_DEBUG_TRACE,
             "PTAPP : user input is %s \n", dest);
  return USER_INPUT_OK;
}

/******************************************************************
 * @brief      Get port list from user and convert it to "1","2",...
 *                 format
 *
 * @param[in]   portList           Buffer to hold the string
 *              size               Max size of the buffer 
 *
 * @retval   USER_INPUT_NONE    There is not input from user
 *           USER_INPUT_ERROR   Input is incorrect
 *           USER_INPUT_OK      Input is successfully read. 
 * @note     
 *********************************************************************/
PTAPP_USER_INPUT_STATUS_t get_port_list_from_user(char *portList, int size)
{
  char tempList[1024] = {0};
  char *token =  NULL;
  char tokenStr[16] = {0};
  const char s1[2] = ",";
  PTAPP_USER_INPUT_STATUS_t retVal; 

  retVal = get_string_from_user(tempList, sizeof(tempList));
  if (retVal != USER_INPUT_OK)
  {
    return retVal;
  }

  memset(portList, 0x00, size);

  token = strtok(tempList, s1);
  /* walk through other tokens */
  while( token != NULL )
  {
    sprintf (tokenStr, "\"%s\"", token);
    strcat(portList, tokenStr);
    token = strtok(NULL, s1);
    if (token != NULL)
    {
      strcat (portList, ",");
    }
  }
  return retVal;
}

/******************************************************************
 * @brief      Target function to read input from user to prepare 
 *             JSON buffer for config packet trace method. If input is not 
 *             provided for particular fields, default values would be taken 
 *             to prepare JSON 
 *
 * @param[in]   jsonFrmt       Format of the JSON buffer 
 *              finalJsonBuf   Buffer to hold the json message with user inputs 
 *
 * @retval   0    Successfully read user inputs and prepared json buffer
 *
 * @note     
 *********************************************************************/
int config_pt_feature(char *jsonFrmt, char *finalJsonBuf)
{
  char asicId[16]  = PTAPP_DEFAULT_ASIC_ID;
  unsigned int ptEnable = PTAPP_DEFAULT_PT_CONFIG;
  unsigned int jsonId = 0;
  PTAPP_USER_INPUT_STATUS_t retValue;

  printf("\t Provide asic Id[%s] :", asicId);
  get_string_from_user(asicId, sizeof(asicId));

  while (1)
  {
    printf("\t Enable Packet Trace[%u] :", ptEnable);
    retValue = get_uint_from_user(&ptEnable);
  
    if ((retValue == USER_INPUT_ERROR) || (ptEnable > 1))
    {
      ptEnable = PTAPP_DEFAULT_PT_CONFIG; 
      printf("\n\t Invalid Value!  Please enter correct value (0/1)\n");
      continue;
    }
    break;
  }
  
  jsonId = get_new_json_id();
  sprintf(finalJsonBuf, jsonFrmt, asicId, ptEnable, jsonId);
  printf("\n Request sent to Agent with Id %u\n", jsonId);
  return 0;
}

/******************************************************************
 * @brief      Function to verify MAC string  
 *
 * @param[in]  s   Holds the MAC address string to be verified
 *
 * @retval    true         If MAC string is correct   
 *            false        If MAC string is incorrect 
 *
 * @note     
 *********************************************************************/
static bool isMacAddress(char *s)
{
  int i = 0;

  for(i = 0; i < 17; i++) 
  {
    if(i % 3 != 2 && !isxdigit(s[i]))
    {
      _PTAPP_LOG(_PTAPP_DEBUG_ERROR, "PTAPP: "
                                    "invalid length/digit in mac address\n");
      return false;
    }
    if(i % 3 == 2 && s[i] != ':')
    {
      _PTAPP_LOG(_PTAPP_DEBUG_ERROR, "PTAPP: "
                                    "no correct ':'s in mac address\n");
      return false;
    }
  }
  if(s[17] != '\0')
  {
    _PTAPP_LOG(_PTAPP_DEBUG_ERROR, "PTAPP: "
                                  "invalid mac address\n");
    return false;
  }
  
  return true;
}

/******************************************************************
 * @brief      Function to convert MAC string to MAC value 
 *
 * @param[in]  macAddrStr  Holds the MAC address string to be converted
 * @param[in]  macAddr     To hold the MAC address
 *
 * @retval    0         If MAC string is correct   
 *           -1         If MAC string is incorrect 
 *
 * @note     
 *********************************************************************/
static int convertMacStrToMacValue(char *macAddrStr, macAddr_t *macAddr)
{
  int data[6];
  int i;

  if( (6 == sscanf(macAddrStr, "%x:%x:%x:%x:%x:%x", &data[0], &data[1], &data[2],
	                                              &data[3], &data[4], &data[5])) )
  {
    /* Convert to uint8_t */
    for( i = 0; i < 6; ++i )
    {
       macAddr->addr[i] = (uint8_t) data[i];
    }
    return 0;
  }
  else
  {
     /* Invalid mac address*/
    _PTAPP_LOG(_PTAPP_DEBUG_ERROR, "PTAPP: invalid mac address\n");
    return -1;
  }
}


/******************************************************************
 * @brief      Function to get MAC address from user 
 *
 * @param[in]  macAddr  To hold the MAC address
 *
 * @retval   USER_INPUT_NONE    There is not input from user
 *           USER_INPUT_ERROR   Input is incorrect
 *           USER_INPUT_OK      Input is successfully read. 
 *
 * @note     
 *********************************************************************/
PTAPP_USER_INPUT_STATUS_t get_mac_address_from_user(macAddr_t *macAddr)
{
  char macAddrStr[ETHERNET_MAC_ADDR_STR_LEN] = {0};
  PTAPP_USER_INPUT_STATUS_t  retValue;
  
  if ((retValue = get_string_from_user(macAddrStr, sizeof(macAddrStr))) 
	                                                  != USER_INPUT_OK)
  {
    return retValue;
  }
  else  
  {
    if (isMacAddress(macAddrStr) != true)
    {
      _PTAPP_LOG(_PTAPP_DEBUG_ERROR, "PTAPP: invalid mac address\n");
      return USER_INPUT_ERROR; 
    }
    convertMacStrToMacValue(macAddrStr, macAddr);
    return USER_INPUT_OK;
  }
}

/******************************************************************
 * @brief      Function to get IPv4 address from user 
 *
 * @param[in]  ipv6Addr  To hold the IPv4 address
 *
 * @retval   USER_INPUT_NONE    There is not input from user
 *           USER_INPUT_ERROR   Input is incorrect
 *           USER_INPUT_OK      Input is successfully read. 
 *
 * @note     
 *********************************************************************/
PTAPP_USER_INPUT_STATUS_t get_ipv4_address_from_user(unsigned int *ipAddr)
{
  char ipAddrStr[INET_ADDRSTRLEN] = {0};
  PTAPP_USER_INPUT_STATUS_t  retValue;

  if ((retValue = get_string_from_user(ipAddrStr, sizeof(ipAddrStr))) 
	                                                  != USER_INPUT_OK)
  {
    return retValue;
  }
  else  
  {
    if (inet_pton(AF_INET, ipAddrStr, ipAddr) != 1)
    {
      return USER_INPUT_ERROR; 
    }
    return USER_INPUT_OK;
  }
}

/******************************************************************
 * @brief      Function to get IPv6 address from user 
 *
 * @param[in]  ipv6Addr  To hold the IPv6 address
 *
 * @retval   USER_INPUT_NONE    There is not input from user
 *           USER_INPUT_ERROR   Input is incorrect
 *           USER_INPUT_OK      Input is successfully read. 
 *
 * @note     
 *********************************************************************/
PTAPP_USER_INPUT_STATUS_t get_ipv6_address_from_user(struct in6_addr *ipv6Addr)
{
  char ipv6AddrStr[INET6_ADDRSTRLEN] = {0};
  PTAPP_USER_INPUT_STATUS_t  retValue;

  if ((retValue = get_string_from_user(ipv6AddrStr, sizeof(ipv6AddrStr))) 
	                                                  != USER_INPUT_OK)
  {
    return retValue;
  }
  else  
  {
    if (inet_pton(AF_INET6, ipv6AddrStr, ipv6Addr) != 1)
    {
      return USER_INPUT_ERROR; 
    }
    return USER_INPUT_OK;
  }
}


/******************************************************************
 * @brief      Function to get user input for different IPv4 packet fields 
 *
 * @param[in]  pktHeader   Buffer to hold the packet header fields and 
 *                         corresponding boolean flags
 *
 * @retval    0         If user input is correct   
 *           -1         If user input is incorrect 
 *
 * @note     
 *********************************************************************/
int get_ipv4_pkt_data_input(PTAPP_PKT_TYPE_t type, ipHeader_t *ipHeader, ipBool_t *ipFlag)
{
  unsigned int srcIp;
  unsigned int destIp;
  PTAPP_USER_INPUT_STATUS_t  retValue; 
  unsigned int tempTos;
  
  while(1)
  {
    printf("\t\t Provide value for type of service:");
    retValue = get_uint_from_user(&tempTos);
    if (retValue == USER_INPUT_NONE)
    {
      ipFlag->tos_set = false;
      break;
    }
    if (retValue == USER_INPUT_OK)
    {
      if (tempTos < 256)
      {
        ipFlag->tos_set = true;
        ipHeader->iph_tos = (uint8_t)tempTos; 
        break;
      }
    }
    printf("\t\t Invalid Input! Max supported value is 255\n");
  }


  while (1)
  {
    printf("\t\t Provide source IP address(a.b.c.d format) [%s]:", 
                                        ((type==PKT_TYPE_IP_MULTICAST)?IPv4_DST_MCAST:IPv4_SRC_ADDR) );
    retValue = get_ipv4_address_from_user(&srcIp);
    if (retValue == USER_INPUT_NONE)
    {
      ipFlag->src_ip_set = false;
      break;
    }
    if (retValue == USER_INPUT_OK)
    {
      ipHeader->iph_src = srcIp;
      ipFlag->src_ip_set = true;
      break; 
    }
    printf("\t\t Invalid Input! Please provide correct value\n");
  }

  while (1)
  {
    printf("\t\t Provide destination IP address(a.b.c.d format) [%s]:", IPv4_DST_ADDR);
    retValue = get_ipv4_address_from_user(&destIp);
    if (retValue == USER_INPUT_NONE)
    {
      ipFlag->dst_ip_set = false;
      break;
    }
    if (retValue == USER_INPUT_OK)
    {
      ipHeader->iph_dst = destIp;
      ipFlag->dst_ip_set = true;
      break; 
    }
    printf("\t\t Invalid Input! Please provide correct value\n");
  }

  return 0;
}

/******************************************************************
 * @brief      Function to get user input for different IPv6 packet fields 
 *
 * @param[in]  pktHeader   Buffer to hold the packet header fields and 
 *                         corresponding boolean flags
 *
 * @retval    0         If user input is correct   
 *           -1         If user input is incorrect 
 *
 * @note     
 *********************************************************************/
int get_ipv6_pkt_data_input(struct ip6_hdr *ip6hdr, ipv6Bool_t *ipv6Flag)
{
  struct in6_addr srcIpv6;
  struct in6_addr destIpv6;
  PTAPP_USER_INPUT_STATUS_t  retValue; 
  
  while (1)
  {
    printf("\t\t Provide source IPv6 address(x:x:x:x:.. format)[%s]:", IPv6_SRC_ADDR);
    retValue = get_ipv6_address_from_user(&srcIpv6);
    if (retValue == USER_INPUT_NONE)
    {
      ipv6Flag->src_ipv6_set = false;
      break;
    }
    if (retValue == USER_INPUT_OK)
    {
      ip6hdr->ip6_src = srcIpv6;
      ipv6Flag->src_ipv6_set = true;
      break; 
    }
    printf("\t\t Invalid Input! Please provide correct value\n");
  }

  while (1)
  {
    printf("\t\t Provide destination IP address(x:x:x:x:.. format):[%s]", IPv6_DST_ADDR);
    retValue = get_ipv6_address_from_user(&destIpv6);
    if (retValue == USER_INPUT_NONE)
    {
      ipv6Flag->dst_ipv6_set = false;
      break;
    }
    if (retValue == USER_INPUT_OK)
    {
      ip6hdr->ip6_dst = destIpv6;
      ipv6Flag->dst_ipv6_set = true;
      break; 
    }
    printf("\t\t Invalid Input! Please provide correct value\n");
  }

  return 0;
}

/******************************************************************
 * @brief      Function to get user input for different TCP packet fields 
 *
 * @param[in]  pktHeader   Buffer to hold the packet header fields and 
 *                         corresponding boolean flags
 *
 * @retval    0         If user input is correct   
 *           -1         If user input is incorrect 
 *
 * @note     
 *********************************************************************/
int get_vxlan_pkt_data_input(vxlanHeader_t *vxlanHeader, vxlanBool_t *vxlanFlag)
{
  unsigned int vnid;
#if !(PTAPP_VXLAN_L2_ENCAP_DEFAULT) 
  char isL2encap[4] = PTAPP_DEFAULT_VXLAN_L2_ENCAP_STR;
#endif
  PTAPP_USER_INPUT_STATUS_t  retValue; 
  
  while(1)
  {
    vnid = VXLAN_VNID_DEFAULT;
    printf("\t\t Provide value for vnid[%d]:", VXLAN_VNID_DEFAULT);
    retValue = get_uint_from_user(&vnid);
    if (retValue == USER_INPUT_NONE)
    {
      vxlanFlag->vNID_set = false; 
      break;
    }
    if (retValue == USER_INPUT_OK)
    {
      if (vnid <= PTAPP_VXLAN_MAX_VNID)
      {
        vxlanFlag->vNID_set = true;
        vxlanHeader->vxlanVnid = vnid;
        break;
      }
    }
    printf("\t\t Invalid Input! Please provide correct value(<= %u)\n", VXLAN_VNID_DEFAULT);
  }

#if !(PTAPP_VXLAN_L2_ENCAP_DEFAULT) 

  while (1)
  {
    strcpy(isL2encap, PTAPP_DEFAULT_VXLAN_L2_ENCAP_STR);
    printf("\t\t Is L2 packet encapsulated(enter yes/no) [%s]:", PTAPP_DEFAULT_VXLAN_L2_ENCAP_STR);
    get_string_from_user(isL2encap, sizeof(isL2encap));
    if (!((strcmp(isL2encap, "yes") == 0) || (strcmp(isL2encap, "no") == 0))) 
    {
      printf("\t\t   Invalid input! Please enter yes/no \n");
      continue;
    }
    if (strcmp(isL2encap, "yes") == 0)
    {
      vxlanFlag->is_l2_encapsulated = true;
    }
    else if (strcmp(isL2encap, "no") == 0)
    {
      vxlanFlag->is_l2_encapsulated = false;
    }
    break;
  } 
#else
  vxlanFlag->is_l2_encapsulated = true;
#endif
  return 0;
}



/******************************************************************
 * @brief      Function to get user input for different TCP packet fields 
 *
 * @param[in]  pktHeader   Buffer to hold the packet header fields and 
 *                         corresponding boolean flags
 *
 * @retval    0         If user input is correct   
 *           -1         If user input is incorrect 
 *
 * @note     
 *********************************************************************/
int get_tcp_pkt_data_input(tcphdr_t *tcpHeader, tcpBool_t *tcpFlag)
{
  unsigned int srcPort;
  unsigned int destPort;
  PTAPP_USER_INPUT_STATUS_t  retValue; 
  
  while(1)
  {
    printf("\t\t Provide value for TCP source port[%u]:", TCP_DEFAULT_SRC_PORT);
    retValue = get_uint_from_user(&srcPort);
    if (retValue == USER_INPUT_NONE)
    {
      tcpFlag->tcpSrcPort_set = false;
      break;
    }
    if (retValue == USER_INPUT_OK)
    {
      if (srcPort <= 0xffff)
      {
        tcpFlag->tcpSrcPort_set = true;
        tcpHeader->th_sport = (unsigned short)srcPort; 
        break;
      }
    }
    printf("\t\t Invalid Input! Please provide correct value(<= %u)\n", 0xffff);
  }

  while(1)
  {
    printf("\t\t Provide value for TCP destination port[%u]:", TCP_DEFAULT_DST_PORT);
    retValue = get_uint_from_user(&destPort);
    if (retValue == USER_INPUT_NONE)
    {
      tcpFlag->tcpDstPort_set = false;
      break;
    }
    if (retValue == USER_INPUT_OK)
    {
      if (destPort <= 0xffff)
      {
        tcpFlag->tcpDstPort_set = true;
        tcpHeader->th_dport = (unsigned short)destPort; 
        break;
      }
    }
    printf("\t\t Invalid Input! Please provide correct value(<= %u)\n", 0xffff);
  }
  return 0;
}


/******************************************************************
 * @brief      Function to get user input for different UDP packet fields 
 *
 * @param[in]  pktHeader   Buffer to hold the packet header fields and 
 *                         corresponding boolean flags
 *
 * @retval    0         If user input is correct   
 *           -1         If user input is incorrect 
 *
 * @note     
 *********************************************************************/
int get_udp_pkt_data_input(PTAPP_PKT_TYPE_t type,
                           udphdr_t *udpHeader, udpBool_t *udpFlag)
{
  unsigned int srcPort;
  unsigned int destPort;
  PTAPP_USER_INPUT_STATUS_t  retValue; 
  
  while(1)
  {
    printf("\t\t Provide value for UDP source port[%u]:", UDP_DEFAULT_SRC_PORT);
    retValue = get_uint_from_user(&srcPort);
    if (retValue == USER_INPUT_NONE)
    {
      udpFlag->udpSrcPort_set = false;
      break;
    }
    if (retValue == USER_INPUT_OK)
    {
      if (srcPort <= 0xffff)
      {
        udpFlag->udpSrcPort_set = true;
        udpHeader->uh_sport = (unsigned short)srcPort; 
        break;
      }
    }
    printf("\t\t Invalid Input! Please provide correct value(<= %u)\n", 0xffff);
  }

  /* Destination port is fixed for VxLan */
  if (!((type == PKT_TYPE_IPv4_VXLAN) ||
        (type == PKT_TYPE_IPv6_VXLAN)))
  {
    while(1)
    {
      printf("\t\t Provide value for UDP destination port[%u]:", UDP_DEFAULT_DST_PORT);
      retValue = get_uint_from_user(&destPort);
      if (retValue == USER_INPUT_NONE)
      {
        udpFlag->udpDstPort_set = false;
        break;
      }
      if (retValue == USER_INPUT_OK)
      {
        if (destPort <= 0xffff)
        {
          udpFlag->udpDstPort_set = true;
          udpHeader->uh_dport = (unsigned short)destPort; 
          break;
        }
      }
      printf("\t\t Invalid Input! Please provide correct value(<= %u)\n", 0xffff);
    }
  }
  return 0;
}

/******************************************************************
 * @brief      Function to get user input for different MPLS packet fields 
 *
 * @param[in]  pktHeader   Buffer to hold the packet header fields and 
 *                         corresponding boolean flags
 *
 * @retval    0         If user input is correct   
 *           -1         If user input is incorrect 
 *
 * @note     
 *********************************************************************/
 
int get_mpls_pkt_data_input(mplshdr_t *mplsHeader, mplsLabelBool_t *mplsFlag)
{
  char isMplsPkt[16] = PTAPP_DEFAULT_MPLS_PKT_STR;
  char labelPresent[16] = PTAPP_MPLS_LABEL_DEF_PRESENCE;
  bool mplsPkt   = false;
  unsigned int labelNum = 0, label, exp;
  unsigned int numOfLabels = MPLS_LABEL_DEF_NUMBER;
  PTAPP_USER_INPUT_STATUS_t  retValue; 
   
  while (1)
  {
    strcpy(isMplsPkt, PTAPP_DEFAULT_MPLS_PKT_STR);
    printf("\t\t Is it an MPLS packet(enter yes/no) [%s]:", PTAPP_DEFAULT_MPLS_PKT_STR);
    get_string_from_user(isMplsPkt, sizeof(isMplsPkt));
    if (!((strcmp(isMplsPkt, "yes") == 0) || (strcmp(isMplsPkt, "no") == 0))) 
    {
      printf("\t\t   Invalid input! Please enter yes/no \n");
      continue;
    }
    if (strcmp(isMplsPkt, "yes") == 0)
    {
      mplsPkt = true;
    }
    else if (strcmp(isMplsPkt, "no") == 0)
    {
      mplsPkt = false;
      mplsFlag->noOfLabels = 0;
    }
    break;
  } 
  if (mplsPkt == true)
  {
    printf("\t\t Provide Number of Labels[%d]:", MPLS_LABEL_DEF_NUMBER);
    while(1)
    {
      retValue = get_uint_from_user(&numOfLabels);

      if ((retValue == USER_INPUT_ERROR) || (numOfLabels > MAX_MPLS_LABELS) 
                                         || (numOfLabels <= 0))
      {
        printf("\t\t\t Invalid Input! Please provide valid input"
                   "(min value is 1, max value is %u)\n", MAX_MPLS_LABELS);
        continue;
      }
      break;
    }   
    mplsFlag->noOfLabels = numOfLabels;

    for (labelNum = 1; labelNum <= numOfLabels; labelNum++)
    {
      while (1)
      {
        strcpy(labelPresent, PTAPP_MPLS_LABEL_DEF_PRESENCE); 
        printf("\t\t Do you want to provide values for Label%u [%s] (enter yes/no):", 
                                         	labelNum, PTAPP_MPLS_LABEL_DEF_PRESENCE);
        get_string_from_user(labelPresent, sizeof(labelPresent));
        if (!((strcmp(labelPresent, "yes") == 0) || (strcmp(labelPresent, "no") == 0))) 
        {
          printf("\t\t   Invalid input! Please enter yes/no \n");
          continue;
        }
        break;
      }
        
      if (strcmp(labelPresent, "no") == 0)
      {
        mplsFlag->mplsLabel_val_set[labelNum-1] = false;
        mplsFlag->mplsLabel_pri_set[labelNum-1] = false;
        /* continue the main for loop for more labels */ 
        continue;
      }

      while(1)
      {
        printf("\t\t\t Provide Label value for Label%u[%d]:", labelNum, MPLS_DEFAULT_LABEL); 
        if ((retValue = get_uint_from_user(&label)) == USER_INPUT_NONE)
        {
          mplsFlag->mplsLabel_val_set[labelNum-1] = false;
          break;
        }
        if ((retValue == USER_INPUT_ERROR) || (label > PTAPP_MPLS_MAX_LABEL_VAL))
        {
          printf("\t\t\t Invalid Input! Please provide valid input(max value is %u)\n", PTAPP_MPLS_MAX_LABEL_VAL);
          continue;
        }
        mplsFlag->mplsLabel_val_set[labelNum-1] = true;
        mplsHeader->label[labelNum-1] = label;
        break;
      } 
      while(1)
      {
        printf("\t\t\t Provide EXP for Label%u[%d]:", labelNum, MPLS_DEFAULT_PRI);  
        if ((retValue = get_uint_from_user(&exp)) == USER_INPUT_NONE)
        {
          mplsFlag->mplsLabel_pri_set[labelNum-1] = false;
          break;
        }
        if ((retValue == USER_INPUT_ERROR) || (exp > PTAPP_MPLS_MAX_EXP_VAL))
        {
          printf("\t\t\t Invalid Input! Please provide valid input(max val is %u)\n", PTAPP_MPLS_MAX_EXP_VAL);
          continue;
        }
        mplsFlag->mplsLabel_pri_set[labelNum-1] = true;
        mplsHeader->pri[labelNum-1] = (uint8_t)exp;
        break;
      } 
    }
  }
  else
  {
    mplsFlag->noOfLabels = 0; 
  }
  return 0;
}
/******************************************************************
 * @brief      Function to get user input for different L2 packet fields 
 *
 * @param[in]  pktHeader   Buffer to hold the packet header fields and 
 *                         corresponding boolean flags
 *
 * @retval    0         If user input is correct   
 *           -1         If user input is incorrect 
 *
 * @note     
 *********************************************************************/
int get_l2ether_pkt_data_input(PTAPP_PKT_TYPE_t type, 
                               ethernetHeader_t *ethHeader, ethBool_t *ethFlag,
                               enetTag_t *ethTag, enetTagBool_t *enetTagFlag,
                               mplshdr_t *mplsHeader, mplsLabelBool_t *mplsFlag)
{
  macAddr_t srcMac;
  macAddr_t destMac;
  PTAPP_USER_INPUT_STATUS_t  retValue; 
  unsigned int tci;
  
  while (1)
  {
    printf("\t\t Provide destination MAC address(aa:bb:cc:... format): ");
    retValue = get_mac_address_from_user(&destMac);
    if (retValue == USER_INPUT_NONE)
    {
      ethFlag->ethDstMac_set = false;
      break;
    }
    if (retValue == USER_INPUT_OK)
    {
      ethHeader->dest = destMac;
      ethFlag->ethDstMac_set = true;
      break; 
    }
    printf("\t\t Invalid Input! Please provide correct value\n");
  }

  while (1)
  {
    printf("\t\t Provide source MAC address(aa:bb:cc:... format): ");
    retValue = get_mac_address_from_user(&srcMac);
    if (retValue == USER_INPUT_NONE)
    {
      ethFlag->ethSrcMac_set = false;
      break;
    }
    if (retValue == USER_INPUT_OK)
    {
      ethHeader->src = srcMac;
      ethFlag->ethSrcMac_set = true;
      break; 
    }
    printf("\t\t Invalid Input! Please provide correct value\n");
  }
  
  printf("\t\t If it is a tagged packet,");
  while(1)
  {
    printf("\n\t\t\t Provide TCI(tag control information):");  
    if ((retValue = get_uint_from_user(&tci)) == USER_INPUT_NONE)
    {
      printf("\t\t\t This is an un-tagged packet\n");
      enetTagFlag->ethTagTci_set = false;
      break;
    }
    if ((tci > 0xffff) || (retValue == USER_INPUT_ERROR))
    {
      printf("\t\t\t Invalid Input! Please provide valid input\n");
      continue;
    }
    enetTagFlag->ethTagTci_set = true;
    ethTag->tci = (unsigned short) tci; 
    break;
  }

  if (!((type == PKT_TYPE_IPv4_VXLAN) ||
        (type == PKT_TYPE_IPv6_VXLAN)))
  {
    get_mpls_pkt_data_input(mplsHeader, mplsFlag);
  }

  return 0;
}

/******************************************************************
 * @brief      Function to get packet offset and value to be put in
 *               the offset from user 
 *
 * @param[in]   totalPktLen  Total packet length
 *              hexdata   Buffer to hold the hexadecimal format of the user 
 *                             input(value) 
 *              maxLen       Size of the hexdata buffer
 *              offset       Offset in the packet
 *              offsetLen    Length of the data in hexdata
 *
 * @retval    0         If user input is correct   
 *           -1         If user input is incorrect 
 *
 * @note     
 *********************************************************************/
int get_pkt_offset_and_value_from_user(unsigned int totalPktLen, char *hexdata, 
                    unsigned int maxLen, unsigned int *offset, unsigned int *offsetLen)
{
  PTAPP_USER_INPUT_STATUS_t retValue;
  char offsetValue[PTAPP_MAX_SUPPORTED_OFFSETVALUE_LEN] = {0};

  while (1)
  {
    *offset = 0;
    printf("\t\t Provide offset(if you wish to change packet content using offset):");  
    retValue = get_uint_from_user(offset);
    if (retValue == USER_INPUT_ERROR)
    {
      printf("\t\t Invalid Input! Please provide correct value\n");
      continue; 
    }

    if (*offset > totalPktLen)
    {
      printf("\t\t Offset exceeds total length of the packet %u! Please provide correct value\n", totalPktLen);
      continue; 
    }

    if (retValue == USER_INPUT_OK)
    {
      while(1)
      {
        *offsetLen = 0;
        printf("\t\t Provide Value(Hexadecimal(even length) string in aabbcc1d232345... format, max 2046 characters:)");
        if (get_string_from_user(offsetValue, sizeof(offsetValue)) != USER_INPUT_NONE)
        {
          if (((strlen(offsetValue)/2) > maxLen))
          {
            printf("\t\t Length of input is larger than the supported! Please provide correct value\n"); 
            continue;
          }
          *offsetLen = (strlen(offsetValue)/2);
          if ((ptapp_hex_to_data_convert((unsigned char *)offsetValue, (unsigned char *)hexdata, *offsetLen) == -1))
          {
            printf("\t\t Invalid Input! Please provide correct value\n");
            continue;
          }
          /* Last 4 bytes indicate CRC, so the data that is allowed to be manipulated is (totalPktLen-4) */
		  if (((*offset)+(*offsetLen)) > (totalPktLen-4))
          {
            printf("\t\t Offset+length of value exceeds total allowed length(%u)!\n" 
                            "\t\t Please provide correct value\n", totalPktLen-4);
            continue;
          }
        }
        break;
      }
    }
    break;
  }
  return 0;
}

/******************************************************************
 * @brief      Function to get Total length of the packet from user 
 *
 * @param[in]  userInputPktLen  To hold the length of the packet that
 *                              user has provided
 *
 * @retval    0         If user input is correct   
 *           -1         If user input is incorrect 
 *
 * @note     
 *********************************************************************/
static int get_pkt_len_from_user(unsigned int *userInputPktLen)
{
  while (1)
  {
    printf("\t\t Provide packet Total length(%u <= length <= %u)[%u]:", 
                         PTAPP_MIN_SUPPORTED_PKT_SIZE, 
                         PTAPP_MAX_SUPPORTED_PKT_SIZE, PTAPP_DEFAULT_PKT_LEN);
    if (get_uint_from_user(userInputPktLen) == USER_INPUT_ERROR)
    {
      printf("\t\t Invalid Input! Please provide correct value\n");
      continue; 
    }
    if ((PTAPP_MAX_SUPPORTED_PKT_SIZE < *userInputPktLen) || 
        (PTAPP_MIN_SUPPORTED_PKT_SIZE > *userInputPktLen))
    {
      printf("\t\t Invalid Length! Length should be between (%u - %u) \n", 
                           PTAPP_MIN_SUPPORTED_PKT_SIZE, PTAPP_MAX_SUPPORTED_PKT_SIZE);
      continue; 
    }

    break;
  }
  return 0;
} 

/******************************************************************
 * @brief      Function to get user input to prepare L2 packet, 
 *             and prepares the packet. 
 *
 * @param[in]  pktData    Buffer to hold the packet
 *             maxLen     Size of the buffer
 *             pktLen     Length of the packet in the buffer
 *
 * @retval    0         If user input is correct and packet is prepared successfully  
 *           -1         If user input is incorrect or failed to prepare packet
 *
 * @note     
 *********************************************************************/
int get_l2ether_pkt_data(char *pktData, unsigned int maxLen, int *pktLen)
{
  pktHeader_t pktHeader;
  unsigned int userInputPktLen = PTAPP_DEFAULT_PKT_LEN;
  unsigned int offset = 0;
  unsigned int offsetLen = 0;
  char hexdata[1024] = {0};

  memset(&pktHeader, 0x00, sizeof(pktHeader));
  get_l2ether_pkt_data_input(PKT_TYPE_ETHERNET,
                             &pktHeader.ethHeader, &pktHeader.ethFlag, 
                             &pktHeader.ethTag, &pktHeader.enetTagFlag,
                             &pktHeader.mplsHeader, &pktHeader.mplsFlag);
  get_pkt_len_from_user(&userInputPktLen);
  get_pkt_offset_and_value_from_user(userInputPktLen, hexdata, sizeof(hexdata), &offset, &offsetLen);
  *pktLen = userInputPktLen;

  return eth_packet_create(PKT_TYPE_ETHERNET, &pktHeader, pktData, *pktLen, hexdata, offset, offsetLen);
}

/******************************************************************
 * @brief      Function to get user input to prepare IPv4 packet, 
 *             and prepares the packet. 
 *
 * @param[in]  pktData    Buffer to hold the packet
 *             maxLen     Size of the buffer
 *             pktLen     Length of the packet in the buffer
 *
 * @retval    0         If user input is correct and packet is prepared successfully  
 *           -1         If user input is incorrect or failed to prepare packet
 *
 * @note     
 *********************************************************************/
int get_ipv4_pkt_data(char *pktData, unsigned int maxLen, int *pktLen)
{
  pktHeader_t pktHeader;
  unsigned int userInputPktLen = PTAPP_DEFAULT_PKT_LEN;
  unsigned int offset = 0;
  unsigned int offsetLen = 0;
  char hexdata[1024] = {0};
  

  memset(&pktHeader, 0x00, sizeof(pktHeader));
  get_l2ether_pkt_data_input(PKT_TYPE_IP,
                             &pktHeader.ethHeader, &pktHeader.ethFlag, 
                             &pktHeader.ethTag, &pktHeader.enetTagFlag,
                             &pktHeader.mplsHeader, &pktHeader.mplsFlag);
  get_ipv4_pkt_data_input(PKT_TYPE_IP, &pktHeader.ipHeader, &pktHeader.ipFlag);
  get_pkt_len_from_user(&userInputPktLen);
  get_pkt_offset_and_value_from_user(userInputPktLen, hexdata, sizeof(hexdata), &offset, &offsetLen);
  *pktLen = userInputPktLen;

  return eth_packet_create(PKT_TYPE_IP, &pktHeader, pktData, *pktLen, hexdata, offset, offsetLen);
}


/**********************************************************************
 * @brief      Function to get user input to prepare IPv4 multicast packet, 
 *             and prepares the packet. 
 *
 * @param[in]  pktData    Buffer to hold the packet
 *             maxLen     Size of the buffer
 *             pktLen     Length of the packet in the buffer
 *
 * @retval    0         If user input is correct and packet is prepared successfully  
 *           -1         If user input is incorrect or failed to prepare packet
 *
 * @note     
 *********************************************************************/
int get_ipv4_multicast_pkt_data(char *pktData, unsigned int maxLen, int *pktLen)
{
  pktHeader_t pktHeader;
  unsigned int userInputPktLen = PTAPP_DEFAULT_PKT_LEN;
  unsigned int offset = 0;
  unsigned int offsetLen = 0;
  char hexdata[1024] = {0};
  

  memset(&pktHeader, 0x00, sizeof(pktHeader));
  get_l2ether_pkt_data_input(PKT_TYPE_IP_MULTICAST,
                             &pktHeader.ethHeader, &pktHeader.ethFlag, 
                             &pktHeader.ethTag, &pktHeader.enetTagFlag,
                             &pktHeader.mplsHeader, &pktHeader.mplsFlag);
  
  get_ipv4_pkt_data_input(PKT_TYPE_IP_MULTICAST, &pktHeader.ipHeader, &pktHeader.ipFlag);
  get_pkt_len_from_user(&userInputPktLen);
  get_pkt_offset_and_value_from_user(userInputPktLen, hexdata, sizeof(hexdata), &offset, &offsetLen);
  *pktLen = userInputPktLen;

  return eth_packet_create(PKT_TYPE_IP_MULTICAST, &pktHeader, pktData, *pktLen, hexdata, offset, offsetLen);
}

/******************************************************************
 * @brief      Function to get user input to prepare TCP packet, 
 *             and prepares the packet. 
 *
 * @param[in]  pktData    Buffer to hold the packet
 *             maxLen     Size of the buffer
 *             pktLen     Length of the packet in the buffer
 *
 * @retval    0         If user input is correct and packet is prepared successfully  
 *           -1         If user input is incorrect or failed to prepare packet
 *
 * @note     
 *********************************************************************/
int get_tcp_pkt_data(char *pktData, unsigned int maxLen, int *pktLen)
{
  pktHeader_t pktHeader;
  unsigned int userInputPktLen = PTAPP_DEFAULT_PKT_LEN;
  unsigned int offset = 0;
  unsigned int offsetLen = 0;
  char hexdata[1024] = {0};
  

  memset(&pktHeader, 0x00, sizeof(pktHeader));
  get_l2ether_pkt_data_input(PKT_TYPE_TCP,
                             &pktHeader.ethHeader, &pktHeader.ethFlag, 
                             &pktHeader.ethTag, &pktHeader.enetTagFlag,
                             &pktHeader.mplsHeader, &pktHeader.mplsFlag);
  get_ipv4_pkt_data_input(PKT_TYPE_TCP, &pktHeader.ipHeader, &pktHeader.ipFlag);
  get_tcp_pkt_data_input(&pktHeader.tcpHeader, &pktHeader.tcpFlag);
  get_pkt_len_from_user(&userInputPktLen);
  get_pkt_offset_and_value_from_user(userInputPktLen, hexdata, sizeof(hexdata), &offset, &offsetLen);
  *pktLen = userInputPktLen;

  return eth_packet_create(PKT_TYPE_TCP, &pktHeader, pktData, *pktLen, hexdata, offset, offsetLen);
}

/******************************************************************
 * @brief      Function to get user input to prepare UDP packet, 
 *             and prepares the packet. 
 *
 * @param[in]  pktData    Buffer to hold the packet
 *             maxLen     Size of the buffer
 *             pktLen     Length of the packet in the buffer
 *
 * @retval    0         If user input is correct and packet is prepared successfully  
 *           -1         If user input is incorrect or failed to prepare packet
 *
 * @note     
 *********************************************************************/
int get_udp_pkt_data(char *pktData, unsigned int maxLen, int *pktLen)
{
  pktHeader_t pktHeader;
  unsigned int userInputPktLen = PTAPP_DEFAULT_PKT_LEN;
  unsigned int offset = 0;
  unsigned int offsetLen = 0;
  char hexdata[1024] = {0};
  

  memset(&pktHeader, 0x00, sizeof(pktHeader));
  get_l2ether_pkt_data_input(PKT_TYPE_UDP,
                             &pktHeader.ethHeader, &pktHeader.ethFlag, 
                             &pktHeader.ethTag, &pktHeader.enetTagFlag,
                             &pktHeader.mplsHeader, &pktHeader.mplsFlag);
  get_ipv4_pkt_data_input(PKT_TYPE_UDP, &pktHeader.ipHeader, &pktHeader.ipFlag);
  get_udp_pkt_data_input(PKT_TYPE_UDP,
                         &pktHeader.udpHeader, &pktHeader.udpFlag);

  get_pkt_len_from_user(&userInputPktLen);
  get_pkt_offset_and_value_from_user(userInputPktLen, hexdata, sizeof(hexdata), &offset, &offsetLen);
  *pktLen = userInputPktLen;

  return eth_packet_create(PKT_TYPE_UDP, &pktHeader, pktData, *pktLen, hexdata, offset, offsetLen);
}

/******************************************************************
 * @brief      Function to get user input to prepare IPv6 packet, 
 *             and prepares the packet. 
 *
 * @param[in]  pktData    Buffer to hold the packet
 *             maxLen     Size of the buffer
 *             pktLen     Length of the packet in the buffer
 *
 * @retval    0         If user input is correct and packet is prepared successfully  
 *           -1         If user input is incorrect or failed to prepare packet
 *
 * @note     
 *********************************************************************/
int get_ipv6_pkt_data(char *pktData, unsigned int maxLen, int *pktLen)
{
  pktHeader_t pktHeader;
  unsigned int userInputPktLen = PTAPP_DEFAULT_PKT_LEN;
  unsigned int offset = 0;
  unsigned int offsetLen = 0;
  char hexdata[1024] = {0};
  

  memset(&pktHeader, 0x00, sizeof(pktHeader));
  get_l2ether_pkt_data_input(PKT_TYPE_IPv6,
                             &pktHeader.ethHeader, &pktHeader.ethFlag, 
                             &pktHeader.ethTag, &pktHeader.enetTagFlag,
                             &pktHeader.mplsHeader, &pktHeader.mplsFlag);
  get_ipv6_pkt_data_input(&pktHeader.ip6hdr, &pktHeader.ipv6Flag);
  get_pkt_len_from_user(&userInputPktLen);
  get_pkt_offset_and_value_from_user(userInputPktLen, hexdata, sizeof(hexdata), &offset, &offsetLen);
  *pktLen = userInputPktLen;

  return eth_packet_create(PKT_TYPE_IPv6, &pktHeader, pktData, *pktLen, hexdata, offset, offsetLen);
}

/******************************************************************
 * @brief      Function to get user input to prepare TCPv6 packet, 
 *             and prepares the packet. 
 *
 * @param[in]  pktData    Buffer to hold the packet
 *             maxLen     Size of the buffer
 *             pktLen     Length of the packet in the buffer
 *
 * @retval    0         If user input is correct and packet is prepared successfully  
 *           -1         If user input is incorrect or failed to prepare packet
 *
 * @note     
 *********************************************************************/
int get_tcpv6_pkt_data(char *pktData, unsigned int maxLen, int *pktLen)
{
  pktHeader_t pktHeader;
  unsigned int userInputPktLen = PTAPP_DEFAULT_PKT_LEN;
  unsigned int offset = 0;
  unsigned int offsetLen = 0;
  char hexdata[1024] = {0};
  

  memset(&pktHeader, 0x00, sizeof(pktHeader));
  get_l2ether_pkt_data_input(PKT_TYPE_TCPv6,
                             &pktHeader.ethHeader, &pktHeader.ethFlag, 
                             &pktHeader.ethTag, &pktHeader.enetTagFlag,
                             &pktHeader.mplsHeader, &pktHeader.mplsFlag);
  get_ipv6_pkt_data_input(&pktHeader.ip6hdr, &pktHeader.ipv6Flag);
  get_tcp_pkt_data_input(&pktHeader.tcpHeader, &pktHeader.tcpFlag);
  get_pkt_len_from_user(&userInputPktLen);
  get_pkt_offset_and_value_from_user(userInputPktLen, hexdata, sizeof(hexdata), &offset, &offsetLen);
  *pktLen = userInputPktLen;

  return eth_packet_create(PKT_TYPE_TCPv6, &pktHeader, pktData, *pktLen, hexdata, offset, offsetLen);
}

/******************************************************************
 * @brief      Function to get user input to prepare UDPv6 packet, 
 *             and prepares the packet. 
 *
 * @param[in]  pktData    Buffer to hold the packet
 *             maxLen     Size of the buffer
 *             pktLen     Length of the packet in the buffer
 *
 * @retval    0         If user input is correct and packet is prepared successfully  
 *           -1         If user input is incorrect or failed to prepare packet
 *
 * @note     
 *********************************************************************/
int get_udpv6_pkt_data(char *pktData, unsigned int maxLen, int *pktLen)
{
  pktHeader_t pktHeader;
  unsigned int userInputPktLen = PTAPP_DEFAULT_PKT_LEN;
  unsigned int offset = 0;
  unsigned int offsetLen = 0;
  char hexdata[1024] = {0};
  

  memset(&pktHeader, 0x00, sizeof(pktHeader));
  get_l2ether_pkt_data_input(PKT_TYPE_UDPv6,
                             &pktHeader.ethHeader, &pktHeader.ethFlag, 
                             &pktHeader.ethTag, &pktHeader.enetTagFlag,
                             &pktHeader.mplsHeader, &pktHeader.mplsFlag);
  get_ipv6_pkt_data_input(&pktHeader.ip6hdr, &pktHeader.ipv6Flag);
  get_udp_pkt_data_input(PKT_TYPE_UDPv6,
                         &pktHeader.udpHeader, &pktHeader.udpFlag);
  get_pkt_len_from_user(&userInputPktLen);
  get_pkt_offset_and_value_from_user(userInputPktLen, hexdata, sizeof(hexdata), &offset, &offsetLen);
  *pktLen = userInputPktLen;

  return eth_packet_create(PKT_TYPE_UDPv6, &pktHeader, pktData, *pktLen, hexdata, offset, offsetLen);
}

/******************************************************************
 * @brief      Function to get user input to prepare GRE packet, 
 *             and prepares the packet. 
 *
 * @param[in]  pktData    Buffer to hold the packet
 *             maxLen     Size of the buffer
 *             pktLen     Length of the packet in the buffer
 *
 * @retval    0         If user input is correct and packet is prepared successfully  
 *           -1         If user input is incorrect or failed to prepare packet
 *
 * @note     
 *********************************************************************/
int get_ipv4_gre_pkt_data(char *pktData, unsigned int maxLen, int *pktLen)
{
  pktHeader_t pktHeader;
  unsigned int userInputPktLen = PTAPP_DEFAULT_PKT_LEN;
  unsigned int offset = 0;
  unsigned int offsetLen = 0;
  char hexdata[1024] = {0};
  char pktType[2] = {0}; 
  PTAPP_USER_INPUT_STATUS_t retValue;  

  memset(&pktHeader, 0x00, sizeof(pktHeader));
  get_l2ether_pkt_data_input(PKT_TYPE_IPv4_GRE, 
                             &pktHeader.ethHeader, &pktHeader.ethFlag, 
                             &pktHeader.ethTag, &pktHeader.enetTagFlag,
                             &pktHeader.mplsHeader, &pktHeader.mplsFlag);
  get_ipv4_pkt_data_input(PKT_TYPE_IPv4_GRE, &pktHeader.ipHeader, &pktHeader.ipFlag);

  while(1)
  {
    strcpy(pktType, "a"); 
    printf("\t  GRE IP packet type\n");
    printf("\t     a)IPv4\n");
    printf("\t     b)IPv6\n");
    printf("\t     Enter your choice[%s]:", pktType);
    retValue = get_string_from_user(pktType, sizeof(pktType));
    if ((retValue == USER_INPUT_ERROR) || 
        ((strcmp(pktType,"a") != 0) && (strcmp(pktType,"b") != 0)))  
    {
      printf("\t   Invalid Input! Please provide correct value\n");
      continue;
    }
    break;
  }

  pktHeader.greFlag.ipv4 = false; 
  pktHeader.greFlag.ipv6 = false; 
  if (strcmp(pktType,"a") == 0)
  {
    pktHeader.greFlag.ipv4 = true; 
    printf("\t     Provide Encapsulated IPv4 packet details\n");
    get_ipv4_pkt_data_input(PKT_TYPE_IPv4_GRE, &pktHeader.greIpHeader, 
                                               &pktHeader.greIpFlag);
  }
  else if (strcmp(pktType,"b") == 0)
  {
    pktHeader.greFlag.ipv6 = true;
    printf("\t     Provide Encapsulated IPv6 packet details\n");
    get_ipv6_pkt_data_input(&pktHeader.greIp6hdr, &pktHeader.greIpv6Flag);
  }

  get_pkt_len_from_user(&userInputPktLen);
  get_pkt_offset_and_value_from_user(userInputPktLen, hexdata, sizeof(hexdata), &offset, &offsetLen);
  *pktLen = userInputPktLen;

  return eth_packet_create(PKT_TYPE_IPv4_GRE, &pktHeader, pktData, *pktLen, hexdata, offset, offsetLen);
}

/******************************************************************
 * @brief      Function to get user input to prepare GRE packet, 
 *             and prepares the packet. 
 *
 * @param[in]  pktData    Buffer to hold the packet
 *             maxLen     Size of the buffer
 *             pktLen     Length of the packet in the buffer
 *
 * @retval    0         If user input is correct and packet is prepared successfully  
 *           -1         If user input is incorrect or failed to prepare packet
 *
 * @note     
 *********************************************************************/
int get_ipv6_gre_pkt_data(char *pktData, unsigned int maxLen, int *pktLen)
{
  pktHeader_t pktHeader;
  unsigned int userInputPktLen = PTAPP_DEFAULT_PKT_LEN;
  unsigned int offset = 0;
  unsigned int offsetLen = 0;
  char hexdata[1024] = {0};
  char pktType[2] = {0}; 
  PTAPP_USER_INPUT_STATUS_t retValue;  

  memset(&pktHeader, 0x00, sizeof(pktHeader));
  get_l2ether_pkt_data_input(PKT_TYPE_IPv6_GRE,
                             &pktHeader.ethHeader, &pktHeader.ethFlag, 
                             &pktHeader.ethTag, &pktHeader.enetTagFlag,
                             &pktHeader.mplsHeader, &pktHeader.mplsFlag);
  get_ipv6_pkt_data_input(&pktHeader.ip6hdr, &pktHeader.ipv6Flag);

  while(1)
  {
    strcpy(pktType, "a"); 
    printf("\t  GRE IP packet type\n");
    printf("\t     a)IPv4\n");
    printf("\t     b)IPv6\n");
    printf("\t     Enter your choice[%s]:", pktType);
    retValue = get_string_from_user(pktType, sizeof(pktType));
    if ((retValue == USER_INPUT_ERROR) || 
        ((strcmp(pktType,"a") != 0) && (strcmp(pktType,"b") != 0)))  
    {
      printf("\t   Invalid Input! Please provide correct value\n");
      continue;
    }
    break;
  }

  pktHeader.greFlag.ipv4 = false; 
  pktHeader.greFlag.ipv6 = false; 
  if (strcmp(pktType,"a") == 0)
  {
    pktHeader.greFlag.ipv4 = true; 
    printf("\t     Provide Encapsulated IPv4 packet details\n");
    get_ipv4_pkt_data_input(PKT_TYPE_IPv6_GRE, &pktHeader.greIpHeader, &pktHeader.greIpFlag);
  }
  else if (strcmp(pktType,"b") == 0)
  {
    pktHeader.greFlag.ipv6 = true;
    printf("\t     Provide Encapsulated IPv6 packet details\n");
    get_ipv6_pkt_data_input(&pktHeader.greIp6hdr, &pktHeader.greIpv6Flag);
  }

  get_pkt_len_from_user(&userInputPktLen);
  get_pkt_offset_and_value_from_user(userInputPktLen, hexdata, sizeof(hexdata), &offset, &offsetLen);
  *pktLen = userInputPktLen;

  return eth_packet_create(PKT_TYPE_IPv6_GRE, &pktHeader, pktData, *pktLen, hexdata, offset, offsetLen);
}

/******************************************************************
 * @brief      Function to get user input to prepare GRE packet, 
 *             and prepares the packet. 
 *
 * @param[in]  pktData    Buffer to hold the packet
 *             maxLen     Size of the buffer
 *             pktLen     Length of the packet in the buffer
 *
 * @retval    0         If user input is correct and packet is prepared successfully  
 *           -1         If user input is incorrect or failed to prepare packet
 *
 * @note     
 *********************************************************************/
int get_ipv4_vxlan_pkt_data(char *pktData, unsigned int maxLen, int *pktLen)
{
  pktHeader_t pktHeader;
  unsigned int userInputPktLen = PTAPP_DEFAULT_PKT_LEN;
  unsigned int offset = 0;
  unsigned int offsetLen = 0;
  char hexdata[1024] = {0};

  memset(&pktHeader, 0x00, sizeof(pktHeader));
  get_l2ether_pkt_data_input(PKT_TYPE_IPv4_VXLAN,
                             &pktHeader.ethHeader, &pktHeader.ethFlag, 
                             &pktHeader.ethTag, &pktHeader.enetTagFlag,
                             &pktHeader.mplsHeader, &pktHeader.mplsFlag);
  get_ipv4_pkt_data_input(PKT_TYPE_IPv4_VXLAN, &pktHeader.ipHeader, &pktHeader.ipFlag);
  get_udp_pkt_data_input(PKT_TYPE_IPv4_VXLAN,
                         &pktHeader.udpHeader, &pktHeader.udpFlag);

  get_vxlan_pkt_data_input(&pktHeader.vxlanHeader, &pktHeader.vxlanFlag);

  if (pktHeader.vxlanFlag.is_l2_encapsulated == true)
  {
    get_l2ether_pkt_data_input(PKT_TYPE_IPv4_VXLAN, 
                               &pktHeader.vxlanEthHeader, &pktHeader.vxlanEthFlag, 
                               &pktHeader.vxlanEthTag, &pktHeader.vxlanEnetTagFlag,
                               NULL, NULL);
  }

  get_pkt_len_from_user(&userInputPktLen);
  get_pkt_offset_and_value_from_user(userInputPktLen, hexdata, sizeof(hexdata), &offset, &offsetLen);
  *pktLen = userInputPktLen;

  return eth_packet_create(PKT_TYPE_IPv4_VXLAN, &pktHeader, pktData, *pktLen, hexdata, offset, offsetLen);
}
/******************************************************************
 * @brief      Function to get user input to prepare GRE packet, 
 *             and prepares the packet. 
 *
 * @param[in]  pktData    Buffer to hold the packet
 *             maxLen     Size of the buffer
 *             pktLen     Length of the packet in the buffer
 *
 * @retval    0         If user input is correct and packet is prepared successfully  
 *           -1         If user input is incorrect or failed to prepare packet
 *
 * @note     
 *********************************************************************/
int get_ipv6_vxlan_pkt_data(char *pktData, unsigned int maxLen, int *pktLen)
{
  pktHeader_t pktHeader;
  unsigned int userInputPktLen = PTAPP_DEFAULT_PKT_LEN;
  unsigned int offset = 0;
  unsigned int offsetLen = 0;
  char hexdata[1024] = {0};

  memset(&pktHeader, 0x00, sizeof(pktHeader));
  get_l2ether_pkt_data_input(PKT_TYPE_IPv6_VXLAN,
                             &pktHeader.ethHeader, &pktHeader.ethFlag, 
                             &pktHeader.ethTag, &pktHeader.enetTagFlag,
                             &pktHeader.mplsHeader, &pktHeader.mplsFlag);
  get_ipv6_pkt_data_input(&pktHeader.ip6hdr, &pktHeader.ipv6Flag);
  get_udp_pkt_data_input(PKT_TYPE_IPv6_VXLAN, 
                         &pktHeader.udpHeader, &pktHeader.udpFlag);

  get_vxlan_pkt_data_input(&pktHeader.vxlanHeader, &pktHeader.vxlanFlag);

  if (pktHeader.vxlanFlag.is_l2_encapsulated == true)
  {
    get_l2ether_pkt_data_input(PKT_TYPE_IPv6_VXLAN,
                               &pktHeader.vxlanEthHeader, &pktHeader.vxlanEthFlag, 
                               &pktHeader.vxlanEthTag, &pktHeader.vxlanEnetTagFlag,
                               NULL, NULL);
  }

  get_pkt_len_from_user(&userInputPktLen);
  get_pkt_offset_and_value_from_user(userInputPktLen, hexdata, sizeof(hexdata), &offset, &offsetLen);
  *pktLen = userInputPktLen;

  return eth_packet_create(PKT_TYPE_IPv6_VXLAN, &pktHeader, pktData, *pktLen, hexdata, offset, offsetLen);
}
/******************************************************************
 * @brief      Function that provides main menu to get packet information 
 *             from user  
 *
 * @param[in]  pktData    Buffer to hold the packet
 *             maxLen     Size of the buffer
 *             pktLen     Length of the packet in the buffer
 *
 * @retval    0         If user input is correct and packet is prepared successfully  
 *           -1         If user input is incorrect or failed to prepare packet
 *
 * @note     
 *********************************************************************/
int get_pkt_input_from_user(char *pktData, unsigned int maxLen, int *pktLen)
{
   PTAPP_PKT_TYPE_t  tempPktType; 
   unsigned int      i;    
   unsigned int      option = 0;
   PTAPP_USER_INPUT_STATUS_t retValue;

   while(1)
   {
     option = 0;
     for (tempPktType = 0; tempPktType < PKT_TYPE_MAX; tempPktType++)   
     {
       for (i = 0; i < (sizeof(pktMenuInfo)/sizeof(PTAPP_PKT_MENU_DATA_t)); i++)
       {
         if (tempPktType == pktMenuInfo[i].pktType)
         {
           printf("\t\t %d.%s\n", tempPktType+1, pktMenuInfo[i].menuString);
           break;
         }
       }
     }

     printf("\t\t Enter your option:");
     retValue = get_uint_from_user(&option);
     if (retValue == USER_INPUT_ERROR)
     {
       printf("\t\t Invalid Input! Please enter correct option\n");
       continue;
     }

     if (retValue == USER_INPUT_NONE)
     {
       continue;
     }
     if ((option < (PKT_TYPE_MAX+1)) && (option != 0))
     {
       break;
     }

     printf("\t\t Invalid Input! Please enter correct option\n");
   }

   for (i = 0; i < (sizeof(pktMenuInfo)/sizeof(PTAPP_PKT_MENU_DATA_t)); i++)
   {
     if ((option-1) == pktMenuInfo[i].pktType)
     {
        return pktMenuInfo[i].get_pkt_data_fn(pktData, maxLen, pktLen);
     }
   }
   return -1;
}

/******************************************************************
 * @brief      Target function to read input from user to prepare 
 *             JSON buffer to get configuration of packet trace feature. 
 *             If input is not provided for particular fields, 
 *             default values would be taken to prepare JSON 
 *
 * @param[in]   jsonFrmt       Format of the JSON buffer 
 *              finalJsonBuf   Buffer to hold the json message with user inputs 
 *
 * @retval   0    Successfully read user inputs and prepared json buffer
 *
 * @note     
 *********************************************************************/
int get_pt_feature(char *jsonFrmt, char *finalJsonBuf)
{
  char asicId[16]  = PTAPP_DEFAULT_ASIC_ID;
  unsigned int jsonId = 0;

  printf("\t Provide asic Id[%s] :", asicId);
  get_string_from_user(asicId, sizeof(asicId));

  jsonId = get_new_json_id();
  sprintf(finalJsonBuf, jsonFrmt, asicId, jsonId);
  printf("\n Request sent to Agent with Id %u\n", jsonId);
  return 0;
}

/******************************************************************
 * @brief      Target function to read input from user to prepare 
 *             JSON buffer to get packet trace profile of a PCAP 
 *             formattted packet. If input is not provided for particular 
 *             fields, default values would be taken to prepare JSON 
 *
 * @param[in]   jsonFrmt       Format of the JSON buffer 
 *              finalJsonBuf   Buffer to hold the json message with user inputs 
 *
 * @retval   0    Successfully read user inputs and prepared json buffer
 *
 * @note     
 *********************************************************************/
int get_pt_profile(char *jsonFrmt, char *finalJsonBuf)
{
  char asicId[16] = PTAPP_DEFAULT_ASIC_ID;
  char choice[4] = {0};
  char pcapFileName[1024] = {0};
  char portList[4*1024] = {0};
  char tempPortList[1024] = PTAPP_DEFAULT_PORT_LIST;
  char dropPktStr[16] = PTAPP_DEFAULT_TRACE_PKT_DROP_CONFIG_STR;
  unsigned int collectionInterval = PTAPP_DEFAULT_COLLECTION_INTERVAL; 
  char pktData[PTAPP_MAX_SUPPORTED_PKT_SIZE] = {0};
  int       pktLen = 0;
  char pcapFileData[PTAPP_MAX_PCAP_FILE_SIZE] = {0};
  char pcapFileB64Data[PTAPP_MAX_B64_PCAP_FILE_SIZE] = {0};
  unsigned int fileLength = 0;  
  unsigned int b64DataLength = 0;
  unsigned int dropPkt = PTAPP_DEFAULT_TRACE_PKT_DROP_CONFIG;
  unsigned int jsonId = 0;

  printf("\t Provide asic Id[%s] :", asicId);
  get_string_from_user(asicId, sizeof(asicId));
  
  while (1)
  {
    printf("\t Do you want to \n\t   a)Provide pcap file  (OR) \n\t   b)Provide packet content"
                             "\n\t       Enter your choice:");
    get_string_from_user(choice, sizeof(choice));
    
    if (!((strcmp(choice,"a") == 0) || (strcmp(choice,"b") == 0))) 
    {
      printf("\n\t Invalid Input! Please provide correct input\n");
      continue;
    }
    break;
  }

  if ((strcmp(choice,"a") == 0))
  {
    while (1)
    {
      printf("\t Provide PCAP file (with complete path, max 1000 characters):");
      get_string_from_user(pcapFileName, sizeof(pcapFileName));
      if (pcapFileName[0] == 0)
      {
        printf("Invalid input! Please enter correct file name\n");
        continue;
      }
      break;
    }
    if (ptapp_read_pcap_file(pcapFileName, pcapFileData, (sizeof(pcapFileData)-1), 
                                                               &fileLength) == -1)
    {
      _PTAPP_LOG(_PTAPP_DEBUG_ERROR, "\nFailed to read PCAP file %s\n", pcapFileName);    
      return -1;
    }

    if (ptapp_convert_pcap_to_b64_data(pcapFileData, fileLength, pcapFileB64Data, 
                                         sizeof(pcapFileB64Data),  &b64DataLength) == -1)
    {
      _PTAPP_LOG(_PTAPP_DEBUG_ERROR, "\n Failed to encode PCAP data to base64\n");    
      return -1;
    }
  }
  else if ((strcmp(choice,"b") == 0))
  {
    /* Get Packet content from user */
    if (get_pkt_input_from_user(pktData, sizeof(pktData), &pktLen) == -1)
    {
      _PTAPP_LOG(_PTAPP_DEBUG_ERROR, "\n Failed to get packet content\n");    
       return -1;
    }
    /* Convert raw packet to PCAP format */ 
    ptapp_convert_raw_pkt_to_pcap_format(pktData, pktLen, pcapFileData, sizeof(pcapFileData), &fileLength);
    if (ptapp_convert_pcap_to_b64_data(pcapFileData, fileLength, pcapFileB64Data, 
                                         sizeof(pcapFileB64Data),  &b64DataLength) == -1)
    {
      _PTAPP_LOG(_PTAPP_DEBUG_ERROR, "\n Failed to encode PCAP data to base64\n");    
      return -1;
    }
  }
  
  printf("\t Provide port list[%s]:", tempPortList);
  sprintf (portList, "\"%s\"", tempPortList);
  get_port_list_from_user(portList, sizeof(portList));
 

  while (1)
  {
    printf("\t Provide collection interval[%u]:", collectionInterval);
    if (get_uint_from_user(&collectionInterval) == USER_INPUT_ERROR)
    {
      printf("\n\t Invalid Value!  Please enter correct value \n");
      continue;
    }
    break;
  }

  while (1)
  {
    printf("\t Drop the packet after trace[%s] (enter yes/no):", PTAPP_DEFAULT_TRACE_PKT_DROP_CONFIG_STR);
    get_string_from_user(dropPktStr, sizeof(dropPktStr));
    if (!((strcmp(dropPktStr,"yes") == 0) || (strcmp(dropPktStr,"no") == 0))) 
    {
      printf("\t Invalid input! Please enter yes/no \n");
      continue;
    }
    if (strcmp(dropPktStr,"yes") == 0)
    {
      dropPkt = TRACE_PKT_DROP;
    }
    else if (strcmp(dropPktStr,"no") == 0)
    {
      dropPkt = TRACE_PKT_FORWARD;
    }

    break;
  } 

  jsonId = get_new_json_id();
  sprintf(finalJsonBuf, jsonFrmt, asicId, pcapFileB64Data, portList, collectionInterval, dropPkt, jsonId);
  printf("\t Request sent to Agent with Id %u\n", jsonId);
  return 0;
}

/******************************************************************
 * @brief      Target function to read input from user to prepare 
 *             JSON buffer to get packet trace profile of a PCAP 
 *             formattted packet. If input is not provided for particular 
 *             fields, default values would be taken to prepare JSON 
 *
 * @param[in]   jsonFrmt       Format of the JSON buffer 
 *              finalJsonBuf   Buffer to hold the json message with user inputs 
 *
 * @retval   0    Successfully read user inputs and prepared json buffer
 *
 * @note     
 *********************************************************************/
int get_live_pt_profile(char *jsonFrmt, char *finalJsonBuf)
{
  char asicId[16] = PTAPP_DEFAULT_ASIC_ID;
  char portList[4*1024] = {0};
  char tempPortList[1024] = PTAPP_DEFAULT_PORT_LIST;

  unsigned int srcIp;
  unsigned int destIp;
  unsigned int tempTos; 
  unsigned int srcPort;
  unsigned int destPort;

  char defaultIpAddr[8] = "any";
  char defaultPort[8] = "any";
  char defaultTos[8]  = "any";

  
  char srcIpStr[32] = {0};
  char destIpStr[32] = {0}; 
  char tosStr[32] = {0}; 
  char srcPortStr[32] = {0};
  char destPortStr[32] = {0}; 

  struct in_addr ipAddr;

  char dropPktStr[16] = PTAPP_DEFAULT_TRACE_PKT_DROP_CONFIG_STR;
  unsigned int collectionInterval = PTAPP_DEFAULT_COLLECTION_INTERVAL; 
  unsigned int dropPkt = PTAPP_DEFAULT_TRACE_PKT_DROP_CONFIG;
  unsigned int packetLimit = 5; 
  unsigned int jsonId = 0;
  PTAPP_USER_INPUT_STATUS_t retValue; 
  printf("\t Provide asic Id[%s] :", asicId);
  get_string_from_user(asicId, sizeof(asicId));
  
  printf("\t Provide Ingress port list[%s]:", tempPortList);
  sprintf (portList, "\"%s\"", tempPortList);
  get_port_list_from_user(portList, sizeof(portList));
 
  while (1)
  {
    printf("\t Provide source IP address(a.b.c.d format) [%s]:", defaultIpAddr);
    retValue = get_ipv4_address_from_user(&srcIp);
    if (retValue == USER_INPUT_NONE)
    {
      strcpy(srcIpStr, defaultIpAddr);
      break;
    }
    if (retValue == USER_INPUT_OK)
    {
      ipAddr.s_addr = srcIp;
      sprintf(srcIpStr, "%s", inet_ntoa(ipAddr));
      break; 
    }
    printf("\t\t Invalid Input! Please provide correct value\n");
  }

  while (1)
  {
    printf("\t Provide destination IP address(a.b.c.d format) [%s]:", defaultIpAddr);
    retValue = get_ipv4_address_from_user(&destIp);
    if (retValue == USER_INPUT_NONE)
    {
      strcpy(destIpStr, defaultIpAddr);
      break;
    }
    if (retValue == USER_INPUT_OK)
    {
      ipAddr.s_addr = destIp;
      sprintf(destIpStr, "%s", inet_ntoa(ipAddr));
      break; 
    }
    printf("\t\t Invalid Input! Please provide correct value\n");
  }

  while(1)
  {
    printf("\t Provide value for protocol[%s]:", defaultTos);
    retValue = get_uint_from_user(&tempTos);
    if (retValue == USER_INPUT_NONE)
    {
      strcpy(tosStr, defaultTos); 
      break;
    }
    if (retValue == USER_INPUT_OK)
    {
      if (tempTos < 256)
      {
        sprintf(tosStr, "%d", tempTos);
        break;
      }
    }
    printf("\t\t Invalid Input! Max supported value is 255\n");
  }

  while(1)
  {
    printf("\t Provide value for source port[%s]:", defaultPort);
    retValue = get_uint_from_user(&srcPort);
    if (retValue == USER_INPUT_NONE)
    {
      strcpy(srcPortStr, defaultPort);
      break;
    }
    if (retValue == USER_INPUT_OK)
    {
      if (srcPort <= 0xffff)
      {
        sprintf(srcPortStr, "%d", srcPort);
        break;
      }
    }
    printf("\t\t Invalid Input! Please provide correct value(<= %u)\n", 0xffff);
  }

  while(1)
  {
    printf("\t Provide value for destination port[%s]:", defaultPort);
    retValue = get_uint_from_user(&destPort);
    if (retValue == USER_INPUT_NONE)
    {
      strcpy(destPortStr, defaultPort);
      break;
    }
    if (retValue == USER_INPUT_OK)
    {
      if (destPort <= 0xffff)
      {
        sprintf(destPortStr, "%d", destPort);
        break;
      }
    }
    printf("\t\t Invalid Input! Please provide correct value(<= %u)\n", 0xffff);
  }

  while (1)
  {
    printf("\t Provide collection interval[%u]:", collectionInterval);
    if (get_uint_from_user(&collectionInterval) == USER_INPUT_ERROR)
    {
      printf("\n\t Invalid Value!  Please enter correct value \n");
      continue;
    }
    break;
  }

  while (1)
  {
    printf("\t Provide packet-limit[%u]:", packetLimit);
    if (get_uint_from_user(&packetLimit) == USER_INPUT_ERROR)
    {
      printf("\n\t Invalid Value!  Please enter correct value \n");
      continue;
    }
    break;
  }

  while (1)
  {
    printf("\t Drop the packet after trace[%s] (enter yes/no):", PTAPP_DEFAULT_TRACE_PKT_DROP_CONFIG_STR);
    get_string_from_user(dropPktStr, sizeof(dropPktStr));
    if (!((strcmp(dropPktStr,"yes") == 0) || (strcmp(dropPktStr,"no") == 0))) 
    {
      printf("\t Invalid input! Please enter yes/no \n");
      continue;
    }
    if (strcmp(dropPktStr,"yes") == 0)
    {
      dropPkt = TRACE_PKT_DROP;
    }
    else if (strcmp(dropPktStr,"no") == 0)
    {
      dropPkt = TRACE_PKT_FORWARD;
    }

    break;
  } 

  jsonId = get_new_json_id();
  sprintf(finalJsonBuf, jsonFrmt, asicId, srcIpStr, destIpStr, tosStr, srcPortStr, destPortStr, portList, packetLimit, collectionInterval, dropPkt, jsonId);
  printf("\t Request sent to Agent with Id %u\n", jsonId);
  return 0;
}

#if DROP_REASON_SUPPORTED 
/******* TBD ************/
int config_pt_drop_reason(char *jsonFrmt, char *finalJsonBuf)
{
  char asicId[16];

  printf("\t Provide asic Id[1] :");
  scanf ("%s", asicId);
  return -1;
}
#endif

/******************************************************************
 * @brief      Target function to read input from user to prepare 
 *             JSON buffer to cancel packet trace profile request. 
 *             If input is not provided for particular fields, 
 *             default values would be taken to prepare JSON 
 *
 * @param[in]   jsonFrmt       Format of the JSON buffer 
 *              finalJsonBuf   Buffer to hold the json message with user inputs 
 *
 * @retval   0    Successfully read user inputs and prepared json buffer
 *
 * @note     
 *********************************************************************/
int cancel_pt_profile_request(char *jsonFrmt, char *finalJsonBuf)
{
  char asicId[16]  = PTAPP_DEFAULT_ASIC_ID;
  unsigned int traceProfileReqId = 0; 
  unsigned int jsonId = 0;

  printf("\t Provide asic Id[%s] :", asicId);
  get_string_from_user(asicId, sizeof(asicId));

  while (1)
  {
    printf("\t Trace Profile request id :");
    
    if (get_uint_from_user(&traceProfileReqId) == USER_INPUT_ERROR)
    {
      printf("\n\t Invalid Value!  Please enter correct value \n");
      continue;
    }
    
    if (traceProfileReqId  == 0)
    {
      printf("\n\t Invalid Value!  Please enter correct value \n");
      continue;
    }
    break;
  }
  
  jsonId = get_new_json_id();
  sprintf(finalJsonBuf, jsonFrmt, asicId, traceProfileReqId, jsonId);
  printf("\t Request sent to Agent with Id %u\n", jsonId);
  return 0;
}

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
void ptapp_get_user_input(PTAPP_REST_MSG_t *ptRestMessages, unsigned int *restMesgIndex)
{
  PTAPP_JSON_METHOD_t  displayMethod = 0;
  PTAPP_JSON_METHOD_t  selectedMethod = 0;
  unsigned int index = 0;
  unsigned int option;
  unsigned int numOfElements = 0;
  PTAPP_USER_INPUT_STATUS_t retValue;
 
  /* Display main menu options */
  numOfElements = sizeof(ptJsonMethodDetails)/sizeof(PTAPP_JSON_METHOD_INFO_t); 
  printf("\n\n");
  for (displayMethod = 0; displayMethod < PTAPP_JSON_METHOD_LAST; displayMethod++)
  {
    for (index = 0; index < numOfElements; index++)
    {
      if (ptJsonMethodDetails[index].methodId == displayMethod)
      {
        printf("%u.%s\n", (displayMethod+1), ptJsonMethodDetails[index].menuString); 
        break;
      }
    }
  }

  printf("%u.Exit\n", (PTAPP_JSON_METHOD_LAST+1));
  printf("Select an option from the menu:");
  retValue = get_uint_from_user(&option);

  if (retValue == USER_INPUT_ERROR)
  {
    printf("\n Invalid input! Please choose correct option \n");
    ptapp_get_user_input(ptRestMessages, restMesgIndex);
    return;
  }
 
  if (retValue == USER_INPUT_NONE)
  {
    ptapp_get_user_input(ptRestMessages, restMesgIndex);
    return;
  }

  if (option == (PTAPP_JSON_METHOD_LAST+1))
  {
    exit(0);
  }
  
  if (option > (PTAPP_JSON_METHOD_LAST+1))
  { 
    printf("\n Invalid Option %u! Please choose correct option \n", option);
    ptapp_get_user_input(ptRestMessages, restMesgIndex);
    return;
  }

  selectedMethod = (option - 1);
  memset(jsonBufGlobal, 0x00, sizeof(jsonBufGlobal));

  for (index = 0; index < PTAPP_JSON_METHOD_LAST; index++)
  {
    if (selectedMethod == ptJsonMethodDetails[index].methodId)
    {
      if (ptJsonMethodDetails[index].get_input_for_json(ptJsonMethodDetails[index].jsonFrmt, jsonBufGlobal) == -1)
      {
        ptapp_get_user_input(ptRestMessages, restMesgIndex);
        return;
      }
      break;
    }
  }
  
  for (index = 0; index < PTAPP_JSON_METHOD_LAST; index++)
  {
    if (selectedMethod == ptRestMessages[index].methodId)
    {
      ptRestMessages[index].json = jsonBufGlobal;
      *restMesgIndex = index;
      break;
    }
  }
  return;
}
