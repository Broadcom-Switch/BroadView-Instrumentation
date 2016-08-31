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
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include "sbplugin.h"
#include "sbplugin_ovsdb.h"
#include "sbplugin_bst_map.h"
#include "sbplugin_bst_ovsdb.h"
#include "sbplugin_bst_cache.h"


/* BST cache for config, stats and thresholds */
static BVIEW_OVSDB_BST_DATA_t  bst_ovsdb_cache; 

/* BST BID table parameters */ 
extern BVIEW_BST_OVSDB_BID_PARAMS_t  bid_tab_params[SB_OVSDB_BST_STAT_ID_MAX_COUNT];


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
BVIEW_STATUS bst_ovsdb_cache_init()
{
  /* Initialize Read Write lock with default attributes */
  if (pthread_rwlock_init (&bst_ovsdb_cache.lock, NULL) != 0)
  {
    SB_OVSDB_LOG (BVIEW_LOG_ERROR, 
	        "Failed to create RW lock with deafault attributes\n")
            
    return BVIEW_STATUS_FAILURE;
  }
  return BVIEW_STATUS_SUCCESS;
}

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
                                            BVIEW_OVSDB_CONFIG_DATA_t *config)
{

  SB_OVSDB_VALID_UNIT_CHECK(asic);
  /* Check for NULL pointer */
  SB_OVSDB_NULLPTR_CHECK(config, BVIEW_STATUS_INVALID_PARAMETER);
  /* Acquire write lock*/
  SB_OVSDB_RWLOCK_WR_LOCK(bst_ovsdb_cache.lock);  

  /* Set bst mode in cache */
  bst_ovsdb_cache.config_data.bst_enable = config->bst_enable;  
  bst_ovsdb_cache.config_data.bst_tracking_mode = config->bst_tracking_mode;
  bst_ovsdb_cache.config_data.periodic_collection = config->periodic_collection;  
  bst_ovsdb_cache.config_data.collection_interval = config->collection_interval;
  /* Release lock */
  SB_OVSDB_RWLOCK_UNLOCK(bst_ovsdb_cache.lock);
  return BVIEW_STATUS_SUCCESS;
}

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
                                            BVIEW_OVSDB_CONFIG_DATA_t *config)
{

  SB_OVSDB_VALID_UNIT_CHECK(asic);
  /* Check for NULL pointer */
  SB_OVSDB_NULLPTR_CHECK(config, BVIEW_STATUS_INVALID_PARAMETER);
  /* Acquire write lock*/
  SB_OVSDB_RWLOCK_RD_LOCK(bst_ovsdb_cache.lock);  

  /* Get mode from cache */
  config->bst_enable = bst_ovsdb_cache.config_data.bst_enable;
  config->bst_tracking_mode = bst_ovsdb_cache.config_data.bst_tracking_mode;
  config->periodic_collection  = bst_ovsdb_cache.config_data.periodic_collection;
  config->collection_interval  = bst_ovsdb_cache.config_data.collection_interval;

  /* Release lock */
  SB_OVSDB_RWLOCK_UNLOCK(bst_ovsdb_cache.lock);
  return BVIEW_STATUS_SUCCESS;
}

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
                                      BVIEW_OVSDB_BID_INFO_t **p_row)
{
  char src_string[1024] = {0};
  char delim[2] = "/";
  char *realm   = NULL;
  char *name    = NULL;
  char *index1   = NULL;
  char *index2   = NULL;
  char *final_token = NULL;
  int num_of_entries = sizeof(bid_tab_params)/sizeof(BVIEW_BST_OVSDB_BID_PARAMS_t);
  int  bid = 0;
  int index =0;
  BVIEW_OVSDB_BID_INFO_t   *p_base = NULL;

  /* Check for NULL pointer */
  SB_OVSDB_NULLPTR_CHECK(p_row, BVIEW_STATUS_INVALID_PARAMETER);

  strcpy(src_string, ovsdb_key);

  /* ovsdb_key string is of the format <realm>/<name>/<index1>/<index2> */
  /* Get realm */
  realm = strtok(src_string, delim);
  /* Get name */
  name = strtok(NULL, delim);
  /* Get  index1*/
  index1 = strtok(NULL, delim);
  /* Get  index2*/
  index2 = strtok(NULL, delim);
  if (realm == NULL || name == NULL ||
      index1 == NULL || index2 == NULL)
  {
    return BVIEW_STATUS_FAILURE;
  }

  final_token = strtok(NULL, delim);
  if (final_token != NULL)
  {
    return BVIEW_STATUS_FAILURE;
  }

  for (bid = 0; bid < num_of_entries; bid++)
  {
    if (strcmp(bid_tab_params[bid].realm_name, realm) == 0)
    {
      if (strcmp(bid_tab_params[bid].counter_name, name) == 0)
      {
        break;
      }
    }
  }

  /* Not able to find out correct bid entry for realm/name combination */
  if (bid == num_of_entries)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* If it is double indexed then port is @first index */
  if (bid_tab_params[bid].is_double_indexed == true)
  {
    index = ((atoi(index1) -1) * bid_tab_params[bid].num_of_columns) + atoi(index2);
  }
  else
  {
    /* Device */
    if (bid_tab_params[bid].is_indexed == false)
    {
      index = 0;
    }
    else
    {
      index = atoi(index1);
    }
  }

  /* Validate the index */
  if (index >= bid_tab_params[bid].size)
  {
    return BVIEW_STATUS_FAILURE;
  }

  p_base = BVIEW_OVSDB_BID_BASE_ADDR (bid, &bst_ovsdb_cache.cache[asic]);
  /* Get pointer to the entry */
  *p_row =  p_base + index;

  return BVIEW_STATUS_SUCCESS;
}

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
                                      BVIEW_OVSDB_BID_INFO_t *p_row)
{
  BVIEW_OVSDB_BID_INFO_t   *p_db_row = NULL;

  SB_OVSDB_NULLPTR_CHECK (p_row, BVIEW_STATUS_INVALID_PARAMETER);

  /* Get exact row of ovsdb_key*/
  if (BVIEW_STATUS_SUCCESS !=  bst_ovsdb_cache_row_get (asic, 
                                                        ovsdb_key,
                                                        &p_db_row))
  {
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire write lock*/
  SB_OVSDB_RWLOCK_WR_LOCK(bst_ovsdb_cache.lock);

  /* Update the cache*/
  if (p_db_row)
  {
    p_db_row->stat      = p_row->stat;
    p_db_row->threshold = p_row->threshold;
  }
  /* Release lock */
  SB_OVSDB_RWLOCK_UNLOCK(bst_ovsdb_cache.lock);

  return BVIEW_STATUS_SUCCESS;
}  
 
/*********************************************************************
* @brief   Get the pointer to BST data
*
* @retval  Pointer to BVIEW_OVSDB_BST_CACHE_t
*
*
* @notes    none
*********************************************************************/
BVIEW_OVSDB_BST_DATA_t  *bst_ovsdb_cache_get ()
{
  return (BVIEW_OVSDB_BST_DATA_t *) &bst_ovsdb_cache;
}

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
BVIEW_STATUS bst_ovsdb_cache_dump(int asic)
{
 
  SB_OVSDB_VALID_UNIT_CHECK(asic);
  
  /* Acquire read lock*/
  SB_OVSDB_RWLOCK_RD_LOCK(bst_ovsdb_cache.lock);  

  printf ("-----------BST OVSDB CACHE DUMP--------------\n\n");
 
  printf ("BST enable = %s\n", 
            (bst_ovsdb_cache.config_data.bst_enable? "true": "false"));
  printf ("BST tracking mode = %d\n", bst_ovsdb_cache.config_data.bst_tracking_mode);
 
  
  /* Release lock */
  SB_OVSDB_RWLOCK_UNLOCK(bst_ovsdb_cache.lock);
   return BVIEW_STATUS_SUCCESS;
}
