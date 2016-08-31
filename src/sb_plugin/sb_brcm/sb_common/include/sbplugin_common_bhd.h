/************************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename sbplugin_common_bhd.h
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

#ifndef INCLUDE_SBPLUGIN_COMMON_BHD_H
#define INCLUDE_SBPLUGIN_COMMON_BHD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "broadview.h"
#include "bhd.h"
#include "sbfeature_bhd.h"
#include "sbplugin_system_map.h"


/** BHD feature support ASIC's Mask*/
#define  BVIEW_BHD_SUPPORT_MASK    (BVIEW_ASIC_TYPE_TD2 | BVIEW_ASIC_TYPE_TH | BVIEW_ASIC_TYPE_QUMRAN)

/*********************************************************************
* @brief  BCM Black Hold Detection feature init
*
* @param[in,out]  bcmBhd     - Black Hold Detection feature data structure
*
* @retval   BVIEW_STATUS_SUCCESS if Black Hold Detection feature is
*                                initialized successfully.
* @retval   BVIEW_STATUS_FAILURE if initialization is failed.
* @retval   BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_bhd_init (BVIEW_SB_BHD_FEATURE_t *bcmBhd);

/*********************************************************************
* @brief  Get Black Hole sflow sampling status get
*
* @param[in]   asic                  - unit
* @param[in]   port                  - Port
* @param[in]   status                - Pointer to SFLOW sampling status
*                                      structure.
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if failed to get SFLOW status.
* @retval BVIEW_STATUS_SUCCESS           if success to get SFLOW status
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_bhd_sflow_status_get (int asic, int port,
                       BVIEW_BHD_PORT_SFLOW_SAMPLING_STATUS_t *status);

/*********************************************************************
* @brief  Delete Black Hold Detection policy
*
* @param[in]   asic                  - unit
*
* @retval BVIEW_STATUS_FAILURE           if failed to delete 
*                                        Black Hold Detection policy.
* @retval BVIEW_STATUS_SUCCESS           if success to delete 
*                                        Black Hold Detection policy.
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_bhd_cancel_black_hole (int asic);

/*********************************************************************
* @brief  Create black hole policy and apply.
*
* @param[in]  asic               - unit
* @param[in]  config             - Pointer to Black Hold Detection 
*                                  configuration structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if failed to create 
*                                        Black Hold Detection policy.
* @retval BVIEW_STATUS_SUCCESS           if Black Hold Detection 
*                                        creation is successful..
*
* @notes
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_bhd_config_black_hole (int asic,
                                 BVIEW_BLACK_HOLE_CONFIG_t  *config);

/*********************************************************************
* @brief  Get ASIC sampling capablities
*
* @param[in]    asic                     - unit
* @param[out]   sampling_cap             - Pointer to sampling capablity
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if failed to get sampling capablities.
* @retval BVIEW_STATUS_SUCCESS           if success to get sampling capabilities.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_common_bhd_asic_sampling_cap_get(int asic,
                                                int *sampling_cap);

/*********************************************************************
* @brief  Get total black holed packets
*
* @param[in]   asic              - unit
* @param[out]  count             - Total Black holed packets
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if failed to get Black Holed packets.
* @retval BVIEW_STATUS_SUCCESS           if success to get Black Holed packets.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_bhd_pkt_count_get (int asic,  uint64_t *count);

/*********************************************************************
* @brief   Is packet Black holded
*
* @param[int]  pkt             -    Pointer to Packet structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if packet is not black holed.
* @retval BVIEW_STATUS_SUCCESS           if packet is black holed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_common_bhd_build_info (SB_BRCM_PKT_t *pkt,
                                              unsigned char *user_header);

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_SBPLUGIN_COMMON_BHD_H */
  


