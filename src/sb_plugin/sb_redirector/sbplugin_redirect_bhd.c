/*********************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename sbplugin_redirect_bhd.c 
  *
  * @purpose BroadView BHD Application south bound redirector APIs 
  *
  * @component Black hole detection 
  *
  * @comments
  *
  * @create 3/7/2016
  *
  * @author 
  * @end
  *
  **********************************************************************/

#include "sbplugin_redirect.h"
#include "sbfeature_bhd.h"
#include "sbplugin_redirect_system.h"

/*********************************************************************
* @brief       Get sflow sampling status 
*
* @param[in]    asic                  Unit number
* @param[out]   status                port sampling status structure
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BHD feature is not present or
*                                     BHD south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      BHD status get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  BHD status get functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_bhd_sflow_sampling_status_get (int asic, int port,
                                BVIEW_BHD_PORT_SFLOW_SAMPLING_STATUS_t *status)
{
  BVIEW_SB_BHD_FEATURE_t *bhdFeaturePtr = NULL;
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
  bhdFeaturePtr =
    (BVIEW_SB_BHD_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BHD);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bhdFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }       
  else if (bhdFeaturePtr->bhd_sflow_sampling_status_get_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {  
    rv = bhdFeaturePtr->bhd_sflow_sampling_status_get_cb (asic, port, status);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}


/*********************************************************************
* @brief       cancel black hole 
*
* @param[in]    asic                  Unit number
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BHD feature is not present or
*                                     BHD south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      BHD cancel is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  BHD cancel functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_bhd_cancel_black_hole (int asic)
{
  BVIEW_SB_BHD_FEATURE_t *bhdFeaturePtr = NULL;
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
  bhdFeaturePtr =
    (BVIEW_SB_BHD_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BHD);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bhdFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }       
  else if (bhdFeaturePtr->bhd_cancel_black_hole_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {  
    rv = bhdFeaturePtr->bhd_cancel_black_hole_cb (asic);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}


/*********************************************************************
* @brief       configure black hole 
*
* @param[in]    asic                  Unit number
* @param[in]    config                structure for config
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BHD feature is not present or
*                                     BHD south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      black hole configuration is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  black hole configuration functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_bhd_configure_black_hole (int asic, BVIEW_BLACK_HOLE_CONFIG_t *config)
{
  BVIEW_SB_BHD_FEATURE_t *bhdFeaturePtr = NULL;
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
  bhdFeaturePtr =
    (BVIEW_SB_BHD_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BHD);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bhdFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }       
  else if (bhdFeaturePtr->bhd_configure_black_hole_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {  
    rv = bhdFeaturePtr->bhd_configure_black_hole_cb(asic, config);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}


/*********************************************************************
* @brief       Get the asic sampling capabilities 
*
* @param[in]    asic                  Unit number
* @param[in]    samplingCapabilities  structure for asic sampling capabilities 
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BHD feature is not present or
*                                     BHD south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      get asic sampling capabilities is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  get asic sampling capabilities functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_bhd_asic_sampling_capabilties_get (int asic, int *samplingCapabilities)
{
  BVIEW_SB_BHD_FEATURE_t *bhdFeaturePtr = NULL;
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
  bhdFeaturePtr =
    (BVIEW_SB_BHD_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BHD);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bhdFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }       
  else if (bhdFeaturePtr->bhd_asic_sampling_capabilties_get_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {  
    rv = bhdFeaturePtr->bhd_asic_sampling_capabilties_get_cb(asic, samplingCapabilities);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}


/*********************************************************************
* @brief       Get the black holed packet count 
*
* @param[in]    asic                  Unit number
* @param[in]    samplingCapabilities  structure for asic sampling capabilities 
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     Not able to get asic type of this unit or
*                                     BHD feature is not present or
*                                     BHD south bound function has returned failure
*
* @retval   BVIEW_STATUS_SUCCESS      packet count get is successful 
*
* @retval   BVIEW_STATUS_UNSUPPORTED  packet count get functionality is 
*                                     not supported on this unit
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS sbapi_bhd_black_holed_pkt_count_get (int asic, uint64_t *pktCount)
{
  BVIEW_SB_BHD_FEATURE_t *bhdFeaturePtr = NULL;
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
  bhdFeaturePtr =
    (BVIEW_SB_BHD_FEATURE_t *) sb_redirect_feature_handle_get (asicType,
                                                               BVIEW_FEATURE_BHD);
  /* Validate feature pointer and south bound handler. 
   * Call south bound handler                        */    
  if (bhdFeaturePtr == NULL)
  {
    rv = BVIEW_STATUS_FAILURE;
  }       
  else if (bhdFeaturePtr->bhd_black_holed_pkt_count_get_cb == NULL)
  {
    rv = BVIEW_STATUS_UNSUPPORTED;
  }
  else
  {  
    rv = bhdFeaturePtr->bhd_black_holed_pkt_count_get_cb(asic, pktCount);
  }
  /* Release read lock */
  SB_REDIRECT_RWLOCK_UNLOCK (sbRedirectRWLock);
  return rv;
}


