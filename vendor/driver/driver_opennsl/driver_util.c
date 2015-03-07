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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sal/commdefs.h>
#include <sal/driver.h>
#include <opennsl/error.h>
#include <opennsl/cosq.h>
#include <opennsl/vlan.h>
#include <opennsl/switch.h>


#define soc_ndev 1
#define DEFAULT_VLAN 1

/*****************************************************************//**
* \brief Include all ports to default vlan
*
* \param unit   [IN]    unit number
*
* \return OPENNSL_E_XXX     OpenNSL API return code
  ********************************************************************/
int switch_default_vlan_config(int unit)
{
  opennsl_port_config_t pcfg;
  opennsl_pbmp_t upbm;
  int rv;

  OPENNSL_PBMP_CLEAR(upbm);
  /*
   * Create VLAN with id DEFAULT_VLAN and
   * add ethernet ports to the VLAN
   */
  rv = opennsl_port_config_get(unit, &pcfg);
  if (rv != OPENNSL_E_NONE) {
    printf("Failed to get port configuration. Error %s\n", opennsl_errmsg(rv));
    return rv;
  }

  rv = opennsl_vlan_port_add(unit, DEFAULT_VLAN, pcfg.e, upbm);
  if (rv != OPENNSL_E_NONE) {
    printf("Failed to add ports to VLAN. Error %s\n", opennsl_errmsg(rv));
    return rv;
  }

  return 0;
}


int driverInit(void)
{
  int                 rc = 1;
  int i= 0, pri = 0;

  rc = opennsl_driver_init();
  if (rc != 0)
  {
    return rc;
  }
  /* create the default vlan */

  rc = switch_default_vlan_config(0);

  for (i = 0; i < soc_ndev; i++) {
    for (pri=0;pri<8;pri++){
     opennsl_cosq_mapping_set (i,pri,pri); 
    }
  }


  return rc;
}

int openapps_driver_init(void)
{
  int rc;
  static int systemMappingInitialized = 0;

  if(systemMappingInitialized != 0)
  {
    printf("\r\nDriver is already initialized.\r\n");
    return 0;
  }
  rc = driverInit();
  if (0 != rc)
  {
    printf("\r\nError initializing driver, rc = %d.\r\n", rc);
    return -1;
  }
  systemMappingInitialized = 1;

  return 0;
}


