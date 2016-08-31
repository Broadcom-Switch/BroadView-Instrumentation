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

#ifndef INCLUDE_SBPLUGIN_SYSTEM_MAP_H
#define INCLUDE_SBPLUGIN_SYSTEM_MAP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sbplugin.h"
#include "sbplugin_ovsdb.h"

#define  SBPLUGIN_NETWORK_OS   "ovsdb"
 
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

#define    BVIEW_OVSDB_PORT_GET(_port)   \
               (_port = _port);


/* Get OVSDB asic number from Application ASIC number.*/
#define    BVIEW_OVSDB_ASIC_GET(_asic) \
               ((_asic) = asicMap[_asic]);

/* Get Applciation ASIC number from OVSDB ASIC number*/
#define    BVIEW_OVSDB_APP_ASIC_GET(_asic) \
               ((_asic) = asicAppMap[_asic]);

extern BVIEW_ASIC_t                 asicDb[];

/*********************************************************************
* @brief  OVSDB South Bound - SYSTEM feature init
*
* @param[in,out]   ovsdbSystemFeat   -  system data structure
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
BVIEW_STATUS sbplugin_ovsdb_system_init (BVIEW_SB_SYSTEM_FEATURE_t *ovsdbSystemFeat);

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
BVIEW_STATUS  sbplugin_ovsdb_system_name_get (char *buffer, int length);

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
BVIEW_STATUS  sbplugin_ovsdb_system_mac_get (unsigned char *buffer, 
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
BVIEW_STATUS sbplugin_ovsdb_system_ipv4_get (unsigned char *buffer, 
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
BVIEW_STATUS sbplugin_ovsdb_system_time_get (time_t *time);

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
BVIEW_STATUS sbplugin_ovsdb_system_asic_translate_from_notation (char *src, 
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
BVIEW_STATUS sbplugin_ovsdb_system_port_translate_from_notation (char *src,
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
BVIEW_STATUS sbplugin_ovsdb_system_asic_translate_to_notation (int asic,
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
BVIEW_STATUS sbplugin_ovsdb_system_port_translate_to_notation (int asic,
                                                            int port,
                                                            char *dst);

/*********************************************************************
* @brief   Verify whether a given unit number is valid or not.
*
* @param[in]   unit                     -  unit number
*
* @retval  BVIEW_STATUS_SUCCESS            if unit number is valid
* @retval  BVIEW_STATUS_FAILURE            if unit number is invalid
*
* @notes
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_valid_unit_check(unsigned int unit);

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
BVIEW_STATUS  sbplugin_ovsdb_system_network_os_get (uint8_t *buffer, int length);

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_SBPLUGIN_SYSTEM_MAP_H */

