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

#ifndef INCLUDE_SBPLUGIN_BST_MAP_H
#define INCLUDE_SBPLUGIN_BST_MAP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "sbplugin.h"
#include "sbplugin_system.h"
#include "sbplugin_system_map.h"
#include "openapps_log_api.h"
#include "sbplugin_ovsdb.h"



#define SB_OVSDB_BST_STAT_ID_DEVICE                    0
#define SB_OVSDB_BST_STAT_ID_EGR_POOL                  1 
#define SB_OVSDB_BST_STAT_ID_EGR_MCAST_POOL            2 
#define SB_OVSDB_BST_STAT_ID_ING_POOL                  3
#define SB_OVSDB_BST_STAT_ID_PORT_POOL                 4
#define SB_OVSDB_BST_STAT_ID_PRI_GROUP_SHARED          5
#define SB_OVSDB_BST_STAT_ID_PRI_GROUP_HEADROOM        6
#define SB_OVSDB_BST_STAT_ID_UCAST                     7
#define SB_OVSDB_BST_STAT_ID_MCAST                     8
#define SB_OVSDB_BST_STAT_ID_EGR_UCAST_PORT_SHARED     9
#define SB_OVSDB_BST_STAT_ID_EGR_PORT_SHARED           10
#define SB_OVSDB_BST_STAT_ID_RQE_QUEUE                 11
#define SB_OVSDB_BST_STAT_ID_RQE_POOL                  12
#define SB_OVSDB_BST_STAT_ID_UCAST_GROUP               13
#define SB_OVSDB_BST_STAT_ID_CPU_QUEUE                 14
#define SB_OVSDB_BST_STAT_ID_MAX_COUNT                 15


#define SB_OVSDB_BST_DEVICE_ROWS                   (1)
#define SB_OVSDB_BST_DEVICE_COLUMNS                (1) 
#define SB_OVSDB_BST_DEVICE_DB_SIZE                (1)

#define SB_OVSDB_BST_EGR_POOL_ROWS                 (1)
#define SB_OVSDB_BST_EGR_POOL_COLUMNS              (BVIEW_ASIC_MAX_SERVICE_POOLS)
#define SB_OVSDB_E_SP_UM_SHARE_STAT_SIZE           (BVIEW_ASIC_MAX_SERVICE_POOLS)

#define SB_OVSDB_BST_EGR_MCAST_POOL_ROWS           (1)
#define SB_OVSDB_BST_EGR_MCAST_POOL_COLUMNS        (BVIEW_ASIC_MAX_SERVICE_POOLS)
#define SB_OVSDB_E_SP_MC_SHARE_STAT_SIZE           (BVIEW_ASIC_MAX_SERVICE_POOLS)

#define SB_OVSDB_BST_ING_POOL_ROWS                 (1)
#define SB_OVSDB_BST_ING_POOL_COLUMNS              (BVIEW_ASIC_MAX_INGRESS_SERVICE_POOLS)
#define SB_OVSDB_I_SP_STAT_SIZE                    (BVIEW_ASIC_MAX_INGRESS_SERVICE_POOLS)

#define SB_OVSDB_BST_PORT_POOL_ROWS                (BVIEW_ASIC_MAX_PORTS)
#define SB_OVSDB_BST_PORT_POOL_COLUMNS             (BVIEW_ASIC_MAX_INGRESS_SERVICE_POOLS)
#define SB_OVSDB_I_P_SP_STAT_SIZE                  (BVIEW_ASIC_MAX_PORTS * \
                                                    BVIEW_ASIC_MAX_INGRESS_SERVICE_POOLS)

#define SB_OVSDB_BST_PRI_GROUP_SHARED_ROWS         (BVIEW_ASIC_MAX_PORTS)
#define SB_OVSDB_BST_PRI_GROUP_SHARED_COLUMNS      (BVIEW_ASIC_MAX_PRIORITY_GROUPS)
#define SB_OVSDB_PG_SHARED_SIZE                    (BVIEW_ASIC_MAX_PORTS * \
                                                    BVIEW_ASIC_MAX_PRIORITY_GROUPS)

#define SB_OVSDB_BST_PRI_GROUP_HEADROOM_ROWS       (BVIEW_ASIC_MAX_PORTS)
#define SB_OVSDB_BST_PRI_GROUP_HEADROOM_COLUMNS    (BVIEW_ASIC_MAX_PRIORITY_GROUPS)
#define SB_OVSDB_PG_HEADROOM_SIZE                  (BVIEW_ASIC_MAX_PORTS * \
                                                    BVIEW_ASIC_MAX_PRIORITY_GROUPS)

#define SB_OVSDB_BST_UCAST_ROWS                    (1)
#define SB_OVSDB_BST_UCAST_COLUMNS                 (BVIEW_ASIC_MAX_UC_QUEUES)
#define SB_OVSDB_E_UC_STAT_SIZE                    (BVIEW_ASIC_MAX_UC_QUEUES)

#define SB_OVSDB_BST_MCAST_ROWS                    (1)
#define SB_OVSDB_BST_MCAST_COLUMNS                 (BVIEW_ASIC_MAX_MC_QUEUES)
#define SB_OVSDB_E_MC_STAT_SIZE                    (BVIEW_ASIC_MAX_MC_QUEUES)

#define SB_OVSDB_BST_EGR_UCAST_PORT_SHARED_ROWS    (BVIEW_ASIC_MAX_PORTS) 
#define SB_OVSDB_BST_EGR_UCAST_PORT_SHARED_COLUMNS (BVIEW_ASIC_MAX_SERVICE_POOLS)
#define SB_OVSDB_E_P_SP_UC_SHARE_STAT_SIZE         (BVIEW_ASIC_MAX_PORTS * \
                                                    BVIEW_ASIC_MAX_SERVICE_POOLS)

#define SB_OVSDB_BST_EGR_PORT_SHARED_ROWS          (BVIEW_ASIC_MAX_PORTS)
#define SB_OVSDB_BST_EGR_PORT_SHARED_COLUMNS       (BVIEW_ASIC_MAX_SERVICE_POOLS)
#define SB_OVSDB_E_P_SP_UM_SHARE_STAT_SIZE         (BVIEW_ASIC_MAX_PORTS * \
                                                    BVIEW_ASIC_MAX_SERVICE_POOLS)

#define SB_OVSDB_BST_RQE_QUEUE_ROWS                (1)
#define SB_OVSDB_BST_RQE_QUEUE_COLUMNS             (BVIEW_ASIC_MAX_RQE_QUEUES)
#define SB_OVSDB_E_RQE_QUEUE_STAT_SIZE             (BVIEW_ASIC_MAX_RQE_QUEUES)

#define SB_OVSDB_BST_RQE_POOL_ROWS                 (1)
#define SB_OVSDB_BST_RQE_POOL_COLUMNS              (BVIEW_ASIC_MAX_RQE_QUEUES)
#define SB_OVSDB_E_RQE_STAT_SIZE                   (BVIEW_ASIC_MAX_RQE_QUEUES)

#define SB_OVSDB_BST_UCAST_GROUP_ROWS              (1)
#define SB_OVSDB_BST_UCAST_GROUP_COLUMNS           (BVIEW_ASIC_MAX_UC_QUEUE_GROUPS)
#define SB_OVSBD_E_UC_Q_GROUP_STAT_SIZE            (BVIEW_ASIC_MAX_UC_QUEUE_GROUPS)

#define SB_OVSDB_BST_CPU_QUEUE_ROWS                (1)
#define SB_OVSDB_BST_CPU_QUEUE_COLUMNS             (BVIEW_ASIC_MAX_CPU_QUEUES)
#define SB_OVSDB_E_CPU_STAT_SIZE                   (BVIEW_ASIC_MAX_CPU_QUEUES)

#define SB_OVSDB_BST_STAT_ID_CHECK(_bid)                                   \
                   if (_bid >= SB_OVSDB_BST_STAT_ID_MAX_COUNT)             \
                   {                                                       \
                     SB_OVSDB_DEBUG_PRINT("BID value %d is greater than"   \
                       "max supported BID %d\n",                           \
                       (_bid), SB_OVSDB_BST_STAT_ID_MAX_COUNT);            \
                     return (BVIEW_STATUS_FAILURE);                        \
                   }


#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_SBPLUGIN_BST_MAP_H */
