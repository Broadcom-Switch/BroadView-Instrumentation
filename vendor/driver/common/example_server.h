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
#ifndef INCLUDE_EXAMPLE_SERVER_H
#define INCLUDE_EXAMPLE_SERVER_H

#ifdef  __cplusplus
extern "C"
{
#endif

#include <stdint.h> 
#include <sys/socket.h> 

typedef enum example_config_type {
  CONFIG_TYPE_VLAN_CREATE = 0,
  CONFIG_TYPE_VLAN_DELETE,
  CONFIG_TYPE_VLAN_MEMBER_ADD,
  CONFIG_TYPE_VLAN_MEMBER_DELETE,
  CONFIG_TYPE_LAG,
  CONFIG_TYPE_LAG_DELETE,
  CONFIG_TYPE_LAG_MEMBER_ADD,
  CONFIG_TYPE_LAG_MEMBER_DELETE,
  CONFIG_TYPE_L3_INTF_ADD,
  CONFIG_TYPE_L3_INTF_DELETE,
  CONFIG_TYPE_L3_ROUTE_ADD,
  CONFIG_TYPE_L3_ROUTE_DELETE,
  CONFIG_TYPE_L3_ARP_ADD,
  CONFIG_TYPE_L3_ARP_DELETE,
  CONFIG_TYPE_MAX
} example_config_type_t;

typedef enum example_data_type {
  UNSIGNED_INT = 1,
  MAC_ADDR
} example_data_type_t;


typedef int(*example_api_handler_t) (int len, char *buffer);

typedef struct example_api_handler_map_s
{
  char *type;
  example_api_handler_t handler;
}example_api_handler_map_t;

typedef struct macAddr_s
{
  char  addr[6];
}macAddr_t;

#define EXAMPLE_APP_MAX_IP_ADDR_LENGTH    20

/* file from where the configuration properties are read. */
#define EXAMPLE_APP_CONFIG_FILE    "example_app_config.cfg"

#define EXAMPLE_APP_CONFIG_PROPERTY_SERVER_PORT   "example_app_server_port"
#define EXAMPLE_APP_CONFIG_PROPERTY_SERVER_PORT_DEFAULT  7891 


typedef struct _example_app_config_s_ 
{
  unsigned int serverPort;
} EXAMPLE_APP_CONFIG_t;

#define EXAMPLE_APP_VLAN_ARGS 1
#define EXAMPLE_APP_VLAN_CREATE_DELETE_ARGS 1
#define EXAMPLE_APP_VLAN_PORT_ADD_ARGS 2
#define EXAMPLE_APP_VLAN_PORT_DELETE_ARGS 2
#define EXAMPLE_APP_CREATE_LAG_ARGS 1
#define EXAMPLE_APP_DELETE_LAG_ARGS 1
#define EXAMPLE_APP_LAG_MEMBER_ADD_ARGS 2
#define EXAMPLE_APP_LAG_MEMBER_DELETE_ARGS 2
#define EXAMPLE_APP_L3_INTF_ADD_ARGS 5
#define EXAMPLE_APP_L3_INTF_DELETE_ARGS 1
#define EXAMPLE_APP_L3_ROUTE_ADD_ARGS 3
#define EXAMPLE_APP_L3_ROUTE_DELETE_ARGS 3
#define EXAMPLE_APP_ARP_ADD_ARGS 2
#define EXAMPLE_APP_ARP_DELETE_ARGS 1
#define EXAMPLE_APP_MAX_LAG 4  
#define JSON_MAX_NODE_LENGTH 128

#define _jsonlog(format,args...)              printf(format, ##args)

#define JSON_VALIDATE_POINTER(x,y,z)  do { \
    if ((x) == NULL) { \
        _jsonlog("Invalid (NULL) value for parameter %s ", (y) ); \
        return (z); \
        } \
} while(0)

#define JSON_VALIDATE_JSON_POINTER(x,y,z) do { \
      if ((x) == NULL) { \
            _jsonlog("Error parsing JSON %s ", (y) ); \
            return (z); \
            } \
}while(0)

#define JSON_VALIDATE_POINTER_AND_CLEANUP(x,y,z)  do { \
      if ((x) == NULL) { \
            _jsonlog("Invalid (NULL) value for parameter %s ", (y) ); \
            if (root != NULL) { \
                  cJSON_Delete(root); \
                  } \
            return (z); \
            } \
} while(0)

#define JSON_VALIDATE_JSON_POINTER_AND_CLEANUP(x,y,z)  do { \
      if ((x) == NULL) { \
            _jsonlog("Error parsing JSON %s ", (y) ); \
            if (root != NULL) { \
                  cJSON_Delete(root); \
                  } \
            return (z); \
            } \
}while(0)


#define JSON_VALIDATE_JSON_AS_STRING(x,y,z)  do { \
      if ((x)->type != cJSON_String) { \
            _jsonlog("Error parsing JSON, %s not a string ", (y) ); \
            if (root != NULL)  { \
                  cJSON_Delete(root); \
                  } \
            return (z); \
            } \
      if((x)->valuestring == NULL) { \
            _jsonlog("Error parsing JSON, %s not a valid string ", (y) ); \
            if (root != NULL) { \
                  cJSON_Delete(root); \
                  } \
            return (z); \
            } \
}while(0)

#define JSON_VALIDATE_JSON_AS_NUMBER(x,y)   do { \
      if ((x)->type != cJSON_Number) { \
            _jsonlog("Error parsing JSON, %s not a integer ", (y) ); \
            if (root != NULL)  { \
                  cJSON_Delete(root); \
                  } \
            return (-1); \
            } \
}while(0)

#define JSON_COMPARE_STRINGS_AND_CLEANUP(x,y,z)  do { \
      if (strcmp((y), (z)) != 0) { \
            _jsonlog("The JSON contains invalid value for %s (actual %s, required %s) ", (x), (y), (z) ); \
            if (root != NULL) { \
                  cJSON_Delete(root); \
                  } \
            return (-1); \
            } \
}while(0)

#define JSON_COMPARE_VALUE_AND_CLEANUP(x,y,z)  do { \
      if ((y) != (z)) { \
            _jsonlog("The JSON contains invalid value for %s (actual %d, required %d) ", (x), (y), (z) ); \
            if (root != NULL) { \
                  cJSON_Delete(root); \
                  } \
            return (-1); \
            } \
}while(0)


#define JSON_CHECK_VALUE_AND_CLEANUP(x,y,z)  do { \
      if ( ((x) < (y)) || ( (x) > (z)) ) { \
            _jsonlog("The JSON number out of range %d (min %d, max %d) ", (x), (y), (z) ); \
            if (root != NULL) { \
                  cJSON_Delete(root); \
                  } \
            return (-1); \
            } \
}while(0)


int example_server_main (void);

/******************************************************************
 *  @brief     Function to process the incoming command request 
 *  
 *  @param[in] buffer -  data buffer read from client 
 *  @param[in] length -  number of bytes read  
 *  
 *  @retval int 
 * 
 *  @note
 **********************************************************************/
int process_input(char *buffer, int length);
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
int example_app_config_vlan_create(int len, char *buffer);

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
int example_app_config_vlan_destroy(int len, char *buffer);

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
int example_app_config_vlan_member_add(int len, char *buffer);

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
int example_app_config_vlan_member_delete(int len, char *buffer);

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
int example_app_config_lag(int len, char *buffer);

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
int example_app_delete_lag(int len, char *buffer);

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
int example_app_config_lag_member(int len, char *buffer);

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
int example_app_config_lag_member_remove(int len, char *buffer);

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
int example_app_config_l3_intf_add(int len, char *buffer);

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
int example_app_config_l3_intf_delete(int len, char *buffer);

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
int example_app_config_l3_route_add(int len, char *buffer);

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
int example_app_config_l3_route_delete(int len, char *buffer);

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
int example_app_config_l3_arp_add(int len, char *buffer);

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
int example_app_config_l3_arp_delete(int len, char *buffer);

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
int example_app_config_agent_config_params_modify(int len, char *buffer);

/******************************************************************
 * @brief  Initializes configuration, reads it from file or assumes defaults.
 *
 * @param[in]  none 
 *                           
 * @retval   0  when configuration is initialized successfully
 *
 * @note     
 *********************************************************************/
int example_app_config_init(void);

#ifdef  __cplusplus
}
#endif

#endif

