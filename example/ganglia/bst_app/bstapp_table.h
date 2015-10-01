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

#ifndef INCLUDE_BSTAPP_TABLE_H
#define INCLUDE_BSTAPP_TABLE_H

#include "bstapp_map.h"

/* Structure to bid information */
typedef struct _bst_bid_params_
{
  int   bid;                 /* bid number */
  char  *realm_name;         /* Realm name */
  char  *counter_name;       /* counter name */
  bool  is_indexed;          /* Is it asingle indexed array */
  bool  is_double_indexed;   /* BID table is double indexed/not */
  int   num_of_rows;         /* Number of rows in the table */
  int   num_of_columns;      /* Number of columns */
  int      size;                /* size of database for BID */
  size_t   offset;              /* offset to BID database */
} BSTAPP_BID_PARAMS_t;


#define   BSTAPP_BID_BASE_ADDR(_bid, _p)  (BSTAPP_BID_INFO_t *)((char *) _p + \
                                                 bid_tab_params[_bid].offset);



/* Macro to acquire read lock */
#define BSTAPP_RWLOCK_RD_LOCK(lock)                             \
           if (pthread_rwlock_rdlock(&lock) != 0)                    \
           {                                                         \
               BSTAPP_DEBUG_PRINT("Failed to take "                \
                                 "read write lock for read\n" );     \
               return -1;                          \
           } 


/* Macro to acquire write lock */
#define BSTAPP_RWLOCK_WR_LOCK(lock)                             \
           if (pthread_rwlock_wrlock(&lock) != 0)                    \
           {                                                         \
               BSTAPP_DEBUG_PRINT("Failed to take "                \
                                 "read write lock for write\n" );    \
               return -1;                          \
           } 

/* Macro to release RW lock */
#define BSTAPP_RWLOCK_UNLOCK(lock)                              \
           if (pthread_rwlock_unlock(&lock) != 0)                    \
           {                                                         \
               BSTAPP_DEBUG_PRINT("Failed to release "             \
                                 "read write lock \n" );             \
           } 


/*********************************************************************
* @brief   Convert bid, port, index to name in bufmon table format
*           <realm>/<name>/<index1>/<index2> 
*
* @param[in]   asic     -  asic number   
* @param[in]   bid      -  bid number   
* @param[in]   port     -  port number   
* @param[out]  key  -   bufmon table's name entry     
* @param[in]   length_ofkey -length of the buffer that _key points to   

*
* @retval -1      Failed to prepare  key from given params 
* @retval 0      key is successfully prepared 
*
*
* @notes    none
*********************************************************************/
int bst_bid_port_index_tokey(int asic, int bid,
                             int port, int index, 
                             char *key, int length_of_key);

/*********************************************************************
* @brief   Convert <realm>/<name>/<index1>/<index2> formatted to key to
*           port, bid, index etc 
*
* @param[in]   asic     -  asic number   
* @param[in]  key  -   bufmon table's name/key entry     
* @param[out]   bid      -  bid number   
* @param[out]   port     -  port number   
* @param[out]  index  -  Queue index    

*
* @retval -1      Failed to prepare given params from  key  
* @retval 0      bid, port and index are are successfully prepared 
*                                    from key
*
*
* @notes    none
*********************************************************************/
int bstkey_to_bid_port_index(int asic, char *_key, int *bid,
                                             int *port, int *index);



/*********************************************************************
* @brief   Get stat from BST  DB
*
* @param[in]   asic     -  asic number   
* @param[in]   port     -  port number   
* @param[in]   index    -  Queue index   
* @param[in]   bid      -  bid number   
* @param[in]   options  -     
* @param[out]  p_stat   -  bst stat pointer

*
* @retval -1      Stat get is failed
* @retval 0      Stat get is successfull
*
*
* @notes    none
*********************************************************************/
int bst_stat_get(int asic, int port, int index,
                                int bid, int options, 
                                uint64_t *p_stat);

/*********************************************************************
* @brief   Set stat in BST  DB
*
* @param[in]   asic     -  asic number   
* @param[in]   port     -  port number   
* @param[in]   index    -  Queue index   
* @param[in]   bid      -  bid number   
* @param[in]   options  -     
* @param[in]  stat   -  bst stat 

*
* @retval -1      Stat set is failed
* @retval 0      Stat set is successfull
*
*
* @notes    none
*********************************************************************/
int bst_stat_set (int asic, int port, int index,
                                 int bid, int options, 
                                 uint64_t stat);


/*********************************************************************
* @brief   Set stat in BST  DB based on key
*
* @param[in]   asic     -  asic number   
* @param[in]   key - name in bufmon table row   
* @param[in]  p_stat   -  bst stat pointer

*
* @retval -1      Stat set is failed
* @retval 0      Stat set is successfull
*
*
* @notes    none
*********************************************************************/
int bst_stat_set_usingkey (int asic, char *key,
                                                 uint64_t stat);

/*********************************************************************
* @brief   Dumps BST BID table parameter. 
*
* @retval none      
*
*
* @notes    none
*********************************************************************/
void bst_dump_bid_tab_params();

#endif
