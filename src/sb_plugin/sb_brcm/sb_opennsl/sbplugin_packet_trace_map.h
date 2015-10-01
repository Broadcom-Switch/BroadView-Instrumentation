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
#include "opennsl/switch.h"
#include "sbplugin_system_map.h"
#include "opennsl/l3.h"


#define SB_BRCM_API_PKT_TRACE_INFO_GET    opennsl_switch_pkt_trace_info_get

#define SB_BRCM_API_L3_EGRESS_GET            opennsl_l3_egress_get
#define SB_BRCM_API_L3_ECMP_MULTIPATH_GET    opennsl_l3_egress_multipath_get
#define SB_BRCM_API_L3_HOST_TRAVERSE         opennsl_l3_host_traverse
#define SB_BRCM_API_L3_INFO                  opennsl_l3_info
#define SB_BRCM_API_TRUNK_GET                opennsl_trunk_get
#define SB_BRCM_PORT_LOCAL_GET               opennsl_port_local_get

#define  SB_BRCM_SWITCH_PKT_TRACE_ECMP_2        OPENNSL_SWITCH_PKT_TRACE_ECMP_2
#define  SB_BRCM_SWITCH_PKT_TRACE_ECMP_1        OPENNSL_SWITCH_PKT_TRACE_ECMP_1
#define  SB_BRCM_SWITCH_PKT_TRACE_TRUNK         OPENNSL_SWITCH_PKT_TRACE_TRUNK
#define  SB_BRCM_RUNK_MAX_PORTCNT               OPENNSL_TRUNK_MAX_PORTCNT
#define  SB_BRCM_SWITCH_PKT_TRACE_FABRIC_TRUNK  OPENNSL_SWITCH_PKT_TRACE_FABRIC_TRUNK

typedef  opennsl_if_t                    SB_BRCM_IF_t;
typedef  opennsl_port_t                  SB_BRCM_PORT_t;
typedef  opennsl_switch_pkt_trace_info_t SB_BRCM_PKT_TRACE_INFO_t;
typedef  opennsl_l3_host_t               SB_BRCM_L3_HOST_t;
typedef  opennsl_l3_info_t               SB_BRCM_L3_INFO_t;
typedef  opennsl_l3_egress_t             SB_BRCM_L3_EGRESS_t;
typedef  opennsl_trunk_info_t            SB_BRCM_TRUNK_INFO_t;
typedef  opennsl_trunk_member_t          SB_BRCM_TRUNK_MEMBER_t;

#ifdef __cplusplus
}
#endif
#endif
