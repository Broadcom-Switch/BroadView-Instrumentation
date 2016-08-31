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

/* Include Header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "broadview.h"
#include "json.h"

#include "cJSON.h"
#include "get_bst_cgsn_drop_counters.h"


BVIEW_STATUS bst_json_cgsn_req_type_get(char *input, BVIEW_BST_CGSN_REQ_TYPE_t *val)
{
  if ((NULL == input)||
      (NULL == val))
    return BVIEW_STATUS_INVALID_PARAMETER;

  unsigned int i = 0;

  const BVIEW_BST_CGSN_DROP_REQ_MAP_t cgsn_drp_req_map[] = {
    {"top-drops", BVIEW_BST_CGSN_TOP_DROPS},
    {"top-port-queue-drops", BVIEW_BST_CGSN_TOP_PRT_Q_DROPS},
    {"port-drops", BVIEW_BST_CGSN_PRT_DROPS},
    {"port-queue-drops", BVIEW_BST_CGSN_PRT_Q_DROPS}
  };
  for (i = 0; i < (sizeof(cgsn_drp_req_map)/sizeof(BVIEW_BST_CGSN_DROP_REQ_MAP_t)); i++)
  {
    if (0 == strcmp (input, cgsn_drp_req_map[i].req_str))
    {
      *val = cgsn_drp_req_map[i].req;
      return BVIEW_STATUS_SUCCESS;
    }
  }

  return BVIEW_STATUS_FAILURE;

}

BVIEW_STATUS bst_json_cgsn_req_q_type_get(char *input, BVIEW_BST_CGSN_CTR_TYPE_t *val)
{
  if ((NULL == input)||
      (NULL == val))
    return BVIEW_STATUS_INVALID_PARAMETER;

  unsigned int i = 0;

  const BVIEW_BST_CGSN_Q_TYPE_MAP_t cgsn_drp_q_type_map[] = {
    {"ucast", BVIEW_BST_CGSN_UCAST},
    {"mcast", BVIEW_BST_CGSN_MCAST},
    {"all", BVIEW_BST_CGSN_ALL}
  };
  for (i = 0; i < (sizeof(cgsn_drp_q_type_map)/sizeof(BVIEW_BST_CGSN_Q_TYPE_MAP_t)); i++)
  {
    if (0 == strcmp (input, cgsn_drp_q_type_map[i].q_str))
    {
      *val = cgsn_drp_q_type_map[i].queue;
      return BVIEW_STATUS_SUCCESS;
    }
  }
  return BVIEW_STATUS_FAILURE;
} 


/******************************************************************
 * @brief  REST API Handler (Generated Code)
 *
 * @param[in]    cookie     Context for the API from Web server
 * @param[in]    jsonBuffer Raw Json Buffer
 * @param[in]    bufLength  Json Buffer length (bytes)
 *                           
 * @retval   BVIEW_STATUS_SUCCESS  JSON Parsed and parameters passed to BST APP
 * @retval   BVIEW_STATUS_INVALID_JSON  JSON is malformatted, or doesn't 
 * 					have necessary data.
 * @retval   BVIEW_STATUS_INVALID_PARAMETER Invalid input parameter
 *
 * @note     See the _impl() function for info passing to BST APP
 *********************************************************************/
BVIEW_STATUS bstjson_get_bst_congestion_drop_counters (void *cookie, char *jsonBuffer, int bufLength)
{

    /* Local Variables for JSON Parsing */
    cJSON *json_jsonrpc, *json_method, *json_asicId;
    cJSON *json_id,  *root, *params;
    cJSON *json_req_type, *json_req_params, *json_count, *json_ports, *json_queue;
    cJSON *json_interval, *json_port_list, *json_q_type, *json_queue_array;

    /* Local non-command-parameter JSON variable declarations */
    char jsonrpc[JSON_MAX_NODE_LENGTH] = {0};
    char method[JSON_MAX_NODE_LENGTH] = {0};
    char req_type_str[JSON_MAX_NODE_LENGTH] = {0};
    char q_type_str[JSON_MAX_NODE_LENGTH] = {0};
    int asicId = 0, id = 0, iter = 0;
    unsigned int prt= 0, queue = 0;
    unsigned int mask = 0;
    BVIEW_PORT_MASK_t port_list;
    BVIEW_QUEUE_MASK_t queue_list;
    bool valid = false;

    /* Local variable declarations */
    BVIEW_STATUS status = BVIEW_STATUS_SUCCESS;
    BSTJSON_GET_BST_CGSN_DROP_CTRS_t command;
    BVIEW_BST_CGSN_REQ_TYPE_t req_type;
    BVIEW_BST_CGSN_CTR_TYPE_t queue_type;

    memset(&command, 0, sizeof (command));

    /* Validating input parameters */

    /* Validating 'cookie' */
    JSON_VALIDATE_POINTER(cookie, "cookie", BVIEW_STATUS_INVALID_PARAMETER);

    /* Validating 'jsonBuffer' */
    JSON_VALIDATE_POINTER(jsonBuffer, "jsonBuffer", BVIEW_STATUS_INVALID_PARAMETER);

    /* Validating 'bufLength' */
    if (bufLength > strlen(jsonBuffer))
    {
        _jsonlog("Invalid value for parameter bufLength %d ", bufLength );
        return BVIEW_STATUS_INVALID_PARAMETER;
    }

    /* Parse JSON to a C-JSON root */
    root = cJSON_Parse(jsonBuffer);
    JSON_VALIDATE_JSON_POINTER(root, "root", BVIEW_STATUS_INVALID_JSON);

    /* Obtain command parameters */
    params = cJSON_GetObjectItem(root, "params");
    JSON_VALIDATE_JSON_POINTER(params, "params", BVIEW_STATUS_INVALID_JSON);

    /* Parsing and Validating 'jsonrpc' from JSON buffer */
    json_jsonrpc = cJSON_GetObjectItem(root, "jsonrpc");
    JSON_VALIDATE_JSON_POINTER(json_jsonrpc, "jsonrpc", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_STRING(json_jsonrpc, "jsonrpc", BVIEW_STATUS_INVALID_JSON);
    /* Copy the string, with a limit on max characters */
    strncpy (&jsonrpc[0], json_jsonrpc->valuestring, JSON_MAX_NODE_LENGTH - 1);
    /* Ensure that 'jsonrpc' in the JSON equals "2.0" */
    JSON_COMPARE_STRINGS_AND_CLEANUP ("jsonrpc", &jsonrpc[0], "2.0");


    /* Parsing and Validating 'method' from JSON buffer */
    json_method = cJSON_GetObjectItem(root, "method");
    JSON_VALIDATE_JSON_POINTER(json_method, "method", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_STRING(json_method, "method", BVIEW_STATUS_INVALID_JSON);
    /* Copy the string, with a limit on max characters */
    strncpy (&method[0], json_method->valuestring, JSON_MAX_NODE_LENGTH - 1);
    /* Ensure that 'method' in the JSON equals "get-bst-congestion-drop-counters" */
    JSON_COMPARE_STRINGS_AND_CLEANUP ("method", &method[0], "get-bst-congestion-drop-counters");


    /* Parsing and Validating 'asic-id' from JSON buffer */
    json_asicId = cJSON_GetObjectItem(root, "asic-id");
    JSON_VALIDATE_JSON_POINTER(json_asicId, "asic-id", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_STRING(json_asicId, "asic-id", BVIEW_STATUS_INVALID_JSON);
    /* Copy the 'asic-id' in external notation to our internal representation */
    JSON_ASIC_ID_MAP_FROM_NOTATION(asicId, json_asicId->valuestring);

    /* Parsing and Validating 'id' from JSON buffer */
    json_id = cJSON_GetObjectItem(root, "id");
    JSON_VALIDATE_JSON_POINTER(json_id, "id", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_id, "id");
    /* Copy the value */
    id = json_id->valueint;
    /* Ensure  that the number 'id' is within range of [1,100000] */
    JSON_CHECK_VALUE_AND_CLEANUP (id, 1, 100000);

    /* Parsing and Validating 'collection-interval' from JSON buffer */
    json_interval = cJSON_GetObjectItem(params, "collection-interval");
    if (NULL != json_interval)
    {
      JSON_VALIDATE_JSON_POINTER(json_interval, "collection-interval", BVIEW_STATUS_INVALID_JSON);
      JSON_VALIDATE_JSON_AS_NUMBER(json_interval, "collection-interval");
      command.intrvl = json_interval->valueint;
      JSON_CHECK_VALUE_AND_CLEANUP (command.intrvl, 0, 3600);
    }
    else
    {
      command.intrvl = 0;
    }


    /* Parsing and Validating 'request-type' from JSON buffer */
    json_req_type = cJSON_GetObjectItem(params, "request-type");
    JSON_VALIDATE_JSON_POINTER(json_req_type, "request-type", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_STRING(json_req_type, "request-type", BVIEW_STATUS_INVALID_JSON);
    strncpy(&req_type_str[0], json_req_type->valuestring, JSON_MAX_NODE_LENGTH - 1);
    status = bst_json_cgsn_req_type_get(&req_type_str[0], &req_type);
    if (BVIEW_STATUS_SUCCESS != status)
    {
      /* Free up any allocated resources and return status code */
      if (root != NULL)
      {
        cJSON_Delete(root);
      }
      return BVIEW_STATUS_INVALID_JSON;
    }
    command.req_type = req_type;
    mask = mask | BVIEW_BST_CGSN_REQ_TYPE;

    /* Parsing and Validating 'request-params' from JSON buffer */
    json_req_params = cJSON_GetObjectItem(params, "request-params");
    JSON_VALIDATE_JSON_POINTER(json_req_params, "request-params", BVIEW_STATUS_INVALID_JSON);

    /* Parsing and Validating 'request-count' from JSON buffer */
    json_count = cJSON_GetObjectItem(json_req_params, "count");

    if (NULL != json_count)
    {
      JSON_VALIDATE_JSON_POINTER(json_count, "count", BVIEW_STATUS_INVALID_JSON);
      JSON_VALIDATE_JSON_AS_NUMBER(json_count, "count");
      /* Copy the value */
      command.count = json_count->valueint;
      /* Ensure  that the number 'count' is within range of [1,100000] */
      JSON_CHECK_VALUE_AND_CLEANUP (command.count, BVIEW_BST_CGSN_COUNT_MIN, BVIEW_BST_CGSN_COUNT_MAX);
      mask = mask | BVIEW_BST_CGSN_COUNT;
    }

    /* Parsing and Validating 'port-list' from JSON buffer */
    memset(&port_list, 0, sizeof(BVIEW_PORT_MASK_t));
    json_port_list = cJSON_GetObjectItem(json_req_params, "port-list");

    if (NULL != json_port_list)
    {
        memset (&command.port_list, 0, sizeof(BVIEW_PORT_MASK_t));
      JSON_VALIDATE_JSON_POINTER(json_port_list, "port-list", BVIEW_STATUS_INVALID_JSON);
      for (iter = 0; iter < cJSON_GetArraySize(json_port_list); iter++)
      {
        json_ports = cJSON_GetArrayItem(json_port_list, iter);
        if (0 == strncmp ("all", json_ports->valuestring, strlen(json_ports->valuestring)))
        {
          if (1 < cJSON_GetArraySize(json_port_list))
          {
            /* expect only "all". Invalid JSON */
            if (root != NULL)
            {
              cJSON_Delete(root);
            }
            return BVIEW_STATUS_INVALID_JSON;
          }
          command.all_prts = true;
        }
        else
        {
          /* convert the port from string to the valid format */
          JSON_PORT_MAP_FROM_NOTATION(prt, json_ports->valuestring);
          if (0 != prt)
          {
            /* set the bit in the mask */
            BVIEW_SETMASKBIT(command.port_list, prt);
          }
        }
      }
      mask = mask | BVIEW_BST_CGSN_PORT_LIST;
    }


      /* Parsing and Validating 'request-type' from JSON buffer */
      json_q_type = cJSON_GetObjectItem(json_req_params, "queue-type");
      
      if (NULL != json_q_type)
      {
        JSON_VALIDATE_JSON_POINTER(json_q_type, "queue-type", BVIEW_STATUS_INVALID_JSON);
        JSON_VALIDATE_JSON_AS_STRING(json_q_type, "queue-type", BVIEW_STATUS_INVALID_JSON);
        strncpy(&q_type_str[0], json_q_type->valuestring, JSON_MAX_NODE_LENGTH - 1);
        status = bst_json_cgsn_req_q_type_get(&q_type_str[0], &queue_type);
        if (BVIEW_STATUS_SUCCESS != status)
        {
          /* Free up any allocated resources and return status code */
          if (root != NULL)
          {
            cJSON_Delete(root);
          }
          return BVIEW_STATUS_INVALID_JSON;
        }
        command.queue_type = queue_type;
        mask = mask | BVIEW_BST_CGSN_QUEUE_TYPE;
      }


    /* Parsing and Validating 'queue-list' from JSON buffer */
    memset(&queue_list, 0, sizeof(BVIEW_QUEUE_MASK_t));
    json_queue_array = cJSON_GetObjectItem(json_req_params, "queue-list");

    if (NULL != json_queue_array)
    {
      memset (&command.queue_list, 0, sizeof(BVIEW_QUEUE_MASK_t));
      JSON_VALIDATE_JSON_POINTER(json_queue_array, "queue-list", BVIEW_STATUS_INVALID_JSON);
      for (iter = 0; iter < cJSON_GetArraySize(json_queue_array); iter++)
      {
        json_queue = cJSON_GetArrayItem(json_queue_array, iter);
        queue = json_queue->valueint;
        queue = queue+1;

        /* add +1 to the queue and remove the same while parsing at the other end */
        if (0 != queue)
        {
          /* set the bit in the mask */
          BVIEW_SETMASKBIT(command.queue_list, queue);
          mask = mask | BVIEW_BST_CGSN_QUEUE_LIST;
        }
      }
    }

    /* validations for permitted combinations */
     if (((BVIEW_BST_CGSN_TOP_DROPS == command.req_type) && 
         (mask & BVIEW_BST_TOP_PRT_DRP_VALID) && 
         (~mask & ~BVIEW_BST_TOP_PRT_DRP_VALID)) ||
         ((BVIEW_BST_CGSN_TOP_PRT_Q_DROPS == command.req_type) && 
         (mask & BVIEW_BST_TOP_PRT_Q_DRP_VALID) && 
         (~mask & ~BVIEW_BST_TOP_PRT_Q_DRP_VALID)) ||
         ((BVIEW_BST_CGSN_PRT_DROPS == command.req_type) && 
         (mask & BVIEW_BST_PRT_DRP_VALID) && 
         (~mask & ~BVIEW_BST_PRT_DRP_VALID)) ||
         ((BVIEW_BST_CGSN_PRT_Q_DROPS == command.req_type) && 
         (mask & BVIEW_BST_PRT_Q_DRP_VALID) && 
         (~mask & ~BVIEW_BST_PRT_Q_DRP_VALID)))
      {
        valid = true;
      }

      if (true != valid)
      {
        if (root != NULL)
        {
          cJSON_Delete(root);
        }
      }

    /* Send the 'command' along with 'asicId' and 'cookie' to the Application thread. */
    status = bstjson_get_bst_cgsn_drop_counters_impl (cookie, asicId, id, &command);

    /* Free up any allocated resources and return status code */
    if (root != NULL)
    {
        cJSON_Delete(root);
    }

    return status;
}
