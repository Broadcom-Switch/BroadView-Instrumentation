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

#include <inttypes.h>
#include <time.h>  
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "sbplugin.h"
#include "sbplugin_system.h"
#include "sbplugin_ovsdb.h"
#include "sbplugin_system_map.h"

/* Array to hold ASIC properties data based for Maximum platforms*/
BVIEW_ASIC_t                 asicDb[BVIEW_MAX_ASICS_ON_A_PLATFORM];

/* Intialize ASIC number Mapping. The Index to table is Applcation ASIC number
 * Application asic numbering starts with '1', so to avoid for loop to get uint
 * the index to the table is Application asic number.
 */

int  asicMap[BVIEW_MAX_ASICS_ON_A_PLATFORM+1] =
                    /* asic */
                    /* -------- */
                    {  -1     ,
                        0     };
int  asicAppMap[BVIEW_MAX_ASICS_ON_A_PLATFORM] = {1};

/*********************************************************************
* @brief   Get the chip type
*
* @param[in]   unit              - unit
* @param[out]  asicType          - ASIC type
*
* @retval BVIEW_STATUS_SUCCESS            if chip get is success.
* @retval BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS sbplugin_ovsdb_system_util_chip_type_get (unsigned int unit,
                                       BVIEW_ASIC_TYPE *asicType)
{
  *asicType = BVIEW_ASIC_TYPE_TD2;
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Init TD2 scaling parametrs.
*
* @param [in,out]  asic               - ASIC specific info data
*
* @retval BVIEW_STATUS_SUCCESS            if init is success.
* @retval BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS sbplugin_ovsdb_system_util_td2_init (BVIEW_ASIC_t *asic)
{
  if (asic == NULL)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  asic->scalingParams.numUnicastQueues = BVIEW_TD2_NUM_UC_QUEUE;
  asic->scalingParams.numUnicastQueueGroups = BVIEW_TD2_NUM_UC_QUEUE_GRP;
  asic->scalingParams.numMulticastQueues = BVIEW_TD2_NUM_MC_QUEUE;
  asic->scalingParams.numServicePools = BVIEW_TD2_NUM_SP;
  asic->scalingParams.numCommonPools = BVIEW_TD2_NUM_COMMON_SP;
  asic->scalingParams.numCpuQueues = BVIEW_TD2_CPU_COSQ;
  asic->scalingParams.numRqeQueues = BVIEW_TD2_NUM_RQE;
  asic->scalingParams.numRqeQueuePools = BVIEW_TD2_NUM_RQE_POOL;
  asic->scalingParams.numPriorityGroups = BVIEW_TD2_NUM_PG;
  asic->scalingParams.cellToByteConv = BVIEW_TD2_CELL_TO_BYTE;

  asic->scalingParams.support1588 = BVIEW_TD2_1588_SUPPORT;

  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  SYSTEM feature init
*
* @param[in,out]   ovsdbSystemFeat   -  system data structure
*
* @returns BVIEW_STATUS_SUCCESS  if intialization is success
*          BVIEW_STATUS_FAILURE  if intialization is fail
*
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_system_init (BVIEW_SB_SYSTEM_FEATURE_t    *ovsdbSystemFeat)
{
  unsigned int     num_front_panel_ports = 0;
  unsigned int     index = 0;
  unsigned int     unit = 0;
  unsigned int     num_ports = 100;
  unsigned int     max_units = 0;

  if (ovsdbSystemFeat == NULL)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  memset (ovsdbSystemFeat, 0x00, sizeof (BVIEW_SB_SYSTEM_FEATURE_t));
  ovsdbSystemFeat->feature.featureId           = BVIEW_FEATURE_SYSTEM;
  ovsdbSystemFeat->feature.supportedAsicMask   = BVIEW_SYSTEM_SUPPORT_MASK;
  ovsdbSystemFeat->numSupportedAsics = 0;

  max_units  =1;

  /* Find out the attached ASIC and fill scaling parametrs*/
  for (unit = 0; unit < max_units; unit++)
  {
    ovsdbSystemFeat->numSupportedAsics++;
    num_front_panel_ports = num_ports;
    asicDb[index].scalingParams.numPorts  = num_front_panel_ports;

    asicDb[index].unit = unit;
    sbplugin_ovsdb_system_util_chip_type_get (unit, &asicDb[index].asicType);

    /* Based on the CHIP call appropriate init function*/
    if (BVIEW_ASIC_TYPE_TD2 == asicDb[index].asicType || 
        BVIEW_ASIC_TYPE_TH  == asicDb[index].asicType)
    {
      sbplugin_ovsdb_system_util_td2_init (&asicDb[index]);
    }
      
    ovsdbSystemFeat->asicList[index] = &asicDb[index];
    index++;
  }    
  ovsdbSystemFeat->system_name_get_cb     = sbplugin_ovsdb_system_name_get;
  ovsdbSystemFeat->system_mac_get_cb      = sbplugin_ovsdb_system_mac_get;
  ovsdbSystemFeat->system_ip4_get_cb      = sbplugin_ovsdb_system_ipv4_get;
  ovsdbSystemFeat->system_time_get_cb     = sbplugin_ovsdb_system_time_get;
  ovsdbSystemFeat->system_asic_translate_from_notation_cb      = sbplugin_ovsdb_system_asic_translate_from_notation;
  ovsdbSystemFeat->system_port_translate_from_notation_cb      = sbplugin_ovsdb_system_port_translate_from_notation;
  ovsdbSystemFeat->system_asic_translate_to_notation_cb        = sbplugin_ovsdb_system_asic_translate_to_notation;
  ovsdbSystemFeat->system_port_translate_to_notation_cb        = sbplugin_ovsdb_system_port_translate_to_notation;
  ovsdbSystemFeat->system_network_os_get_cb                    = sbplugin_ovsdb_system_network_os_get;

  return BVIEW_STATUS_SUCCESS;
} 

/*********************************************************************
* @brief  Get the system name
*
* @param[out] buffer                         - buffer
* @param[in]  length                         - length of the buffer
*
* @retval  BVIEW_STATUS_SUCCESS            if Name get is success.
* @retval  BVIEW_STATUS_FAILURE            if Name get is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_ovsdb_system_name_get (char *buffer, int length)
{
  if (buffer == NULL)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  strncpy (buffer, "OVSDB-PLUGIN", length);
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Get the MAC address of the system
*
* @param[out] buffer                         - buffer
* @param[in]  length                         - length of the buffer
*
* @retval  BVIEW_STATUS_SUCCESS            if MAC get is success.
* @retval  BVIEW_STATUS_FAILURE            if MAC get is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes    Get MAC address of the service port.
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_ovsdb_system_mac_get (unsigned char *buffer, 
                                           int length)
{
  int fd;
  struct ifreq ifr;
  BVIEW_STATUS  rv = BVIEW_STATUS_FAILURE;

  if (buffer == NULL)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  fd = socket(AF_INET, SOCK_DGRAM, 0);

  if (fd > -1)
  {
    ifr.ifr_addr.sa_family = AF_INET;
    snprintf(ifr.ifr_name, IFNAMSIZ-1, "%s", "eth0");

    if (ioctl(fd, SIOCGIFHWADDR, &ifr) != -1)
    {
      memcpy(buffer, ifr.ifr_hwaddr.sa_data, length);
      rv = BVIEW_STATUS_SUCCESS;
    }
    close(fd);
  }

  return rv;
}

/*********************************************************************
* @brief  Get the IP address of system
*
* @param[out] buffer                         - buffer
* @param[in]  length                         - length of the buffer
*
* @retval  BVIEW_STATUS_SUCCESS            if IP get is success.
* @retval  BVIEW_STATUS_FAILURE            if IP get is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes    Get IP address of service port.
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_system_ipv4_get (unsigned char *buffer, 
                                           int length)
{
  int fd;
  struct ifreq ifr;
  BVIEW_STATUS  rv  = BVIEW_STATUS_FAILURE;

  if (buffer == NULL)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 
  fd = socket(AF_INET, SOCK_DGRAM, 0);

  if (fd > -1)
  {
    /* Get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* IP address attached to "eth0" */
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
    if (ioctl(fd, SIOCGIFADDR, &ifr) != -1)
    {
      memcpy (buffer, &((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr, length); 
      rv = BVIEW_STATUS_SUCCESS;
    }
    close(fd);
  }

  return rv;
}

/*********************************************************************
* @brief  Get Current local time.
*
* @param[out] tm                          - Pointer to tm structure
*
* @retval  BVIEW_STATUS_SUCCESS            if time get is success.
* @retval  BVIEW_STATUS_FAILURE            if time get is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes    Get IP address of service port.
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_ovsdb_system_time_get (time_t *ptime)
{
  /* NULL pointer check*/
  SB_OVSDB_NULLPTR_CHECK (ptime, BVIEW_STATUS_INVALID_PARAMETER);

  time (ptime);
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Translate ASIC String notation to ASIC Number.
*
* @param[in]  src                         - ASIC ID String
* @param[out] asic                        - ASIC Number
*
* @retval  BVIEW_STATUS_SUCCESS            if ASIC Translation is success.
* @retval  BVIEW_STATUS_FAILURE            if ASIC Translation is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes   
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_system_asic_translate_from_notation (char *src, 
                                                               int *asic) 
{
  SB_OVSDB_NULLPTR_CHECK (src, BVIEW_STATUS_INVALID_PARAMETER);
  SB_OVSDB_NULLPTR_CHECK (asic, BVIEW_STATUS_INVALID_PARAMETER);

  /* Convert to Interger*/
  *asic = atoi(src);
 
  if (*asic > (BVIEW_MAX_ASICS_ON_A_PLATFORM))
     return BVIEW_STATUS_INVALID_PARAMETER;
 
  /* Get OVSDB ASIC ID from mapping table*/
  BVIEW_OVSDB_ASIC_GET (*asic)

  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Translate Port String notation to Port Number.
*
* @param[in]   src                         - Port ID String
* @param[out]  port                        - PortId
*
* @retval  BVIEW_STATUS_SUCCESS            if Port Tranlate is success.
* @retval  BVIEW_STATUS_FAILURE            if Port  is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes  
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_system_port_translate_from_notation (char *src, 
                                                               int *port) 
{
  SB_OVSDB_NULLPTR_CHECK (src, BVIEW_STATUS_INVALID_PARAMETER);
  SB_OVSDB_NULLPTR_CHECK (port, BVIEW_STATUS_INVALID_PARAMETER);
  
  /* Convert to Interger*/
  *port = atoi(src); 
  
  /* Get OVSDB port*/
  BVIEW_OVSDB_PORT_GET (*port);

  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Translate ASIC number to ASIC string notation.
*
* @param[in]   asic                         - ASIC ID
* @param[out]  dst                          - ASIC ID String
*
* @retval  BVIEW_STATUS_SUCCESS            if ASIC ID Tranlate is success.
* @retval  BVIEW_STATUS_FAILURE            if ASIC ID Tranlate is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes 
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_system_asic_translate_to_notation (int asic, 
                                                             char *dst) 
{
  SB_OVSDB_NULLPTR_CHECK (dst, BVIEW_STATUS_INVALID_PARAMETER);

  if (asic >= (BVIEW_MAX_ASICS_ON_A_PLATFORM))
     return BVIEW_STATUS_INVALID_PARAMETER;

  /* Get OVSDB ASIC ID from mapping table*/
  BVIEW_OVSDB_APP_ASIC_GET (asic)
  
  /* Convert to String*/
  sprintf(dst, "%d", asic); 

  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief   Translate port number to port string notation.
*
* @param[in]   asic                         - ASIC 
* @param[in]   port                         - Port Number
* @param[out]  dst                          - ASIC String
*
* @retval  BVIEW_STATUS_SUCCESS            if Port Tranlate is success.
* @retval  BVIEW_STATUS_FAILURE            if Port Tranlate is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes  
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_system_port_translate_to_notation (int asic, 
                                                             int port, 
                                                             char *dst) 
{
  SB_OVSDB_NULLPTR_CHECK (dst, BVIEW_STATUS_INVALID_PARAMETER);
   
  /* Get OVSDB port*/
  BVIEW_OVSDB_PORT_GET (port);
  
  /* Convert to String*/
  sprintf(dst, "%d", port); 

  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief   Verify whether a given unit number is valid or not.
*
* @param[in]   unit                     -  unit number
*
* @retval  BVIEW_STATUS_SUCCESS            if it is a valid unit 
* @retval  BVIEW_STATUS_FAILURE            if it is not a valid unit
*
* @notes  
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_valid_unit_check(unsigned int unit)
{
  int index = 0;
  BVIEW_STATUS rv = BVIEW_STATUS_FAILURE; 
 
  for (index = 0; index < BVIEW_MAX_ASICS_ON_A_PLATFORM; index++)
  {
     if (asicDb[index].unit == unit)
     {
       rv = BVIEW_STATUS_SUCCESS;
       break;
     }
  }

  return rv;
}

/*********************************************************************
* @brief       Get Network OS
*
* @param[out]  buffer                 Pointer to network OS String
* @param[in]   length                 length of the buffer
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure
*                                     Failed to get network os
*
* @retval   BVIEW_STATUS_SUCCESS      Network OS is successfully
*                                     queried
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS  sbplugin_ovsdb_system_network_os_get (uint8_t *buffer, int length)
{
  SB_OVSDB_NULLPTR_CHECK (buffer, BVIEW_STATUS_INVALID_PARAMETER);

  memcpy (buffer, SBPLUGIN_NETWORK_OS, length);
  return BVIEW_STATUS_SUCCESS;
}

