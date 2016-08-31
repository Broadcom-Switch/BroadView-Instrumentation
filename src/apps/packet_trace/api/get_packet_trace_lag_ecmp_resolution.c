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

#define _GNU_SOURCE
/* Include Header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "broadview.h"
#include "json.h"

#include "cJSON.h"
#include "get_packet_trace_profile.h"
#include "packet_trace_util.h"

/******************************************************************
 * @brief  REST API Handler 
 *
 * @param[in]    cookie     Context for the API from Web server
 * @param[in]    jsonBuffer Raw Json Buffer
 * @param[in]    bufLength  Json Buffer length (bytes)
 *                           
 * @retval   BVIEW_STATUS_SUCCESS  JSON Parsed and parameters passed to PT APP
 * @retval   BVIEW_STATUS_INVALID_JSON  JSON is malformatted, or doesn't 
 * 					have necessary data.
 * @retval   BVIEW_STATUS_INVALID_PARAMETER Invalid input parameter
 *
 * @note     See the _impl() function for info passing to PT APP
 *********************************************************************/
BVIEW_STATUS ptjson_get_pt_lag_resolution (void *cookie, char *jsonBuffer, int bufLength)
{

    /* Local Variables for JSON Parsing */
    cJSON *json_jsonrpc, *json_method, *json_asicId;
    cJSON *json_id, *json_packet, *json_ports_array, *json_ports, *json_droppkt;
    cJSON *json_collection_interval, *root, *params;
    cJSON *json_src_ip, *json_dst_ip, *json_protocol, *json_src_port, *json_dst_port, *json_pkt_limit; 
    unsigned int tuple_mask = 0, wild_card_cnt = 0, *ip = NULL;
    bool wild_card = false;

    /* Local non-command-parameter JSON variable declarations */
    char jsonrpc[JSON_MAX_NODE_LENGTH] = {0};
    char method[JSON_MAX_NODE_LENGTH] = {0};
    char ports[256] = {0};
    int asicId = 0, id = 0, iter = 0;

    char src_ip_str[JSON_MAX_NODE_LENGTH] = {0};
    char dst_ip_str[JSON_MAX_NODE_LENGTH] = {0};
    char protocol_str[JSON_MAX_NODE_LENGTH] = {0};
    char src_port_str[JSON_MAX_NODE_LENGTH] = {0};
    char dst_port_str[JSON_MAX_NODE_LENGTH] = {0};
    int protocol = 0, src_port = 0, dst_port = 0;

    /* Local variable declarations */
    BVIEW_STATUS status = BVIEW_STATUS_SUCCESS;
    PTJSON_GET_PT_PROFILE_t command;
    BVIEW_PORT_MASK_t temp_pbmp;

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
    /* Ensure that 'method' in the JSON equals "get-packet-trace-profile" */
    JSON_COMPARE_STRINGS_AND_CLEANUP ("method", &method[0], "get-packet-trace-lag-resolution");


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

  /* source - ip */
  json_src_ip = cJSON_GetObjectItem(params, "src-ip");
  if (NULL != json_src_ip)
  {
    wild_card = false;
    JSON_VALIDATE_JSON_POINTER(json_src_ip, "src-ip", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_STRING(json_src_ip, "src-ip", BVIEW_STATUS_INVALID_JSON);
    strncpy (&src_ip_str[0], json_src_ip->valuestring, JSON_MAX_NODE_LENGTH - 1);
    BVIEW_COMPARE_WILDCARD(src_ip_str, wild_card);
    if (false == wild_card)
    {
      ip = &command.req_prfl.tuple.src_ip;
      if (inet_pton(AF_INET, src_ip_str, ip) != 1)
      {
        if (root != NULL)
        {
          cJSON_Delete(root);
        }
        return BVIEW_STATUS_INVALID_JSON;
      }
      /* set the mask */
      tuple_mask = (tuple_mask | PT_CONFIG_TUPLE_SRC_IP);
    }
    else
    {
      wild_card_cnt++;
      if(BVIEW_PT_MAX_WC < wild_card_cnt)
      {
        if (root != NULL)
        {
          cJSON_Delete(root);
        }
        return BVIEW_STATUS_INVALID_JSON;
      }
    }

    /* destination - ip */
    wild_card = false;
    json_dst_ip = cJSON_GetObjectItem(params, "dst-ip");
    JSON_VALIDATE_JSON_POINTER(json_dst_ip, "dst-ip", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_STRING(json_dst_ip, "dst-ip", BVIEW_STATUS_INVALID_JSON);
    strncpy (&dst_ip_str[0], json_dst_ip->valuestring, JSON_MAX_NODE_LENGTH - 1);
    BVIEW_COMPARE_WILDCARD(dst_ip_str, wild_card);
    if (false == wild_card)
    {
      ip = &command.req_prfl.tuple.dst_ip;
      if (inet_pton(AF_INET, dst_ip_str, ip) != 1)
      {
        if (root != NULL)
        {
          cJSON_Delete(root);
        }
        return BVIEW_STATUS_INVALID_JSON;
      }
      /* set the mask */
      tuple_mask = (tuple_mask | PT_CONFIG_TUPLE_DST_IP);
    }
    else
    {
      wild_card_cnt++;
      if(BVIEW_PT_MAX_WC < wild_card_cnt)
      {
        if (root != NULL)
        {
          cJSON_Delete(root);
        }
        return BVIEW_STATUS_INVALID_JSON;
      }
    }

    /* protocol */
    wild_card = false;
    json_protocol = cJSON_GetObjectItem(params, "protocol");
    JSON_VALIDATE_JSON_POINTER(json_protocol, "protocol", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_STRING(json_protocol, "protocol", BVIEW_STATUS_INVALID_JSON);
    strncpy (&protocol_str[0], json_protocol->valuestring, JSON_MAX_NODE_LENGTH - 1);
    BVIEW_COMPARE_WILDCARD(protocol_str, wild_card);
    if (false == wild_card)
    {
      protocol = atoi(protocol_str);
      if ((protocol < BVIEW_PT_MIN_PROTOCOL || 
            protocol > BVIEW_PT_MAX_PROTOCOL))
      {
        if (root != NULL)
        {
          cJSON_Delete(root);
        }
        return BVIEW_STATUS_INVALID_JSON;
      }
      /* set the mask */
      command.req_prfl.tuple.protocol = protocol;
      tuple_mask = (tuple_mask | PT_CONFIG_TUPLE_PROTOCOL);
    }
    else
    {
      wild_card_cnt++;
      if(BVIEW_PT_MAX_WC < wild_card_cnt)
      {
        if (root != NULL)
        {
          cJSON_Delete(root);
        }
        return BVIEW_STATUS_INVALID_JSON;
      }
    }

    /* src-port */
    wild_card = false;
    json_src_port = cJSON_GetObjectItem(params, "l4-src-port");
    JSON_VALIDATE_JSON_POINTER(json_src_port, "l4-src-port", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_STRING(json_src_port, "l4-src-port", BVIEW_STATUS_INVALID_JSON);
    strncpy (&src_port_str[0], json_src_port->valuestring, JSON_MAX_NODE_LENGTH - 1);
    BVIEW_COMPARE_WILDCARD(src_port_str, wild_card);
    if (false == wild_card)
    {
      src_port = atoi(src_port_str);
      if ((src_port < BVIEW_PT_MIN_SRC_PORT || 
            src_port > BVIEW_PT_MAX_SRC_PORT))
      {
        if (root != NULL)
        {
          cJSON_Delete(root);
        }
        return BVIEW_STATUS_INVALID_JSON;
      }
      /* set the mask */
      command.req_prfl.tuple.src_port = src_port;
      tuple_mask = (tuple_mask | PT_CONFIG_TUPLE_SRC_PORT);
    }
    else
    {
      wild_card_cnt++;
      if(BVIEW_PT_MAX_WC < wild_card_cnt)
      {
        if (root != NULL)
        {
          cJSON_Delete(root);
        }
        return BVIEW_STATUS_INVALID_JSON;
      }
    }


    /* dst-port */
    wild_card = false;
    json_dst_port = cJSON_GetObjectItem(params, "l4-dst-port");
    JSON_VALIDATE_JSON_POINTER(json_dst_port, "l4-dst-port", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_STRING(json_dst_port, "l4-dst-port", BVIEW_STATUS_INVALID_JSON);
    strncpy (&dst_port_str[0], json_dst_port->valuestring, JSON_MAX_NODE_LENGTH - 1);
    BVIEW_COMPARE_WILDCARD(dst_port_str, wild_card);
    if (false == wild_card)
    {
      dst_port = atoi(dst_port_str);
      if ((dst_port < BVIEW_PT_MIN_DST_PORT || 
            dst_port > BVIEW_PT_MAX_DST_PORT))
      {
        if (root != NULL)
        {
          cJSON_Delete(root);
        }
        return BVIEW_STATUS_INVALID_JSON;
      }
      /* set the mask */
      command.req_prfl.tuple.dst_port = dst_port;
      tuple_mask = (tuple_mask | PT_CONFIG_TUPLE_DST_PORT);
    }
    else
    {
      wild_card_cnt++;
      if(BVIEW_PT_MAX_WC < wild_card_cnt)
      {
        if (root != NULL)
        {
          cJSON_Delete(root);
        }
        return BVIEW_STATUS_INVALID_JSON;
      }
    }

    command.req_prfl.tuple.tuple_mask = tuple_mask;

    /* packet-limit */
    json_pkt_limit = cJSON_GetObjectItem(params, "packet-limit");
    JSON_VALIDATE_JSON_POINTER(json_pkt_limit, "packet-limit", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_pkt_limit, "packet-limit");
    JSON_CHECK_VALUE_AND_CLEANUP (json_pkt_limit->valueint, BVIEW_PT_MIN_PACKET_LIMIT, BVIEW_PT_MAX_PACKET_LIMIT);
    command.pkt_limit = json_pkt_limit->valueint;

    command.req_method = PT_5_TUPLE;

  }
  else
  {
    /* Parsing and Validating 'include-ingress-port-priority-group' from JSON buffer */
    json_packet = cJSON_GetObjectItem(params, "packet");
    JSON_VALIDATE_JSON_POINTER(json_packet, "packet", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_STRING(json_packet, "packet", BVIEW_STATUS_INVALID_JSON);
    if (PT_JSON_MAX_PKT_LEN < strlen(json_packet->valuestring))
    {
      _jsonlog("length (%llu) of the input packet string in json buffer"
          "is larger than the supported length (%d) ", (long long unsigned)strlen(json_packet->valuestring), PT_JSON_MAX_PKT_LEN);
      return BVIEW_STATUS_INVALID_PARAMETER;
    }
    strncpy (&command.req_prfl.pkt.packet[0], json_packet->valuestring, strlen(json_packet->valuestring));
    command.req_method = PT_PKT;
    command.req_prfl.pkt.packet_len = strlen(json_packet->valuestring);
  }
    /* Parsing and Validating 'include-ingress-port-priority-group' from JSON buffer */
    json_ports_array = cJSON_GetObjectItem(params, "port-list");
    JSON_VALIDATE_JSON_POINTER(json_ports_array, "port-list", BVIEW_STATUS_INVALID_JSON);

    memset(&command.pbmp, 0, sizeof(BVIEW_PORT_MASK_t));

    for (iter = 0; iter < cJSON_GetArraySize(json_ports_array); iter++)
    {
      memset (ports, 0, 128);
      memset (&temp_pbmp, 0, sizeof(BVIEW_PORT_MASK_t));
      json_ports = cJSON_GetArrayItem(json_ports_array, iter);
      strncpy (&ports[0], json_ports->valuestring, strlen(json_ports->valuestring));
      PT_PORT_LIST_TO_MASK_CONVERT(ports, &temp_pbmp);
      BVIEW_MASKOREQ (command.pbmp, temp_pbmp, sizeof(BVIEW_PORT_MASK_t));
    }


    /* Parsing and Validating "collection-interval" from JSON buffer */
    json_collection_interval = cJSON_GetObjectItem(params, "collection-interval");
    JSON_VALIDATE_JSON_POINTER(json_collection_interval, "collection-interval", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_collection_interval, "collection-interval");
    /* Copy the value */
    command.collection_interval = json_collection_interval->valueint;
    /* Ensure  that the number 'collection interval' is within range of [0,120] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.collection_interval, 0, 120);

    /* Parsing and Validating 'droppkt' from JSON buffer */
    json_droppkt = cJSON_GetObjectItem(params, "drop-packet");
    JSON_VALIDATE_JSON_POINTER(json_droppkt, "drop-packet", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_droppkt, "drop-packet");
    /* Copy the value */
    command.drop_packet = json_droppkt->valueint;
    /* Ensure  that the number 'include-device' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.drop_packet, 0, 1);


    /* Send the 'command' along with 'asicId' and 'cookie' to the Application thread. */
    status = ptjson_get_pt_lag_resolution_impl (cookie, asicId, id, &command);

    /* Free up any allocated resources and return status code */
    if (root != NULL)
    {
        cJSON_Delete(root);
    }

    return status;
}



/******************************************************************
 * @brief  REST API Handler 
 *
 * @param[in]    cookie     Context for the API from Web server
 * @param[in]    jsonBuffer Raw Json Buffer
 * @param[in]    bufLength  Json Buffer length (bytes)
 *                           
 * @retval   BVIEW_STATUS_SUCCESS  JSON Parsed and parameters passed to PT APP
 * @retval   BVIEW_STATUS_INVALID_JSON  JSON is malformatted, or doesn't 
 * 					have necessary data.
 * @retval   BVIEW_STATUS_INVALID_PARAMETER Invalid input parameter
 *
 * @note     See the _impl() function for info passing to PT APP
 *********************************************************************/
BVIEW_STATUS ptjson_get_pt_ecmp_resolution (void *cookie, char *jsonBuffer, int bufLength)
{

  /* Local Variables for JSON Parsing */
  cJSON *json_jsonrpc, *json_method, *json_asicId;
  cJSON *json_id, *json_packet, *json_ports_array, *json_ports, *json_droppkt;
  cJSON *json_collection_interval, *root, *params;
  cJSON *json_src_ip, *json_dst_ip, *json_protocol, *json_src_port, *json_dst_port, *json_pkt_limit; 
  unsigned int tuple_mask = 0, wild_card_cnt = 0, *ip = NULL;
  bool wild_card = false;

  /* Local non-command-parameter JSON variable declarations */
  char jsonrpc[JSON_MAX_NODE_LENGTH] = {0};
  char method[JSON_MAX_NODE_LENGTH] = {0};
  char ports[128] = {0};
  int asicId = 0, id = 0, iter = 0;
  char src_ip_str[JSON_MAX_NODE_LENGTH] = {0};
  char dst_ip_str[JSON_MAX_NODE_LENGTH] = {0};
  char protocol_str[JSON_MAX_NODE_LENGTH] = {0};
  char src_port_str[JSON_MAX_NODE_LENGTH] = {0};
  char dst_port_str[JSON_MAX_NODE_LENGTH] = {0};
  int protocol = 0, src_port = 0, dst_port = 0;

  /* Local variable declarations */
  BVIEW_STATUS status = BVIEW_STATUS_SUCCESS;
  PTJSON_GET_PT_PROFILE_t command;
  BVIEW_PORT_MASK_t temp_pbmp;

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
  /* Ensure that 'method' in the JSON equals "get-packet-trace-ecmp-resolution" */
  JSON_COMPARE_STRINGS_AND_CLEANUP ("method", &method[0], "get-packet-trace-ecmp-resolution");


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

  /* source - ip */
  json_src_ip = cJSON_GetObjectItem(params, "src-ip");
  if (NULL != json_src_ip)
  {
    wild_card = false;
    JSON_VALIDATE_JSON_POINTER(json_src_ip, "src-ip", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_STRING(json_src_ip, "src-ip", BVIEW_STATUS_INVALID_JSON);
    strncpy (&src_ip_str[0], json_src_ip->valuestring, JSON_MAX_NODE_LENGTH - 1);
    BVIEW_COMPARE_WILDCARD(src_ip_str, wild_card);
    if (false == wild_card)
    {
      ip = &command.req_prfl.tuple.src_ip;
      if (inet_pton(AF_INET, src_ip_str, ip) != 1)
      {
        if (root != NULL)
        {
          cJSON_Delete(root);
        }
        return BVIEW_STATUS_INVALID_JSON;
      }
      /* set the mask */
      tuple_mask = (tuple_mask | PT_CONFIG_TUPLE_SRC_IP);
    }
    else
    {
      wild_card_cnt++;
      if(BVIEW_PT_MAX_WC < wild_card_cnt)
      {
        if (root != NULL)
        {
          cJSON_Delete(root);
        }
        return BVIEW_STATUS_INVALID_JSON;
      }
    }

    /* destination - ip */
    wild_card = false;
    json_dst_ip = cJSON_GetObjectItem(params, "dst-ip");
    JSON_VALIDATE_JSON_POINTER(json_dst_ip, "dst-ip", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_STRING(json_dst_ip, "dst-ip", BVIEW_STATUS_INVALID_JSON);
    strncpy (&dst_ip_str[0], json_dst_ip->valuestring, JSON_MAX_NODE_LENGTH - 1);
    BVIEW_COMPARE_WILDCARD(dst_ip_str, wild_card);
    if (false == wild_card)
    {
      ip = &command.req_prfl.tuple.dst_ip;
      if (inet_pton(AF_INET, dst_ip_str, ip) != 1)
      {
        if (root != NULL)
        {
          cJSON_Delete(root);
        }
        return BVIEW_STATUS_INVALID_JSON;
      }
      /* set the mask */
      tuple_mask = (tuple_mask | PT_CONFIG_TUPLE_DST_IP);
    }
    else
    {
      wild_card_cnt++;
      if(BVIEW_PT_MAX_WC < wild_card_cnt)
      {
        if (root != NULL)
        {
          cJSON_Delete(root);
        }
        return BVIEW_STATUS_INVALID_JSON;
      }
    }

    /* protocol */
    wild_card = false;
    json_protocol = cJSON_GetObjectItem(params, "protocol");
    JSON_VALIDATE_JSON_POINTER(json_protocol, "protocol", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_STRING(json_protocol, "protocol", BVIEW_STATUS_INVALID_JSON);
    strncpy (&protocol_str[0], json_protocol->valuestring, JSON_MAX_NODE_LENGTH - 1);
    BVIEW_COMPARE_WILDCARD(protocol_str, wild_card);
    if (false == wild_card)
    {
      protocol = atoi(protocol_str);
      if ((protocol < BVIEW_PT_MIN_PROTOCOL || 
            protocol > BVIEW_PT_MAX_PROTOCOL))
      {
        if (root != NULL)
        {
          cJSON_Delete(root);
        }
        return BVIEW_STATUS_INVALID_JSON;
      }
      /* set the mask */
      command.req_prfl.tuple.protocol = protocol;
      tuple_mask = (tuple_mask | PT_CONFIG_TUPLE_PROTOCOL);
    }
    else
    {
      wild_card_cnt++;
      if(BVIEW_PT_MAX_WC < wild_card_cnt)
      {
        if (root != NULL)
        {
          cJSON_Delete(root);
        }
        return BVIEW_STATUS_INVALID_JSON;
      }
    }

    /* src-port */
    wild_card = false;
    json_src_port = cJSON_GetObjectItem(params, "l4-src-port");
    JSON_VALIDATE_JSON_POINTER(json_src_port, "l4-src-port", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_STRING(json_src_port, "l4-src-port", BVIEW_STATUS_INVALID_JSON);
    strncpy (&src_port_str[0], json_src_port->valuestring, JSON_MAX_NODE_LENGTH - 1);
    BVIEW_COMPARE_WILDCARD(src_port_str, wild_card);
    if (false == wild_card)
    {
      src_port = atoi(src_port_str);
      if ((src_port < BVIEW_PT_MIN_SRC_PORT || 
            src_port > BVIEW_PT_MAX_SRC_PORT))
      {
        if (root != NULL)
        {
          cJSON_Delete(root);
        }
        return BVIEW_STATUS_INVALID_JSON;
      }
      /* set the mask */
      command.req_prfl.tuple.src_port = src_port;
      tuple_mask = (tuple_mask | PT_CONFIG_TUPLE_SRC_PORT);
    }
    else
    {
      wild_card_cnt++;
      if(BVIEW_PT_MAX_WC < wild_card_cnt)
      {
        if (root != NULL)
        {
          cJSON_Delete(root);
        }
        return BVIEW_STATUS_INVALID_JSON;
      }
    }


    /* dst-port */
    wild_card = false;
    json_dst_port = cJSON_GetObjectItem(params, "l4-dst-port");
    JSON_VALIDATE_JSON_POINTER(json_dst_port, "l4-dst-port", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_STRING(json_dst_port, "l4-dst-port", BVIEW_STATUS_INVALID_JSON);
    strncpy (&dst_port_str[0], json_dst_port->valuestring, JSON_MAX_NODE_LENGTH - 1);
    BVIEW_COMPARE_WILDCARD(dst_port_str, wild_card);
    if (false == wild_card)
    {
      dst_port = atoi(dst_port_str);
      if ((dst_port < BVIEW_PT_MIN_DST_PORT || 
            dst_port > BVIEW_PT_MAX_DST_PORT))
      {
        if (root != NULL)
        {
          cJSON_Delete(root);
        }
        return BVIEW_STATUS_INVALID_JSON;
      }
      /* set the mask */
      command.req_prfl.tuple.dst_port = dst_port;
      tuple_mask = (tuple_mask | PT_CONFIG_TUPLE_DST_PORT);
    }
    else
    {
      wild_card_cnt++;
      if(BVIEW_PT_MAX_WC < wild_card_cnt)
      {
        if (root != NULL)
        {
          cJSON_Delete(root);
        }
        return BVIEW_STATUS_INVALID_JSON;
      }
    }

    command.req_prfl.tuple.tuple_mask = tuple_mask;

    /* packet-limit */
    json_pkt_limit = cJSON_GetObjectItem(params, "packet-limit");
    JSON_VALIDATE_JSON_POINTER(json_pkt_limit, "packet-limit", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_pkt_limit, "packet-limit");
    JSON_CHECK_VALUE_AND_CLEANUP (json_pkt_limit->valueint, BVIEW_PT_MIN_PACKET_LIMIT, BVIEW_PT_MAX_PACKET_LIMIT);
    command.pkt_limit = json_pkt_limit->valueint;

    command.req_method = PT_5_TUPLE;

  }
  else
  {
    /* Parsing and Validating 'include-ingress-port-priority-group' from JSON buffer */
    json_packet = cJSON_GetObjectItem(params, "packet");
    JSON_VALIDATE_JSON_POINTER(json_packet, "packet", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_STRING(json_packet, "packet", BVIEW_STATUS_INVALID_JSON);
    if (PT_JSON_MAX_PKT_LEN < strlen(json_packet->valuestring))
    {
      _jsonlog("length (%llu) of the input packet string in json buffer"
          "is larger than the supported length (%d) ", (long long unsigned)strlen(json_packet->valuestring), PT_JSON_MAX_PKT_LEN);
      return BVIEW_STATUS_INVALID_PARAMETER;
    }
    strncpy (&command.req_prfl.pkt.packet[0], json_packet->valuestring, strlen(json_packet->valuestring));
    command.req_method = PT_PKT;
    command.req_prfl.pkt.packet_len = strlen(json_packet->valuestring);
  }

  /* Parsing and Validating 'include-ingress-port-priority-group' from JSON buffer */
  json_ports_array = cJSON_GetObjectItem(params, "port-list");
  JSON_VALIDATE_JSON_POINTER(json_ports_array, "port-list", BVIEW_STATUS_INVALID_JSON);


  memset(&command.pbmp, 0, sizeof(BVIEW_PORT_MASK_t));
  for (iter = 0; iter < cJSON_GetArraySize(json_ports_array); iter++)
  {
    memset (ports, 0, 128);
    memset (&temp_pbmp, 0, sizeof(BVIEW_PORT_MASK_t));
    json_ports = cJSON_GetArrayItem(json_ports_array, iter);

    strncpy (&ports[0], json_ports->valuestring, strlen(json_ports->valuestring));
    PT_PORT_LIST_TO_MASK_CONVERT(ports, &temp_pbmp);
    BVIEW_MASKOREQ (command.pbmp, temp_pbmp, sizeof(BVIEW_PORT_MASK_t));
  }

  /* Parsing and Validating "collection-interval" from JSON buffer */
  json_collection_interval = cJSON_GetObjectItem(params, "collection-interval");
  JSON_VALIDATE_JSON_POINTER(json_collection_interval, "collection-interval", BVIEW_STATUS_INVALID_JSON);
  JSON_VALIDATE_JSON_AS_NUMBER(json_collection_interval, "collection-interval");
  /* Copy the value */
  command.collection_interval = json_collection_interval->valueint;
  /* Ensure  that the number 'collection interval' is within range of [0,120] */
  JSON_CHECK_VALUE_AND_CLEANUP (command.collection_interval, 0, 120);

  /* Parsing and Validating 'droppkt' from JSON buffer */
  json_droppkt = cJSON_GetObjectItem(params, "drop-packet");
  JSON_VALIDATE_JSON_POINTER(json_droppkt, "drop-packet", BVIEW_STATUS_INVALID_JSON);
  JSON_VALIDATE_JSON_AS_NUMBER(json_droppkt, "drop-packet");
  /* Copy the value */
  command.drop_packet = json_droppkt->valueint;
  /* Ensure  that the number 'include-device' is within range of [0,1] */
  JSON_CHECK_VALUE_AND_CLEANUP (command.drop_packet, 0, 1);


  /* Send the 'command' along with 'asicId' and 'cookie' to the Application thread. */
  status = ptjson_get_pt_ecmp_resolution_impl (cookie, asicId, id, &command);

  /* Free up any allocated resources and return status code */
  if (root != NULL)
  {
    cJSON_Delete(root);
  }

  return status;
}
