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

#ifndef INCLUDE_PKT_LIB_H
#define	INCLUDE_PKT_LIB_H

#include<netinet/ip.h>
#include<netinet/tcp.h>
#include<netinet/udp.h>
#include<stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netinet/ip6.h>     
#include "ptapp.h"
 
#define ETHERNET_MAC_ADDR_LEN         6 
#define ETHERNET_MAC_ADDR_STR_LEN     18
#define MIN_PKT_SIZE   PTAPP_MIN_SUPPORTED_PKT_SIZE 
#define MAX_PKT_SIZE   PTAPP_MAX_SUPPORTED_PKT_SIZE
#define IPv4_VERSION                  4 
#define IPv4_VERSION_LEN              69 
#define IPv4_SRC_ADDR                 "192.168.1.1" 
#define IPv4_DST_ADDR                 "25.25.25.25" 
#define IPv4_DST_MCAST                "224.1.1.1" 
#define IPv6_SRC_ADDR                 "2001:db8::214:51ff:fe2f:1556" 
#define IPv6_DST_ADDR                 "2001:db8::214:51ff:fe2f:1666" 
#define IPv4_TTL                      0x40 
#define IPv4_PROT                     255 
#define MAX_MPLS_LABELS               4 /*Maximum number of MPLS headers supported*/ 
#define ETHERNET_UNTAGEED_HEADER_LEN  sizeof(struct ethernetHeader_s) 
#define SINGLE_VLAN_TAG_HDR_LEN       4 
#define ETHERNET_TAGEED_HEADER_LEN    (sizeof(struct ethernetHeader_s) + SINGLE_VLAN_TAG_HDR_LEN) 
#define IP_HEADER_LEN                 (sizeof(struct ipHeader_s)) 
#define IPv6_HEADER_LEN               (sizeof(struct ip6_hdr)) 
#define TCP_HEADER_LEN                (sizeof(struct tcphdr_s)) 
#define UDP_HEADER_LEN                (sizeof(struct udphdr_s)) 
#define GRE_HEADER_LEN                (sizeof(greHeader_t)) 
#define VXLAN_HEADER_LEN              (sizeof(vxlanHeader_t)) 
#define TCP_WINDOW_SIZE               5840 
#define TCP_DEFAULT_SRC_PORT          80 
#define TCP_DEFAULT_DST_PORT          81 
#define UDP_DEFAULT_SRC_PORT          2000 
#define UDP_DEFAULT_DST_PORT          2001 
#define IPV6_VERSION                  6 
#define VLAN_TPID                     0x8100 
#define MPLS_DEFAULT_LABEL            10 
#define MPLS_DEFAULT_PRI              0 
#define MPLS_TTL                      64 
#define MPLS_SIZE                     4 
#define MPLS_ETHER_TYPE               0x8847 
#define VXLAN_VNID_DEFAULT            10 
#define IPv4_ETHER_TYPE               0x0800
#define IPv6_ETHER_TYPE               0x86DD
#define VXLAN_UDP_PORT                4789
#define VXLAN_FLAG                    8
#define GRE_PROT_TYPE                 0x2F
#define END_OF_STACK                  1
#define GRE_DEFAULT_PTYPE             0x00
#define GRE_DEFAULT_FLAG              0x00
#define ETH_MAX_LENGTH                0x0600
#define ETH_JUMBO_ETH_TYPE            0x8870
#define IP_HEADER_ERROR               "IP Header cannot be included due to insufficient packet length"
#define TCP_HEADER_ERROR              "TCP Header cannot be included due to insufficient packet length"
#define UDP_HEADER_ERROR              "UDP Header cannot be included due to insufficient packet length"
#define GRE_HEADER_ERROR              "GRE Header cannot be included due to insufficient packet length"
#define VXLAN_HEADER_ERROR            "VXLAN Header cannot be included due to insufficient packet length"
#define GRE_ENCAP_ERROR               "GRE Encapsulated frame cannot be included due to insufficient packet length"
#define VXLAN_ENCAP_ERROR             "VXLAN Encapsulated frame cannot be included due to insufficient packet length"

/* Ethernet MAC Address */
typedef struct macAddr_s
{
  char  addr[ETHERNET_MAC_ADDR_LEN];
}macAddr_t;


typedef struct ethernetHeader_s
{
  macAddr_t    dest;
  macAddr_t    src;
  ushort       etherType;
}ethernetHeader_t;


typedef struct enetTag_s
{
  ushort      tpid;                            /* Tag Protocol ID - 0x8100 if 802.1Q Tagged */
  ushort      tci;                             /* Tag Control Information */
}enetTag_t;

typedef struct ipHeader_s
{
  unsigned char  iph_versLen;      /* version/hdr len */
  unsigned char  iph_tos;          /* type of service */
  unsigned short iph_len;          /* pdu len */
  unsigned short iph_ident;        /* identifier */
  unsigned short iph_flags_frag;   /* flags/ fragment offset */
  unsigned char  iph_ttl;          /* lifetime */
  unsigned char  iph_prot;         /* protocol ID */
  unsigned short iph_csum;         /* checksum */
  unsigned int   iph_src;          /* src add in network byte order*/
  unsigned int   iph_dst;          /* dst add in network byte order*/
}ipHeader_t;

#define PTAPP_GET_IP_HEADER_LEN(l3HeaderPtr) (unsigned int)((l3HeaderPtr->iph_versLen & (0x0F)) * 4)

typedef struct ethBool_s
{
  bool ethSrcMac_set;
  bool ethDstMac_set;
}ethBool_t;


typedef struct enetTagBool_s
{
  bool ethTagTci_set;
}enetTagBool_t;

typedef struct mplsLabelBool_s
{
  unsigned int noOfLabels;
  bool mplsLabel_val_set[MAX_MPLS_LABELS];
  bool mplsLabel_pri_set[MAX_MPLS_LABELS];
}mplsLabelBool_t;

typedef struct ipBool_s
{
  bool  tos_set;          /*bool value to check if value is set*/
  bool  prot_set;
  bool  src_ip_set;
  bool  dst_ip_set; 
}ipBool_t;

typedef struct udpBool_s
{
  bool udpSrcPort_set;
  bool udpDstPort_set;
}udpBool_t;

typedef struct tcpBool_s
{
  bool tcpSrcPort_set;
  bool tcpDstPort_set;
}tcpBool_t;

typedef struct ipv6Bool_s
{
  bool  src_ipv6_set;
  bool  dst_ipv6_set;
}ipv6Bool_t;
typedef struct udphdr_s
{
  unsigned short uh_sport;           /* source port */
  unsigned short uh_dport;           /* destination port */
  unsigned short uh_ulen;            /* udp length */
  unsigned short uh_sum;             /* udp checksum */
}udphdr_t;

typedef struct ipv4PseudoHeader_s
{
  unsigned int   iph_src;          /* src add */
  unsigned int   iph_dst;          /* dst add */
/*  u_int8_t       reserved;
  u_int8_t       protocol; */
  unsigned short protocol;
  unsigned short length;
}ipv4PseudoHeader_t;
  
typedef struct ipv6PseudoHeader_s
{
  struct in6_addr ip6_src;      /* source address */
  struct in6_addr ip6_dst;      /* destination address */
  unsigned int length;
  unsigned int nextHeader;
}ipv6PseudoHeader_t;

typedef struct vxlanHeader_s
{
  unsigned int vxlanFlag;
  unsigned int vxlanVnid;
}vxlanHeader_t;

typedef struct greHeader_s
{
  unsigned short flagVersion;
  unsigned short pType;
}greHeader_t;

typedef struct tcphdr_s
  {
    unsigned short th_sport;         /* source port */
    unsigned short th_dport;         /* destination port */
    unsigned int th_seq;             /* sequence number */
    unsigned int th_ack;             /* acknowledgement number */
#  if __BYTE_ORDER == __LITTLE_ENDIAN
    u_int8_t th_x2:4;           /* (unused) */
    u_int8_t th_off:4;          /* data offset */
#  endif
#  if __BYTE_ORDER == __BIG_ENDIAN
    u_int8_t th_off:4;          /* data offset */
    u_int8_t th_x2:4;           /* (unused) */
#  endif
    char th_flags;
#  define TH_FIN        0x01
#  define TH_SYN        0x02
#  define TH_RST        0x04
#  define TH_PUSH       0x08
#  define TH_ACK        0x10
#  define TH_URG        0x20
    unsigned short th_win;           /* window */
    unsigned short th_sum;           /* checksum */
    unsigned short th_urp;           /* urgent pointer */
}tcphdr_t;


typedef struct mplshdr_s
{
  unsigned int label[MAX_MPLS_LABELS];  
  uint8_t pri[MAX_MPLS_LABELS];
}mplshdr_t;

typedef struct vxlanBool_s
{
  bool vNID_set;
  bool is_l2_encapsulated;  
}vxlanBool_t;

typedef struct greBool_s
{
  bool ipv6;  
  bool ipv4;  
}greBool_t;

typedef struct pktHeader_s
{
  ethernetHeader_t      ethHeader;
  enetTag_t             ethTag;
  ipHeader_t            ipHeader;
  ipHeader_t            greIpHeader;
  tcphdr_t              tcpHeader;
  udphdr_t              udpHeader; 
  struct ip6_hdr        ip6hdr;
  struct ip6_hdr        greIp6hdr;
  mplshdr_t             mplsHeader; 
  vxlanHeader_t         vxlanHeader; 
  greHeader_t           greHeader; 
  ethernetHeader_t      vxlanEthHeader;
  enetTag_t             vxlanEthTag;
  ethBool_t             ethFlag;
  enetTagBool_t         enetTagFlag;
  ethBool_t             vxlanEthFlag;
  enetTagBool_t         vxlanEnetTagFlag;
  ipBool_t              ipFlag;
  ipBool_t              greIpFlag;
  udpBool_t             udpFlag;
  tcpBool_t             tcpFlag;
  ipv6Bool_t            ipv6Flag;
  ipv6Bool_t            greIpv6Flag;
  mplsLabelBool_t       mplsFlag;
  vxlanBool_t           vxlanFlag;
  greBool_t             greFlag;
}pktHeader_t;
 

 
typedef enum pktType
{
  PKT_TYPE_ETHERNET = 0,
  PKT_TYPE_IP,
  PKT_TYPE_TCP,
  PKT_TYPE_UDP,
  PKT_TYPE_IPv6,
  PKT_TYPE_TCPv6,
  PKT_TYPE_UDPv6,
  PKT_TYPE_IP_MULTICAST,
  PKT_TYPE_IPv4_VXLAN,
  PKT_TYPE_IPv6_VXLAN,
  PKT_TYPE_IPv4_GRE,
  PKT_TYPE_IPv6_GRE,
  PKT_TYPE_MAX
}PTAPP_PKT_TYPE_t;


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

void default_l2_packet_create(pktHeader_t *pktHeader, char *l2DefPkt);

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

void default_l3_packet_create(PTAPP_PKT_TYPE_t type,pktHeader_t *pktHeader,char *l3DefPkt,int pktLen);

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

void default_l4Udp_packet_create(PTAPP_PKT_TYPE_t type,pktHeader_t *pktHeader,char *l4UdpDefPkt,int pktLen);

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

void default_l4Tcp_packet_create(PTAPP_PKT_TYPE_t type,pktHeader_t *pktHeader,char *l4TcpDefPkt,int pktLen);

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
                               char *offset,unsigned int offsetLoc, unsigned int offsetLen);


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

void crc32_lookup_init (void);


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

int crc32_calculate (char * buffer, int length);


/*********************************************************************
* @brief        Function used to calculate checksum
*
* @param[in]    pData Buffer for which checksum is needed
* @param[in]    len   Buffer length
* @param[in]    csum  Initial checksum
*
* @retval       checksum
*
* @note         NA
*
* @end
*********************************************************************/

short header_checksum(char *pData, unsigned int len);


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

void default_l3V6_packet_create(pktHeader_t *pktHeader,char *l3IpV6Pkt);

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

void default_mpls_create(pktHeader_t *pktHeader, char *l2DefPkt);

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
                        uint8_t ttl, uint8_t *updatedHeader);

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

void offset_apply(char *addr,char *offset, unsigned int offsetLoc,unsigned int offsetLen);

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

short ipv4_checksum_pseudo_header(char *addr,unsigned int length,pktHeader_t *pktHeader);

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

short ipv6_checksum_pseudo_header(char *addr,unsigned int length,pktHeader_t *pktHeader);

/*********************************************************************
* @brief            Function to create UDP Packet with VXLAN Payload
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
                         char *offset,unsigned int offsetLoc, unsigned int offsetLen);

/*********************************************************************
* @brief            Function to create vxlan packet
*
* @param[in,out]    pktHeader   Packet details
* @param[out]       vxlanDefPkt Buffer which will have the VXLAN packet 
* @param[in ]       pktLen      Total Packet length 
*
* @retval           NA
*
* @note             NA
*
* @end
*********************************************************************/

void default_vxlan_packet_create(pktHeader_t *pktHeader,char *vxlanDefPkt,int pktLen);

/*********************************************************************
* @brief            Function to create gre packet
*
* @param[in,out]    pktHeader   Packet details
* @param[out]       greDefPkt   Buffer which will hve the entire GRE packet
* @param[in ]       pktLen      Total Packet length 
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/

void default_gre_packet_create(pktHeader_t *pktHeader,char *greDefPkt,int pktLen);

/*********************************************************************
* @brief            Function to create GRE
*
* @param[in,out]    pktHeader   Packet details
* @param[out]       l2pktBuf    Packet which will have the entire GRE packet 
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
                         char *offset,unsigned int offsetLoc, unsigned int offsetLen);


#endif	/* INCLUDE_PKT_LIB_H */

