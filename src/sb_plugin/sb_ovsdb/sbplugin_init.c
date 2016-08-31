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

#include "sbplugin.h"
#include "sbplugin_bst.h"
#include "sbplugin_system.h"
#include "sbplugin_system_map.h"
#include "sbplugin_ovsdb.h"
#include "sb_redirector_api.h"

/* BST feature data structure*/
static BVIEW_SB_BST_FEATURE_t       ovsdbBstFeat;
/* SYSTEM feature data structure*/
static BVIEW_SB_SYSTEM_FEATURE_t    ovsdbSystemFeat;
/* SB Plugin data structure*/
static BVIEW_SB_PLUGIN_t sbPlugin;

/* Flag to enable/disable debug */
int sbOvsdbDebugFlag = false;

/*********************************************************************
* @brief    OVSDB South bound plugin init
*
* @retval   BVIEW_STATUS_SUCCESS if BST feature is
*                                initialized successfully.
* @retval   BVIEW_STATUS_FAILURE if initialization fails.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_common_init ()
{
  BVIEW_STATUS      rv = BVIEW_STATUS_SUCCESS;
  unsigned int      featureIndex = 0;

  sbPlugin.numSupportedFeatures = 0;
  
  /* Init SYSTEM feature*/
  rv = sbplugin_ovsdb_system_init (&ovsdbSystemFeat);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    SB_OVSDB_DEBUG_PRINT ("Failed to Register OVSDB plugin");
    return rv;
  }
  sbPlugin.featureList[featureIndex] = (BVIEW_SB_FEATURE_t *)&ovsdbSystemFeat;
  sbPlugin.numSupportedFeatures++;
  featureIndex++;

  /* Init BST feature*/  
  rv = sbplugin_ovsdb_bst_init (&ovsdbBstFeat);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    SB_OVSDB_DEBUG_PRINT ("Failed to Intialize OVSDB BST feature");
    return rv;
  }
  sbPlugin.featureList[featureIndex] = (BVIEW_SB_FEATURE_t *)&ovsdbBstFeat;
  sbPlugin.numSupportedFeatures++;
  ovsdbSystemFeat.featureMask |= BVIEW_FEATURE_BST; 
  featureIndex++;

  /* Register OVSDB plugin to the sb-redirector*/
  rv = sb_plugin_register (sbPlugin);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    SB_OVSDB_DEBUG_PRINT ("Failed to Register OVSDB plugin");
    return rv;
  }
  
  return rv;
}
  
