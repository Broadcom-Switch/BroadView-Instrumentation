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
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include "example_server.h"
#include <errno.h>
#include <arpa/inet.h>
#include "cJSON.h"
#include "example_app_config_api.h"

#define EXAMPLE_MAX_BUFFER_LEN 1024

extern int rest_agent_config_params_modify(char *ipaddr, unsigned int clientPort); 

static const example_api_handler_map_t example_api_list[] = {
  {"vlan-create", example_app_config_vlan_create},
  {"vlan-delete", example_app_config_vlan_destroy},
  {"vlan-member-add", example_app_config_vlan_member_add},
  {"vlan-member-delete", example_app_config_vlan_member_delete},
  {"lag-create", example_app_config_lag},
  {"lag-delete", example_app_delete_lag},
  {"lag-member-add", example_app_config_lag_member},
  {"lag-member-delete", example_app_config_lag_member_remove},
  {"l3intf-create", example_app_config_l3_intf_add},
  {"l3intf-delete", example_app_config_l3_intf_delete},
  {"route-add", example_app_config_l3_route_add},
  {"route-delete", example_app_config_l3_route_delete},
  {"arp-add", example_app_config_l3_arp_add},
  {"arp-delete", example_app_config_l3_arp_delete},
  {"client-params-modify", example_app_config_agent_config_params_modify}
};

EXAMPLE_APP_CONFIG_t example_app_config;




/******************************************************************
 *  @brief      Function to get the handler for the config type 
 *  
 *  @param[in] type - char pointer to the command type 
 *  @param[out] handler -  function pointer for the corresponsing config type 
 *  
 *  @retval int 
 * 
 *  @note
 *  *********************************************************************/

int example_api_handler_get(char *type, example_api_handler_t *handler)
{
  unsigned int i = 0;

  for (i = 0; i < CONFIG_TYPE_MAX; i++)
  {
    if (0 == strcmp(type, example_api_list[i].type))
    {
      *handler = example_api_list[i].handler;
      return 0;
    }
  }
  return -1;
}

/******************************************************************
 * @brief      Function to convert MAC string to MAC value 
 *
 * @param[in]  macAddrStr  Holds the MAC address string to be converted
 * @param[in]  macAddr     To hold the MAC address
 *
 * @retval    0         If MAC string is correct   
 *           -1         If MAC string is incorrect 
 *
 * @note     
 *********************************************************************/
static int convertMacStrToMacValue(char *macAddrStr, macAddr_t *macAddr)
{
  int data[6];
  int i;

  if( (6 == sscanf(macAddrStr, "%x:%x:%x:%x:%x:%x", &data[0], &data[1], &data[2],
	                                              &data[3], &data[4], &data[5])) )
  {
    /* Convert to uint8_t */
    for( i = 0; i < 6; ++i )
    {
       macAddr->addr[i] = (uint8_t) data[i];
    }
    return 0;
  }
  else
  {
     /* Invalid mac address*/
    printf("EXAMPLE_APP: invalid MAC address\n");
    return -1;
  }
}


/******************************************************************
 *  @brief     Function to process the incoming command request 
 *  
 *  @param[in] buffer -  data buffer read from client 
 *  @param[in] length -  number of bytes read  
 *  
 *  @retval int 
 * 
 *  @note
 *  *********************************************************************/
int process_input(char *buffer, int length)
{
  char *buff_ptr, *json;
  example_api_handler_t handler;
  char jsonrpc[JSON_MAX_NODE_LENGTH] = {0};
  char method[JSON_MAX_NODE_LENGTH] = {0}; 
  cJSON *root, *params, *json_jsonrpc, *json_method;
  int len = 0;

  if (0 == length)
  {
    printf("no bytes read. error !!\n");
    return -1;
  }

  json = strstr(buffer, "\r\n\r\n");

  if (NULL == json)
  {
    /* the http header is not present.
       assume that the passed buffer is json.
       if not the validation would fail */

    json = buffer;
  }

  buff_ptr = json;
  len = strlen(json);

  /* Parse JSON to a C-JSON root */
  root = cJSON_Parse(buff_ptr);
  JSON_VALIDATE_JSON_POINTER(root, "root", -1);

  /* Obtain command parameters */
  params = cJSON_GetObjectItem(root, "params");
  JSON_VALIDATE_JSON_POINTER(params, "params", -1);

  /* Parsing and Validating 'jsonrpc' from JSON buffer */
  json_jsonrpc = cJSON_GetObjectItem(root, "jsonrpc");
  JSON_VALIDATE_JSON_POINTER(json_jsonrpc, "jsonrpc", -1);
  JSON_VALIDATE_JSON_AS_STRING(json_jsonrpc, "jsonrpc", -1);
  /* Copy the string, with a limit on max characters */
  strncpy (&jsonrpc[0], json_jsonrpc->valuestring, JSON_MAX_NODE_LENGTH - 1);
  /* Ensure that 'jsonrpc' in the JSON equals "2.0" */
  JSON_COMPARE_STRINGS_AND_CLEANUP ("jsonrpc", &jsonrpc[0], "2.0");


  /* Parsing and Validating 'method' from JSON buffer */
  json_method = cJSON_GetObjectItem(root, "command");
  JSON_VALIDATE_JSON_POINTER(json_method, "command", -1);
  JSON_VALIDATE_JSON_AS_STRING(json_method, "command", -1);
  /* Copy the string, with a limit on max characters */
  strncpy (&method[0], json_method->valuestring, JSON_MAX_NODE_LENGTH - 1);

  if (-1 == example_api_handler_get(&method[0], &handler))
  {
    if (root != NULL)
    {
      cJSON_Delete(root);
    }
    printf("Unable to find the corresponding function for the input type %s\n", &method[0]);
    return -1;
  }

  return handler(len, buff_ptr);
}
/******************************************************************
 *  @brief      Function to create VLAN
 *  
 * @param[in]  len - length of data buffer 
 * @param[in]  input data buffer
 *  
 *  @retval   0 - success, -1 failure 
 * 
 *  @note
 *  *********************************************************************/

int example_app_config_vlan_create(int len, char *buffer)
{
  unsigned int vid = 0;
  unsigned int unit = 0;
  cJSON *root, *params, *json_vid;


  /* Validating 'length' */
  if (len > strlen(buffer))
  {
    printf("Invalid value for parameter length %d ", len );
    return -1;
  }

  /* Parse JSON to a C-JSON root */
  root = cJSON_Parse(buffer);
  JSON_VALIDATE_JSON_POINTER(root, "root", -1);

  /* Obtain command parameters */
  params = cJSON_GetObjectItem(root, "params");
  JSON_VALIDATE_JSON_POINTER(params, "params", -1);


  /* Parsing and Validating 'id' from JSON buffer */
  json_vid = cJSON_GetObjectItem(params, "vid");
  JSON_VALIDATE_JSON_POINTER(json_vid, "vid", -1);
  JSON_VALIDATE_JSON_AS_NUMBER(json_vid, "vid");
  /* Copy the value */
  vid = json_vid->valueint;
  /* Ensure  that the number 'id' is within range of [1,100000] */
  JSON_CHECK_VALUE_AND_CLEANUP (vid, 1, 4094);

  if (0 != EXAMPLE_APP_VLAN_CREATE(unit, vid))
    printf("Failed to create vlan %d\r\n", vid);

  if (root != NULL)
  {
    cJSON_Delete(root);
  }
  return 0;
}


/******************************************************************
 *  @brief      Function to destroy VLAN
 * 
 * @param[in]  len - length of data buffer 
 * @param[in]  input data buffer
 *  
 *  @retval   0 - success, -1 failure 
 * 
 *  @note
 **********************************************************************/
int example_app_config_vlan_destroy(int len, char *buffer)
{
  unsigned int vid = 0;
  unsigned int unit = 0;
  cJSON *root, *params, *json_vid;

  /* Validating 'length' */
  if (len > strlen(buffer))
  {
    printf("Invalid value for parameter length %d ", len );
    return -1;
  }

  /* Parse JSON to a C-JSON root */
  root = cJSON_Parse(buffer);
  JSON_VALIDATE_JSON_POINTER(root, "root", -1);

  /* Obtain command parameters */
  params = cJSON_GetObjectItem(root, "params");
  JSON_VALIDATE_JSON_POINTER(params, "params", -1);


  /* Parsing and Validating 'id' from JSON buffer */
  json_vid = cJSON_GetObjectItem(params, "vid");
  JSON_VALIDATE_JSON_POINTER(json_vid, "vid", -1);
  JSON_VALIDATE_JSON_AS_NUMBER(json_vid, "vid");
  /* Copy the value */
  vid = json_vid->valueint;
  /* Ensure  that the number 'id' is within range of [1,100000] */
  JSON_CHECK_VALUE_AND_CLEANUP (vid, 1, 4094);


  /* call the api to destroy the VLAN
   *    */
  if(0 != EXAMPLE_APP_VLAN_DESTROY(unit, vid))
    printf("Failed to destroy vlan %d\r\n", vid);

  if (root != NULL)
  {
    cJSON_Delete(root);
  }
  return 0;
}

/******************************************************************
 * @brief      Function to add member to VLAN
 *
 * @param[in]  len - length of data buffer 
 * @param[in]  input data buffer
 *  
 *  @retval   0 - success, -1 failure 
 * 
 * @retval   none
 * 
 * @note
 **********************************************************************/
int example_app_config_vlan_member_add(int len, char *buffer)
{
  unsigned int unit = 0;
  unsigned int vid = 0;
  unsigned int port = 0;
  int rv = 0;
  cJSON *root, *params, *json_vid, *json_port;

  /* Validating 'length' */
  if (len > strlen(buffer))
  {
    printf("Invalid value for parameter length %d ", len );
    return -1;
  }

  /* Parse JSON to a C-JSON root */
  root = cJSON_Parse(buffer);
  JSON_VALIDATE_JSON_POINTER(root, "root", -1);

  /* Obtain command parameters */
  params = cJSON_GetObjectItem(root, "params");
  JSON_VALIDATE_JSON_POINTER(params, "params", -1);


  /* Parsing and Validating 'id' from JSON buffer */
  json_vid = cJSON_GetObjectItem(params, "vid");
  JSON_VALIDATE_JSON_POINTER(json_vid, "vid", -1);
  JSON_VALIDATE_JSON_AS_NUMBER(json_vid, "vid");
  /* Copy the value */
  vid = json_vid->valueint;
  /* Ensure  that the number 'id' is within range of [1,100000] */
  JSON_CHECK_VALUE_AND_CLEANUP (vid, 1, 4094);

  /* Parsing and Validating 'id' from JSON buffer */
  json_port = cJSON_GetObjectItem(params, "port");
  JSON_VALIDATE_JSON_POINTER(json_port, "port", -1);
  JSON_VALIDATE_JSON_AS_NUMBER(json_port, "port");
  /* Copy the value */
  port = json_port->valueint;
  /* Ensure  that the number 'id' is within range of [1,100000] */
  JSON_CHECK_VALUE_AND_CLEANUP (port, 1, 48);

  /* call the api to add the VLAN member
   *      */
  rv = EXAMPLE_APP_VLAN_MEMBER_ADD(unit, vid, port);
  if (0 != rv)
    printf("Failed to add member port %d to  vlan %d, rv = %d\r\n", port, vid, rv);

  if (root != NULL)
  {
    cJSON_Delete(root);
  }
  return 0;
}


/******************************************************************
 * @brief      Function to delete member from VLAN
 * 
 * @param[in]  len - length of data buffer 
 * @param[in]  input data buffer
 *  
 *  @retval   0 - success, -1 failure 
 * 
 * @note
 * *********************************************************************/
int example_app_config_vlan_member_delete(int len, char *buffer)
{
  unsigned int unit = 0;
  unsigned int vid = 0;
  unsigned int port = 0;
  cJSON *root, *params, *json_vid, *json_port;

  /* Validating 'length' */
  if (len > strlen(buffer))
  {
    printf("Invalid value for parameter length %d ", len );
    return -1;
  }

  /* Parse JSON to a C-JSON root */
  root = cJSON_Parse(buffer);
  JSON_VALIDATE_JSON_POINTER(root, "root", -1);

  /* Obtain command parameters */
  params = cJSON_GetObjectItem(root, "params");
  JSON_VALIDATE_JSON_POINTER(params, "params", -1);


  /* Parsing and Validating 'id' from JSON buffer */
  json_vid = cJSON_GetObjectItem(params, "vid");
  JSON_VALIDATE_JSON_POINTER(json_vid, "vid", -1);
  JSON_VALIDATE_JSON_AS_NUMBER(json_vid, "vid");
  /* Copy the value */
  vid = json_vid->valueint;
  /* Ensure  that the number 'id' is within range of [1,100000] */
  JSON_CHECK_VALUE_AND_CLEANUP (vid, 1, 4094);

  /* Parsing and Validating 'id' from JSON buffer */
  json_port = cJSON_GetObjectItem(params, "port");
  JSON_VALIDATE_JSON_POINTER(json_port, "port", -1);
  JSON_VALIDATE_JSON_AS_NUMBER(json_port, "port");
  /* Copy the value */
  port = json_port->valueint;
  /* Ensure  that the number 'id' is within range of [1,100000] */
  JSON_CHECK_VALUE_AND_CLEANUP (port, 1, 48);

  /* call the api to destroy the VLAN member
   *      */
  if (0 != EXAMPLE_APP_VLAN_MEMBER_DELETE(unit, vid, port))
    printf("Failed to delete member port %d from vlan %d\r\n", port, vid);

  if (root != NULL)
  {
    cJSON_Delete(root);
  }
  return 0;

}
/******************************************************************
 * @brief      Function to configure LAG id
 * 
 * @param[in]  len - length of data buffer 
 * @param[in]  input data buffer
 *  
 *  @retval   0 - success, -1 failure 
 * 
 * @note
 **********************************************************************/

int example_app_config_lag(int len, char *buffer)
{
  unsigned int unit = 0;
  unsigned int lag_id = 0;
  cJSON *root, *params, *json_lag_id;

  /* Validating 'length' */
  if (len > strlen(buffer))
  {
    printf("Invalid value for parameter length %d ", len );
    return -1;
  }

  /* Parse JSON to a C-JSON root */
  root = cJSON_Parse(buffer);
  JSON_VALIDATE_JSON_POINTER(root, "root", -1);

  /* Obtain command parameters */
  params = cJSON_GetObjectItem(root, "params");
  JSON_VALIDATE_JSON_POINTER(params, "params", -1);


  /* Parsing and Validating 'id' from JSON buffer */
  json_lag_id = cJSON_GetObjectItem(params, "lag-id");
  JSON_VALIDATE_JSON_POINTER(json_lag_id, "lag-id", -1);
  JSON_VALIDATE_JSON_AS_NUMBER(json_lag_id, "lag-id");
  /* Copy the value */
  lag_id = json_lag_id->valueint;
  /* Ensure  that the number 'id' is within range of [1,100000] */
  JSON_CHECK_VALUE_AND_CLEANUP (lag_id, 1, EXAMPLE_APP_MAX_LAG);

  /* call the api to add lag id
   *      */
  if (0 != EXAMPLE_APP_TRUNK_CREATE(unit, lag_id))
    printf("Failed to create trunk id  %d\r\n", lag_id);


  if (root != NULL)
  {
    cJSON_Delete(root);
  }
  return 0;
}


/******************************************************************
 *  @brief      Function to delete LAG id
 * 
 * @param[in]  len - length of data buffer 
 * @param[in]  input data buffer
 *  
 *  @retval   0 - success, -1 failure 
 * 
 *  @note
 **********************************************************************/
int example_app_delete_lag(int len, char *buffer)
{
  unsigned int unit = 0;
  unsigned int lag_id = 0;
  cJSON *root, *params, *json_lag_id;

  /* Validating 'length' */
  if (len > strlen(buffer))
  {
    printf("Invalid value for parameter length %d ", len );
    return -1;
  }

  /* Parse JSON to a C-JSON root */
  root = cJSON_Parse(buffer);
  JSON_VALIDATE_JSON_POINTER(root, "root", -1);

  /* Obtain command parameters */
  params = cJSON_GetObjectItem(root, "params");
  JSON_VALIDATE_JSON_POINTER(params, "params", -1);


  /* Parsing and Validating 'id' from JSON buffer */
  json_lag_id = cJSON_GetObjectItem(params, "lag-id");
  JSON_VALIDATE_JSON_POINTER(json_lag_id, "lag-id", -1);
  JSON_VALIDATE_JSON_AS_NUMBER(json_lag_id, "lag-id");
  /* Copy the value */
  lag_id = json_lag_id->valueint;
  /* Ensure  that the number is within range  */
  JSON_CHECK_VALUE_AND_CLEANUP (lag_id, 1, EXAMPLE_APP_MAX_LAG);

  /* call the api to add lag id
   *      */
  if (0 != EXAMPLE_APP_TRUNK_DESTROY(unit, lag_id))
    printf("Failed to destroy trunk id  %d\r\n", lag_id);

  if (root != NULL)
  {
    cJSON_Delete(root);
  }
  return 0;
}


/******************************************************************
 * @brief      Function to add member to LAG
 * 
 * @param[in]  len - length of data buffer 
 * @param[in]  input data buffer
 *  
 *  @retval   0 - success, -1 failure 
 * 
 * @note
 **********************************************************************/
int example_app_config_lag_member(int len, char *buffer)
{
  unsigned int unit = 0;
  unsigned int lag_id = 0;
  unsigned int lag_member = 0;
  cJSON *root, *params, *json_lag_id, *json_lag_member;

  /* Validating 'length' */
  if (len > strlen(buffer))
  {
    printf("Invalid value for parameter length %d ", len );
    return -1;
  }

  /* Parse JSON to a C-JSON root */
  root = cJSON_Parse(buffer);
  JSON_VALIDATE_JSON_POINTER(root, "root", -1);

  /* Obtain command parameters */
  params = cJSON_GetObjectItem(root, "params");
  JSON_VALIDATE_JSON_POINTER(params, "params", -1);


  /* Parsing and Validating 'id' from JSON buffer */
  json_lag_id = cJSON_GetObjectItem(params, "lag-id");
  JSON_VALIDATE_JSON_POINTER(json_lag_id, "lag-id", -1);
  JSON_VALIDATE_JSON_AS_NUMBER(json_lag_id, "lag-id");
  /* Copy the value */
  lag_id = json_lag_id->valueint;
  /* Ensure  that the number is within range  */
  JSON_CHECK_VALUE_AND_CLEANUP (lag_id, 1, EXAMPLE_APP_MAX_LAG);


  /* Parsing and Validating 'id' from JSON buffer */
  json_lag_member = cJSON_GetObjectItem(params, "member-port");
  JSON_VALIDATE_JSON_POINTER(json_lag_member, "member-port", -1);
  JSON_VALIDATE_JSON_AS_NUMBER(json_lag_member, "member-port");
  /* Copy the value */
  lag_member = json_lag_member->valueint;
  /* Ensure  that the number is within range  */
  JSON_CHECK_VALUE_AND_CLEANUP (lag_member, 1, 48);

  /* call the api to add lag member
   *      */
  if (0 != EXAMPLE_APP_TRUNK_MEMBER_ADD(unit, lag_id, lag_member))
    printf("Failed to add port %d to  trunk id  %d\r\n", lag_member, lag_id);

  if (root != NULL)
  {
    cJSON_Delete(root);
  }
  return 0;
}


/******************************************************************
 *@brief      Function to delete member to LAG
 *
 * @param[in]  len - length of data buffer 
 * @param[in]  input data buffer
 *  
 *  @retval   0 - success, -1 failure 
 *
 * @note
 **********************************************************************/
int example_app_config_lag_member_remove(int len, char *buffer)
{
  unsigned int unit = 0;
  unsigned int lag_id = 0;
  unsigned int lag_member = 0;
  cJSON *root, *params, *json_lag_id, *json_lag_member;

  /* Validating 'length' */
  if (len > strlen(buffer))
  {
    printf("Invalid value for parameter length %d ", len );
    return -1;
  }

  /* Parse JSON to a C-JSON root */
  root = cJSON_Parse(buffer);
  JSON_VALIDATE_JSON_POINTER(root, "root", -1);

  /* Obtain command parameters */
  params = cJSON_GetObjectItem(root, "params");
  JSON_VALIDATE_JSON_POINTER(params, "params", -1);


  /* Parsing and Validating 'id' from JSON buffer */
  json_lag_id = cJSON_GetObjectItem(params, "lag-id");
  JSON_VALIDATE_JSON_POINTER(json_lag_id, "lag-id", -1);
  JSON_VALIDATE_JSON_AS_NUMBER(json_lag_id, "lag-id");
  /* Copy the value */
  lag_id = json_lag_id->valueint;
  /* Ensure  that the number is within range  */
  JSON_CHECK_VALUE_AND_CLEANUP (lag_id, 1, EXAMPLE_APP_MAX_LAG);


  /* Parsing and Validating 'id' from JSON buffer */
  json_lag_member = cJSON_GetObjectItem(params, "member-port");
  JSON_VALIDATE_JSON_POINTER(json_lag_member, "member-port", -1);
  JSON_VALIDATE_JSON_AS_NUMBER(json_lag_member, "member-port");
  /* Copy the value */
  lag_member = json_lag_member->valueint;
  /* Ensure  that the number is within range  */
  JSON_CHECK_VALUE_AND_CLEANUP (lag_member, 1, 48);


  /* call the api to remove member from lag
   *      */
  if (0 != EXAMPLE_APP_TRUNK_MEMBER_DELETE(unit, lag_id, lag_member))
    printf("Failed to delete port %d to  trunk id  %d\r\n", lag_member, lag_id);

  if (root != NULL)
  {
    cJSON_Delete(root);
  }
  return 0;
}

/******************************************************************
 *@brief      Function to configure L3 Interface
 *
 * @param[in]  len - length of data buffer 
 * @param[in]  input data buffer
 *  
 *  @retval   0 - success, -1 failure 
 *
 * @note
 **********************************************************************/

int example_app_config_l3_intf_add(int len, char *buffer)
{
  unsigned int unit = 0;
  macAddr_t my_mac_addr;
  unsigned int ipaddr;
  unsigned int mask;
  unsigned int in_vid;
  unsigned int port;
  char macStr[JSON_MAX_NODE_LENGTH];
  char ipStr[JSON_MAX_NODE_LENGTH];
  char maskStr[JSON_MAX_NODE_LENGTH];
  cJSON *root, *params, *json_vid, *json_port, *json_mac;
  cJSON *json_ipaddr, *json_ipmask;

  /* Validating 'length' */
  if (len > strlen(buffer))
  {
    printf("Invalid value for parameter length %d ", len );
    return -1;
  }

  /* Parse JSON to a C-JSON root */
  root = cJSON_Parse(buffer);
  JSON_VALIDATE_JSON_POINTER(root, "root", -1);

  /* Obtain command parameters */
  params = cJSON_GetObjectItem(root, "params");
  JSON_VALIDATE_JSON_POINTER(params, "params", -1);


  /* Parsing and Validating 'id' from JSON buffer */
  json_vid = cJSON_GetObjectItem(params, "vid");
  JSON_VALIDATE_JSON_POINTER(json_vid, "vid", -1);
  JSON_VALIDATE_JSON_AS_NUMBER(json_vid, "vid");
  /* Copy the value */
  in_vid = json_vid->valueint;
  /* Ensure  that the number is within range  */
  JSON_CHECK_VALUE_AND_CLEANUP (in_vid, 1, 4094);


  /* Parsing and Validating 'id' from JSON buffer */
  json_port = cJSON_GetObjectItem(params, "member-port");
  JSON_VALIDATE_JSON_POINTER(json_port, "member-port", -1);
  JSON_VALIDATE_JSON_AS_NUMBER(json_port, "member-port");
  /* Copy the value */
  port = json_port->valueint;
  /* Ensure  that the number is within range  */
  JSON_CHECK_VALUE_AND_CLEANUP (port, 1, 48);


  /* Parsing and Validating 'macStr' from JSON buffer */
  json_mac = cJSON_GetObjectItem(params, "mac-addr");
  JSON_VALIDATE_JSON_POINTER(json_mac, "mac-addr", -1);
  JSON_VALIDATE_JSON_AS_STRING(json_mac, "mac-addr", -1);
  /* Copy the string, with a limit on max characters */
  strncpy (&macStr[0], json_mac->valuestring, JSON_MAX_NODE_LENGTH - 1);
  convertMacStrToMacValue(macStr, &my_mac_addr);

  /* Parsing and Validating 'ipStr' from JSON buffer */
  json_ipaddr = cJSON_GetObjectItem(params, "ip-addr");
  JSON_VALIDATE_JSON_POINTER(json_ipaddr, "ip-addr", -1);
  JSON_VALIDATE_JSON_AS_STRING(json_ipaddr, "ip-addr", -1);
  /* Copy the string, with a limit on max characters */
  strncpy (&ipStr[0], json_ipaddr->valuestring, JSON_MAX_NODE_LENGTH - 1);
  if (inet_pton(AF_INET, ipStr, &ipaddr) != 1)
  {
    if (root != NULL)
    {
      cJSON_Delete(root);
    }
    return -1;
  }


  /* Parsing and Validating 'maskStr' from JSON buffer */
  json_ipmask = cJSON_GetObjectItem(params,"subnet-mask");
  JSON_VALIDATE_JSON_POINTER(json_ipmask,"subnet-mask", -1);
  JSON_VALIDATE_JSON_AS_STRING(json_ipmask,"subnet-mask", -1);
  /* Copy the string, with a limit on max characters */
  strncpy (&maskStr[0], json_ipmask->valuestring, JSON_MAX_NODE_LENGTH - 1);
  if (inet_pton(AF_INET, maskStr, &mask) != 1)
  {
    if (root != NULL)
    {
      cJSON_Delete(root);
    }
    return -1;
  }


  /* call the api to create the l3 interface
   *      */
  if ( 0 != EXAMPLE_APP_L3_INTF_ADD(unit, in_vid, port, &my_mac_addr,
        ipaddr, mask))
    printf("Failed to create routing interface with id %d \r\n", in_vid);

  /* Free up any allocated resources and return status code */
  if (root != NULL)
  {
    cJSON_Delete(root);
  }

  return 0;
}


/******************************************************************
 * @brief      Function to delete L3 Interface
 *
 * @param[in]  len - length of data buffer 
 * @param[in]  input data buffer
 *  
 *  @retval   0 - success, -1 failure 
 *
 * @note
 **********************************************************************/
int example_app_config_l3_intf_delete(int len, char *buffer)
{
  unsigned int in_vid;
  cJSON *root, *params, *json_vid;

  /* Validating 'length' */
  if (len > strlen(buffer))
  {
    printf("Invalid value for parameter length %d ", len );
    return -1;
  }

  /* Parse JSON to a C-JSON root */
  root = cJSON_Parse(buffer);
  JSON_VALIDATE_JSON_POINTER(root, "root", -1);

  /* Obtain command parameters */
  params = cJSON_GetObjectItem(root, "params");
  JSON_VALIDATE_JSON_POINTER(params, "params", -1);


  /* Parsing and Validating 'vid' from JSON buffer */
  json_vid = cJSON_GetObjectItem(params, "vid");
  JSON_VALIDATE_JSON_POINTER(json_vid, "vid", -1);
  JSON_VALIDATE_JSON_AS_NUMBER(json_vid, "vid");
  /* Copy the value */
  in_vid = json_vid->valueint;
  /* Ensure  that the number is within range  */
  JSON_CHECK_VALUE_AND_CLEANUP (in_vid, 1, 4094);

  /* call the api to delete the l3 interface
   *      */
  if ( 0 != EXAMPLE_APP_L3_INTF_DELETE(in_vid))
    printf("Failed to delete routing interface with id %d \r\n", in_vid);

  if (root != NULL)
  {
    cJSON_Delete(root);
  }
  return 0;
}


/******************************************************************
 * @brief      Function to add route
 *
 * @param[in]  len - length of data buffer 
 * @param[in]  input data buffer
 *  
 *  @retval   0 - success, -1 failure 
 *
 * @note
 **********************************************************************/
int example_app_config_l3_route_add(int len, char *buffer)
{
  unsigned int ipaddr = 0, subnet_mask = 0, next_hop;
  char ipStr[JSON_MAX_NODE_LENGTH];
  char maskStr[JSON_MAX_NODE_LENGTH];
  char nexthopStr[JSON_MAX_NODE_LENGTH];
  cJSON *root, *params;
  cJSON *json_ipaddr, *json_ipmask, *json_nexthop;

  /* Validating 'length' */
  if (len > strlen(buffer))
  {
    printf("Invalid value for parameter length %d ", len );
    return -1;
  }

  /* Parse JSON to a C-JSON root */
  root = cJSON_Parse(buffer);
  JSON_VALIDATE_JSON_POINTER(root, "root", -1);

  /* Obtain command parameters */
  params = cJSON_GetObjectItem(root, "params");
  JSON_VALIDATE_JSON_POINTER(params, "params", -1);

  /* Parsing and Validating 'ipStr' from JSON buffer */
  json_ipaddr = cJSON_GetObjectItem(params, "route");
  JSON_VALIDATE_JSON_POINTER(json_ipaddr, "route", -1);
  JSON_VALIDATE_JSON_AS_STRING(json_ipaddr, "route", -1);
  /* Copy the string, with a limit on max characters */
  strncpy (&ipStr[0], json_ipaddr->valuestring, JSON_MAX_NODE_LENGTH - 1);
  if (inet_pton(AF_INET, ipStr, &ipaddr) != 1)
  {
    if (root != NULL)
    {
      cJSON_Delete(root);
    }
    return -1;
  }


  /* Parsing and Validating 'maskStr' from JSON buffer */
  json_ipmask = cJSON_GetObjectItem(params,"mask");
  JSON_VALIDATE_JSON_POINTER(json_ipmask,"mask", -1);
  JSON_VALIDATE_JSON_AS_STRING(json_ipmask,"mask", -1);
  /* Copy the string, with a limit on max characters */
  strncpy (&maskStr[0], json_ipmask->valuestring, JSON_MAX_NODE_LENGTH - 1);
  if (inet_pton(AF_INET, maskStr, &subnet_mask) != 1)
  {
    if (root != NULL)
    {
      cJSON_Delete(root);
    }
    return -1;
  }


  /* Parsing and Validating 'maskStr' from JSON buffer */
  json_nexthop = cJSON_GetObjectItem(params,"next-hop");
  JSON_VALIDATE_JSON_POINTER(json_nexthop,"next-hop", -1);
  JSON_VALIDATE_JSON_AS_STRING(json_nexthop,"next-hop", -1);
  /* Copy the string, with a limit on max characters */
  strncpy (&nexthopStr[0], json_nexthop->valuestring, JSON_MAX_NODE_LENGTH - 1);
  if (inet_pton(AF_INET, nexthopStr, &next_hop) != 1)
  {
    if (root != NULL)
    {
      cJSON_Delete(root);
    }
    return -1;
  }
  /* call the api to add l3 route
   *      */
  if (0 != EXAMPLE_APP_L3_ROUTE_ADD(ipaddr, subnet_mask, next_hop))
    printf("Failed to add route \r\n");

  if (root != NULL)
  {
    cJSON_Delete(root);
  }
  return 0;
}



/******************************************************************
 * @brief      Function to delete route
 * 
 * @param[in]  len - length of data buffer 
 * @param[in]  input data buffer
 *  
 *  @retval   0 - success, -1 failure 
 * 
 *  @note
 * *********************************************************************/

int example_app_config_l3_route_delete(int len, char *buffer)
{
  unsigned int ipaddr = 0, subnet_mask = 0, next_hop = 0;
  char ipStr[JSON_MAX_NODE_LENGTH];
  char maskStr[JSON_MAX_NODE_LENGTH];
  char nexthopStr[JSON_MAX_NODE_LENGTH];
  cJSON *root, *params;
  cJSON *json_ipaddr, *json_ipmask, *json_nexthop;

  /* Validating 'length' */
  if (len > strlen(buffer))
  {
    printf("Invalid value for parameter length %d ", len );
    return -1;
  }

  /* Parse JSON to a C-JSON root */
  root = cJSON_Parse(buffer);
  JSON_VALIDATE_JSON_POINTER(root, "root", -1);

  /* Obtain command parameters */
  params = cJSON_GetObjectItem(root, "params");
  JSON_VALIDATE_JSON_POINTER(params, "params", -1);

  /* Parsing and Validating 'ipStr' from JSON buffer */
  json_ipaddr = cJSON_GetObjectItem(params, "route");
  JSON_VALIDATE_JSON_POINTER(json_ipaddr, "route", -1);
  JSON_VALIDATE_JSON_AS_STRING(json_ipaddr, "route", -1);
  /* Copy the string, with a limit on max characters */
  strncpy (&ipStr[0], json_ipaddr->valuestring, JSON_MAX_NODE_LENGTH - 1);
  if (inet_pton(AF_INET, ipStr, &ipaddr) != 1)
  {
    if (root != NULL)
    {
      cJSON_Delete(root);
    }
    return -1;
  }


  /* Parsing and Validating 'maskStr' from JSON buffer */
  json_ipmask = cJSON_GetObjectItem(params,"mask");
  JSON_VALIDATE_JSON_POINTER(json_ipmask,"mask", -1);
  JSON_VALIDATE_JSON_AS_STRING(json_ipmask,"mask", -1);
  /* Copy the string, with a limit on max characters */
  strncpy (&maskStr[0], json_ipmask->valuestring, JSON_MAX_NODE_LENGTH - 1);
  if (inet_pton(AF_INET, maskStr, &subnet_mask) != 1)
  {
    if (root != NULL)
    {
      cJSON_Delete(root);
    }
    return -1;
  }


  /* Parsing and Validating 'maskStr' from JSON buffer */
  json_nexthop = cJSON_GetObjectItem(params,"next-hop");
  JSON_VALIDATE_JSON_POINTER(json_nexthop,"next-hop", -1);
  JSON_VALIDATE_JSON_AS_STRING(json_nexthop,"next-hop", -1);
  /* Copy the string, with a limit on max characters */
  strncpy (&nexthopStr[0], json_nexthop->valuestring, JSON_MAX_NODE_LENGTH - 1);
  if (inet_pton(AF_INET, nexthopStr, &next_hop) != 1)
  {
    if (root != NULL)
    {
      cJSON_Delete(root);
    }
    return -1;
  }

  /* call the api to delete l3 route
   *      */
  if (0 != EXAMPLE_APP_L3_ROUTE_DELETE(ipaddr, subnet_mask, next_hop))
    printf("Failed to delete route \r\n");

  if (root != NULL)
  {
    cJSON_Delete(root);
  }
  return 0;
}



/******************************************************************
 * @brief      Function to add arp
 *
 * @param[in]  len - length of data buffer 
 * @param[in]  input data buffer
 *  
 *  @retval   0 - success, -1 failure 
 * 
 * @note
 **********************************************************************/

int example_app_config_l3_arp_add(int len, char *buffer)
{
  unsigned int ipaddr = 0;
  macAddr_t mac_addr;
  char macStr[JSON_MAX_NODE_LENGTH];
  char ipStr[JSON_MAX_NODE_LENGTH];
  cJSON *root, *params, *json_mac;
  cJSON *json_ipaddr;

  /* Validating 'length' */
  if (len > strlen(buffer))
  {
    printf("Invalid value for parameter length %d ", len );
    return -1;
  }

  /* Parse JSON to a C-JSON root */
  root = cJSON_Parse(buffer);
  JSON_VALIDATE_JSON_POINTER(root, "root", -1);

  /* Obtain command parameters */
  params = cJSON_GetObjectItem(root, "params");
  JSON_VALIDATE_JSON_POINTER(params, "params", -1);
  /* Parsing and Validating 'ipStr' from JSON buffer */
  json_ipaddr = cJSON_GetObjectItem(params, "ipaddr");
  JSON_VALIDATE_JSON_POINTER(json_ipaddr, "ipaddr", -1);
  JSON_VALIDATE_JSON_AS_STRING(json_ipaddr, "ipaddr", -1);


  json_mac = cJSON_GetObjectItem(params, "mac-addr");
  JSON_VALIDATE_JSON_POINTER(json_mac, "mac-addr", -1);
  JSON_VALIDATE_JSON_AS_STRING(json_mac, "mac-addr", -1);
  /* Copy the string, with a limit on max characters */
  strncpy (&macStr[0], json_mac->valuestring, JSON_MAX_NODE_LENGTH - 1);
  convertMacStrToMacValue(macStr, &mac_addr);


  /* Copy the string, with a limit on max characters */
  strncpy (&ipStr[0], json_ipaddr->valuestring, JSON_MAX_NODE_LENGTH - 1);
  if (inet_pton(AF_INET, ipStr, &ipaddr) != 1)
  {
    if (root != NULL)
    {
      cJSON_Delete(root);
    }
    return -1;
  }

  if (0 != EXAMPLE_APP_L3_ARP_ADD(0, ipaddr, &mac_addr))
    printf("Failed to add arp \r\n");

  if (root != NULL)
  {
    cJSON_Delete(root);
  }
  return 0;
}


/******************************************************************
 * @brief      Function to delete arp
 *
 * @param[in]  len - length of data buffer 
 * @param[in]  input data buffer
 *  
 *  @retval   0 - success, -1 failure 
 *
 *  @note
 **********************************************************************/

int example_app_config_l3_arp_delete(int len, char *buffer)
{
  unsigned int ipaddr = 0;
  char ipStr[JSON_MAX_NODE_LENGTH];
  cJSON *root, *params, *json_ipaddr;

  /* Validating 'length' */
  if (len > strlen(buffer))
  {
    printf("Invalid value for parameter length %d ", len );
    return -1;
  }

  /* Parse JSON to a C-JSON root */
  root = cJSON_Parse(buffer);
  JSON_VALIDATE_JSON_POINTER(root, "root", -1);

  /* Obtain command parameters */
  params = cJSON_GetObjectItem(root, "params");
  JSON_VALIDATE_JSON_POINTER(params, "params", -1);

  /* Parsing and Validating 'ipStr' from JSON buffer */
  json_ipaddr = cJSON_GetObjectItem(params, "ipaddr");
  JSON_VALIDATE_JSON_POINTER(json_ipaddr, "ipaddr", -1);
  JSON_VALIDATE_JSON_AS_STRING(json_ipaddr, "ipaddr", -1);
  /* Copy the string, with a limit on max characters */
  strncpy (&ipStr[0], json_ipaddr->valuestring, JSON_MAX_NODE_LENGTH - 1);
  if (inet_pton(AF_INET, ipStr, &ipaddr) != 1)
  {
    if (root != NULL)
    {
      cJSON_Delete(root);
    }
    return -1;
  }
  if (0 != EXAMPLE_APP_L3_ARP_DELETE(0, ipaddr))
    printf("Failed to add arp \r\n");

  if (root != NULL)
  {
    cJSON_Delete(root);
  }
  return 0;
}

/******************************************************************
 * @brief      Function to  modify the client ipaddr and port in agent
 *
 * @param[in]  len - length of data buffer 
 * @param[in]  input data buffer
 *  
 *  @retval   0 - success, -1 failure 
 *
 * @note
 **********************************************************************/
int example_app_config_agent_config_params_modify(int len, char *buffer)
{
  unsigned int port = 0;
  char ipStr[JSON_MAX_NODE_LENGTH];
  cJSON *root, *params;
  cJSON *json_ipaddr, *json_port;

  /* Validating 'length' */
  if (len > strlen(buffer))
  {
    printf("Invalid value for parameter length %d ", len );
    return -1;
  }

  /* Parse JSON to a C-JSON root */
  root = cJSON_Parse(buffer);
  JSON_VALIDATE_JSON_POINTER(root, "root", -1);

  /* Obtain command parameters */
  params = cJSON_GetObjectItem(root, "params");
  JSON_VALIDATE_JSON_POINTER(params, "params", -1);

  /* Parsing and Validating 'ipStr' from JSON buffer */
  json_ipaddr = cJSON_GetObjectItem(params, "ipaddr");
  JSON_VALIDATE_JSON_POINTER(json_ipaddr, "ipaddr", -1);
  JSON_VALIDATE_JSON_AS_STRING(json_ipaddr, "ipaddr", -1);
  /* Copy the string, with a limit on max characters */
  strncpy (&ipStr[0], json_ipaddr->valuestring, JSON_MAX_NODE_LENGTH - 1);

  /* Parsing and Validating 'id' from JSON buffer */
  json_port = cJSON_GetObjectItem(params, "port");
  JSON_VALIDATE_JSON_POINTER(json_port, "port", -1);
  JSON_VALIDATE_JSON_AS_NUMBER(json_port, "port");
  /* Copy the value */
  port = json_port->valueint;

  /* call the api to modify the client ipaddr and port in agent */
  rest_agent_config_params_modify(ipStr, port);

   if (root != NULL)
   {
     cJSON_Delete(root);
   }
  return 0;
}


/******************************************************************
 * @brief  Sets the configuration, to defaults.
 *
 * @param[in] void 
 *                           
 * @retval   0  when configuration is initialized successfully
 *
 * @note     
 *********************************************************************/

static int example_app_config_set_defaults(void)
{
    memset(&example_app_config, 0, sizeof (EXAMPLE_APP_CONFIG_t));

    /* setup default server port */
    example_app_config.serverPort = EXAMPLE_APP_CONFIG_PROPERTY_SERVER_PORT_DEFAULT;

    return 0;
}

/******************************************************************
 * @brief  Reads configuration from a file.
 *
 * @param[in] void 
 *                           
 * @retval   0  when configuration is initialized successfully
 *                           
 * @note     
 *********************************************************************/

static int example_app_config_read(void)
{
  FILE *configFile;
  char line[256] = { 0 };
  int numLinesRead = 0;

  int temp;

  /* for string manipulation */
  char *property, *value;

  memset(&example_app_config, 0, sizeof (EXAMPLE_APP_CONFIG_t));

  /* open the file. if file not available/readable, return appropriate error */
  configFile = fopen(EXAMPLE_APP_CONFIG_FILE, "r");

  if (configFile == NULL)
  {
    printf("example app Configuration file %s not found: \n",
        EXAMPLE_APP_CONFIG_FILE);
    return -1;
  }

  /* read the lines one-by-one. if any of the lines is corrupted 
   * i.e., doesn't contain valid tokens, return error 
   */

  while (numLinesRead < 1)
  {
    memset (&line[0], 0, 256);

    /* read one line from the file */
    property = fgets(&line[0], 256, configFile);
    if (property == NULL)
    {  
      fclose(configFile);
      return -1;
    }

    numLinesRead++;

    /* split the line into tokens, based on the file format */
    property = strtok(&line[0], "=");
    if (property == NULL)
    {
      fclose(configFile);
      return -1;
    }
    value = property + strlen(property) + 1;

    /* Is this token the server port number ?*/
    if (strcmp(property, EXAMPLE_APP_CONFIG_PROPERTY_SERVER_PORT) == 0)
    {
      /* is this port number valid ? */
      temp = strtol(value, NULL, 10);
      if (errno == ERANGE)
      {
        fclose(configFile);
        return -1;
      }

      /* copy the server port number */
      example_app_config.serverPort = temp;
      continue;
    }

    fclose(configFile);
    return -1;
  }

  printf("example_config : Using configuration :%d \n",
      example_app_config.serverPort);

  fclose(configFile);

  return 0;
}

/******************************************************************
 * @brief  Initializes configuration, reads it from file or assumes defaults.
 *
 * @param[in]  none 
 *                           
 * @retval   0  when configuration is initialized successfully
 *
 * @note     
 *********************************************************************/
int example_app_config_init(void)
{
    int status;


    /* aim to read */
    status = example_app_config_read();
    if (status != 0)
    {
        example_app_config_set_defaults();
    }
    return 0;
}


/******************************************************************
 * @brief      Function to read data received from client using socket 
 *
 * @param[in]  filedes : socket
 *
 * @retval   none
 *
 *  @note
 **********************************************************************/

int read_from_client (int filedes)
{
  char buffer[1024];
  int nbytes;

  nbytes = read (filedes, buffer, EXAMPLE_MAX_BUFFER_LEN);
  if (nbytes < 0)
  {
    /* Read error. */
    perror ("read");
    return -1;
  }
  else if (nbytes == 0)
    /* End-of-file. */
    return -1;
  else
  {
    /* Data read. */
    return process_input(buffer, nbytes);
  }
}

int example_server_main (void) 
{
  int socket_fd;
  fd_set active_fd_set, read_fd_set;
  int i;
  struct sockaddr_in clientname;
  struct sockaddr_in serverAddr;
  socklen_t size;


  socket_fd = socket(PF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1)
  {
    perror("Socket");
    return -1;
  }

  example_app_config_init();
  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(example_app_config.serverPort);

  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);


  /*---- Bind the address struct to the socket ----*/
  if (bind(socket_fd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1)
  {
    perror ("bind");
    close(socket_fd);
    return -1;
  }

  if (listen (socket_fd, 5) < 0)
  {
    perror ("listen");
    close(socket_fd);
    return -1;
  }

  /* Initialize the set of active sockets. */
  FD_ZERO (&active_fd_set);
  FD_SET (socket_fd, &active_fd_set);

  while (1)
  {
    /* Block until input arrives on one or more active sockets. */
    read_fd_set = active_fd_set;
    if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
    {
      perror ("select");
      return -1;
    }

    /* Service all the sockets with input pending. */
    for (i = 0; i < FD_SETSIZE; ++i)
      if (FD_ISSET (i, &read_fd_set))
      {
        if (i == socket_fd)
        {
          /* Connection request on original socket. */
          int new;
          size = sizeof (clientname);
          new = accept (socket_fd,
              (struct sockaddr *) &clientname,
              &size);
          if (new < 0)
          {
            perror ("accept");
            return -1;
          }
          fprintf (stderr,
              "Server: connect from host %s, port %hd.\n",
              inet_ntoa (clientname.sin_addr),
              ntohs (clientname.sin_port));
          FD_SET (new, &active_fd_set);
        }
        else
        {
          /* Data arriving on an already-connected socket. */
          if (read_from_client (i) < 0)
          {
            close (i);
            FD_CLR (i, &active_fd_set);
          }
        }
      }
  }
}

