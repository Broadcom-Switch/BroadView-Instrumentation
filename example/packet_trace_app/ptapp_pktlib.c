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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ptapp_pktlib.h"
#include "ptapp_debug.h"

static int crc32_table_lookup[256];


#define PACK_BYTE(val, cp) (*(cp)++ = (char)(val))

#define PACK_SHORT(val, cp) \
       (val) = htons(val);\
        memcpy((cp), &(val), sizeof(short));\
       (cp) += sizeof(short);

#define PACK_LONG(val, cp) \
       (val) = htonl(val);\
        memcpy((cp), &(val), sizeof(int));\
        (cp) += sizeof(int);

#define PACK_MAC(val, cp) \
        memcpy((cp), (val), ETHERNET_MAC_ADDR_LEN);\
        (cp) += ETHERNET_MAC_ADDR_LEN;

#define PACK_N_BYTES(val, cp, num) \
        memcpy((cp), (val), (num));\
        (cp) += num;

#define swapInt(value) \
  ( (((value) >> 24) & 0x000000FF) | (((value) >> 8) & 0x0000FF00) | \
    (((value) << 8) & 0x00FF0000) | (((value) << 24) & 0xFF000000) )

int ethHeaderLen = ETHERNET_UNTAGEED_HEADER_LEN;
int ipHeaderLen = IP_HEADER_LEN;

#define CHECK_PKT_LEN(condition,errString) do{\
    if (!(condition)) { \
        _PTAPP_LOG(_PTAPP_DEBUG_TRACE, \
                    "PTAPP (%s:%d) %s \n", \
                    __func__, __LINE__, errString); \
        return;\
    }\
} while(0)


/*********************************************************************
* @brief        Function to create MPLS packet
*
* @param[in,out]    pktHeader Packet details
* @param[in,out]    l2DefPkt  Packet Buffer
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/

void default_mpls_create(pktHeader_t *pktHeader, char *l2DefPkt)
{
  int eos = 0, label = 0;
  uint8_t mpls_buf[MPLS_SIZE];
  pktHeader->ethHeader.etherType = MPLS_ETHER_TYPE;
  PACK_SHORT(pktHeader->ethHeader.etherType,l2DefPkt);
  for (label = 0;label < pktHeader->mplsFlag.noOfLabels;label++)
  {
    if (pktHeader->mplsFlag.mplsLabel_val_set[label] != true)
    {
      pktHeader->mplsHeader.label[label] = MPLS_DEFAULT_LABEL;
    }
    if (pktHeader->mplsFlag.mplsLabel_pri_set[label] != true)
    {
      pktHeader->mplsHeader.pri[label] = MPLS_DEFAULT_PRI;
    }
    memset(mpls_buf, 0, sizeof(mpls_buf));
    if (label == (pktHeader->mplsFlag.noOfLabels - 1))
    { 
      /*End of stack must be 1 for the last MPLS header in stack*/
      eos = END_OF_STACK;
    }
    create_mpls_header(pktHeader->mplsHeader.label[label],pktHeader->mplsHeader.pri[label], 
                       eos, MPLS_TTL, mpls_buf);
    PACK_N_BYTES(mpls_buf,l2DefPkt,4);
  }
}

/*********************************************************************
* @brief        Function to create L2 packet
*
* @param[in,out]    pktHeader Packet details
* @param[out]       l2DefPkt  Packet Buffer 
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/

void default_l2_packet_create(pktHeader_t *pktHeader, char *l2DefPkt)
{
  if (pktHeader->ethFlag.ethSrcMac_set != true)
  { 
    pktHeader->ethHeader.src.addr[0] = 0xAA; 
    pktHeader->ethHeader.src.addr[1] = 0xBB; 
    pktHeader->ethHeader.src.addr[2] = 0xCC; 
    pktHeader->ethHeader.src.addr[3] = 0xDD; 
    pktHeader->ethHeader.src.addr[4] = 0xEE; 
    pktHeader->ethHeader.src.addr[5] = 0xEF; 
  }
  if (pktHeader->ethFlag.ethDstMac_set != true)
  {
    pktHeader->ethHeader.dest.addr[0] = 0x11; 
    pktHeader->ethHeader.dest.addr[1] = 0x22; 
    pktHeader->ethHeader.dest.addr[2] = 0x33; 
    pktHeader->ethHeader.dest.addr[3] = 0x44; 
    pktHeader->ethHeader.dest.addr[4] = 0x55; 
    pktHeader->ethHeader.dest.addr[5] = 0x66;
  }
  PACK_MAC(pktHeader->ethHeader.dest.addr,l2DefPkt); 
  PACK_MAC(pktHeader->ethHeader.src.addr,l2DefPkt); 
  if (pktHeader->enetTagFlag.ethTagTci_set == true)
  {
    pktHeader->ethTag.tpid = VLAN_TPID;
    PACK_SHORT(pktHeader->ethTag.tpid,l2DefPkt);
    PACK_SHORT(pktHeader->ethTag.tci,l2DefPkt);
  }
  if ((pktHeader->mplsFlag.noOfLabels > 0) && (pktHeader->mplsFlag.noOfLabels <= MAX_MPLS_LABELS))
  {
    default_mpls_create(pktHeader,l2DefPkt);
  }
  else
  {
    PACK_SHORT(pktHeader->ethHeader.etherType,l2DefPkt);
  }
}

/*********************************************************************
* @brief            Function to create L3 packet
*
* @param[in,out]    pktHeader Packet details
* @param[out]       l3DefPkt  Packet Buffer 
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/

void default_l3_packet_create(PTAPP_PKT_TYPE_t type,pktHeader_t *pktHeader,char *l3DefPkt,int pktLen)
{
  char *pChecksum = NULL;
  short checksum;
  char *l3DefPktStart;
  ipHeader_t  *l3HeaderPtr = NULL;

  l3DefPktStart = l3DefPkt;
  l3HeaderPtr = (ipHeader_t *) l3DefPkt;
  CHECK_PKT_LEN((pktLen > (ethHeaderLen + ipHeaderLen)),IP_HEADER_ERROR);
  if (type == PKT_TYPE_IPv6)
  {
    pktHeader->ip6hdr.ip6_plen = pktLen - (ethHeaderLen + ipHeaderLen);
    pktHeader->ip6hdr.ip6_nxt = 59;
    default_l3V6_packet_create(pktHeader,l3DefPkt);
    return;
  }
  if (type == PKT_TYPE_TCPv6)
  {
    pktHeader->ip6hdr.ip6_plen = pktLen - (ethHeaderLen + ipHeaderLen);
    pktHeader->ip6hdr.ip6_nxt = IPPROTO_TCP;
    default_l3V6_packet_create(pktHeader,l3DefPkt);
    return;
  }
  if (type == PKT_TYPE_UDPv6)
  {
    pktHeader->ip6hdr.ip6_plen = pktLen - (ethHeaderLen + ipHeaderLen);
    pktHeader->ip6hdr.ip6_nxt = IPPROTO_UDP;
    default_l3V6_packet_create(pktHeader,l3DefPkt);
    return;
  }
  if (type == PKT_TYPE_IPv6_GRE)
  {
    pktHeader->ip6hdr.ip6_plen = pktLen - (ethHeaderLen + ipHeaderLen);
    pktHeader->ip6hdr.ip6_nxt = GRE_PROT_TYPE;
    default_l3V6_packet_create(pktHeader,l3DefPkt);
    return;
  }
  pktHeader->ipHeader.iph_versLen = IPv4_VERSION_LEN; 
  pktHeader->ipHeader.iph_ttl = 64;
  pktHeader->ipHeader.iph_ident = 0;
  pktHeader->ipHeader.iph_flags_frag = 0;
 
  if (pktHeader->ipFlag.src_ip_set != true)
  {
    pktHeader->ipHeader.iph_src = inet_addr(IPv4_SRC_ADDR);
  }
  if (pktHeader->ipFlag.dst_ip_set != true)
  {
    pktHeader->ipHeader.iph_dst = inet_addr(IPv4_DST_ADDR);
  }
  if (pktHeader->ipFlag.tos_set != true)
  {
    pktHeader->ipHeader.iph_tos = 0;
  }
  PACK_BYTE(pktHeader->ipHeader.iph_versLen, l3DefPkt);
  PACK_BYTE(pktHeader->ipHeader.iph_tos, l3DefPkt);
  PACK_SHORT(pktHeader->ipHeader.iph_len, l3DefPkt);
  PACK_SHORT(pktHeader->ipHeader.iph_ident, l3DefPkt);
  PACK_SHORT(pktHeader->ipHeader.iph_flags_frag, l3DefPkt);
  PACK_BYTE(pktHeader->ipHeader.iph_ttl, l3DefPkt);
  PACK_BYTE(pktHeader->ipHeader.iph_prot, l3DefPkt);
  pChecksum = l3DefPkt;
  checksum = 0;
  PACK_SHORT(checksum, l3DefPkt);
  PACK_N_BYTES(&pktHeader->ipHeader.iph_src, l3DefPkt,4);
  PACK_N_BYTES(&pktHeader->ipHeader.iph_dst, l3DefPkt,4);

  checksum = header_checksum(l3DefPktStart, PTAPP_GET_IP_HEADER_LEN(l3HeaderPtr));
  PACK_SHORT(checksum, pChecksum);
}

/*********************************************************************
* @brief            Function to create L4 UDP packet
*
* @param[in,out]    pktHeader   Packet details
* @param[out]       l4UdpDefPkt Packet Buffer 
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/

void default_l4Udp_packet_create(PTAPP_PKT_TYPE_t type,pktHeader_t *pktHeader,char *l4UdpDefPkt,int pktLen)
{
  short checksum = 0;
  unsigned int length;
  char *l4UdpDefPktStart;
  l4UdpDefPktStart = l4UdpDefPkt;
  CHECK_PKT_LEN((pktLen > (ethHeaderLen + ipHeaderLen + UDP_HEADER_LEN)),UDP_HEADER_ERROR);
  length = pktLen - (ethHeaderLen + ipHeaderLen);
  if (pktHeader->udpFlag.udpSrcPort_set != true)
  {
    pktHeader->udpHeader.uh_sport = UDP_DEFAULT_SRC_PORT;
  }
  if (pktHeader->udpFlag.udpDstPort_set != true)
  {
    pktHeader->udpHeader.uh_dport = UDP_DEFAULT_DST_PORT;
  }
  PACK_SHORT(pktHeader->udpHeader.uh_sport, l4UdpDefPkt);
  PACK_SHORT(pktHeader->udpHeader.uh_dport, l4UdpDefPkt);
  PACK_SHORT(pktHeader->udpHeader.uh_ulen, l4UdpDefPkt);
  if (type == PKT_TYPE_UDPv6)
  {
    checksum = ipv6_checksum_pseudo_header(l4UdpDefPktStart,length,pktHeader);
  }
  else if (type == PKT_TYPE_UDP)
  { 
    checksum = ipv4_checksum_pseudo_header(l4UdpDefPktStart,length,pktHeader);
  }
  PACK_SHORT(checksum,l4UdpDefPkt);
}

/*********************************************************************
* @brief            Function to create gre packet
*
* @param[in,out]    pktHeader   Packet details
* @param[out]       greDefPkt Packet Buffer 
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/

void default_gre_packet_create(pktHeader_t *pktHeader,char *greDefPkt,int pktLen)
{
  PTAPP_PKT_TYPE_t greInnerPktType = PKT_TYPE_MAX;
  int length = 0,encapIpHeaderLen = 0;
  pktHeader->greHeader.flagVersion = GRE_DEFAULT_FLAG;
  CHECK_PKT_LEN((pktLen > (ethHeaderLen + ipHeaderLen + GRE_HEADER_LEN)),GRE_HEADER_ERROR);
  if (pktHeader->greFlag.ipv6 == true)
  {
    pktHeader->greHeader.pType = IPv6_ETHER_TYPE;
    greInnerPktType = PKT_TYPE_IPv6; 
  }
  else if (pktHeader->greFlag.ipv4 == true)
  {
    pktHeader->greHeader.pType = IPv4_ETHER_TYPE;
    greInnerPktType = PKT_TYPE_IP; 
  }
  else
  {
    pktHeader->greHeader.pType = GRE_DEFAULT_PTYPE;
  }
  PACK_SHORT(pktHeader->greHeader.flagVersion,greDefPkt);
  PACK_SHORT(pktHeader->greHeader.pType,greDefPkt);
  if ((greInnerPktType == PKT_TYPE_IP) || (greInnerPktType == PKT_TYPE_IPv6))
  {
    if (greInnerPktType == PKT_TYPE_IP)
    {
      encapIpHeaderLen = IP_HEADER_LEN;
    }
    else
    {
      encapIpHeaderLen = IPv6_HEADER_LEN;
    }
    CHECK_PKT_LEN((pktLen > (ethHeaderLen + ipHeaderLen + GRE_HEADER_LEN + encapIpHeaderLen)),GRE_ENCAP_ERROR);
    memcpy(&pktHeader->ipHeader,&pktHeader->greIpHeader,sizeof(ipHeader_t));
    memcpy(&pktHeader->ip6hdr,&pktHeader->greIp6hdr,sizeof(struct ip6_hdr));
    memcpy(&pktHeader->ipFlag,&pktHeader->greIpFlag,sizeof(ipBool_t));
    memcpy(&pktHeader->ipv6Flag,&pktHeader->greIpv6Flag,sizeof(ipv6Bool_t));
    length = pktLen - (ethHeaderLen + ipHeaderLen + GRE_HEADER_LEN);
    pktHeader->ipHeader.iph_prot = 0xFF;
    pktHeader->ipHeader.iph_len = length;
    default_l3_packet_create(greInnerPktType,pktHeader,greDefPkt,length);
  }
}

/*********************************************************************
* @brief            Function to create vxlan packet
*
* @param[in,out]    pktHeader   Packet details
* @param[out]       vxlanDefPkt Packet Buffer 
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/

void default_vxlan_packet_create(pktHeader_t *pktHeader,char *vxlanDefPkt,int pktLen)
{
  unsigned int encapEthHeadLen,payload; 
  CHECK_PKT_LEN((pktLen > (ethHeaderLen + ipHeaderLen + VXLAN_HEADER_LEN)),VXLAN_HEADER_ERROR);
  pktHeader->vxlanHeader.vxlanFlag = (VXLAN_FLAG << 24);
  if (pktHeader->vxlanFlag.vNID_set != true)
  {
    pktHeader->vxlanHeader.vxlanVnid = VXLAN_VNID_DEFAULT;
  }
  pktHeader->vxlanHeader.vxlanVnid = ((pktHeader->vxlanHeader.vxlanVnid) << 8);
  PACK_LONG(pktHeader->vxlanHeader.vxlanFlag,vxlanDefPkt);
  PACK_LONG(pktHeader->vxlanHeader.vxlanVnid,vxlanDefPkt);
  if (pktHeader->vxlanFlag.is_l2_encapsulated == true)
  {
    memcpy(&pktHeader->ethFlag,&pktHeader->vxlanEthFlag,sizeof(ethBool_t));
    memcpy(&pktHeader->enetTagFlag,&pktHeader->vxlanEnetTagFlag,sizeof(enetTagBool_t));
    memcpy(&pktHeader->ethTag,&pktHeader->vxlanEthTag,sizeof(enetTag_t));
    memcpy(pktHeader->ethHeader.dest.addr,pktHeader->vxlanEthHeader.dest.addr, ETHERNET_MAC_ADDR_LEN); 
    memcpy(pktHeader->ethHeader.src.addr,pktHeader->vxlanEthHeader.src.addr, ETHERNET_MAC_ADDR_LEN);
    encapEthHeadLen = ETHERNET_UNTAGEED_HEADER_LEN;
    if (pktHeader->vxlanEnetTagFlag.ethTagTci_set == true)
    {
      encapEthHeadLen = ETHERNET_TAGEED_HEADER_LEN;
    }
    CHECK_PKT_LEN((pktLen > (ethHeaderLen + ipHeaderLen + VXLAN_HEADER_LEN + encapEthHeadLen)),VXLAN_ENCAP_ERROR);
    payload = pktLen -(ethHeaderLen + ipHeaderLen + UDP_HEADER_LEN + VXLAN_HEADER_LEN + encapEthHeadLen);
     if (payload >=ETH_MAX_LENGTH)
     {
       pktHeader->ethHeader.etherType = ETH_JUMBO_ETH_TYPE;
     }
     else
     { 
       pktHeader->ethHeader.etherType = payload;
     }
    default_l2_packet_create(pktHeader,vxlanDefPkt);
  }
}

/*********************************************************************
* @brief            Function to create L4 TCP packet
*
* @param[in,out]    pktHeader   Packet details
* @param[out]       l4TcpDefPkt Packet Buffer 
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/

void default_l4Tcp_packet_create(PTAPP_PKT_TYPE_t type,pktHeader_t *pktHeader,char *l4TcpDefPkt,int pktLen)
{
  short checksum;
  int offsetRes = 0;
  unsigned int length;
  char *l4TcpDefPktStart,*pChecksum;
  l4TcpDefPktStart = l4TcpDefPkt;
  length = pktLen - (ethHeaderLen + ipHeaderLen);
  CHECK_PKT_LEN((pktLen > (ethHeaderLen + ipHeaderLen + TCP_HEADER_LEN)),TCP_HEADER_ERROR);
  if (pktHeader->tcpFlag.tcpSrcPort_set != true)
  {
    pktHeader->tcpHeader.th_sport = TCP_DEFAULT_SRC_PORT;
  }
  if (pktHeader->tcpFlag.tcpDstPort_set != true)
  {
    pktHeader->tcpHeader.th_dport = TCP_DEFAULT_DST_PORT;
  }
  PACK_SHORT(pktHeader->tcpHeader.th_sport, l4TcpDefPkt);
  PACK_SHORT(pktHeader->tcpHeader.th_dport, l4TcpDefPkt);
  pktHeader->tcpHeader.th_off = 5;
  offsetRes = ((pktHeader->tcpHeader.th_off << 4) |  0);
  PACK_LONG(pktHeader->tcpHeader.th_seq, l4TcpDefPkt);
  PACK_LONG(pktHeader->tcpHeader.th_ack, l4TcpDefPkt);
  PACK_BYTE(offsetRes, l4TcpDefPkt);
  PACK_BYTE(pktHeader->tcpHeader.th_flags, l4TcpDefPkt);
  PACK_SHORT(pktHeader->tcpHeader.th_win, l4TcpDefPkt);
  pChecksum = l4TcpDefPkt;
  checksum = 0;
  PACK_SHORT(checksum, l4TcpDefPkt);
  PACK_SHORT(pktHeader->tcpHeader.th_urp, l4TcpDefPkt);
  if (type == PKT_TYPE_TCPv6)
  {
    checksum = ipv6_checksum_pseudo_header(l4TcpDefPktStart,length,pktHeader);
  }
  else
  { 
    checksum = ipv4_checksum_pseudo_header(l4TcpDefPktStart,length,pktHeader);
  }
  PACK_SHORT(checksum,pChecksum);
}


/*********************************************************************
* @brief            Function to create IPv6 IP packet
*
* @param[in,out]    pktHeader   Packet details
* @param[out]       l4UdpDefPkt Packet Buffer 
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/

void default_l3V6_packet_create(pktHeader_t *pktHeader,char *l3IpV6Pkt)

{
  int ret;
  pktHeader->ip6hdr.ip6_flow = ((IPV6_VERSION << 28) | (0 << 20) | 0);
  PACK_LONG(pktHeader->ip6hdr.ip6_flow,l3IpV6Pkt);
  PACK_SHORT(pktHeader->ip6hdr.ip6_plen,l3IpV6Pkt);
  PACK_BYTE(pktHeader->ip6hdr.ip6_nxt,l3IpV6Pkt);
  pktHeader->ip6hdr.ip6_hops = 255;
  PACK_BYTE(pktHeader->ip6hdr.ip6_hops,l3IpV6Pkt);
  if (pktHeader->ipv6Flag.src_ipv6_set != true)
  {
    if ((ret= inet_pton (AF_INET6, IPv6_SRC_ADDR, &(pktHeader->ip6hdr.ip6_src))) != 1) 
    {
      fprintf (stderr, "inet_pton() failed.\nError message: %s", strerror (errno));
      exit (EXIT_FAILURE);
    }
  } 
  PACK_N_BYTES(&(pktHeader->ip6hdr.ip6_src),l3IpV6Pkt,16);
  if (pktHeader->ipv6Flag.dst_ipv6_set != true)
  {
    if ((ret = inet_pton (AF_INET6, IPv6_DST_ADDR, &(pktHeader->ip6hdr.ip6_dst))) != 1) 
    {
      fprintf (stderr, "inet_pton() failed.\nError message: %s", strerror (errno));
      exit (EXIT_FAILURE);
    }
  }
  PACK_N_BYTES(&(pktHeader->ip6hdr.ip6_dst),l3IpV6Pkt,16);
}



/*********************************************************************
* @brief            Function to create packet
*
* @param[in,out]    pktHeader   Packet details
* @param[out]       l2pktBuf    Packet Buffer 
* @param[in]        pktLen      Packet Length
* @param[in]        offset      Data to be filled at a particular location
* @param[in]        offsetLoc   Offset from the begining of the packet 
* @param[in]        offsetLen   Size of data(Bytes) to be filled at offset 
*                               location
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/

void create_ip_packet(PTAPP_PKT_TYPE_t type,pktHeader_t *pktHeader,
                      char * l2pktBuf, unsigned int pktLen,
                      char *offset,unsigned int offsetLoc, unsigned int offsetLen)
{
  unsigned int crc;
  unsigned short checksum;
  char *l2DefPktStart;
  ipHeader_t *l3HeaderPtr = NULL;

  l2DefPktStart = l2pktBuf;
  pktHeader->ipHeader.iph_prot = 0xFF;
  pktHeader->ipHeader.iph_len = pktLen-ethHeaderLen;
  default_l2_packet_create(pktHeader,l2pktBuf);

  l2pktBuf += ethHeaderLen;
  l3HeaderPtr = (ipHeader_t *)l2pktBuf;
  default_l3_packet_create(type,pktHeader,l2pktBuf,pktLen);

  if ((offset != NULL) && (offsetLen !=0))
  {
    offset_apply(l2DefPktStart,offset,offsetLoc,offsetLen);
    l3HeaderPtr->iph_csum = 0;
    checksum = header_checksum(l2pktBuf, PTAPP_GET_IP_HEADER_LEN(l3HeaderPtr));
    l2pktBuf += 10;
    PACK_SHORT(checksum, l2pktBuf);
  }
  crc = crc32_calculate(l2DefPktStart,(pktLen));
  l2DefPktStart += pktLen;
  PACK_LONG(crc,l2DefPktStart);
}

/*********************************************************************
* @brief            Function to create GRE
*
* @param[in,out]    pktHeader   Packet details
* @param[out]       l2pktBuf    Packet Buffer 
* @param[in]        pktLen      Packet Length
* @param[in]        offset      Data to be filled at a particular location
* @param[in]        offsetLoc   Offset from the begining of the packet 
* @param[in]        offsetLen   Size of data(Bytes) to be filled at offset 
*                               location
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/

void create_gre_packet(PTAPP_PKT_TYPE_t type,pktHeader_t *pktHeader,
                         char * l2pktBuf, unsigned int pktLen,
                         char *offset,unsigned int offsetLoc, unsigned int offsetLen)
{
  PTAPP_PKT_TYPE_t type_local;
  unsigned int crc;
  unsigned short checksum;
  char *l2DefPktStart, *l3pktBuf;
  ipHeader_t *l3HeaderPtr = NULL;
  type_local = PKT_TYPE_IPv6_GRE;
  if (type == PKT_TYPE_IPv4_GRE)
  {
    type_local = PKT_TYPE_IP;
  }
  l2DefPktStart = l2pktBuf;
  pktHeader->ipHeader.iph_prot = GRE_PROT_TYPE;
  pktHeader->ipHeader.iph_len = pktLen-ethHeaderLen;
  default_l2_packet_create(pktHeader,l2pktBuf);

  l2pktBuf += ethHeaderLen;
  l3HeaderPtr = (ipHeader_t *)l2pktBuf;
  default_l3_packet_create(type_local,pktHeader,l2pktBuf,pktLen);
  
  l3pktBuf = l2pktBuf + ipHeaderLen;
  default_gre_packet_create(pktHeader,l3pktBuf,pktLen);

  if ((offset != NULL) && (offsetLen !=0))
  {
    offset_apply(l2DefPktStart,offset,offsetLoc,offsetLen);
    l3HeaderPtr->iph_csum = 0;
    checksum = header_checksum(l2pktBuf, PTAPP_GET_IP_HEADER_LEN(l3HeaderPtr));
    l2pktBuf += 10;
    PACK_SHORT(checksum, l2pktBuf);
  }
  crc = crc32_calculate(l2DefPktStart,(pktLen));
  l2DefPktStart += pktLen;
  PACK_LONG(crc,l2DefPktStart);
}

/*********************************************************************
* @brief            Function to create VXLAN
*
* @param[in,out]    pktHeader   Packet details
* @param[out]       l2pktBuf    Packet Buffer 
* @param[in]        pktLen      Packet Length
* @param[in]        offset      Data to be filled at a particular location
* @param[in]        offsetLoc   Offset from the begining of the packet 
* @param[in]        offsetLen   Size of data(Bytes) to be filled at offset 
*                               location
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/

void create_vxlan_packet(PTAPP_PKT_TYPE_t type,pktHeader_t *pktHeader,
                         char * l2pktBuf, unsigned int pktLen,
                         char *offset,unsigned int offsetLoc, unsigned int offsetLen)
{
  PTAPP_PKT_TYPE_t type_local = PKT_TYPE_MAX;
  unsigned int crc,length = 0;
  unsigned short checksum = 0;
  char *l2DefPktStart,*l3pktBuf,*vxlanPktBuf;
  ipHeader_t *l3HeaderPtr = NULL;
  udphdr_t  *l4HeaderPtr = NULL; 
  if (type == PKT_TYPE_IPv4_VXLAN)
  {
    type_local = PKT_TYPE_UDP;
  }
  else if (type == PKT_TYPE_IPv6_VXLAN)
  {
    type_local = PKT_TYPE_UDPv6;
  }
  
  l2DefPktStart = l2pktBuf;
  pktHeader->ipHeader.iph_prot = IPPROTO_UDP;
  pktHeader->ipHeader.iph_len = pktLen-ethHeaderLen;
  pktHeader->udpHeader.uh_ulen = pktLen-(ipHeaderLen+ethHeaderLen);
  pktHeader->udpHeader.uh_dport = VXLAN_UDP_PORT;
  pktHeader->udpFlag.udpDstPort_set = true;
  length = pktHeader->ipHeader.iph_len;

  default_l2_packet_create(pktHeader,l2pktBuf);

  l2pktBuf += ethHeaderLen;
  l3HeaderPtr = (ipHeader_t *)l2pktBuf;
  default_l3_packet_create(type_local,pktHeader,l2pktBuf,pktLen);

  l3pktBuf = l2pktBuf + ipHeaderLen;
  l4HeaderPtr = (udphdr_t *)l3pktBuf;
  default_l4Udp_packet_create(type,pktHeader,l3pktBuf,pktLen);

  vxlanPktBuf = l3pktBuf + UDP_HEADER_LEN;
  default_vxlan_packet_create(pktHeader,vxlanPktBuf,pktLen);

  if ((offset != NULL) && (offsetLen !=0))
  {
    offset_apply(l2DefPktStart,offset,offsetLoc,offsetLen);
    if (type == PKT_TYPE_UDP)
    {
      l3HeaderPtr->iph_csum = 0;
      checksum = header_checksum(l2pktBuf, PTAPP_GET_IP_HEADER_LEN(l3HeaderPtr));
      l2pktBuf += 10;
      PACK_SHORT(checksum, l2pktBuf);
    }
  }
  length = pktLen - (ethHeaderLen + ipHeaderLen);
  l4HeaderPtr->uh_sum = 0;
  if (type_local == PKT_TYPE_UDPv6)
  {
    checksum = ipv6_checksum_pseudo_header(l3pktBuf,length,pktHeader);
  }
  else if(type_local == PKT_TYPE_UDP)
  {
    checksum = 0; /*Checksum is 0 for IPv4 VXLAN packets*/  
  }
  l3pktBuf += 6;
  PACK_SHORT(checksum, l3pktBuf);
  crc = crc32_calculate(l2DefPktStart,(pktLen));
  l2DefPktStart += pktLen ;
  PACK_LONG(crc,l2DefPktStart);
}

/*********************************************************************
* @brief            Convert Multicast IP to MAC
*
* @param[in]        ipAddr   Multicast Ip Address 
* @param[in,out]    macAddr  Multicast MAC
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/

void multicast_ip_to_mac(int ipAddr, char *macAddr)
{
  macAddr[0] = 0x01;
  macAddr[1] = 0x00;
  macAddr[2] = 0x5E;
  macAddr[3] = (ipAddr & 0x007f0000) >> 16;
  macAddr[4] = (ipAddr & 0x0000ff00) >> 8;
  macAddr[5] = (ipAddr & 0x000000ff);
}

/*********************************************************************
* @brief            Function to create IP Multicast Packet
*
* @param[in,out]    pktHeader   Packet details
* @param[out]       l2pktBuf    Packet Buffer 
* @param[in]        pktLen      Packet Length
* @param[in]        offset      Data to be filled at a particular location
* @param[in]        offsetLoc   Offset from the begining of the packet 
* @param[in]        offsetLen   Size of data(Bytes) to be filled at offset 
*                               location
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/

void create_ip_multicast_packet(PTAPP_PKT_TYPE_t type,pktHeader_t *pktHeader,
                                char * l2pktBuf, unsigned int pktLen,
                                char *offset,unsigned int offsetLoc, unsigned int offsetLen)
{
  unsigned int crc;
  unsigned short checksum;
  char buf[24];
  char *l2DefPktStart;
  ipHeader_t  *l3HeaderPtr = NULL;

  l2DefPktStart = l2pktBuf;
  pktHeader->ipHeader.iph_prot = 0xFF;
  pktHeader->ipHeader.iph_len = pktLen-ethHeaderLen;
  if (pktHeader->ethFlag.ethDstMac_set != true)
  {
    if(pktHeader->ipFlag.dst_ip_set == true)
    {
      multicast_ip_to_mac(htonl(pktHeader->ipHeader.iph_dst),pktHeader->ethHeader.dest.addr);
      pktHeader->ethFlag.ethDstMac_set = true;
    }
    else
    {
      pktHeader->ipHeader.iph_dst = inet_addr(IPv4_DST_MCAST);
      multicast_ip_to_mac(pktHeader->ipHeader.iph_dst,pktHeader->ethHeader.dest.addr);
      pktHeader->ipFlag.dst_ip_set = true;
      pktHeader->ethFlag.ethDstMac_set = true;
    }
  } 
  else
  {
    if(pktHeader->ipFlag.dst_ip_set != true)
    {
      sprintf(buf, "224.%d.%d.%d", 
              pktHeader->ethHeader.dest.addr[3],
              pktHeader->ethHeader.dest.addr[4], pktHeader->ethHeader.dest.addr[5]);
      pktHeader->ipHeader.iph_dst = inet_addr(buf);
      pktHeader->ipFlag.dst_ip_set = true;
    }
  }
  default_l2_packet_create(pktHeader,l2pktBuf);

  l2pktBuf += ethHeaderLen;
  l3HeaderPtr = (ipHeader_t *)l2pktBuf;   
  default_l3_packet_create(type,pktHeader,l2pktBuf,pktLen);
  if ((offset != NULL) && (offsetLen !=0))
  {
    offset_apply(l2DefPktStart,offset,offsetLoc,offsetLen);
    l3HeaderPtr->iph_csum = 0; 
    checksum = header_checksum(l2pktBuf, PTAPP_GET_IP_HEADER_LEN(l3HeaderPtr));
    l2pktBuf += 10;
    PACK_SHORT(checksum, l2pktBuf);
  }
  crc = crc32_calculate(l2DefPktStart,(pktLen));
  l2DefPktStart += pktLen;
  PACK_LONG(crc,l2DefPktStart);
}



/*********************************************************************
* @brief            Function to create TCP packet
*
* @param[in,out]    pktHeader   Packet details
* @param[out]       l2pktBuf    Packet Buffer 
* @param[in]        pktLen      Packet Length
* @param[in]        offset      Data to be filled at a particular location
* @param[in]        offsetLoc   Offset from the begining of the packet 
* @param[in]        offsetLen   Size of data(Bytes) to be filled at offset 
*                               location
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/

void create_tcp_packet(PTAPP_PKT_TYPE_t type,pktHeader_t *pktHeader,
                      char * l2pktBuf, unsigned int pktLen,
                      char *offset,unsigned int offsetLoc, unsigned int offsetLen)
{
  unsigned int crc,length=0;
  unsigned short checksum;
  char *l2DefPktStart,*l3pktBuf;
  ipHeader_t  *l3HeaderPtr = NULL; 
  tcphdr_t  *l4HeaderPtr = NULL; 

  l2DefPktStart = l2pktBuf;
  pktHeader->ipHeader.iph_prot = IPPROTO_TCP;
  pktHeader->tcpHeader.th_win = (htons(TCP_WINDOW_SIZE));
  pktHeader->ipHeader.iph_len = pktLen-ethHeaderLen;
  length = pktHeader->ipHeader.iph_len;
  default_l2_packet_create(pktHeader,l2pktBuf);

  l2pktBuf += ethHeaderLen;
  l3HeaderPtr = (ipHeader_t *)l2pktBuf;
  default_l3_packet_create(type,pktHeader,l2pktBuf,pktLen);

  l3pktBuf = l2pktBuf + ipHeaderLen;
  l4HeaderPtr = (tcphdr_t *)l3pktBuf;
  default_l4Tcp_packet_create(type,pktHeader,l3pktBuf,pktLen);
  if ((offset != NULL) && (offsetLen !=0))
  {
    offset_apply(l2DefPktStart, offset, offsetLoc, offsetLen);
    if (type == PKT_TYPE_TCP)
    {
      l3HeaderPtr->iph_csum = 0;
      checksum = header_checksum(l2pktBuf, PTAPP_GET_IP_HEADER_LEN(l3HeaderPtr));
      l2pktBuf += 10;
      PACK_SHORT(checksum, l2pktBuf);
    }

    length = pktLen - (ethHeaderLen + ipHeaderLen);
     l4HeaderPtr->th_sum = 0;
    if (type == PKT_TYPE_TCPv6)
    {
      checksum = ipv6_checksum_pseudo_header(l3pktBuf,length,pktHeader);
    }
    else
    { 
      checksum = ipv4_checksum_pseudo_header(l3pktBuf,length,pktHeader);
    }
    l3pktBuf += 16;
    PACK_SHORT(checksum, l3pktBuf);
  }
  crc = crc32_calculate(l2DefPktStart,(pktLen));
  l2DefPktStart += pktLen;
  PACK_LONG(crc,l2DefPktStart);
}


/*********************************************************************
* @brief            Function to create UDP packet
*
* @param[in,out]    pktHeader   Packet details
* @param[out]       l2pktBuf    Packet Buffer 
* @param[in]        pktLen      Packet Length
* @param[in]        offset      Data to be filled at a particular location
* @param[in]        offsetLoc   Offset from the begining of the packet 
* @param[in]        offsetLen   Size of data(Bytes) to be filled at offset 
*                               location
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/

void create_udp_packet(PTAPP_PKT_TYPE_t type,pktHeader_t *pktHeader,
                      char * l2pktBuf, unsigned int pktLen,
                      char *offset,unsigned int offsetLoc, unsigned int offsetLen)
{
  unsigned int crc,length = 0;
  unsigned short checksum;
  char *l2DefPktStart,*l3pktBuf;
  ipHeader_t *l3HeaderPtr = NULL;
  udphdr_t  *l4HeaderPtr = NULL; 
  
  l2DefPktStart = l2pktBuf;
  pktHeader->ipHeader.iph_prot = IPPROTO_UDP;
  pktHeader->ipHeader.iph_len = pktLen-ethHeaderLen;
  pktHeader->udpHeader.uh_ulen = pktLen-(ipHeaderLen+ethHeaderLen);
  length = pktHeader->ipHeader.iph_len;

  default_l2_packet_create(pktHeader,l2pktBuf);

  l2pktBuf += ethHeaderLen;
  l3HeaderPtr = (ipHeader_t *)l2pktBuf;
  default_l3_packet_create(type,pktHeader,l2pktBuf,pktLen);

  l3pktBuf = l2pktBuf + ipHeaderLen;
  l4HeaderPtr = (udphdr_t *)l3pktBuf;
  default_l4Udp_packet_create(type,pktHeader,l3pktBuf,pktLen);

  if ((offset != NULL) && (offsetLen !=0))
  {
    offset_apply(l2DefPktStart,offset,offsetLoc,offsetLen);
    if (type == PKT_TYPE_UDP)
    {
      l3HeaderPtr->iph_csum = 0;
      checksum = header_checksum(l2pktBuf, PTAPP_GET_IP_HEADER_LEN(l3HeaderPtr));
      l2pktBuf += 10;
      PACK_SHORT(checksum, l2pktBuf);
    }
    length = pktLen - (ethHeaderLen + ipHeaderLen);
    l4HeaderPtr->uh_sum = 0;
    if (type == PKT_TYPE_UDPv6)
    {
      checksum = ipv6_checksum_pseudo_header(l3pktBuf,length,pktHeader);
    }
    else
    { 
      checksum = ipv4_checksum_pseudo_header(l3pktBuf,length,pktHeader);
    }
    l3pktBuf += 6;
    PACK_SHORT(checksum, l3pktBuf);
  }
  crc = crc32_calculate(l2DefPktStart,(pktLen));
  l2DefPktStart += pktLen ;
  PACK_LONG(crc,l2DefPktStart);
}

/*********************************************************************
* @purpose  Create MPLS Label
*
* @param[in]     label   MPLS label
* @param[in]     pri     Priority
* @param[in]     bos     Bottom of Stack
* @param[in]     ttl     TTL
* @param[in,out] header  MPLS Header
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/

void create_mpls_header(unsigned int label, uint8_t pri, uint8_t bos,
                        uint8_t ttl, uint8_t *updatedHeader)
{
  unsigned int header = 0;

  header |= (label << 12);
  header |= (((unsigned int)pri & 0x00000007L) << 9);
  header |= (((unsigned int)bos & 0x00000001L) << 8);
  header |= ((unsigned int)ttl & 0x000000FFL);

  header = htonl(header);
  memcpy(updatedHeader, &header, sizeof(header));
}



/*********************************************************************
* @brief            Function to create packet
*
* @param[in]        type        Type of packet (Eg. Ethernet, L3, etc)
* @param[in,out]    pktHeader   Packet details
* @param[out]       l2pktBuf    Packet Buffer 
* @param[in]        pktLen      Packet Length
* @param[in]        offset      Data to be filled at a particular location
* @param[in]        offsetLoc   Offset from the begining of the packet 
* @param[in]        offsetLen   Size of data(Bytes) to be filled at offset 
*                               location
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/

int eth_packet_create(PTAPP_PKT_TYPE_t type, pktHeader_t *pktHeader,
                               char * l2pktBuf, unsigned int pktLen,
                               char *offset,unsigned int offsetLoc, unsigned int offsetLen)
{
  unsigned int crc;
  char *l2DefPktStart;
  l2DefPktStart = l2pktBuf;
  unsigned int payloadLen = pktLen-4;

  if ((pktHeader == NULL) || (pktLen < MIN_PKT_SIZE) || (pktLen > MAX_PKT_SIZE))
  {
     return -1;
  }
  if (offset == NULL) 
  {
     printf("Offset out of range/ IGNORED\n");
  }
  if ((offset != NULL) && ((offsetLoc + offsetLen) > pktLen))
  {
     printf("Offset out of range/ IGNORED\n");
     return -1;
  }
  ethHeaderLen =  ETHERNET_UNTAGEED_HEADER_LEN; 
  if (pktHeader->enetTagFlag.ethTagTci_set == true)
  {
    ethHeaderLen = ETHERNET_TAGEED_HEADER_LEN;
  }
  if ((pktHeader->mplsFlag.noOfLabels > 0) && (pktHeader->mplsFlag.noOfLabels <= MAX_MPLS_LABELS))
  {
    ethHeaderLen += (pktHeader->mplsFlag.noOfLabels *MPLS_SIZE);
  }

  ipHeaderLen = IP_HEADER_LEN;

  if ((type == PKT_TYPE_IPv6) || (type == PKT_TYPE_TCPv6) || (type == PKT_TYPE_UDPv6)
      || (type == PKT_TYPE_IPv6_GRE) || (type == PKT_TYPE_IPv6_VXLAN))
  {
    ipHeaderLen = IPv6_HEADER_LEN;
  }
  memset(l2pktBuf,0,pktLen);
  switch (type)
  {
   case PKT_TYPE_ETHERNET:
     if (payloadLen >=ETH_MAX_LENGTH)
     {
       pktHeader->ethHeader.etherType = ETH_JUMBO_ETH_TYPE;
     }
     else
     { 
       pktHeader->ethHeader.etherType = (payloadLen - ethHeaderLen);
     }
     default_l2_packet_create(pktHeader,l2pktBuf);
     crc = crc32_calculate(l2DefPktStart,(pktLen-4));
     l2pktBuf += pktLen -4;
     PACK_LONG(crc,l2pktBuf);
     break; 
   case PKT_TYPE_IP:
     pktHeader->ethHeader.etherType = IPv4_ETHER_TYPE;
     create_ip_packet(type,pktHeader,l2pktBuf,payloadLen,offset,offsetLoc,offsetLen);
     break; 
   case PKT_TYPE_IPv6:
     pktHeader->ethHeader.etherType = IPv6_ETHER_TYPE;
     create_ip_packet(type,pktHeader,l2pktBuf,payloadLen,offset,offsetLoc,offsetLen);
     break;
   case PKT_TYPE_TCP:
     pktHeader->ethHeader.etherType = IPv4_ETHER_TYPE;
     create_tcp_packet(type,pktHeader,l2pktBuf,payloadLen,offset,offsetLoc,offsetLen);
     break; 
   case PKT_TYPE_TCPv6:
     pktHeader->ethHeader.etherType = IPv6_ETHER_TYPE;
     create_tcp_packet(type,pktHeader,l2pktBuf,payloadLen,offset,offsetLoc,offsetLen);
     break;
   case PKT_TYPE_UDP:
     pktHeader->ethHeader.etherType = IPv4_ETHER_TYPE;
     create_udp_packet(type,pktHeader,l2pktBuf,payloadLen,offset,offsetLoc,offsetLen);
     break; 
   case PKT_TYPE_UDPv6:
     pktHeader->ethHeader.etherType = IPv6_ETHER_TYPE;
     create_udp_packet(type,pktHeader,l2pktBuf,payloadLen,offset,offsetLoc,offsetLen);
     break;
   case PKT_TYPE_IPv4_GRE:
     pktHeader->ethHeader.etherType = IPv4_ETHER_TYPE;
     create_gre_packet(type,pktHeader,l2pktBuf,payloadLen,offset,offsetLoc,offsetLen);
     break;
   case PKT_TYPE_IPv6_GRE:
     pktHeader->ethHeader.etherType = IPv6_ETHER_TYPE;
     create_gre_packet(type,pktHeader,l2pktBuf,payloadLen,offset,offsetLoc,offsetLen);
     break;
   case PKT_TYPE_IPv4_VXLAN:
     pktHeader->ethHeader.etherType = IPv4_ETHER_TYPE;
     create_vxlan_packet(type,pktHeader,l2pktBuf,payloadLen,offset,offsetLoc,offsetLen);
     break;
   case PKT_TYPE_IPv6_VXLAN:
     pktHeader->ethHeader.etherType = IPv6_ETHER_TYPE;
     create_vxlan_packet(type,pktHeader,l2pktBuf,payloadLen,offset,offsetLoc,offsetLen);
     break;
   case PKT_TYPE_IP_MULTICAST:
     pktHeader->ethHeader.etherType = IPv4_ETHER_TYPE;
     create_ip_multicast_packet(type,pktHeader,l2pktBuf,payloadLen,offset,offsetLoc,offsetLen);
     break;
   default:
     break; 
  }
  return 0;
}

/*********************************************************************
* @brief        Creates CRC Lookup table
*
* @param[in]    NA
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/

void crc32_lookup_init (void)
{
  static int first = 1;
  unsigned int val = 0;
  unsigned int i = 0;
  unsigned int j = 0;

  if (first != 0)
  {
    for (i = 0; i < 256; i++)
    {
      val = i;

      for (j = 0; j < 8; j++)
      {
        if ((val & 1) != 0)
        {
          val = 0xedb88320 ^ (val >> 1);
        }
        else
        {
          val = val >> 1;
        }
      }

      crc32_table_lookup[i] = val;
    }

    first = 0;
  }
}



/*********************************************************************
* @brief        Calculated 32 bit CRC
*
* @param[in]    buffer Starting location of the buffer for which
*               CRC is needed
* @param[in]    Length of buffer
*
* @retval       CRC
*
* @note         NA
*
* @end
*********************************************************************/

int crc32_calculate (char * buffer, int length)
{
  unsigned int result = ~0;
  unsigned int i = 0;
  crc32_lookup_init ();

  for (i = 0; i < length; i++)
  {
    result = crc32_table_lookup[(result ^ buffer[i]) & 0xff] ^ (result >> 8);
  }

  result = (~result) & 0xffffffff;
  result = swapInt(result);
  return result;
}


/*********************************************************************
* @brief        Function used to calculate checksum
*
* @param[in]    pData Buffer for which checksum is needed
* @param[in]    len   Buffer length
*
* @retval       checksum
*
* @note         NA
*
* @end
*********************************************************************/

short header_checksum(char *addr, unsigned int count)
{
  unsigned int sum = 0;
  while(count > 1)
  {
    sum += *((ushort *) addr);
    addr += 2;
    count = count - 2;
  }
  if (count > 0)
    sum = sum + *((char *) addr);
  while (sum>>16)
    sum = (sum & 0xFFFF) + (sum >> 16);
  return(htons(~sum));
}

/*********************************************************************
* @brief        Function used to apply offset
*
* @param[in]    addr      Starting Address 
* @param[in]    offset    Offset Value
* @param[in]    offsetLoc Offset Location
* @param[in]    offsetLen Offset Length
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/

void offset_apply(char *addr,char *offset, unsigned int offsetLoc,unsigned int offsetLen)
{
  char *hdrStart = addr;
  hdrStart +=offsetLoc;
  PACK_N_BYTES(offset,hdrStart,offsetLen);
}

/*********************************************************************
* @brief        Function to calculate checksum (TCP/UDP)
*
* @param[in]    addr      Starting Address 
* @param[in]    length    Length based on protocol
* @param[in]    pktHeader Packet details
*
* @retval       Checksum
*
* @note         NA
*
* @end
*********************************************************************/
short ipv4_checksum_pseudo_header(char *addr,unsigned int length,pktHeader_t *pktHeader)
{
  char buff[MAX_PKT_SIZE];
  char *buffPtr;
  ipv4PseudoHeader_t ipv4PseudoHeader;
   
  buffPtr = buff;
  ipv4PseudoHeader.iph_src = (pktHeader->ipHeader.iph_src);
 
  ipv4PseudoHeader.iph_dst = (pktHeader->ipHeader.iph_dst);
 /* ipv4PseudoHeader.reserved = 0; */
  ipv4PseudoHeader.protocol = htons(pktHeader->ipHeader.iph_prot);
  ipv4PseudoHeader.length = htons(length);

  memset(buffPtr,0,sizeof(buff));
 
  memcpy(buffPtr,(char *)&ipv4PseudoHeader,sizeof(ipv4PseudoHeader_t));
  buffPtr += sizeof(ipv4PseudoHeader_t); 
  memcpy(buffPtr,addr,length);

  return (header_checksum(buff, (length+sizeof(ipv4PseudoHeader_t))));
}

/*********************************************************************
* @brief        Function to calculate checksum (TCP/UDP)
*
* @param[in]    addr      Starting Address 
* @param[in]    length    Length based on protocol
* @param[in]    pktHeader Packet details
*
* @retval       Checksum
*
* @note         NA
*
* @end
*********************************************************************/

short ipv6_checksum_pseudo_header(char *addr,unsigned int length,pktHeader_t *pktHeader)
{
  char buff[MAX_PKT_SIZE];
  char *buffPtr;
  ipv6PseudoHeader_t ipv6PseudoHeader;

  buffPtr = buff;
  ipv6PseudoHeader.ip6_src = pktHeader->ip6hdr.ip6_src;
  ipv6PseudoHeader.ip6_dst = pktHeader->ip6hdr.ip6_dst;
  ipv6PseudoHeader.length = htonl(length);
  ipv6PseudoHeader.nextHeader = htonl(pktHeader->ip6hdr.ip6_nxt);
  memset(buffPtr,0,sizeof(buff));

  memcpy(buffPtr,(char *)&ipv6PseudoHeader,sizeof(ipv6PseudoHeader_t));
  buffPtr += sizeof(ipv6PseudoHeader_t); 
  memcpy(buffPtr,addr,length);

  return (header_checksum(buff,(length+sizeof(ipv6PseudoHeader_t))));
}

  
  
