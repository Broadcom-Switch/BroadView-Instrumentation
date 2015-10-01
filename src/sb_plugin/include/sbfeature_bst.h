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

#ifndef INCLUDE_SBFEATURE_BST_H
#define INCLUDE_SBFEATURE_BST_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "broadview.h"
#include "sbplugin.h"
#include "bst.h"

/** The BST Southbound Feature Definition */

typedef struct _bst_sb_feature_
{
    /** parent object */
    BVIEW_SB_FEATURE_t feature;

    /** Feature configuration set/get functions */
    BVIEW_STATUS(*bst_config_set_cb)(int asic, BVIEW_BST_CONFIG_t *config);
    BVIEW_STATUS(*bst_config_get_cb)(int asic, BVIEW_BST_CONFIG_t *config);

    /** Obtain Complete ASIC Statistics Report */
    BVIEW_STATUS(*bst_snapshot_get_cb)(int asic, BVIEW_BST_ASIC_SNAPSHOT_DATA_t *snapshot, BVIEW_TIME_t *time);

    /** Obtain Device Statistics */
    BVIEW_STATUS(*bst_device_data_get_cb)(int asic, BVIEW_BST_DEVICE_DATA_t *data, BVIEW_TIME_t *time);

    /** Obtain Ingress Port + Priority Groups Statistics */
    BVIEW_STATUS(*bst_ippg_data_get_cb)(int asic, BVIEW_BST_INGRESS_PORT_PG_DATA_t *data, BVIEW_TIME_t *time);

    /** Obtain Ingress Port + Service Pools Statistics */
    BVIEW_STATUS(*bst_ipsp_data_get_cb)(int asic, BVIEW_BST_INGRESS_PORT_SP_DATA_t *data, BVIEW_TIME_t *time);

    /** Obtain Ingress Service Pools Statistics */
    BVIEW_STATUS(*bst_isp_data_get_cb)(int asic, BVIEW_BST_INGRESS_SP_DATA_t *data, BVIEW_TIME_t *time);

    /** Obtain Egress Port + Service Pools Statistics */
    BVIEW_STATUS(*bst_epsp_data_get_cb)(int asic, BVIEW_BST_EGRESS_PORT_SP_DATA_t *data, BVIEW_TIME_t *time);

    /** Obtain Egress Service Pools Statistics */
    BVIEW_STATUS(*bst_esp_data_get_cb)(int asic, BVIEW_BST_EGRESS_SP_DATA_t *data, BVIEW_TIME_t *time);

    /** Obtain Egress Egress Unicast Queues Statistics */
    BVIEW_STATUS(*bst_eucq_data_get_cb)(int asic, BVIEW_BST_EGRESS_UC_QUEUE_DATA_t *data, BVIEW_TIME_t *time);

    /** Obtain Egress Egress Unicast Queue Groups Statistics */
    BVIEW_STATUS(*bst_eucqg_data_get_cb)(int asic, BVIEW_BST_EGRESS_UC_QUEUEGROUPS_DATA_t *data, BVIEW_TIME_t *time);

    /** Obtain Egress Egress Multicast Queues Statistics */
    BVIEW_STATUS(*bst_emcq_data_get_cb)(int asic, BVIEW_BST_EGRESS_MC_QUEUE_DATA_t *data, BVIEW_TIME_t *time);

    /** Obtain Egress Egress CPU Queues Statistics */
    BVIEW_STATUS(*bst_cpuq_data_get_cb)(int asic, BVIEW_BST_EGRESS_CPU_QUEUE_DATA_t *data, BVIEW_TIME_t *time);

    /** Obtain Egress Egress RQE Queues Statistics */
    BVIEW_STATUS(*bst_rqeq_data_get_cb)(int asic, BVIEW_BST_EGRESS_RQE_QUEUE_DATA_t *data, BVIEW_TIME_t *time);

    /** Set threshold configuration for Device Statistics */
    BVIEW_STATUS(*bst_device_threshold_set_cb)(int asic, BVIEW_BST_DEVICE_THRESHOLD_t *threshold);

    /** Set threshold configuration for  Ingress Port + Priority Groups Statistics */
    BVIEW_STATUS(*bst_ippg_threshold_set_cb)(int asic, int port, int pg, BVIEW_BST_INGRESS_PORT_PG_THRESHOLD_t *threshold);

    /** Set threshold configuration for Ingress Port + Service Pools Statistics */
    BVIEW_STATUS(*bst_ipsp_threshold_set_cb)(int asic, int port, int sp, BVIEW_BST_INGRESS_PORT_SP_THRESHOLD_t *threshold);

    /** Set threshold configuration for Ingress Service Pools Statistics */
    BVIEW_STATUS(*bst_isp_threshold_set_cb)(int asic, int sp, BVIEW_BST_INGRESS_SP_THRESHOLD_t *threshold);

    /** Set Profile configuration for Egress Port + Service Pools Statistics */
    BVIEW_STATUS(*bst_epsp_threshold_set_cb)(int asic, int port, int sp, BVIEW_BST_EGRESS_PORT_SP_THRESHOLD_t *threshold);

    /** Set threshold configuration for Egress Service Pools Statistics */
    BVIEW_STATUS(*bst_esp_threshold_set_cb)(int asic, int sp, BVIEW_BST_EGRESS_SP_THRESHOLD_t *threshold);

    /** Set threshold configuration for Egress Egress Unicast Queues Statistics */
    BVIEW_STATUS(*bst_eucq_threshold_set_cb)(int asic, int ucQueue, BVIEW_BST_EGRESS_UC_QUEUE_THRESHOLD_t *threshold);

    /** Set threshold configuration for Egress Egress Unicast Queue Groups Statistics */
    BVIEW_STATUS(*bst_eucqg_threshold_set_cb)(int asic, int ucQueueGrp, BVIEW_BST_EGRESS_UC_QUEUEGROUPS_THRESHOLD_t *threshold);

    /** Set threshold configuration for Egress Egress Multicast Queues Statistics */
    BVIEW_STATUS(*bst_emcq_threshold_set_cb)(int asic, int mcQueue, BVIEW_BST_EGRESS_MC_QUEUE_THRESHOLD_t *threshold);

    /** Set threshold configuration for Egress Egress CPU Queues Statistics */
    BVIEW_STATUS(*bst_cpuq_threshold_set_cb)(int asic, int cpuQueue, BVIEW_BST_EGRESS_CPU_QUEUE_THRESHOLD_t *threshold);

    /** Set threshold configuration for Egress Egress RQE Queues Statistics */
    BVIEW_STATUS(*bst_rqeq_threshold_set_cb)(int asic, int rqeQueue, BVIEW_BST_EGRESS_RQE_QUEUE_THRESHOLD_t *threshold);

    /** Get all threshold configurations  */
    BVIEW_STATUS(*bst_threshold_get_cb)(int asic, BVIEW_BST_ASIC_SNAPSHOT_DATA_t *thresholdSnapshot, BVIEW_TIME_t *time);

    /** Clear all statistics  */
    BVIEW_STATUS(*bst_clear_stats_cb) (int asic);

    /** Clear all thresholds */
    BVIEW_STATUS(*bst_clear_thresholds_cb) (int asic);
    
    /** register a callback to be invoked when a configured trigger goes off*/
    BVIEW_STATUS(*bst_register_trigger_cb) (int asic, BVIEW_BST_TRIGGER_CALLBACK_t callback, void *cookie);

    /** Obtain Complete ASIC default buffer values  */
    BVIEW_STATUS(*bst_default_snapshot_get_cb)(int asic, BVIEW_BST_ASIC_SNAPSHOT_DATA_t *snapshot);
} BVIEW_SB_BST_FEATURE_t;

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SBPLUGIN_BST_H */
