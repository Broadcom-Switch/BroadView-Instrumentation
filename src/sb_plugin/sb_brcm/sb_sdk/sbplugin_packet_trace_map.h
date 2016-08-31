/*********************************************************************
*
* Copyright © 2016 Broadcom.  The term "Broadcom" refers
* to Broadcom Limited and/or its subsidiaries.
*
***********************************************************************
*
* @filename sbplugin_packet_trace_map.h
*
* @purpose BroadView SDK south bound plugin API's and Macros
*
* @component South Bound Plugin
*
* @comments
*
* @create 12/16/2014
*
* @author Murali
* @end
*
**********************************************************************/

#ifndef INCLUDE_SBPLUGIN_PACKET_TRACE_MAP_H
#define INCLUDE_SBPLUGIN_PACKET_TRACE_MAP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "sbplugin.h"
#include "sbplugin_system.h"
#include "openapps_log_api.h"
#include "sbplugin.h"
#include "bcm/switch.h"
#include "sbplugin_system_map.h"
#include "bcm/l3.h"


#define SB_BRCM_API_PKT_TRACE_INFO_GET    bcm_switch_pkt_trace_info_get


#define SB_BRCM_API_L3_EGRESS_GET            bcm_l3_egress_get
#define SB_BRCM_API_L3_ECMP_MULTIPATH_GET    bcm_l3_egress_multipath_get
#define  SB_BRCM_API_TRUNK_GET   bcm_trunk_get


#define  SB_BRCM_RUNK_MAX_PORTCNT BCM_TRUNK_MAX_PORTCNT
#define  SB_BRCM_SWITCH_PKT_TRACE_ECMP_2        BCM_SWITCH_PKT_TRACE_ECMP_2
#define  SB_BRCM_SWITCH_PKT_TRACE_ECMP_1        BCM_SWITCH_PKT_TRACE_ECMP_1
#define  SB_BRCM_SWITCH_PKT_TRACE_TRUNK         BCM_SWITCH_PKT_TRACE_TRUNK
#define  SB_BRCM_SWITCH_PKT_TRACE_FABRIC_TRUNK  BCM_SWITCH_PKT_TRACE_FABRIC_TRUNK
 
typedef  bcm_switch_pkt_trace_info_t SB_BRCM_PKT_TRACE_INFO_t;
typedef  bcm_l3_egress_t   SB_BRCM_L3_EGRESS_t;
typedef  bcm_trunk_info_t   SB_BRCM_TRUNK_INFO_t;
typedef  bcm_trunk_member_t SB_BRCM_TRUNK_MEMBER_t;

#ifdef __cplusplus
}
#endif
#endif
