/*! \file sbplugin_redirect_bhd.h
 */

/*! @addtogroup GLUSMDB API Definitions
 *  @{
 *    @addtogroup GLBVIEWSBREDIRECTORBHD BroadView BHD SB Redirector Feature Declarations And Definitions [BVIEW SB REDIRECTOR]
 *    @{
 */
/*********************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename sbplugin_redirect_bhd.h 
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


#ifndef INCLUDE_SBPLUGIN_REDIRECT_BHD_H
#define	INCLUDE_SBPLUGIN_REDIRECT_BHD_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include "broadview.h"
#include "openapps_feature.h"
#include "bhd.h"


/*****************************************************************//**
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
                                BVIEW_BHD_PORT_SFLOW_SAMPLING_STATUS_t *status);

/*****************************************************************//**
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
BVIEW_STATUS sbapi_bhd_cancel_black_hole (int asic);

/*****************************************************************//**
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
BVIEW_STATUS sbapi_bhd_configure_black_hole (int asic, BVIEW_BLACK_HOLE_CONFIG_t *config);
/*****************************************************************//**
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
BVIEW_STATUS sbapi_bhd_asic_sampling_capabilties_get (int asic, int *samplingCapabilities);

/*****************************************************************//**
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
BVIEW_STATUS sbapi_bhd_black_holed_pkt_count_get (int asic, uint64_t *pktCount);



#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_SBPLUGIN_REDIRECT_BHD_H */

/*!  @}
 * @}
 */
