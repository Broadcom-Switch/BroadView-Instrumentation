/*********************************************************************
*
* Copyright © 2016 Broadcom.  The term "Broadcom" refers
* to Broadcom Limited and/or its subsidiaries.
*
***********************************************************************
*
* @filename sbplugin_bst_map.h
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

#ifndef INCLUDE_SBPLUGIN_BST_MAP_H
#define INCLUDE_SBPLUGIN_BST_MAP_H

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
#include "sbplugin_system.h"
#include "sbplugin_system_map.h"


#define SB_BRCM_SWITCH_EVENT_MMU_BST_TRIGGER   BCM_SWITCH_EVENT_MMU_BST_TRIGGER

#define SB_BRCM_SWITCH_BST_ENABLE          bcmSwitchBstEnable

#define SB_BRCM_SWITCH_BST_TRACKING_MODE   bcmSwitchBstTrackingMode

#define SB_BRCM_BST_STAT_ID_DEVICE                    bcmBstStatIdDevice
#define SB_BRCM_BST_STAT_ID_EGR_POOL                  bcmBstStatIdEgrPool 
#define SB_BRCM_BST_STAT_ID_EGR_MCAST_POOL            bcmBstStatIdEgrMCastPool 
#define SB_BRCM_BST_STAT_ID_ING_POOL                  bcmBstStatIdIngPool
#define SB_BRCM_BST_STAT_ID_PORT_POOL                 bcmBstStatIdPortPool
#define SB_BRCM_BST_STAT_ID_PRI_GROUP_SHARED          bcmBstStatIdPriGroupShared
#define SB_BRCM_BST_STAT_ID_PRI_GROUP_HEADROOM        bcmBstStatIdPriGroupHeadroom
#define SB_BRCM_BST_STAT_ID_UCAST                     bcmBstStatIdUcast
#define SB_BRCM_BST_STAT_ID_MCAST                     bcmBstStatIdMcast
#define SB_BRCM_BST_STAT_ID_CPU                       bcmBstStatIdCpuQueue
#define SB_BRCM_BST_STAT_ID_EGR_UCAST_PORT_SHARED     bcmBstStatIdEgrUCastPortShared
#define SB_BRCM_BST_STAT_ID_EGR_PORT_SHARED           bcmBstStatIdEgrPortShared
#define SB_BRCM_BST_STAT_ID_RQE_QUEUE                 bcmBstStatIdRQEQueue 
#define SB_BRCM_BST_STAT_ID_RQE_POOL                  bcmBstStatIdRQEPool
#define SB_BRCM_BST_STAT_ID_UCAST_GROUP               bcmBstStatIdUcastGroup
#define SB_BRCM_BST_STAT_ID_MAX_COUNT                 bcmBstStatIdMaxCount



#define SB_BRCM_API_COSQ_BST_STAT_GET(unit, gport, cosq, bid, options, data_ptr)     \
            bcm_cosq_bst_stat_get(unit, gport, cosq, bid, options, (uint64 *)data_ptr)

#define SB_BRCM_COSQ_BST_PROFILE_SET(unit, gport, cosq, bid, profile_ptr)  \
            bcm_cosq_bst_profile_set(unit, gport, cosq, bid, profile_ptr)

#define SB_BRCM_COSQ_BST_PROFILE_GET(unit, gport, cosq, bid, profile_ptr)  \
            bcm_cosq_bst_profile_get(unit, gport, cosq, bid, profile_ptr)

#define SB_BRCM_COSQ_BST_STAT_CLEAR(unit, gport, cosq, bid)                \
            bcm_cosq_bst_stat_clear(unit, gport, cosq, bid)  

#define SB_BRCM_API_COSQ_BST_STAT_SYNC(unit, bid)                          \
            bcm_cosq_bst_stat_sync(unit, bid)

typedef bcm_cosq_bst_profile_t SB_BRCM_COSQ_BST_PROFILE_t;

#ifdef __cplusplus
}
#endif
#endif
