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
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <pthread.h>
#include "bstapp_table.h"
#include "bstapp_cache.h"

char *stat_prefix = "bst_";
/* Table to hold the different parameters of BID */ 
BSTAPP_BID_PARAMS_t  bid_tab_params[BSTAPP_STAT_ID_MAX_COUNT] = 
                              {
                                {
                                  .bid = BSTAPP_STAT_ID_DEVICE, 
                                  .realm_name = "device",
                                  .counter_name  = "data",
                                  .is_indexed = false,
                                  .is_double_indexed = false,
                                  .num_of_rows = BSTAPP_DEVICE_ROWS,
                                  .num_of_columns = BSTAPP_DEVICE_COLUMNS,
                                  .size = BSTAPP_DEVICE_DB_SIZE,
                                  .offset = offsetof (BSTAPP_STAT_DB_t, device)
                                },
                                {
                                  .bid = BSTAPP_STAT_ID_EGR_POOL, 
                                  .realm_name = "egress-service-pool",
                                  .counter_name  = "um-share-buffer-count",
                                  .is_indexed = true,
                                  .is_double_indexed = false,
                                  .num_of_rows = BSTAPP_EGR_POOL_ROWS,
                                  .num_of_columns = BSTAPP_EGR_POOL_COLUMNS,
                                  .size = BSTAPP_E_SP_UM_SHARE_STAT_SIZE, 
                                  .offset = offsetof (BSTAPP_STAT_DB_t, eSPumShare)
                                },
                                {
                                  .bid = BSTAPP_STAT_ID_EGR_MCAST_POOL,
                                  .realm_name = "egress-service-pool",
                                  .counter_name  = "mc-share-buffer-count",
                                  .is_indexed = true,
                                  .is_double_indexed = false,
                                  .num_of_rows = BSTAPP_EGR_MCAST_POOL_ROWS,
                                  .num_of_columns = BSTAPP_EGR_MCAST_POOL_COLUMNS,

                                  .size = BSTAPP_E_SP_MC_SHARE_STAT_SIZE,
                                  .offset = offsetof (BSTAPP_STAT_DB_t, eSPmcShare)
                                },
                                {
                                  .bid = BSTAPP_STAT_ID_ING_POOL, 
                                  .realm_name = "ingress-service-pool",
                                  .counter_name  = "um-share-buffer-count",
                                  .is_indexed = true,
                                  .is_double_indexed = false,
                                  .num_of_rows = BSTAPP_ING_POOL_ROWS,
                                  .num_of_columns = BSTAPP_ING_POOL_COLUMNS,
                                  .size = BSTAPP_I_SP_STAT_SIZE,
                                  .offset = offsetof (BSTAPP_STAT_DB_t, iSP)
                                },
                                {
                                  .bid = BSTAPP_STAT_ID_PORT_POOL,
                                  .realm_name = "ingress-port-service-pool",
                                  .counter_name  = "um-share-buffer-count",
                                  .is_indexed = true,
                                  .is_double_indexed = true,
                                  .num_of_rows = BSTAPP_PORT_POOL_ROWS,
                                  .num_of_columns = BSTAPP_PORT_POOL_COLUMNS,
                                  .size = BSTAPP_I_P_SP_STAT_SIZE,
                                  .offset = offsetof (BSTAPP_STAT_DB_t, iPortSP)
                                },
                                {
                                  .bid = BSTAPP_STAT_ID_PRI_GROUP_SHARED,
                                  .realm_name = "ingress-port-priority-group",
                                  .counter_name  = "um-share-buffer-count",
                                  .is_indexed = true,
                                  .is_double_indexed = true,
                                  .num_of_rows = BSTAPP_PRI_GROUP_SHARED_ROWS,
                                  .num_of_columns = BSTAPP_PRI_GROUP_SHARED_COLUMNS,
                                  .size = BSTAPP_PG_SHARED_SIZE,
                                  .offset = offsetof (BSTAPP_STAT_DB_t, iPGShared)
                                },
                                {
                                  .bid = BSTAPP_STAT_ID_PRI_GROUP_HEADROOM,
                                  .realm_name = "ingress-port-priority-group",
                                  .counter_name  = "um-headroom-buffer-count",
                                  .is_indexed = true,
                                  .is_double_indexed = true,
                                  .num_of_rows = BSTAPP_PRI_GROUP_HEADROOM_ROWS,
                                  .num_of_columns = BSTAPP_PRI_GROUP_HEADROOM_COLUMNS,
                                  .size = BSTAPP_PG_HEADROOM_SIZE,
                                  .offset = offsetof (BSTAPP_STAT_DB_t, iPGHeadroom)
                                },
                                {
                                  .bid = BSTAPP_STAT_ID_UCAST, 
                                  .realm_name = "egress-uc-queue",
                                  .counter_name  = "uc-buffer-count",
                                  .is_indexed = true,
                                  .is_double_indexed = false,
                                  .num_of_rows = BSTAPP_UCAST_ROWS,
                                  .num_of_columns = BSTAPP_UCAST_COLUMNS,
                                  .size = BSTAPP_E_UC_STAT_SIZE,
                                  .offset = offsetof (BSTAPP_STAT_DB_t, ucQ)
                                },
                                {
                                  .bid = BSTAPP_STAT_ID_MCAST, 
                                  .realm_name = "egress-mc-queue",
                                  .counter_name  = "mc-buffer-count",
                                  .is_indexed = true,
                                  .is_double_indexed = false,
                                  .num_of_rows = BSTAPP_MCAST_ROWS,
                                  .num_of_columns = BSTAPP_MCAST_COLUMNS,
                                  .size = BSTAPP_E_MC_STAT_SIZE,
                                  .offset = offsetof (BSTAPP_STAT_DB_t, mcQ)
                                },
                                {
                                  .bid = BSTAPP_STAT_ID_MCAST_QUEUE_ENTRIES, 
                                  .realm_name = "egress-mc-queue",
                                  .counter_name  = "mc-queue-entries",
                                  .is_indexed = true,
                                  .is_double_indexed = false,
                                  .num_of_rows = BSTAPP_MCAST_ROWS,
                                  .num_of_columns = BSTAPP_MCAST_COLUMNS,
                                  .size = BSTAPP_E_MC_STAT_SIZE,
                                  .offset = offsetof (BSTAPP_STAT_DB_t, mcQEntries)
                                },
                                {
                                  .bid = BSTAPP_STAT_ID_EGR_UCAST_PORT_SHARED,
                                  .realm_name = "egress-port-service-pool",
                                  .counter_name  = "uc-share-buffer-count",
                                  .is_indexed = true,
                                  .is_double_indexed = true,
                                  .num_of_rows = BSTAPP_EGR_UCAST_PORT_SHARED_ROWS,
                                  .num_of_columns = BSTAPP_EGR_UCAST_PORT_SHARED_COLUMNS,
                                  .size = BSTAPP_E_P_SP_UC_SHARE_STAT_SIZE,
                                  .offset = offsetof (BSTAPP_STAT_DB_t, ePortSPucShare)
                                },
                                {
                                  .bid = BSTAPP_STAT_ID_EGR_PORT_SHARED,
                                  .realm_name = "egress-port-service-pool",
                                  .counter_name  = "um-share-buffer-count",
                                  .is_indexed = true,
                                  .is_double_indexed = true,
                                  .num_of_rows = BSTAPP_EGR_PORT_SHARED_ROWS,
                                  .num_of_columns = BSTAPP_EGR_PORT_SHARED_COLUMNS,
                                  .size = BSTAPP_E_P_SP_UM_SHARE_STAT_SIZE,
                                  .offset = offsetof (BSTAPP_STAT_DB_t, ePortSPumShare)
                                },
                                {
                                  .bid = BSTAPP_STAT_ID_RQE_QUEUE,
                                  .realm_name = "egress-rqe-queue",
                                  .counter_name  = "rqe-buffer-count",
                                  .is_indexed = true,
                                  .is_double_indexed = false,
                                  .num_of_rows = BSTAPP_RQE_QUEUE_ROWS,
                                  .num_of_columns = BSTAPP_RQE_QUEUE_COLUMNS,
                                  .size = BSTAPP_E_RQE_QUEUE_STAT_SIZE,
                                  .offset = offsetof (BSTAPP_STAT_DB_t, rqe)
                                },
                                {
                                  .bid = BSTAPP_STAT_ID_RQE_POOL,
                                  .realm_name = "egress-rqe-queue",
                                  .counter_name  = "rqe-queue-count",
                                  .is_indexed = true,
                                  .is_double_indexed = false,
                                  .num_of_rows = BSTAPP_RQE_POOL_ROWS,
                                  .num_of_columns = BSTAPP_RQE_POOL_COLUMNS,
                                  .size = BSTAPP_E_RQE_STAT_SIZE,
                                  .offset = offsetof (BSTAPP_STAT_DB_t, rqeQueueEntries)
                                },
                                {
                                  .bid = BSTAPP_STAT_ID_UCAST_GROUP,
                                  .realm_name = "egress-uc-queue-group",
                                  .counter_name  = "uc-buffer-count",
                                  .is_indexed = true,
                                  .is_double_indexed = false,
                                  .num_of_rows = BSTAPP_UCAST_GROUP_ROWS,
                                  .num_of_columns = BSTAPP_UCAST_GROUP_COLUMNS,
                                  .size = BSTAPP_E_UC_Q_GROUP_STAT_SIZE,
                                  .offset = offsetof (BSTAPP_STAT_DB_t, eUCqGroup)
                                },
                                {
                                  .bid = BSTAPP_STAT_ID_CPU_QUEUE, 
                                  .realm_name = "egress-cpu-queue",
                                  .counter_name  = "cpu-buffer-count",
                                  .is_indexed = true,
                                  .is_double_indexed = false,
                                  .num_of_rows = BSTAPP_CPU_QUEUE_ROWS,
                                  .num_of_columns = BSTAPP_CPU_QUEUE_COLUMNS,
                                  .size = BSTAPP_E_CPU_STAT_SIZE,
                                  .offset = offsetof (BSTAPP_STAT_DB_t, eCPU)
                                },
                                 {
                                  .bid = BSTAPP_STAT_ID_EGR_MCAST_SHARE_QUEUE,
                                  .realm_name = "egress-service-pool",
                                  .counter_name  = "mc-share-queue-entries",
                                  .is_indexed = true,
                                  .is_double_indexed = false,
                                  .num_of_rows = BSTAPP_EGR_MCAST_SHARED_QUEUE_POOL_ROWS,
                                  .num_of_columns = BSTAPP_EGR_MCAST_SHARED_QUEUE_POOL_COLUMNS,
                                  .size = BSTAPP_E_SP_MC_SHARED_QUEUE_STAT_SIZE,
                                  .offset = offsetof (BSTAPP_STAT_DB_t, eSpMcShareQueue)
                                },
                                 {
                                  .bid = BSTAPP_STAT_ID_EGR_PORT_MCAST_SHARED,
                                  .realm_name = "egress-port-service-pool",
                                  .counter_name  = "mc-share-buffer-count",
                                  .is_indexed = true,
                                  .is_double_indexed = true,
                                  .num_of_rows = BSTAPP_EGR_PORT_MCAST_SHARED_BUFFER_ROWS,
                                  .num_of_columns = BSTAPP_EGR_PORT_MCAST_SHARED_BUFFER_COLUMNS,
                                  .size = BSTAPP_E_PORT_SP_MC_SHARED_BUFFER_STAT_SIZE,
                                  .offset = offsetof (BSTAPP_STAT_DB_t, ePSpMcShareBuffer)
                                },
                                  {
                                  .bid = BSTAPP_STAT_ID_EGR_PORT_MCAST_SHARED_QUEUE,
                                  .realm_name = "egress-port-service-pool",
                                  .counter_name  = "mc-share-queue-entries",
                                  .is_indexed = true,
                                  .is_double_indexed = true,
                                  .num_of_rows = BSTAPP_EGR_PORT_MCAST_SHARED_QUEUE_ROWS,
                                  .num_of_columns = BSTAPP_EGR_PORT_MCAST_SHARED_QUEUE_COLUMNS,
                                  .size = BSTAPP_E_PORT_SP_MC_SHARED_QUEUE_STAT_SIZE,
                                  .offset = offsetof (BSTAPP_STAT_DB_t, ePSpMcShareQueue)
                                },

                              };         

 
/*********************************************************************
* @brief   Resolve  cache index using bid table
*
* @param[in]   asic     -  asic number   
* @param[in]   bid      -  bid number   
* @param[in]   port     -  port number   
* @param[in]   index    -  Queue index   
* @param[out]   db_index -  Index in bid stat/threshold array   

*
* @retval -1      Failed to get db index from given params 
* @retval 0      db_index is successfully calculated
*
*
* @notes    none
*********************************************************************/
static int bst_resolve_index(int asic, int bid, int port, int index, int *db_index)
{
  int  i = 0;
  int num_of_entries = sizeof(bid_tab_params)/sizeof(BSTAPP_BID_PARAMS_t);


  for (i = 0;  i < num_of_entries; i++)  
  {  
    if (bid == bid_tab_params[i].bid)
    {
      if (bid_tab_params[i].is_double_indexed == true)
      {
        *db_index = (((port - 1) * bid_tab_params[i].num_of_columns) + index); 
      }
      else
      {
        *db_index = (index);
      }
      return 0;
    }
  }
  return -1;
}


/*********************************************************************
* @brief   Convert bid, port, index to name in bufmon table format
*           <realm>/<name>/<index1>/<index2> 
*
* @param[in]   asic     -  asic number   
* @param[in]   bid      -  bid number   
* @param[in]   port     -  port number   
* @param[out]  _key  -  bst app bufmon table's name entry     
* @param[in]   length_of_key -length of the buffer that _key points to   

*
* @retval -1      Failed to prepare  key from given params 
* @retval 0      _key is successfully prepared 
*
*
* @notes    none
*********************************************************************/
int bst_bid_port_index_to_key(int asic, int bid,
                             int port, int index,  
                             char *_key, int length_of_key)
{
  char src_string[1024] = {0};
  char delim[2] = "_";
  int num_of_entries = sizeof(bid_tab_params)/sizeof(BSTAPP_BID_PARAMS_t);
  int i = 0;
  char port_str[16] = {0};
  char index_str[16] = {0};
  int src_str_empty_size;

  src_str_empty_size = sizeof(src_string);
 
 
  /* Get bid table entry details */
  for (i = 0; i < num_of_entries; i++)
  {
    if (bid_tab_params[i].bid  == bid)
    {
      break;
    }
  }

  /* Entry is not found */ 
  if (i == num_of_entries)
  {
    BSTAPP_DEBUG_PRINT("Not able to find bid %d entry in"
                        "BST table\n", bid);
    return -1;
  }

  strcpy(src_string, stat_prefix);
  /* _key string is of the format <realm>/<name>/<index1>/<index2> */ 
  strncat(src_string, bid_tab_params[i].realm_name, src_str_empty_size);

  src_str_empty_size -= strlen(src_string);
  if (src_str_empty_size < 2)
  {
   BSTAPP_DEBUG_PRINT("Not enough memory in source string");
  }

  
  strcat(src_string, delim);
  strncat(src_string, bid_tab_params[i].counter_name, src_str_empty_size);
  src_str_empty_size -= strlen(src_string);
  if (src_str_empty_size < 2)
  {
   BSTAPP_DEBUG_PRINT("Not enough memory in source string");
  }

  strcat(src_string, delim);
  if (bid_tab_params[i].is_indexed == true)
  {
    if (bid_tab_params[i].is_double_indexed == true) 
    {
      sprintf(port_str, "%d", port);

      strncat(src_string, port_str, src_str_empty_size);
      src_str_empty_size -= strlen(src_string);
      if (src_str_empty_size < 2)
      {
        BSTAPP_DEBUG_PRINT("Not enough memory in source string");
      }
      strcat(src_string, delim);
    }
 
    sprintf(index_str, "%d", index);
    strncat(src_string, index_str, src_str_empty_size);
    src_str_empty_size -= strlen(src_string);
    if (src_str_empty_size < 2)
    {
      BSTAPP_DEBUG_PRINT("Not enough memory in source string");
    }
    
    if (bid_tab_params[i].is_double_indexed == false)
    {
      strcat(src_string, delim);
      strncat(src_string, "NONE", src_str_empty_size);
      src_str_empty_size -= strlen(src_string);
      if (src_str_empty_size < 2)
      {
        BSTAPP_DEBUG_PRINT("Not enpough memory in source string");
      }
    }
  }
  else
  {
    sprintf(index_str, "%s%s%s", "NONE",delim,"NONE");  
    strncat(src_string, index_str, src_str_empty_size);
    src_str_empty_size -= strlen(src_string);
    if (src_str_empty_size < 2)
    {
      BSTAPP_DEBUG_PRINT("Not enough memory in source string");
    }
  }
  
  if (length_of_key <= strlen(src_string))
  {
    BSTAPP_DEBUG_PRINT("Not enough memory in source string");
    return -1;
  }
  strcpy (_key, src_string);
  return 0;
}


/*********************************************************************
* @brief   Convert <realm>/<name>/<index1>/<index2> formatted to key to
*           port, bid, index etc 
*
* @param[in]   asic     -  asic number   
* @param[in]  _key  -  bst app bufmon table's name/key entry     
* @param[out]   bid      -  bid number   
* @param[out]   port     -  port number   
* @param[out]  index  -  Queue index    

*
* @retval -1      Failed to prepare given params from  key  
* @retval 0      bid, port and index are are successfully prepared 
*                                    from _key
*
*
* @notes    none
*********************************************************************/
int bst_key_to_bid_port_index(int asic, char *key, int *bid,
                                             int *port, int *index)
{
  char src_string[1024] = {0};
  char delim[2] = "_";
  char *realm   = NULL;
  char *name    = NULL;
  char *index1   = NULL;
  char *index2   = NULL;
  char *final_token = NULL;
  char *prefix = NULL;
  int num_of_entries = sizeof(bid_tab_params)/sizeof(BSTAPP_BID_PARAMS_t);
  int i = 0;

 
  strcpy(src_string, key);
 
  /* key string is of the format bst_<realm>_<name>_<index1>_<index2> */ 
  /* Get realm */
  prefix = strtok(src_string, delim);
  if (prefix == NULL)
  {
    return -1;
  }

  realm = strtok(NULL, delim);
  if (realm == NULL)
  {
    return -1;
  }

  /* Get name */
  name = strtok(NULL, delim);
  if (name == NULL)
  {
    return -1;
  }

  /* Get  index1*/
  index1 = strtok(NULL, delim);
  if (index1 == NULL)
  {
    return -1;
  }

  /* Get  index2*/
  index2 = strtok(NULL, delim);
  if (index2 == NULL)
  {
    return -1;
  }

  final_token = strtok(NULL, delim);
  if (final_token != NULL)
  {
    return -1;
  }

  
  for (i = 0; i < num_of_entries; i++)
  {
    if (strcmp(bid_tab_params[i].realm_name, realm) == 0)
    {
      if (strcmp(bid_tab_params[i].counter_name, name) == 0)
      {
        break;
      }
    }
  }

  /* Not able to find out correct bid entry for realm/name combination */
  if (i == num_of_entries)
  {
    return -1;
  }

  *bid = bid_tab_params[i].bid;

  /* If it is double indexed then port is @first index */  
  if (bid_tab_params[i].is_double_indexed == true)  
  {
    *port = atoi(index1);
    *index = atoi(index2);
  }
  else
  {
    /* Device */
    if (bid_tab_params[i].is_indexed == false)
    {
      *index = 0;
    }
    else
    {
      *index = atoi(index1);
    }
  }
  return 0;
}

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
                                uint64_t *p_stat)
{
  int rv;
  int db_index;

  if ((rv = bst_resolve_index(asic, bid, port, index, &db_index))
                                               != 0)
  {
    BSTAPP_DEBUG_PRINT ("Failed to resolve db_index\n");
    return rv;
  }

  /* Get stat from BST cache */
  rv = bst_cache_stat_get(asic, bid, db_index, p_stat);
  if ( rv != 0)
  {
    return -1;    
  }
  return rv;
}

/*********************************************************************
* @brief   Set stat in BST  DB
*
* @param[in]   asic     -  asic number   
* @param[in]   port     -  port number   
* @param[in]   index    -  Queue index   
* @param[in]   bid      -  bid number   
* @param[in]   options  -     
* @param[in]   stat   -  bst stat 

*
* @retval -1      Stat set is failed
* @retval 0      Stat set is successfull
*
*
* @notes    none
*********************************************************************/
int bst_stat_set (int asic, int port, int index,
                                 int bid, int options, 
                                 uint64_t stat)
{
  int rv;
  int db_index;

  if ((rv = bst_resolve_index(asic, bid, port, index, &db_index))
                                               != 0)
  {
    BSTAPP_DEBUG_PRINT ("Failed to resolve db_index\n");
    return rv;
  }
  /* Set stat in BST cache */
  rv = bst_cache_stat_set(asic, bid, db_index, stat);
  if (rv != 0)
  {
    return -1;    
  }
  return 0;
}

/*********************************************************************
* @brief   Set stat in BST  DB based on key
*
* @param[in]   asic     -  asic number   
* @param[in]   _key - name in bufmon table row   
* @param[in]   stat   -  bst stat 

*
* @retval -1      Stat set is failed
* @retval 0      Stat set is successfull
*
*
* @notes    none
*********************************************************************/
int bst_stat_set_using_key (int asic, char *_key,
                                                           uint64_t stat)
{
  int bid;
  int port;
  int index;
  int options  = 0xff;
  int rv = 0;


  /* Convert _key to bid, port , index */ 
  if ((rv = bst_key_to_bid_port_index(asic, _key, &bid,
                                       &port, &index)) != 0)
  {
    BSTAPP_DEBUG_PRINT ("Failed to convert  key %s\n",
                             _key);
    return rv;
  }

  /* Set stat */
  if ((rv = bst_stat_set(asic, port, index, bid, options, stat)) !=
                                                       0) 
  {
    BSTAPP_DEBUG_PRINT ("Failed to set stat using  key %s\n",
                             _key);
                             
    return rv;
  }
 
  return rv;
}


/*********************************************************************
* @brief   Dumps BST BID table parameter. 
*
* @retval none      
*
*
* @notes    none
*********************************************************************/
void bst_dump_bid_tab_params()
{
  int index = 0;

  printf ("--------BID table params------\n"); 
  for (index = 0; index < BSTAPP_STAT_ID_MAX_COUNT; index ++)
  {
    printf("Table index = %d\n", index);
    printf("BID = %d\n", bid_tab_params[index].bid);
    printf("Realm = %s\n", bid_tab_params[index].realm_name);
    printf("Counter name= %s\n", bid_tab_params[index].counter_name);
    printf("Is indexed = %s\n", (bid_tab_params[index].is_indexed?"true":"false")); 
    printf("Is Double indexed = %s\n", (bid_tab_params[index].is_double_indexed?"true":"false")); 
    printf("Rows = %d\n", bid_tab_params[index].num_of_rows);
    printf("Columns = %d\n", bid_tab_params[index].num_of_columns);
  }
}

