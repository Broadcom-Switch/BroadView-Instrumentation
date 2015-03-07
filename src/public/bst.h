/*! \file bst.h
 */

/*! @addtogroup GLUSMDB API Definitions
 *  @{
 *    @addtogroup GLBVIEW BroadView BST Feature Declarations And Definitions [BVIEW]
 *    @{
 */
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

#ifndef INCLUDE_BST_H
#define INCLUDE_BST_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "broadview.h"
#include "sbplugin.h"

/* Buffer Count for the device */
typedef struct _bst_device_
{
    uint64_t bufferCount;
} BVIEW_BST_DEVICE_DATA_t;

/* Buffer Count for Ingress Port + Priority Groups */
typedef struct _bst_i_p_pg_
{

    struct _ippg_data_
    {
        uint64_t umShareBufferCount;
        uint64_t umHeadroomBufferCount;
    } data[BVIEW_ASIC_MAX_PORTS][BVIEW_ASIC_MAX_PRIORITY_GROUPS];

} BVIEW_BST_INGRESS_PORT_PG_DATA_t;

/* Buffer Count for Ingress Port + Service Pools */
typedef struct _bst_i_p_sp_
{

    struct _ipsp_data_
    {
        uint64_t umShareBufferCount;
    } data[BVIEW_ASIC_MAX_PORTS][BVIEW_ASIC_MAX_INGRESS_SERVICE_POOLS];

} BVIEW_BST_INGRESS_PORT_SP_DATA_t;

/* Buffer Count for Ingress Service Pools */
typedef struct _bst_i_sp_
{

    struct _isp_data_
    {
        uint64_t umShareBufferCount;
    } data[BVIEW_ASIC_MAX_INGRESS_SERVICE_POOLS];

} BVIEW_BST_INGRESS_SP_DATA_t;

/* Buffer Count for Egress Port + Service Pools */
typedef struct _bst_e_p_sp_
{

    struct _epsp_data_
    {
        uint64_t ucShareBufferCount;
        uint64_t umShareBufferCount;
        uint64_t mcShareBufferCount;
        uint64_t mcShareQueueEntries;
    } data[BVIEW_ASIC_MAX_PORTS][BVIEW_ASIC_MAX_SERVICE_POOLS];

} BVIEW_BST_EGRESS_PORT_SP_DATA_t;

/* Buffer Count for Egress Service Pools */
typedef struct _bst_e_sp_
{

    struct _esp_data_
    {
        uint64_t umShareBufferCount;
        uint64_t mcShareBufferCount;
        uint64_t mcShareQueueEntries;
    } data[BVIEW_ASIC_MAX_SERVICE_POOLS];

} BVIEW_BST_EGRESS_SP_DATA_t;

/* Buffer Count for Egress Unicast Queues */
typedef struct _bst_e_ucq_
{

    struct _eucq_data_
    {
        uint64_t ucBufferCount;
        uint64_t port; /* to indicate the port number using this queue */
    } data[BVIEW_ASIC_MAX_UC_QUEUES];

} BVIEW_BST_EGRESS_UC_QUEUE_DATA_t;

/* Buffer Count for Egress Unicast Queue Groups */
typedef struct _bst_e_ucqg_
{

    struct _eucqg_data_
    {
        uint64_t ucBufferCount;
    } data[BVIEW_ASIC_MAX_UC_QUEUE_GROUPS];

} BVIEW_BST_EGRESS_UC_QUEUEGROUPS_DATA_t;

/* Buffer Count for Egress Multicast Queues */
typedef struct _bst_e_mcq_
{

    struct _emcq_data_
    {
        uint64_t mcBufferCount;
        uint64_t mcQueueEntries;
        uint64_t port; /* to indicate the port number using this queue */
    } data[BVIEW_ASIC_MAX_MC_QUEUES];

} BVIEW_BST_EGRESS_MC_QUEUE_DATA_t;

/* Buffer Count for CPU Queues */
typedef struct _bst_cpu_q_
{

    struct _cpuq_data_
    {
        uint64_t cpuBufferCount;
        uint64_t cpuQueueEntries;
    } data[BVIEW_ASIC_MAX_CPU_QUEUES];

} BVIEW_BST_EGRESS_CPU_QUEUE_DATA_t;

/* Buffer Count for RQE Queues */
typedef struct _bst_rqe_q_
{

    struct _rqeq_data_
    {
        uint64_t rqeBufferCount;
        uint64_t rqeQueueEntries;
    } data[BVIEW_ASIC_MAX_RQE_QUEUES];

} BVIEW_BST_EGRESS_RQE_QUEUE_DATA_t;

/* A Complete Data set for a 'snapshot' */

typedef struct _bst_asic_data_snapshot_
{
    /* Device Section */
    BVIEW_BST_DEVICE_DATA_t device;

    /* Ingress Section */
    BVIEW_BST_INGRESS_PORT_PG_DATA_t iPortPg;
    BVIEW_BST_INGRESS_PORT_SP_DATA_t iPortSp;
    BVIEW_BST_INGRESS_SP_DATA_t iSp;

    /* Egress Section */
    BVIEW_BST_EGRESS_PORT_SP_DATA_t ePortSp;
    BVIEW_BST_EGRESS_SP_DATA_t eSp;
    BVIEW_BST_EGRESS_UC_QUEUE_DATA_t eUcQ;
    BVIEW_BST_EGRESS_UC_QUEUEGROUPS_DATA_t eUcQg;
    BVIEW_BST_EGRESS_MC_QUEUE_DATA_t eMcQ;
    BVIEW_BST_EGRESS_CPU_QUEUE_DATA_t cpqQ;
    BVIEW_BST_EGRESS_RQE_QUEUE_DATA_t rqeQ;

} BVIEW_BST_ASIC_SNAPSHOT_DATA_t;

/* Statistics collection mode */
typedef enum _bst_collection_mode_
{
    BVIEW_BST_MODE_CURRENT = 1,
    BVIEW_BST_MODE_PEAK
} BVIEW_BST_COLLECTION_MODE;

/* Feature Configuration */
typedef struct _bst_config_
{
    /* enable the feature */
    bool enableStatsMonitoring;

    /* For enabling selectively. When all are disabled, feature is disabled. */
    bool enableDeviceStatsMonitoring;
    bool enableIngressStatsMonitoring;
    bool enableEgressStatsMonitoring;

    /* Statistics collection mode */
    BVIEW_BST_COLLECTION_MODE mode;

} BVIEW_BST_CONFIG_t;

/* Trigger Type */
typedef enum _bst_trigger_type_
{
    BVIEW_BST_TRIGGER_DEVICE = (0x1 << 0),
    BVIEW_BST_TRIGGER_INGRESS = (0x1 << 1),
    BVIEW_BST_TRIGGER_EGRESS = (0x1 << 2)

} BVIEW_BST_TRIGGER_TYPE;

/* Profile configuration  for Egress Port + Service Pools */
typedef struct _bst_ep_sp_threshold_
{
    uint64_t ucShareThreshold;
    uint64_t umShareThreshold;
    uint64_t mcShareThreshold;
    uint64_t mcShareQueueEntriesThreshold;
} BVIEW_BST_EGRESS_PORT_SP_THRESHOLD_t;

/* Profile configuration for the device level buffers*/
typedef struct _bst_device_threshold_
{
    uint64_t threshold;
} BVIEW_BST_DEVICE_THRESHOLD_t;

/* Profile configuration for Ingress Port + Priority Groups */
typedef struct _bst_i_p_pg_threshold_
{
    uint64_t umShareThreshold;
    uint64_t umHeadroomThreshold;
} BVIEW_BST_INGRESS_PORT_PG_THRESHOLD_t;

/* Profile configuration for Ingress Port + Service Pools */
typedef struct _bst_i_p_sp_threshold_
{
    uint64_t umShareThreshold;
} BVIEW_BST_INGRESS_PORT_SP_THRESHOLD_t;

/* Profile configuration for Ingress Service Pools */
typedef struct _bst_i_sp_threshold_
{
    uint64_t umShareThreshold;
} BVIEW_BST_INGRESS_SP_THRESHOLD_t;

/* Profile configuration for Egress Service Pools */
typedef struct _bst_e_sp_threshold_
{
    uint64_t umShareThreshold;
    uint64_t mcShareThreshold;
} BVIEW_BST_EGRESS_SP_THRESHOLD_t;

/* Profile configuration for Egress Unicast Queues */
typedef struct _bst_e_ucq_threshold_
{
    uint64_t ucBufferThreshold;
} BVIEW_BST_EGRESS_UC_QUEUE_THRESHOLD_t;

/* Profile configuration for Egress Unicast Queue Groups */
typedef struct _bst_e_ucqg_threshold_
{
    uint64_t ucBufferThreshold;
} BVIEW_BST_EGRESS_UC_QUEUEGROUPS_THRESHOLD_t;

/* Profile configuration for Egress Multicast Queues */
typedef struct _bst_e_mcq_threshold_
{
    uint64_t mcBufferThreshold;
    uint64_t mcQueueThreshold;
} BVIEW_BST_EGRESS_MC_QUEUE_THRESHOLD_t;

/* Profile configuration for CPU Queues */
typedef struct _bst_cpu_q_threshold_
{
    uint64_t cpuBufferThreshold;
    uint64_t cpuQueueThreshold;
} BVIEW_BST_EGRESS_CPU_QUEUE_THRESHOLD_t;

/* Profile configuration for RQE Queues */
typedef struct _bst_rqe_q_threshold_
{
    uint64_t rqeBufferThreshold;
    uint64_t rqeQueueThreshold;
} BVIEW_BST_EGRESS_RQE_QUEUE_THRESHOLD_t;

/* The callback for invoking when a configured trigger goes off */
typedef BVIEW_STATUS(*BVIEW_BST_TRIGGER_CALLBACK_t) (int asic,
        void *cookie,
        BVIEW_BST_TRIGGER_TYPE type );


/* macros for threshold validation */



/* Check the validity of The BST_Threshold for the Egress CPU queues in units of bytes */
#define BVIEW_BST_E_CPU_THRESHOLD_CHECK(_p)  ((_p)->cpuBufferThreshold <= 0 || \
                                              (_p)->cpuBufferThreshold > (BVIEW_BST_E_CPU_UCMC_THRES_DEFAULT))

/* Check the validy of threshold configuration for Egress CPU Queues Entries*/
#define BVIEW_BST_E_CPU_QUEUE_THRESHOLD_CHECK(_p)  ((_p)->cpuQueueThreshold <= 0 || \
                                                    (_p)->cpuQueueThreshold > (BVIEW_BST_E_CPU_UCMC_THRES_DEFAULT))

/* Check the validy of threshold configuration for Egress Multicast Queues Statistics */
#define BVIEW_BST_E_MC_THRESHOLD_CHECK(_p)  ((_p)->mcBufferThreshold <= 0 || \
                                             (_p)->mcBufferThreshold > BVIEW_BST_MCAST_THRES_DEFAULT)

/* Check the validy of threshold configuration for Egress Multicast Queues Entries*/
#define BVIEW_BST_E_MC_QUEUE_THRESHOLD_CHECK(_p)  ((_p)->mcQueueThreshold <= 0 || \
                                                   (_p)->mcQueueThreshold > (0x1FFFF * 208))

/* Check the validy of configuration for Egress Unicast Queue Groups Statistics */
#define BVIEW_BST_E_UC_GRP_THRESHOLD_CHECK(_p)  ((_p)->ucBufferThreshold <= 0 || \
                                                 (_p)->ucBufferThreshold > BVIEW_BST_UCAST_QUEUE_GROUP_DEFAULT)

/* Check the validy of configuration for Egress Unicast Queue Entries */
#define BVIEW_BST_E_UC_THRESHOLD_CHECK(_p)  ((_p)->ucBufferThreshold <= 0 || \
                                             (_p)->ucBufferThreshold > BVIEW_BST_UCAST_THRES_DEFAULT)

/* Check the validy of threshold configuration for Egress Port Service Pools UC Statistics */
#define BVIEW_BST_EPSP_UC_THRESHOLD_CHECK(_p)  ((_p)->ucShareThreshold <= 0 || \
                                                (_p)->ucShareThreshold > BVIEW_BST_E_P_SP_UC_THRES_DEFAULT)

/* Check the validy of threshold configuration for Egress Service Pools UC+MC Statistics */
#define BVIEW_BST_EPSP_UM_THRESHOLD_CHECK(_p)  ((_p)->umShareThreshold <= 0 || \
                                                (_p)->umShareThreshold > BVIEW_BST_E_P_SP_UCMC_THRES_DEFAULT)

/* Check the validy of threshold configuration for Egress Service Pools MC Statistics */
#define BVIEW_BST_EPSP_MC_THRESHOLD_CHECK(_p)  ((_p)->mcShareThreshold <= 0 || \
                                                (_p)->mcShareThreshold > (0x1FFFF * 208))

/* Check the validy of threshold configuration for Egress Service Pools UC+MC Statistics */
#define BVIEW_BST_EPSP_MC_SQ_THRESHOLD_CHECK(_p)  ((_p)->mcShareQueueEntriesThreshold <= 0 || \
                                                (_p)->mcShareQueueEntriesThreshold > (0x1FFFF * 208))

/* Check the validy of threshold configuration for Ingress Service Pools MC+UC Statistics */
#define BVIEW_BST_IPSP_THRESHOLD_CHECK(_p)  ((_p)->umShareThreshold <= 0 || \
                                             (_p)->umShareThreshold > BVIEW_BST_I_SP_UCMC_SHARED_THRES_DEFAULT)

/* Check the validy of threshold configuration for Ingress Priority Group MC+UC Statistics */
#define BVIEW_BST_IPPG_SHRD_THRESHOLD_CHECK(_p)  ((_p)->umShareThreshold <= 0 || \
                                                  (_p)->umShareThreshold > BVIEW_BST_I_P_SP_UCMC_SHARED_THRES_DEFAULT)

/* Check the validy of threshold configuration for Ingress Priority Group Headroom Statistics */
#define BVIEW_BST_IPPG_HDRM_THRESHOLD_CHECK(_p)  ((_p)->umHeadroomThreshold <= 0 || \
                                                  (_p)->umHeadroomThreshold > BVIEW_BST_I_P_PG_UCMC_HDRM_THRES_DEFAULT)


/* Check the validy of threshold configuration for Device level Statistics*/ 
#define BVIEW_BST_DEVICE_THRESHOLD_CHECK(_p)      ((_p)->threshold <= 0 || \
                                                   (_p)->threshold > BVIEW_BST_DEVICE_THRES_DEFAULT)

/* Check the validy of threshold configuration for Ingress Serivce Pool*/
#define BVIEW_BST_ISP_THRESHOLD_CHECK(_p)         ((_p)->umShareThreshold <=0 || \
                                                   (_p)->umShareThreshold > (0x1FFFF * 208))

/* Check the validy of threshold configuration for Egress Service Pools UC + MCStatistics */
#define BVIEW_BST_E_SP_UM_THRESHOLD_CHECK(_p)  ((_p)->umShareThreshold <= 0 || \
                                               (_p)->umShareThreshold > BVIEW_BST_E_SP_UCMC_THRES_DEFAULT)


/* Check the validy of threshold configuration for Egress Service Pools MC Statistics */
#define BVIEW_BST_E_SP_MC_THRESHOLD_CHECK(_p)  ((_p)->mcShareThreshold <= 0 || \
                                                (_p)->mcShareThreshold > BVIEW_BST_E_SP_MC_THRES_DEFAULT)

/* Check the validy of threshold configuration for Egress Service Pools MC in Queue Entries */
#define BVIEW_BST_E_SP_MC_SQ_THRESHOLD_CHECK(_p)  ((_p)->mcShareQueueEntriesThreshold <= 0 || \
                                                (_p)->mcShareQueueEntriesThreshold > (0x1FFFF * 208))



/* Check the validy of threshold configuration for Egress RQE Queues in units of buffers */
#define BVIEW_BST_E_RQE_THRESHOLD_CHECK(_p)        ((_p)->rqeBufferThreshold <= 0 || \
                                                    (_p)->rqeBufferThreshold > BVIEW_BST_E_RQE_THRES_DEFAULT)

/* Check the validy of threshold configuration for RQE Queues in units of queue Entries*/
#define BVIEW_BST_E_RQE_QUEUE_THRESHOLD_CHECK(_p)  ((_p)->rqeQueueThreshold <= 0 || \
                                                    (_p)->rqeQueueThreshold > (0xFFF * 208))

/* Check the validy of threshold configuration for mc egress Queues  grp in units of queue Entries*/
#define BVIEW_BST_E_MC_QG_THRESHOLD_CHECK(_p)  ((_p)->mcThreshold <= 0 || \
                                                    (_p)->mcThreshold > (0x1FFFF * 208))

/* Check the validy of threshold configuration for mc shared egress Queues  grp in units of queue Entries*/
#define BVIEW_BST_E_MC_SQG_THRESHOLD_CHECK(_p)  ((_p)->mcQueueEntriesThreshold <= 0 || \
                                                    (_p)->mcQueueEntriesThreshold > (0x1FFFF * 208))


/* Check the validy of threshold configuration for Egress CPU Queues Statistics */
#define BVIEW_BST_EGRESS_CPU_THRESHOLD_CHECK(_p)  ((_p)->cpuThreshold <= 0 || \
                                              (_p)->cpuThreshold > (0x1FFFF * 208))


/* Check the validy of threshold configuration for Egress RQE Queues Statistics */
#define BVIEW_BST_EGRESS_RQE_QUEUE_THRESHOLD_CHECK(_p)  ((_p)->rqeThreshold <= 0 || \
                                              (_p)->rqeThreshold > (0x1FFFF * 208))


/* Check the validy of threshold configuration for Egress RQE Queues Statistics */
#define BVIEW_BST_EGRESS_UC_THRESHOLD_CHECK(_p)  ((_p)->ucThreshold <= 0 || \
                                              (_p)->ucThreshold > (0x1FFFF * 208))

/*****************************************************************//**
* @brief : function to creathe the bst application thread.
*
* @param[in]  none 
*
* @retval  : BVIEW_STATUS_RESOURCE_NOT_AVAILABLE : Returns when memory is not available
* @retval  : BVIEW_STATUS_FAILURE : failed to create the message queue or thread. 
* @retval  : BVIEW_STATUS_SUCCESS : successful completion of task creation. 
*
* @note : Main function to invoke the bst thread. This function allocates the required memory 
*         for bst application. Creates the mutexes for the bst data. Creates the message queue
*         It assumes that the system is initialized before the bst thread is invoked. This api
*        depends on the system variables such as number of units per device. Incase of no such 
*        need, the number of units can be #defined.
*
*********************************************************************/
BVIEW_STATUS bst_main ();

/*****************************************************************//**
*  @brief:  function to clean up  bst application  
*
* @param[in]  none 
*
* @retval  : none
*
* @note  When in case of any error event, this api cleans up all the resources.
*         i.e it deletes the timer notes, if any are present.
*         destroy the mutexes. clears the memory and destroys the message queue.
*
*********************************************************************/
void bst_app_uninit();

/*****************************************************************//**
* @brief : bst main application function which does processing of messages
*
* @param[in] : none
*
* @retval  : BVIEW_STATUS_SUCCESS: Fails to register with module manager 
*             or some error happened due to which the bst application is
*             unable to process incoming messages.
*
*********************************************************************/
BVIEW_STATUS bst_app_main(void);

/*****************************************************************//**
* @brief : function to initializes bst structures to default values 
*
* @param[in] : number of units on the agent
*
* @retval  : BVIEW_STATUS_SUCCESS : configuration is successfully initialized.
* @retval  : BVIEW_STATUS_FAILURE : Fail to initialize the BST appliation to default.
*
* @note  : Initialization functon to make sure both software and asic are configured with
*          same default values. We are not setting the default values for threshold, but 
*         we are using the same as that of asic.
*
*********************************************************************/
BVIEW_STATUS bst_app_config_init(unsigned int num_units);

       
#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_BST_H */

/*!  @}
 * @}
 */
