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

#ifndef INCLUDE_SBPLUGIN_COMMON_BST_H
#define INCLUDE_SBPLUGIN_COMMON_BST_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "broadview.h"
#include "sbfeature_bst.h"
#include "sbplugin_bst_map.h"


  /* structure to map bid to realm-counter */
  typedef struct _bst_realm_to_realm_counter_ {
    /* bid */
    int bid;
    /* realm string */
    char *realm;
    /* threshold  type*/
    char *counter;
  }SB_BRCM_BID_REALM_t;

/** BST feature support ASIC's Mask*/
#define  BVIEW_BST_SUPPORT_MASK    (BVIEW_ASIC_TYPE_TD2 | BVIEW_ASIC_TYPE_TH)
/** Maximum number of COS Queue per port*/
#define  BVIEW_BST_NUM_COS_PORT    8

/* Check _asic (unit) is valid along with _data & _time for NULL pointer */
#define  BVIEW_BST_INPUT_VALIDATE(_asic,_data,_time)                   \
                            if (((_data) == NULL) || ((_time) == NULL) ||  \
                                (SB_BRCM_RV_ERROR(SB_BRCM_API_UNIT_CHECK(_asic))))              \
                            {                                          \
                               SB_DEBUG_PRINT (BVIEW_LOG_ERROR,"Invalid input data ASIC %d", \
                                  _asic );                             \
                              return BVIEW_STATUS_INVALID_PARAMETER;   \
                            }                                          

/* Check _asic (unit) is valid along with _data NULL pointer check*/
#define  BVIEW_BST_INPUT_VALID_CHECK(_asic,_data)                      \
                            if (((_data) == NULL) ||                     \
                                (SB_BRCM_RV_ERROR(SB_BRCM_API_UNIT_CHECK(_asic))))              \
                            {                                          \
                               SB_DEBUG_PRINT (BVIEW_LOG_ERROR,"Invalid input data ASIC %d", \
                                  _asic );                             \
                              return BVIEW_STATUS_INVALID_PARAMETER;   \
                            }

/* sync to copy HW stats*/
#define BVIEW_BST_STAT_SYNC(_asic,_bid)                                    \
                  if (SB_BRCM_API_COSQ_BST_STAT_SYNC((_asic),(_bid)) != SB_BRCM_E_NONE)    \
                  {                                                        \
                     SB_DEBUG_PRINT (BVIEW_LOG_ERROR,"Stat sync failed for ASIC %d BID %d", \
                                  (_asic),( _bid));                            \
                     return BVIEW_STATUS_FAILURE;                          \
                  }  

/* Macro to iterate all ports*/
#define  BVIEW_BST_PORT_ITER(_asic,_port)                                         \
              for ((_port) = 1; (_port) <= asicDb[(_asic)].scalingParams.numPorts; (_port)++)

/* Macro to iterate all Priority Groups*/
#define  BVIEW_BST_PG_ITER(_pg)                                                 \
              for ((_pg) = 0; (_pg) < BVIEW_ASIC_MAX_PRIORITY_GROUPS; (_pg)++)

/* Macro to iterate all Service Pools*/                           
#define  BVIEW_BST_SP_ITER(_sp)                                                 \
              for ((_sp) = 0; (_sp) < BVIEW_ASIC_MAX_SERVICE_POOLS; (_sp)++)

/* Macro to iterate all Service Pools + Common Service Pool*/
#define  BVIEW_BST_TOTAL_SP_ITER(_sp)                                           \
              for ((_sp) = 0; (_sp) < BVIEW_ASIC_MAX_INGRESS_SERVICE_POOLS; (_sp)++)


/* Macro to iterate 'n'  times*/
#define BVIEW_BST_ITER(_index,_n)                              \
              for ((_index) = 0; (_index) < (_n); (_index)++)


#define SB_BRCM_BST_STAT_ID_MAX 13  
/*********************************************************************
* @brief  BST feature init
*
* @param[in,out]  bcmBst     - BST feature data structure
*
* @retval   BVIEW_STATUS_SUCCESS if BST feature is
*                                initialized successfully.
* @retval   BVIEW_STATUS_FAILURE if initialization is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_bst_init (BVIEW_SB_BST_FEATURE_t *bcmBst);

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
BVIEW_STATUS sbplugin_common_bst_config_set (int asic, BVIEW_BST_CONFIG_t *data);

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
BVIEW_STATUS sbplugin_common_bst_config_get (int asic, BVIEW_BST_CONFIG_t *data);

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
BVIEW_STATUS sbplugin_common_bst_snapshot_get (int asic, 
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
BVIEW_STATUS sbplugin_common_bst_device_data_get (int asic, 
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
BVIEW_STATUS sbplugin_common_bst_ippg_data_get (int asic, 
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
BVIEW_STATUS sbplugin_common_bst_ipsp_data_get (int asic, 
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
BVIEW_STATUS sbplugin_common_bst_isp_data_get (int asic, 
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
BVIEW_STATUS sbplugin_common_bst_epsp_data_get (int asic, 
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
BVIEW_STATUS sbplugin_common_bst_esp_data_get  (int asic, 
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
BVIEW_STATUS sbplugin_common_bst_eucq_data_get (int asic, 
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
BVIEW_STATUS sbplugin_common_bst_eucqg_data_get (int asic, 
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
BVIEW_STATUS sbplugin_common_bst_emcq_data_get (int asic, 
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
BVIEW_STATUS sbplugin_common_bst_cpuq_data_get (int asic, 
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
BVIEW_STATUS sbplugin_common_bst_rqeq_data_get (int asic, 
                                   BVIEW_BST_EGRESS_RQE_QUEUE_DATA_t *data, 
                                   BVIEW_TIME_t *time); 

/*********************************************************************
* @brief  Set profile configuration for Device Statistics
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
BVIEW_STATUS sbplugin_common_bst_device_threshold_set (int asic, BVIEW_BST_DEVICE_THRESHOLD_t *thres);

/*********************************************************************
* @brief  Set profile configuration for  
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
BVIEW_STATUS sbplugin_common_bst_ippg_threshold_set (int asic, int port, int pg, 
                                     BVIEW_BST_INGRESS_PORT_PG_THRESHOLD_t *thres);

/*********************************************************************
* @brief  Set profile configuration for Ingress Port + Service Pools 
*           Statistics
*
* @param[in] asic                     - unit
* @param[in] port                     - port
* @param[in] sp                       - service pool
* @param[in] thres                    - Threshold data structure
*                                                    
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if threshold set is succes.
* @retval BVIEW_STATUS_SUCCESS           if threshold set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_bst_ipsp_threshold_set (int asic, int port, int sp, 
                                      BVIEW_BST_INGRESS_PORT_SP_THRESHOLD_t *thres);

/*********************************************************************
* @brief  Set profile configuration for Ingress Service Pools 
*           Statistics
*
* @param[in] asic                       - unit
* @param[in] sp                         - service pool 
* @param[in] thres                      - Threshold data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if threshold set is succes.
* @retval BVIEW_STATUS_SUCCESS           if threshold set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_bst_isp_threshold_set (int asic, 
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
* @retval BVIEW_STATUS_FAILURE           if threshold set is succes.
* @retval BVIEW_STATUS_SUCCESS           if threshold set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_bst_epsp_threshold_set (int asic, 
                                      int port, int sp, 
                                      BVIEW_BST_EGRESS_PORT_SP_THRESHOLD_t *thres);

/*********************************************************************
* @brief  Set profile configuration for Egress Service Pools Statistics
*
* @param[in] asic                        - unit
* @param[in] sp                          - service pool
* @param[in] thres                       - Threshold data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if threshold set is succes.
* @retval BVIEW_STATUS_SUCCESS           if threshold set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_bst_esp_threshold_set (int asic, 
                                     int sp, 
                                     BVIEW_BST_EGRESS_SP_THRESHOLD_t *thres);

/*********************************************************************
* @brief  Set profile configuration for Egress Unicast Queues 
*           Statistics
*
* @param[in] asic                        - unit
* @param[in] ucQueue                     - uc queue
* @param[in] thres                       - Threshold data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if threshold set is succes.
* @retval BVIEW_STATUS_SUCCESS           if threshold set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_bst_eucq_threshold_set (int asic, 
                                      int ucQueue, 
                              BVIEW_BST_EGRESS_UC_QUEUE_THRESHOLD_t *thres);

/*********************************************************************
* @brief  Set profile configuration for Egress Unicast Queue Groups 
*           Statistics 
*
* @param[in]asic                          -unit
* @param[in]ucQueueGrp                    -uc queue group 
* @param[in]thres                         -Threshold data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if threshold set is succes.
* @retval BVIEW_STATUS_SUCCESS           if threshold set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_bst_eucqg_threshold_set (int asic, 
                                       int ucQueueGrp, 
                                       BVIEW_BST_EGRESS_UC_QUEUEGROUPS_THRESHOLD_t *thres);

/*********************************************************************
* @brief  Set profile configuration for Egress Multicast Queues 
*           Statistics
*
* @param[in] asic                     - unit
* @param[in] mcQueue                  - mcQueue
* @param[in] thres                    - Threshold data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if threshold set is succes.
* @retval BVIEW_STATUS_SUCCESS           if threshold set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_bst_emcq_threshold_set (int asic, 
                                      int mcQueue, 
                                      BVIEW_BST_EGRESS_MC_QUEUE_THRESHOLD_t *thres);

/*********************************************************************
* @brief  Set profile configuration for Egress Egress CPU Queues 
*           Statistics
*
* @param[in] asic                        - unit
* @param[in] cpuQueue                    - cpuQueue
* @param[in] thres                       - Threshold data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if threshold set is succes.
* @retval BVIEW_STATUS_SUCCESS           if threshold set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_bst_cpuq_threshold_set (int asic, 
                                      int cpuQueue, 
                                      BVIEW_BST_EGRESS_CPU_QUEUE_THRESHOLD_t *thres);

/*********************************************************************
* @brief  Set profile configuration for Egress Egress RQE Queues 
*           Statistics
*
* @param[in]    asic                   - unit
* @param[in]    rqeQueue               - rqeQueue
* @param[out]   thres                  - Threshold data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if threshold set is succes.
* @retval BVIEW_STATUS_SUCCESS           if threshold set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_bst_rqeq_threshold_set (int asic, 
                                     int rqeQueue, 
                                     BVIEW_BST_EGRESS_RQE_QUEUE_THRESHOLD_t *thres);

/*********************************************************************
* @brief  Clear stats
*           
* @param[in]   asic                            - unit
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if clear stats is succes.
* @retval BVIEW_STATUS_SUCCESS           if clear stats is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_common_bst_clear_stats (int asic);

/*********************************************************************
* @brief  Restore threshold configuration 
*
* @param   asic                              - unit
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if restore is succes.
* @retval BVIEW_STATUS_SUCCESS           if restore set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_common_bst_clear_thresholds  (int asic);

/*********************************************************************
* @brief  Register hw trigger callback
*
* @param   asic                              - unit
* @param   callback                          - function to be called 
*                                              when trigger happens
* @param   cookie                            - user data
* 
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if restore is succes.
* @retval BVIEW_STATUS_SUCCESS           if restore set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_common_bst_register_trigger (int asic, 
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
* @retval BVIEW_STATUS_FAILURE           if snapshot is succes.
* @retval BVIEW_STATUS_SUCCESS           if snapshot set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_bst_threshold_get (int asic, 
                              BVIEW_BST_ASIC_SNAPSHOT_DATA_t *thresholdSnapshot,
                              BVIEW_TIME_t * time);

/*********************************************************************
* @brief  callback function to process Hw trigers
*
*
* @param  [in]  asic                         - unit
* @param  [in]  event                        - Event
* @param  [bid] bid                          - BST stat
* @param  [port] Port                        - Port ID
* @param  [cosq] COSQ                        - cosq
* @param  [in,out] cookie                    - User data
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if HW trigger process is success.
* @retval BVIEW_STATUS_SUCCESS           if failed to handle trigger.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_bst_callback (int asic, SB_BRCM_SWITCH_EVENT_t event,
                       int bid, int port, int cosq, void *cookie);

/*********************************************************************
* @brief   Get unicast congestion drop counter of a particular port-queue
*                     combination
*
*
* @param[in]  asic                                   - unit
* @param[in]  port                                   - port number
* @param[in]  queue                                  - Queue number
* @param[out] dropCount                              - Drop counter value
*
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if drop counter get is succes.
* @retval BVIEW_STATUS_SUCCESS           if drop counter get is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_bst_port_ucast_cgs_drop_get(int asic, int port, 
                                                int queue, uint64_t *dropCount);



/*********************************************************************
* @brief   Get multicast congestion drop counter of a particular port-queue
*                     combination
*
*
* @param[in]  asic                                   - unit
* @param[in]  port                                   - port number
* @param[in]  queue                                  - Queue number
* @param[out] dropCount                              - Drop counter value
*
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if drop counter get is succes.
* @retval BVIEW_STATUS_SUCCESS           if drop counter get is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_bst_port_mcast_cgs_drop_get(int asic, int port, 
                                              int queue, uint64_t *dropCount);


/*********************************************************************
* @brief   Get Total congestion drop counter of a particular port
*
*
* @param[in]  asic                                   - unit
* @param[in]  port                                   - port number
* @param[out] dropCount                              - Drop counter value
*
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if drop counter get is succes.
* @retval BVIEW_STATUS_SUCCESS           if drop counter get is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_bst_port_total_cgs_drop_get(int asic, int port, 
                                                         uint64_t *dropCount);

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_SBPLUGIN_COMMON_BST_H */
  


