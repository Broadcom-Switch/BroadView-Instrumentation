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

#ifndef INCLUDE_SBPLUGIN_BST_H
#define INCLUDE_SBPLUGIN_BST_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "broadview.h"
#include "sbfeature_bst.h"
#include "sbplugin_bst_map.h"


/** BST feature support ASIC's Mask*/
#define  BVIEW_BST_SUPPORT_MASK    (BVIEW_ASIC_TYPE_TD2 | BVIEW_ASIC_TYPE_TH)

/* Check _asic (unit) is valid along with _data & _time for NULL pointer */
#define  BVIEW_BST_INPUT_VALIDATE(_asic,_data,_time)                   \
                            if (((_data) == NULL) || ((_time) == NULL) ||  \
                                (SB_OVSDB_RV_ERROR(SB_OVSDB_API_UNIT_CHECK(_asic))))              \
                            {                                                                     \
                               SB_OVSDB_DEBUG_PRINT ("Invalid input data ASIC %d", _asic );       \
                              return BVIEW_STATUS_INVALID_PARAMETER;                              \
                            }                                          

/* Check _asic (unit) is valid along with _data NULL pointer check*/
#define  BVIEW_BST_INPUT_VALID_CHECK(_asic,_data)                      \
                            if (((_data) == NULL) ||                     \
                                (SB_OVSDB_RV_ERROR(SB_OVSDB_API_UNIT_CHECK(_asic))))              \
                            {                                                                     \
                               SB_OVSDB_DEBUG_PRINT ("Invalid input data ASIC %d", _asic );       \
                              return BVIEW_STATUS_INVALID_PARAMETER;                              \
                            }

/*********************************************************************
* @brief  OVSDB South Bound BST feature init
*
* @param[in,out]  ovsdbBstFeat     - BST feature data structure
*
* @retval   BVIEW_STATUS_SUCCESS if BST feature is
*                                initialized successfully.
* @retval   BVIEW_STATUS_FAILURE if initialization is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_init (BVIEW_SB_BST_FEATURE_t *ovsdbBstFeat);

/*********************************************************************
* @brief  BST feature configuration set function
*
* @param[in]   asic                  - unit
* @param[in]   data                  - BST config structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if config set is failed.
* @retval BVIEW_STATUS_SUCCESS           if config set is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_config_set (int asic, BVIEW_BST_CONFIG_t *data);

/*********************************************************************
* @brief  Get BST configuration 
*
* @param[in]   asic                  - unit
* @param[out]  data                  - BST config structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if config get is failed.
* @retval BVIEW_STATUS_SUCCESS           if config get is success.
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_config_get (int asic, BVIEW_BST_CONFIG_t *data);

/*********************************************************************
* @brief  Obtain Complete ASIC Statistics Report
*
* @param[in]      asic               - unit
* @param[out]     snapshot           - snapshot data structure
* @param[out]     time               - time
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if snapshot get is failed.
* @retval BVIEW_STATUS_SUCCESS           if snapshot get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_snapshot_get (int asic, 
                                 BVIEW_BST_ASIC_SNAPSHOT_DATA_t *snapshot, 
                                 BVIEW_TIME_t *time);

/*********************************************************************
* @brief  Obtain Device Statistics
*
* @param[in]   asic             - unit
* @param[out]  data             - Device data structure
* @param[out]  time             - time
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if device stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if device stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_device_data_get (int asic, 
                                    BVIEW_BST_DEVICE_DATA_t *data, 
                                    BVIEW_TIME_t *time);

/*********************************************************************
* @brief  Obtain Ingress Port + Priority Groups Statistics
*
* @param[in]   asic             - unit
* @param[out]  data             - i_p_pg data structure
* @param[out]  time             - time
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if ippg stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if ippg stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_ippg_data_get (int asic, 
                              BVIEW_BST_INGRESS_PORT_PG_DATA_t *data, 
                              BVIEW_TIME_t *time);

/*********************************************************************
* @brief  Obtain Ingress Port + Service Pools Statistics
*
* @param[in]   asic             - unit
* @param[out]  data             - i_p_sp data structure
* @param[out]  time             - time
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if ipsp stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if ipsp stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_ipsp_data_get (int asic, 
                                  BVIEW_BST_INGRESS_PORT_SP_DATA_t *data, 
                                  BVIEW_TIME_t *time);

/*********************************************************************
* @brief  Obtain Ingress Service Pools Statistics
*
* @param[in]   asic             - unit
* @param[out]  data             - i_sp structure
* @param[out]  time             - time
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if isp stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if isp stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_isp_data_get (int asic, 
                                 BVIEW_BST_INGRESS_SP_DATA_t *data, 
                                 BVIEW_TIME_t *time);
 
/*********************************************************************
* @brief  Obtain Egress Port + Service Pools Statistics
*
* @param[in]   asic             - unit
* @param[out]  data             - e_p_sp data structure
* @param[out]  time             - time
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if epsp stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if epsp stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_epsp_data_get (int asic, 
                                BVIEW_BST_EGRESS_PORT_SP_DATA_t *data, 
                                BVIEW_TIME_t *time);

/*********************************************************************
* @brief  Obtain Egress Service Pools Statistics
*
* @param[in]   asic             - unit
* @param[out]  data             - e_sp data structure
* @param[out]  time             - time
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if esp stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if esp stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_esp_data_get  (int asic, 
                               BVIEW_BST_EGRESS_SP_DATA_t *data, 
                               BVIEW_TIME_t *time);

/*********************************************************************
* @brief  Obtain Egress Egress Unicast Queues Statistics
*
* @param[in]   asic             - unit
* @param[out]  data             - e_uc_q data structure
* @param[out]  time             - time
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if eucq stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if eucq stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_eucq_data_get (int asic, 
                              BVIEW_BST_EGRESS_UC_QUEUE_DATA_t *data, 
                              BVIEW_TIME_t *time);
   
/*********************************************************************
* @brief  Obtain Egress Egress Unicast Queue Groups Statistics
*
* @param[in]   asic             - unit
* @param[out]  data             - e_uc_qg data structure
* @param[out]  time             - time
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if eucqg stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if eucqg stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_eucqg_data_get (int asic, 
                        BVIEW_BST_EGRESS_UC_QUEUEGROUPS_DATA_t *data, 
                        BVIEW_TIME_t *time);

/*********************************************************************
* @brief  Obtain Egress Egress Multicast Queues Statistics
*
* @param[in]   asic             - unit
* @param[out]  data             - e_mc_q data structure
* @param[out]  time             - time
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if emcq stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if emcq stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_emcq_data_get (int asic, 
                              BVIEW_BST_EGRESS_MC_QUEUE_DATA_t *data, 
                              BVIEW_TIME_t *time);

/*********************************************************************
* @brief  Obtain Egress Egress CPU Queues Statistics
*
* @param[in]   asic             - unit
* @param[out]  data             - CPU queue data structure
* @param[out]  time             - time
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if CPU stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if CPU stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_cpuq_data_get (int asic, 
                             BVIEW_BST_EGRESS_CPU_QUEUE_DATA_t *data, 
                             BVIEW_TIME_t *time);

/*********************************************************************
* @brief  Obtain Egress Egress RQE Queues Statistics 
*
* @param[in]   asic             - unit
* @param[out]  data             - RQE data data structure
* @param[out]  time             - time
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if RQE stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if RQE stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_rqeq_data_get (int asic, 
                                   BVIEW_BST_EGRESS_RQE_QUEUE_DATA_t *data, 
                                   BVIEW_TIME_t *time); 

/*********************************************************************
* @brief  Set threshold configuration for Device Statistics
*
* @param[in]  asic               - unit
* @param[in]  thres              - Device threshold data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if threshold set is success.
* @retval BVIEW_STATUS_SUCCESS           if threshold set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_device_threshold_set (int asic, BVIEW_BST_DEVICE_THRESHOLD_t *thres);

/*********************************************************************
* @brief  Set threshold configuration for  
*           Ingress Port + Priority Groups Statistics
*
* @param[in]  asic              - unit
* @param[in]  port              - port
* @param[in]  pg                - Priority Group
* @param[in]  thres             - Threshold structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if threshold set is success.
* @retval BVIEW_STATUS_SUCCESS           if threshold set is failed.
*                                                             
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_ippg_threshold_set (int asic, int port, int pg, 
                                     BVIEW_BST_INGRESS_PORT_PG_THRESHOLD_t *thres);

/*********************************************************************
* @brief  Set threshold configuration for Ingress Port + Service Pools 
*           Statistics
*
* @param[in] asic                     - unit
* @param[in] port                     - port
* @param[in] sp                       - service pool
* @param[in] thres                    - Threshold data structure
*                                                    
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if threshold set is success.
* @retval BVIEW_STATUS_SUCCESS           if threshold set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_ipsp_threshold_set (int asic, int port, int sp, 
                                      BVIEW_BST_INGRESS_PORT_SP_THRESHOLD_t *thres);

/*********************************************************************
* @brief  Set threshold configuration for Ingress Service Pools 
*           Statistics
*
* @param[in] asic                       - unit
* @param[in] sp                         - service pool 
* @param[in] thres                      - Threshold data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if threshold set is success.
* @retval BVIEW_STATUS_SUCCESS           if threshold set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_isp_threshold_set (int asic, 
                                     int sp, 
                                     BVIEW_BST_INGRESS_SP_THRESHOLD_t *thres);

/*********************************************************************
* @brief  Set Profile configuration for Egress Port + Service Pools 
*           Statistics
*
* @param[in] asic                       - unit
* @param[in] port                       - port
* @param[in] sp                         - service pool
* @param[in] thres                      - Threshold data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if threshold set is success.
* @retval BVIEW_STATUS_SUCCESS           if threshold set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_epsp_threshold_set (int asic, 
                                      int port, int sp, 
                                      BVIEW_BST_EGRESS_PORT_SP_THRESHOLD_t *thres);

/*********************************************************************
* @brief  Set threshold configuration for Egress Service Pools Statistics
*
* @param[in] asic                        - unit
* @param[in] sp                          - service pool
* @param[in] thres                       - Threshold data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if threshold set is success.
* @retval BVIEW_STATUS_SUCCESS           if threshold set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_esp_threshold_set (int asic, 
                                     int sp, 
                                     BVIEW_BST_EGRESS_SP_THRESHOLD_t *thres);

/*********************************************************************
* @brief  Set threshold configuration for Egress Unicast Queues 
*           Statistics
*
* @param[in] asic                        - unit
* @param[in] ucQueue                     - uc queue
* @param[in] thres                       - Threshold data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if threshold set is success.
* @retval BVIEW_STATUS_SUCCESS           if threshold set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_eucq_threshold_set (int asic, 
                                      int ucQueue, 
                              BVIEW_BST_EGRESS_UC_QUEUE_THRESHOLD_t *thres);

/*********************************************************************
* @brief  Set threshold configuration for Egress Unicast Queue Groups 
*           Statistics 
*
* @param[in]asic                          -unit
* @param[in]ucQueueGrp                    -uc queue group 
* @param[in]thres                         -Threshold data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if threshold set is success.
* @retval BVIEW_STATUS_SUCCESS           if threshold set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_eucqg_threshold_set (int asic, 
                                       int ucQueueGrp, 
                                       BVIEW_BST_EGRESS_UC_QUEUEGROUPS_THRESHOLD_t *thres);

/*********************************************************************
* @brief  Set threshold configuration for Egress Multicast Queues 
*           Statistics
*
* @param[in] asic                     - unit
* @param[in] mcQueue                  - mcQueue
* @param[in] thres                    - Threshold data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if threshold set is success.
* @retval BVIEW_STATUS_SUCCESS           if threshold set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_emcq_threshold_set (int asic, 
                                      int mcQueue, 
                                      BVIEW_BST_EGRESS_MC_QUEUE_THRESHOLD_t *thres);

/*********************************************************************
* @brief  Set threshold configuration for Egress Egress CPU Queues 
*           Statistics
*
* @param[in] asic                        - unit
* @param[in] cpuQueue                    - cpuQueue
* @param[in] thres                       - Threshold data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if threshold set is success.
* @retval BVIEW_STATUS_SUCCESS           if threshold set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_cpuq_threshold_set (int asic, 
                                      int cpuQueue, 
                                      BVIEW_BST_EGRESS_CPU_QUEUE_THRESHOLD_t *thres);

/*********************************************************************
* @brief  Set threshold configuration for Egress Egress RQE Queues 
*           Statistics
*
* @param[in]    asic                   - unit
* @param[in]    rqeQueue               - rqeQueue
* @param[out]   thres                  - Threshold data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if threshold set is success.
* @retval BVIEW_STATUS_SUCCESS           if threshold set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_rqeq_threshold_set (int asic, 
                                     int rqeQueue, 
                                     BVIEW_BST_EGRESS_RQE_QUEUE_THRESHOLD_t *thres);

/*********************************************************************
* @brief  Clear stats
*           
* @param[in]   asic                            - unit
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if clear stats is success.
* @retval BVIEW_STATUS_SUCCESS           if clear stats is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_ovsdb_bst_clear_stats (int asic);

/*********************************************************************
* @brief  Restore threshold configuration 
*
* @param   asic                              - unit
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if restore is success.
* @retval BVIEW_STATUS_SUCCESS           if restore set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_ovsdb_bst_clear_thresholds  (int asic);

/*********************************************************************
* @brief  Register hw trigger callback
*
* @param   asic                              - unit
* @param   callback                          - function to be called 
*                                              when trigger happens
* @param   cookie                            - user data
* 
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if restore is success.
* @retval BVIEW_STATUS_SUCCESS           if restore set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_ovsdb_bst_register_trigger (int asic, 
                                        BVIEW_BST_TRIGGER_CALLBACK_t callback, 
                                        void *cookie);

/*********************************************************************
* @brief  Get snapshot of all thresholds configured
*
*
* @param  [in]  asic                         - unit
* @param  [out] thresholdSnapshot            - Threshold snapshot
*                                              data structure
* @param  [out] time                         - Time
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if snapshot is success.
* @retval BVIEW_STATUS_SUCCESS           if snapshot set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_threshold_get (int asic, 
                              BVIEW_BST_ASIC_SNAPSHOT_DATA_t *thresholdSnapshot,
                              BVIEW_TIME_t * time);

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_SBPLUGIN_COMMON_BST_H */
  


