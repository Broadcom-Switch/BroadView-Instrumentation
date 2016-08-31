/*! \file sbplugin_redirect_system.h
 */

/*! @addtogroup GLUSMDB API Definitions
 *  @{
 *    @addtogroup GLBVIEW This File Contains System Feature Related South Bound APIs [BVIEW]
 *    @{
 */
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

#ifndef INCLUDE_SBPLUGIN_REDIRECT_SYSTEM_H
#define	INCLUDE_SBPLUGIN_REDIRECT_SYSTEM_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include "broadview.h"
#include "openapps_feature.h"
#include "system.h"
#include "asic.h"


/*****************************************************************//**
* @brief       Get asic capabilites of a unit
*
* @param[in]   asic          Unit number
* @param[out]  asicCapPtr    Pointer to asic capabilities  
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Found asic capabilities
*
*********************************************************************/
BVIEW_STATUS sbapi_system_asic_capabilities_get (int asic, 
                                          BVIEW_ASIC_CAPABILITIES_t *asicCapPtr);

/*****************************************************************//**
* @brief       Get number of units in the system
*
* @param[out]  unitNum            Numbers of units
*
* @retval   BVIEW_STATUS_FAILURE   When lock acquistion fails or
*                                  System feature is not avaialable or
*                                  The number of units is '0'
*
* @retval   BVIEW_STATUS_SUCCESS   Read number of units from system feature
*                                  handler sucessfully
*
*********************************************************************/
BVIEW_STATUS sbapi_system_num_units_get(int *unitNum);

/*****************************************************************//**
* @brief       Get asic type of a  unit
*
* @param[in]  asic        Unit number
* @param[out]  asicType    Asic type of unit
*
* @retval   BVIEW_STATUS_FAILURE   When lock acquistion fails or
*                                  System feature is not avaialable or
*                                  The number of units is '0' or 
*                                  unit entry is not found
*
* @retval   BVIEW_STATUS_SUCCESS   Able to convert unit number to asic type
*                                  sucessfully
*
*********************************************************************/
BVIEW_STATUS sbapi_system_unit_to_asic_type_get(int asic,  BVIEW_ASIC_TYPE  *asicType);

/*****************************************************************//**
* @brief       Get system name
*
* @param[out]  buffer        System name
* @param[in]   length        Buffer length
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     System feature is not present or
*                                     System south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      System south bound function for name get is
*                                     successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  System name get functionality is 
*                                     not supported on this unit
*
*********************************************************************/
BVIEW_STATUS  sbapi_system_name_get(char *buffer, int length);

/*****************************************************************//**
* @brief       Get system MAC address
*
* @param[out]  buffer        System MAC address
* @param[in]   length        Buffer length
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     System feature is not present or
*                                     System south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      System south bound function for mac get is
*                                     successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  System mac get functionality is 
*                                     not supported on this unit
*
*********************************************************************/
BVIEW_STATUS  sbapi_system_mac_get(uint8_t *buffer, int length);

/*****************************************************************//**
* @brief       Get system IPv4 address
*
* @param[out]  buffer        IPv4 address
* @param[in]   length        Buffer length
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     System feature is not present or
*                                     System south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      System south bound function for ipv4 get is
*                                     successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  System  ipv4 get functionality is 
*                                     not supported on this unit
*
*********************************************************************/
BVIEW_STATUS  sbapi_system_ip4_get(uint8_t *buffer, int length);

/*****************************************************************//**
* @brief       Get system asic number from notational asic number
*                                  representation - string form
*
* @param[in]   src           Asic number in notational(string) form
* @param[out]  asic        System asic number 
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     System feature is not present or
*                                     System south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      System south bound function for name get is
*                                     successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  System name get functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_asic_translate_from_notation(char *src, int *asic);

/*******************************************************************//**
* @brief       Get asic number in notational representation(string) 
*                from system asic number
*
* @param[in]  asic        System asic number 
* @param[out] dst         Asic number in notational(string) form
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     System feature is not present or
*                                     System south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      System south bound function for name get is
*                                     successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  System name get functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_asic_translate_to_notation(int asic, char *dst);

/*******************************************************************//**
* @brief       Get system port number from notational port number
*                                  representation - string form
*
* @param[in]   src         Port number in notational(string) form
* @param[out]  port        System port number 
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     System feature is not present or
*                                     System south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      System south bound function for name get is
*                                     successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  System name get functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_port_translate_from_notation(char *src, int *port);

/*******************************************************************//**
* @brief       Get port number in notational representation(string) 
*                from system port number and asic number
*
* @param[in]  asic        System asic number 
* @param[in]  port        System port number 
* @param[out] dst         Port number in notational(string) form
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     System feature is not present or
*                                     System south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      System south bound function for name get is
*                                     successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  System name get functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_port_translate_to_notation(int asic, int port, char *dst);

  /* Acquire Read lock */
/*******************************************************************//**
* @brief       Get number of ports in a unit
*
* @param[in]   asic          Unit number
* @param[out]  numPorts      Number of ports
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Get number of ports is successful
*
*********************************************************************/
BVIEW_STATUS  sbapi_system_asic_num_ports_get(int asic, int *numPorts);

/*****************************************************************//**
* @brief       Get number of unicast queues in a unit
*
* @param[in]   asic                  Unit number
* @param[out]  numUnicastQueues      Number of Unicast queues
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Get number of unicast queues is successful
*
*********************************************************************/
BVIEW_STATUS  sbapi_system_asic_num_ucq_get(int asic, int *numUnicastQueues);

/*****************************************************************//**
* @brief       Get number of unicast queue groups in a unit
*
* @param[in]   asic                    Unit number
* @param[out]  numUnicastQueueGroups   Number of Unicast queue groups
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Get number of unicast queue groups 
*                                     is successful
*
*********************************************************************/
BVIEW_STATUS  sbapi_system_asic_num_ucqg_get(int asic, int *numUnicastQueueGroups);

/*****************************************************************//**
* @brief       Get number of multicast queues in a unit
*
* @param[in]   asic                    Unit number
* @param[out]  numMulticastQueueGroups   Number of Multicast queues
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Get number of multicast queues 
*                                     is successful
*
*********************************************************************/
BVIEW_STATUS  sbapi_system_asic_num_mcq_get(int asic, int *numMulticastQueues);

/*****************************************************************//**
* @brief       Get number of service pools in a unit
*
* @param[in]   asic                    Unit number
* @param[out]  numServicePools         Number of Service pools
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Get number of service pools 
*                                     is successful
*
*********************************************************************/
BVIEW_STATUS  sbapi_system_asic_num_sp_get(int asic, int *numServicePools);

/*****************************************************************//**
* @brief       Get number of common pools in a unit
*
* @param[in]   asic                  Unit number
* @param[out]  numCommonPools        Number of common pools
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Get number of common pools 
*                                     is successful
*
*********************************************************************/
BVIEW_STATUS  sbapi_system_asic_num_cp_get(int asic, int *numCommonPools);

/*****************************************************************//**
* @brief       Get number of cpu queues in a unit
*
* @param[in]   asic                Unit number
* @param[out]  numCpuQueues        Number of cpu queues
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Get number of cpu queues 
*                                     is successful
*
*********************************************************************/
BVIEW_STATUS  sbapi_system_asic_num_cpuq_get(int asic, int *numCpuQueues);

/*****************************************************************//**
* @brief       Get number of RQE queues in a unit
*
* @param[in]   asic                Unit number
* @param[out]  numRqeQueues        Number of RQE queues
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Get number of RQE queues 
*                                     is successful
*
*********************************************************************/
BVIEW_STATUS  sbapi_system_asic_num_rqeq_get(int asic, int *numRqeQueues);

/*****************************************************************//**
* @brief       Get number of RQE queue pools in a unit
*
* @param[in]   asic                Unit number
* @param[out]  numRqeQueuePools    Number of RQE queue pools
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Get number of RQE queue pools
*                                     is successful
*
*********************************************************************/
BVIEW_STATUS  sbapi_system_asic_num_rqeqp_get(int asic, int *numRqeQueuePools);

/*****************************************************************//**
* @brief       Get number of priority groups in a unit
*
* @param[in]   asic                 Unit number
* @param[out]  numPriorityGroups    Number of priority groups
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Get number of priority groups
*                                     is successful
*
*********************************************************************/
BVIEW_STATUS  sbapi_system_asic_num_pg_get(int asic, int *numPriorityGroups);

/*****************************************************************//**
* @brief       Get whether a unit supports 1588 or not
*
* @param[in]   asic                 Unit number
* @param[out]  support1588          Support for 1588
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Unit's capability for 1588 is successfully 
*                                     queried
*
*********************************************************************/
BVIEW_STATUS  sbapi_system_asic_support_1588_get(int asic, bool *support1588);

/*********************************************************************//**
* @brief       Get Supported Feature Mask
*
* @param[out]  featureMask            Supported Feature Mask
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Supported Feature Mask is successfully
*                                     queried
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS  sbapi_system_feature_mask_get (int *featureMask);

/*********************************************************************//**
* @brief       Get Network OS
*
* @param[out]  buffer                 Pointer to network OS String
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
BVIEW_STATUS  sbapi_system_network_os_get (uint8_t *buffer, int length);

/*********************************************************************//**
* @brief       Get system UID 
*
* @param[out]  buffer                 Pointer to UID String
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure 
*                                     Failed to get network os
*
* @retval   BVIEW_STATUS_SUCCESS      UID is successfully
*                                     queried
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS  sbapi_system_uid_get (uint8_t *buffer, int length);

/*********************************************************************//**
* @brief  Get Current local time.
*
* @param[out] tm                          - Pointer to tm structure
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     System feature is not present or
*                                     System south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      System south bound function for time get is
*                                     successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  System time get functionality is 
*                                     not supported on this unit
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_time_get (time_t *ptime);


/*********************************************************************//**
* @brief       Get lag number in notational representation(string) 
*                from system lag number and asic number
*
* @param[in]  asic        System asic number 
* @param[in]  lag         System lag number 
* @param[out] dst         lag number in notational(string) form
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     System feature is not present or
*                                     System south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      System south bound function for name get is
*                                     successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  System name get functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_lag_translate_to_notation(int asic, int lag, char *dst);

/*********************************************************************//**
* @brief  Get snapshot of max buffers allocated 
*
*
* @param[in]   asic                          unit
* @param[out]  maxBufSnapshot                Max buffers snapshot
* @param[out]  time                          time
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     system feature is not present or
*                                     System south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Snapshot get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Snapshot get functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_system_max_buf_snapshot_get (int asic,
                                      BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *maxBufSnapshot,
                                      BVIEW_TIME_t * time);

/*********************************************************************
* @brief  Register with silicon for CPU bound packets
*
*
* @param[in]   asic                          ASIC number
* @param[in]   callback                      Function to called when
*                                            packet is recived.
* @param[int]  cookie                        cookie
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or
*                                     Not able to get asic type of this unit or
*                                     system feature is not present or
*                                     System south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Registration is successful
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Packet RX registration is not supported 
*                                    
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_system_packet_rx_register (int asic,
                                              BVIEW_PACKET_RX_CALLBACK_t callback,
                                              char *name,
                                              void *cookie);

#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_SBPLUGIN_REDIRECT_SYSTEM_H */

/*!  @}
 * @}
 */
