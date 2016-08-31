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

#ifndef INCLUDE_EXAMPLE_CLIENT_H
#define	INCLUDE_EXAMPLE_CLIENT_H

#ifdef	__cplusplus
extern "C"
{
#endif


#define EXAMPLE_CONFIG_PORT 7891
#define EXAMPLE_MAX_BUFFER_LEN 1024
#define DEF_SERVER "127.0.0.1"
#define EXAMPLE_PACKLEN_U32 4


#define EXAMPLE_APP_MAX_TABLE_SIZE        20
#define ETHERNET_MAC_ADDR_LEN       6
#define EXAMPLE_APP_MAX_LAG       4
#define EXAMPLE_APP_MAX_USER_INPUT_STRING_SIZE       1024
#define ETHERNET_MAC_ADDR_STR_LEN       128
#define EXAMPLE_APP_MIN_VLAN  1
#define EXAMPLE_APP_MAX_VLAN 4094
#define EXAMPLE_APP_MIN_PORT 1
#define EXAMPLE_APP_MAX_PORT 48
#define EXAMPLE_APP_MAX_MULTIPATHS 8


#define _EXAMPLE_APP_DEBUG
#define _EXAMPLE_APP_DEBUG_LEVEL        (0x0)

#define _EXAMPLE_APP_DEBUG_TRACE        (0x1)
#define _EXAMPLE_APP_DEBUG_INFO         (0x01 << 1)
#define _EXAMPLE_APP_DEBUG_ERROR        (0x01 << 2)
#define _EXAMPLE_APP_DEBUG_ALL          (0xFF)

#ifdef _EXAMPLE_APP_DEBUG
#define _EXAMPLE_APP_LOG(level, format,args...)   do { \
              if ((level) & _EXAMPLE_APP_DEBUG_LEVEL) { \
                                printf(format, ##args); \
                            } \
          }while(0)
#else
#define _EXAMPLE_APP_LOG(level, format,args...)
#endif


#define EXAMPLE_PACK_U32(_buf, _var)           \
      do {                                    \
                (_buf)[0] = ((_var) >> 24) & 0xff;  \
                (_buf)[1] = ((_var) >> 16) & 0xff;  \
                (_buf)[2] = ((_var) >> 8) & 0xff;   \
                (_buf)[3] = (_var) & 0xff;          \
                (_buf) += EXAMPLE_PACKLEN_U32;         \
            } while (0)

#define EXAMPLE_UNPACK_U32(_buf, _var)         \
      do {                                    \
                (_var) = (((_buf)[0] << 24) |       \
                                      ((_buf)[1] << 16) |       \
                                      ((_buf)[2] << 8)  |       \
                                      (_buf)[3]);               \
                (_buf) += EXAMPLE_PACKLEN_U32;         \
            } while (0)


  typedef struct macAddr_s
  {
    char  addr[ETHERNET_MAC_ADDR_LEN];
  }macAddr_t;

typedef enum example_config_grp_ {
  CONFIG_TYPE_L2 = 0,
  CONFIG_TYPE_L3,
  CONFIG_TYPE_SYSTEM,
  CONFIG_TYPE_MAX_GRP
} example_config_grp_t;



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
  CONFIG_TYPE_SYSTEM_IPADDR_MODIFY,
  CONFIG_TYPE_MAX
} example_config_type_t;

#define L2_CONFIG_OPTIONS_START CONFIG_TYPE_VLAN_CREATE
#define L2_CONFIG_OPTIONS_END CONFIG_TYPE_LAG_MEMBER_DELETE

#define L3_CONFIG_OPTIONS_START CONFIG_TYPE_L3_INTF_ADD
#define L3_CONFIG_OPTIONS_END CONFIG_TYPE_L3_ARP_DELETE

#define SYSTEM_CONFIG_OPTIONS_START CONFIG_TYPE_SYSTEM_IPADDR_MODIFY 
#define SYSTEM_CONFIG_OPTIONS_END CONFIG_TYPE_SYSTEM_IPADDR_MODIFY
typedef enum example_data_type {
  UNSIGNED_INT = 1,
  MAC_ADDR
} example_data_type_t;


typedef struct example_config_hdr_s {
  uint8_t num_args;
  uint8_t config_type;
}example_config_hdr_t;

typedef struct example_config_tlv_hdr_s {
  uint8_t type;
  uint8_t length;
}example_config_tlv_hdr_t;

typedef struct _example_app_config_method_info_  {
  example_config_type_t methodId;
  char *method;
  char *menuString;
  void(*get_input_config)(char *buffer);
} EXAMPLE_APP_METHOD_INFO_t;

typedef struct _example_app_config_method_grp_  {
  example_config_grp_t methodId;
  char *method;
  char *menuString;
} EXAMPLE_APP_METHOD_GRP_t;

typedef enum _example_app_user_input_status_ {
  USER_INPUT_OK = 0,
  USER_INPUT_NONE,
  USER_INPUT_ERROR
} EXAMPLE_APP_USER_INPUT_STATUS_t;
/******************************************************************
 * @brief      Function to create VLAN 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/

void example_client_app_config_vlan_create(char *buffer);

/******************************************************************
 * @brief      Function to destroy VLAN 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_config_vlan_destroy(char *buffer);

/******************************************************************
 * @brief      Function to add member to VLAN 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_config_vlan_member_add(char *buffer);

/******************************************************************
 * @brief      Function to delete member from VLAN 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_config_vlan_member_delete(char *buffer);

/******************************************************************
 * @brief      Function to configure LAG id 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_config_lag(char *buffer);

/******************************************************************
 * @brief      Function to delete LAG id 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_delete_lag(char *buffer);

/******************************************************************
 * @brief      Function to add member to LAG 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/

void example_client_app_config_lag_member(char *buffer);

/******************************************************************
 * @brief      Function to delete member to LAG 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_config_lag_member_remove(char *buffer);
/******************************************************************
 * @brief      Function to configure L3 Interface 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_config_l3_intf_add(char *buffer);

/******************************************************************
 * @brief      Function to delete L3 Interface 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_config_l3_intf_delete(char *buffer);

/******************************************************************
 * @brief      Function to add route 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_config_l3_route_add(char *buffer);

/******************************************************************
 * @brief      Function to delete route 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_config_l3_route_delete(char *buffer);

/******************************************************************
 * @brief      Function to add arp 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_config_l3_arp_add(char *buffer);

/******************************************************************
 * @brief      Function to delete arp 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_config_l3_arp_delete(char *buffer);

/******************************************************************
 * @brief      Function to change the client ip addr and port
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_ip_addr_modify(char *buffer);

/******************************************************************
 * @brief      Function to display main menu
 *
 * @param[in]  none 
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_app_display_root_menu();

/******************************************************************
 * @brief      Function to display menu
 *
 * @param[in]  option 
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_app_display_menu(unsigned int option);
/******************************************************************
 * @brief      Function to provide (main)features menu to user  
 *
 * @param[in] fd 
 *
 *
 * @retval   none   
 *
 * @note     
 *********************************************************************/
void example_app_get_user_input(int fd);

int main(int argc, char *argv[]);


#ifdef  __cplusplus
}
#endif

#endif  /* INCLUDE_EXAMPLE_CLIENT_H */
