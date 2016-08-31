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

#ifndef INCLUDE_SBPLUGIN_COMMON_SYSTEM_H
#define INCLUDE_SBPLUGIN_COMMON_SYSTEM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common/platform_spec.h"
#include "sbplugin.h"
#include "sbplugin_common.h"
#include "sbplugin_system_map.h"
#include "libxml/parser.h"
#include "libxml/xpath.h"
#include "libxml/xpathInternals.h"
#include "libxml/tree.h"

/* SYSTEM feature supported ASIC's Mask.*/
#define  BVIEW_SYSTEM_SUPPORT_MASK   (BVIEW_ASIC_TYPE_ALL)

#define BVIEW_TD2_NUM_UC_QUEUE        2960
#define BVIEW_TD2_NUM_UC_QUEUE_GRP    128
#define BVIEW_TD2_NUM_MC_QUEUE        1040
#define BVIEW_TD2_NUM_SP              4
#define BVIEW_TD2_NUM_COMMON_SP       1
#define BVIEW_TD2_NUM_RQE             11
#define BVIEW_TD2_NUM_RQE_POOL        4
#define BVIEW_TD2_NUM_PG              8
#define BVIEW_TD2_1588_SUPPORT        1
#define BVIEW_TD2_CPU_COSQ            8
#define BVIEW_TD2_CELL_TO_BYTE        208

#define  BVIEW_SYSTEM_NUM_COS_PORT    8
/* Driver port number is same as application port number.
 * if the application port numbering is different from the driver port numbering then
 * this need to define to get proper BCM port number.
 */
#define    BVIEW_BCM_PORT_GET(_port)   \
               (_port = _port);


/* Get BCM asic number from Application ASIC number.*/
#define    BVIEW_BCM_ASIC_GET(_asic) \
               ((_asic) = asicMap[_asic]);

/* Get Applciation ASIC number from BCM ASIC number*/
#define    BVIEW_BCM_APP_ASIC_GET(_asic) \
               ((_asic) = asicAppMap[_asic]);

#define    BVIEW_BCM_LAG_GET(_port)   \
               (_port = _port+1);

/* Macro to iterate all ports*/
#define  BVIEW_SYSTEM_PORT_ITER(_asic,_port)                                         \
              for ((_port) = 1; (_port) <= asicDb[(_asic)].scalingParams.numPorts; (_port)++)

/* Macro to iterate all Priority Groups*/
#define  BVIEW_SYSTEM_PG_ITER(_pg)                                                 \
              for ((_pg) = 0; (_pg) < BVIEW_ASIC_MAX_PRIORITY_GROUPS; (_pg)++)

/* Macro to iterate all Service Pools*/                           
#define  BVIEW_SYSTEM_SP_ITER(_sp)                                                 \
              for ((_sp) = 0; (_sp) < BVIEW_ASIC_MAX_SERVICE_POOLS; (_sp)++)

/* Macro to iterate all Service Pools + Common Service Pool*/
#define  BVIEW_SYSTEM_TOTAL_SP_ITER(_sp)                                           \
              for ((_sp) = 0; (_sp) < BVIEW_ASIC_MAX_INGRESS_SERVICE_POOLS; (_sp)++)


/* Macro to iterate 'n'  times*/
#define BVIEW_SYSTEM_ITER(_index,_n)                              \
              for ((_index) = 0; (_index) < (_n); (_index)++)

#define BST_MAX_BUF_CFG_FILE_NAME  "BstMaxBufConfig.xml"
#define BST_MAX_BUF_CFG_SCHEMA_FILE_NAME  "BstMaxBufConfigSchema.xsd"
#define SB_XML_CONTENT_BASE         10

/* User Inserted Header Maximum size in Qumran */
#define BVIEW_MAX_USER_HEADER_SIZE  8

/* Realm Function handlers that should be called to process the xml data
 * and to update the snapshot of max bufs 
 */
typedef BVIEW_STATUS(*sb_system_realm_xml_handler_t) (xmlNode *node,
                      BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot);

/* Structure of Realm to XML data handler function map */
typedef struct sb_system_realm_xml_handler_map_s
{
  char realm[256];
  sb_system_realm_xml_handler_t handler;
}SB_SYSTEM_REALM_XML_HANDLER_MAP_t;

/* Structure of port-cosq-gport map */
typedef struct sb_port_cosq_gports_map_
{
  SB_BRCM_GPORT_t ucastGport[BVIEW_SYSTEM_PORT_MAX_UCAST_QUEUES];
  SB_BRCM_GPORT_t mcastGport[BVIEW_SYSTEM_PORT_MAX_MCAST_QUEUES];
} SB_PORT_COSQ_GPORTS_MAP_t;

/* Structure of cosq gports  map*/
typedef struct sb_system_cosq_gports_map_
{
  SB_PORT_COSQ_GPORTS_MAP_t portInfo[BVIEW_ASIC_MAX_PORTS+1];
} SB_SYSTEM_COSQ_GPORTS_MAP_t;

extern BVIEW_ASIC_t                 asicDb[];

/*********************************************************************
* @brief  South Bound - SYSTEM feature init
*
* @param[in,out]   bcmSystem   -  system data structure
*
* @returns BVIEW_STATUS_SUCCESS  if intialization is success
*          BVIEW_STATUS_FAILURE  if intialization is fail
*
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_init (BVIEW_SB_SYSTEM_FEATURE_t *bcmSystem);

/*********************************************************************
* @brief  Get the system name
*
* @param[out] buffer                         - buffer
* @param[in]  length                         - length of the buffer
*
* @retval  BVIEW_STATUS_SUCCESS            if Name get is success.
* @retval  BVIEW_STATUS_FAILURE            if Name get is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_common_system_name_get (char *buffer, int length);

/*********************************************************************
* @brief  Get the MAC address of the system
*
* @param[out] buffer                         - buffer
* @param[in]  length                         - length of the buffer
*
* @retval  BVIEW_STATUS_SUCCESS            if MAC get is success.
* @retval  BVIEW_STATUS_FAILURE            if MAC get is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes    Get MAC address of the service port.
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_common_system_mac_get (unsigned char *buffer, 
                                           int length);

/*********************************************************************
* @brief  Get the IP address of system
*
* @param[out] buffer                         - buffer
* @param[in]  length                         - length of the buffer
*
* @retval  BVIEW_STATUS_SUCCESS            if IP get is success.
* @retval  BVIEW_STATUS_FAILURE            if IP get is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes    Get IP address of service port.
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_ipv4_get (unsigned char *buffer, 
                                           int length);

/*********************************************************************
* @brief  Get Current local time. 
*
* @param[out] tm                          - Pointer to tm structure
*
* @retval  BVIEW_STATUS_SUCCESS            if time get is success.
* @retval  BVIEW_STATUS_FAILURE            if time get is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes    Get IP address of service port.
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_time_get (time_t *time);

/*********************************************************************
* @brief  Translate ASIC String notation to ASIC Number.
*
* @param[in]  src                         - ASIC ID String
* @param[out] asic                        - ASIC Number
*
* @retval  BVIEW_STATUS_SUCCESS            if ASIC Translation is success.
* @retval  BVIEW_STATUS_FAILURE            if ASIC Translation is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes    Get IP address of service port.
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_asic_translate_from_notation (char *src, 
                                                               int *asic);

/*********************************************************************
* @brief  Translate Port String notation to Port Number.
*
* @param[in]   src                         - Port ID String
* @param[out]  port                        - PortId
*
* @retval  BVIEW_STATUS_SUCCESS            if Port Tranlate is success.
* @retval  BVIEW_STATUS_FAILURE            if Port  is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_port_translate_from_notation (char *src,
                                                               int *port);

/*********************************************************************
* @brief  Translate ASIC number to ASIC string notation.
*
* @param[in]   asic                         - ASIC ID
* @param[out]  dst                          - ASIC ID String
*
* @retval  BVIEW_STATUS_SUCCESS            if ASIC ID Tranlate is success.
* @retval  BVIEW_STATUS_FAILURE            if ASIC ID Tranlate is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_asic_translate_to_notation (int asic,
                                                             char *dst);

/*********************************************************************
* @brief   Translate port number to port string notation.
*
* @param[in]   port                         - Port Number
* @param[in]   asic                         - ASIC
* @param[out]  dst                          - ASIC String
*
* @retval  BVIEW_STATUS_SUCCESS            if Port Tranlate is success.
* @retval  BVIEW_STATUS_FAILURE            if Port Tranlate is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes    Get IP address of service port.
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_port_translate_to_notation (int asic,
                                                            int port,
                                                            char *dst);

/*********************************************************************
* @brief       Get Network OS
*
* @param[out]  buffer                 Pointer to network OS String
* @param[in]   length                 length of the buffer
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure
*                                     Failed to get network os
*
* @retval   BVIEW_STATUS_SUCCESS      Network OS is successfully
*                                     queried
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS  sbplugin_common_system_network_os_get (uint8_t *buffer, int length);

/*********************************************************************
* @brief  Get the UID of the system
*
* @param[out] buffer                         - buffer
* @param[in]  length                         - length of the buffer
*
* @retval  BVIEW_STATUS_SUCCESS            if UID get is success.
* @retval  BVIEW_STATUS_FAILURE            if UID get is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes   get the UID of the system 
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_uid_get (unsigned char *buffer, 
                                           int length);

/*********************************************************************
* @brief   Translate lag number to lag string notation.
*
* @param[in]   asic                         - ASIC 
* @param[in]   lag                          - lag Number
* @param[out]  dst                          - ASIC String
*
* @retval  BVIEW_STATUS_SUCCESS            if lag Tranlate is success.
* @retval  BVIEW_STATUS_FAILURE            if lag Tranlate is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes  
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_lag_translate_to_notation (int asic, 
                                                             int lag, 
                                                             char *dst);
/*********************************************************************
* @brief  Get snapshot of max buffers allocated  
*
*
* @param  [in]  asic                         - unit
* @param[out]  maxBufSnapshot                - Max buffers snapshot
* @param[out]  time                          - time
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if snapshot is succes.
* @retval BVIEW_STATUS_SUCCESS           if snapshot set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_max_buf_snapshot_get (int asic, 
                              BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *maxBufSnapshot,
                              BVIEW_TIME_t * time);

/*********************************************************************
* @brief  Register with silicon for CPU bound packets
*
*
* @param  [in]  asic                - unit
* @param  [in]  callback            - Function to be called when packet
*                                     is received
* @param  [in]  name                - Name of the function
* @param  [in]  cookie              - Cookie
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if registration is failed
* @retval BVIEW_STATUS_SUCCESS           if snapshot set is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS   sbplugin_common_system_packet_rx_register (int asic,
                                BVIEW_PACKET_RX_CALLBACK_t callback,
                                char *name,
                                void *cookie);

/*********************************************************************
* @brief  Get the sizes of user headers in bits
*         
*
* @param  [in]  asic                - unit
* @param  [out  user_header0        - Length of User Header 0
* @param  [out] user_header1        - Length of User Header 1
* @param  [out] user_header2        - Length of User Header 2
* @param  [out] user_header3        - Length of User Header 3
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_SUCCESS           if snapshot set is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_user_header_size_get (int asic,
                                            int *user_header0,
                                            int *user_header1,
                                            int *user_header2,
                                            int *user_header3);

/*********************************************************************
* @brief  Packet Receive and notify the applciation
*
*
* @param  [in]  asic                   - unit
* @param  [in]  bcm_pkt                - Pointer to ASIC Packet structure
* @param  [in]  cookie                 - Cookie
*
* @retval SB_BRCM_RX_HANDLED     Packet is handled.
*
* @notes    none
*
*
*********************************************************************/
SB_BRCM_RX_t   sbplugin_common_system_receive (int asic,
                                               SB_BRCM_PKT_t *bcm_pkt,
                                               void *cookie);


/*********************************************************************
* @brief   Get the chip type
*
* @param[in]   unit              - unit
* @param[out]  asicType          - ASIC type
*
* @retval BVIEW_STATUS_SUCCESS            if chip get is success.
* @retval BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_util_chip_type_get (unsigned int unit,
                                                BVIEW_ASIC_TYPE *asicType);

/*********************************************************************
* @brief   Get the gport of a unicast cosQ
*
* @param[in]   asic              - unit
* @param[in]   port              - port number
* @param[in]   cosq              - cosq number
* @param[out]  gport             - cosq gport number
*
* @retval BVIEW_STATUS_SUCCESS            if the gport get is successful .
* @retval BVIEW_STATUS_INVALID_PARAMETER  if input parameters are invalid.
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_ucastq_gport_get(int asic, int port, int cosq,
                                             SB_BRCM_GPORT_t *gport);


/*********************************************************************
* @brief   Get the gport of a multicast cosQ
*
* @param[in]   asic              - unit
* @param[in]   port              - port number
* @param[in]   cosq              - cosq number
* @param[out]  gport             - cosq gport number
*
* @retval BVIEW_STATUS_SUCCESS            if the gport get is successful .
* @retval BVIEW_STATUS_INVALID_PARAMETER  if input parameters are invalid.
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_mcastq_gport_get(int asic, int port, int cosq,
                                             SB_BRCM_GPORT_t *gport);


#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_SBPLUGIN_COMMON_SYSTEM_H */

