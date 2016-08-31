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

#ifndef INCLUDE_SBPLUGIN_BST_CACHE_H
#define INCLUDE_SBPLUGIN_BST_CACHE_H


#ifdef __cplusplus
extern "C"
{
#endif

#include "sbplugin.h"
#include "sbplugin_ovsdb.h"
#include "sbplugin_bst_map.h"
#include "sbplugin_bst_ovsdb.h"

/* OVSDB BST stat and threshold info per BID*/
typedef struct _bst_ovsdb_bid_info_
{
  uint64_t  stat;      /* buffer usage of a particular BID */
  uint64_t  threshold; /* Threshold configured */
} BVIEW_OVSDB_BID_INFO_t;

typedef struct _bst_ovsdb_stat_db_
{
  /*Device Data*/
  BVIEW_OVSDB_BID_INFO_t         device;
  /* Ingress Data*/
  BVIEW_OVSDB_BID_INFO_t         iPGShared[SB_OVSDB_PG_SHARED_SIZE];
  BVIEW_OVSDB_BID_INFO_t         iPGHeadroom[SB_OVSDB_PG_HEADROOM_SIZE];
  BVIEW_OVSDB_BID_INFO_t         iPortSP[SB_OVSDB_I_P_SP_STAT_SIZE];
  BVIEW_OVSDB_BID_INFO_t         iSP[SB_OVSDB_I_SP_STAT_SIZE];
  /* Egress Data*/
  BVIEW_OVSDB_BID_INFO_t         ePortSPucShare[SB_OVSDB_E_P_SP_UC_SHARE_STAT_SIZE];
  BVIEW_OVSDB_BID_INFO_t         ePortSPumShare[SB_OVSDB_E_P_SP_UM_SHARE_STAT_SIZE];
  BVIEW_OVSDB_BID_INFO_t         ePortSPmcShare[SB_OVSDB_E_P_SP_UM_SHARE_STAT_SIZE];
  BVIEW_OVSDB_BID_INFO_t         eSPumShare[SB_OVSDB_E_SP_UM_SHARE_STAT_SIZE];
  BVIEW_OVSDB_BID_INFO_t         eSPmcShare[SB_OVSDB_E_SP_MC_SHARE_STAT_SIZE];
  BVIEW_OVSDB_BID_INFO_t         ucQ[SB_OVSDB_E_UC_STAT_SIZE];
  BVIEW_OVSDB_BID_INFO_t         eUCqGroup[SB_OVSBD_E_UC_Q_GROUP_STAT_SIZE];
  BVIEW_OVSDB_BID_INFO_t         mcQ[SB_OVSDB_E_MC_STAT_SIZE];
  BVIEW_OVSDB_BID_INFO_t         eCPU[SB_OVSDB_E_CPU_STAT_SIZE];
  BVIEW_OVSDB_BID_INFO_t         rqe[SB_OVSDB_E_RQE_STAT_SIZE];
  BVIEW_OVSDB_BID_INFO_t         rqeQueueEntries[SB_OVSDB_E_RQE_QUEUE_STAT_SIZE];
} BVIEW_OVSDB_BST_STAT_DB_t;

/* BST Config cache of OVSDB */
typedef struct _bst_ovsdb_config_data_
{
  bool bst_enable;          /* BST enable data */
  int  bst_tracking_mode;   /* BST tracking mode */
  bool periodic_collection; /* Periodic collections of stats */
  int  collection_interval;  /* Periodic collection interval */
} BVIEW_OVSDB_CONFIG_DATA_t;


typedef struct _bst_ovsdb_data_
{ 
  /* Semaphore */
  pthread_rwlock_t  lock;
  /* BST Configuration data*/
  BVIEW_OVSDB_CONFIG_DATA_t     config_data;  
  /* OVSDB plugin Cache */
  BVIEW_OVSDB_BST_STAT_DB_t     cache[BVIEW_MAX_ASICS_ON_A_PLATFORM];

} BVIEW_OVSDB_BST_DATA_t;

/*********************************************************************
* @brief   Initialise BST OVSDB cache
*
* @param[in]   @none 
* @param[out]  @none 
*
* @retval BVIEW_STATUS_FAILURE   Failed to initialise BST ovsdb cache
* @retval BVIEW_STATUS_SUCCESS   Successfully initialised BST ovsdb cache
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS bst_ovsdb_cache_init();

/*********************************************************************
* @brief   Set BST configuration in ovsdb cache
*
* @param[in]   asic     -  asic number   
* @param[in]   config   -  Pointer to BST configuration.

* @param[out]  @none 
*
* @retval BVIEW_STATUS_FAILURE  Failed to set bst configuration in ovsdb cache         
* @retval BVIEW_STATUS_SUCCESS  Successfully set bst cofiguration in cache
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS bst_ovsdb_cache_bst_config_set(int asic,
                                            BVIEW_OVSDB_CONFIG_DATA_t *config);

/*********************************************************************
* @brief   Get BST configuration from ovsdb cache
*
* @param[in]   asic     -  asic number.   
* @param[out]  config   -  Pointer to BST configuration data.
*
* @retval BVIEW_STATUS_INVALID_PARAMETER  Invalid input parameter(s)         
* @retval BVIEW_STATUS_SUCCESS            Successfully obtained bst 
*                                         cofiguration from cache
*
*
* @notes    none
*********************************************************************/
BVIEW_STATUS bst_ovsdb_cache_bst_config_get(int asic,
                                            BVIEW_OVSDB_CONFIG_DATA_t *config);

/*********************************************************************
* @brief   Get the pointer to BST data
*
* @retval  Pointer to BVIEW_OVSDB_BST_CACHE_t
*
*
* @notes    none
*********************************************************************/
BVIEW_OVSDB_BST_DATA_t  *bst_ovsdb_cache_get ();

/*********************************************************************
* @brief    Get row from ovsdb-key  <realm>/<name>/<index1>/<index2>
*
*
* @param[in]   asic      -  asic number
* @param[in]   ovsdb_key -  ovsdb bufmon table's name/key entry
* @param[out]  p_row     -  Pointer to the row

*
* @retval BVIEW_STATUS_FAILURE      Failed to get row from ovsdb key
* @retval BVIEW_STATUS_SUCCESS
*
*
*
* @notes    none
*********************************************************************/
BVIEW_STATUS bst_ovsdb_cache_row_get (int asic, char *ovsdb_key,
                                      BVIEW_OVSDB_BID_INFO_t **p_row);

/*********************************************************************
* @brief    Update the stat/threshold of row with key 'ovsdb_key'. 
*           
*
* @param[in]   asic      -  asic number   
* @param[in]   ovsdb_key -  ovsdb bufmon table's name/key entry     
* @param[out]  p_row     -  Pointer to the row    
*
* @retval BVIEW_STATUS_FAILURE      Failed to get the row for ovsdb_key.
* @retval BVIEW_STATUS_SUCCESS      updated cache successfully. 
*                                   
*
*
* @notes    none
*********************************************************************/
BVIEW_STATUS    bst_ovsdb_row_update (int asic, char *ovsdb_key,
                                      BVIEW_OVSDB_BID_INFO_t *prow);

/*********************************************************************
* @brief   Dumps BST ovsdb cache. 
*          Non zero Stats and thresholds are dumped
*
* @retval BVIEW_STATUS_FAILURE      
* @retval BVIEW_STATUS_SUCCESS      
*
*
* @notes    none
*********************************************************************/
BVIEW_STATUS bst_ovsdb_cache_dump (int asic);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SBPLUGIN_BST_CACHE_H */


