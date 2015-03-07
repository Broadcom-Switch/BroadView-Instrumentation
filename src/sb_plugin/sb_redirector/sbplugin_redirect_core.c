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

#include <pthread.h>
#include "sbplugin_redirect.h"
#include "sbplugin_system.h"

/* sbRedirectPluginList holds the south bound plug-ins that are registered with
   redirector
 */
BVIEW_SB_REDIRECT_PLUGIN_t sbRedirectPluginList[BVIEW_MAX_SUPPORTED_SB_PLUGINS];

/* Read-Write lock for south bound plug-in redirector local data */
pthread_rwlock_t sbRedirectRWLock;

/* Debug flag                    */
int sbRedirectDebugFlag = 0;

/*********************************************************************
* @brief       Initialize South bound redirector's local data with defaults
*
* @retval   BVIEW_STATUS_FAILURE   if failed to initialize read-write lock
*
* @retval   BVIEW_STATUS_SUCCESS   All the data structures are sucessfully 
*                                  initialized
*
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sb_redirector_init ()
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

  /* clear sbRedirectPluginList */
  memset (sbRedirectPluginList, 0x00, sizeof (sbRedirectPluginList));
  /* Initialize Read Write lock with default attributes */
  if (pthread_rwlock_init (&sbRedirectRWLock, NULL) != 0)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to create read write lock with deafault attributes\n",
                             __FILE__, __LINE__);
    rv = BVIEW_STATUS_FAILURE;
  }
  return rv;
}

/*********************************************************************
* @brief   Get Function list for a particular feature based on Silicon type.
*          A function list is selected in the following order.
*          1. If an exact match for a silicon is found then that function list 
*                                                                  is returned.
*          2. Else return a function list that has support for 
*                                             all silicon types.
*                               
*
* @param[in]  asicType            Asic type
* @param[in]  featureId           Feature ID
*
* @retval   NULL                   if function fails
* @retval   FunctionListPtr        Function list that matches with 
*                                            the silicon type(/all) 
*
*
* @notes    none
*
***************************************************************************************/
void *sb_redirect_feature_handle_get (BVIEW_ASIC_TYPE asicType,
                                      BVIEW_FEATURE_ID featureId)
{
  unsigned int sbPluginIndex = 0;
  unsigned int featureListIndex = 0;
  BVIEW_SB_PLUGIN_t *sbPluginPtr = NULL;
  BVIEW_SB_FEATURE_t *featurePtr = NULL;
  BVIEW_SB_FEATURE_t *featurePtrOut = NULL;
  bool asicEntryFound = false;
  bool allAsicEntryFound = false;

  /* Acquire Read lock */
  if (pthread_rwlock_rdlock (&sbRedirectRWLock) != 0)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to take SB redirect read write lock for read\n",
                             __FILE__, __LINE__);
    return NULL;
  }

  /* Loop through the plug-in list */
  for (sbPluginIndex = 0;
       sbPluginIndex < (sizeof (sbRedirectPluginList) / sizeof (sbRedirectPluginList[0]))
       && (asicEntryFound == false); sbPluginIndex++)
  {                             /* Check for in use entry */
    if (sbRedirectPluginList[sbPluginIndex].isInUse == true)
    {
      sbPluginPtr = &sbRedirectPluginList[sbPluginIndex].sbPlugin;
      /* Loop through the list of features */
      for (featureListIndex = 0;
           (featureListIndex < sbPluginPtr->numSupportedFeatures)
           && (asicEntryFound == false); featureListIndex++)
      {
        featurePtr = sbPluginPtr->featureList[featureListIndex];
        if (featurePtr == NULL)
        {
          continue;
        }
        /* Check for feature ID */
        if (featurePtr->featureId == featureId)
        {
          /* Corresponding asic entry for the feature is found */
          if ((featurePtr->supportedAsicMask == BVIEW_ASIC_TYPE_ALL)
              && (allAsicEntryFound == false))
          {
            featurePtrOut = featurePtr;
            allAsicEntryFound = true;
          }
          else if (((featurePtr->supportedAsicMask & asicType) == asicType)
                   && (asicEntryFound == false))
          { 
	        /* All asic entry is found */
            featurePtrOut = featurePtr;
            asicEntryFound = true;
          }
        }
      } /* End of for (featureListIndex = 0; .... */
    }  /* End of if (sbRedirectPluginList[sb..... */
  }   /* End of for (sbPluginIndex = 0; (sb..... */

  /* Entry is found */
  if ((asicEntryFound == true) || (allAsicEntryFound == true))
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_INFO,
                             "(%s:%d) SB plugin entry is found for feature %d silicon %d\n",
                             __FILE__, __LINE__, featureId, asicType);
  }
  else
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to find SB plugin entry for feature %d, silicon %d\n",
                             __FILE__, __LINE__, featureId, asicType);
  }
  /* Release the read lock */
  if (pthread_rwlock_unlock (&sbRedirectRWLock) != 0)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to release SB redirect read write lock after read\n",
                             __FILE__, __LINE__);
  }

  return (void *) featurePtrOut;
}

/*********************************************************************
* @brief       Get asic capabilities of a silicon
*
* @param[in]  asicType        Asic type
* @param[out] asicCapPtr      Asic capabilities pointer
*
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     System feature is not present or
*                                     Asic capabilities for a particular 
*                                                         asic not found
*
* @retval   BVIEW_STATUS_SUCCESS      Asic capabilities are present 
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sb_redirect_asic_capabilities_get (BVIEW_ASIC_TYPE asicType,
                                                BVIEW_ASIC_CAPABILITIES_t **asicCapPtr)
{
  unsigned int asicListIndex = 0;
  BVIEW_ASIC_t *asicListPtr = NULL;
  bool asicEntryFound = false;
  BVIEW_SB_SYSTEM_FEATURE_t *systemFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

  /* Acquire Read Lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);

  /* Get system feature's south bound plug-in pointer */
  systemFeaturePtr =
    (BVIEW_SB_SYSTEM_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                                  BVIEW_FEATURE_SYSTEM);
  /* Check for NULL */
  if (systemFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }
  else
  {
    /* Loop through the asics list in system south bound plug-in */
    for (asicListIndex = 0;
         (asicListIndex < systemFeaturePtr->numSupportedAsics)
         && (asicEntryFound == false); asicListIndex++)
    {
      asicListPtr = systemFeaturePtr->asicList[asicListIndex];
      /* Validate asicListPtr */
      if (asicListPtr == NULL)
      {
        /* Go to next entry */
        continue;
      }
      /* Check for asic type */
      if (asicListPtr->asicType == asicType)
      {
        /* Correct entry found */
        *asicCapPtr = &(asicListPtr->scalingParams);
        asicEntryFound = true;
      }
    } /* End of for (featureListIndex = 0; .... */

    /* Asic entry is found */
    if (asicEntryFound == true)
    {
      SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_INFO,
                               "(%s:%d) Asic Cap entry is found for silicon %d\n",
                               __FILE__, __LINE__, asicType);
      rv = BVIEW_STATUS_SUCCESS;
    }
    else
    {
      SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                               "(%s:%d) Failed to find Asic Cap entry for silicon %d\n",
                               __FILE__, __LINE__, asicType);
      rv = BVIEW_STATUS_FAILURE;
    }
  }
  /* Release the read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);

  return rv;
}

/*********************************************************************
* @brief       Validate a south bound plug-in w.r.t to max supported 
*                 features  in a plug-in, and w.r.t to max supported 
*                 units on a system   
*
* @param[in]  sbPlugin        South bound plug-in
*
*
* @retval   BVIEW_STATUS_FAILURE   Validation fails w.r.t max supported
*                                  features or max supported units on a system
*
* @retval   BVIEW_STATUS_SUCCESS   sb plug-in has valid content
*
*
* @notes    none
*
*********************************************************************/
static BVIEW_STATUS sb_plugin_validate (BVIEW_SB_PLUGIN_t sbPlugin)
{
  BVIEW_SB_SYSTEM_FEATURE_t *systemFeaturePtr = NULL;
  unsigned int featureListIndex = 0;
  BVIEW_SB_FEATURE_t *featurePtr = NULL;

  /* Check for the supported number of features limit */
  if (sbPlugin.numSupportedFeatures > BVIEW_SBPLUGIN_MAX_FEATURES)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Number of supported features %d in SB plugin are more than"
                             " max supported limit BVIEW_SBPLUGIN_MAX_FEATURES=%d\n",
                             __FILE__, __LINE__, sbPlugin.numSupportedFeatures,
                             BVIEW_SBPLUGIN_MAX_FEATURES);
    return BVIEW_STATUS_FAILURE;
  }

  for (featureListIndex = 0; (featureListIndex < sbPlugin.numSupportedFeatures);
       (featureListIndex++))
  {
    featurePtr = sbPlugin.featureList[featureListIndex];
    if (featurePtr == NULL)
    {
      continue;
    }
    /* Check for feature ID */
    if (featurePtr->featureId == BVIEW_FEATURE_SYSTEM)
    {
      systemFeaturePtr = (BVIEW_SB_SYSTEM_FEATURE_t *) featurePtr;
      /* Check with max supported units on a platform */
      if (systemFeaturePtr->numSupportedAsics > BVIEW_MAX_ASICS_ON_A_PLATFORM)
      {
        SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                                 "(%s:%d) Number of asics supported %d in System plugin are "
                                 "more than max supported limit BVIEW_MAX_ASICS_ON_A_PLATFORM = %d\n",
                                 __FILE__, __LINE__,
                                 systemFeaturePtr->numSupportedAsics,
                                 BVIEW_MAX_ASICS_ON_A_PLATFORM);
        return BVIEW_STATUS_FAILURE;
      }
    }
  }                             /* End of for (featureListIndex = 0; .... */

  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief       Register a south bound plug-in with
*                               south bound redirector
*
* @param[in]  sbPlugin        South bound plug-in
*
*
* @retval   BVIEW_STATUS_FAILURE   When validation of sb plug-in fails or
*                                  Lock acquistion fails 
*
* @retval   BVIEW_STATUS_TABLE_FULL  Number of plug-ins already registered 
*                                    are BVIEW_MAX_SUPPORTED_SB_PLUGINS.
*
* @retval   BVIEW_STATUS_SUCCESS    Successfully registered sb plug-in with 
*                                    sb redirector
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sb_plugin_register (BVIEW_SB_PLUGIN_t sbPlugin)
{
  unsigned int sbPluginIndex = 0;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

  /* Validate SB plug-in */
  if (sb_plugin_validate (sbPlugin) == BVIEW_STATUS_FAILURE)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) SB plug-in validation failed\n", __FILE__,
                             __LINE__);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire write lock */
  SB_REDIRECT_RWLOCK_WR_LOCK (sbRedirectRWLock);

  /* Loop through the south bound plug-in list to find out free entry */
  for (sbPluginIndex = 0; (sbPluginIndex < BVIEW_MAX_SUPPORTED_SB_PLUGINS);
       sbPluginIndex++)
  {
    if (sbRedirectPluginList[sbPluginIndex].isInUse == false)
    {                           /* Found free entry */
      sbRedirectPluginList[sbPluginIndex].sbPlugin = sbPlugin;
      sbRedirectPluginList[sbPluginIndex].isInUse = true;
      SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_INFO,
                               "(%s:%d) Free SB plugin entry is at index %d \n",
                               __FILE__, __LINE__, sbPluginIndex);
      break;
    }
  }
  /* Failed to find Free entry */
  if (sbPluginIndex == BVIEW_MAX_SUPPORTED_SB_PLUGINS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to find Free SB plugin entry \n",
                             __FILE__, __LINE__);
    rv = BVIEW_STATUS_TABLE_FULL;
  }
  /* Release write lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief       Set debug flag value for south bound plug-in redirector
*
* @param[in]   val       Value
*
*
* @retval   none
*
* @notes    none
*
*********************************************************************/
void sb_redirect_debug_set (int val)
{
  sbRedirectDebugFlag = val;
}
