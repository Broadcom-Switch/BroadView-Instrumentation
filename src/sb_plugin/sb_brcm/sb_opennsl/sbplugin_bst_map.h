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

#ifndef INCLUDE_SBPLUGIN_BST_MAP_H
#define INCLUDE_SBPLUGIN_BST_MAP_H

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
#include "opennsl/stat.h"
#include "opennsl/error.h"
#include "sbplugin.h"
#include "sbplugin_system.h"
#include "sbplugin_system_map.h"


#define SB_BRCM_SWITCH_EVENT_MMU_BST_TRIGGER          OPENNSL_SWITCH_EVENT_MMU_BST_TRIGGER

#define SB_BRCM_SWITCH_BST_ENABLE                     opennslSwitchBstEnable

#define SB_BRCM_SWITCH_BST_TRACKING_MODE              opennslSwitchBstTrackingMode

#define SB_BRCM_BST_STAT_ID_DEVICE                    opennslBstStatIdDevice
#define SB_BRCM_BST_STAT_ID_EGR_POOL                  opennslBstStatIdEgrPool 
#define SB_BRCM_BST_STAT_ID_EGR_MCAST_POOL            opennslBstStatIdEgrMCastPool 
#define SB_BRCM_BST_STAT_ID_ING_POOL                  opennslBstStatIdIngPool
#define SB_BRCM_BST_STAT_ID_PORT_POOL                 opennslBstStatIdPortPool
#define SB_BRCM_BST_STAT_ID_PRI_GROUP_SHARED          opennslBstStatIdPriGroupShared
#define SB_BRCM_BST_STAT_ID_PRI_GROUP_HEADROOM        opennslBstStatIdPriGroupHeadroom
#define SB_BRCM_BST_STAT_ID_UCAST                     opennslBstStatIdUcast
#define SB_BRCM_BST_STAT_ID_MCAST                     opennslBstStatIdMcast
#define SB_BRCM_BST_STAT_ID_EGR_UCAST_PORT_SHARED     opennslBstStatIdEgrUCastPortShared
#define SB_BRCM_BST_STAT_ID_EGR_PORT_SHARED           opennslBstStatIdEgrPortShared
#define SB_BRCM_BST_STAT_ID_RQE_QUEUE                 opennslBstStatIdRQEQueue 
#define SB_BRCM_BST_STAT_ID_RQE_POOL                  opennslBstStatIdRQEPool
#define SB_BRCM_BST_STAT_ID_UCAST_GROUP               opennslBstStatIdUcastGroup
#define SB_BRCM_BST_STAT_ID_MAX_COUNT                 opennslBstStatIdMaxCount



#define SB_BRCM_API_COSQ_BST_STAT_GET(unit, gport, cosq, bid, options, data_ptr)     \
            opennsl_cosq_bst_stat_get(unit, gport, cosq, bid, options, (uint64 *)data_ptr)

#define SB_BRCM_COSQ_BST_PROFILE_SET(unit, gport, cosq, bid, profile_ptr)  \
            opennsl_cosq_bst_profile_set(unit, gport, cosq, bid, profile_ptr)

#define SB_BRCM_COSQ_BST_PROFILE_GET(unit, gport, cosq, bid, profile_ptr)  \
            opennsl_cosq_bst_profile_get(unit, gport, cosq, bid, profile_ptr)

#define SB_BRCM_COSQ_BST_STAT_CLEAR(unit, gport, cosq, bid)                \
            opennsl_cosq_bst_stat_clear(unit, gport, cosq, bid)  

#define SB_BRCM_API_COSQ_BST_STAT_SYNC(unit, bid)                          \
            opennsl_cosq_bst_stat_sync(unit, bid)

typedef opennsl_cosq_bst_profile_t SB_BRCM_COSQ_BST_PROFILE_t;

#ifdef __cplusplus
}
#endif
#endif
