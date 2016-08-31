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
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "example_client.h" 

EXAMPLE_APP_METHOD_GRP_t drivAppMethodGrps[CONFIG_TYPE_MAX_GRP] = {
  {
    .methodId = CONFIG_TYPE_L2,
    .menuString = "Configure L2 related parameters",
  },
  {
    .methodId = CONFIG_TYPE_L3,
    .menuString = "Configure L3 related parameters",
  },
  {
    .methodId = CONFIG_TYPE_SYSTEM,
    .menuString = "Configure system related parameters",
  }
};


EXAMPLE_APP_METHOD_INFO_t drivAppMethodDetails[CONFIG_TYPE_MAX] = {
  {
    .methodId = CONFIG_TYPE_VLAN_CREATE,
    .menuString = "Create VLAN",
    .get_input_config = example_client_app_config_vlan_create
  },
  {
    .methodId = CONFIG_TYPE_VLAN_DELETE,
    .menuString = "Delete VLAN",
    .get_input_config = example_client_app_config_vlan_destroy
  },
  {
    .methodId = CONFIG_TYPE_VLAN_MEMBER_ADD,
    .menuString = "Add member to VLAN",
    .get_input_config = example_client_app_config_vlan_member_add
  },
  {
    .methodId = CONFIG_TYPE_VLAN_MEMBER_DELETE,
    .menuString = "Delete member from VLAN",
    .get_input_config = example_client_app_config_vlan_member_delete
  },
  {
    .methodId = CONFIG_TYPE_LAG,
    .menuString = "Configure LAG",
    .get_input_config = example_client_app_config_lag
  },
  {
    .methodId = CONFIG_TYPE_LAG_MEMBER_ADD,
    .menuString = "Add member to LAG",
    .get_input_config = example_client_app_config_lag_member
  },
  {
    .methodId = CONFIG_TYPE_LAG_MEMBER_DELETE,
    .menuString = "Delete member from LAG",
    .get_input_config = example_client_app_config_lag_member_remove
  },
  {
    .methodId = CONFIG_TYPE_LAG_DELETE,
    .menuString = "Delete LAG",
    .get_input_config = example_client_app_delete_lag
  },
  {
    .methodId = CONFIG_TYPE_L3_INTF_ADD,
    .menuString = "Create L3 Interface",
    .get_input_config = example_client_app_config_l3_intf_add
  },
  {
    .methodId = CONFIG_TYPE_L3_INTF_DELETE,
    .menuString = "Delete L3 Interface",
    .get_input_config = example_client_app_config_l3_intf_delete
  },
  {
    .methodId = CONFIG_TYPE_L3_ROUTE_ADD,
    .menuString = "Add L3 route",
    .get_input_config = example_client_app_config_l3_route_add
  },

  {
    .methodId = CONFIG_TYPE_L3_ROUTE_DELETE,
    .menuString = "Delete L3 route",
    .get_input_config = example_client_app_config_l3_route_delete
  },
  {
    .methodId = CONFIG_TYPE_L3_ARP_ADD,
    .menuString = "Add ARP entry",
    .get_input_config = (void *)example_client_app_config_l3_arp_add
  },
  {
    .methodId = CONFIG_TYPE_L3_ARP_DELETE,
    .menuString = "Delete ARP entry",
    .get_input_config = (void *)example_client_app_config_l3_arp_delete
  },
  {
    .methodId = CONFIG_TYPE_SYSTEM_IPADDR_MODIFY,
    .menuString = "Modify the Client IP address and port in agent",
    .get_input_config = (void *)example_client_ip_addr_modify
  }

};
/******************************************************************
 * @brief      Read input string from user and convert it to integer
 *
 * @param[in]   dest           Buffer to hold the integer
 *
 * @retval   USER_INPUT_NONE    There is not input from user
 *           USER_INPUT_ERROR   Input is incorrect
 *           USER_INPUT_OK      Input is successfully read and converted to integer
 * @note     
 *********************************************************************/

static EXAMPLE_APP_USER_INPUT_STATUS_t get_uint_from_user(unsigned int *dest)
{
  char inputData[1024] = {0};
  int originalLen = 0;
  
  while (1)
  {
    if (fgets(inputData, sizeof(inputData), stdin) != NULL)
    {
      /* fgets reads next line char(\n) as part of string */
      /* only \n is present */ 
      if (strlen(inputData) == 1)
      {
        _EXAMPLE_APP_LOG(_EXAMPLE_APP_DEBUG_TRACE,
                   "EXAMPLE_APP : no integer input from user \n");
        return USER_INPUT_NONE;
      }
      originalLen = strlen(inputData);
      inputData[originalLen-1] = 0;

      if (1 == sscanf(inputData, "%u", dest))
      {
        fflush(stdin); 
        break;
      }
      else
      {
        fflush(stdin); 
        _EXAMPLE_APP_LOG(_EXAMPLE_APP_DEBUG_ERROR,
                   "EXAMPLE_APP : invalid integer input from user \n");
        return USER_INPUT_ERROR;
      } 
    }
    return USER_INPUT_NONE;
  }
  
  _EXAMPLE_APP_LOG(_EXAMPLE_APP_DEBUG_TRACE,
             "EXAMPLE_APP : user input is %u \n", *dest);
  return USER_INPUT_OK;
}

/******************************************************************
 * @brief      Read input string from user
 *
 * @param[in]   dest           Buffer to hold the string
 *              sizeOfDest     Max size of the buffer 
 *
 * @retval   USER_INPUT_NONE    There is not input from user
 *           USER_INPUT_ERROR   Input is incorrect
 *           USER_INPUT_OK      Input is successfully read. 
 * @note     
 *********************************************************************/
EXAMPLE_APP_USER_INPUT_STATUS_t get_string_from_user(char *dest, int sizeOfDest)
{
  char inputData[EXAMPLE_APP_MAX_USER_INPUT_STRING_SIZE] = {0};
  int originalLen = 0;
  
  if (sizeOfDest > sizeof(inputData))
  {
    return USER_INPUT_ERROR; 
  }

  while (1)
  {
    if (fgets(inputData, sizeof(inputData), stdin) != NULL)
    {
      /* fgets returns string with End of line and null character */
      if (strlen(inputData) == 1)
      {
        _EXAMPLE_APP_LOG(_EXAMPLE_APP_DEBUG_TRACE,
                   "EXAMPLE_APP : no string input from user \n");
        return USER_INPUT_NONE;
      }
      originalLen = strlen(inputData);
      /* Replace EOL character with null character */
      inputData[originalLen-1] = 0;

      if (strlen(inputData) > (sizeOfDest-1))
      {
        printf("\t\t Maximum allowed is %u characters!\n", (sizeOfDest-1)); 
        fflush(stdin);
        return USER_INPUT_ERROR;
	  }
      strcpy(dest, inputData);
      fflush(stdin); 
      break;
    }
    _EXAMPLE_APP_LOG(_EXAMPLE_APP_DEBUG_TRACE,
               "EXAMPLE_APP : no  string input from user \n");
    return USER_INPUT_NONE;
  }
  _EXAMPLE_APP_LOG(_EXAMPLE_APP_DEBUG_TRACE,
             "EXAMPLE_APP : user input is %s \n", dest);
  return USER_INPUT_OK;
}

/******************************************************************
 * @brief      Get port list from user and convert it to "1","2",...
 *                 format
 *
 * @param[in]   portList           Buffer to hold the string
 *              size               Max size of the buffer 
 *
 * @retval   USER_INPUT_NONE    There is not input from user
 *           USER_INPUT_ERROR   Input is incorrect
 *           USER_INPUT_OK      Input is successfully read. 
 * @note     
 *********************************************************************/
EXAMPLE_APP_USER_INPUT_STATUS_t get_port_list_from_user(char *portList, int size)
{
  char tempList[1024] = {0};
  char *token =  NULL;
  char tokenStr[16] = {0};
  const char s1[2] = ",";
  EXAMPLE_APP_USER_INPUT_STATUS_t retVal; 

  retVal = get_string_from_user(tempList, sizeof(tempList));
  if (retVal != USER_INPUT_OK)
  {
    return retVal;
  }

  memset(portList, 0x00, size);

  token = strtok(tempList, s1);
  /* walk through other tokens */
  while( token != NULL )
  {
    sprintf (tokenStr, "\"%s\"", token);
    strcat(portList, tokenStr);
    token = strtok(NULL, s1);
    if (token != NULL)
    {
      strcat (portList, ",");
    }
  }
  return retVal;
}

/******************************************************************
 * @brief      Function to verify MAC string  
 *
 * @param[in]  s   Holds the MAC address string to be verified
 *
 * @retval    true         If MAC string is correct   
 *            false        If MAC string is incorrect 
 *
 * @note     
 *********************************************************************/
static bool isMacAddress(char *s)
{
  int i = 0;

  for(i = 0; i < 17; i++) 
  {
    if(i % 3 != 2 && !isxdigit(s[i]))
    {
      _EXAMPLE_APP_LOG(_EXAMPLE_APP_DEBUG_ERROR, "EXAMPLE_APP: "
                                    "invalid length/digit in mac address\n");
      return false;
    }
    if(i % 3 == 2 && s[i] != ':')
    {
      _EXAMPLE_APP_LOG(_EXAMPLE_APP_DEBUG_ERROR, "EXAMPLE_APP: "
                                    "no correct ':'s in mac address\n");
      return false;
    }
  }
  if(s[17] != '\0')
  {
    _EXAMPLE_APP_LOG(_EXAMPLE_APP_DEBUG_ERROR, "EXAMPLE_APP: "
                                  "invalid mac address\n");
    return false;
  }
  
  return true;
}

/******************************************************************
 * @brief      Function to get MAC address from user 
 *
 * @param[in]  macAddr  To hold the MAC address
 *
 * @retval   USER_INPUT_NONE    There is not input from user
 *           USER_INPUT_ERROR   Input is incorrect
 *           USER_INPUT_OK      Input is successfully read. 
 *
 * @note     
 *********************************************************************/
EXAMPLE_APP_USER_INPUT_STATUS_t get_mac_address_from_user(char *macAddrStr)
{
  EXAMPLE_APP_USER_INPUT_STATUS_t  retValue;
  
  if ((retValue = get_string_from_user(macAddrStr, ETHERNET_MAC_ADDR_STR_LEN-1)) 
	                                                  != USER_INPUT_OK)
  {
    return retValue;
  }
  else  
  {
    if (isMacAddress(macAddrStr) != true)
    {
      _EXAMPLE_APP_LOG(_EXAMPLE_APP_DEBUG_ERROR, "EXAMPLE_APP: invalid mac address\n");
      return USER_INPUT_ERROR; 
    }
    return USER_INPUT_OK;
  }
}

/******************************************************************
 * @brief      Function to get IPv4 address from user 
 *
 * @param[in]  ipv6Addr  To hold the IPv4 address
 *
 * @retval   USER_INPUT_NONE    There is not input from user
 *           USER_INPUT_ERROR   Input is incorrect
 *           USER_INPUT_OK      Input is successfully read. 
 *
 * @note     
 *********************************************************************/
EXAMPLE_APP_USER_INPUT_STATUS_t get_ipv4_address_from_user(char *ipAddrStr)
{
  EXAMPLE_APP_USER_INPUT_STATUS_t  retValue;
  unsigned int ipaddr;

  if ((retValue = get_string_from_user(ipAddrStr, strlen(ipAddrStr))) 
	                                                  != USER_INPUT_OK)
  {
    return retValue;
  }
  else  
  {
    if (inet_pton(AF_INET, ipAddrStr, &ipaddr) != 1)
    {
      return USER_INPUT_ERROR; 
    }
    return USER_INPUT_OK;
  }
}

/******************************************************************
 * @brief      Function to get IPv6 address from user 
 *
 * @param[in]  ipv6Addr  To hold the IPv6 address
 *
 * @retval   USER_INPUT_NONE    There is not input from user
 *           USER_INPUT_ERROR   Input is incorrect
 *           USER_INPUT_OK      Input is successfully read. 
 *
 * @note     
 *********************************************************************/
EXAMPLE_APP_USER_INPUT_STATUS_t get_ipv6_address_from_user(struct in6_addr *ipv6Addr)
{
  char ipv6AddrStr[INET6_ADDRSTRLEN] = {0};
  EXAMPLE_APP_USER_INPUT_STATUS_t  retValue;

  if ((retValue = get_string_from_user(ipv6AddrStr, sizeof(ipv6AddrStr))) 
	                                                  != USER_INPUT_OK)
  {
    return retValue;
  }
  else  
  {
    if (inet_pton(AF_INET6, ipv6AddrStr, ipv6Addr) != 1)
    {
      return USER_INPUT_ERROR; 
    }
    return USER_INPUT_OK;
  }
}

/******************************************************************
 * @brief      Function to create VLAN 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_config_vlan_create(char *buffer)
{
  unsigned int vid = 0;
  char *vlan_create_template = "{\
         \"jsonrpc\" : \"2.0\",\
         \"command\" : \"vlan-create\",\
         \"params\" : {\
                        \"vid\" : %d \
                      }\
  }";

  while (1)
  {
    printf ("Enter VLAN id to create: ");
    get_uint_from_user(&vid);
    if (!(vid < EXAMPLE_APP_MIN_VLAN || vid > EXAMPLE_APP_MAX_VLAN))
      break;
    printf("Invalid input. Please enter correct value (%d-%d)\n", EXAMPLE_APP_MIN_VLAN, EXAMPLE_APP_MAX_VLAN);
  }
  
  snprintf (buffer, 1023, vlan_create_template, vid);
}


/******************************************************************
 * @brief      Function to destroy VLAN 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_config_vlan_destroy(char *buffer)
{
  unsigned int vid = 0;
  char *vlan_delete_template = "{\
         \"jsonrpc\" : \"2.0\",\
         \"command\" : \"vlan-delete\",\
         \"params\" : {\
                        \"vid\" : %d \
                      }\
  }";

  while (1)
  {
    printf ("Enter VLAN id to delete : ");
    get_uint_from_user(&vid);
    if (!(vid < EXAMPLE_APP_MIN_VLAN || vid > EXAMPLE_APP_MAX_VLAN))
      break;
    printf("Invalid input. Please enter correct value (%d-%d)\n", EXAMPLE_APP_MIN_VLAN, EXAMPLE_APP_MAX_VLAN);
  }
  /* call the api to pack the data 
  */
  snprintf (buffer, 1023, vlan_delete_template, vid);
  return;
}

/******************************************************************
 * @brief      Function to add member to VLAN 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/

void example_client_app_config_vlan_member_add(char *buffer)
{
  unsigned int vid = 0;
  unsigned int port = 0;
  char *vlan_member_add_template = "{\
         \"jsonrpc\" : \"2.0\",\
         \"command\" : \"vlan-member-add\",\
         \"params\" : {\
                        \"vid\" : %d, \
                        \"port\" : %d \
                      }\
  }";

  while (1)
  {
    printf ("Enter VLAN id : ");
    get_uint_from_user(&vid);
    if (!(vid < EXAMPLE_APP_MIN_VLAN || vid > EXAMPLE_APP_MAX_VLAN))
      break;
    printf("Invalid input. Please enter correct value (%d-%d)\n", EXAMPLE_APP_MIN_VLAN, EXAMPLE_APP_MAX_VLAN);
  }

  while (1)
  {
    printf ("Enter member port : ");
    get_uint_from_user(&port);
    if (!(port < EXAMPLE_APP_MIN_PORT || port > EXAMPLE_APP_MAX_PORT))
      break;
    printf("Invalid input. Please enter correct value (%d-%d)\n", EXAMPLE_APP_MIN_PORT, EXAMPLE_APP_MAX_PORT);
  }

  snprintf (buffer, 1023, vlan_member_add_template, vid, port);
}

/******************************************************************
 * @brief      Function to delete member from VLAN 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_config_vlan_member_delete(char *buffer)
{
  unsigned int vid = 0;
  unsigned int port = 0;
  char *vlan_member_delete_template = "{\
         \"jsonrpc\" : \"2.0\",\
         \"command\" : \"vlan-member-delete\",\
         \"params\" : {\
                        \"vid\" : %d, \
                        \"port\" : %d \
                      }\
  }";

  while (1)
  {
    printf ("Enter VLAN id : ");
    get_uint_from_user(&vid);
    if (!(vid < EXAMPLE_APP_MIN_VLAN || vid > EXAMPLE_APP_MAX_VLAN))
      break;
    printf("Invalid input. Please enter correct value (%d-%d)\n", EXAMPLE_APP_MIN_VLAN, EXAMPLE_APP_MAX_VLAN);
  }

  while (1)
  {
    printf ("Enter member port : ");
    get_uint_from_user(&port);
    if (!(port < EXAMPLE_APP_MIN_PORT || port > EXAMPLE_APP_MAX_PORT))
      break;
    printf("Invalid input. Please enter correct value (%d-%d)\n", EXAMPLE_APP_MIN_PORT, EXAMPLE_APP_MAX_PORT);
  }

  snprintf (buffer, 1023, vlan_member_delete_template, vid, port);
}

/******************************************************************
 * @brief      Function to configure LAG id 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_config_lag(char *buffer)
{
  unsigned int lag_id = 0;
  char *lag_create_template = "{\
         \"jsonrpc\" : \"2.0\",\
         \"command\" : \"lag-create\",\
         \"params\" : {\
                        \"lag-id\" : %d \
                      }\
  }";

  while(1)
  {
  printf ("Enter LAG id to create : ");
  get_uint_from_user(&lag_id);

  if ((0 < lag_id) && (EXAMPLE_APP_MAX_LAG+1 > lag_id))
    break;
    printf("Allowed LAG id is from 1-%d\n",EXAMPLE_APP_MAX_LAG);
  }

  snprintf (buffer, 1023, lag_create_template, lag_id);
}

/******************************************************************
 * @brief      Function to delete LAG id 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_delete_lag(char *buffer)
{
  unsigned int lag_id = 0;
  char *lag_delete_template = "{\
         \"jsonrpc\" : \"2.0\",\
         \"command\" : \"lag-delete\",\
         \"params\" : {\
                        \"lag-id\" : %d \
                      }\
  }";

  while(1)
  {
    printf ("Enter LAG  id to destroy : ");
    get_uint_from_user(&lag_id);

    if ((0 < lag_id) && (EXAMPLE_APP_MAX_LAG+1 > lag_id))
      break;
    printf("Allowed LAG id is from 1-%d\n",EXAMPLE_APP_MAX_LAG);
  }
  snprintf (buffer, 1023, lag_delete_template, lag_id);
}

/******************************************************************
 * @brief      Function to add member to LAG 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_config_lag_member(char *buffer)
{
  unsigned int lag_id = 0;
  unsigned int lag_member = 0;
  char *lag_member_template = "{\
         \"jsonrpc\" : \"2.0\",\
         \"command\" : \"lag-member-add\",\
         \"params\" : {\
                        \"lag-id\" : %d, \
                        \"member-port\" : %d \
                      }\
  }";

  while(1)
  {
    printf ("Enter LAG  id : ");
    get_uint_from_user(&lag_id);

    if ((0 < lag_id) && (EXAMPLE_APP_MAX_LAG+1 > lag_id))
      break;
    printf("Allowed LAG id is from 1-%d\n",EXAMPLE_APP_MAX_LAG);
  }

  printf ("Enter LAG  member : ");
  get_uint_from_user(&lag_member);

  snprintf (buffer, 1023, lag_member_template, lag_id, lag_member);
}

/******************************************************************
 * @brief      Function to delete member to LAG 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_config_lag_member_remove(char *buffer)
{
  unsigned int lag_id = 0;
  unsigned int lag_member = 0;
  char *lag_member_template = "{\
         \"jsonrpc\" : \"2.0\",\
         \"command\" : \"lag-member-delete\",\
         \"params\" : {\
                        \"lag-id\" : %d, \
                        \"member-port\" : %d \
                      }\
  }";

  while(1)
  {
    printf ("Enter LAG  id : ");
    get_uint_from_user(&lag_id);

    if ((0 < lag_id) && (EXAMPLE_APP_MAX_LAG+1 > lag_id))
      break;
    printf("Allowed LAG id is from 1-%d\n",EXAMPLE_APP_MAX_LAG);
  }

  printf ("Enter LAG  member : ");
  get_uint_from_user(&lag_member);

  snprintf (buffer, 1023, lag_member_template, lag_id, lag_member);
}
/******************************************************************
 * @brief      Function to configure L3 Interface 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_config_l3_intf_add(char *buffer)
{
  char macAddrStr[ETHERNET_MAC_ADDR_STR_LEN] = {0};
  char ipAddrStr[INET_ADDRSTRLEN] = {0};
  char ipMask[INET_ADDRSTRLEN] = {0};
  unsigned int in_vid;
  unsigned int port;
  EXAMPLE_APP_USER_INPUT_STATUS_t  retValue;
  char *l3intf_template = "{\
         \"jsonrpc\" : \"2.0\",\
         \"command\" : \"l3intf-create\",\
         \"params\" : {\
                        \"vid\" : %d, \
                        \"member-port\" : %d, \
                        \"mac-addr\" : \"%s\", \
                        \"ip-addr\" : \"%s\", \
                        \"subnet-mask\" : \"%s\" \
                      }\
  }";

  printf ("Enter interface VLAN id : ");
  get_uint_from_user(&in_vid);


  printf ("Enter member port of the VLAN : ");
  get_uint_from_user(&port);


  while (1)
  {
    printf ("Enter L3 interface (self) mac addr : ");
    retValue =   get_mac_address_from_user(&macAddrStr[0]);

    if (retValue == USER_INPUT_OK)
    {
      break;
    }
    printf("\t\t Invalid input! Please provide correct value\n");
  }

  while (1)
  {
    printf ("Enter IP address: ");
    retValue = get_ipv4_address_from_user(&ipAddrStr[0]);

    if (retValue == USER_INPUT_OK)
    {
      break;
    }
    printf("\t\t Invalid input! Please provide correct value\n");
  }

  while (1)
  {
    printf ("Enter mask: ");
    retValue = get_ipv4_address_from_user(&ipMask[0]);

    if (retValue == USER_INPUT_OK)
    {
      break;
    }
    printf("\t\t Invalid input! Please provide correct value\n");
  }
  snprintf (buffer, 1023, l3intf_template, in_vid, port, &macAddrStr[0], &ipAddrStr[0], &ipMask[0]);

}

/******************************************************************
 * @brief      Function to delete L3 Interface 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_config_l3_intf_delete(char *buffer)
{
  unsigned int in_vid;
  char *l3intf_template = "{\
         \"jsonrpc\" : \"2.0\",\
         \"command\" : \"l3intf-delete\",\
         \"params\" : {\
                        \"vid\" : %d \
                      }\
  }";

  printf ("Enter interface VLAN id : ");
  get_uint_from_user(&in_vid);

  snprintf (buffer, 1023, l3intf_template, in_vid);
}

/******************************************************************
 * @brief      Function to add route 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_config_l3_route_add(char *buffer)
{
  EXAMPLE_APP_USER_INPUT_STATUS_t  retValue;
  char nextHopStr[ETHERNET_MAC_ADDR_STR_LEN] = {0};
  char ipAddrStr[INET_ADDRSTRLEN] = {0};
  char ipMask[INET_ADDRSTRLEN] = {0};
  char *l3route_template = "{\
         \"jsonrpc\" : \"2.0\",\
         \"command\" : \"route-add\",\
         \"params\" : {\
                        \"route\" : \"%s\", \
                        \"mask\" : \"%s\", \
                        \"next-hop\" : \"%s\" \
                      }\
  }";

  while (1)
  {
    printf ("Enter IP addr : ");
    retValue = get_ipv4_address_from_user(&ipAddrStr[0]);

    if (retValue == USER_INPUT_OK)
    {
      break;
    }
    printf("\t\t Invalid input! Please provide correct value\n");
  }

  while (1)
  {
    printf ("Enter subnet mask : ");
    retValue = get_ipv4_address_from_user(&ipMask[0]);

    if (retValue == USER_INPUT_OK)
    {
      break;
    }
    printf("\t\t Invalid input! Please provide correct value\n");
  }

  while (1)
  {
    printf ("Enter next_hop : ");
    retValue = get_ipv4_address_from_user(&nextHopStr[0]);

    if (retValue == USER_INPUT_OK)
    {
      break;
    }
    printf("\t\t Invalid input! Please provide correct value\n");
  }

  snprintf (buffer, 1023, l3route_template, &ipAddrStr[0], &ipMask[0], &nextHopStr[0]);
}

/******************************************************************
 * @brief      Function to delete route 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/


void example_client_app_config_l3_route_delete(char *buffer)
{
  EXAMPLE_APP_USER_INPUT_STATUS_t  retValue;
  char nextHopStr[ETHERNET_MAC_ADDR_STR_LEN] = {0};
  char ipAddrStr[INET_ADDRSTRLEN] = {0};
  char ipMask[INET_ADDRSTRLEN] = {0};
  char *l3route_template = "{\
         \"jsonrpc\" : \"2.0\",\
         \"command\" : \"route-delete\",\
         \"params\" : {\
                        \"route\" : \"%s\", \
                        \"mask\" : \"%s\", \
                        \"next-hop\" : \"%s\" \
                      }\
  }";

  while (1)
  {
    printf ("Enter IP addr : ");
    retValue = get_ipv4_address_from_user(&ipAddrStr[0]);

    if (retValue == USER_INPUT_OK)
    {
      break;
    }
    printf("\t\t Invalid input! Please provide correct value\n");
  }


  while (1)
  {
    printf ("Enter subnet mask : ");
    retValue = get_ipv4_address_from_user(&ipMask[0]);

    if (retValue == USER_INPUT_OK)
    {
      break;
    }
    printf("\t\t Invalid input! Please provide correct value\n");
  }


  while (1)
  {
    printf ("Enter next_hop : ");
    retValue = get_ipv4_address_from_user(&nextHopStr[0]);

    if (retValue == USER_INPUT_OK)
    {
      break;
    }
    printf("\t\t Invalid input! Please provide correct value\n");
  }

  snprintf (buffer, 1023, l3route_template, &ipAddrStr[0], &ipMask[0], &nextHopStr[0]);
}

/******************************************************************
 * @brief      Function to add arp 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_config_l3_arp_add(char *buffer)
{
  EXAMPLE_APP_USER_INPUT_STATUS_t  retValue;
  char ipAddrStr[INET_ADDRSTRLEN] = {0};
  char nextHopStr[ETHERNET_MAC_ADDR_STR_LEN] = {0};
  char *l3arp_template = "{\
         \"jsonrpc\" : \"2.0\",\
         \"command\" : \"arp-add\",\
         \"params\" : {\
                        \"ipaddr\" : \"%s\", \
                        \"mac-addr\" : \"%s\" \
                      }\
  }";


  while (1)
  {
    printf ("Enter IP addr : ");
    retValue = get_ipv4_address_from_user(&ipAddrStr[0]);
    if (retValue == USER_INPUT_OK)
    {
      break;
    }
    printf("\t\t Invalid input! Please provide correct value\n");
  }

  while (1)
  {
    printf ("Enter next hop mac addr : ");
    retValue =   get_mac_address_from_user(&nextHopStr[0]);

    if (retValue == USER_INPUT_OK)
    {
      break;
    }
    printf("\t\t Invalid input! Please provide correct value\n");
  }

  snprintf (buffer, 1023, l3arp_template, &ipAddrStr[0], &nextHopStr[0]);
}


/******************************************************************
 * @brief      Function to delete arp 
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_app_config_l3_arp_delete(char *buffer)
{
  char ipAddrStr[INET_ADDRSTRLEN] = {0};
  EXAMPLE_APP_USER_INPUT_STATUS_t  retValue;
  char *l3arp_template = "{\
         \"jsonrpc\" : \"2.0\",\
         \"command\" : \"arp-delete\",\
         \"params\" : {\
                        \"ipaddr\" : \"%s\" \
                      }\
  }";

  while (1)
  {
    printf ("Enter IP addr : ");
    retValue = get_ipv4_address_from_user(&ipAddrStr[0]);

    if (retValue == USER_INPUT_OK)
    {
      break;
    }
    printf("\t\t Invalid input! Please provide correct value\n");
  }
  snprintf (buffer, 1023, l3arp_template, &ipAddrStr[0]);

}

/******************************************************************
 * @brief      Function to change the client ip addr and port
 *
 * @param[in]  buffer to hold the input data  
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_client_ip_addr_modify(char *buffer)
{
  EXAMPLE_APP_USER_INPUT_STATUS_t  retValue;
  char ipAddrStr[INET_ADDRSTRLEN] = {0};
  unsigned int port = 0;
  char *client_ip_port_template = "{\
         \"jsonrpc\" : \"2.0\",\
         \"command\" : \"client-params-modify\",\
         \"params\" : {\
                        \"ipaddr\" : \"%s\", \
                        \"port\" : %d \
                      }\
  }";

  while (1)
  {
    printf ("Enter client IP addr : ");
    retValue = get_ipv4_address_from_user(&ipAddrStr[0]);

    if (retValue == USER_INPUT_OK)
    {
      break;
    }
    printf("\t\t Invalid input! Please provide correct value\n");
  }

    printf ("Enter client port : ");
    get_uint_from_user(&port);

  snprintf (buffer, 1023, client_ip_port_template, &ipAddrStr[0], port);
}



/******************************************************************
 * @brief      Function to display main menu
 *
 * @param[in]  none 
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_app_display_root_menu(void)
{
  example_config_grp_t  displayMethod = 0;
  unsigned int numOfElements = 0;
  unsigned int index = 0;

  /* Display main menu options */
  numOfElements = sizeof(drivAppMethodGrps)/sizeof(EXAMPLE_APP_METHOD_GRP_t); 
  printf("\n\n");
  for (displayMethod = 0; displayMethod < CONFIG_TYPE_MAX_GRP; displayMethod++)
  {
    for (index = 0; index < numOfElements; index++)
    {
      if (drivAppMethodGrps[index].methodId == displayMethod)
      {
        printf("%u.%s\n", (displayMethod+1), drivAppMethodGrps[index].menuString); 
        break;
      }
    }
  }

  printf("%u.Exit\n", (CONFIG_TYPE_MAX_GRP+1));
  printf("Select an option from the menu:");

}

/******************************************************************
 * @brief      Function to display menu
 *
 * @param[in]  option 
 *
 * @retval   none
 *
 * @note     
 *********************************************************************/
void example_app_display_menu(unsigned int option)
{
  example_config_type_t  displayMethod = 0;
  unsigned int numOfElements = 0;
  unsigned int index = 0;
  unsigned int start = 0, end = 0;

  if(CONFIG_TYPE_L2 == option)
  {
    start = L2_CONFIG_OPTIONS_START;
    end = L2_CONFIG_OPTIONS_END;
  }

  if (CONFIG_TYPE_L3 == option)
  {
    start = L3_CONFIG_OPTIONS_START;
    end = L3_CONFIG_OPTIONS_END;
  }

  if (CONFIG_TYPE_SYSTEM == option)
  {
    start = SYSTEM_CONFIG_OPTIONS_START;
    end = SYSTEM_CONFIG_OPTIONS_END;
  }


  /* Display main menu options */
  numOfElements = sizeof(drivAppMethodDetails)/sizeof(EXAMPLE_APP_METHOD_INFO_t); 
  printf("\n\n");
  for (displayMethod = start; displayMethod < end+1; displayMethod++)
  {
    for (index = 0; index < numOfElements; index++)
    {
      if (drivAppMethodDetails[index].methodId == displayMethod)
      {
        printf("%u.%s\n", (displayMethod-start+1), drivAppMethodDetails[index].menuString); 
        break;
      }
    }
  }

  printf("%u.Exit\n", (CONFIG_TYPE_MAX+1));
  printf("Select an option from the menu:");

}
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
void example_app_get_user_input(int fd)
{
  example_config_type_t  selectedMethod = 0;
  unsigned int index = 0;
  unsigned int option, root_option;
  EXAMPLE_APP_USER_INPUT_STATUS_t retValue;
  char buffer[1024] = {0};
  char *ptr;
  int temp =0;
  unsigned int start = 0;

  /* Display main menu options */
  example_app_display_root_menu();

  retValue = get_uint_from_user(&root_option);

  while ((retValue == USER_INPUT_ERROR) || (retValue == USER_INPUT_NONE))
  {
    printf("\n Invalid input! Please choose correct option \n");
    example_app_display_root_menu();
    retValue = get_uint_from_user(&root_option);
  }

  while (root_option > (CONFIG_TYPE_MAX_GRP+1))
  {
    printf("\n Invalid Option %u! Please choose correct option \n", root_option);
    example_app_display_root_menu();
    retValue = get_uint_from_user(&root_option);
  }
  if (root_option == (CONFIG_TYPE_MAX_GRP+1))
  {
    close(fd);
    exit(0);
  }



  example_app_display_menu(root_option-1);

  retValue = get_uint_from_user(&option);

  while ((retValue == USER_INPUT_ERROR) || (retValue == USER_INPUT_NONE))
  {
    printf("\n Invalid input! Please choose correct option \n");
    example_app_display_menu(root_option-1);
    retValue = get_uint_from_user(&option);
  }

  while (option > (CONFIG_TYPE_MAX+1))
  {
    printf("\n Invalid Option %u! Please choose correct option \n", option);
    example_app_display_menu(root_option-1);
    retValue = get_uint_from_user(&option);
  }
  if (option == (CONFIG_TYPE_MAX+1))
  {
    close(fd);
    exit(0);
  }

  if(CONFIG_TYPE_L2 == root_option-1)
  {
    start = L2_CONFIG_OPTIONS_START;
  }

  if (CONFIG_TYPE_L3 == root_option-1)
  {
    start = L3_CONFIG_OPTIONS_START;
  }

  if (CONFIG_TYPE_SYSTEM == root_option-1)
  {
    start = SYSTEM_CONFIG_OPTIONS_START;
  }

  selectedMethod = (option+start - 1);

  memset(buffer, 0, sizeof(buffer));

  for (index = 0; index < CONFIG_TYPE_MAX; index++)
  {
    if (selectedMethod == drivAppMethodDetails[index].methodId)
    {
      ptr = &buffer[0];
      drivAppMethodDetails[index].get_input_config(ptr);
      temp = send(fd, buffer, strlen(buffer), 0);
      if (0 > temp)
      {
        printf("failed to send any data \n");
      }
      break;
    }
  }
  return;
}

/******************************************************************
 * @brief      main function for the client   
 *
 * @param[in] argc, argv 
 *
 *
 * @retval   none   
 *
 * @note    : The server name can be provided as command line arg 
 *********************************************************************/

int main(int argc, char *argv[]){
  int clientSocket;
  int temp = 0;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  char server[128] = {0};

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  if (clientSocket == -1)
  {
    printf("failed to create socket \n");
    exit(0);
  }

  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(EXAMPLE_CONFIG_PORT);

  /*If the server hostname is supplied*/

  if(argc > 1)

  {
    /*Use the supplied argument*/
    strncpy(server, argv[1],(sizeof(server) - 1));
    printf("Connecting to the server %s, port %d ...\n", server, EXAMPLE_CONFIG_PORT);
  }
  else
  {
    /*Use the default server name or IP*/
    strcpy(server, DEF_SERVER);
  }

  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr(server);
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*---- Connect the socket to the server using the address struct ----*/
  addr_size = sizeof serverAddr;
  temp = connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
  if (temp == -1)
  {
    printf("failed to connect \n");
    close(clientSocket);
    exit(0);
  }

  while (1)
  {
    /* prompt for menu */
    example_app_get_user_input(clientSocket);
  }

  close(clientSocket);
  exit(0);
}

