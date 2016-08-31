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

#ifndef INCLUDE_SBPLUGIN_BST_OVSDB_H
#define INCLUDE_SBPLUGIN_BST_OVSDB_H

#include "broadview.h"
#include "sbplugin.h"
#include "sbplugin_bst_map.h"
#include "sbplugin_bst_cache.h"

/* Structure to bid information */
typedef struct _bst_ovsdb_bid_params_
{
  int      bid;                 /* bid number */
  char     *realm_name;         /* Realm name */
  char     *counter_name;       /* counter name */
  bool     is_indexed;          /* Is it asingle indexed array */
  bool     is_double_indexed;   /* BID table is double indexed/not */
  int      num_of_rows;         /* Number of rows*/
  int      num_of_columns;      /* Number of columns*/
  int      size;                /* size of database for BID */
  size_t   offset;              /* offset to BID database */
} BVIEW_BST_OVSDB_BID_PARAMS_t;


/* Get base address of Database for BID */
#define   BVIEW_OVSDB_BID_BASE_ADDR(_bid, _p)  (BVIEW_OVSDB_BID_INFO_t *)((char *) _p + \
                                                 bid_tab_params[_bid].offset);


/* Macro to acquire read lock */
#define SB_OVSDB_RWLOCK_RD_LOCK(lock)                             \
           if (pthread_rwlock_rdlock(&lock) != 0)                    \
           {                                                         \
               SB_OVSDB_DEBUG_PRINT("Failed to take "                \
                                 "read write lock for read\n" );     \
               return BVIEW_STATUS_FAILURE;                          \
           } 


/* Macro to acquire write lock */
#define SB_OVSDB_RWLOCK_WR_LOCK(lock)                             \
           if (pthread_rwlock_wrlock(&lock) != 0)                    \
           {                                                         \
               SB_OVSDB_DEBUG_PRINT("Failed to take "                \
                                 "read write lock for write\n" );    \
               return BVIEW_STATUS_FAILURE;                          \
           } 

/* Macro to release RW lock */
#define SB_OVSDB_RWLOCK_UNLOCK(lock)                              \
           if (pthread_rwlock_unlock(&lock) != 0)                    \
           {                                                         \
               SB_OVSDB_DEBUG_PRINT("Failed to release "             \
                                 "read write lock \n" );             \
           } 


/*********************************************************************
* @brief   Convert bid, port, index to name in bufmon table format
*           <realm>/<name>/<index1>/<index2>
*
* @param[in]   asic     -  asic number
* @param[in]   bid      -  bid number
* @param[in]   port     -  port number
* @param[out]  ovsdb_key  -  ovsdb bufmon table's name entry
* @param[in]   length_of_key -length of the buffer that ovsdb_key points to

*
* @retval BVIEW_STATUS_FAILURE      Failed to prepare ovsdb key from given params
* @retval BVIEW_STATUS_SUCCESS      ovsdb_key is successfully prepared
*
*
* @notes    none
*********************************************************************/
BVIEW_STATUS bst_bid_port_index_to_ovsdb_key(int asic, int bid,
                             int port, int index,
                             char *ovsdb_key, int length_of_key);

/*********************************************************************
* @brief   Get threshold from BST ovsdb DB
*
* @param[in]   asic     -  asic number   
* @param[in]   port     -  port number   
* @param[in]   index    -  Queue index   
* @param[in]   bid      -  bid number   
* @param[out]  p_threshold   -  bst threshold pointer

*
* @retval BVIEW_STATUS_FAILURE      Threshold get is failed
* @retval BVIEW_STATUS_SUCCESS      Threshold get is successfull
*
*
* @notes    none
*********************************************************************/
BVIEW_STATUS bst_ovsdb_threshold_get(int asic, int port, int index,
                                     int bid, uint64_t *p_threshold);


/*********************************************************************
* @brief   Set threshold in BST ovsdb DB
*
* @param[in]   asic     -  asic number   
* @param[in]   port     -  port number   
* @param[in]   index    -  Queue index   
* @param[in]   bid      -  bid number   
* @param[out]  threshold   -  bst threshold 

*
* @retval BVIEW_STATUS_FAILURE      Threshold set is failed
* @retval BVIEW_STATUS_SUCCESS      Threshold set is successfull
*
*
* @notes    none
*********************************************************************/
BVIEW_STATUS bst_ovsdb_threshold_set(int asic, int port, int index,
                                     int bid, uint64_t threshold);

/*********************************************************************
* @brief   Get BST Config BST ovsdb DB
*
* @param[in]   asic     -  asic number   
* @param[out]  enable   -  Pointer to BST configuration Data.

*
* @retval BVIEW_STATUS_FAILURE           Config get is failed
* @retval BVIEW_STATUS_SUCCESS           Config get is successfull
*
*
* @notes    none
*********************************************************************/
BVIEW_STATUS bst_ovsdb_bst_config_get(int asic,
                                      BVIEW_OVSDB_CONFIG_DATA_t *config);

/*********************************************************************
* @brief   Set BST Config BST ovsdb DB
*
* @param[in]   asic     -  asic number   
* @param[in]   config   -  Pointer to BST configuration Data.

*
* @retval BVIEW_STATUS_FAILURE           Config set is failed
* @retval BVIEW_STATUS_SUCCESS           Config set is successfull
*
*
* @notes    none
*********************************************************************/
BVIEW_STATUS bst_ovsdb_bst_config_set(int asic,
                                      BVIEW_OVSDB_CONFIG_DATA_t *config);


/*********************************************************************
* @brief   Resolve ovsdb cache index using bid table
*
* @param[in]   asic     -  asic number
* @param[in]   bid      -  bid number
* @param[in]   port     -  port number
* @param[in]   index    -  Queue index/Priority group Index/Service Pool
* @param[out]  db_index -  Index in bid stat/threshold array

*
* @retval BVIEW_STATUS_FAILURE      Failed to get db index from given params
* @retval BVIEW_STATUS_SUCCESS      db_index is successfully calculated
*
*
* @notes    none
*********************************************************************/
BVIEW_STATUS bst_ovsdb_resolve_index(int asic, int bid, int port,
                                     int index, int *db_index);

/*********************************************************************
* @brief   Dumps BST BID table parameter. 
*
* @retval none      
*
*
* @notes    none
*********************************************************************/
void bst_ovsdb_dump_bid_tab_params();

#endif
