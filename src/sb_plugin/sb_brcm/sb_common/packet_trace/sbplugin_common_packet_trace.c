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
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "packet_trace.h"
#include "sbplugin.h"
#include "sbplugin_common_packet_trace.h"
#include "sbplugin_common.h"
#include "sbplugin_common_system.h"
#include "common/platform_spec.h"
#include "sbplugin_system_map.h"
#include "sbplugin_packet_trace_map.h"

#define BVIEW_PT_DGPP_BYTE0                79
#define BVIEW_PT_DGPP_BYTE1                78
#define BVIEW_PT_DGPP_BYTE0_SHIFT          8

BVIEW_PT_LTC_RULE_INFO_t  ptLtcRuleInfo;

/*********************************************************************
* @brief  Clear LTC rule map information. 
*
* @param[in]   asic                  - unit
*
* @retval BVIEW_STATUS_SUCCESS       - LTC Rule map is successfully cleared
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS sbplugin_pt_ltc_req_rule_map_clear_all()
{
   int asic = 0;
   int ruleNum = 0;
 
   /* Acquire write lock */
   SBPLUGIN_PT_RWLOCK_WR_LOCK(ptLtcRuleInfo.ltcLock);

   ptLtcRuleInfo.ltcCpuGroupId = SB_BRCM_FIELD_ENTRY_INVALID;
   ptLtcRuleInfo.ltcCpuEntry = SB_BRCM_FIELD_ENTRY_INVALID;
   
   for (asic = 0; asic < BVIEW_MAX_ASICS_ON_A_PLATFORM; asic++)
   {
      for (ruleNum = 0; ruleNum < BVIEW_PKT_TRACE_MAX_SUPPORTED_REQ; ruleNum++)
      {
         ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].requestId 
                                         = BVIEW_PKT_TRACE_LTC_INVALID_REQ_ID;
         ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].ruleMatchId 
                                         = BVIEW_PKT_TRACE_LTC_INVALID_RULE_ID;
         ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].ingressGroupId 
                                         = SB_BRCM_FIELD_ENTRY_INVALID; 
         ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].ingressEntry   
                                         = SB_BRCM_FIELD_ENTRY_INVALID;
      }
   }

   /* Release the lock */  
   SBPLUGIN_PT_RWLOCK_UNLOCK(ptLtcRuleInfo.ltcLock);
   return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief   Initialize packet trace configuration 
*
*
* @retval BVIEW_STATUS_FAILURE      Failed to init packet trace config  
* @retval BVIEW_STATUS_SUCCESS      Successfully initialised packet trace config
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS sbplugin_packet_trace_config_init ()
{
   BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

   /* Initialize Read Write lock with default attributes */
   if (pthread_rwlock_init (&ptLtcRuleInfo.ltcLock, NULL) != 0)
   {
      SB_DEBUG_PRINT (BVIEW_LOG_ERROR,
       "(%s:%d) Failed to create read write lock with deafault attributes\n",
       __FILE__, __LINE__);
       rv = BVIEW_STATUS_FAILURE;
   }

   sbplugin_pt_ltc_req_rule_map_clear_all(); 
   return rv; 
}

/*********************************************************************
* @brief  Get LTC free rule match id. 
*
* @param[in]   asic                  - unit
* @param[out]  ruleId                - Rule id
*
* @retval BVIEW_STATUS_FAILURE           if all the reserved rule match ids
*                                        are used
* @retval BVIEW_STATUS_SUCCESS           if a free rule is found
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS sbplugin_pt_ltc_free_rule_match_id_get(int asic,
                                           unsigned int *ruleId)
{
   unsigned int curRuleMatchId;
   bool isFree = true;
   int ruleNum = 0;
   BVIEW_STATUS  rv = BVIEW_STATUS_FAILURE;

   /*  Acquire Read lock */
   SBPLUGIN_PT_RWLOCK_RD_LOCK(ptLtcRuleInfo.ltcLock);

   for ((curRuleMatchId = BVIEW_PKT_TRACE_RULE_MATCH_IDX_START); 
               (curRuleMatchId <= BVIEW_PKT_TRACE_RULE_MATCH_IDX_END);   
               (curRuleMatchId++))
   {
      isFree = true;
      for (ruleNum = 0; ruleNum < BVIEW_PKT_TRACE_MAX_SUPPORTED_REQ; ruleNum++)
      {
         if (ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].ruleMatchId == curRuleMatchId)
         {
            isFree = false;
            break;
         }
      }
      if (isFree == true)
      {
         *ruleId = curRuleMatchId;
         rv = BVIEW_STATUS_SUCCESS;
         break;
      }
   }
   /* Release Lock */ 
   SBPLUGIN_PT_RWLOCK_UNLOCK(ptLtcRuleInfo.ltcLock);
   return rv;
}

/*********************************************************************
* @brief  Set LTC Rule map information. 
*
* @param[in]   asic                  - unit
* @param[in]  requestId              - Request id
* @param[in]  ruleMatchId           - rule match number
* @param[in]  ingressGroupId        - Ingress group id
* @param[in]  ingressEntry          - Ingress entry information
*
* @retval BVIEW_STATUS_FAILURE           if cpu rule map is not present  
* @retval BVIEW_STATUS_SUCCESS           if the rule map is present
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS sbplugin_pt_ltc_req_rule_map_set(int asic, int requestId, unsigned int ruleMatchId,
                                SB_BRCM_FIELD_GROUP_CONFIG_t ingressGroupId, SB_BRCM_FIELD_ENTRY_t ingressEntry)
{
   int ruleNum = 0;
   BVIEW_STATUS  rv = BVIEW_STATUS_TABLE_FULL;

   /* Acquire write lock */
   SBPLUGIN_PT_RWLOCK_WR_LOCK(ptLtcRuleInfo.ltcLock);

   for (ruleNum = 0; ruleNum < BVIEW_PKT_TRACE_MAX_SUPPORTED_REQ; ruleNum++)
   {
      if (ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].requestId == requestId)
      {
         rv = BVIEW_STATUS_DUPLICATE;
         break;
      }
   }

   if (rv != BVIEW_STATUS_DUPLICATE)
   {
     for (ruleNum = 0; ruleNum < BVIEW_PKT_TRACE_MAX_SUPPORTED_REQ; ruleNum++)
     {
        if (ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].requestId == BVIEW_PKT_TRACE_LTC_INVALID_REQ_ID)
        {
           ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].requestId = requestId;
           ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].ruleMatchId = ruleMatchId;
           ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].ingressGroupId = ingressGroupId;
           ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].ingressEntry = ingressEntry;
           rv = BVIEW_STATUS_SUCCESS;
           break;
        }
      }
   }  
   /* Release the lock */  
   SBPLUGIN_PT_RWLOCK_UNLOCK(ptLtcRuleInfo.ltcLock);
   return rv;
}

/*********************************************************************
* @brief  Get total number of configured LTC rules 
*
* @param[in]   asic                  - unit
* @param[out]  totalRules            - Total number of rules
*
* @retval BVIEW_STATUS_SUCCESS           
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS sbplugin_pt_ltc_total_rule_num_get(int asic, int *totalRules)
{
   int ruleNum = 0;
   BVIEW_STATUS  rv = BVIEW_STATUS_SUCCESS;
   int total = 0;

   /*  Acquire Read lock */
   SBPLUGIN_PT_RWLOCK_RD_LOCK(ptLtcRuleInfo.ltcLock);

   for (ruleNum = 0; ruleNum < BVIEW_PKT_TRACE_MAX_SUPPORTED_REQ; ruleNum++)
   {
      if (ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].requestId != BVIEW_PKT_TRACE_LTC_INVALID_REQ_ID)
      {
        total++; 
      }
   }
    
   /* Release the lock */  
   SBPLUGIN_PT_RWLOCK_UNLOCK(ptLtcRuleInfo.ltcLock);

   *totalRules = total;
   return rv;
}


/*********************************************************************
* @brief  Set Rule map information of  CPU rule. 
*
* @param[in]   asic                  - unit
* @param[in]  ltcCpuGroupId         -  Cpu rule Group id
* @param[in]  ltcCpuEntry           -  Cpu rule entry information
*
* @retval BVIEW_STATUS_FAILURE           if cpu rule map is not present  
* @retval BVIEW_STATUS_SUCCESS           if the rule map is present
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS sbplugin_pt_ltc_cpu_rule_map_set(int asic, 
                 SB_BRCM_FIELD_GROUP_CONFIG_t ltcCpuGroupId, SB_BRCM_FIELD_ENTRY_t ltcCpuEntry)
{
   /* Acquire write lock */
   SBPLUGIN_PT_RWLOCK_WR_LOCK(ptLtcRuleInfo.ltcLock);

   ptLtcRuleInfo.ltcCpuGroupId = ltcCpuGroupId;
   ptLtcRuleInfo.ltcCpuEntry = ltcCpuEntry;

   /* Release the lock */  
   SBPLUGIN_PT_RWLOCK_UNLOCK(ptLtcRuleInfo.ltcLock);

   return BVIEW_STATUS_SUCCESS;
}
/*********************************************************************
* @brief  Get Rule map information of  CPU rule. 
*
* @param[in]   asic                  - unit
* @param[out]  ltcCpuGroupId         -  Cpu rule Group id
* @param[out]  ltcCpuEntry           -  Cpu rule entry information
*
* @retval BVIEW_STATUS_FAILURE           if cpu rule map is not present  
* @retval BVIEW_STATUS_SUCCESS           if the rule map is present
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS sbplugin_pt_ltc_cpu_rule_map_get(int asic, 
            SB_BRCM_FIELD_GROUP_CONFIG_t *ltcCpuGroupId, SB_BRCM_FIELD_ENTRY_t *ltcCpuEntry)
{
     
   /*  Acquire Read lock */
   SBPLUGIN_PT_RWLOCK_RD_LOCK(ptLtcRuleInfo.ltcLock);

   *ltcCpuGroupId = ptLtcRuleInfo.ltcCpuGroupId;
   *ltcCpuEntry = ptLtcRuleInfo.ltcCpuEntry;

   /* Release the lock */  
   SBPLUGIN_PT_RWLOCK_UNLOCK(ptLtcRuleInfo.ltcLock);

   return BVIEW_STATUS_SUCCESS;
}
/*********************************************************************
* @brief  Get CPU rule map information. 
*
* @param[in]   asic                  - unit
*
* @retval BVIEW_STATUS_SUCCESS           CPU Rule map is successfully cleared
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS sbplugin_pt_ltc_cpu_rule_clear(int asic) 
{
     
   /* Acquire write lock */
   SBPLUGIN_PT_RWLOCK_WR_LOCK(ptLtcRuleInfo.ltcLock);

   ptLtcRuleInfo.ltcCpuGroupId = SB_BRCM_FIELD_ENTRY_INVALID;
   ptLtcRuleInfo.ltcCpuEntry = SB_BRCM_FIELD_ENTRY_INVALID;

   /* Release the lock */  
   SBPLUGIN_PT_RWLOCK_UNLOCK(ptLtcRuleInfo.ltcLock);

   return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Get LTC rule map information of a request id. 
*
* @param[in]   asic                  - unit
* @param[in]  requestId              - Request id
* @param[out]  ruleMatchId           - rule match number
* @param[out]  ingressGroupId        - Ingress group id
* @param[out]  ingressEntry          - Ingress entry information
*
* @retval BVIEW_STATUS_FAILURE           if rule map is not present with 
*                                         the corresponding request id
* @retval BVIEW_STATUS_SUCCESS           if the rule map is present
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS sbplugin_pt_ltc_req_rule_map_get (int asic, int request_id, 
                     unsigned int *ruleMatchId, SB_BRCM_FIELD_GROUP_CONFIG_t *ingressGroupId,
                     SB_BRCM_FIELD_ENTRY_t *ingressEntry)
{
   int ruleNum = 0;
   BVIEW_STATUS  rv = BVIEW_STATUS_FAILURE;
  
   /*  Acquire Read lock */
   SBPLUGIN_PT_RWLOCK_RD_LOCK(ptLtcRuleInfo.ltcLock);

   for (ruleNum = 0; ruleNum < BVIEW_PKT_TRACE_MAX_SUPPORTED_REQ; ruleNum++)
   {
      if (ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].requestId == request_id)
      {
         *ruleMatchId = ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].ruleMatchId;
         *ingressGroupId = ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].ingressGroupId;
         *ingressEntry = ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].ingressEntry;
         rv = BVIEW_STATUS_SUCCESS;
         break;
      }
   }

   /* Release the lock */  
   SBPLUGIN_PT_RWLOCK_UNLOCK(ptLtcRuleInfo.ltcLock);
   return rv;
}

/*********************************************************************
* @brief  Clear LTC rule map information of a request id. 
*
* @param[in]   asic                  - unit
* @param[in]  requestId             - Request id
*
* @retval BVIEW_STATUS_FAILURE           if rule map is not present with 
*                                         the corresponding request id
* @retval BVIEW_STATUS_SUCCESS           if the rule map is successfully cleared
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS sbplugin_pt_ltc_req_rule_map_clear(int asic, int requestId)
{
   int ruleNum = 0;
   BVIEW_STATUS  rv = BVIEW_STATUS_FAILURE;

   /* Acquire write lock */
   SBPLUGIN_PT_RWLOCK_WR_LOCK(ptLtcRuleInfo.ltcLock);

   for (ruleNum = 0; ruleNum < BVIEW_PKT_TRACE_MAX_SUPPORTED_REQ; ruleNum++)
   {
      if (ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].requestId == requestId)
      {
         ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].requestId 
                                         = BVIEW_PKT_TRACE_LTC_INVALID_REQ_ID;
         ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].ruleMatchId 
                                         = BVIEW_PKT_TRACE_LTC_INVALID_RULE_ID;
         ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].ingressGroupId 
                                         = SB_BRCM_FIELD_ENTRY_INVALID; 
         ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].ingressEntry   
                                         = SB_BRCM_FIELD_ENTRY_INVALID;
         rv = BVIEW_STATUS_SUCCESS;
         break; 
      }
   }
    
   /* Release the lock */  
   SBPLUGIN_PT_RWLOCK_UNLOCK(ptLtcRuleInfo.ltcLock);
   return rv;
}

/*********************************************************************
* @brief  Get LTC request id from rule number .
*
* @param[in]   asic                  - unit
* @param[in]   ruleMatchId           - rule match number
* @param[out]  requestId             - Matched request id
*
* @retval BVIEW_STATUS_FAILURE           if rule number is not matched.
* @retval BVIEW_STATUS_SUCCESS           if the rule map is present.
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS sbplugin_pt_ltc_rule_to_req_map_get(int asic, unsigned int ruleMatchId,
                                                  int *requestId)
{
   int ruleNum = 0;
   BVIEW_STATUS  rv = BVIEW_STATUS_FAILURE;
  
   /*  Acquire Read lock */
   SBPLUGIN_PT_RWLOCK_RD_LOCK(ptLtcRuleInfo.ltcLock);

   for (ruleNum = 0; ruleNum < BVIEW_PKT_TRACE_MAX_SUPPORTED_REQ; ruleNum++)
   {
      if (ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].ruleMatchId == ruleMatchId)
      {
        if (BVIEW_PKT_TRACE_LTC_INVALID_REQ_ID != ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].requestId)
        {
          *requestId = ptLtcRuleInfo.reqToRuleMap[asic][ruleNum].requestId;
          rv = BVIEW_STATUS_SUCCESS;
          break;
        }
      }
   }
   /* Release the lock */  
   SBPLUGIN_PT_RWLOCK_UNLOCK(ptLtcRuleInfo.ltcLock);
   return rv;
}



/**********************************************************************
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
BVIEW_STATUS sbplugin_common_packet_trace_init (BVIEW_SB_PT_FEATURE_t *bcmPT)
{
  /* NULL Pointer check*/
  BVIEW_NULLPTR_CHECK (bcmPT);

  /* Initialize BST functions*/
  bcmPT->feature.featureId           = BVIEW_FEATURE_PACKET_TRACE;
  bcmPT->feature.supportedAsicMask   = BVIEW_PACKET_TRACE_SUPPORT_MASK;
  bcmPT->pt_drop_reason_config_set_cb    = 
                  sbplugin_common_pt_drop_reason_config_set;
  bcmPT->pt_drop_reason_config_get_cb    = 
                  sbplugin_common_pt_drop_reason_config_get;
  bcmPT->pt_supported_drop_reason_get_cb = 
                  sbplugin_common_pt_supported_drop_reason_get;
  bcmPT->pt_trace_profile_get_cb         = 
                  sbplugin_common_pt_trace_profile_get;
  bcmPT->pt_drop_counter_get_cb          = 
                  sbplugin_common_pt_drop_counter_get;
  bcmPT->pt_clear_drop_reason_cb         = 
                  sbplugin_common_pt_clear_drop_reason;
  bcmPT->pt_register_trigger_cb          = 
                  sbplugin_common_pt_register_trigger;
  bcmPT->pt_5_tuple_profile_create_cb    =
                  sbplugin_common_pt_5_tuple_profile_create;
  bcmPT->pt_5_tuple_profile_delete_cb    =
                  sbplugin_common_pt_5_tuple_profile_delete;

  sbplugin_packet_trace_config_init ();

  return BVIEW_STATUS_SUCCESS;
}

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
                                           BVIEW_PT_DROP_REASON_CONFIG_t *config)
{
 return  BVIEW_STATUS_SUCCESS;
}

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
                                            BVIEW_PT_DROP_REASON_CONFIG_t *config)
{
 return  BVIEW_STATUS_SUCCESS;
}

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
                                BVIEW_PT_DROP_REASON_MASK_t *mask) 
{
  return BVIEW_STATUS_SUCCESS;
}

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
int     sbplugin_pkt_trace_callback (int asic,  int index,
                                     SB_BRCM_L3_HOST_t *info,
                                     void *user_data)
{
  BVIEW_ECMP_MEMBER_t    *ecmp = user_data; 
 
  if (info->l3a_intf == ecmp->member)
  {
    ecmp->ip = info->l3a_ip_addr;
  }
  return 0;
}  
  
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
                                    BVIEW_TIME_t *time)
{
  SB_BRCM_PKT_TRACE_INFO_t     pkt_trace_info;
  unsigned int                 pkt_trace_options = 0;
  int                          b_rv = 0;
  SB_BRCM_PORT_t               localport;
  SB_BRCM_GPORT_t              gport;
  BVIEW_ECMP_MEMBER_t    *ecmp_temp;
  SB_BRCM_L3_EGRESS_t          egress_info;
  SB_BRCM_IF_t                 ecmp_egress_member[BVIEW_ECMP_MAX_LEVEL];
  SB_BRCM_IF_t                 ecmp_group[BVIEW_ECMP_MAX_LEVEL];
  SB_BRCM_IF_t                 ecmp_members[BVIEW_ECMP_MAX_MEMBERS];
  SB_BRCM_L3_INFO_t            l3_info;
  SB_BRCM_TRUNK_INFO_t         tinfo;
  SB_BRCM_TRUNK_MEMBER_t       member_array[SB_BRCM_RUNK_MAX_PORTCNT];
  int                          intf_count =0;
  int                          ecmp_level =0;
  int                          member_index = 0;
  int                          memberCount =0;
  unsigned int                 dgpp =0;

  /* NULL Pointer validation*/
  BVIEW_NULLPTR_CHECK (packet);
  BVIEW_NULLPTR_CHECK (traceProfile);
  BVIEW_NULLPTR_CHECK (time);

  memset (&pkt_trace_info, 0x00, sizeof (pkt_trace_info));
  memset (ecmp_egress_member, 0x00, sizeof(ecmp_egress_member));
  memset (ecmp_group, 0x00, sizeof(ecmp_group));
   
  /* Check the Validity of the port*/
  b_rv = SB_BRCM_API_PORT_GPORT_GET(asic, port, &gport);
  if (b_rv != SB_BRCM_E_NONE)
  {
      return BVIEW_STATUS_FAILURE;
  }

   /*Get total use-count is expressed in terms of buffers used in the device*/
  b_rv = SB_BRCM_API_PKT_TRACE_INFO_GET (asic, pkt_trace_options, 
                                         port, packet->pkt_len, packet->data,
                                         &pkt_trace_info);
  if (b_rv != SB_BRCM_E_NONE)
  {
    return BVIEW_STATUS_FAILURE;
  }
  /* Get Destination port*/
  dgpp = (pkt_trace_info.raw_data[BVIEW_PT_DGPP_BYTE1] << BVIEW_PT_DGPP_BYTE0_SHIFT )| 
          pkt_trace_info.raw_data[BVIEW_PT_DGPP_BYTE0];
  
  if (dgpp != 0)
  {
    BVIEW_SETMASKBIT (traceProfile->destPortMask, dgpp);
    traceProfile->hashingInfo.flags |= BVIEW_PT_DGPP_RESOLUTION;
  }

  /* Get ECMP level 1 Hashing Information*/
  if (pkt_trace_info.pkt_trace_hash_info.flags & SB_BRCM_SWITCH_PKT_TRACE_ECMP_1 )
  {
    if (pkt_trace_info.pkt_trace_hash_info.ecmp_1_group < BVIEW_ECMP_IDX_START )
   { 
      ecmp_group[PKT_TRACE_ECMP_1_INDEX] = BVIEW_ECMP_IDX_START + 
                    pkt_trace_info.pkt_trace_hash_info.ecmp_1_group;
   }
   else 
   {
      ecmp_group[PKT_TRACE_ECMP_1_INDEX] = pkt_trace_info.pkt_trace_hash_info.ecmp_1_group;
   }

   if (pkt_trace_info.pkt_trace_hash_info.ecmp_1_egress < BVIEW_L3_EGRESS_IDX_START)
   {
    ecmp_egress_member[PKT_TRACE_ECMP_1_INDEX] = BVIEW_L3_EGRESS_IDX_START +
                          pkt_trace_info.pkt_trace_hash_info.ecmp_1_egress;
   }
   else
   {
     ecmp_egress_member[PKT_TRACE_ECMP_1_INDEX] = pkt_trace_info.pkt_trace_hash_info.ecmp_1_egress;
   }
    traceProfile->hashingInfo.flags |= BVIEW_PT_ECMP_1_RESOLUTION;
  }

  if (pkt_trace_info.pkt_trace_hash_info.flags & SB_BRCM_SWITCH_PKT_TRACE_ECMP_2)
  {
    if (pkt_trace_info.pkt_trace_hash_info.ecmp_2_group < BVIEW_ECMP_IDX_START)
    {
    ecmp_group[PKT_TRACE_ECMP_2_INDEX] = BVIEW_ECMP_IDX_START +
                    pkt_trace_info.pkt_trace_hash_info.ecmp_2_group;
    }
    else
    {
      ecmp_group[PKT_TRACE_ECMP_2_INDEX] = pkt_trace_info.pkt_trace_hash_info.ecmp_2_group;
    }

    if (pkt_trace_info.pkt_trace_hash_info.ecmp_2_egress < BVIEW_L3_EGRESS_IDX_START)
    {
    ecmp_egress_member[PKT_TRACE_ECMP_2_INDEX] = BVIEW_L3_EGRESS_IDX_START +
                          pkt_trace_info.pkt_trace_hash_info.ecmp_2_egress;
    }
    else
    {
      ecmp_egress_member[PKT_TRACE_ECMP_2_INDEX] = pkt_trace_info.pkt_trace_hash_info.ecmp_2_egress;
    }
    traceProfile->hashingInfo.flags |= BVIEW_PT_ECMP_2_RESOLUTION;
  }

  for (ecmp_level = 0; (ecmp_level < BVIEW_ECMP_MAX_LEVEL && ecmp_group[ecmp_level]); 
       ecmp_level++)
  {
    traceProfile->hashingInfo.ecmp[ecmp_level].ecmp_egress_info.member = 
              ecmp_egress_member[ecmp_level];
    traceProfile->hashingInfo.ecmp[ecmp_level].ecmp_group = ecmp_group[ecmp_level];
    /* Get L3 Interface info*/
    if (SB_BRCM_E_NONE == SB_BRCM_API_L3_EGRESS_GET (asic, ecmp_egress_member[ecmp_level], 
                                   &egress_info))
    {
      traceProfile->hashingInfo.ecmp[ecmp_level].ecmp_egress_info.port =
              egress_info.port;
    }

    /* Get ECMP members info*/
    SB_BRCM_API_L3_ECMP_MULTIPATH_GET (asic, ecmp_group[ecmp_level], 
                                              0, NULL, &intf_count);
    /* Get ECMP Members*/
    if (SB_BRCM_E_NONE == SB_BRCM_API_L3_ECMP_MULTIPATH_GET (asic, ecmp_group[ecmp_level], 
                                              intf_count, 
                                              &ecmp_members[0], &intf_count))
    {
      for (member_index = 0; member_index < intf_count; 
          member_index++)
      {
        traceProfile->hashingInfo.ecmp[ecmp_level].ecmp_group_members[member_index].member = 
          ecmp_members[member_index]; 
        /* get the port and the ip address of the member interface */
        /* Get L3 Interface info*/
        if (SB_BRCM_E_NONE == SB_BRCM_API_L3_EGRESS_GET (asic, ecmp_members[member_index], 
              &egress_info))
        {
          traceProfile->hashingInfo.ecmp[ecmp_level].ecmp_group_members[member_index].port =
            egress_info.port;
        }

        /* get the ip addres */

        /* Get size of L3 Host table*/

        ecmp_temp = &traceProfile->hashingInfo.ecmp[ecmp_level].ecmp_group_members[member_index];
        SB_BRCM_API_L3_INFO (asic, &l3_info);
        /* Traverse L3 Host table and get the IP of interetsed L3 Interface*/
        SB_BRCM_API_L3_HOST_TRAVERSE (asic, 0, 0, l3_info.l3info_max_host, 
            sbplugin_pkt_trace_callback, 
            ecmp_temp);


      }
    }
   
    ecmp_temp = &traceProfile->hashingInfo.ecmp[ecmp_level].ecmp_egress_info;

    /* Get size of L3 Host table*/
    SB_BRCM_API_L3_INFO (asic, &l3_info);
    /* Traverse L3 Host table and get the IP of interetsed L3 Interface*/
    SB_BRCM_API_L3_HOST_TRAVERSE (asic, 0, 0, l3_info.l3info_max_host, 
                                  sbplugin_pkt_trace_callback, 
                                  ecmp_temp);

  } 

  /* Get Trunk Hashing Information*/
  if (pkt_trace_info.pkt_trace_hash_info.flags & SB_BRCM_SWITCH_PKT_TRACE_TRUNK)
  {
    traceProfile->hashingInfo.lag.trunk  = 
                           pkt_trace_info.pkt_trace_hash_info.trunk;

    /* Get Physical port from GPORT*/
    SB_BRCM_PORT_LOCAL_GET (asic,  
                        pkt_trace_info.pkt_trace_hash_info.trunk_member, 
                        &localport);
    traceProfile->hashingInfo.lag.trunk_member = localport;
    /* Get Trunk Members*/
    if (SB_BRCM_E_NONE == SB_BRCM_API_TRUNK_GET (asic, pkt_trace_info.pkt_trace_hash_info.trunk,
                                  &tinfo, SB_BRCM_RUNK_MAX_PORTCNT,
                                  &member_array[0],&memberCount))
    {
      for (member_index = 0; member_index < memberCount; 
         member_index++)
      {
       /* Get Physical port from GPORT*/
        SB_BRCM_PORT_LOCAL_GET (asic, member_array[member_index].gport,
                              &localport);

        traceProfile->hashingInfo.lag.trunk_members[member_index] =
                                localport;
      }
    }                                 
    traceProfile->hashingInfo.flags |= BVIEW_PT_TRUNK_RESOLUTION;
  }
  
  /* Get Higig Trunk Hashing Information*/
  if (SB_BRCM_SWITCH_PKT_TRACE_FABRIC_TRUNK & pkt_trace_info.pkt_trace_hash_info.flags)
  {
    traceProfile->hashingInfo.lag.fabric_trunk  = 
                           pkt_trace_info.pkt_trace_hash_info.fabric_trunk;
     /* Get Physical port from GPORT*/
    SB_BRCM_PORT_LOCAL_GET (asic,
                        pkt_trace_info.pkt_trace_hash_info.fabric_trunk_member,
                        &localport);
    traceProfile->hashingInfo.lag.fabric_trunk_member = localport;
    traceProfile->hashingInfo.flags |= BVIEW_PT_FABRIC_TRUNK_RESOLUTION;
  }

  /* Update current local time*/
  sbplugin_common_system_time_get (time);
 
  return BVIEW_STATUS_SUCCESS;
}

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
                              BVIEW_TIME_t *time)
{
  return BVIEW_STATUS_SUCCESS;
} 
    

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
BVIEW_STATUS sbplugin_common_pt_clear_drop_reason (int asic)
{
  return BVIEW_STATUS_SUCCESS;
}

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
                                 void *cookie)
{
 return BVIEW_STATUS_SUCCESS;
}


/*********************************************************************
* @brief  Install Egress Rate limit policy on CPU port.
*
* @param[in]      asic               - unit
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if snapshot get is failed.
* @retval BVIEW_STATUS_SUCCESS           if snapshot get is success.
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS sbplugin_common_pt_egress_policy (int asic,
                                                SB_BRCM_FIELD_GROUP_CONFIG_t *group,
                                                SB_BRCM_FIELD_ENTRY_t *rule)
{
  SB_BRCM_FIELD_QSET_t         qset;
  SB_BRCM_FIELD_ENTRY_t        entry;
  SB_BRCM_FIELD_GROUP_CONFIG_t ptCpuGroup;
  SB_BRCM_POLICER_CONFIG_t     policer;
  SB_BRCM_POLICER_t            policerId;
  int                          port = 0;


  SB_BRCM_FIELD_QSET_INIT(qset);
  SB_BRCM_FIELD_QSET_ADD(qset, SB_BRCM_QSET_FIELD_QUALIFY_STAGE_EGRESS);
  SB_BRCM_FIELD_QSET_ADD(qset, SB_BRCM_QSET_FIELD_QUALIFY_OUT_PORT);
  SB_BRCM_FIELD_QSET_ADD(qset, SB_BRCM_QSET_FIELD_QUALIFY_INGRESS_CLASS_FIELD);

   /* Create the group*/
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_GROUP_CREATE (asic, qset,0, &ptCpuGroup));

   /*Get the CPU port*/
  SB_BRCM_CPU_PORT_GET(asic, &port);
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_ENTRY_CREATE (asic, ptCpuGroup, &entry));
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_QUALIFY_OUTPORT (asic, entry , port, 0xff));
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_QUALIFY_INGRESSCLASSFIELD (asic, entry, 
                                      BVIEW_PKT_TRACE_LTC_CLASS_TAG_ID,0xff));
  /* Attach the same stat for all the entries*/
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_ACTION_ADD (asic, entry,
                  SB_BRCM_FIELD_ACTION_RP_DROP,0,0));

  /* Create the policer and attach to the rule*/
  SB_BRCM_POLICER_CONFIG_INIT(&policer);
  policer.mode = SB_BRCM_POLICER_MODE_COMMITTED ;
  policer.flags |= (SB_BRCM_POLICER_MODE_PACKETS);
  policer.flags |= (SB_BRCM_POLICER_COLOR_BLIND);
  policer.ckbits_sec = 100;
  policer.ckbits_burst = 128;
  BVIEW_ERROR_RETURN (SB_BRCM_POLICER_CREATE (asic, &policer, &policerId));
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_ENTRY_POLICER_ATTACH (asic, entry, 0, policerId));
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_GROUP_INSTALL (asic, ptCpuGroup));

  *group = ptCpuGroup;
  *rule =  entry;
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Create a 5 tuple profile/rule in HW to capture live traffic.
*
* @param[in]    asic              unit
* @param[in]    request_id        request id of the profile
* @param[in]    port_list         List of ports on which this rule has to be applied
* @param[in]    tupleParams      5 tuple params
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
BVIEW_STATUS sbplugin_common_pt_5_tuple_profile_create (int asic, int requestId,
                                BVIEW_PORT_MASK_t *portList, PT_5_TUPLE_PARAMS_t *tupleParams)
{
  int                           rv = SB_BRCM_E_NONE;
  SB_BRCM_FIELD_QSET_t          qset;
  SB_BRCM_FIELD_ENTRY_t         ptLtcEntry = SB_BRCM_FIELD_ENTRY_INVALID;
  SB_BRCM_FIELD_GROUP_CONFIG_t  ptLtcGroup = SB_BRCM_FIELD_ENTRY_INVALID;
  SB_BRCM_FIELD_GROUP_CONFIG_t  ptLtcCpuGroup = SB_BRCM_FIELD_ENTRY_INVALID;
  SB_BRCM_FIELD_ENTRY_t         ptLtcCpuEntry = SB_BRCM_FIELD_ENTRY_INVALID;
  SB_BRCM_PBMP_t                InPbmp;
  SB_BRCM_PORT_CONFIG_t         port_cfg;
  int                           port = 0;
  unsigned int                  ruleId = 0;  
  int                           totalRules = 0;

  if (requestId <= BVIEW_PKT_TRACE_LTC_INVALID_REQ_ID)
  {
     SB_DEBUG_PRINT (BVIEW_LOG_ERROR,"Invalid Request ID %d\n", requestId);
     return BVIEW_STATUS_FAILURE;
  }

  BVIEW_NULLPTR_CHECK(portList);
  BVIEW_NULLPTR_CHECK(tupleParams);

  SB_BRCM_PBMP_CLEAR(InPbmp);

  port = 0;
  /* Get new rule match id */
  if (sbplugin_pt_ltc_free_rule_match_id_get(asic, &ruleId) != BVIEW_STATUS_SUCCESS)
  {
     SB_DEBUG_PRINT (BVIEW_LOG_ERROR, 
         "Not able to get unique rule id for request id %d\n", requestId);
     return BVIEW_STATUS_FAILURE;
  }

  if (SB_BRCM_RV_ERROR(SB_BRCM_API_PORT_CONFIG_GET(asic, &port_cfg)))
  {
     SB_DEBUG_PRINT (BVIEW_LOG_ERROR, 
         "Not able to get port config\n");
     return BVIEW_STATUS_FAILURE;
  }


  /* Set Qualifiers */
  SB_BRCM_FIELD_QSET_INIT(qset);
  SB_BRCM_FIELD_QSET_ADD(qset, SB_BRCM_QUALIFY_STAGE_INGRESS);
  SB_BRCM_FIELD_QSET_ADD(qset, SB_BRCM_QUALIFY_INPORTS);

  if (tupleParams->tuple_mask & PT_CONFIG_TUPLE_SRC_IP)
  {
     SB_BRCM_FIELD_QSET_ADD(qset, SB_BRCM_QUALIFY_SRC_IP);
  }
  
  if (tupleParams->tuple_mask & PT_CONFIG_TUPLE_DST_IP)
  {
     SB_BRCM_FIELD_QSET_ADD(qset, SB_BRCM_QUALIFY_DST_IP);
  }

  if (tupleParams->tuple_mask &  PT_CONFIG_TUPLE_PROTOCOL)
  {
     SB_BRCM_FIELD_QSET_ADD(qset, SB_BRCM_QUALIFY_IP_PROTO);
  }
  
  if (tupleParams->tuple_mask & PT_CONFIG_TUPLE_SRC_PORT)
  {
     SB_BRCM_FIELD_QSET_ADD(qset, SB_BRCM_QUALIFY_L4_SRC_PORT);
  }
  
  if (tupleParams->tuple_mask & PT_CONFIG_TUPLE_DST_PORT)
  {
     SB_BRCM_FIELD_QSET_ADD(qset, SB_BRCM_QUALIFY_L4_DST_PORT);
  }

  /* Build the BCM Port Mask*/
  BVIEW_SYSTEM_PORT_ITER (asic, port)
  {
    if (BVIEW_ISMASKBITSET (*portList, port))
    {  
      SB_BRCM_PBMP_PORT_ADD(InPbmp, port);

    }
  }

  /* Create the group*/
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_GROUP_CREATE (asic, qset,0, &ptLtcGroup));  
  /* Create the entry */
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_ENTRY_CREATE (asic, ptLtcGroup, &ptLtcEntry));

  /* Qualifiers: Ingress Ports
   *           : SRC IP
   *           : DST IP
   *           : IP PROTO
   *           : SRC PORT
   *           : DST PORT
   *             
   */
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_QUALIFY_INPORTS (asic, ptLtcEntry, InPbmp, port_cfg.e));
  if (tupleParams->tuple_mask & PT_CONFIG_TUPLE_SRC_IP)
  {
     BVIEW_ERROR_RETURN (
       SB_BRCM_FIELD_QUALIFY_SRC_IP(asic,ptLtcEntry,tupleParams->src_ip,SB_BRCM_PT_IPv4_ADDR_MASK));
  }
  if (tupleParams->tuple_mask & PT_CONFIG_TUPLE_DST_IP)
  {
     BVIEW_ERROR_RETURN (
       SB_BRCM_FIELD_QUALIFY_DST_IP(asic,ptLtcEntry,tupleParams->dst_ip,SB_BRCM_PT_IPv4_ADDR_MASK));
  }
  if (tupleParams->tuple_mask &  PT_CONFIG_TUPLE_PROTOCOL)
  {
     BVIEW_ERROR_RETURN (
       SB_BRCM_FIELD_QUALIFY_IP_PROTO(asic,ptLtcEntry,tupleParams->protocol,SB_BRCM_PT_IP_PROTO_MASK));
  }
  if (tupleParams->tuple_mask & PT_CONFIG_TUPLE_SRC_PORT)
  {
     BVIEW_ERROR_RETURN (
       SB_BRCM_FIELD_QUALIFY_L4_SRC_PORT(asic,ptLtcEntry,tupleParams->src_port,SB_BRCM_PT_SRC_L4_PORT_MASK));
  }
  if (tupleParams->tuple_mask & PT_CONFIG_TUPLE_DST_PORT)
  {
     BVIEW_ERROR_RETURN (
       SB_BRCM_FIELD_QUALIFY_L4_DST_PORT(asic,ptLtcEntry,tupleParams->dst_port,SB_BRCM_PT_DST_L4_PORT_MASK));
  }


  /* Add copy to cpu action */
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_ACTION_ADD (asic, ptLtcEntry, 
          SB_BRCM_FIELD_ACTION_COPYTOCPU,1, ruleId));

  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_ACTION_ADD (asic, ptLtcEntry,
                                    SB_BRCM_FIELD_ACTION_EGRESS_CLASS_SELECT,
                                    11,0));
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_ACTION_ADD (asic, ptLtcEntry, 
                                    SB_BRCM_FIELD_ACTION_NEW_CLASS_ID,
                                    BVIEW_PKT_TRACE_LTC_CLASS_TAG_ID,0));  

  rv = SB_BRCM_FIELD_GROUP_INSTALL (asic, ptLtcGroup); 

  if (rv != SB_BRCM_E_NONE)
  {
    SB_DEBUG_PRINT (BVIEW_LOG_ERROR, 
             "Failed to ingress rule by PT \n");
    return BVIEW_STATUS_FAILURE;
  }
 
  if (sbplugin_pt_ltc_req_rule_map_set(asic, requestId, ruleId, ptLtcGroup, ptLtcEntry)
                   != BVIEW_STATUS_SUCCESS)
  {
     SB_DEBUG_PRINT (BVIEW_LOG_ERROR, 
              "Failed to set Rule map for request Id \n", requestId);
     return BVIEW_STATUS_FAILURE;
  }

  if (sbplugin_pt_ltc_total_rule_num_get(asic, &totalRules) 
                   != BVIEW_STATUS_SUCCESS)
  {
     SB_DEBUG_PRINT (BVIEW_LOG_ERROR, 
              "Failed to get total number of ingress rules configured by PT \n");
     return BVIEW_STATUS_FAILURE;
  }

  /* Create CPU policy when the first rule was added */ 
  if (totalRules == 1)
  {
     rv = sbplugin_common_pt_egress_policy (asic,
                            &ptLtcCpuGroup,
                            &ptLtcCpuEntry);

     if (rv != SB_BRCM_E_NONE)
     {
        SB_DEBUG_PRINT (BVIEW_LOG_ERROR, 
                  "Failed to create cpu rules \n");
        return BVIEW_STATUS_FAILURE;
     }

     if (sbplugin_pt_ltc_cpu_rule_map_set(asic, ptLtcCpuGroup, ptLtcCpuEntry) 
                        != BVIEW_STATUS_SUCCESS)
     {
        SB_DEBUG_PRINT (BVIEW_LOG_ERROR, 
            "Failed to set cpu rule information \n");
        return BVIEW_STATUS_FAILURE;
     }
  
  }
  return BVIEW_STATUS_SUCCESS;
}

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
BVIEW_STATUS sbplugin_common_pt_5_tuple_profile_delete (int asic, int requestId)
{
  int                           rv = SB_BRCM_E_NONE;
  SB_BRCM_FIELD_ENTRY_t         ptLtcEntry = SB_BRCM_FIELD_ENTRY_INVALID;
  SB_BRCM_FIELD_GROUP_CONFIG_t  ptLtcGroup = SB_BRCM_FIELD_ENTRY_INVALID;
  SB_BRCM_FIELD_GROUP_CONFIG_t  ptLtcCpuGroup = SB_BRCM_FIELD_ENTRY_INVALID;
  SB_BRCM_FIELD_ENTRY_t         ptLtcCpuEntry = SB_BRCM_FIELD_ENTRY_INVALID;
  int                  ruleNum = 0;
  int                  curRules = 0; 
  SB_BRCM_POLICER_t             policerId;
  unsigned int                  ruleId;
 
  if (sbplugin_pt_ltc_total_rule_num_get(asic, &ruleNum) != BVIEW_STATUS_SUCCESS)
  {
     SB_DEBUG_PRINT (BVIEW_LOG_ERROR, 
         "Failed to get number of rules from rule map table\n" );
     return BVIEW_STATUS_FAILURE;
  }

  curRules = ruleNum;
  if (ruleNum == 0)
  {
     SB_DEBUG_PRINT (BVIEW_LOG_ERROR, 
         "Not able to delete rule for request id %d as the number of rules are %d\n", requestId, ruleNum);
     return BVIEW_STATUS_FAILURE;
  }

  if (sbplugin_pt_ltc_req_rule_map_get(asic, requestId, &ruleId, &ptLtcGroup, &ptLtcEntry)
          != BVIEW_STATUS_SUCCESS)
  {
     SB_DEBUG_PRINT (BVIEW_LOG_ERROR, 
         "Not able to find rule for request id %d \n", requestId);
     return BVIEW_STATUS_FAILURE;
  }

  /* Delete entry */
  rv = SB_BRCM_FIELD_ENTRY_DESTROY(asic, ptLtcEntry);
  if (rv != SB_BRCM_E_NONE)
  {
     SB_DEBUG_PRINT (BVIEW_LOG_ERROR, 
         "Not able to delete CAP entry for request id\n", requestId);
     return BVIEW_STATUS_FAILURE;
  }

  rv = SB_BRCM_FIELD_GROUP_DESTORY (asic, ptLtcGroup);
  if (rv != SB_BRCM_E_NONE)
  {
     SB_DEBUG_PRINT (BVIEW_LOG_ERROR, 
         "Failed to destroy Group id for request id\n", requestId);
    return BVIEW_STATUS_FAILURE;
  }
  
  /* Clear the rule map for the deleted request */ 
  if (sbplugin_pt_ltc_req_rule_map_clear(asic, requestId) != BVIEW_STATUS_SUCCESS)
  {
     SB_DEBUG_PRINT (BVIEW_LOG_ERROR, 
         "Failed to clear rule map for request id\n", requestId);
    return BVIEW_STATUS_FAILURE;
  }

  curRules = ruleNum-1;

  /* Check if it is the last request */
  if (curRules == 0)
  {
    /************************ Delete CPU rule ******************/
    /** Get cpu rule related info **/
    if (sbplugin_pt_ltc_cpu_rule_map_get(asic, &ptLtcCpuGroup, &ptLtcCpuEntry) 
                                  != BVIEW_STATUS_SUCCESS)               
    {
       SB_DEBUG_PRINT (BVIEW_LOG_ERROR, 
           "Failed to get cpu rules information\n");
       return BVIEW_STATUS_FAILURE;
    }

    if (ptLtcCpuGroup != SB_BRCM_FIELD_ENTRY_INVALID)
    {
      if (ptLtcCpuEntry != SB_BRCM_FIELD_ENTRY_INVALID)
      {
         /* Destroy any attached policer before destroying the entry */
        rv = SB_BRCM_FIELD_ENTRY_POLICER_GET(asic, ptLtcCpuEntry, 0, &policerId);
        if (rv == SB_BRCM_E_NONE)
        {
          SB_BRCM_FIELD_ENTRY_POLICER_DETACH(asic, ptLtcCpuEntry, 0);

          /* policer deletion might fail if the policer is being shared */
          (void) SB_BRCM_POLICER_DESTROY(asic, policerId);
        }

        rv = SB_BRCM_FIELD_ENTRY_REMOVE(asic, ptLtcCpuEntry);
        rv = SB_BRCM_FIELD_ENTRY_DESTROY(asic, ptLtcCpuEntry);
      } 

      rv = SB_BRCM_FIELD_GROUP_DESTORY (asic, ptLtcCpuGroup);
      if (rv != SB_BRCM_E_NONE)
      {
        return BVIEW_STATUS_FAILURE;
      }
    }
    /* Clear the deleted cpu rule info */ 
    if (sbplugin_pt_ltc_cpu_rule_clear(asic) != BVIEW_STATUS_SUCCESS) 
    {
       SB_DEBUG_PRINT (BVIEW_LOG_ERROR, 
           "Failed to clear rule map for cpu rules\n");
      return BVIEW_STATUS_FAILURE;
    }
  }
  return BVIEW_STATUS_SUCCESS;
}



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
BVIEW_STATUS  sbplugin_common_pt_build_info (SB_BRCM_PKT_t *pkt, int *requestId)
{
  BVIEW_STATUS rv = BVIEW_STATUS_FAILURE;

  BVIEW_NULLPTR_CHECK(pkt);

  rv = sbplugin_pt_ltc_rule_to_req_map_get( pkt->unit, pkt->rx_matched,
                                                            requestId);

  return rv;
}



