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

#ifndef INCLUDE_SBPLUGIN_COMMON_PACKET_TRACE_H
#define INCLUDE_SBPLUGIN_COMMON_PACKET_TRACE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "broadview.h"
#include "sbfeature_packet_trace.h"

/** Packet Trace feature support ASIC's Mask*/
#define  BVIEW_PACKET_TRACE_SUPPORT_MASK    (BVIEW_ASIC_TYPE_TH)


/*********************************************************************
* @brief  BCM Packet Trace Plugin feature init
*
* @param[in,out]  bcmPT     - Packet Trace feature data structure
*
* @retval   BVIEW_STATUS_SUCCESS if BST feature is
*                                initialized successfully.
* @retval   BVIEW_STATUS_FAILURE if initialization is failed.
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_packet_trace_init (BVIEW_SB_PT_FEATURE_t *bcmPT);

/*********************************************************************
* @brief  Configure Drop reasons.
*
* @param[in]   asic                  - unit
* @param[in]   config                - Drop Reason config structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if config set is failed.
* @retval BVIEW_STATUS_SUCCESS           if config set is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_pt_drop_reason_config_set (int asic,
                                           BVIEW_PT_DROP_REASON_CONFIG_t *config);

/*********************************************************************
* @brief  Get drop reasons configured.
*
* @param[in]   asic                  - unit
* @param[out]  config                - Drop Reason config structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if config get is failed.
* @retval BVIEW_STATUS_SUCCESS           if config get is success.
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_pt_drop_reason_config_get (int asic,
                                            BVIEW_PT_DROP_REASON_CONFIG_t *config);

/*********************************************************************
* @brief  Obtain ASIC List of supported drop reasons.
*
* @param[in]      asic               - unit
* @param[out]     mask               - Mask of drop reasons
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if snapshot get is failed.
* @retval BVIEW_STATUS_SUCCESS           if snapshot get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_pt_supported_drop_reason_get (int asic,
                                BVIEW_PT_DROP_REASON_MASK_t *mask);

/*********************************************************************
* @brief  Obtain Trace-profile for a give packet and Port
*
* @param[in]   asic             - unit
* @param[in]   Port             - Port
* @param[in]   Packet           - Packet Structure
* @param[out]  traceProfile     - Complete Trace Profile
* @param[out]  time             - time
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if device stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if device stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_pt_trace_profile_get (int asic,
                                    int  port,
                                    BVIEW_PT_PACKET_t *packet,
                                    BVIEW_PT_TRACE_PROFILE_t *traceProfile,
                                    BVIEW_TIME_t *time);

/*********************************************************************
* @brief  Obtain drop counter report
*
* @param[in]   asic             - unit
* @param[out]  data             - Report
* @param[out]  time             - time
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if ippg stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if ippg stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_pt_drop_counter_get (int asic,
                              BVIEW_PT_DROP_COUNTER_REPORT_t *data,
                              BVIEW_TIME_t *time);

/*********************************************************************
* @brief  Clear drop reasons configured
*
* @param[in]   asic             - unit
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if ipsp stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if ipsp stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_pt_clear_drop_reason (int asic);

/*********************************************************************
* @brief Register hardware trigger callback
*
* @param[in]   asic             - unit
* @param[out]  callback         - callback function
* @param[out]  cookie           - application cookie
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if isp stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if isp stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_pt_register_trigger (int asic,
                                 BVIEW_PT_TRIGGER_CALLBACK_t callback,
                                 void *cookie);


#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_SBPLUGIN_COMMON_PACKET_TRACE_H */
  


