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

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include "bstapp_map.h"
#include "bstapp_table.h"
#include "bstapp_cache.h"



/* BST cache for config, stats and thresholds */
static BSTAPP_DATA_t  bst_cache; 

/* BST BID table parameters */ 
extern BSTAPP_BID_PARAMS_t  bid_tab_params[BSTAPP_STAT_ID_MAX_COUNT];

/*********************************************************************
* @brief   Initialise BST OVSDB cache
*
* @param[in]   @none 
* @param[out]  @none 
*
* @retval -1   Failed to initialise BST bstapp cache
* @retval 0   Successfully initialised BST bstapp cache
*
*
* @notes    none
*
*
*********************************************************************/
int bst_cache_init()
{
  /* Initialize Read Write lock with default attributes */
  if (pthread_rwlock_init (&bst_cache.lock, NULL) != 0)
  {
     debug_msg("Failed to create RW lock with deafault attributes\n");
            
    return -1;
  }
  return 0;
}

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
int bst_cache_stat_get(int asic, int bid, 
                       int db_index, uint64_t *p_stat)
{
  BSTAPP_BID_INFO_t *p_row = NULL;
  BSTAPP_BID_INFO_t *p_base = NULL;

  /* Check for NULL pointer */
  if (NULL == p_stat)
    return -1;

  /* Check BID */
  BSTAPP_STAT_ID_CHECK(bid);

  /* Acquire read lock*/
  BSTAPP_RWLOCK_RD_LOCK(bst_cache.lock);  

    /* Get Base pointer to DB of BID*/
  p_base = BSTAPP_BID_BASE_ADDR (bid, &bst_cache.cache[asic]);
  /* Get pointer to the entry */
  p_row = p_base + db_index;

  *p_stat = p_row->stat; 

  /* Release lock */
  BSTAPP_RWLOCK_UNLOCK(bst_cache.lock);
  return 0;
}

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
int bst_cache_stat_set (int asic, int bid, 
                        int db_index, uint64_t stat)
{
  BSTAPP_BID_INFO_t *p_row = NULL;
  BSTAPP_BID_INFO_t *p_base = NULL;

  /* Check BID */
  BSTAPP_STAT_ID_CHECK(bid);

  /* Get Base pointer to DB of BID*/
  p_base = BSTAPP_BID_BASE_ADDR (bid, &bst_cache.cache[asic]);

  if (bid_tab_params[bid].size <= db_index)
  {
    printf ("Invalid db index for the bid = %d, db_index = %d\n", bid, db_index);
 
  }
  /* Get pointer to the entry */
  p_row = p_base + db_index;
  
  p_row->stat = stat;
   
  /* Release lock */
  BSTAPP_RWLOCK_UNLOCK(bst_cache.lock);

  return 0;
}

