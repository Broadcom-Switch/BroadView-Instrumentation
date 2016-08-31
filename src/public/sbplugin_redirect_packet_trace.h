/*! \file sbplugin_redirect_packet_trace.h
 */

/*! @addtogroup GLUSMDB API Definitions
 *  @{
 *    @addtogroup GLBVIEWSBREDIRECTORPT BroadView Packet Trace SB Redirector Feature Declarations And Definitions [BVIEW SB REDIRECTOR]
 *    @{
 */
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
#ifndef INCLUDE_SBPLUGIN_REDIRECT_PACKET_TRACE_H
#define INCLUDE_SBPLUGIN_REDIRECT_PACKET_TRACE_H  
/*#include "sbplugin_redirect.h"*/
#include "sbfeature_packet_trace.h"
#include "sbplugin_redirect_system.h"

/*********************************************************************//**
* @brief       Get Packet Trace configuration
*
* @param[in]    asic                  Unit number
* @param[out]   config                Packet Trace config structure
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     Packet Trace feature is not present or
*                                     Packet Trace south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Packet Trace config get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Packet Trace config get functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_pt_drop_reason_config_get_cb (int asic, 
                                BVIEW_PT_DROP_REASON_CONFIG_t * config);

/*********************************************************************//**
* @brief       Set Packet Trace configuration Paramters.
*
* @param[in]    asic                  Unit number
* @param[in]    config                Packet Trace config structure
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     Packet Trace feature is not present or
*                                     Packet Trace south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Packet Trace config set is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Packet Trace config set functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_pt_drop_reason_config_set (int asic, 
                                 BVIEW_PT_DROP_REASON_CONFIG_t *config);

/*********************************************************************//**
* @brief  Get Mask of ASIC Configurable drop reasons.
*
* @param[in]    asic             unit
* @param[out]   mask             Mask of drop reasons
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     Packet Trace feature is not present or
*                                     Packet Trace south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Device stat get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Device stat get functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_pt_asic_supported_drop_reason_get_cb (int asic,
                                     BVIEW_PT_DROP_REASON_MASK_t *mask);

/*********************************************************************//**
* @brief  Obtain Trace Profile for a given packet.
*
* @param[in]    asic              unit
* @param[out]   traceProfile      Trace Profile Data Struture             
* @param[out]   time              time
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     Packet Trace feature is not present or
*                                     Packet Trace south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Ingress Port + Priority Groups 
*                                     stat get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Ingress Port + Priority Groups 
*                                     stat get functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_pt_trace_profile_get_cb (int asic,
                                      int  port,
                                      BVIEW_PT_PACKET_t *packet,
                                      BVIEW_PT_TRACE_PROFILE_t *traceProfile,
                                      BVIEW_TIME_t * time);

/*********************************************************************//**
* @brief  Obtain Drop Counter Report for all Drop Reasons.
*
* @param[in]    asic             unit
* @param[out]   data             i_p_sp data structure
* @param[out]   time             time
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     Packet Trace feature is not present or
*                                     Packet Trace south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Ingress Port + Service Pools 
*                                     stat get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Ingress Port + Service Pools 
*                                     stat get functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_pt_drop_counter_get_cb (int asic,
                                      BVIEW_PT_DROP_COUNTER_REPORT_t *data,
                                      BVIEW_TIME_t * time);

/*********************************************************************//**
* @brief  Clear all configured Drop Reasons.
*
* @param[in]    asic              unit
* @param[out]   data              i_sp structure
* @param[out]   time              time
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     Packet Trace feature is not present or
*                                     Packet Trace south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Ingress Service Pools 
*                                     stat get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Ingress Service Pools 
*                                     stat get functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_pt_clear_drop_reason_cb (int asic);

/*********************************************************************//**
* @brief  Register hw trigger callback
*
*
* @param[in] asic                  unit
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     Packet Trace feature is not present or
*                                     Packet Trace south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Callback is successfully registered 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Trigger callback registration is not supported 
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_pt_register_trigger_cb (int asic,
                                         BVIEW_PT_TRIGGER_CALLBACK_t callback,
                                         void *cookie);

/*********************************************************************
* @brief  Create a 5 tuple profile/rule in HW to capture live traffic.
*
* @param[in]    asic              unit
* @param[in]    request_id        request id of the profile
* @param[in]    port_list         List of ports on which this rule has to be applied
* @param[in]    tuple_params      5 tuple params
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or
*                                     Not able to get asic type of this unit or
*                                     Packet Trace feature is not present or
*                                     Packet Trace south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Profile is created in HW
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Profile creation for this 5 tuple is
*                                     not supported on this unit
*
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_pt_5_tuple_profile_create (int asic, int request_id,
                                BVIEW_PORT_MASK_t *port_list, PT_5_TUPLE_PARAMS_t *tuple_params);

/*********************************************************************
* @brief  Delete 5 tuple profile/rule from HW.
*
* @param[in]    asic              unit
* @param[in]    request_id        request id of the profile
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or
*                                     Not able to get asic type of this unit or
*                                     Packet Trace feature is not present or
*                                     Packet Trace south bound function has returned failure
*                                     Failed to delete 5 tuple rule
*
* @retval   BVIEW_STATUS_SUCCESS      Profile is deleted
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Profile deletion for this request id is
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_pt_5_tuple_profile_delete (int asic, int request_id);

#endif
