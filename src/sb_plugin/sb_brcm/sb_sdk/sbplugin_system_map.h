/*********************************************************************
*
* Copyright © 2016 Broadcom.  The term "Broadcom" refers
* to Broadcom Limited and/or its subsidiaries.
*
***********************************************************************
*
* @filename sbplugin_system_map.h
*
* @purpose BroadView SDK south bound plugin API's and Macros
*
* @component South Bound Plugin
*
* @comments
*
* @create 12/16/2014
*
* @author Eeshwar
* @end
*
**********************************************************************/

#ifndef INCLUDE_SBPLUGIN_SYSTEM_MAP_H
#define INCLUDE_SBPLUGIN_SYSTEM_MAP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "sbplugin.h"
#include "sbplugin_system.h"
#include "openapps_log_api.h"
#include "soc/mcm/allenum.h"
#include "soc/drv.h"
#include "soc/defs.h"
#include "bcm/cosq.h"
#include "sbplugin.h"
#include "bcm/error.h"
#include "bcm/switch.h"
#include "bcm_int/control.h"
#include "bcmx/types.h"
#include <bcm/port.h>
#include <bcm/types.h>
#include "sbplugin_system.h"

#define  SBPLUGIN_NETWORK_OS   "SDK"

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


#define SB_BRCM_E_NONE          BCM_E_NONE            
#define SB_BRCM_E_BUSY          BCM_E_BUSY            
#define SB_BRCM_E_NOT_FOUND     BCM_E_NOT_FOUND
#define SB_BRCM_E_EXISTS        BCM_E_EXISTS

#define SB_BRCM_RV_ERROR(_rv)     ((_rv) < 0)

#define SB_BRCM_PBMP_COUNT(_port_bit_map, _count)     BCM_PBMP_COUNT((_port_bit_map), (_count))

#define SB_BRCM_CPU_PORT_GET(_unit, _port_ptr)                                                \
                             {                                                              \
                                int _port = 0;                                               \
                                while(_port < BCM_PBMP_PORT_MAX)                             \
                                {                                                           \
                                  if (BCM_PBMP_MEMBER(sb_brcm_port_config[(_unit)].cpu, _port)) \
                                  {                                                         \
                                    *(_port_ptr) = _port;                                       \
                                    break;                                                 \
                                  }                                                         \
                                  _port++;                                                   \
                                }                                                           \
                             }

#define SB_BRCM_IS_XE_PORT(_unit, _port)                                            \
                          (BCM_PBMP_MEMBER(sb_brcm_port_config[(_unit)].xe, (_port)))    
						   
#define SB_BRCM_API_PORT_GPORT_GET(_unit, _port, _gport_ptr)                        \
                                bcm_port_gport_get((_unit), (_port), (_gport_ptr))

#define SB_BRCM_API_SWITCH_CONTROL_GET(_unit, _control_type, _arg_ptr)                 \
                             bcm_switch_control_get((_unit), (_control_type), (_arg_ptr)) 

#define SB_BRCM_API_SWITCH_CONTROL_SET(_unit, _control_type, _arg)                     \
                             bcm_switch_control_set((_unit), (_control_type), (_arg)) 


#define SB_BRCM_API_UNIT_INFO_GET(_unit,_info_ptr)      bcm_info_get((_unit),(_info_ptr))


#define SOC_BRCM_API_ATTACH_MAX_UNITS_GET(_max_units_ptr)  { bcm_attach_max((int *)_max_units_ptr);    \
                                                   *(_max_units_ptr) = (*(_max_units_ptr))+1; }


#define SB_BRCM_API_UNIT_CHECK(_unit)              \
                              bcm_attach_check(_unit)

#define SB_BRCM_API_PORT_CONFIG_GET(_unit,_port_cfg_ptr)           \
                  bcm_port_config_get(_unit, _port_cfg_ptr)  

#define SB_BRCM_SWITCH_EVENT_REGISTER(_unit, _cb, _user_data_ptr)     \
            bcm_switch_event_register(_unit, _cb, _user_data_ptr)


#define SB_BRCM_PORT_LOCAL_GET(_unit,_gport,_localport_ptr)                 \
            bcm_port_local_get(_unit, _gport, _localport_ptr)

#define SB_BRCM_PBMP_CLEAR(_pbmp)  BCM_PBMP_CLEAR(_pbmp)
#define SB_BRCM_PBMP_ASSIGN(_dst, _src)  BCM_PBMP_ASSIGN(_dst, _src)
#define SB_BRCM_PBMP_AND(_dst, _src)  BCM_PBMP_AND(_dst, _src)
#define SB_BRCM_PBMP_IS_NULL(_pbmp) BCM_PBMP_IS_NULL(_pbmp)
#define SB_BRCM_PBMP_MEMBER(_pbmp, _port)  BCM_PBMP_MEMBER(_pbmp, _port)
#define SB_BRCM_PBMP_PORT_REMOVE(_pbmp, _port)  BCM_PBMP_PORT_REMOVE(_pbmp, _port)
#define SB_BRCM_PBMP_PORT_ADD(_pbmp, _port)  BCM_PBMP_PORT_ADD(_pbmp, _port)
#define SB_BRCM_PBMP_ITER(_pbmp, _port)        BCM_PBMP_ITER(_pbmp, _port)         
#define SB_BRCM_RX_HANDLED                     BCM_RX_HANDLED         
#define SB_BRCM_API_SWITCH_RX_REGISTER(_asic, _name, _fn, _pri, _cookie, _flags) \
         bcm_rx_register (_asic, _name, _fn, _pri, _cookie, _flags)

#define SB_BRCM_RX_START(_asic,_cfg)       bcm_rx_start(_asic,_cfg)
#define SB_BRCM_RCO_F_ALL_COS  BCM_RCO_F_ALL_COS

#define SB_BRCM_API_L3_INFO                  bcm_l3_info
#define SB_BRCM_API_L3_HOST_TRAVERSE         bcm_l3_host_traverse

typedef bcm_gport_t             SB_BRCM_GPORT_t;
typedef bcm_port_t              SB_BRCM_PORT_t;
typedef bcm_info_t              SB_BRCM_UNIT_INFO_t;
typedef bcm_port_config_t       SB_BRCM_PORT_CONFIG_t;
typedef bcm_switch_event_t      SB_BRCM_SWITCH_EVENT_t;
typedef bcm_switch_event_cb_t   SB_BRCM_SWITCH_EVENT_CB_t;
typedef bcm_if_t                SB_BRCM_IF_t;
typedef bcm_pkt_t               SB_BRCM_PKT_t;
typedef bcm_rx_t                SB_BRCM_RX_t;
typedef bcm_pbmp_t              SB_BRCM_PBMP_t;
typedef  bcm_l3_host_t     SB_BRCM_L3_HOST_t;
typedef  bcm_l3_info_t     SB_BRCM_L3_INFO_t;

extern SB_BRCM_PORT_CONFIG_t     sb_brcm_port_config[];

typedef bcm_field_aset_t         SB_BRCM_FIELD_ASET_t;
typedef bcm_field_qset_t         SB_BRCM_FIELD_QSET_t;
typedef bcm_field_presel_set_t   SB_BRCM_FIELD_PRESEL_SET_t;
typedef bcm_field_group_t        SB_BRCM_FIELD_GROUP_CONFIG_t;
typedef bcm_field_entry_t        SB_BRCM_FIELD_ENTRY_t;
typedef bcm_policer_config_t     SB_BRCM_POLICER_CONFIG_t;
typedef bcm_policer_t            SB_BRCM_POLICER_t;
typedef bcm_field_stat_t         SB_BRCM_FIELD_STAT_t;
typedef bcm_mirror_destination_t         SB_BRCM_MIRROR_DESTINATION_t;
typedef bcm_rx_snoop_config_t    SB_BRCM_RX_SNOOP_CONFIG_t;
typedef bcm_rx_trap_config_t     SB_BRCM_RX_TRAP_CONFIG_t;

typedef bcm_cosq_gport_traverse_cb  SB_BRCM_COSQ_GPORT_TRAVERSE_CB;

#define   SB_BRCM_FIELD_ENTRY_INVALID     (-1)

#define   SB_BRCM_COSQ_STAT_DROPPED_PACKETS bcmCosqStatDroppedPackets

#define   SB_BRCM_FIELD_STAT_COUNT          bcmFieldStatCount
#define   SB_BRCM_FIELD_QSET_INIT(_qset)  BCM_FIELD_QSET_INIT(_qset) 
#define   SB_BRCM_FIELD_QSET_ADD(_qset,_qual)  BCM_FIELD_QSET_ADD(_qset, _qual)

#define   SB_BRCM_FIELD_GROUP_CREATE(_asic, _qset, _pri, _pGroup) \
           bcm_field_group_create(_asic,_qset, _pri, _pGroup)

#define   SB_BRCM_FIELD_ENTRY_CREATE(_asic, _group, _pEntry)  \
           bcm_field_entry_create(_asic, _group, _pEntry)

#define   SB_BRCM_FIELD_QUALIFY_STAGE(_asic, _entry, _stage)  \
           bcm_field_qualify_Stage(_asic, _entry, _stage)

#define   SB_BRCM_FIELD_QUALIFY_INPORTS(_asic,_entry, _inpbmp, _inpbmpMask)  \
           bcm_field_qualify_InPorts(_asic,_entry, _inpbmp, _inpbmpMask)


#define   SB_BRCM_FIELD_QUALIFY_DSTPORT(_asic,_entry,_dst_modid, _modid_mask,_dst_port, _port_mask) \
           bcm_field_qualify_DstPort(_asic,_entry,_dst_modid, _modid_mask,_dst_port, _port_mask)

#define    SB_BRCM_FIELD_QUALIFY_SRC_IP(_asic,_entry,_ip_addr,_ip_mask) \
           bcm_field_qualify_SrcIp(_asic,_entry,_ip_addr,_ip_mask)

#define    SB_BRCM_FIELD_QUALIFY_DST_IP(_asic,_entry,_ip_addr,_ip_mask) \
           bcm_field_qualify_DstIp(_asic,_entry,_ip_addr,_ip_mask)
           
#define    SB_BRCM_FIELD_QUALIFY_IP_PROTO(_asic,_entry,_protocol,_mask) \
           bcm_field_qualify_IpProtocol(_asic,_entry,_protocol,_mask)

#define    SB_BRCM_FIELD_QUALIFY_L4_SRC_PORT(_asic,_entry,_src_port,_port_mask) \
           bcm_field_qualify_L4SrcPort(_asic,_entry,_src_port,_port_mask)

#define    SB_BRCM_FIELD_QUALIFY_L4_DST_PORT(_asic,_entry,_dst_port,_port_mask) \
           bcm_field_qualify_L4DstPort(_asic,_entry,_dst_port,_port_mask)


#define   SB_BRCM_POLICER_CONFIG_INIT(_pConfig) \
           bcm_policer_config_t_init(_pConfig)

#define   SB_BRCM_POLICER_MODE_COMMITTED  bcmPolicerModeCommitted
#define   SB_BRCM_POLICER_MODE_SR_TCM     bcmPolicerModeSrTcm
           
#define   SB_BRCM_COSQ_GPORT_UCAST_QUEUE_GROUP  BCM_COSQ_GPORT_UCAST_QUEUE_GROUP
#define   SB_BRCM_COSQ_GPORT_MCAST_QUEUE_GROUP  BCM_COSQ_GPORT_MCAST_QUEUE_GROUP

#define   SB_BRCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(_gport)    \
                       BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(_gport)

#define   SB_BRCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(_gport)    \
                       BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(_gport)

#define   SB_BRCM_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_GET(_gport)    \
                       BCM_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_GET(_gport)

#define   SB_BRCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(_gport)    \
                       BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(_gport)


#define   SB_BRCM_POLICER_MODE_COMMITTED  bcmPolicerModeCommitted

#define   SB_BRCM_POLICER_MODE_PACKETS    BCM_POLICER_MODE_PACKETS
#define   SB_BRCM_POLICER_COLOR_BLIND     BCM_POLICER_COLOR_BLIND

#define   SB_BRCM_POLICER_WITH_ID    BCM_POLICER_WITH_ID
#define   SB_BRCM_FIELD_ACTION_COPYTOCPU  bcmFieldActionCopyToCpu
#define   SB_BRCM_FIELD_ACTION_RP_COPYTOCPU  bcmFieldActionRpCopyToCpu
#define   SB_BRCM_FIELD_ACTION_INGSAMPLEENABLE  bcmFieldActionIngSampleEnable
#define   SB_BRCM_FIELD_ACTION_RP_DROP                bcmFieldActionRpDrop
#define   SB_BRCM_FIELD_ACTION_EGRESS_CLASS_SELECT    bcmFieldActionEgressClassSelect
#define   SB_BRCM_FIELD_ACTION_NEW_CLASS_ID           bcmFieldActionNewClassId
#define   SB_BRCM_FIELD_ACTION_DROP      bcmFieldActionDrop
#define   SB_BRCM_FIELD_ACTION_SNOOP      bcmFieldActionSnoop
#define   SB_BRCM_FIELD_ACTION_STAT0      bcmFieldActionStat0
#define   SB_BRCM_FIELD_ACTION_USE_POLICER_RESULT      bcmFieldActionUsePolicerResult
#define   SB_BRCM_FIELD_ACTION_POLICER_LEVEL0      bcmFieldActionPolicerLevel0
#define   SB_BRCM_FIELD_ACTION_CLASS_DEST_SET      bcmFieldActionClassDestSet
#define   SB_BRCM_FIELD_ACTION_CLASS_SOURCE_SET      bcmFieldActionClassSourceSet

#define   SB_BRCM_FIELD_STATS_PACKETS   bcmFieldStatPackets
#define   SB_BRCM_FIELD_STAGE_INGRESS   bcmFieldStageIngress
#define   SB_BRCM_QUALIFY_INPORTS       bcmFieldQualifyInPorts
#define   SB_BRCM_QUALIFY_DSTPORT       bcmFieldQualifyDstPort
#define   SB_BRCM_QUALIFY_STAGE_INGRESS  bcmFieldQualifyStageIngress
#define   SB_BRCM_QUALIFY_SRC_IP         bcmFieldQualifySrcIp
#define   SB_BRCM_QUALIFY_DST_IP         bcmFieldQualifyDstIp
#define   SB_BRCM_QUALIFY_IP_PROTO       bcmFieldQualifyIpProtocol
#define   SB_BRCM_QUALIFY_L4_SRC_PORT    bcmFieldQualifyL4SrcPort
#define   SB_BRCM_QUALIFY_L4_DST_PORT    bcmFieldQualifyL4DstPort
#define   SB_BRCM_FIELD_QUALIFY_DST_CLASS_FIELD_ENUM       bcmFieldQualifyDstClassField
#define   SB_BRCM_QSET_FIELD_QUALIFY_STAGE_EGRESS     bcmFieldQualifyStageEgress
#define   SB_BRCM_QSET_FIELD_QUALIFY_OUT_PORT         bcmFieldQualifyOutPort
#define   SB_BRCM_QSET_FIELD_QUALIFY_INGRESS_CLASS_FIELD  bcmFieldQualifyIngressClassField
#define   SB_BRCM_FIELD_QUALIFY_INTERFACE_CLASS_PROCESSING_PORT_ENUM  bcmFieldQualifyInterfaceClassProcessingPort
#define   SB_BRCM_FIELD_QUALIFY_COLOR_ENUM    bcmFieldQualifyColor
#define   SB_BRCM_FIELD_QUALIFY_PRESEL        BCM_FIELD_QUALIFY_PRESEL
#define   SB_BRCM_FIELD_COLOR_GREEN           BCM_FIELD_COLOR_GREEN
#define   SB_BRCM_PORT_CLASS_FIELD_EGRESS_PACKET_PROCESSING      bcmPortClassFieldEgressPacketProcessing

#define   SB_BRCM_SAMPLE_FLEX_SAMPLED_PKTS    snmpBcmSampleFlexSampledPkts
#define   SB_BRCM_SAMPLE_FLEX_SNAPSHOT_PKTS   snmpBcmSampleFlexSnapshotPkts
#define   SB_BRCM_SAMPLE_FLEX_PKTS            snmpBcmSampleFlexPkts
#define   SB_BRCM_MIRROR_PORT_INGRESS         BCM_MIRROR_PORT_INGRESS
#define   SB_BRCM_MIRROR_PORT_SFLOW           BCM_MIRROR_PORT_SFLOW
#define   SB_BRCM_MIRROR_DEST_TUNNEL_SFLOW    BCM_MIRROR_DEST_TUNNEL_SFLOW
#define   SB_BRCM_MIRROR_DEST_WITH_ID         BCM_MIRROR_DEST_WITH_ID
#define   SB_BRCM_RX_TRAP_USER_DEFINE         bcmRxTrapUserDefine
#define   SB_BRCM_RX_SNOOP_UPDATE_DEST        BCM_RX_SNOOP_UPDATE_DEST
#define   SB_BRCM_GPORT_LOCAL_CPU             BCM_GPORT_LOCAL_CPU
#define   SB_BRCM_RX_TRAP_REPLACE             BCM_RX_TRAP_REPLACE

#define   SB_BRCM_PORT_CONTROL_SAMPLE_FLEX_RATE  bcmPortControlSampleFlexRate
#define   SB_BRCM_PORT_CONTROL_SAMPLE_FLEX_DEST  bcmPortControlSampleFlexDest
#define   SB_BRCM_PORT_CONTROL_SAMPLE_DEST_MIRROR  BCM_PORT_CONTROL_SAMPLE_DEST_MIRROR
#define   SB_BRCM_SWITCH_SAMPLE_INGRESS_RANDOM_SEED  bcmSwitchSampleIngressRandomSeed

#define   SB_BRCM_SWITCH_DIRECTED_MIRRORING    bcmSwitchDirectedMirroring
#define   SB_BRCM_SWITCH_FLEXIBLE_MIRROR_DESTINATIONS  bcmSwitchFlexibleMirrorDestinations

#define  SB_BRCM_POLICER_CREATE(_asic,_pPolicer, _id)  \
                  bcm_policer_create(_asic,_pPolicer, _id)

#define   SB_BRCM_FIELD_ENTRY_POLICER_ATTACH(_asic,_entry,_level,_policerid) \
                  bcm_field_entry_policer_attach(_asic,_entry,_level,_policerid)
#define   SB_BRCM_FIELD_STAT_CREATE(_asic,_group,_nstat,_pStattype, _pStatId) \
                  bcm_field_stat_create(_asic,_group,_nstat,_pStattype, _pStatId)
#define   SB_BRCM_FIELD_STAT_ATTACH(_asic,_entry,_statId) \
                  bcm_field_entry_stat_attach(_asic,_entry,_statId)
#define   SB_BRCM_FIELD_STAT_DETACH(_asic,_entry,_statId) \
                  bcm_field_entry_stat_detach(_asic,_entry,_statId)
#define   SB_BRCM_FIELD_ACTION_ADD(_asic,_entry,_action,_param0,_param1) \
                  bcm_field_action_add(_asic,_entry,_action,_param0,_param1)
#define   SB_BRCM_FIELD_GROUP_INSTALL(_asic,_group) \
                  bcm_field_group_install(_asic,_group)
#define   SB_BRCM_FIELD_STAT_GET(_asic, _statId, _stat_type,_pCount) \
                  bcm_field_stat_get(_asic, _statId, _stat_type,_pCount)
#define   SB_BRCM_FIELD_STAT_DESTORY(_asic, _statId) \
                  bcm_field_stat_destroy(_asic, _statId)
#define   SB_BRCM_FIELD_GROUP_DESTORY(_asic, _group) \
                  bcm_field_group_destroy(_asic, _group)
#define   SB_BRCM_STAT_CLEAR_SINGLE(_asic, _port, _stat_type)  \
                  bcm_stat_clear_single(_asic, _port, _stat_type)
#define   SB_BRCM_FIELD_ENTRY_STAT_GET(_asic,_entry,_statId)  \
                  bcm_field_entry_stat_get(_asic,_entry,_statId)


#define  SB_BRCM_FIELD_ENTRY_REMOVE(_asic,_entry)        \
             bcm_field_entry_remove(_asic,_entry)

#define  SB_BRCM_FIELD_ENTRY_DESTROY(_asic,_entry)       \
             bcm_field_entry_destroy(_asic,_entry)

#define  SB_BRCM_FIELD_ENTRY_POLICER_GET(_asic,_entry,_level,_policer_id_ptr)   \
             bcm_field_entry_policer_get(_asic,_entry,_level,_policer_id_ptr)

#define  SB_BRCM_FIELD_ENTRY_POLICER_DETACH(_asic,_entry,_level)    \
             bcm_field_entry_policer_detach(_asic,_entry,_level)

#define  SB_BRCM_FIELD_ENTRY_PRIO_SET(_asic, _entry, _priority)  \
             bcm_field_entry_prio_set(_asic, _entry, _priority)

#define  SB_BRCM_POLICER_DESTROY(_asic,_policer_id)   \
             bcm_policer_destroy(_asic,_policer_id)


#define  SB_BRCM_FIELD_QUALIFY_OUTPORT(_asic, _entry, _port, _mask) \
                        bcm_field_qualify_OutPort(_asic, _entry, _port, _mask)

#define  SB_BRCM_FIELD_QUALIFY_INGRESSCLASSFIELD(_asic, _entry, _class_id, _mask) \
                bcm_field_qualify_IngressClassField(_asic, _entry, _class_id, _mask)

#define   SB_BRCM_FIELD_QUALIFY_INTERFACE_CLASS_PROCESSING_PORT(_asic, _entry, _port_class, _mask)  \
                 bcm_field_qualify_InterfaceClassProcessingPort(_asic, _entry, _port_class, _mask)

#define  SB_BRCM_STAT_GET32(_asic, _port, _stat_type, _pCount) \
                bcm_stat_get32(_asic, _port, _stat_type, _pCount)

#define  SB_BRCM_MIRROR_INIT(_asic)  \
                bcm_mirror_init(_asic)

#define  SB_BRCM_GPORT_MIRROR_SET(_gport, _value) \
                BCM_GPORT_MIRROR_SET(_gport, _value)

#define  SB_BRCM_MIRROR_DESTINATION_INIT(_mirror_dest)  \
                bcm_mirror_destination_t_init(_mirror_dest)

#define  SB_BRCM_MIRROR_PORT_DESTINATION_CREATE(_asic, _mirror_dest)  \
                bcm_mirror_destination_create(_asic,  _mirror_dest)

#define  SB_BRCM_MIRROR_PORT_DEST_ADD(_asic, _port, _flag, _mirror_dest)  \
                bcm_mirror_port_dest_add(_asic, _port, _flag, _mirror_dest)

#define  SB_BRCM_MIRROR_DESTINATION_GET(_asic, _port, _mirror_dest)  \
                bcm_mirror_destination_get(_asic, _port, _mirror_dest)

#define  SB_BRCM_MIRROR_PORT_DEST_DELETE(_asic, _port, _flag, _mirror_dest)  \
                bcm_mirror_port_dest_delete(_asic, _port, _flag, _mirror_dest)

#define  SB_BRCM_MIRROR_PORT_DESTINATION_DESTROY(_asic, _port)  \
                bcm_mirror_destination_destroy(_asic, _port)

#define   SB_BRCM_RX_SNOOP_CONFIG_INIT(_snoop_config)  \
                  bcm_rx_snoop_config_t_init(_snoop_config)

#define   SB_BRCM_RX_SNOOP_CREATE(_asic, _flag, _snoop_cmd)  \
                  bcm_rx_snoop_create(_asic, _flag, _snoop_cmd)

#define   SB_BRCM_RX_SNOOP_SET(_asic,  _snoop_cmd, _snoop_config)  \
                  bcm_rx_snoop_set(_asic, _snoop_cmd, _snoop_config)

#define   SB_BRCM_RX_SNOOP_DESTROY(_asic, _flag, _snoop_cmd)  \
                  bcm_rx_snoop_destroy(_asic, _flag, _snoop_cmd)

#define   SB_BRCM_RX_TRAP_CONFIG_INIT(_trap_config)  \
                  bcm_rx_trap_config_t_init(_trap_config)

#define   SB_BRCM_RX_TRAP_TYPE_CREATE(_asic, _flag, _trap_type, _trap_id)  \
                  bcm_rx_trap_type_create(_asic, _flag, _trap_type, _trap_id)

#define   SB_BRCM_RX_TRAP_SET(_asic, _trap_id, _trap_config)  \
                  bcm_rx_trap_set(_asic,_trap_id, _trap_config)

#define   SB_BRCM_RX_TRAP_TYPE_DESTROY(_asic, _trap_id)  \
                  bcm_rx_trap_type_destroy(_asic, _trap_id)

#define   SB_BRCM_GPORT_TRAP_SET(_gport, _trap_id, _trap_strength, _snoop_strength)  \
                  BCM_GPORT_TRAP_SET(_gport, _trap_id, _trap_strength, _snoop_strength)

#define   SB_BRCM_FIELD_PRESEL_INIT(_presel)  \
                  BCM_FIELD_PRESEL_INIT(_presel)

#define   SB_BRCM_FIELD_PRESEL_CREATE(_asic, _preselId)  \
                  bcm_field_presel_create(_asic, _preselId)

#define   SB_BRCM_FIELD_PRESEL_ADD(_presel, _preselId)  \
                  BCM_FIELD_PRESEL_ADD(_presel, _preselId)

#define   SB_BRCM_FIELD_GROUP_PRESEL_SET(_asic, _group, _presel)  \
                  bcm_field_group_presel_set(_asic, _group, _presel)

#define   SB_BRCM_FIELD_ASET_INIT(_aset)  \
                  BCM_FIELD_ASET_INIT(_aset)

#define   SB_BRCM_FIELD_ASET_ADD(_aset, _type)  \
                  BCM_FIELD_ASET_ADD(_aset, _type)

#define   SB_BRCM_FIELD_GROUP_ACTION_SET(_asic, _group, _aset)  \
                  bcm_field_group_action_set(_asic, _group, _aset)

#define   SB_BRCM_PORT_CLASS_SET(_asic, _port, _port_class, _class_id)  \
                  bcm_port_class_set(_asic, _port, _port_class, _class_id)

#define   SB_BRCM_FIELD_QUALIFY_COLOR(_asic, _entry, _color)  \
                  bcm_field_qualify_Color(_asic, _entry, _color)

#define   SB_BRCM_FIELD_QUALIFY_DST_CLASS_FIELD(_asic, _entry, _data, _mask)  \
                  bcm_field_qualify_DstClassField(_asic, _entry, _data, _mask)

#define   SB_BRCM_FIELD_QUALIFY_SRC_CLASS_FIELD(_asic, _entry, _data, _mask)  \
                  bcm_field_qualify_SrcClassField(_asic, _entry, _data, _mask)

#define   SB_BRCM_PORT_CONTROL_SET(_asic, _port, _type, _value)  \
                  bcm_port_control_set(_asic, _port, _type, _value)


#define  SB_BRCM_COSQ_GPORT_TRAVERSE(_aisc, _cb, _user_data_ptr)   \
                 bcm_cosq_gport_traverse(_aisc, _cb, _user_data_ptr)

#define  SB_BRCM_COSQ_STAT_GET(_asic, _port, _cosq, _stat, _val_ptr) \
                 bcm_cosq_stat_get(_asic, _port, _cosq, _stat, _val_ptr)

#ifdef __cplusplus
}
#endif
#endif
