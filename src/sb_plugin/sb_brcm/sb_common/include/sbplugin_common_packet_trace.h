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

#ifndef INCLUDE_SBPLUGIN_COMMON_PACKET_TRACE_H
#define INCLUDE_SBPLUGIN_COMMON_PACKET_TRACE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "broadview.h"
#include "sbfeature_packet_trace.h"
#include "sbplugin_common_system.h"

/** Packet Trace feature support ASIC's Mask*/
#define  BVIEW_PACKET_TRACE_SUPPORT_MASK    (BVIEW_ASIC_TYPE_TH)

#define    BVIEW_ECMP_IDX_START       200000
#define    BVIEW_L3_EGRESS_IDX_START  100000

#define    BVIEW_PKT_TRACE_RULE_MATCH_IDX_START  0x40
#define    BVIEW_PKT_TRACE_RULE_MATCH_IDX_END    (BVIEW_PKT_TRACE_RULE_MATCH_IDX_START + BVIEW_PT_MAX_PROFILE_REQUESTS)
#define    BVIEW_PKT_TRACE_MAX_SUPPORTED_REQ     BVIEW_PT_MAX_PROFILE_REQUESTS
#define    BVIEW_PKT_TRACE_LTC_CLASS_TAG_ID      9
#define    BVIEW_PKT_TRACE_LTC_INVALID_REQ_ID    -1 
#define    BVIEW_PKT_TRACE_LTC_INVALID_RULE_ID   0

#define    SB_BRCM_PT_IPv4_ADDR_MASK             0xffffffff
#define    SB_BRCM_PT_SRC_L4_PORT_MASK           0xffff
#define    SB_BRCM_PT_DST_L4_PORT_MASK           0xffff
#define    SB_BRCM_PT_IP_PROTO_MASK              0xff


typedef struct _pt_ltc_req_to_rule_map_
{
   int                          requestId;
   unsigned int                 ruleMatchId;
   SB_BRCM_FIELD_GROUP_CONFIG_t ingressGroupId;
   SB_BRCM_FIELD_ENTRY_t        ingressEntry;
} BVIEW_PT_LTC_REQ_TO_RULE_MAP_t;

typedef struct _pt_rule_info_
{
   BVIEW_PT_LTC_REQ_TO_RULE_MAP_t reqToRuleMap[BVIEW_MAX_ASICS_ON_A_PLATFORM][BVIEW_PKT_TRACE_MAX_SUPPORTED_REQ];
   SB_BRCM_FIELD_GROUP_CONFIG_t ltcCpuGroupId;
   SB_BRCM_FIELD_ENTRY_t        ltcCpuEntry;
   pthread_rwlock_t             ltcLock;
} BVIEW_PT_LTC_RULE_INFO_t;



/* Macro to acquire read lock */
#define SBPLUGIN_PT_RWLOCK_RD_LOCK(lock)                             \
           if (pthread_rwlock_rdlock(&lock) != 0)                    \
           {                                                         \
               SB_DEBUG_PRINT (BVIEW_LOG_ERROR,                      \
			   "(%s:%d) Failed to take sbplugin PT LTC"              \
               "read write lock for read\n", __FILE__, __LINE__);    \
               return BVIEW_STATUS_FAILURE;                          \
           } 


/* Macro to acquire write lock */
#define SBPLUGIN_PT_RWLOCK_WR_LOCK(lock)                             \
           if (pthread_rwlock_wrlock(&lock) != 0)                    \
           {                                                         \
               SB_DEBUG_PRINT (BVIEW_LOG_ERROR,                      \
			   "(%s:%d) Failed to take sbplugin PT LTC"              \
               "read write lock for write\n", __FILE__, __LINE__);   \
               return BVIEW_STATUS_FAILURE;                          \
           } 

/* Macro to release RW lock */
#define SBPLUGIN_PT_RWLOCK_UNLOCK(lock)                              \
           if (pthread_rwlock_unlock(&lock) != 0)                    \
           {                                                         \
               SB_DEBUG_PRINT (BVIEW_LOG_ERROR,                      \
			   "(%s:%d) Failed to take sbplugin PT LTC"              \
               "read write lock          \n", __FILE__, __LINE__);   \
           } 



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


/*********************************************************************
* @brief  Create a 5 tuple profile/rule in HW to capture live traffic.
*
* @param[in]    asic              unit
* @param[in]    request_id        request id of the profile
* @param[in]    port_list         List of ports on which this rule has to be applied
* @param[in]    tuple_params      5 tuple params
*
* @retval   BVIEW_STATUS_FAILURE      Failed to create a profile/rule with the requested 
*                                     params. 
*
* @retval   BVIEW_STATUS_SUCCESS      Profile is created in HW
*
*
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_pt_5_tuple_profile_create (int asic, int request_id,
                                BVIEW_PORT_MASK_t *port_list, PT_5_TUPLE_PARAMS_t *tuple_params);

/*********************************************************************
* @brief  Delete 5 tuple profile/rule from HW.
*
* @param[in]    asic              unit
* @param[in]    request_id        request id of the profile
*
* @retval   BVIEW_STATUS_FAILURE      Failed to delete profile with 
*                                      the request id from HW
*
* @retval   BVIEW_STATUS_SUCCESS      Profile is deleted
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_pt_5_tuple_profile_delete (int asic, int request_id);

/*********************************************************************
* @brief   Is packet is due to Live Traffic Capture rules
*
* @param[int]  pkt             -    Pointer to Packet structure
* @param[out]  requestId       -    Request id matching with
*                                    the packets rule match id
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if packet is not because of
*                                         Live Traffic Capture rules.
* @retval BVIEW_STATUS_SUCCESS           if packet is because of 
*                                         Live Traffic Capture rules.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_common_pt_build_info (SB_BRCM_PKT_t *pkt, int *requestId);

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_SBPLUGIN_COMMON_PACKET_TRACE_H */
  


