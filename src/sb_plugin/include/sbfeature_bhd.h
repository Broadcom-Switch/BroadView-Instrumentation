/*********************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename sbfeature_bhd.h 
  *
  * @purpose BroadView BHD Application south bound apis 
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

#ifndef INCLUDE_SBPLUGIN_BHD_H
#define INCLUDE_SBPLUGIN_BHD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "broadview.h"
#include "sbplugin.h"
#include "bhd.h"

/** The Packet Trace Southbound Feature Definition */

typedef struct _bhd_sb_feature_
{
    /** parent object */
    BVIEW_SB_FEATURE_t feature;

    /** get the sampling status  */
    BVIEW_STATUS(*bhd_sflow_sampling_status_get_cb)(int asic, int port,
                                                BVIEW_BHD_PORT_SFLOW_SAMPLING_STATUS_t  *status);

    /* cancel black hole */
    BVIEW_STATUS(*bhd_cancel_black_hole_cb)(int asic);

    /** configure black hole */
    BVIEW_STATUS(*bhd_configure_black_hole_cb)(int asic, 
                                                   BVIEW_BLACK_HOLE_CONFIG_t  *config);

    /** Obtain asic sampling capabilitues  */
    BVIEW_STATUS(*bhd_asic_sampling_capabilties_get_cb)(int asic, int *sampling_capabilitues);

    /** Obtain Compete Drop Counter Report */
    BVIEW_STATUS(*bhd_black_holed_pkt_count_get_cb)(int asic, uint64_t *count); 

} BVIEW_SB_BHD_FEATURE_t;

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SBPLUGIN_BHD_H */

