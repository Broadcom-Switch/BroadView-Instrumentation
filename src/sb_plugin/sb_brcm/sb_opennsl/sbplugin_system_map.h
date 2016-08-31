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

#include "sbplugin.h"
#include "sbplugin_system.h"
#include "openapps_log_api.h"
#include "sal/commdefs.h"
#include "opennsl/types.h"
#include "opennsl/init.h"
#include "opennsl/switch.h"
#include "opennsl/port.h"
#include "opennsl/cosq.h"
#include "opennsl/stat.h"
#include "opennsl/error.h"
#include "opennsl/field.h"
#include "opennsl/pkt.h"
#include "opennsl/rx.h"
#include "shared/pbmp.h"
#include "opennsl/trunk.h"
#include "opennsl/l3.h"
#include "sbplugin.h"
#include "sbplugin_system.h"

#define  SBPLUGIN_NETWORK_OS   "openNSL" 

/* TRIDENT 2      */ 
#define SB_BRCM_56850_DEVICE_ID      0xb850
#define SB_BRCM_56851_DEVICE_ID      0xb851
#define SB_BRCM_56852_DEVICE_ID      0xb852
#define SB_BRCM_56853_DEVICE_ID      0xb853
#define SB_BRCM_56854_DEVICE_ID      0xb854
#define SB_BRCM_56855_DEVICE_ID      0xb855

/* Trident 2 plus */
#define SB_BRCM_56860_DEVICE_ID      0xb860
#define SB_BRCM_56861_DEVICE_ID      0xb861
#define SB_BRCM_56862_DEVICE_ID      0xb862
#define SB_BRCM_56864_DEVICE_ID      0xb864
#define SB_BRCM_56865_DEVICE_ID      0xb865
#define SB_BRCM_56867_DEVICE_ID      0xb867
#define SB_BRCM_56760_DEVICE_ID      0xb760
#define SB_BRCM_56832_DEVICE_ID      0xb832
#define SB_BRCM_56833_DEVICE_ID      0xb833
#define SB_BRCM_56865_DEVICE_ID      0xb865
#define SB_BRCM_56833_DEVICE_ID      0xb833
/* Tomahawk */
#define SB_BRCM_56960_DEVICE_ID      0xb960
#define SB_BRCM_56961_DEVICE_ID      0xb961
#define SB_BRCM_56962_DEVICE_ID      0xb962
#define SB_BRCM_56965_DEVICE_ID      0xb965


/*QAX*/
#define SB_BRCM_88375_DEVICE_ID      0x8375



#define SB_BRCM_IS_CHIP_TD2(_info)                                               \
                                   (((_info).device == SB_BRCM_56850_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56851_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56852_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56853_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56854_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56855_DEVICE_ID)) 


#define SB_BRCM_IS_CHIP_TD2_PLUS(_info)                                          \
                                   (((_info).device == SB_BRCM_56860_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56861_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56862_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56864_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56865_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56867_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56760_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56832_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56833_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56865_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56833_DEVICE_ID)) 

#define SB_BRCM_IS_CHIP_TH(_info)                                                \
                                   (((_info).device == SB_BRCM_56960_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56961_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56962_DEVICE_ID) || \
                                    ((_info).device == SB_BRCM_56965_DEVICE_ID)) 

#define SB_BRCM_IS_CHIP_QUMRAN(_info) ((_info).device == SB_BRCM_88375_DEVICE_ID)

#define SB_BRCM_E_NONE          OPENNSL_E_NONE            
#define SB_BRCM_E_BUSY          OPENNSL_E_BUSY
#define SB_BRCM_E_NOT_FOUND     OPENNSL_E_NOT_FOUND
#define SB_BRCM_E_EXISTS        OPENNSL_E_EXISTS

#define SB_BRCM_RV_ERROR(_rv)     ((_rv) < 0)

#define SB_BRCM_PBMP_COUNT(_port_bit_map, _count)     OPENNSL_PBMP_COUNT((_port_bit_map), (_count))

#define SB_BRCM_CPU_PORT_GET(_unit, _port_ptr)                                                \
                             {                                                              \
                                int _port = 0;                                               \
                                while(_port < OPENNSL_PBMP_PORT_MAX)                             \
                                {                                                           \
                                  if (OPENNSL_PBMP_MEMBER(sb_brcm_port_config[(_unit)].cpu, _port)) \
                                  {                                                         \
                                    *(_port_ptr) = _port;                                       \
                                    break;                                                 \
                                  }                                                         \
                                  _port++;                                                   \
                                }                                                           \
                             }

#define SB_BRCM_IS_XE_PORT(_unit, _port)                                            \
                          (OPENNSL_PBMP_MEMBER(sb_brcm_port_config[(_unit)].xe, (_port)))    
						   
#define SB_BRCM_API_PORT_GPORT_GET(_unit, _port, _gport_ptr)                        \
                                opennsl_port_gport_get((_unit), (_port), (_gport_ptr))

#define SB_BRCM_API_SWITCH_CONTROL_GET(_unit, _control_type, _arg_ptr)                 \
                             opennsl_switch_control_get((_unit), (_control_type), (_arg_ptr)) 

#define SB_BRCM_API_SWITCH_CONTROL_SET(_unit, _control_type, _arg)                     \
                             opennsl_switch_control_set((_unit), (_control_type), (_arg)) 


#define SB_BRCM_API_UNIT_INFO_GET(_unit,_info_ptr)      opennsl_info_get((_unit),(_info_ptr))


#define SOC_BRCM_API_ATTACH_MAX_UNITS_GET(_max_units_ptr)  { opennsl_attach_max((int *)_max_units_ptr);    \
                                                   *(_max_units_ptr) = (*(_max_units_ptr))+1; }


#define SB_BRCM_API_UNIT_CHECK(_unit)              \
                              opennsl_attach_check(_unit)

#define SB_BRCM_API_PORT_CONFIG_GET(_unit,_port_cfg_ptr)           \
                  opennsl_port_config_get(_unit, _port_cfg_ptr)  

#define SB_BRCM_SWITCH_EVENT_REGISTER(_unit, _cb, _user_data_ptr)     \
            opennsl_switch_event_register(_unit, _cb, _user_data_ptr)



#define SB_BRCM_PBMP_CLEAR(_pbmp)  OPENNSL_PBMP_CLEAR(_pbmp)
#define SB_BRCM_PBMP_ASSIGN(_dst, _src)  OPENNSL_PBMP_ASSIGN(_dst, _src)
#define SB_BRCM_PBMP_AND(_dst, _src)  OPENNSL_PBMP_AND(_dst, _src)
#define SB_BRCM_PBMP_IS_NULL(_pbmp)        OPENNSL_PBMP_IS_NULL(_pbmp)
#define SB_BRCM_PBMP_MEMBER(_pbmp, _port)  OPENNSL_PBMP_MEMBER(_pbmp, _port)
#define SB_BRCM_PBMP_PORT_REMOVE(_pbmp, _port)  OPENNSL_PBMP_PORT_REMOVE(_pbmp, _port)
#define SB_BRCM_PBMP_PORT_ADD(_pbmp, _port)     OPENNSL_PBMP_PORT_ADD(_pbmp, _port)
#define SB_BRCM_PBMP_ITER(_pbmp, _port)        OPENNSL_PBMP_ITER(_pbmp, _port)
#define SB_BRCM_RX_HANDLED                     OPENNSL_RX_HANDLED
#define SB_BRCM_API_SWITCH_RX_REGISTER(_asic, _name, _fn, _pri, _cookie, _flags) \
         opennsl_rx_register (_asic, _name, _fn, _pri, _cookie, _flags)

#define SB_BRCM_RX_START(_asic,_cfg)       opennsl_rx_start(_asic,_cfg)
#define SB_BRCM_RCO_F_ALL_COS  OPENNSL_RCO_F_ALL_COS

#define SB_BRCM_API_L3_HOST_TRAVERSE         opennsl_l3_host_traverse
#define SB_BRCM_API_L3_INFO                  opennsl_l3_info


typedef opennsl_gport_t             SB_BRCM_GPORT_t;
typedef opennsl_info_t              SB_BRCM_UNIT_INFO_t;
typedef opennsl_port_config_t       SB_BRCM_PORT_CONFIG_t;
typedef opennsl_switch_event_t      SB_BRCM_SWITCH_EVENT_t;
typedef opennsl_switch_event_cb_t   SB_BRCM_SWITCH_EVENT_CB_t;
typedef opennsl_pkt_t               SB_BRCM_PKT_t;
typedef opennsl_rx_t                SB_BRCM_RX_t;
typedef opennsl_pbmp_t                  SB_BRCM_PBMP_t;
typedef  opennsl_l3_host_t               SB_BRCM_L3_HOST_t;
typedef  opennsl_l3_info_t               SB_BRCM_L3_INFO_t;

extern SB_BRCM_PORT_CONFIG_t     sb_brcm_port_config[];

typedef opennsl_field_aset_t         SB_BRCM_FIELD_ASET_t;
typedef opennsl_field_qset_t         SB_BRCM_FIELD_QSET_t;
typedef opennsl_field_presel_set_t   SB_BRCM_FIELD_PRESEL_SET_t;

typedef opennsl_field_group_t            SB_BRCM_FIELD_GROUP_CONFIG_t;
typedef opennsl_field_entry_t        SB_BRCM_FIELD_ENTRY_t;
typedef opennsl_policer_config_t     SB_BRCM_POLICER_CONFIG_t;
typedef opennsl_policer_t           SB_BRCM_POLICER_t;
typedef opennsl_field_stat_t         SB_BRCM_FIELD_STAT_t;
typedef opennsl_mirror_destination_t         SB_BRCM_MIRROR_DESTINATION_t;
typedef opennsl_rx_snoop_config_t    SB_BRCM_RX_SNOOP_CONFIG_t;
typedef opennsl_rx_trap_config_t     SB_BRCM_RX_TRAP_CONFIG_t;


typedef opennsl_cosq_gport_traverse_cb  SB_BRCM_COSQ_GPORT_TRAVERSE_CB;

#define   SB_BRCM_COSQ_STAT_DROPPED_PACKETS opennslCosqStatDroppedPackets

#define   SB_BRCM_FIELD_STAT_COUNT          opennslFieldStatCount
#define   SB_BRCM_FIELD_QSET_INIT(_qset)  OPENNSL_FIELD_QSET_INIT(_qset) 
#define   SB_BRCM_FIELD_QSET_ADD(_qset,_qual)  OPENNSL_FIELD_QSET_ADD(_qset, _qual)

#define   SB_BRCM_FIELD_GROUP_CREATE(_asic, _qset, _pri, _pGroup) \
           opennsl_field_group_create(_asic,_qset, _pri, _pGroup)

#define   SB_BRCM_FIELD_ENTRY_CREATE(_asic, _group, _pEntry)  \
           opennsl_field_entry_create(_asic, _group, _pEntry)

#define   SB_BRCM_FIELD_QUALIFY_STAGE(_asic, _entry, _stage)  \
           opennsl_field_qualify_Stage(_asic, _entry, _stage)

#define   SB_BRCM_FIELD_QUALIFY_INPORTS(_asic,_entry, _inpbmp, _inpbmpMask)  \
           opennsl_field_qualify_InPorts(_asic,_entry, _inpbmp, _inpbmpMask)


#define   SB_BRCM_FIELD_QUALIFY_DSTPORT(_asic,_entry,_dst_modid, _modid_mask,_dst_port, _port_mask) \
           opennsl_field_qualify_DstPort(_asic,_entry,_dst_modid, _modid_mask,_dst_port, _port_mask)

#define    SB_BRCM_FIELD_QUALIFY_SRC_IP(_asic,_entry,_ip_addr,_ip_mask) \
           opennsl_field_qualify_SrcIp(_asic,_entry,_ip_addr,_ip_mask)

#define    SB_BRCM_FIELD_QUALIFY_DST_IP(_asic,_entry,_ip_addr,_ip_mask) \
           opennsl_field_qualify_DstIp(_asic,_entry,_ip_addr,_ip_mask)
           
#define    SB_BRCM_FIELD_QUALIFY_IP_PROTO(_asic,_entry,_protocol,_mask) \
           opennsl_field_qualify_IpProtocol(_asic,_entry,_protocol,_mask)

#define    SB_BRCM_FIELD_QUALIFY_L4_SRC_PORT(_asic,_entry,_src_port,_port_mask) \
           opennsl_field_qualify_L4SrcPort(_asic,_entry,_src_port,_port_mask)

#define    SB_BRCM_FIELD_QUALIFY_L4_DST_PORT(_asic,_entry,_dst_port,_port_mask) \
           opennsl_field_qualify_L4DstPort(_asic,_entry,_dst_port,_port_mask)

#define   SB_BRCM_POLICER_CONFIG_INIT(_pConfig) \
           opennsl_policer_config_t_init(_pConfig)

#define   SB_BRCM_POLICER_MODE_COMMITTED  opennslPolicerModeCommitted
#define   SB_BRCM_POLICER_MODE_SR_TCM     opennslPolicerModeSrTcm

#define   SB_BRCM_COSQ_GPORT_UCAST_QUEUE_GROUP  OPENNSL_COSQ_GPORT_UCAST_QUEUE_GROUP
#define   SB_BRCM_COSQ_GPORT_MCAST_QUEUE_GROUP  OPENNSL_COSQ_GPORT_MCAST_QUEUE_GROUP

#define   SB_BRCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(_gport)    \
                       OPENNSL_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(_gport)

#define   SB_BRCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(_gport)    \
                       OPENNSL_GPORT_UCAST_QUEUE_GROUP_QID_GET(_gport)

#define   SB_BRCM_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_GET(_gport)    \
                       OPENNSL_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_GET(_gport)

#define   SB_BRCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(_gport)    \
                       OPENNSL_GPORT_MCAST_QUEUE_GROUP_QID_GET(_gport)



#define   SB_BRCM_POLICER_MODE_PACKETS    OPENNSL_POLICER_MODE_PACKETS
#define   SB_BRCM_POLICER_COLOR_BLIND     OPENNSL_POLICER_COLOR_BLIND

#define   SB_BRCM_FIELD_ENTRY_INVALID   (-1)
#define   SB_BRCM_POLICER_WITH_ID    OPENNSL_POLICER_WITH_ID
#define   SB_BRCM_FIELD_ACTION_COPYTOCPU              opennslFieldActionCopyToCpu
#define   SB_BRCM_FIELD_ACTION_RP_COPYTOCPU           opennslFieldActionRpCopyToCpu
#define   SB_BRCM_FIELD_ACTION_INGSAMPLEENABLE        opennslFieldActionIngSampleEnable
#define   SB_BRCM_FIELD_ACTION_RP_DROP                opennslFieldActionRpDrop
#define   SB_BRCM_FIELD_ACTION_EGRESS_CLASS_SELECT    opennslFieldActionEgressClassSelect
#define   SB_BRCM_FIELD_ACTION_NEW_CLASS_ID           opennslFieldActionNewClassId
#define   SB_BRCM_FIELD_ACTION_DROP                   opennslFieldActionDrop
#define   SB_BRCM_FIELD_ACTION_SNOOP                  opennslFieldActionSnoop
#define   SB_BRCM_FIELD_ACTION_STAT0                  opennslFieldActionStat0
#define   SB_BRCM_FIELD_ACTION_USE_POLICER_RESULT     opennslFieldActionUsePolicerResult
#define   SB_BRCM_FIELD_ACTION_POLICER_LEVEL0         opennslFieldActionPolicerLevel0
#define   SB_BRCM_FIELD_ACTION_CLASS_DEST_SET         opennslFieldActionClassDestSet
#define   SB_BRCM_FIELD_ACTION_CLASS_SOURCE_SET       opennslFieldActionClassSourceSet


#define   SB_BRCM_FIELD_STATS_PACKETS   opennslFieldStatPackets
#define   SB_BRCM_FIELD_STAGE_INGRESS   opennslFieldStageIngress
#define   SB_BRCM_QUALIFY_INPORTS       opennslFieldQualifyInPorts
#define   SB_BRCM_QUALIFY_DSTPORT       opennslFieldQualifyDstPort
#define   SB_BRCM_QUALIFY_STAGE_INGRESS  opennslFieldQualifyStageIngress
#define   SB_BRCM_QUALIFY_SRC_IP         opennslFieldQualifySrcIp
#define   SB_BRCM_QUALIFY_DST_IP         opennslFieldQualifyDstIp
#define   SB_BRCM_QUALIFY_IP_PROTO       opennslFieldQualifyIpProtocol
#define   SB_BRCM_QUALIFY_L4_SRC_PORT    opennslFieldQualifyL4SrcPort
#define   SB_BRCM_QUALIFY_L4_DST_PORT    opennslFieldQualifyL4DstPort
#define   SB_BRCM_FIELD_QUALIFY_DST_CLASS_FIELD_ENUM     opennslFieldQualifyDstClassField

#define SB_BRCM_QSET_FIELD_QUALIFY_STAGE_EGRESS     opennslFieldQualifyStageEgress
#define SB_BRCM_QSET_FIELD_QUALIFY_OUT_PORT         opennslFieldQualifyOutPort
#define SB_BRCM_QSET_FIELD_QUALIFY_INGRESS_CLASS_FIELD  opennslFieldQualifyIngressClassField
#define   SB_BRCM_FIELD_QUALIFY_INTERFACE_CLASS_PROCESSING_PORT_ENUM  opennslFieldQualifyInterfaceClassProcessingPort
#define   SB_BRCM_FIELD_QUALIFY_COLOR_ENUM    opennslFieldQualifyColor
#define   SB_BRCM_FIELD_QUALIFY_PRESEL        OPENNSL_FIELD_QUALIFY_PRESEL
#define   SB_BRCM_FIELD_COLOR_GREEN           OPENNSL_FIELD_COLOR_GREEN
#define   SB_BRCM_PORT_CLASS_FIELD_EGRESS_PACKET_PROCESSING      opennslPortClassFieldEgressPacketProcessing

#define   SB_BRCM_SAMPLE_FLEX_SAMPLED_PKTS    snmpOpenNSLSampleFlexSampledPkts
#define   SB_BRCM_SAMPLE_FLEX_SNAPSHOT_PKTS   snmpOpenNSLSampleFlexSnapshotPkts
#define   SB_BRCM_SAMPLE_FLEX_PKTS            snmpOpenNSLSampleFlexPkts
#define   SB_BRCM_MIRROR_PORT_INGRESS         OPENNSL_MIRROR_PORT_INGRESS
#define   SB_BRCM_MIRROR_PORT_SFLOW           OPENNSL_MIRROR_PORT_SFLOW
#define   SB_BRCM_MIRROR_DEST_TUNNEL_SFLOW    OPENNSL_MIRROR_DEST_TUNNEL_SFLOW
#define   SB_BRCM_MIRROR_DEST_WITH_ID         OPENNSL_MIRROR_DEST_WITH_ID
#define   SB_BRCM_RX_TRAP_USER_DEFINE         opennslRxTrapUserDefine 
#define   SB_BRCM_RX_SNOOP_UPDATE_DEST        OPENNSL_RX_SNOOP_UPDATE_DEST
#define   SB_BRCM_GPORT_LOCAL_CPU             OPENNSL_GPORT_LOCAL_CPU
#define   SB_BRCM_RX_TRAP_REPLACE             OPENNSL_RX_TRAP_REPLACE

#define   SB_BRCM_PORT_CONTROL_SAMPLE_FLEX_RATE  opennslPortControlSampleFlexRate
#define   SB_BRCM_PORT_CONTROL_SAMPLE_FLEX_DEST  opennslPortControlSampleFlexDest
#define   SB_BRCM_PORT_CONTROL_SAMPLE_DEST_MIRROR  OPENNSL_PORT_CONTROL_SAMPLE_DEST_MIRROR
#define   SB_BRCM_SWITCH_SAMPLE_INGRESS_RANDOM_SEED  opennslSwitchSampleIngressRandomSeed

#define   SB_BRCM_SWITCH_DIRECTED_MIRRORING    opennslSwitchDirectedMirroring
#define   SB_BRCM_SWITCH_FLEXIBLE_MIRROR_DESTINATIONS  opennslSwitchFlexibleMirrorDestinations

#define  SB_BRCM_POLICER_CREATE(_asic,_pPolicer, _id)  \
                  opennsl_policer_create(_asic,_pPolicer, _id)

#define   SB_BRCM_FIELD_ENTRY_POLICER_ATTACH(_asic,_entry,_level,_policerid) \
                  opennsl_field_entry_policer_attach(_asic,_entry,_level,_policerid)
#define   SB_BRCM_FIELD_STAT_CREATE(_asic,_group,_nstat,_pStattype, _pStatId) \
                  opennsl_field_stat_create(_asic,_group,_nstat,_pStattype, _pStatId)
#define   SB_BRCM_FIELD_STAT_ATTACH(_asic,_entry,_statId) \
                  opennsl_field_entry_stat_attach(_asic,_entry,_statId)
#define   SB_BRCM_FIELD_STAT_DETACH(_asic,_entry,_statId) \
                  opennsl_field_entry_stat_detach(_asic,_entry,_statId)
#define   SB_BRCM_FIELD_ACTION_ADD(_asic,_entry,_action,_param0,_param1) \
                  opennsl_field_action_add(_asic,_entry,_action,_param0,_param1)
#define   SB_BRCM_FIELD_GROUP_INSTALL(_asic,_group) \
                  opennsl_field_group_install(_asic,_group)
#define   SB_BRCM_FIELD_STAT_GET(_asic, _statId, _stat_type,_pCount) \
                  opennsl_field_stat_get(_asic, _statId, _stat_type,_pCount)
#define   SB_BRCM_FIELD_STAT_DESTORY(_asic, _statId) \
                  opennsl_field_stat_destroy(_asic, _statId)
#define   SB_BRCM_FIELD_GROUP_DESTORY(_asic, _group) \
                  opennsl_field_group_destroy(_asic, _group)
#define   SB_BRCM_STAT_CLEAR_SINGLE(_asic, _port, _stat_type)  \
                  opennsl_stat_clear_single(_asic, _port, _stat_type)
#define   SB_BRCM_FIELD_ENTRY_STAT_GET(_asic,_entry,_statId)  \
                  opennsl_field_entry_stat_get(_asic,_entry,_statId)


#define   SB_BRCM_FIELD_QUALIFY_OUTPORT(_asic, _entry, _port, _mask) \
                 opennsl_field_qualify_OutPort(_asic, _entry, _port, _mask)

#define   SB_BRCM_FIELD_QUALIFY_INGRESSCLASSFIELD(_asic, _entry, _class_id, _mask) \
                 opennsl_field_qualify_IngressClassField(_asic, _entry, _class_id, _mask)

#define   SB_BRCM_FIELD_QUALIFY_INTERFACE_CLASS_PROCESSING_PORT(_asic, _entry, _port_class, _mask)  \
                 opennsl_field_qualify_InterfaceClassProcessingPort(_asic, _entry, _port_class, _mask)

#define   SB_BRCM_FIELD_ENTRY_REMOVE(_asic,_entry)        \
                 opennsl_field_entry_remove(_asic,_entry)

#define   SB_BRCM_FIELD_ENTRY_DESTROY(_asic,_entry)       \
                 opennsl_field_entry_destroy(_asic,_entry)

#define   SB_BRCM_FIELD_ENTRY_POLICER_GET(_asic,_entry,_level,_policer_id_ptr)   \
                 opennsl_field_entry_policer_get(_asic,_entry,_level,_policer_id_ptr)

#define   SB_BRCM_FIELD_ENTRY_POLICER_DETACH(_asic,_entry,_level)    \
                 opennsl_field_entry_policer_detach(_asic,_entry,_level)

#define  SB_BRCM_FIELD_ENTRY_PRIO_SET(_asic, _entry, _priority)  \
                 opennsl_field_entry_prio_set(_asic, _entry, _priority)

#define   SB_BRCM_POLICER_DESTROY(_asic,_policer_id)   \
                 opennsl_policer_destroy(_asic,_policer_id)

#define   SB_BRCM_STAT_GET32(_asic, _port, _stat_type, _pCount) \
                  opennsl_stat_get32(_asic, _port, _stat_type, _pCount)

#define    SB_BRCM_MIRROR_INIT(_asic)  \
                  opennsl_mirror_init(_asic)

#define   SB_BRCM_GPORT_MIRROR_SET(_gport, _value) \
                  OPENNSL_GPORT_MIRROR_SET(_gport, _value)

#define   SB_BRCM_MIRROR_DESTINATION_INIT(_mirror_dest)  \
                  opennsl_mirror_destination_t_init(_mirror_dest)

#define   SB_BRCM_MIRROR_PORT_DESTINATION_CREATE(_asic, _mirror_dest)  \
                  opennsl_mirror_destination_create(_asic,  _mirror_dest)

#define   SB_BRCM_MIRROR_PORT_DEST_ADD(_asic, _port, _flag, _mirror_dest)  \
                  opennsl_mirror_port_dest_add(_asic, _port, _flag, _mirror_dest)

#define   SB_BRCM_MIRROR_DESTINATION_GET(_asic, _port, _mirror_dest)  \
                  opennsl_mirror_destination_get(_asic, _port, _mirror_dest)

#define   SB_BRCM_MIRROR_PORT_DEST_DELETE(_asic, _port, _flag, _mirror_dest)  \
                  opennsl_mirror_port_dest_delete(_asic, _port, _flag, _mirror_dest)

#define   SB_BRCM_MIRROR_PORT_DESTINATION_DESTROY(_asic, _port)  \
                  opennsl_mirror_destination_destroy(_asic, _port)

#define   SB_BRCM_RX_SNOOP_CONFIG_INIT(_snoop_config)  \
                  opennsl_rx_snoop_config_t_init(_snoop_config)

#define   SB_BRCM_RX_SNOOP_CREATE(_asic, _flag, _snoop_cmd)  \
                  opennsl_rx_snoop_create(_asic, _flag, _snoop_cmd)

#define   SB_BRCM_RX_SNOOP_SET(_asic,  _snoop_cmd, _snoop_config)  \
                  opennsl_rx_snoop_set(_asic, _snoop_cmd, _snoop_config)

#define   SB_BRCM_RX_SNOOP_DESTROY(_asic, _flag, _snoop_cmd)  \
                  opennsl_rx_snoop_destroy(_asic, _flag, _snoop_cmd)

#define   SB_BRCM_RX_TRAP_CONFIG_INIT(_trap_config)  \
                  opennsl_rx_trap_config_t_init(_trap_config)

#define   SB_BRCM_RX_TRAP_TYPE_CREATE(_asic, _flag, _trap_type, _trap_id)  \
                  opennsl_rx_trap_type_create(_asic, _flag, _trap_type, _trap_id)

#define   SB_BRCM_RX_TRAP_SET(_asic, _trap_id, _trap_config)  \
                  opennsl_rx_trap_set(_asic,_trap_id, _trap_config)

#define   SB_BRCM_RX_TRAP_TYPE_DESTROY(_asic, _trap_id)  \
                  opennsl_rx_trap_type_destroy(_asic, _trap_id)
#define   SB_BRCM_GPORT_TRAP_SET(_gport, _trap_id, _trap_strength, _snoop_strength)  \
                  OPENNSL_GPORT_TRAP_SET(_gport, _trap_id, _trap_strength, _snoop_strength)

#define   SB_BRCM_FIELD_PRESEL_INIT(_presel)  \
                  OPENNSL_FIELD_PRESEL_INIT(_presel)

#define   SB_BRCM_FIELD_PRESEL_CREATE(_asic, _preselId)  \
                  opennsl_field_presel_create(_asic, _preselId)

#define   SB_BRCM_FIELD_PRESEL_ADD(_presel, _preselId)  \
                  OPENNSL_FIELD_PRESEL_ADD(_presel, _preselId)

#define   SB_BRCM_FIELD_GROUP_PRESEL_SET(_asic, _group, _presel)  \
                  opennsl_field_group_presel_set(_asic, _group, _presel)

#define   SB_BRCM_FIELD_ASET_INIT(_aset)  \
                  OPENNSL_FIELD_ASET_INIT(_aset)

#define   SB_BRCM_FIELD_ASET_ADD(_aset, _type)  \
                  OPENNSL_FIELD_ASET_ADD(_aset, _type)\

#define   SB_BRCM_FIELD_GROUP_ACTION_SET(_asic, _group, _aset)  \
                  opennsl_field_group_action_set(_asic, _group, _aset)

#define   SB_BRCM_PORT_CLASS_SET(_asic, _port, _port_class, _class_id)  \
                  opennsl_port_class_set(_asic, _port, _port_class, _class_id)

#define   SB_BRCM_FIELD_QUALIFY_COLOR(_asic, _entry, _color)  \
                  opennsl_field_qualify_Color(_asic, _entry, _color)

#define   SB_BRCM_FIELD_QUALIFY_DST_CLASS_FIELD(_asic, _entry, _data, _mask)  \
                  opennsl_field_qualify_DstClassField(_asic, _entry, _data, _mask)

#define   SB_BRCM_FIELD_QUALIFY_SRC_CLASS_FIELD(_asic, _entry, _data, _mask)  \
                  opennsl_field_qualify_SrcClassField(_asic, _entry, _data, _mask)

#define   SB_BRCM_PORT_CONTROL_SET(_asic, _port, _type, _value)  \
                  opennsl_port_control_set(_asic, _port, _type, _value)


#define  SB_BRCM_COSQ_GPORT_TRAVERSE(_aisc, _cb, _user_data_ptr)   \
                 opennsl_cosq_gport_traverse(_aisc, _cb, _user_data_ptr)

#define  SB_BRCM_COSQ_STAT_GET(_asic, _port, _cosq, _stat, _val_ptr) \
                 opennsl_cosq_stat_get(_asic, _port, _cosq, _stat, _val_ptr)



#ifdef __cplusplus
}
#endif
#endif
