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

#ifndef INCLUDE_SBPLUGIN_PACKET_TRACE_H
#define INCLUDE_SBPLUGIN_PACKET_TRACE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "broadview.h"
#include "sbplugin.h"
#include "packet_trace.h"

/** The Packet Trace Southbound Feature Definition */

typedef struct _pt_sb_feature_
{
    /** parent object */
    BVIEW_SB_FEATURE_t feature;

    /** Feature configuration set/get functions */
    BVIEW_STATUS(*pt_drop_reason_config_set_cb)(int asic, 
                                                BVIEW_PT_DROP_REASON_CONFIG_t *config);
    BVIEW_STATUS(*pt_drop_reason_config_get_cb)(int asic, 
                                                BVIEW_PT_DROP_REASON_CONFIG_t *config);

    /** Supported list of  drop reasons*/
    BVIEW_STATUS(*pt_supported_drop_reason_get_cb)(int asic, 
                                                   BVIEW_PT_DROP_REASON_MASK_t *mask);

    /** Obtain Trace Profile Report */
    BVIEW_STATUS(*pt_trace_profile_get_cb)(int asic, int  port,
                                           BVIEW_PT_PACKET_t *packet,
                                           BVIEW_PT_TRACE_PROFILE_t *traceProfile, 
                                           BVIEW_TIME_t *time);

    /** Obtain Compete Drop Counter Report */
    BVIEW_STATUS(*pt_drop_counter_get_cb)(int asic, 
                                          BVIEW_PT_DROP_COUNTER_REPORT_t *data, 
                                          BVIEW_TIME_t *time);

    /** Clear Drop Reasons Configuration*/
    BVIEW_STATUS(*pt_clear_drop_reason_cb)(int asic);     

    /** register a callback to be invoked when packet droped of matching configured drop reason*/
    BVIEW_STATUS(*pt_register_trigger_cb) (int asic, 
                                           BVIEW_PT_TRIGGER_CALLBACK_t callback, 
                                           void *cookie);

   /** Create 5 tuple profile to capture live traffic     */
   BVIEW_STATUS(*pt_5_tuple_profile_create_cb) (int asic, int request_id,
                                BVIEW_PORT_MASK_t *port_list, PT_5_TUPLE_PARAMS_t *tuple_params);

  /** Delete 5 tuple profile          */
  BVIEW_STATUS (*pt_5_tuple_profile_delete_cb) (int asic, int request_id);

} BVIEW_SB_PT_FEATURE_t;

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SBPLUGIN_PACKET_TRACE_H */

