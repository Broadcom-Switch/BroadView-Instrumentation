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

#ifndef INCLUDE_BSTAPP_MAP_H
#define INCLUDE_BSTAPP_MAP_H

#ifdef __cplusplus
extern "C"
{
#endif

#define BSTAPP_DEBUG_PRINT   printf

#define BSTAPP_ASIC_MAX_PORTS            130
#define BSTAPP_ASIC_MAX_UC_QUEUES        4096
#define BSTAPP_ASIC_MAX_UC_QUEUE_GROUPS  128
#define BSTAPP_ASIC_MAX_MC_QUEUES        1040
#define BSTAPP_ASIC_MAX_SERVICE_POOLS    4
#define BSTAPP_ASIC_MAX_COMMON_POOLS     1
#define BSTAPP_ASIC_MAX_CPU_QUEUES       8
#define BSTAPP_ASIC_MAX_RQE_QUEUES       11
#define BSTAPP_ASIC_MAX_RQE_QUEUE_POOLS  4
#define BSTAPP_ASIC_MAX_PRIORITY_GROUPS  8

#define BSTAPP_ASIC_MAX_INGRESS_SERVICE_POOLS  \
      (BSTAPP_ASIC_MAX_SERVICE_POOLS + BSTAPP_ASIC_MAX_COMMON_POOLS)


#define BSTAPP_BASIC_STAT_SIZE            sizeof(uint64_t)


#define BSTAPP_STAT_ID_DEVICE                    0
#define BSTAPP_STAT_ID_EGR_POOL                  1 
#define BSTAPP_STAT_ID_EGR_MCAST_POOL            2 
#define BSTAPP_STAT_ID_ING_POOL                  3
#define BSTAPP_STAT_ID_PORT_POOL                 4
#define BSTAPP_STAT_ID_PRI_GROUP_SHARED          5
#define BSTAPP_STAT_ID_PRI_GROUP_HEADROOM        6
#define BSTAPP_STAT_ID_UCAST                     7
#define BSTAPP_STAT_ID_MCAST                     8
#define BSTAPP_STAT_ID_MCAST_QUEUE_ENTRIES       9
#define BSTAPP_STAT_ID_EGR_UCAST_PORT_SHARED     10
#define BSTAPP_STAT_ID_EGR_PORT_SHARED           11
#define BSTAPP_STAT_ID_RQE_QUEUE                 12
#define BSTAPP_STAT_ID_RQE_POOL                  13
#define BSTAPP_STAT_ID_UCAST_GROUP               14
#define BSTAPP_STAT_ID_CPU_QUEUE                 15
#define BSTAPP_STAT_ID_EGR_MCAST_SHARE_QUEUE     16
#define BSTAPP_STAT_ID_EGR_PORT_MCAST_SHARED     17
#define BSTAPP_STAT_ID_EGR_PORT_MCAST_SHARED_QUEUE     18
#define BSTAPP_STAT_ID_MAX_COUNT                 19


#define BSTAPP_DEVICE_ROWS                   (1)
#define BSTAPP_DEVICE_COLUMNS                (1) 
#define BSTAPP_DEVICE_DB_SIZE                (1)

#define BSTAPP_EGR_POOL_ROWS                 (1)
#define BSTAPP_EGR_POOL_COLUMNS              (BSTAPP_ASIC_MAX_SERVICE_POOLS)
#define BSTAPP_E_SP_UM_SHARE_STAT_SIZE           (BSTAPP_ASIC_MAX_SERVICE_POOLS)

#define BSTAPP_EGR_MCAST_POOL_ROWS           (1)
#define BSTAPP_EGR_MCAST_POOL_COLUMNS        (BSTAPP_ASIC_MAX_SERVICE_POOLS)
#define BSTAPP_E_SP_MC_SHARE_STAT_SIZE           (BSTAPP_ASIC_MAX_SERVICE_POOLS)

#define BSTAPP_ING_POOL_ROWS                 (1)
#define BSTAPP_ING_POOL_COLUMNS              (BSTAPP_ASIC_MAX_INGRESS_SERVICE_POOLS)
#define BSTAPP_I_SP_STAT_SIZE                    (BSTAPP_ASIC_MAX_INGRESS_SERVICE_POOLS)

#define BSTAPP_PORT_POOL_ROWS                (BSTAPP_ASIC_MAX_PORTS)
#define BSTAPP_PORT_POOL_COLUMNS             (BSTAPP_ASIC_MAX_INGRESS_SERVICE_POOLS)
#define BSTAPP_I_P_SP_STAT_SIZE                  (BSTAPP_ASIC_MAX_PORTS * \
                                                    BSTAPP_ASIC_MAX_INGRESS_SERVICE_POOLS)

#define BSTAPP_PRI_GROUP_SHARED_ROWS         (BSTAPP_ASIC_MAX_PORTS)
#define BSTAPP_PRI_GROUP_SHARED_COLUMNS      (BSTAPP_ASIC_MAX_PRIORITY_GROUPS)
#define BSTAPP_PG_SHARED_SIZE                    (BSTAPP_ASIC_MAX_PORTS * \
                                                    BSTAPP_ASIC_MAX_PRIORITY_GROUPS)

#define BSTAPP_PRI_GROUP_HEADROOM_ROWS       (BSTAPP_ASIC_MAX_PORTS)
#define BSTAPP_PRI_GROUP_HEADROOM_COLUMNS    (BSTAPP_ASIC_MAX_PRIORITY_GROUPS)
#define BSTAPP_PG_HEADROOM_SIZE                  (BSTAPP_ASIC_MAX_PORTS * \
                                                    BSTAPP_ASIC_MAX_PRIORITY_GROUPS)

#define BSTAPP_UCAST_ROWS                    (1)
#define BSTAPP_UCAST_COLUMNS                 (BSTAPP_ASIC_MAX_UC_QUEUES)
#define BSTAPP_E_UC_STAT_SIZE                    (BSTAPP_ASIC_MAX_UC_QUEUES)

#define BSTAPP_MCAST_ROWS                    (1)
#define BSTAPP_MCAST_COLUMNS                 (BSTAPP_ASIC_MAX_MC_QUEUES)
#define BSTAPP_E_MC_STAT_SIZE                    (BSTAPP_ASIC_MAX_MC_QUEUES)

#define BSTAPP_EGR_UCAST_PORT_SHARED_ROWS    (BSTAPP_ASIC_MAX_PORTS) 
#define BSTAPP_EGR_UCAST_PORT_SHARED_COLUMNS (BSTAPP_ASIC_MAX_SERVICE_POOLS)
#define BSTAPP_E_P_SP_UC_SHARE_STAT_SIZE         (BSTAPP_ASIC_MAX_PORTS * \
                                                    BSTAPP_ASIC_MAX_SERVICE_POOLS)

#define BSTAPP_EGR_PORT_SHARED_ROWS          (BSTAPP_ASIC_MAX_PORTS)
#define BSTAPP_EGR_PORT_SHARED_COLUMNS       (BSTAPP_ASIC_MAX_SERVICE_POOLS)
#define BSTAPP_E_P_SP_UM_SHARE_STAT_SIZE         (BSTAPP_ASIC_MAX_PORTS * \
                                                    BSTAPP_ASIC_MAX_SERVICE_POOLS)

#define BSTAPP_RQE_QUEUE_ROWS                (1)
#define BSTAPP_RQE_QUEUE_COLUMNS             (BSTAPP_ASIC_MAX_RQE_QUEUES)
#define BSTAPP_E_RQE_QUEUE_STAT_SIZE             (BSTAPP_ASIC_MAX_RQE_QUEUES)

#define BSTAPP_RQE_POOL_ROWS                 (1)
#define BSTAPP_RQE_POOL_COLUMNS              (BSTAPP_ASIC_MAX_RQE_QUEUES)
#define BSTAPP_E_RQE_STAT_SIZE                   (BSTAPP_ASIC_MAX_RQE_QUEUES)

#define BSTAPP_UCAST_GROUP_ROWS              (1)
#define BSTAPP_UCAST_GROUP_COLUMNS           (BSTAPP_ASIC_MAX_UC_QUEUE_GROUPS)
#define BSTAPP_E_UC_Q_GROUP_STAT_SIZE            (BSTAPP_ASIC_MAX_UC_QUEUE_GROUPS)

#define BSTAPP_CPU_QUEUE_ROWS                (1)
#define BSTAPP_CPU_QUEUE_COLUMNS             (BSTAPP_ASIC_MAX_CPU_QUEUES)
#define BSTAPP_E_CPU_STAT_SIZE                   (BSTAPP_ASIC_MAX_CPU_QUEUES)


#define BSTAPP_EGR_MCAST_SHARED_QUEUE_POOL_ROWS           (1)
#define BSTAPP_EGR_MCAST_SHARED_QUEUE_POOL_COLUMNS        (BSTAPP_ASIC_MAX_SERVICE_POOLS)
#define BSTAPP_E_SP_MC_SHARED_QUEUE_STAT_SIZE           (BSTAPP_ASIC_MAX_SERVICE_POOLS)

#define BSTAPP_EGR_PORT_MCAST_SHARED_BUFFER_ROWS           (BSTAPP_ASIC_MAX_PORTS)
#define BSTAPP_EGR_PORT_MCAST_SHARED_BUFFER_COLUMNS        (BSTAPP_ASIC_MAX_SERVICE_POOLS)
#define BSTAPP_E_PORT_SP_MC_SHARED_BUFFER_STAT_SIZE        (BSTAPP_ASIC_MAX_SERVICE_POOLS * BSTAPP_ASIC_MAX_PORTS)

#define BSTAPP_EGR_PORT_MCAST_SHARED_QUEUE_ROWS           (BSTAPP_ASIC_MAX_PORTS)
#define BSTAPP_EGR_PORT_MCAST_SHARED_QUEUE_COLUMNS        (BSTAPP_ASIC_MAX_SERVICE_POOLS)
#define BSTAPP_E_PORT_SP_MC_SHARED_QUEUE_STAT_SIZE               (BSTAPP_ASIC_MAX_SERVICE_POOLS * BSTAPP_ASIC_MAX_PORTS)

#define BSTAPP_EGR_POOL_NUM_OF_ROWS                 (1)

#define BSTAPP_EGR_POOL_NUM_OF_ROWS                 (1)
#define BSTAPP_EGR_POOL_NUM_OF_COLUMNS              (BSTAPP_ASIC_MAX_SERVICE_POOLS)
#define BSTAPP_E_SP_STAT_SIZE                   (BSTAPP_ASIC_MAX_CPU_QUEUES)

#define BSTAPP_EGR_MCAST_SHARE_QUEUE_NUM_OF_ROWS                 (1)
#define BSTAPP_EGR_MCAST_SHARE_QUEUENUM_OF_COLUMNS              (BSTAPP_ASIC_MAX_SERVICE_POOLS)

#define BSTAPP_STAT_ID_CHECK(_bid)                                   \
                   if (_bid >= BSTAPP_STAT_ID_MAX_COUNT)             \
                   {                                                       \
                     BSTAPP_DEBUG_PRINT("BID value %d is greater than"   \
                       "max supported BID %d\n",                           \
                       (_bid), BSTAPP_STAT_ID_MAX_COUNT);            \
                     return (-1);                        \
                   }

#define BSTAPP_VALID_UNIT_CHECK(_asc)                                   \
                   if (_asc >= 2)             \
                   {                                                       \
                     BSTAPP_DEBUG_PRINT("asic value %d is greater than"   \
                       "max supported \n",                           \
                       (_asc));            \
                     return (-1);                        \
                   }



#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_BSTAPP_MAP_H */
