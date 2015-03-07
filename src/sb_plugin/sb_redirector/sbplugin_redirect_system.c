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
#include "sbplugin_system.h"

/*********************************************************************
* @brief       Get number of units in the system
*
* @param[out]  unitNum            Numbers of units
*
* @retval   BVIEW_STATUS_FAILURE   When lock acquistion fails or
*                                  System feature is not avaialable or
*                                  The number of units is '0'
*
* @retval   BVIEW_STATUS_SUCCESS   Read number of units from system feature
*                                  handler sucessfully
*
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_num_units_get (int *unitNum)
{
  BVIEW_SB_SYSTEM_FEATURE_t *systemFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType = BVIEW_ASIC_TYPE_ALL;

  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);

  /* Get system south bound plug-in based on Asic type */
  systemFeaturePtr =
    (BVIEW_SB_SYSTEM_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                                  BVIEW_FEATURE_SYSTEM);
  /* Validate System feature pointer and the data in the system feature */ 
  if (systemFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }
  else if ((systemFeaturePtr->numSupportedAsics) == 0)
  {
    rv = BVIEW_STATUS_FAILURE;
  }
  else
  { 
    /* Get number of units in the system */
    *unitNum = systemFeaturePtr->numSupportedAsics;
    rv = BVIEW_STATUS_SUCCESS;
  }
  /* Release Read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief       Get asic type of a  unit
*
* @param[int]  asic        Unit number
* @param[out]  asicType    Asic type of unit
*
* @retval   BVIEW_STATUS_FAILURE   When lock acquistion fails or
*                                  System feature is not avaialable or
*                                  The number of units is '0' or 
*                                  unit entry is not found
*
* @retval   BVIEW_STATUS_SUCCESS   Able to convert unit number to asic type
*                                  sucessfully
*
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_unit_to_asic_type_get (int asic,
                                                 BVIEW_ASIC_TYPE * asicType)
{
  BVIEW_SB_SYSTEM_FEATURE_t *systemFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_t *asicInfoPtr = NULL;
  int asicListIndex = 0;

  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get system south bound plug-in based on Asic type */
  systemFeaturePtr =
    (BVIEW_SB_SYSTEM_FEATURE_t *)
    sb_redirect_feature_handle_get (BVIEW_ASIC_TYPE_ALL, BVIEW_FEATURE_SYSTEM);

  /* Validate System feature pointer and the data in the system feature */ 
  if (systemFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }
  else if ((systemFeaturePtr->numSupportedAsics) == 0)
  {
    rv = BVIEW_STATUS_FAILURE;
  }
  else
  {
    /* Loop through asic list in the system plug-in */
    for (asicListIndex = 0; asicListIndex < systemFeaturePtr->numSupportedAsics;
         asicListIndex++)
    {
      asicInfoPtr = systemFeaturePtr->asicList[asicListIndex];
      if ((asicInfoPtr != NULL) && (asicInfoPtr->unit == asic))
      {
        /* Get asic type */
        *asicType = asicInfoPtr->asicType;
        break;
      }
    }
    /* Failed to find the correct entry */
    if (asicListIndex == systemFeaturePtr->numSupportedAsics)
    {
      rv = BVIEW_STATUS_FAILURE;
    }
    else
    {
      rv = BVIEW_STATUS_SUCCESS;
    }
  }
  /* Release Read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief       Get system name
*
* @param[out]  buffer        System name
* @param[in]   length        Buffer length
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     System feature is not present or
*                                     System south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      System south bound function for name get is
*                                     successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  System name get functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_name_get (char *buffer, int length)
{
  BVIEW_SB_SYSTEM_FEATURE_t *systemFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType = BVIEW_ASIC_TYPE_ALL;

  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get system south bound plug-in */
  systemFeaturePtr =
    (BVIEW_SB_SYSTEM_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                                  BVIEW_FEATURE_SYSTEM);
  /* Validate system feature pointer and south bound handler.
   * Call south bound handler                               */
  if (systemFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                              
  else if ((systemFeaturePtr->system_name_get_cb) == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = systemFeaturePtr->system_name_get_cb (buffer, length);
  }
  /* Release read-lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief       Get system MAC address
*
* @param[out]  buffer        System MAC address
* @param[in]   length        Buffer length
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     System feature is not present or
*                                     System south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      System south bound function for mac get is
*                                     successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  System mac get functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_mac_get (uint8_t * buffer, int length)
{
  BVIEW_SB_SYSTEM_FEATURE_t *systemFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType = BVIEW_ASIC_TYPE_ALL;

  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get system south bound plug-in */
  systemFeaturePtr =
    (BVIEW_SB_SYSTEM_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                                  BVIEW_FEATURE_SYSTEM);
  /* Validate system feature pointer and south bound handler.
   * Call south bound handler                               */
  if (systemFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                              
  else if ((systemFeaturePtr->system_mac_get_cb) == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = systemFeaturePtr->system_mac_get_cb (buffer, length);
  }
  /* Release Read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief       Get system IPv4 address
*
* @param[out]  buffer        IPv4 address
* @param[in]   length        Buffer length
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     System feature is not present or
*                                     System south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      System south bound function for ipv4 get is
*                                     successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  System  ipv4 get functionality is 
*                                     not supported on this unit
*
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_ip4_get (uint8_t * buffer, int length)
{
  BVIEW_SB_SYSTEM_FEATURE_t *systemFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType = BVIEW_ASIC_TYPE_ALL;

  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get system south bound plug-in */
  systemFeaturePtr =
    (BVIEW_SB_SYSTEM_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                                  BVIEW_FEATURE_SYSTEM);
  /* Validate system feature pointer and south bound handler.
   * Call south bound handler                               */
  if (systemFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                              
  else if ((systemFeaturePtr->system_ip4_get_cb) == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = systemFeaturePtr->system_ip4_get_cb (buffer, length);
  }
  /* Release Read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief       Get system asic number from notational asic number
*                                  representation - string form
*
* @param[in]   src           Asic number in notational(string) form
* @param[out]  asic        System asic number 
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     System feature is not present or
*                                     System south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      System south bound function for name get is
*                                     successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  System name get functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_asic_translate_from_notation(char *src, int *asic)
{
  BVIEW_SB_SYSTEM_FEATURE_t *systemFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType = BVIEW_ASIC_TYPE_ALL;

  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get system south bound plug-in */
  systemFeaturePtr =
    (BVIEW_SB_SYSTEM_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                                  BVIEW_FEATURE_SYSTEM);
  /* Validate system feature pointer and south bound handler.
   * Call south bound handler                               */
  if (systemFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                              
  else if ((systemFeaturePtr->system_asic_translate_from_notation_cb) == NULL)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                           "(%s:%d) System feature does not support"
                           "system_asic_translate_from_notation call back\n",
                           __FILE__, __LINE__);
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = systemFeaturePtr->system_asic_translate_from_notation_cb(src, asic);
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_DEBUG,
                    "(%s:%d) After calling "
                    "system_asic_translate_from_notation call back"
                    " src=%s, asic = %d, rv = %d\n",
                     __FILE__, __LINE__,src, *asic, rv);
  }
  /* Release read-lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief       Get asic number in notational representation(string) 
*                from system asic number
*
* @param[in]  asic        System asic number 
* @param[out] dst         Asic number in notational(string) form
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     System feature is not present or
*                                     System south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      System south bound function for name get is
*                                     successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  System name get functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_asic_translate_to_notation(int asic, char *dst)
{
  BVIEW_SB_SYSTEM_FEATURE_t *systemFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType = BVIEW_ASIC_TYPE_ALL;

  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get system south bound plug-in */
  systemFeaturePtr =
    (BVIEW_SB_SYSTEM_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                                  BVIEW_FEATURE_SYSTEM);
  /* Validate system feature pointer and south bound handler.
   * Call south bound handler                               */
  if (systemFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                              
  else if ((systemFeaturePtr->system_asic_translate_to_notation_cb) == NULL)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                           "(%s:%d) System feature does not support"
                           "system_asic_translate_to_notation call back\n",
                           __FILE__, __LINE__);
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    dst[0] = 0;
    rv = systemFeaturePtr->system_asic_translate_to_notation_cb(asic, dst);
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_DEBUG,
                    "(%s:%d) After calling "
                    "system_asic_translate_to_notation call back"
                    " asic = %d, dst = %s, rv = %d\n",
                     __FILE__, __LINE__,asic, dst, rv);
  }
  /* Release read-lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief       Get system port number from notational port number
*                                  representation - string form
*
* @param[in]   src         Port number in notational(string) form
* @param[out]  port        System port number 
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     System feature is not present or
*                                     System south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      System south bound function for name get is
*                                     successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  System name get functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_port_translate_from_notation(char *src, int *port)
{
  BVIEW_SB_SYSTEM_FEATURE_t *systemFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType = BVIEW_ASIC_TYPE_ALL;

  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get system south bound plug-in */
  systemFeaturePtr =
    (BVIEW_SB_SYSTEM_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                                  BVIEW_FEATURE_SYSTEM);
  /* Validate system feature pointer and south bound handler.
   * Call south bound handler                               */
  if (systemFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                              
  else if ((systemFeaturePtr->system_port_translate_from_notation_cb) == NULL)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                           "(%s:%d) System feature does not support"
                           "system_port_translate_from_notation call back\n",
                           __FILE__, __LINE__);
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    rv = systemFeaturePtr->system_port_translate_from_notation_cb(src, port);
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_DEBUG,
                    "(%s:%d) After calling "
                    "system_port_translate_from_notation call back"
                    " src=%s, port = %d, rv = %d\n",
                     __FILE__, __LINE__,src, *port, rv);
  }
  /* Release read-lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief       Get port number in notational representation(string) 
*                from system port number and asic number
*
* @param[in]  asic        System asic number 
* @param[in]  port        System port number 
* @param[out] dst         Port number in notational(string) form
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     System feature is not present or
*                                     System south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      System south bound function for name get is
*                                     successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  System name get functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_port_translate_to_notation(int asic, int port, char *dst)
{
  BVIEW_SB_SYSTEM_FEATURE_t *systemFeaturePtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_ASIC_TYPE asicType = BVIEW_ASIC_TYPE_ALL;

  /* Acquire Read lock */
  SB_REDIRECT_RWLOCK_RD_LOCK (sbRedirectRWLock);
  /* Get system south bound plug-in */
  systemFeaturePtr =
    (BVIEW_SB_SYSTEM_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                                  BVIEW_FEATURE_SYSTEM);
  /* Validate system feature pointer and south bound handler.
   * Call south bound handler                               */
  if (systemFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }                              
  else if ((systemFeaturePtr->system_port_translate_to_notation_cb) == NULL)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                           "(%s:%d) System feature does not support"
                           "system_port_translate_to_notation call back\n",
                           __FILE__, __LINE__);
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {                              
    dst[0] = 0;
    rv = systemFeaturePtr->system_port_translate_to_notation_cb(asic, port, dst);
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_DEBUG,
                    "(%s:%d) After calling "
                    "system_port_translate_to_notation call back"
                    " asic = %d, port = %d, dst = %s, rv = %d\n",
                     __FILE__, __LINE__,asic, port, dst, rv);
  }
  /* Release read-lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}
/*********************************************************************
* @brief       Get asic capabilites of a unit
*
* @param[in]   asic          Unit number
* @param[out]  asicCapPtr    Pointer to asic capabilities  
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Found asic capabilities
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_asic_capabilities_get (int asic, 
                                          BVIEW_ASIC_CAPABILITIES_t *asicCapPtr)
{
  BVIEW_ASIC_TYPE asicType;
  BVIEW_ASIC_CAPABILITIES_t *asicCapPtrLocal = NULL;
  BVIEW_STATUS rv;

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
  /* Get asic capabilities based on asic type */
  if (sb_redirect_asic_capabilities_get (asicType, &asicCapPtrLocal) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic capabilities for unit %d \n",
                             __FILE__, __LINE__, asic);
    rv = BVIEW_STATUS_FAILURE;
  }
  else
  { 
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_INFO,
                             "(%s:%d) Found Asic capabilities for unit %d \n",
                             __FILE__, __LINE__, asic);
    *asicCapPtr = *asicCapPtrLocal;
    rv = BVIEW_STATUS_SUCCESS;
  }
  /* Release Read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief       Get number of ports in a unit
*
* @param[in]   asic          Unit number
* @param[out]  numPorts      Number of ports
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Get number of ports is successful
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_asic_num_ports_get (int asic, int *numPorts)
{
  BVIEW_ASIC_TYPE asicType;
  BVIEW_ASIC_CAPABILITIES_t *asicCapPtr = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

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
  /* Get asic capabilities based on asic type */
  if (sb_redirect_asic_capabilities_get (asicType, &asicCapPtr) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic capabilities for unit %d \n",
                             __FILE__, __LINE__, asic);
    rv = BVIEW_STATUS_FAILURE;
  }
  else
  { 
	/* Get number of ports on the silicon */
    *numPorts = asicCapPtr->numPorts;
    rv = BVIEW_STATUS_SUCCESS;
  }
  /* Release Read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief       Get number of unicast queues in a unit
*
* @param[in]   asic                  Unit number
* @param[out]  numUnicastQueues      Number of Unicast queues
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Get number of unicast queues is successful
*
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_asic_num_ucq_get (int asic, int *numUnicastQueues)
{
  BVIEW_ASIC_TYPE asicType;
  BVIEW_ASIC_CAPABILITIES_t *asicCapPtr = NULL;
  BVIEW_STATUS rv;

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
  /* Get asic capabilities based on asic type */
  if (sb_redirect_asic_capabilities_get (asicType, &asicCapPtr) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic capabilities for unit %d \n",
                             __FILE__, __LINE__, asic);
    rv = BVIEW_STATUS_FAILURE;
  }
  else
  { 
    /* Get number of unicast queues */
    *numUnicastQueues = asicCapPtr->numUnicastQueues;
    rv = BVIEW_STATUS_SUCCESS;
  }
  /* Release Read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief       Get number of unicast queue groups in a unit
*
* @param[in]   asic                    Unit number
* @param[out]  numUnicastQueueGroups   Number of Unicast queue groups
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Get number of unicast queue groups 
*                                     is successful
*
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_asic_num_ucqg_get (int asic,
                                             int *numUnicastQueueGroups)
{
  BVIEW_ASIC_TYPE asicType;
  BVIEW_ASIC_CAPABILITIES_t *asicCapPtr = NULL;
  BVIEW_STATUS rv;

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
  /* Get asic capabilities based on asic type */
  if (sb_redirect_asic_capabilities_get (asicType, &asicCapPtr) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic capabilities for unit %d \n",
                             __FILE__, __LINE__, asic);
    rv = BVIEW_STATUS_FAILURE;
  }
  else
  {
    /* Get number of Unicast queue groups */
    *numUnicastQueueGroups = asicCapPtr->numUnicastQueueGroups;
    rv = BVIEW_STATUS_SUCCESS;
  }
  /* Release Read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief       Get number of multicast queues in a unit
*
* @param[in]   asic                    Unit number
* @param[out]  numMulticastQueueGroups   Number of Multicast queues
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Get number of multicast queues 
*                                     is successful
*
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_asic_num_mcq_get (int asic, int *numMulticastQueues)
{
  BVIEW_ASIC_TYPE asicType;
  BVIEW_ASIC_CAPABILITIES_t *asicCapPtr = NULL;
  BVIEW_STATUS rv;

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
  /* Get asic capabilities based on asic type */
  if (sb_redirect_asic_capabilities_get (asicType, &asicCapPtr) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic capabilities for unit %d \n",
                             __FILE__, __LINE__, asic);
    rv = BVIEW_STATUS_FAILURE;
  }
  else
  {
    /* Get number of multi cast queues */
    *numMulticastQueues = asicCapPtr->numMulticastQueues;
    rv = BVIEW_STATUS_SUCCESS;
  }
  /* Release Read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief       Get number of service pools in a unit
*
* @param[in]   asic                    Unit number
* @param[out]  numServicePools         Number of Service pools
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Get number of service pools 
*                                     is successful
*
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_asic_num_sp_get (int asic, int *numServicePools)
{
  BVIEW_ASIC_TYPE asicType;
  BVIEW_ASIC_CAPABILITIES_t *asicCapPtr = NULL;
  BVIEW_STATUS rv;

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
  /* Get asic capabilities based on asic type */
  if (sb_redirect_asic_capabilities_get (asicType, &asicCapPtr) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic capabilities for unit %d \n",
                             __FILE__, __LINE__, asic);
    rv = BVIEW_STATUS_FAILURE;
  }
  else
  {
	/* Get number of service pools */
    *numServicePools = asicCapPtr->numServicePools;
    rv = BVIEW_STATUS_SUCCESS;
  }
  /* Release Read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief       Get number of common pools in a unit
*
* @param[in]   asic                  Unit number
* @param[out]  numCommonPools        Number of common pools
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Get number of common pools 
*                                     is successful
*
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_asic_num_cp_get (int asic, int *numCommonPools)
{
  BVIEW_ASIC_TYPE asicType;
  BVIEW_ASIC_CAPABILITIES_t *asicCapPtr = NULL;
  BVIEW_STATUS rv;

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
  /* Get asic capabilities based on asic type */
  if (sb_redirect_asic_capabilities_get (asicType, &asicCapPtr) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic capabilities for unit %d \n",
                             __FILE__, __LINE__, asic);
    rv = BVIEW_STATUS_FAILURE;
  }
  else
  { 
	/* Get Number of common pools */
    *numCommonPools = asicCapPtr->numCommonPools;
    rv = BVIEW_STATUS_SUCCESS;
  }
  /* Release Read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief       Get number of cpu queues in a unit
*
* @param[in]   asic                Unit number
* @param[out]  numCpuQueues        Number of cpu queues
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Get number of cpu queues 
*                                     is successful
*
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_asic_num_cpuq_get (int asic, int *numCpuQueues)
{
  BVIEW_ASIC_TYPE asicType;
  BVIEW_ASIC_CAPABILITIES_t *asicCapPtr = NULL;
  BVIEW_STATUS rv;

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
  /* Get asic capabilities based on asic type */
  if (sb_redirect_asic_capabilities_get (asicType, &asicCapPtr) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic capabilities for unit %d \n",
                             __FILE__, __LINE__, asic);
    rv = BVIEW_STATUS_FAILURE;
  }
  else
  { 
    /* Get number of CPU queues */
    *numCpuQueues = asicCapPtr->numCpuQueues;
    rv = BVIEW_STATUS_SUCCESS;
  }
  /* Release Read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief       Get number of RQE queues in a unit
*
* @param[in]   asic                Unit number
* @param[out]  numRqeQueues        Number of RQE queues
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Get number of RQE queues 
*                                     is successful
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_asic_num_rqeq_get (int asic, int *numRqeQueues)
{
  BVIEW_ASIC_TYPE asicType;
  BVIEW_ASIC_CAPABILITIES_t *asicCapPtr = NULL;
  BVIEW_STATUS rv;

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
  /* Get asic capabilities based on asic type */
  if (sb_redirect_asic_capabilities_get (asicType, &asicCapPtr) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic capabilities for unit %d \n",
                             __FILE__, __LINE__, asic);
    rv = BVIEW_STATUS_FAILURE;
  }
  else
  { 
    /* Get number of RQE queues  */
    *numRqeQueues = asicCapPtr->numRqeQueues;
    rv = BVIEW_STATUS_SUCCESS;
  }
  /* Release Read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief       Get number of RQE queue pools in a unit
*
* @param[in]   asic                Unit number
* @param[out]  numRqeQueuePools    Number of RQE queue pools
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Get number of RQE queue pools
*                                     is successful
*
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_asic_num_rqeqp_get (int asic, int *numRqeQueuePools)
{
  BVIEW_ASIC_TYPE asicType;
  BVIEW_ASIC_CAPABILITIES_t *asicCapPtr = NULL;
  BVIEW_STATUS rv;

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
  /* Get asic capabilities based on asic type */
  if (sb_redirect_asic_capabilities_get (asicType, &asicCapPtr) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic capabilities for unit %d \n",
                             __FILE__, __LINE__, asic);
    rv = BVIEW_STATUS_FAILURE;
  }
  else
  { 
    /* Get number of RQE queue pools */
    *numRqeQueuePools = asicCapPtr->numRqeQueuePools;
    rv = BVIEW_STATUS_SUCCESS;
  }
  /* Release Read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief       Get number of priority groups in a unit
*
* @param[in]   asic                 Unit number
* @param[out]  numPriorityGroups    Number of priority groups
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Get number of priority groups
*                                     is successful
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_asic_num_pg_get (int asic, int *numPriorityGroups)
{
  BVIEW_ASIC_TYPE asicType;
  BVIEW_ASIC_CAPABILITIES_t *asicCapPtr = NULL;
  BVIEW_STATUS rv;

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
  /* Get asic capabilities based on asic type */
  if (sb_redirect_asic_capabilities_get (asicType, &asicCapPtr) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic capabilities for unit %d \n",
                             __FILE__, __LINE__, asic);
    rv = BVIEW_STATUS_FAILURE;
  }
  else
  { 
    /* get number of priority groups */
    *numPriorityGroups = asicCapPtr->numPriorityGroups;
    rv = BVIEW_STATUS_SUCCESS;
  }
  /* Release Read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}

/*********************************************************************
* @brief       Get whether a unit supports 1588 or not
*
* @param[in]   asic                 Unit number
* @param[out]  support1588          Support for 1588
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Failed to get asic type from unit or
*                                     Failed to get asic capabilites for this unit
*
* @retval   BVIEW_STATUS_SUCCESS      Unit's capability for 1588 is successfully 
*                                     queried
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_system_asic_support_1588_get (int asic, bool * support1588)
{
  BVIEW_ASIC_TYPE asicType;
  BVIEW_ASIC_CAPABILITIES_t *asicCapPtr = NULL;
  BVIEW_STATUS rv;

  /*  Get asic type of the unit */
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
  /* Get asic capabilities based on asic type */
  if (sb_redirect_asic_capabilities_get (asicType, &asicCapPtr) !=
      BVIEW_STATUS_SUCCESS)
  {
    SB_REDIRECT_DEBUG_PRINT (BVIEW_LOG_ERROR,
                             "(%s:%d) Failed to get asic capabilities for unit %d \n",
                             __FILE__, __LINE__, asic);
    rv = BVIEW_STATUS_FAILURE;
  }
  else
  {
	/* Get support for 1588 */
    *support1588 = asicCapPtr->support1588;
    rv = BVIEW_STATUS_SUCCESS;
  }
  /* Release Read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}
