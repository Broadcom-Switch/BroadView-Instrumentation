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

#ifndef INCLUDE_BSTAPP_CACHE_H
#define INCLUDE_BSTAPP_CACHE_H


#ifdef __cplusplus
extern "C"
{
#endif


#define BSTAPP_MAX_ASICS_ON_A_PLATFORM  1
/*  BST stat and threshold info per BID*/
typedef struct _bst_bid_info_
{
  uint64_t  stat;      /* buffer usage of a particular BID */
  uint64_t  threshold; /* Threshold configured */
} BSTAPP_BID_INFO_t;


typedef struct _bst_app_stat_db_
{
  /*Device Data*/
  BSTAPP_BID_INFO_t         device;
  /* Ingress Data*/
  BSTAPP_BID_INFO_t         iPGShared[BSTAPP_PG_SHARED_SIZE];
  BSTAPP_BID_INFO_t         iPGHeadroom[BSTAPP_PG_HEADROOM_SIZE];
  BSTAPP_BID_INFO_t         iPortSP[BSTAPP_I_P_SP_STAT_SIZE];
  BSTAPP_BID_INFO_t         iSP[BSTAPP_I_SP_STAT_SIZE];
  /* Egress Data*/
  BSTAPP_BID_INFO_t         ePortSPucShare[BSTAPP_E_P_SP_UC_SHARE_STAT_SIZE];
  BSTAPP_BID_INFO_t         ePortSPumShare[BSTAPP_E_P_SP_UM_SHARE_STAT_SIZE];
  BSTAPP_BID_INFO_t         ePortSPmcShare[BSTAPP_E_P_SP_UM_SHARE_STAT_SIZE];
  BSTAPP_BID_INFO_t         eSPumShare[BSTAPP_E_SP_UM_SHARE_STAT_SIZE];
  BSTAPP_BID_INFO_t         eSPmcShare[BSTAPP_E_SP_MC_SHARE_STAT_SIZE];
  BSTAPP_BID_INFO_t         ucQ[BSTAPP_E_UC_STAT_SIZE];
  BSTAPP_BID_INFO_t         eUCqGroup[BSTAPP_E_UC_Q_GROUP_STAT_SIZE];
  BSTAPP_BID_INFO_t         mcQ[BSTAPP_E_MC_STAT_SIZE];
  BSTAPP_BID_INFO_t         mcQEntries[BSTAPP_E_MC_STAT_SIZE];
  BSTAPP_BID_INFO_t         eCPU[BSTAPP_E_CPU_STAT_SIZE];
  BSTAPP_BID_INFO_t         rqe[BSTAPP_E_RQE_STAT_SIZE];
  BSTAPP_BID_INFO_t         rqeQueueEntries[BSTAPP_E_RQE_QUEUE_STAT_SIZE];
  BSTAPP_BID_INFO_t         eSpMcShareQueue[BSTAPP_E_SP_MC_SHARED_QUEUE_STAT_SIZE];
  BSTAPP_BID_INFO_t         ePSpMcShareBuffer[BSTAPP_E_PORT_SP_MC_SHARED_BUFFER_STAT_SIZE];
  BSTAPP_BID_INFO_t         ePSpMcShareQueue[BSTAPP_E_PORT_SP_MC_SHARED_QUEUE_STAT_SIZE];
} BSTAPP_STAT_DB_t;


typedef struct _bst_data_
{ 
  /* Semaphore */
  pthread_rwlock_t  lock;
 
  /* OVSDB plugin Cache */
  BSTAPP_STAT_DB_t     cache[BSTAPP_MAX_ASICS_ON_A_PLATFORM];

} BSTAPP_DATA_t;

/*********************************************************************
* @brief   Initialise BST  cache
*
* @param[in]   @none 
* @param[out]  @none 
*
* @retval -1   Failed to initialise BST  cache
* @retval 0   Successfully initialised BST  cache
*
*
* @notes    none
*
*
*********************************************************************/
int bst_cache_init();

/*********************************************************************
* @brief   Get stat from BST  cache
*
* @param[in]   asic     -  asic number   
* @param[in]   bid      -  bid number   
* @param[in]   db_index -  Index in bid stat array   
* @param[out]  p_stat   -  bst stat pointer

*
* @retval -1      Failed to get stat from cache
* @retval 0      Successfully obtained stat from cache 
*
*
* @notes    none
*********************************************************************/
int bst_cache_stat_get(int asic, int bid, int db_index,
                                           uint64_t *p_stat);

/*********************************************************************
* @brief   Set stat in BST  cache
*
* @param[in]   asic     -  asic number   
* @param[in]   bid      -  bid number   
* @param[in]   db_index -  Index in bid stat array   
* @param[in]   stat   -  bst stat 

*
* @retval -1      Failed to set stat value in cache
* @retval 0      Successfully set stat value in cache
*
*
* @notes    none
*********************************************************************/
int bst_cache_stat_set (int asic, int bid, int db_index,
                                          uint64_t stat);

/*********************************************************************
* @brief   Dumps BST  cache. 
*          Non zero Stats and thresholds are dumped
*
* @retval -1      
* @retval 0      
*
*
* @notes    none
*********************************************************************/
int bst_cache_dump (int asic);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_BSTAPP_CACHE_H */


