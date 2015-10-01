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

#include "sbplugin.h"
#include "sbplugin_common_bst.h"
#include "sbplugin_common_system.h"
#include "sbplugin_common_packet_trace.h"
#include "sbplugin_common.h"
#include "sb_redirector_api.h"

/* BST feature data structure*/
BVIEW_SB_BST_FEATURE_t       bcmBst;
/* SYSTEM feature data structure*/
BVIEW_SB_SYSTEM_FEATURE_t    bcmSystem;
/* Packet Trace feature data structure*/
BVIEW_SB_PT_FEATURE_t       bcmPT;
/* SB Plugin data structure*/
BVIEW_SB_PLUGIN_t sbPlugin;

/* Flag to enable/disable debug */
int sbSdkDebugFlag = false;

/*********************************************************************
* @brief    South bound plugin init
*
* @retval   BVIEW_STATUS_SUCCESS if BST feature is
*                                initialized successfully.
* @retval   BVIEW_STATUS_FAILURE if initialization is failed.
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
  rv = sbplugin_common_system_init (&bcmSystem);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    SB_LOG (BVIEW_LOG_ERROR,"Failed to Register plugin");
    return rv;
  }
  sbPlugin.featureList[featureIndex] = (BVIEW_SB_FEATURE_t *)&bcmSystem;
  sbPlugin.numSupportedFeatures++;
  featureIndex++;

  /* Init BST feature*/  
  rv = sbplugin_common_bst_init (&bcmBst);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    SB_LOG (BVIEW_LOG_ERROR,"Failed to Intialize BST feature");
    return rv;
  }
  sbPlugin.featureList[featureIndex] = (BVIEW_SB_FEATURE_t *)&bcmBst;
  sbPlugin.numSupportedFeatures++;
  bcmSystem.featureMask |= BVIEW_FEATURE_BST; 
  featureIndex++;

  /* Init Packet Trace Feature*/
  rv = sbplugin_common_packet_trace_init (&bcmPT);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    SB_LOG (BVIEW_LOG_ERROR,"Failed to Intialize Packet Trace feature");
    return rv;
  }
  sbPlugin.featureList[featureIndex] = (BVIEW_SB_FEATURE_t *)&bcmPT;
  sbPlugin.numSupportedFeatures++;
  bcmSystem.featureMask |= BVIEW_FEATURE_PACKET_TRACE; 
  /* Register plugin to the sb-redirector*/
  rv = sb_plugin_register (sbPlugin);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    SB_LOG (BVIEW_LOG_ERROR,"Failed to Register plugin");
    return rv;
  }

  return rv;
}
  
