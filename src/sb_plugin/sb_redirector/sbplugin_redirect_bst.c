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

#include "sbplugin_redirect.h"
#include "sbfeature_bst.h"
#include "sbplugin_redirect_system.h"

/*********************************************************************
* @brief       Get BST configuration
*
* @param[in]    asic                  Unit number
* @param[out]   config                BST config structure
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      BST config get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  BST config get functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_config_get (int asic, BVIEW_BST_CONFIG_t * config)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }       
  else if (bstFeaturePtr->bst_config_get_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {  
    rv = bstFeaturePtr->bst_config_get_cb (asic, config);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief       Set BST configuration
*
* @param[in]    asic                  Unit number
* @param[in]    config                BST config structure
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      BST config set is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  BST config set functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_config_set (int asic, BVIEW_BST_CONFIG_t * config)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }     
  else if (bstFeaturePtr->bst_config_set_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  { 
    rv = bstFeaturePtr->bst_config_set_cb (asic, config);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief       Get BST snapshot
*
* @param[in]     asic                  Unit number
* @param[out]    snapshot              BST snapshot
* @param[out]    time                  Time
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      BST snapshot get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  BST snapshot get functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_snapshot_get (int asic,
                                     BVIEW_BST_ASIC_SNAPSHOT_DATA_t * snapshot,
                                     BVIEW_TIME_t * time)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }  
  else if (bstFeaturePtr->bst_snapshot_get_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  { 
    rv = bstFeaturePtr->bst_snapshot_get_cb (asic, snapshot, time);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Obtain Device Statistics
*
* @param[in]    asic             unit
* @param[out]   data             Device data structure
* @param[out]   time             time
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Device stat get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Device stat get functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_device_data_get (int asic,
                                        BVIEW_BST_DEVICE_DATA_t * data,
                                        BVIEW_TIME_t * time)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }

  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }  
  else if (bstFeaturePtr->bst_device_data_get_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  { 
    rv = bstFeaturePtr->bst_device_data_get_cb (asic, data, time);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Obtain Ingress Port + Priority Groups Statistics
*
* @param[in]    asic              unit
* @param[out]   data              i_p_pg data structure
* @param[out]   time              time
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Ingress Port + Priority Groups 
*                                     stat get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Ingress Port + Priority Groups 
*                                     stat get functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_ippg_data_get (int asic,
                                      BVIEW_BST_INGRESS_PORT_PG_DATA_t * data,
                                      BVIEW_TIME_t * time)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }  
  else if (bstFeaturePtr->bst_ippg_data_get_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {
    rv = bstFeaturePtr->bst_ippg_data_get_cb (asic, data, time);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Obtain Ingress Port + Service Pools Statistics
*
* @param[in]    asic             unit
* @param[out]   data             i_p_sp data structure
* @param[out]   time             time
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Ingress Port + Service Pools 
*                                     stat get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Ingress Port + Service Pools 
*                                     stat get functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_ipsp_data_get (int asic,
                                      BVIEW_BST_INGRESS_PORT_SP_DATA_t * data,
                                      BVIEW_TIME_t * time)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  } 
  else if (bstFeaturePtr->bst_ipsp_data_get_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_ipsp_data_get_cb (asic, data, time);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Obtain Ingress Service Pools Statistics
*
* @param[in]    asic              unit
* @param[out]   data              i_sp structure
* @param[out]   time              time
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Ingress Service Pools 
*                                     stat get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Ingress Service Pools 
*                                     stat get functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_isp_data_get (int asic,
                                     BVIEW_BST_INGRESS_SP_DATA_t * data,
                                     BVIEW_TIME_t * time)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  } 
  else if (bstFeaturePtr->bst_isp_data_get_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_isp_data_get_cb (asic, data, time);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Obtain Egress Port + Service Pools Statistics
*
* @param[in]    asic              unit
* @param[out]   data              e_p_sp data structure
* @param[out]   time              time
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Egress Port + Service Pools
*                                     stat get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Egress Port + Service Pools
*                                     stat get functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_epsp_data_get (int asic,
                                      BVIEW_BST_EGRESS_PORT_SP_DATA_t * data,
                                      BVIEW_TIME_t * time)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }  
  else if (bstFeaturePtr->bst_epsp_data_get_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_epsp_data_get_cb (asic, data, time);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Obtain Egress Service Pools Statistics
*
* @param[in]    asic              unit
* @param[out]   data              e_sp data structure
* @param[out]   time              time
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Egress Service Pools
*                                     stat get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Egress Service Pools
*                                     stat get functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_esp_data_get (int asic,
                                     BVIEW_BST_EGRESS_SP_DATA_t * data,
                                     BVIEW_TIME_t * time)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  } 
  else if (bstFeaturePtr->bst_esp_data_get_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_esp_data_get_cb (asic, data, time);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Obtain Egress Unicast Queues Statistics
*
* @param[in]    asic             unit
* @param[out]   data             Egress Unicast Queue data structure
* @param[out]   time             time
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Egress Unicast Queues
*                                     stat get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Egress Unicast Queues
*                                     stat get functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_eucq_data_get (int asic,
                                      BVIEW_BST_EGRESS_UC_QUEUE_DATA_t * data,
                                      BVIEW_TIME_t * time)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);

  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }
  else if (bstFeaturePtr->bst_eucq_data_get_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_eucq_data_get_cb (asic, data, time);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Obtain Egress Unicast Queue Groups Statistics
*
* @param[in]    asic              unit
* @param[out]   data              Egress Unicast Queue Groups data structure
* @param[out]   time              time
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Egress Unicast Queue Groups
*                                     stat get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Egress Unicast Queue Groups
*                                     stat get functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_eucqg_data_get (int asic,
                                       BVIEW_BST_EGRESS_UC_QUEUEGROUPS_DATA_t *
                                       data, BVIEW_TIME_t * time)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                
  else if (bstFeaturePtr->bst_eucqg_data_get_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_eucqg_data_get_cb (asic, data, time);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Obtain Egress Multicast Queues Statistics
*
* @param[in]    asic             unit
* @param[out]   data             Egress Multicast Queue data structure
* @param[out]   time             time
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Egress Multicast Queues
*                                     stat get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Egress Multicast Queues
*                                     stat get functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_emcq_data_get (int asic,
                                      BVIEW_BST_EGRESS_MC_QUEUE_DATA_t * data,
                                      BVIEW_TIME_t * time)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                
  else if (bstFeaturePtr->bst_emcq_data_get_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_emcq_data_get_cb (asic, data, time);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Obtain Egress CPU Queues Statistics
*
* @param[in]    asic              unit
* @param[out]   data              Egress CPU Queue data structure
* @param[out]   time              time
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Egress CPU Queues
*                                     stat get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Egress CPU Queues
*                                     stat get functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_cpuq_data_get (int asic,
                                      BVIEW_BST_EGRESS_CPU_QUEUE_DATA_t * data,
                                      BVIEW_TIME_t * time)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                
  else if (bstFeaturePtr->bst_cpuq_data_get_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_cpuq_data_get_cb (asic, data, time);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Obtain Egress RQE Queues Statistics
*
* @param[in]    asic              unit
* @param[out]   data              Egress RQE Queue data structure
* @param[out]   time              time
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Egress RQE Queues 
*                                     stat get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Egress RQE Queues 
*                                     stat get functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_rqeq_data_get (int asic,
                                      BVIEW_BST_EGRESS_RQE_QUEUE_DATA_t * data,
                                      BVIEW_TIME_t * time)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                
  else if (bstFeaturePtr->bst_rqeq_data_get_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_rqeq_data_get_cb (asic, data, time);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Clear BST stats of a particular unit
*
* @param[in]    asic            unit
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Successfully cleared BST stats 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Clear BST stats functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_clear_stats (int asic)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                
  else if (bstFeaturePtr->bst_clear_stats_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_clear_stats_cb (asic);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Set profile configuration for Device Statistics
*
* @param[in]   asic                unit
* @param[in]   thres               threshold
* 
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Device Statistics 
*                                     threshold set is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Device Statistics 
*                                     threshold set functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_device_threshold_set (int asic,
                                             BVIEW_BST_DEVICE_THRESHOLD_t *threshold)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                
  else if (bstFeaturePtr->bst_device_threshold_set_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_device_threshold_set_cb (asic, threshold);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Set profile configuration for
*           Ingress Port + Priority Groups Statistics
*
* @param[in]   asic               unit
* @param[in]   port               port
* @param[in]   pg                 Priority Group
* @param[out]  thres              Threshold structure
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Ingress Port + Priority Groups 
*                                     threshold set is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Ingress Port + Priority Groups
*                                     threshold set functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_ippg_threshold_set (int asic, int port, int pg,
                                           BVIEW_BST_INGRESS_PORT_PG_THRESHOLD_t  *threshold)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                
  else if (bstFeaturePtr->bst_ippg_threshold_set_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_ippg_threshold_set_cb (asic, port, pg, threshold);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Set profile configuration for Ingress Port + Service Pools
*           Statistics
*
* @param[in]  asic                                    unit
* @param[in]  port                                    port
* @param[in]  sp                                      service pool
* @param[in]  thres                                   threshold
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Ingress Port + Service Pools 
*                                     threshold set is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Ingress Port + Service Pools
*                                     threshold set functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_ipsp_threshold_set (int asic, int port, int sp,
                                           BVIEW_BST_INGRESS_PORT_SP_THRESHOLD_t * threshold)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                
  else if (bstFeaturePtr->bst_ipsp_threshold_set_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_ipsp_threshold_set_cb (asic, port, sp, threshold);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Set profile configuration for Ingress Service Pools
*           Statistics
*
* @param[in]  asic                               unit
* @param[in]  sp                                 service pool
* @param[in]  thres                              threshold
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Ingress Service Pools 
*                                     threshold set is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Ingress Service Pools
*                                     threshold set functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_isp_threshold_set (int asic, int sp,
                                          BVIEW_BST_INGRESS_SP_THRESHOLD_t *threshold)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                
  else if (bstFeaturePtr->bst_isp_threshold_set_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_isp_threshold_set_cb (asic, sp, threshold);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Set Profile configuration for Egress Port + Service Pools
*           Statistics
*
* @param[in]  asic                                  unit
* @param[in]  port                                  port
* @param[in]  sp                                    service pool
* @param[in]  thres                                 threshold
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Egress Port + Service Pools
*                                     threshold set is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Egress Port + Service Pools
*                                     threshold set functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_epsp_threshold_set (int asic, int port, int sp,
                                           BVIEW_BST_EGRESS_PORT_SP_THRESHOLD_t *threshold)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                
  else if (bstFeaturePtr->bst_epsp_threshold_set_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_epsp_threshold_set_cb (asic, port, sp, threshold);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Set profile configuration for Egress Service Pools Statistics
*
* @param[in]  asic                                unit
* @param[in]  sp                                  service pool
* @param[in]  thres                               threshold
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Egress Service Pools
*                                     threshold set is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Egress Service Pools
*                                     threshold set functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_esp_threshold_set (int asic, int sp,
                                          BVIEW_BST_EGRESS_SP_THRESHOLD_t *threshold)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                
  else if (bstFeaturePtr->bst_esp_threshold_set_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_esp_threshold_set_cb (asic, sp, threshold);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Set profile configuration for Egress Unicast Queues
*           Statistics
*
* @param[in]  asic                            unit
* @param[in]  ucQueue                         uc queue
* @param[in]  thres                           threshold
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Egress Unicast Queues
*                                     threshold set is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Egress Unicast Queues
*                                     threshold set functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_eucq_threshold_set (int asic, int ucQueue,
                                           BVIEW_BST_EGRESS_UC_QUEUE_THRESHOLD_t *threshold)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                
  else if (bstFeaturePtr->bst_eucq_threshold_set_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_eucq_threshold_set_cb (asic, ucQueue, threshold);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Set profile configuration for Egress Unicast Queue Groups
*           Statistics
*
* @param[in] asic                                      unit
* @param[in] ucQueueGrp                                uc queue group
* @param[in] thres                                     threshold
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Egress Unicast Queue Groups
*                                     threshold set is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Egress Unicast Queue Groups
*                                     threshold set functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_eucqg_threshold_set (int asic, int ucQueueGrp,
                                            BVIEW_BST_EGRESS_UC_QUEUEGROUPS_THRESHOLD_t
                                            * threshold)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                
  else if (bstFeaturePtr->bst_eucqg_threshold_set_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv =
      bstFeaturePtr->bst_eucqg_threshold_set_cb (asic, ucQueueGrp, threshold);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Set profile configuration for Egress Multicast Queues
*           Statistics
*
* @param[in]  asic                                   - unit
* @param[in]  mcQueue                                - mcQueue
* @param[in]  thres                                  - threshold
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Egress Multicast Queues
*                                     threshold set is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Egress Multicast Queues
*                                     threshold set functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_emcq_threshold_set (int asic, int mcQueue,
                                           BVIEW_BST_EGRESS_MC_QUEUE_THRESHOLD_t
                                           * threshold)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                
  else if (bstFeaturePtr->bst_emcq_threshold_set_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_emcq_threshold_set_cb (asic, mcQueue, threshold);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Set profile configuration for Egress CPU Queues
*           Statistics
*
* @param[in]  asic                                    unit
* @param[in]  cpuQueue                                cpuQueue
* @param[in]  thres                                   threshold
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Egress CPU Queues
*                                     threshold set is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Egress CPU Queues
*                                     threshold set functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_cpuq_threshold_set (int asic, int cpuQueue,
                                           BVIEW_BST_EGRESS_CPU_QUEUE_THRESHOLD_t
                                           * threshold)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                
  else if (bstFeaturePtr->bst_cpuq_threshold_set_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_cpuq_threshold_set_cb (asic, cpuQueue, threshold);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Set profile configuration for Egress RQE Queues
*           Statistics
*
* @param[in]     asic                    unit
* @param[in]     rqeQueue                rqeQueue
* @param[out]    thres                   Threshold data structure
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Egress RQE Queues
*                                     threshold set is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Egress RQE Queues
*                                     threshold set functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_rqeq_threshold_set (int asic, int rqeQueue,
                                           BVIEW_BST_EGRESS_RQE_QUEUE_THRESHOLD_t
                                           * threshold)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;
  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                
  else if (bstFeaturePtr->bst_rqeq_threshold_set_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_rqeq_threshold_set_cb (asic, rqeQueue, threshold);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Get snapshot of all thresholds configured
*
*
* @param[in]   asic                          unit
* @param[out]  thresholdSnapshot             threshold snapshot
* @param[out]  time                          time
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Snapshot get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Snapshot get functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_threshold_get (int asic,
                                      BVIEW_BST_ASIC_SNAPSHOT_DATA_t *thresholdSnapshot,
                                      BVIEW_TIME_t * time)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                
  else if (bstFeaturePtr->bst_threshold_get_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_threshold_get_cb (asic, thresholdSnapshot, time);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Restore threshold configuration to defaults
*
*
* @param[in] asic                        unit
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Resetting threshold to default is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Clear threshold functionality is 
*                                     not supported on this unit
*
*
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_clear_thresholds (int asic)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;
  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                
  else if (bstFeaturePtr->bst_clear_thresholds_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_clear_thresholds_cb (asic);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief  Register hw trigger callback
*
*
* @param[in] asic                  unit
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      Callback is successfully registered 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  Trigger callback registration is not supported 
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_register_trigger (int asic,
                                         BVIEW_BST_TRIGGER_CALLBACK_t callback,
                                         void *cookie)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }
  else if (bstFeaturePtr->bst_register_trigger_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = bstFeaturePtr->bst_register_trigger_cb (asic, callback, cookie);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}


/*********************************************************************
* @brief       Get BST default buffer values 
*
* @param[in]     asic                  Unit number
* @param[out]    snapshot              BST snapshot
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BST feature is not present or
*                                     BST south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      BST snapshot get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  BST snapshot get functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_bst_default_snapshot_get (int asic,
                                     BVIEW_BST_ASIC_SNAPSHOT_DATA_t * snapshot)
{
  BVIEW_SB_BST_FEATURE_t *bstFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType;

  /* Get asic type of the unit */
  if (sbapi_system_unit_to_asic_type_get (asic, &asicType) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic type for unit %d \n",
                             __FILE__, __LINE__, asic);
    return BVIEW_STATUS_FAILURE;
  }
  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get best matching south bound feature functions based on Asic type */
  bstFeaturePtr =
    (BVIEW_SB_BST_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BST);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bstFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }  
  else if (bstFeaturePtr->bst_default_snapshot_get_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  { 
    rv = bstFeaturePtr->bst_default_snapshot_get_cb (asic, snapshot);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}


