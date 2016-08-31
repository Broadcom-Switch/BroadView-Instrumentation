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

/* OVSDB includes*/
#include <pthread.h>

#include "ovsdb/column.h"
#include "ovsdb/table.h"
#include "ovsdb/ovsdb.h"
#include "compiler.h"
#include "json.h"
#include "jsonrpc.h"
#include "table.h"
#include "ovsdb-data.h"
#include "ovsdb-error.h"
#include "poll-loop.h"
#include "sort.h"
#include "stream.h"
#include "util.h"

/* BroadView Includes*/
#include "sbplugin_bst_ovsdb.h"
#include "sbplugin_bst_cache.h"

#define OVSDB_CONFIG_MAX_LINE_LENGTH       256
#define OVSDB_CONFIG_FILE                  "ovsdb_config.cfg"
#define OVSDB_CONFIG_READMODE              "r"
#define OVSDB_CONFIG_DELIMITER             "="
#define OVSDB_CONFIG_MODE_DELIMITER        ":"
#define OVSDB_MODE_TCP                     "tcp"
#define OVSDB_MODE_FILE                    "unix"
#define OVSDB_SOCKET                       "ovsdb_socket"
#define OVSDB_MAX_LINES                    2  
#define _DEFAULT_OVSDB_SOCKET "unix:/usr/local/var/run/openvswitch/db.sock"

#define OVSDB_ASSERT_CONFIG_FILE_ERROR(condition) do { \
    if (!(condition)) { \
        SB_OVSDB_LOG(BVIEW_LOG_ERROR, \
                    "OVSDB (%s:%d) Unrecognized Config File format, may be corrupted. Errno : %s  \n", \
                    __func__, __LINE__, strerror(errno)); \
                        fclose(configFile); \
        return (BVIEW_STATUS_FAILURE); \
    } \
} while(0)


/* UUID size id 36 as per RFC 7047*/
#define   OVSDB_UUID_SIZE            36


#define   BST_OVSDB_THRESHOLD_JSON    "[\"bufmon\",{\"op\":\"update\",\"table\":\"bufmon\",\"row\":{\"trigger_threshold\":%lld},\"where\":[[\"name\",\"==\", \"%s\"]]}]"

#define   BST_OVSDB_CONFIG_JSON_FORMAT       "[\"bufmon\",{\"op\":\"update\",\"table\":\"System\",\"row\":{\"bufmon_config\":[\"map\",[[\"enabled\",\"%s\"], [\"counters_mode\",\"%s\"], [\"periodic_collection_enabled\",\"%s\"],[\"collection_period\",\"%s\"]]]} , \"where\":[[\"_uuid\",\"==\",[\"uuid\", \"%s\"]]]}]"


#define  BST_OVSDB_FORM_CONFIG_JSON(_buf, _format, args...) \
                                  {\
                                    sprintf ((_buf), (_format), ##args);\
                                  }  

#define  OVSDB_GET_COLUMN(_column,_old, _new,_columnname)   \
       {\
         if (!(_old)) \
         { \
           (_column) = shash_find_data (json_object((_new)), (_columnname));  \
         } \
         else if (!(_new)) \
         { \
           (_column) = shash_find_data (json_object((_old)), (_columnname));  \
         } \
         else \
         { \
           (_column) = shash_find_data (json_object((_old)), (_columnname)); \
         }\
       }

static char system_table_uuid[OVSDB_UUID_SIZE];

/* Table/columns to be monitored*/
struct m_table {
    struct ovsdb_table_schema *table;
    struct ovsdb_column_set columns;
};

/*********************************************************************
* @brief       Check the return error code of both reply and request
*
* @param[in]  error               -  Erorr 
* @param[in]  reply               -  Pointer to reply JSON message
*                       
* @retval     schema if successful.
*             NULL   if failure.
*
*********************************************************************/
static void
check_txn(int error, struct jsonrpc_msg **reply_)
{
  struct jsonrpc_msg *reply ;

  if (!reply_)
  {
    return;
  }
  reply = *reply_;

  if (error) 
  {
    SB_OVSDB_DEBUG_PRINT ("Transaction failed");
    return;  
  }

  if (reply->error) 
  {
    SB_OVSDB_DEBUG_PRINT ("Transaction returned errors");
    return;
  }
}

/*********************************************************************
* @brief       Get Schema from OVSDB-SERVER.
*
* @param[in]  rpc               -  RPC session
* @param[in]  database          -  Database
*                       
* @retval     schema if successful.
*             NULL   if failure.
*
*********************************************************************/
static struct ovsdb_schema *
get_schema (struct jsonrpc *rpc, const char *database)
{
  struct jsonrpc_msg *request, *reply;
  struct ovsdb_schema *schema;
  struct ovsdb_error *error;

  /* NULL Pointer validation*/
  SB_OVSDB_NULLPTR_CHECK (rpc, NULL);
  SB_OVSDB_NULLPTR_CHECK (database, NULL);

  /* Create get_schema request and send */
  request = jsonrpc_create_request ("get_schema",
                                    json_array_create_1(
                                    json_string_create(database)),
                                    NULL);
  check_txn(jsonrpc_transact_block(rpc, request, &reply), &reply);
  error = ovsdb_schema_from_json(reply->result, &schema);
  if (error)
  {
    SB_OVSDB_LOG (BVIEW_LOG_ERROR, "Failed to get schema %s", 
                          ovsdb_error_to_string(error));

    return NULL;
  }
  jsonrpc_msg_destroy(reply);

  return schema;
}

/*********************************************************************
* @brief    Open JSON RPC session.
*
* @param[in]     server          - Sock file/TCP/UDP port     
*
* @retval        Pointer to JSON RPC session.
*
* @notes   
*
*
*********************************************************************/
static struct jsonrpc *
open_jsonrpc(const char *server)
{
  struct stream *stream;
  int error;
  
  /* NULL pointer validation*/
  SB_OVSDB_NULLPTR_CHECK (server, NULL);

  error = stream_open_block(jsonrpc_stream_open(server, &stream,
                              DSCP_DEFAULT), &stream);
  if (error == EAFNOSUPPORT) 
  {
    struct pstream *pstream;

    error = jsonrpc_pstream_open(server, &pstream, DSCP_DEFAULT);
    if (error) 
    {
      SB_OVSDB_LOG (BVIEW_LOG_ERROR,
                   "Failed to connect to: (%s)",
                    server);
      return NULL;
    }

    error = pstream_accept_block(pstream, &stream);
    if (error) 
    {
      SB_OVSDB_LOG (BVIEW_LOG_ERROR,
                   "Failed to accept connection: (%s)",
                    server);
      
      pstream_close(pstream);
      return NULL; 
    }
  } 
  else if (error) 
  {
    SB_OVSDB_LOG (BVIEW_LOG_ERROR,
                  "Failed to connect to (%s)",
                  server);
    return NULL;
  }

  return jsonrpc_open(stream);
}


/*********************************************************************
* @brief    Get 'Value' associated with 'Key' in bufmon_config and 
*           bufmon_info columns.
*
* @param[in]    json_object       - Pointer to the JSON object.
* @param[in]    p_string          - Pointer to the "Key" String.
* @param[out]   p_value           - Pointer to the "value" String. 
*
* @retval
* 
* @notes     This function is to get "Value" string of perticualr 
*            "key" in bufmon_config and bufmon_info columns.
*
*            Need to enchance this function to parse all row elements 
*            at a time.
*********************************************************************/

static BVIEW_STATUS
bst_system_bufmon_config_update (struct json *json_object)
{
  int elem = 0;
  struct json *array = NULL;
  struct json *key = NULL;
  struct json *value = NULL;
  struct json *sub_array = NULL;
  struct json *map = NULL;
  BVIEW_OVSDB_BST_DATA_t     *p_cache = NULL;
  BVIEW_OVSDB_CONFIG_DATA_t  *bufmon_config = NULL; 

  /* NULL pointer validation */ 
  SB_OVSDB_NULLPTR_CHECK (json_object, BVIEW_STATUS_INVALID_PARAMETER);

  /* bufmon_config column is array of 'key' and 'value' strings
   * the first element is always string "map".
   * ["map",[["enabled","true"],["counters_mode","peak"],[ ], p].....] 
   */

  /* Return failure if object is not an array or 
   *                if first element is not the string "map"
   *                if second element type is not an array
   */
  map =  json_array(json_object)->elems[0];
  array = json_array(json_object)->elems[1];
  if ((json_object->type != JSON_ARRAY) ||
      (strcmp ("map",map->u.string) != 0) || 
      (array->type != JSON_ARRAY))
  {
    return BVIEW_STATUS_FAILURE;
  }

  p_cache = bst_ovsdb_cache_get ();
  if (!p_cache)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Acquire read lock*/
  SB_OVSDB_RWLOCK_WR_LOCK(p_cache->lock);


  bufmon_config = &p_cache->config_data;
  for (elem = 0; elem < json_array(array)->n_allocated; elem++)
  {
    /* Each 'key' 'value' pair is an array with two elements*/
    sub_array = json_array (array)->elems[elem];
    if (sub_array->type == JSON_ARRAY)
    {
      key = json_array (sub_array)->elems[0];
      value = json_array (sub_array)->elems[1];
      if (strcmp (key->u.string, "counters_mode") == 0)
      {
         bufmon_config->bst_tracking_mode = 
               ((strcmp (value->u.string, "peak") == 0)? 1 : 0);
      }
      else if (strcmp (key->u.string, "enabled") ==0)
      {
         bufmon_config->bst_enable = 
               ((strcmp (value->u.string, "true") == 0)? true : false);  
      }
      else if (strcmp (key->u.string, "periodic_collection_enabled") ==0)
      {
         bufmon_config->periodic_collection  =
               ((strcmp (value->u.string, "true") == 0)? true : false);
      }
      else if (strcmp (key->u.string, "collection_period") ==0) 
      {
         bufmon_config->collection_interval = atoi(value->u.string);
      }
    }
  }

  /* Release lock */
  SB_OVSDB_RWLOCK_UNLOCK(p_cache->lock);

  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief    Update SB PLUGIN cache.
*
* @param[in]   table_name   - Pointer to the table_name string.
* @param[in]   table_update - Pointer to "Update" JSON Object.
* @param[in]   Initial      - Initial rows
*
* @retval
*
* @notes    if Initial 
*
*
*********************************************************************/
static BVIEW_STATUS
bst_ovsdb_cache_update_table(char *table_name, struct json *table_update,
                               bool initial)
{
  struct shash_node *node;

  /* NULL Pointer validation*/
  SB_OVSDB_NULLPTR_CHECK (table_update, BVIEW_STATUS_INVALID_PARAMETER);
  SB_OVSDB_NULLPTR_CHECK (table_name, BVIEW_STATUS_INVALID_PARAMETER);

  /* return if JSON type is not object*/
  if (table_update->type != JSON_OBJECT) 
  {
     return BVIEW_STATUS_FAILURE;
  }
  /* Loop through all Nodes and update the cahce*/
  SHASH_FOR_EACH (node, json_object(table_update)) 
  {
    BVIEW_OVSDB_BID_INFO_t   row = {0,0};
    struct json *row_update = node->data;
    struct json *old, *new, *hw_unit_id, *name, *counter_value, *trigger_threshold;

    if (row_update->type != JSON_OBJECT) {
        continue;
    }
    old = shash_find_data(json_object(row_update), "old");
    new = shash_find_data(json_object(row_update), "new");
    if (strcmp (table_name, "bufmon") == 0)
    {
      OVSDB_GET_COLUMN (hw_unit_id, old, new , "hw_unit_id")
      OVSDB_GET_COLUMN (name, old, new , "name")
      OVSDB_GET_COLUMN (counter_value, old, new , "counter_value")
      OVSDB_GET_COLUMN (trigger_threshold, old, new , "trigger_threshold")

      /* Name + hw_unit_id is key, if both are NULL don't update the cache.*/
      if (name && hw_unit_id)
      {
        if (counter_value && counter_value->type == JSON_INTEGER)
        {
          row.stat = counter_value->u.integer;
        }
          
        if (trigger_threshold && trigger_threshold->type == JSON_INTEGER)
        {
           row.threshold = trigger_threshold->u.integer;
        }
    
        /* Update BST cache*/
        bst_ovsdb_row_update (hw_unit_id->u.integer, 
                              name->u.string,
                              &row);
      }
    } /* if (strcmp (table_name, ..... */
    else if (strcmp (table_name,"System") ==0)
    {
      struct json *config;

      /* Validate UUID length*/
      if (strlen (node->name) !=  OVSDB_UUID_SIZE)
      {
        SB_OVSDB_LOG (BVIEW_LOG_ERROR,
               "OVSDB BST monitor: Invalid UUID length (%d)",
                strlen (node->name));
        continue;
      }  
      /* COPY UUID*/
      strcpy (system_table_uuid, node->name);

      OVSDB_GET_COLUMN (config, old, new, "bufmon_config");
      if (config)
      {
        bst_system_bufmon_config_update (config);
      }
    } 
  } /* SHASH_FOR_EACH (node, json_object(table_update)) */
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief     update the sbplugin cache.
*
*@param[in]  table_updates    -  Pointer to "Update" JSON object.
*@param[in]  mts              -  Pointer to local monitored table.
*@param[in]  n_mts            -  number of tables monitored.
*@param[in]  initial          -  Is it initial Notification from server.
*
* @retval
* @notes   
*
*
*********************************************************************/
static BVIEW_STATUS
bst_ovsdb_cache_update(struct json *table_updates,
                         const struct m_table *mts, size_t n_mts,
                         bool initial)
{
  size_t i;
  const struct m_table *mt;
  struct json *table_update;    

  SB_OVSDB_NULLPTR_CHECK (table_updates, BVIEW_STATUS_INVALID_PARAMETER);
  SB_OVSDB_NULLPTR_CHECK (mts, BVIEW_STATUS_INVALID_PARAMETER);
  /* Table is JSON object. Dont Process if the type is not
   * JSON object.
   */ 
  if (table_updates->type != JSON_OBJECT) 
  {
    SB_OVSDB_DEBUG_PRINT(
                         "Update JSON type is not object %d", 
                          table_updates->type);
    return BVIEW_STATUS_FAILURE;
  }
  /* Loop through all the tables which are configured to be monitored*/
  for (i = 0; i < n_mts; i++) 
  {
    mt = &mts[i];
    table_update = shash_find_data(json_object(table_updates),
                                  mt->table->name);
    if (table_update) 
    {
       bst_ovsdb_cache_update_table(mt->table->name, table_update, initial);
    }
  }
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief     Add the colum to JSON request
*  
* @param[in]  Column       - Pointer to column
* @param[in]  columns      - Pointer to the Column Set
* @param[in]  column_json  - Pointer JSON Object of the column
*
* @retval
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS
bst_ovsdb_add_column (const struct ovsdb_column *column,
                     struct ovsdb_column_set *columns, struct json *columns_json)
{
  SB_OVSDB_NULLPTR_CHECK (column, BVIEW_STATUS_INVALID_PARAMETER);
  SB_OVSDB_NULLPTR_CHECK (columns, BVIEW_STATUS_INVALID_PARAMETER);
  SB_OVSDB_NULLPTR_CHECK (columns_json, BVIEW_STATUS_INVALID_PARAMETER); 

  if (ovsdb_column_set_contains(columns, column->index)) 
  {
    return BVIEW_STATUS_FAILURE;
  }
  ovsdb_column_set_add(columns, column);
  json_array_add(columns_json, json_string_create(column->name));
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief   Parse and build the JSON request 
*
* @param[in]   table          -  Pointer to Schema Table
* @param[in]   columns        -  Pointer to Columns Set
*
* @retval
*
* @notes    none
*
*
*********************************************************************/

static struct json *
bst_ovsdb_parse_monitor_columns (const struct ovsdb_table_schema *table,
                                struct ovsdb_column_set *columns)
{
  struct json *mr, *columns_json;

  SB_OVSDB_NULLPTR_CHECK (table, NULL);
  SB_OVSDB_NULLPTR_CHECK (columns, NULL);

  mr = json_object_create();
  columns_json = json_array_create_empty();
  json_object_put(mr, "columns", columns_json);

  if (columns_json->u.array.n == 0) 
  {
    const struct shash_node **nodes;
    size_t i, n;

    n = shash_count(&table->columns);
    nodes = shash_sort(&table->columns);
    for (i = 0; i < n; i++) 
    {
      const struct ovsdb_column *column = nodes[i]->data;
      if (column->index != OVSDB_COL_UUID
         && column->index != OVSDB_COL_VERSION) 
      {
         bst_ovsdb_add_column (column, columns, columns_json);
      }
    }
    free(nodes);

    bst_ovsdb_add_column (ovsdb_table_schema_get_column(table, "_version"),
                   columns, columns_json);
  }
  
  return mr;
}

/*********************************************************************
* @brief   Add  :columns to be monitored
*
* @param[in]   n_columns          -  Number of columns.
* @param[in]   table              -  Table Schema
* @param[in]   monitor_requests   -  JSON object of monitor request
* @param[in]   mts                -  Stored Moniter table/Column Info 
* @param[in]   n_mts              -  Number of Tables 
* @param[in]   allocated_mts      -  Number of allocated tables
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS
bst_ovsdb_add_monitored_table(int n_columns,
                              struct ovsdb_table_schema *table,
                              struct json *monitor_requests,
                              struct m_table **mts,
                              size_t *n_mts, size_t *allocated_mts)
{
  struct json *monitor_request_array;
  struct m_table *mt;

  SB_OVSDB_NULLPTR_CHECK (table, BVIEW_STATUS_INVALID_PARAMETER);
  SB_OVSDB_NULLPTR_CHECK (monitor_requests, BVIEW_STATUS_INVALID_PARAMETER);
  SB_OVSDB_NULLPTR_CHECK (mts, BVIEW_STATUS_INVALID_PARAMETER);
  SB_OVSDB_NULLPTR_CHECK (allocated_mts, BVIEW_STATUS_INVALID_PARAMETER);

  if (*n_mts >= *allocated_mts) 
  {
    *mts = x2nrealloc(*mts, allocated_mts, sizeof **mts);
  }
  mt = &(*mts)[(*n_mts)++];
  mt->table = table;
  ovsdb_column_set_init(&mt->columns);

  monitor_request_array = json_array_create_empty();
  if (n_columns > 1) 
  {
    int i;

    for (i = 0; i < n_columns; i++) 
    {
      json_array_add(monitor_request_array,
                     bst_ovsdb_parse_monitor_columns(table,
                                      &mt->columns));
    }
  } 
  else 
  {
    json_array_add(monitor_request_array,
              bst_ovsdb_parse_monitor_columns (table, &mt->columns));
  }

  json_object_put(monitor_requests, table->name, monitor_request_array);
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief           Set default connection parameters                                 
*
* @param[in,out]   connectMode  - Connection Type 
*
* @notes
*
* @retval          none
*
*********************************************************************/

void ovsdb_set_default(char * connectMode)
{
  if (connectMode != NULL)
  {
    strcpy(connectMode,_DEFAULT_OVSDB_SOCKET);
  }
  else
  {
    SB_OVSDB_LOG (BVIEW_LOG_ERROR,
               "OVSDB Set Default : NULL Pointer\n");
    return;
  } 
}	

/*********************************************************************
* @brief           Read OVSDB Config File                                 
*
* @param[in,out]   connectMode  - Connection Type 
*
* @notes
*
* @retval          BVIEW_STATUS_SUCCESS for successful execution
*
*********************************************************************/

BVIEW_STATUS ovsdb_file_read(char * connectMode)
{
  FILE *configFile;
  char line[OVSDB_CONFIG_MAX_LINE_LENGTH] = {0};
  char line_copy[OVSDB_CONFIG_MAX_LINE_LENGTH] = {0};
  int numLines = 0;
  char *property,*value;
  if (connectMode == NULL)
  {
    SB_OVSDB_LOG (BVIEW_LOG_ERROR,
               "OVSDB File Read : NULL Pointer\n");
    return BVIEW_STATUS_FAILURE;
  }
  configFile = fopen(OVSDB_CONFIG_FILE, OVSDB_CONFIG_READMODE);
  if (configFile == NULL)
  {
    ovsdb_set_default(connectMode);
    SB_OVSDB_LOG (BVIEW_LOG_ERROR,
               "OVSDB File Read : Failed to open config file %s", 
                OVSDB_CONFIG_FILE);
    return BVIEW_STATUS_FAILURE;
  }
  else
  { 
    while (numLines < OVSDB_MAX_LINES)
    {
      memset (&line[0], 0, OVSDB_CONFIG_MAX_LINE_LENGTH);
      memset (&line_copy[0], 0, OVSDB_CONFIG_MAX_LINE_LENGTH);
      property= fgets(&line[0], OVSDB_CONFIG_MAX_LINE_LENGTH, configFile);
      OVSDB_ASSERT_CONFIG_FILE_ERROR(property != NULL);
      /*Ignoring commented line in config file*/
      if (line[0] == '#')
      {
        numLines++;
        continue;
      }    
      property = strtok(&line[0], OVSDB_CONFIG_DELIMITER);
      OVSDB_ASSERT_CONFIG_FILE_ERROR(property != NULL);
      value = property + strlen(property) + 1; 
      if (strcmp(property, OVSDB_SOCKET) == 0)
      {
        value[strlen(value)] = 0;
        strcpy(line_copy,value);
        property = strtok(&value[0], OVSDB_CONFIG_MODE_DELIMITER);
        OVSDB_ASSERT_CONFIG_FILE_ERROR(property != NULL);
        if ((strcmp(property, OVSDB_MODE_TCP) == 0) || (strcmp(property, OVSDB_MODE_FILE) == 0))
        {
          strncpy(connectMode,line_copy,(strlen(line_copy) - 1));
        }
        else
        {
          SB_OVSDB_LOG (BVIEW_LOG_ERROR,
                   "OVSDB File Read : Invalid content in config file %s", 
                    OVSDB_CONFIG_FILE);
          fclose(configFile);  
          return BVIEW_STATUS_FAILURE;
        } 
        numLines++;
        continue; 
      }
      else
      {
          SB_OVSDB_LOG (BVIEW_LOG_ERROR,
                   "OVSDB File Read : Invalid content in config file %s", 
                    OVSDB_CONFIG_FILE);
          fclose(configFile);  
          return BVIEW_STATUS_FAILURE;
      }    
    }
  }  
  SB_OVSDB_LOG (BVIEW_LOG_INFO,
               "OVSDB File Read : File contents processed sucessfully. config file %s", 
                OVSDB_CONFIG_FILE);
  fclose(configFile);
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief   BST OVSDB monitor thread
*
*
*
* @notes   Receive JSON notification from OVSDB-SERVER and Update the 
*          SB PLUGIN cache.
*
*
*********************************************************************/
void
bst_ovsdb_monitor()
{
  const char *server;
  struct ovsdb_schema *schema;
  struct jsonrpc_msg *request;
  struct json *monitor, *monitor_requests,*request_id;
  struct jsonrpc *rpc;
  const char *database = "bufmon";
  int     n_columns = 1;
  struct jsonrpc_msg *msg;
  int error;
  struct m_table *mts;
  size_t n_mts, allocated_mts;
  size_t n,i;
  const struct shash_node **nodes;
  char connectMode[OVSDB_CONFIG_MAX_LINE_LENGTH];
  struct json *params;
    
    /* Open RPC Session*/
  memset (&connectMode[0], 0, OVSDB_CONFIG_MAX_LINE_LENGTH);
  if (ovsdb_file_read(connectMode) != BVIEW_STATUS_SUCCESS)
  {
    /*Set default connection mode*/
    memset (&connectMode[0], 0, OVSDB_CONFIG_MAX_LINE_LENGTH);
    ovsdb_set_default(connectMode);
  }
  rpc = open_jsonrpc (connectMode); 
  if (!rpc) 
  {
    SB_OVSDB_LOG (BVIEW_LOG_ERROR,
               "OVSDB BST monitor:Failed to open JSNON RPC session %s", 
                connectMode);
    return;
  }
    /* Get name of the server*/ 
  server = jsonrpc_get_name (rpc);
  if (!server)
  {
    return;
  }

    /* Build the Table and/or columns to be monitored*/
  schema = get_schema(rpc, database);

    /* Create JSON monitor request and send*/
  monitor_requests = json_object_create();

  mts = NULL;
  n_mts = allocated_mts = 0;

  n = shash_count(&schema->tables);
    /* Memory is allocated with in shash_sort function*/
  nodes = shash_sort(&schema->tables);
  if (!nodes)
  {
    SB_OVSDB_DEBUG_PRINT("Failed to sort the nodes in schema");
    return;
  }
    /* Add each column in schema to monitor request*/
  for (i = 0; i < n; i++) 
  {
    struct ovsdb_table_schema *table = nodes[i]->data;

    bst_ovsdb_add_monitored_table (n_columns, 
                                   table,
                                   monitor_requests,
                                   &mts, &n_mts, &allocated_mts);
  }
    /* Free the memory allocated in shash_sort function*/
  free(nodes);

  monitor = json_array_create_3(json_string_create(database),
                                  json_null_create(), monitor_requests);
  request = jsonrpc_create_request("monitor", monitor, NULL);
  request_id = json_clone (request->id);
  jsonrpc_send(rpc, request);
 
        
  for (;;) 
  {
    while (1) 
    {
      error = jsonrpc_recv(rpc, &msg);
      if (error)
      {
        break;
      }
         /* Initial entries notified by ovsdb-server server through 
          * Message type "Reply"
          */
      if (msg->type == JSONRPC_REPLY &&
          json_equal(msg->id, request_id))
      {
        bst_ovsdb_cache_update (msg->result, mts, n_mts, true);
      }
         /* Row/Column Modify (s) are notfied by ovsdb-server through 
          * Message type "Update
          */
      else if (msg->type == JSONRPC_NOTIFY &&
               !strcmp(msg->method, "update")) 
      {
        params = msg->params;
        if (params->type == JSON_ARRAY
         && params->u.array.n == 2
         && params->u.array.elems[0]->type == JSON_NULL) 
        {
              /* extract data and update plugin cache*/
          bst_ovsdb_cache_update (params->u.array.elems[1], mts, n_mts, false);                 
        }
      }
      jsonrpc_msg_destroy(msg);
    }
    jsonrpc_run(rpc);
    jsonrpc_wait(rpc);
    jsonrpc_recv_wait(rpc);
    poll_block();
  }
}

/*********************************************************************
* @brief       Commit column "trigger_threshold" in table "bufmon" to 
*              OVSDB database.
*
* @param[in]   asic             -  ASIC ID
* @param[in]   port             -  Port 
* @param[in]   index            -  Index 
* @param[in]   bid              -  Stat ID
* @param[in]   threshold      -  Threshold.
*
* @notes       
*          
*
*
*********************************************************************/
BVIEW_STATUS bst_ovsdb_threshold_commit (int asic , int port, int index,
                                         int bid, uint64_t threshold)
{
  char   s_transact[1024] = {0};
  char   s_key[1024]       = {0};
  struct json *transaction;
  struct jsonrpc_msg *request, *reply;
  struct jsonrpc *rpc;
  char connectMode[OVSDB_CONFIG_MAX_LINE_LENGTH]; 
  BVIEW_STATUS   rv = BVIEW_STATUS_SUCCESS;

  /* Get Row name */
  rv = bst_bid_port_index_to_ovsdb_key (asic, bid, port, index, 
                                        s_key, sizeof(s_key));
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    SB_OVSDB_DEBUG_PRINT(
                         "Failed to build the key for (asic(%d) bid(%d) port(%d) index(%d) type (%s)",
                         asic, bid, port, index, "threshold");
    return BVIEW_STATUS_FAILURE;
  }
  /* Create JSON Request*/
  sprintf (s_transact, BST_OVSDB_THRESHOLD_JSON ,(unsigned long long int) threshold, s_key);

  transaction = json_from_string(s_transact);
  request = jsonrpc_create_request("transact", transaction, NULL);
  memset (&connectMode[0], 0, OVSDB_CONFIG_MAX_LINE_LENGTH);
  if (ovsdb_file_read(connectMode) != BVIEW_STATUS_SUCCESS)
  {
    /*Set default connection mode*/
    memset (&connectMode[0], 0, OVSDB_CONFIG_MAX_LINE_LENGTH);
    ovsdb_set_default(connectMode);
  } 
  rpc = open_jsonrpc (connectMode);
  if (rpc == NULL)
  { 
    SB_OVSDB_LOG (BVIEW_LOG_ERROR,
                 "Trigger_threshold commit:Failed to open JSNON RPC session");
    return BVIEW_STATUS_FAILURE;
  }
  check_txn(jsonrpc_transact_block(rpc, request, &reply),&reply);
  jsonrpc_msg_destroy(reply);
  jsonrpc_close(rpc);
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief   Commit Table "System" columns to OVSDB database.
*
* @param[in]   asic              -   ASIC ID
* @param[in]   config            -   Pointer to BST config Data.
*
* @retval      
* 
* @notes    
*          
*
*
*********************************************************************/
BVIEW_STATUS bst_ovsdb_bst_config_commit (int asic , 
                                          BVIEW_OVSDB_CONFIG_DATA_t *config)
{
  char   s_transact[1024] = {0};
  char   buf[16]          = {0};
  struct json *transaction;
  struct jsonrpc_msg *request, *reply;
  struct jsonrpc *rpc;
  char connectMode[OVSDB_CONFIG_MAX_LINE_LENGTH];
 
  /* NULL Pointer validation */
  SB_OVSDB_NULLPTR_CHECK (config, BVIEW_STATUS_INVALID_PARAMETER);

  sprintf (buf, "%d",config->collection_interval);
  /* Create JSON request*/ 
  BST_OVSDB_FORM_CONFIG_JSON (s_transact, BST_OVSDB_CONFIG_JSON_FORMAT,
                              (config->bst_enable ? "true":"false"),
                              ((config->bst_tracking_mode == BVIEW_BST_MODE_PEAK) ? "peak":"current"),
                              (config->periodic_collection ? "true":"false"), 
                              buf,
                              system_table_uuid);
                                  
  transaction = json_from_string(s_transact);
  request = jsonrpc_create_request("transact", transaction, NULL);
  memset (&connectMode[0], 0, OVSDB_CONFIG_MAX_LINE_LENGTH);
  if (ovsdb_file_read(connectMode) != BVIEW_STATUS_SUCCESS)
  {
    /*Set default connection mode*/
    memset (&connectMode[0], 0, OVSDB_CONFIG_MAX_LINE_LENGTH);
    ovsdb_set_default(connectMode);
  } 
  rpc = open_jsonrpc (connectMode);
  if (rpc == NULL)
  { 
    SB_OVSDB_LOG (BVIEW_LOG_ERROR,
                 "System config commit:Failed to open JSNON RPC session");
    return BVIEW_STATUS_FAILURE;
  }
  check_txn(jsonrpc_transact_block(rpc, request, &reply),&reply);
  jsonrpc_msg_destroy(reply);
  jsonrpc_close(rpc);
  return BVIEW_STATUS_SUCCESS;
}


