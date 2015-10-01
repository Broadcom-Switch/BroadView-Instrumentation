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
#include "packet_trace.h"
#include "sbplugin_common_packet_trace.h"
#include "sbplugin_common.h"
#include "sbplugin_common_system.h"
#include "common/platform_spec.h"
#include "sbplugin_system_map.h"
#include "sbplugin_packet_trace_map.h"

#define    PKT_TRACE_ECMP_2_INDEX     1
#define    PKT_TRACE_ECMP_1_INDEX     0
#define    BVIEW_ECMP_IDX_START       200000
#define    BVIEW_L3_EGRESS_IDX_START  100000

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
    traceProfile->hashingInfo.flags &= BVIEW_PT_ECMP_1_RESOLUTION;
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
    traceProfile->hashingInfo.flags &= BVIEW_PT_ECMP_2_RESOLUTION;
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
    traceProfile->hashingInfo.flags &= BVIEW_PT_TRUNK_RESOLUTION;
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
    traceProfile->hashingInfo.flags &= BVIEW_PT_FABRIC_TRUNK_RESOLUTION;
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

