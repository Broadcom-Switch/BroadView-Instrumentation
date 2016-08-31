
/*********************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename bhdapp_menu.c 
  *
  * @purpose BroadView BHD reference application 
  *
  * @component Black hole detection 
  *
  * @comments
  *
  * @create 3/17/2016
  *
  * @author 
  * @end
  *
  **********************************************************************/

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "bhdapp.h"
#include "bhdapp_json.h"
#include "bhdapp_menu.h"
#include "bhdapp_debug.h"





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

static BHDAPP_USER_INPUT_STATUS_t get_uint_from_user(unsigned int *dest)
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
        _BHDAPP_LOG(_BHDAPP_DEBUG_TRACE,
                   "BHDAPP : no integer input from user \n");
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
        _BHDAPP_LOG(_BHDAPP_DEBUG_ERROR,
                   "BHDAPP : invalid integer input from user \n");
        return USER_INPUT_ERROR;
      } 
    }
    return USER_INPUT_NONE;
  }
  
  _BHDAPP_LOG(_BHDAPP_DEBUG_TRACE,
             "BHDAPP : user input is %u \n", *dest);
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
BHDAPP_USER_INPUT_STATUS_t get_string_from_user(char *dest, int sizeOfDest)
{
  char inputData[BHDAPP_MAX_USER_INPUT_STRING_SIZE] = {0};
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
        _BHDAPP_LOG(_BHDAPP_DEBUG_TRACE,
                   "BHDAPP : no string input from user \n");
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
    _BHDAPP_LOG(_BHDAPP_DEBUG_TRACE,
               "BHDAPP : no  string input from user \n");
    return USER_INPUT_NONE;
  }
  _BHDAPP_LOG(_BHDAPP_DEBUG_TRACE,
             "BHDAPP : user input is %s \n", dest);
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
BHDAPP_USER_INPUT_STATUS_t get_port_list_from_user(char *portList, int size)
{
  char tempList[1024] = {0};
  char *token =  NULL;
  char tokenStr[16] = {0};
  const char s1[2] = ",";
  BHDAPP_USER_INPUT_STATUS_t retVal; 

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
      _BHDAPP_LOG(_BHDAPP_DEBUG_ERROR, "BHDAPP: "
                                    "invalid length/digit in mac address\n");
      return false;
    }
    if(i % 3 == 2 && s[i] != ':')
    {
      _BHDAPP_LOG(_BHDAPP_DEBUG_ERROR, "BHDAPP: "
                                    "no correct ':'s in mac address\n");
      return false;
    }
  }
  if(s[17] != '\0')
  {
    _BHDAPP_LOG(_BHDAPP_DEBUG_ERROR, "BHDAPP: "
                                  "invalid mac address\n");
    return false;
  }
  
  return true;
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
    _BHDAPP_LOG(_BHDAPP_DEBUG_ERROR, "BHDAPP: invalid mac address\n");
    return -1;
  }
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
BHDAPP_USER_INPUT_STATUS_t get_mac_address_from_user(macAddr_t *macAddr)
{
  char macAddrStr[ETHERNET_MAC_ADDR_STR_LEN] = {0};
  BHDAPP_USER_INPUT_STATUS_t  retValue;
  
  if ((retValue = get_string_from_user(macAddrStr, sizeof(macAddrStr))) 
	                                                  != USER_INPUT_OK)
  {
    return retValue;
  }
  else  
  {
    if (isMacAddress(macAddrStr) != true)
    {
      _BHDAPP_LOG(_BHDAPP_DEBUG_ERROR, "BHDAPP: invalid mac address\n");
      return USER_INPUT_ERROR; 
    }
    convertMacStrToMacValue(macAddrStr, macAddr);
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
BHDAPP_USER_INPUT_STATUS_t get_ipv4_address_from_user(char *ipAddrPtr)
{
  unsigned int ipAddr;	
  char ipAddrStr[INET_ADDRSTRLEN] = {0};
  BHDAPP_USER_INPUT_STATUS_t  retValue;

  if ((retValue = get_string_from_user(ipAddrStr, sizeof(ipAddrStr))) 
	                                                  != USER_INPUT_OK)
  {
    return retValue;
  }
  else  
  {
    if (inet_pton(AF_INET, ipAddrStr, &ipAddr) != 1)
    {
      return USER_INPUT_ERROR; 
    }
    strcpy(ipAddrPtr, ipAddrStr);
    return USER_INPUT_OK;
  }
}

/******************************************************************
 * @brief      Target function to read input from user to prepare 
 *             JSON buffer for black-hole-detection-enable method. If input is not 
 *             provided for particular fields, default values would be taken 
 *             to prepare JSON 
 *
 * @param[in]   jsonFrmt       Format of the JSON buffer 
 *              finalJsonBuf   Buffer to hold the json message with user inputs 
 *
 * @retval   0    Successfully read user inputs and prepared json buffer
 *
 * @note     
 *********************************************************************/
int config_bhd_feature(char *jsonFrmt, char *finalJsonBuf)
{
  char asicId[16]  = BHDAPP_DEFAULT_ASIC_ID;
  unsigned int bhdEnable = BHDAPP_DEFAULT_BHD_CONFIG;
  unsigned int jsonId = 0;
  BHDAPP_USER_INPUT_STATUS_t retValue;

  printf("\t Provide asic Id[%s] :", asicId);
  get_string_from_user(asicId, sizeof(asicId));

  while (1)
  {
    printf("\t Enable Black Hole Detection[%u] :", bhdEnable);
    retValue = get_uint_from_user(&bhdEnable);
  
    if ((retValue == USER_INPUT_ERROR) || (bhdEnable > 1))
    {
      bhdEnable = BHDAPP_DEFAULT_BHD_CONFIG; 
      printf("\t\t Invalid Value!  Please enter correct value (0/1)\n");
      continue;
    }
    break;
  }
  
  jsonId = get_new_json_id();
  sprintf(finalJsonBuf, jsonFrmt, asicId, bhdEnable, jsonId);
  printf("\n Request sent to Agent with Id %u\n", jsonId);
  return 0;
}

/**************************************************************************************
 * @brief      Target function to read input from user to prepare 
 *             JSON buffer for   get-black-hole-detection-enable method. If input is not 
 *             provided for particular fields, default values would be taken 
 *             to prepare JSON 
 *
 * @param[in]   jsonFrmt       Format of the JSON buffer 
 *              finalJsonBuf   Buffer to hold the json message with user inputs 
 *
 * @retval   0    Successfully read user inputs and prepared json buffer
 *
 * @note     
 ****************************************************************************************/
int get_bhd_feature(char *jsonFrmt, char *finalJsonBuf)
{
  char asicId[16]  = BHDAPP_DEFAULT_ASIC_ID;
  unsigned int jsonId = 0;

  printf("\t Provide asic Id[%s] :", asicId);
  get_string_from_user(asicId, sizeof(asicId));

  jsonId = get_new_json_id();
  sprintf(finalJsonBuf, jsonFrmt, asicId, jsonId);
  printf("\n Request sent to Agent with Id %u\n", jsonId);
  return 0;
}

/******************************************************************
 * @brief      Target function to read input from user to prepare 
 *             JSON buffer to configure black hole. 
 *             If input is not provided for particular 
 *             fields, default values would be taken to prepare JSON 
 *
 * @param[in]   jsonFrmt       Format of the JSON buffer 
 *              finalJsonBuf   Buffer to hold the json message with user inputs 
 *
 * @retval   0    Successfully read user inputs and prepared json buffer
 *
 * @note     
 *********************************************************************/
int configure_bhd_black_hole(char *jsonFrmt, char *finalJsonBuf)
{
  char asicId[16] = BHDAPP_DEFAULT_ASIC_ID;
  char portList[4*1024] = {0};
  char tempPortList[1024] = BHDAPP_DEFAULT_PORT_LIST;
  char samplingMethod[32] = BHDAPP_DEFAULT_SAMPLING_METHOD;   
  char *agentSamplingParamsFmt =         "{                               \
                                             \"water-mark\": %u,         \
                                             \"sample-periodicity\": %u, \
                                             \"sample-count\": %u        \
                                         }";

  unsigned int waterMark = BHDAPP_DEFAULT_WATERMARK;
  unsigned int samplePeriodicity = BHDAPP_DEFAULT_PERIODICITY;
  unsigned int sampleCount =  BHDAPP_DEFAULT_SAMPLE_COUNT;

  char *sflowSamplingParamsFmt =         "{                                        \
                                             \"encapsulation-params\": {          \
                                              %s                                  \
                                             \"destination-ip\": \"%s\",          \
                                             \"source-udp-port\": %u,             \
                                             \"destination-udp-port\": %u         \
                                             },                                   \
                                             \"mirror-port\": \"%s\",             \
                                             \"sample-pool-size\": %u             \
	                                     }";
  char vlan_id_str_frmt[32] = "\"vlan-id\": %u,"; 
  char vlan_id_str[64] = {0};
  unsigned int vlan = 0; 
  char destIp[64] = BHDAPP_DEFAULT_DEST_IP;
  unsigned int sPort = BHDAPP_DEFAULT_SRC_UDP_PORT;
  unsigned int dPort = BHDAPP_DEFAULT_DEST_UDP_PORT;
  char mirrorPort[16] = BHDAPP_DEFAULT_MIRROR_PORT;
  unsigned int samplePoolSize = BHDAPP_DEFAULT_SAMPLE_POOL_SIZE; 
  char agentSamplingParams[2*1024] = {0};
  char sflowSamplingParams[2*1024] = {0};
  unsigned int jsonId = 0;

  printf("\t Provide asic Id[%s] :", asicId);
  get_string_from_user(asicId, sizeof(asicId));
  
  printf("\t Provide port list[%s]:", tempPortList);
  sprintf (portList, "\"%s\"", tempPortList);
  get_port_list_from_user(portList, sizeof(portList));
 

  while (1)
  {
    printf("\t Provide sampling method[%s] (Enter agent/sflow):", BHDAPP_DEFAULT_SAMPLING_METHOD);
    get_string_from_user(samplingMethod, sizeof(samplingMethod));
    if (!((strcmp(samplingMethod,"agent") == 0) || (strcmp(samplingMethod,"sflow") == 0))) 
    {
      printf("\t Invalid input! Please enter agent/sflow \n");
      continue;
    }
    break;
  }

  if (strcmp(samplingMethod,"agent") == 0)
  {	
    printf("\n\t Provide agent sampling params\n");
    while (1)
    {
      printf("\t\t Enter Water Mark[%u]:", BHDAPP_DEFAULT_WATERMARK);
    
      if (get_uint_from_user(&waterMark) == USER_INPUT_ERROR)
      {
        printf("\t\t Invalid Value!  Please enter correct value \n");
        continue;
      }
    
      if (waterMark == 0)
      {
        printf("\t\t Invalid Value!  Please enter correct value \n");
        continue;
      }
      break;
   }
  
    while (1)
    {
      printf("\t\t Enter sample periodicity[%u]:", BHDAPP_DEFAULT_PERIODICITY);
    
      if (get_uint_from_user(&samplePeriodicity) == USER_INPUT_ERROR)
      {
        printf("\t\t Invalid Value!  Please enter correct value \n");
        continue;
      }
    
      if ((samplePeriodicity < BHDAPP_MIN_SAMPLE_PERIODICITY) ||
          (samplePeriodicity > BHDAPP_MAX_SAMPLE_PERIODICITY))
      {
        printf("\t\t Invalid Value!  Please enter correct value (%d <= sample periodicity <= %d)\n", 
                            BHDAPP_MIN_SAMPLE_PERIODICITY, BHDAPP_MAX_SAMPLE_PERIODICITY);
        continue;
      }
      break;
   }
     
    while (1)
    {
      printf("\t\t Enter sample count[%u]:", BHDAPP_DEFAULT_SAMPLE_COUNT);
    
      if (get_uint_from_user(&sampleCount) == USER_INPUT_ERROR)
      {
        printf("\t\t Invalid Value!  Please enter correct value \n");
        continue;
      }
      if ((sampleCount > BHDAPP_MAX_SAMPLE_COUNT)) 
      {
        printf("\t\t Invalid Value!  Please enter correct value (%d <= sample count <= %d)\n", 
                            BHDAPP_MIN_SAMPLE_COUNT, BHDAPP_MAX_SAMPLE_COUNT);
        continue;
      }
    
      break;
   }
   jsonId = get_new_json_id();
   sprintf(agentSamplingParams, agentSamplingParamsFmt, waterMark, samplePeriodicity, sampleCount);
   sprintf(finalJsonBuf, jsonFrmt, asicId, portList, samplingMethod, agentSamplingParams, jsonId);
  }
  else if (strcmp(samplingMethod,"sflow") == 0)
  {
    printf("\n\t Provide sflow sampling params\n");
    printf ("\t\t Provide sflow encapsulation parameters:\n");
    while (1)
    {
      printf("\t\t If it is tagged packet, Enter vlan ID:");
    
      if (get_uint_from_user(&vlan) == USER_INPUT_ERROR)
      {
        printf("\t\t Invalid Value!  Please enter correct value \n");
        continue;
      }
    
	  if (vlan == 0)
      {
        break;         
      }
      if ((vlan > BHDAPP_MAX_VLAN))
      {
        printf("\n\t\t Out of range! Please enter correct value(vlan <= %d)\n", BHDAPP_MAX_VLAN);
        continue;
      }
      sprintf(vlan_id_str, vlan_id_str_frmt, vlan);
      break;
    }
  
    while (1)
    {
      printf("\t\t Provide destination IP address(a.b.c.d format) [%s]:", BHDAPP_DEFAULT_DEST_IP);
    
      if (get_ipv4_address_from_user(destIp) == USER_INPUT_ERROR)
      {
        printf("\t\t Invalid Input! Please provide correct value\n");
        continue; 
      }
      break;
    }
    while(1)
    {
      printf("\t\t Provide value for UDP source port[%u]:", BHDAPP_DEFAULT_SRC_UDP_PORT);
      if (get_uint_from_user(&sPort) == USER_INPUT_ERROR)
      {
        printf("\t\t Invalid Input! Please provide correct value\n");
        continue; 
      }
      
      if (sPort > 0xffff)
      {
        printf("\t\t Invalid Input! Please provide correct value(<= %u)\n", 0xffff);
        continue; 
      }
      break;
    }

    while(1)
    {
      printf("\t\t Provide value for UDP destination port[%u]:", BHDAPP_DEFAULT_DEST_UDP_PORT);
      if (get_uint_from_user(&dPort) == USER_INPUT_ERROR)
      {
        printf("\t\t Invalid Input! Please provide correct value\n");
        continue; 
      }
      
      if (dPort > 0xffff)
      {
        printf("\t\t Invalid Input! Please provide correct value(<= %u)\n", 0xffff);
        continue; 
      }
      break;
    }
     
    while(1)
    {
      printf("\t Provide value for mirror port[%s]:", BHDAPP_DEFAULT_MIRROR_PORT);
      if (get_string_from_user(mirrorPort, sizeof(mirrorPort)) == USER_INPUT_ERROR)
      {
        printf("\t Invalid Input! Please provide correct value\n");
        continue; 
      }
      
      break;
    }
    while (1)
    {
      printf("\t Provide value for sample pool size[%u]:", BHDAPP_DEFAULT_SAMPLE_POOL_SIZE);
    
      if (get_uint_from_user(&samplePoolSize) == USER_INPUT_ERROR)
      {
        printf("\t Invalid Value!  Please enter correct value \n");
        continue;
      }
    
      if ((samplePoolSize < BHDAPP_MIN_SAMPLE_POOL_SIZE))
      {
        printf("\t Invalid value!  Please enter correct value(%u <= sample pool size)\n",  BHDAPP_MIN_SAMPLE_POOL_SIZE);
        continue;
      }
      break;
   }
   jsonId = get_new_json_id();
   sprintf(sflowSamplingParams, sflowSamplingParamsFmt, vlan_id_str, destIp, sPort, dPort, mirrorPort, samplePoolSize);
   sprintf(finalJsonBuf, jsonFrmt, asicId, portList, samplingMethod, sflowSamplingParams, jsonId);

  }

  printf("\t Request sent to Agent with Id %u\n", jsonId);
  return 0;
}


/******************************************************************
 * @brief      Target function to read input from user to prepare 
 *             JSON buffer to cancel black hole. 
 *             If input is not provided for particular fields, 
 *             default values would be taken to prepare JSON 
 *
 * @param[in]   jsonFrmt       Format of the JSON buffer 
 *              finalJsonBuf   Buffer to hold the json message with user inputs 
 *
 * @retval   0    Successfully read user inputs and prepared json buffer
 *
 * @note     
 *********************************************************************/
int cancel_bhd_black_hole(char *jsonFrmt, char *finalJsonBuf)
{
  char asicId[16]  = BHDAPP_DEFAULT_ASIC_ID;
  unsigned int jsonId = 0;

  printf("\t Provide asic Id[%s] :", asicId);
  get_string_from_user(asicId, sizeof(asicId));

  jsonId = get_new_json_id();
  sprintf(finalJsonBuf, jsonFrmt, asicId, jsonId);
  printf("\t Request sent to Agent with Id %u\n", jsonId);
  return 0;
}


/******************************************************************
 * @brief      Target function to read input from user to prepare 
 *             JSON buffer to get black hole configuration. 
 *             If input is not provided for particular fields, 
 *             default values would be taken to prepare JSON 
 *
 * @param[in]   jsonFrmt       Format of the JSON buffer 
 *              finalJsonBuf   Buffer to hold the json message with user inputs 
 *
 * @retval   0    Successfully read user inputs and prepared json buffer
 *
 * @note     
 *********************************************************************/
int get_bhd_black_hole(char *jsonFrmt, char *finalJsonBuf)
{
  char asicId[16]  = BHDAPP_DEFAULT_ASIC_ID;
  unsigned int jsonId = 0;

  printf("\t Provide asic Id[%s] :", asicId);
  get_string_from_user(asicId, sizeof(asicId));

  jsonId = get_new_json_id();
  sprintf(finalJsonBuf, jsonFrmt, asicId, jsonId);
  printf("\t Request sent to Agent with Id %u\n", jsonId);
  return 0;
}

/******************************************************************
 * @brief      Target function to read input from user to prepare 
 *             JSON buffer to get sflow sampling status. 
 *             If input is not provided for particular fields, 
 *             default values would be taken to prepare JSON 
 *
 * @param[in]   jsonFrmt       Format of the JSON buffer 
 *              finalJsonBuf   Buffer to hold the json message with user inputs 
 *
 * @retval   0    Successfully read user inputs and prepared json buffer
 *
 * @note     
 *********************************************************************/
int get_bhd_sflow_sampling_status(char *jsonFrmt, char *finalJsonBuf)
{
  char asicId[16]  = BHDAPP_DEFAULT_ASIC_ID;
  char portList[4*1024] = {0};
  char tempPortList[1024] = BHDAPP_DEFAULT_PORT_LIST;
  unsigned int jsonId = 0;

  printf("\t Provide asic Id[%s] :", asicId);
  get_string_from_user(asicId, sizeof(asicId));
  
  printf("\t Provide port list[%s]:", tempPortList);
  sprintf (portList, "\"%s\"", tempPortList);
  get_port_list_from_user(portList, sizeof(portList));

  jsonId = get_new_json_id();
  sprintf(finalJsonBuf, jsonFrmt, asicId, portList, jsonId);
  printf("\t Request sent to Agent with Id %u\n", jsonId);
  return 0;
}

/******************************************************************
 * @brief      Function to provide (main)features menu to user  
 *
 * @param[in]   bhdRestMessages  Buffer that holds the rest message for 
 *                              different methods. And the JSON buffer of
 *                              user selected method would get updated with
 *                              JSON string
 *
 *              restMesgIndex   User selected method  
 *
 * @retval   none   
 *
 * @note     
 *********************************************************************/
void bhdapp_get_user_input(BHDAPP_REST_MSG_t *bhdRestMessages, unsigned int *restMesgIndex)
{
  BHDAPP_JSON_METHOD_t  displayMethod = 0;
  BHDAPP_JSON_METHOD_t  selectedMethod = 0;
  unsigned int index = 0;
  unsigned int option;
  unsigned int numOfElements = 0;
  BHDAPP_USER_INPUT_STATUS_t retValue;
 
  /* Display main menu options */
  numOfElements = sizeof(bhdJsonMethodDetails)/sizeof(BHDAPP_JSON_METHOD_INFO_t); 
  printf("\n");
  for (displayMethod = 0; displayMethod < BHDAPP_JSON_METHOD_LAST; displayMethod++)
  {
    for (index = 0; index < numOfElements; index++)
    {
      if (bhdJsonMethodDetails[index].methodId == displayMethod)
      {
        printf("%u.%s\n", (displayMethod+1), bhdJsonMethodDetails[index].menuString); 
        break;
      }
    }
  }

  printf("%u.Exit\n", (BHDAPP_JSON_METHOD_LAST+1));
  printf("Select an option from the menu:");
  retValue = get_uint_from_user(&option);

  if (retValue == USER_INPUT_ERROR)
  {
    printf("\n Invalid input! Please choose correct option \n");
    bhdapp_get_user_input(bhdRestMessages, restMesgIndex);
    return;
  }
 
  if (retValue == USER_INPUT_NONE)
  {
    bhdapp_get_user_input(bhdRestMessages, restMesgIndex);
    return;
  }

  if (option == (BHDAPP_JSON_METHOD_LAST+1))
  {
    exit(0);
  }
  
  if (option > (BHDAPP_JSON_METHOD_LAST+1))
  { 
    printf("\n Invalid Option %u! Please choose correct option \n", option);
    bhdapp_get_user_input(bhdRestMessages, restMesgIndex);
    return;
  }

  selectedMethod = (option - 1);
  memset(jsonBufGlobal, 0x00, sizeof(jsonBufGlobal));

  for (index = 0; index < BHDAPP_JSON_METHOD_LAST; index++)
  {
    if (selectedMethod == bhdJsonMethodDetails[index].methodId)
    {
      if (bhdJsonMethodDetails[index].get_input_for_json(bhdJsonMethodDetails[index].jsonFrmt, jsonBufGlobal) == -1)
      {
        bhdapp_get_user_input(bhdRestMessages, restMesgIndex);
        return;
      }
      break;
    }
  }
  
  for (index = 0; index < BHDAPP_JSON_METHOD_LAST; index++)
  {
    if (selectedMethod == bhdRestMessages[index].methodId)
    {
      bhdRestMessages[index].json = jsonBufGlobal;
      *restMesgIndex = index;
      break;
    }
  }
  return;
}
