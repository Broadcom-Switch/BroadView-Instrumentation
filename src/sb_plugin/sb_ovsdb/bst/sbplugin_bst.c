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

#include <pthread.h>
#include <stdbool.h>
#include "common/platform_spec.h"
#include "bst.h"
#include "sbfeature_bst.h"
#include "sbplugin_bst.h"
#include "sbplugin_system.h"
#include "sbplugin_bst_map.h"
#include "sbplugin_bst_ovsdb.h"
#include "ovsdb_bst_ctl.h"

/* Macro to iterate all ports*/
#define  BVIEW_BST_PORT_ITER(_asic,_port)                                         \
              for ((_port) = 1; (_port) <= asicDb[(_asic)].scalingParams.numPorts; (_port)++)

/* Macro to iterate all Priority Groups*/
#define  BVIEW_BST_PG_ITER(_asic,_pg)                                                 \
              for ((_pg) = 0; (_pg) < asicDb[(_asic)].scalingParams.numPriorityGroups; (_pg)++)

/* Macro to iterate all Service Pools*/
#define  BVIEW_BST_SP_ITER(_asic,_sp)                                                 \
              for ((_sp) = 0; (_sp) < asicDb[(_asic)].scalingParams.numServicePools; (_sp)++)

/* Macro to iterate all Service Pools + Common Service Pool*/
#define  BVIEW_BST_TOTAL_SP_ITER(_asic,_sp)                                            \
              for ((_sp) = 0; (_sp) < (asicDb[(_asic)].scalingParams.numServicePools + \
                           asicDb[(_asic)].scalingParams.numCommonPools)  ; (_sp)++)

/* Macro to iterate all Unicast queues*/
#define  BVIEW_BST_UC_QUEUE_ITER(_asic, _ucqueue)                                          \
              for ((_ucqueue) = 0; (_ucqueue) < asicDb[(_asic)].scalingParams.numUnicastQueues; \
                   (_ucqueue)++)

/* Macro to iterate all Multicast queues */
#define  BVIEW_BST_MC_QUEUE_ITER(_asic, _mcqueue)                                                 \
              for ((_mcqueue) = 0; (_mcqueue) < asicDb[(_asic)].scalingParams.numMulticastQueues; \
                   (_mcqueue)++)

/* Macro to iterate all CPU queues */
#define  BVIEW_BST_CPU_QUEUE_ITER(_asic, _cpuqueue)                                              \
              for ((_cpuqueue) = 0; (_cpuqueue) < asicDb[(_asic)].scalingParams.numCpuQueues;    \
                   (_cpuqueue)++)

/* Macro to iterate all RQE queues */
#define  BVIEW_BST_RQE_QUEUE_ITER(_asic, _rqe)                                                    \
               for ((_rqe) = 0; (_rqe) < asicDb[(_asic)].scalingParams.numRqeQueues;              \
                   (_rqe)++)

/* Macro to iterate all Queue Groups */
#define  BVIEW_BST_UC_QUEUE_GRP_ITER(_asic, _qgroup)                                                             \
               for ((_qgroup) = 0; (_qgroup) < asicDb[(_asic)].scalingParams.numUnicastQueueGroups;              \
                    (_qgroup)++) 


#define BVIEW_OVSDB_BST_GET_DB_INDEX(_asic, _bid, _index1, _index2, _p_dbindex) \
                          { \
                            if (BVIEW_STATUS_SUCCESS != bst_ovsdb_resolve_index ((_asic), (_bid), \
                                                         (_index1), (_index2), (_p_dbindex))) \
                            { \
                              return BVIEW_STATUS_FAILURE;\
                            } \
                          } 

#define BVIEW_OVSDB_BST_CACHE_GET(_cache) \
              {\
                (_cache) = bst_ovsdb_cache_get ();\
                if (!(_cache)) \
                { \
                  return BVIEW_STATUS_FAILURE; \
                } \
              }

pthread_t ovsdb_client_thread;

/*********************************************************************
* @brief    OVSDB BST client  
*           
*
*
* @notes    open a JSON RPC session and process "update" notification
*           from OVSDB-server
*
*********************************************************************/
void bst_ovsdb_client()
{
  bst_ovsdb_monitor ();
}

/*********************************************************************
* @brief    OVSDB client init and spawn a thread to act as ovsdb client 
*            for BST
*
* @retval   BVIEW_STATUS_SUCCESS if ovsdb client 
*                                initialized successfully.
* @retval   BVIEW_STATUS_FAILURE if initialization is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS bst_ovsdb_client_init()
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
/*  char *ovsdb_soc = OVSDB_SOCKET_FILE;*/


  /* Spawn a thread for ovsdb client */
  if (0 != pthread_create (&ovsdb_client_thread, NULL, (void *) &bst_ovsdb_client, NULL))
  {
    SB_OVSDB_DEBUG_PRINT ("Failed to create ovsdb client thread");
    return BVIEW_STATUS_FAILURE;
  }
  return rv;
}

/*********************************************************************
* @brief    OVSDB client and ovsdb bst data cache init
*
* @retval   BVIEW_STATUS_SUCCESS if ovsdb client and cache are
*                                initialized successfully.
* @retval   BVIEW_STATUS_FAILURE if initialization is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_infra_init()
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

  /* Initialize ovsdb bst data cache */
  if ((rv = bst_ovsdb_cache_init()) != BVIEW_STATUS_SUCCESS)
  {
    SB_OVSDB_DEBUG_PRINT ("Failed to initialize OVSDB BST cache");
    return rv;
  }

  /* Initialize ovsdb bst data cache */
  if ((rv = bst_ovsdb_client_init()) != BVIEW_STATUS_SUCCESS)
  {
    SB_OVSDB_DEBUG_PRINT ("Failed to initialize OVSDB BST cache");
    return rv;
  }


  
  return rv;
}

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
BVIEW_STATUS sbplugin_ovsdb_bst_config_set (int asic, BVIEW_BST_CONFIG_t *data)
{
  BVIEW_STATUS   rv = BVIEW_STATUS_SUCCESS;
  BVIEW_OVSDB_CONFIG_DATA_t     config;
  
  /* Check validity of input data and asic validity check*/
  BVIEW_BST_INPUT_VALID_CHECK (asic, data);

  memset (&config, 0x00, sizeof (config));
  /* enableStatsMonitoring can be either 'true' or 'false'*/
  if (data->enableStatsMonitoring != true &&
      data->enableStatsMonitoring != false) 
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }  
 
  /* Check the validity of tracking mode*/
  if (BVIEW_BST_MODE_CURRENT != data->mode &&
      BVIEW_BST_MODE_PEAK != data->mode)
  {
     return BVIEW_STATUS_INVALID_PARAMETER;
  }
  
  config.bst_enable           = data->enableStatsMonitoring;
  config.bst_tracking_mode    = (int) data->mode; 
  config.periodic_collection  = data->enablePeriodicCollection;
  config.collection_interval  = data->collectionPeriod;

  rv = bst_ovsdb_bst_config_set(asic, &config);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    SB_OVSDB_DEBUG_PRINT (
                "BST:ASIC(%d) Failed to set bst mode",asic);
    return BVIEW_STATUS_FAILURE;
  }
  return BVIEW_STATUS_SUCCESS;
}

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
BVIEW_STATUS sbplugin_ovsdb_bst_config_get (int asic, 
                                            BVIEW_BST_CONFIG_t *data)
{
  BVIEW_STATUS   rv = BVIEW_STATUS_SUCCESS;
  BVIEW_OVSDB_CONFIG_DATA_t     config;

  /* Check validity of input data*/
  if (data == NULL)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
  /*validate ASIC*/
  if (SB_OVSDB_RV_ERROR(SB_OVSDB_API_UNIT_CHECK(asic)))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  rv = bst_ovsdb_bst_config_get(asic,
                               &config);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    SB_OVSDB_DEBUG_PRINT (
                "BST:ASIC(%d) Failed to get BST configuration",asic);
    return BVIEW_STATUS_FAILURE;
  }

  data->enableStatsMonitoring = config.bst_enable;
  data->mode = config.bst_tracking_mode;
  data->enablePeriodicCollection = config.periodic_collection;
  data->collectionPeriod =  config.collection_interval;

 return  BVIEW_STATUS_SUCCESS;
}

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
                                 BVIEW_TIME_t *time)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

  /* Check validity of input data*/
  BVIEW_BST_INPUT_VALIDATE (asic, snapshot, time);

  /* Obtain Device Statistics */ 
  rv = sbplugin_ovsdb_bst_device_data_get (asic, &snapshot->device, time);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Ingress Port + Priority Groups Statistics */
  rv = sbplugin_ovsdb_bst_ippg_data_get (asic, &snapshot->iPortPg, time);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }
  /* Obtain Ingress Port + Service Pools Statistics */
  rv = sbplugin_ovsdb_bst_ipsp_data_get (asic, &snapshot->iPortSp, time);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Ingress Service Pools Statistics */
  rv = sbplugin_ovsdb_bst_isp_data_get (asic, &snapshot->iSp,time);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Egress Port + Service Pools Statistics */
  rv = sbplugin_ovsdb_bst_epsp_data_get (asic, &snapshot->ePortSp, time);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Egress Service Pools Statistics */
  rv = sbplugin_ovsdb_bst_esp_data_get (asic, &snapshot->eSp, time);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }
 
  /* Obtain Egress Egress Unicast Queues Statistics */
  rv = sbplugin_ovsdb_bst_eucq_data_get (asic, &snapshot->eUcQ, time);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Egress Egress Unicast Queue Groups Statistics */
  rv = sbplugin_ovsdb_bst_eucqg_data_get (asic, &snapshot->eUcQg ,time);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Egress Egress Multicast Queues Statistics */
  rv = sbplugin_ovsdb_bst_emcq_data_get (asic, &snapshot->eMcQ, time);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Egress Egress CPU Queues Statistics */
  rv = sbplugin_ovsdb_bst_cpuq_data_get (asic, &snapshot->cpqQ,time);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Egress Egress RQE Queues Statistics */
  rv = sbplugin_ovsdb_bst_rqeq_data_get (asic, &snapshot->rqeQ, time);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }
  return BVIEW_STATUS_SUCCESS;
}
 
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
                                    BVIEW_TIME_t *time)
{
  BVIEW_OVSDB_BST_DATA_t     *p_cache = NULL;

  /* Check validity of input data*/
  BVIEW_BST_INPUT_VALIDATE (asic, data, time); 

  /* Get OVSDB cache*/
  BVIEW_OVSDB_BST_CACHE_GET (p_cache);
  /* Acquire read lock*/
  SB_OVSDB_RWLOCK_RD_LOCK(p_cache->lock);

  /* Update current local time*/
  sbplugin_ovsdb_system_time_get (time);

  /*Get total use-count is expressed in terms of buffers used in the device*/ 
  data->bufferCount = p_cache->cache[asic].device.stat;

  /* Release lock */
  SB_OVSDB_RWLOCK_UNLOCK(p_cache->lock);
  return BVIEW_STATUS_SUCCESS; 
}

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
                              BVIEW_TIME_t *time)
{
  unsigned int  port  =0; 
  unsigned int  pg    =0; 
  BVIEW_OVSDB_BST_DATA_t     *p_cache = NULL;
  int         db_index = 0;

  /* Check validity of input data*/
  BVIEW_BST_INPUT_VALIDATE (asic, data, time);

  /* Update current local time*/
  sbplugin_ovsdb_system_time_get (time);

  /* Get OVSDB cache*/
  BVIEW_OVSDB_BST_CACHE_GET (p_cache);
  /* Acquire read lock*/
  SB_OVSDB_RWLOCK_RD_LOCK(p_cache->lock);

  /* Loop through all the ports*/
  BVIEW_BST_PORT_ITER (asic, port)
  {
    /* Loop through all priority groups*/
    BVIEW_BST_PG_ITER (asic, pg)
    {
      /*BST_Stat for each of the (Ingress Port, PG) UC plus MC 
       * Shared use-counts in units of buffers.
       */
      BVIEW_OVSDB_BST_GET_DB_INDEX(asic, SB_OVSDB_BST_STAT_ID_PRI_GROUP_SHARED, 
                                   port, pg, &db_index);
      data->data[port - 1][pg].umShareBufferCount = 
                     p_cache->cache[asic].iPGShared[db_index].stat;   

      /* BST_Stat for each of the (Ingress Port, PG) UC plus MC 
       * Headroom use-counts in units of buffers.
       */
      BVIEW_OVSDB_BST_GET_DB_INDEX(asic, SB_OVSDB_BST_STAT_ID_PRI_GROUP_HEADROOM, 
                                   port, pg, &db_index);
      data->data[port - 1][pg].umHeadroomBufferCount =
                     p_cache->cache[asic].iPGHeadroom[db_index].stat;
    } /* for (pg = 0; pg < BVI ....*/
  } /* for (port = 0; port < BVIEW......*/
  /* Release lock */
  SB_OVSDB_RWLOCK_UNLOCK(p_cache->lock);

  return BVIEW_STATUS_SUCCESS;
} 
    

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
                                  BVIEW_TIME_t *time)
{
 unsigned int         port =0; 
 unsigned int         sp =0;
 BVIEW_OVSDB_BST_DATA_t     *p_cache = NULL;
 int         db_index = 0;
 
 /* Check validity of input data*/
 BVIEW_BST_INPUT_VALIDATE (asic, data, time);
 /* Update current local time*/
 sbplugin_ovsdb_system_time_get (time);
 /* Get OVSDB cache*/
 BVIEW_OVSDB_BST_CACHE_GET (p_cache);
  /* Acquire read lock*/
 SB_OVSDB_RWLOCK_RD_LOCK(p_cache->lock);

 /* Loop through all the ports*/
 BVIEW_BST_PORT_ITER (asic, port)
 {
   /* BST_Stat for each of the 4 SPs Shared use-counts 
    * associated with this Port in units of buffers.
    */
   BVIEW_BST_SP_ITER (asic,sp)
   {
     BVIEW_OVSDB_BST_GET_DB_INDEX(asic,SB_OVSDB_BST_STAT_ID_PORT_POOL, 
                                  port, sp, &db_index);
     data->data[port - 1][sp].umShareBufferCount =
                           p_cache->cache[asic].iPortSP[db_index].stat;
   }
 }
  
 /* Release lock */
  SB_OVSDB_RWLOCK_UNLOCK(p_cache->lock);

 return BVIEW_STATUS_SUCCESS;
}

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
                                 BVIEW_TIME_t *time)
{
 int                  sp =0;
 int         db_index = 0;
 BVIEW_OVSDB_BST_DATA_t     *p_cache = NULL;

 /* Check validity of input data*/
 BVIEW_BST_INPUT_VALIDATE (asic, data, time);

 /* Update current local time*/
 sbplugin_ovsdb_system_time_get (time);

 /* Get OVSDB cache*/
 BVIEW_OVSDB_BST_CACHE_GET (p_cache);

  /* Acquire read lock*/
 SB_OVSDB_RWLOCK_RD_LOCK(p_cache->lock);
 /* BST_Stat for each of the 5 Ingress SPs Shared use-counts in units of buffers*/
 BVIEW_BST_SP_ITER (asic, sp)
 {
   BVIEW_OVSDB_BST_GET_DB_INDEX (asic,SB_OVSDB_BST_STAT_ID_ING_POOL,
                                 0, sp, &db_index);
   data->data[sp].umShareBufferCount = 
         p_cache->cache[asic].iSP[db_index].stat;
 }
  /* Release lock */
 SB_OVSDB_RWLOCK_UNLOCK(p_cache->lock);
 return BVIEW_STATUS_SUCCESS;
} 
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
                                BVIEW_TIME_t *time)
{
 unsigned int         port  =0;
 unsigned int         sp =0;
 int         db_index = 0;
 BVIEW_OVSDB_BST_DATA_t     *p_cache = NULL;

 /* Check validity of input data*/
 BVIEW_BST_INPUT_VALIDATE (asic, data, time);

 /* Update current local time*/
 sbplugin_ovsdb_system_time_get (time);

 /* Get OVSDB cache*/
 BVIEW_OVSDB_BST_CACHE_GET (p_cache);
  /* Acquire read lock*/
 SB_OVSDB_RWLOCK_RD_LOCK(p_cache->lock);
 /* Loop through all the ports*/
 BVIEW_BST_PORT_ITER (asic, port)
 {
   BVIEW_BST_SP_ITER (asic, sp)
   { 
     /* Obtain Egress Port + Service Pools Statistics - U cast stats*/
     BVIEW_OVSDB_BST_GET_DB_INDEX (asic, SB_OVSDB_BST_STAT_ID_EGR_UCAST_PORT_SHARED,
                                   port, sp, &db_index);
     data->data[port - 1][sp].ucShareBufferCount =
             p_cache->cache[asic].ePortSPucShare[db_index].stat;

     /* Obtain Egress Port + Service Pools Statistics - Ucast+Mcast cast stats*/
     BVIEW_OVSDB_BST_GET_DB_INDEX (asic, SB_OVSDB_BST_STAT_ID_EGR_PORT_SHARED,
                                   port, sp, &db_index)
     data->data[port - 1][sp].umShareBufferCount = 
             p_cache->cache[asic].ePortSPumShare[db_index].stat;
   }
 }
  /* Release lock */
 SB_OVSDB_RWLOCK_UNLOCK(p_cache->lock);
 return BVIEW_STATUS_SUCCESS;
}

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
                               BVIEW_TIME_t *time)
{
 unsigned int         sp =0;
 int         db_index = 0;
 BVIEW_OVSDB_BST_DATA_t     *p_cache = NULL;

 /* Check validity of input data*/
 BVIEW_BST_INPUT_VALIDATE (asic, data, time);

 /* Update current local time*/
 sbplugin_ovsdb_system_time_get (time);

 /* Get OVSDB cache*/
 BVIEW_OVSDB_BST_CACHE_GET (p_cache);
  /* Acquire read lock*/
 SB_OVSDB_RWLOCK_RD_LOCK(p_cache->lock);
 /* Loop through all the ports*/
 BVIEW_BST_SP_ITER (asic, sp)
 {
   /* BST_Stat for each of the 4 Egress SPs Shared use-counts in units of buffers.
    * This use-count includes both UC and MC buffers.
    */
   BVIEW_OVSDB_BST_GET_DB_INDEX (asic, SB_OVSDB_BST_STAT_ID_EGR_POOL,
                                 0, sp, &db_index);
   data->data[sp].umShareBufferCount = 
                    p_cache->cache[asic].eSPumShare[db_index].stat; 
   /*BST_Threshold for each of the 4 Egress SP MC Share use-counts in units of buffers.*/
   BVIEW_OVSDB_BST_GET_DB_INDEX (asic, SB_OVSDB_BST_STAT_ID_EGR_MCAST_POOL,
                                 0, sp, &db_index);
   data->data[sp].mcShareBufferCount = 
                    p_cache->cache[asic].eSPmcShare[db_index].stat;
 }
  /* Release lock */
 SB_OVSDB_RWLOCK_UNLOCK(p_cache->lock);
 return BVIEW_STATUS_SUCCESS;
}

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
                              BVIEW_TIME_t *time)
{
 unsigned int         cosq = 0;
 int         db_index = 0;
 BVIEW_OVSDB_BST_DATA_t     *p_cache = NULL;

  /* Check validity of input data*/
 BVIEW_BST_INPUT_VALIDATE (asic, data, time);

 /* Update current local time*/
 sbplugin_ovsdb_system_time_get (time);

 /* Get OVSDB cache*/
 BVIEW_OVSDB_BST_CACHE_GET (p_cache);
  /* Acquire read lock*/
 SB_OVSDB_RWLOCK_RD_LOCK(p_cache->lock);
 /* Iterate COSQ*/
 BVIEW_BST_UC_QUEUE_ITER (asic, cosq) 
 {
   /*BST_Stat for the UC queue total use-counts in units of buffers.*/
   BVIEW_OVSDB_BST_GET_DB_INDEX (asic, SB_OVSDB_BST_STAT_ID_UCAST,
                                 0, cosq, &db_index);
   data->data[cosq].ucBufferCount = 
                 p_cache->cache[asic].ucQ[db_index].stat;
 }
  /* Release lock */
 SB_OVSDB_RWLOCK_UNLOCK(p_cache->lock);
 return BVIEW_STATUS_SUCCESS;
}
   
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
                        BVIEW_TIME_t *time)
{
 unsigned int         cosq = 0;
 int         db_index = 0;
 BVIEW_OVSDB_BST_DATA_t     *p_cache = NULL;
 /* Check validity of input data*/
 BVIEW_BST_INPUT_VALIDATE (asic, data, time);

 /* Update current local time*/
 sbplugin_ovsdb_system_time_get (time);

 /* Get OVSDB cache*/
 BVIEW_OVSDB_BST_CACHE_GET (p_cache);
  /* Acquire read lock*/
 SB_OVSDB_RWLOCK_RD_LOCK(p_cache->lock);
 /* Loop through all the UC_QUEUE_GROUPS*/
 BVIEW_BST_UC_QUEUE_GRP_ITER (asic, cosq)
 {
   /* BST_Stat for each of the 128 Egress Unicast Queue-Group 
    * Total use-counts in units of buffers.
    */
   BVIEW_OVSDB_BST_GET_DB_INDEX (asic, SB_OVSDB_BST_STAT_ID_UCAST_GROUP,
                                 0, cosq, &db_index);
   data->data[cosq].ucBufferCount = 
            p_cache->cache[asic].eUCqGroup[db_index].stat;
 }
  /* Release lock */
 SB_OVSDB_RWLOCK_UNLOCK(p_cache->lock);
 return BVIEW_STATUS_SUCCESS;
}

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
                              BVIEW_TIME_t *time)
{
 unsigned int         cosq =0;
 int         db_index = 0;
 BVIEW_OVSDB_BST_DATA_t     *p_cache = NULL;

 /* Check validity of input data*/
 BVIEW_BST_INPUT_VALIDATE (asic, data, time);

 /* Update current local time*/
 sbplugin_ovsdb_system_time_get (time);

 /* Get OVSDB cache*/
 BVIEW_OVSDB_BST_CACHE_GET (p_cache);
 /* Acquire read lock*/
 SB_OVSDB_RWLOCK_RD_LOCK(p_cache->lock);
 /* Loop through cos queue max per port*/
 BVIEW_BST_MC_QUEUE_ITER (asic, cosq)
 {
     /*BST_Stat for the MC queue total use-counts in units of buffers.*/
   BVIEW_OVSDB_BST_GET_DB_INDEX (asic, SB_OVSDB_BST_STAT_ID_MCAST,
                                 0, cosq, &db_index);
   data->data[cosq].mcBufferCount = 
          p_cache->cache[asic].mcQ[db_index].stat;
 }
  /* Release lock */
 SB_OVSDB_RWLOCK_UNLOCK(p_cache->lock);
 return BVIEW_STATUS_SUCCESS;
}

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
                             BVIEW_TIME_t *time)
{
 unsigned int         cosq = 0;
 int         db_index = 0;
 BVIEW_OVSDB_BST_DATA_t     *p_cache = NULL;

 /* Check validity of input data*/
 BVIEW_BST_INPUT_VALIDATE (asic, data, time);

 /* Get OVSDB cache*/
 BVIEW_OVSDB_BST_CACHE_GET (p_cache);
 /* Update current local time*/
 sbplugin_ovsdb_system_time_get (time);

  /* Acquire read lock*/
 SB_OVSDB_RWLOCK_RD_LOCK(p_cache->lock);
 /* iterate through Maximum CPU cosqs*/
 BVIEW_BST_CPU_QUEUE_ITER (asic, cosq)
 {
   /*The BST_Threshold for the Egress CPU queues in units of buffers.*/
   BVIEW_OVSDB_BST_GET_DB_INDEX (asic, SB_OVSDB_BST_STAT_ID_CPU_QUEUE,
                                 0, cosq, &db_index);
   data->data[cosq].cpuBufferCount = 
              p_cache->cache[asic].eCPU[db_index].stat;
 }
  /* Release lock */
 SB_OVSDB_RWLOCK_UNLOCK(p_cache->lock);
 return BVIEW_STATUS_SUCCESS;
}

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
                                   BVIEW_TIME_t *time) 
{
 unsigned int         cosq = 0;
 int         db_index = 0;
 BVIEW_OVSDB_BST_DATA_t     *p_cache = NULL;

 /* Check validity of input data*/
 BVIEW_BST_INPUT_VALIDATE (asic, data, time);

 /* Update current local time*/
 sbplugin_ovsdb_system_time_get (time);

 /* Get OVSDB cache*/
 BVIEW_OVSDB_BST_CACHE_GET (p_cache);
  /* Acquire read lock*/
 SB_OVSDB_RWLOCK_RD_LOCK(p_cache->lock);
 /* Loop through all the RQE queues*/
 BVIEW_BST_RQE_QUEUE_ITER (asic, cosq)
 {
   /* BST_Stat for each of the 11 RQE queues total use-counts in units of buffers.*/
   BVIEW_OVSDB_BST_GET_DB_INDEX (asic, SB_OVSDB_BST_STAT_ID_RQE_QUEUE,
                                 0 ,cosq, &db_index);
   data->data[cosq].rqeBufferCount = 
           p_cache->cache[asic].rqe[db_index].stat;
 }
  /* Release lock */
 SB_OVSDB_RWLOCK_UNLOCK(p_cache->lock);
 return BVIEW_STATUS_SUCCESS;
}

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
BVIEW_STATUS sbplugin_ovsdb_bst_device_threshold_set (int asic, 
                                          BVIEW_BST_DEVICE_THRESHOLD_t *thres)
{
  BVIEW_STATUS                rv  = BVIEW_STATUS_SUCCESS;

   /*validate ASIC*/
  SB_OVSDB_VALID_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (thres == NULL || 
      BVIEW_BST_DEVICE_THRESHOLD_CHECK (thres))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 

  rv = bst_ovsdb_threshold_set(asic, 0, 0, SB_OVSDB_BST_STAT_ID_DEVICE, thres->threshold);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    SB_OVSDB_DEBUG_PRINT (
                "BST:ASIC(%d) Device Stat:Failed to set Threshold",asic);
    return BVIEW_STATUS_FAILURE;
  }
  return BVIEW_STATUS_SUCCESS;
}

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
BVIEW_STATUS sbplugin_ovsdb_bst_ippg_threshold_set (int asic, int port, int pg, 
                                     BVIEW_BST_INGRESS_PORT_PG_THRESHOLD_t *thres)
{
  BVIEW_STATUS           rv = BVIEW_STATUS_SUCCESS;

   /*validate ASIC*/
  SB_OVSDB_VALID_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (thres == NULL || 
      BVIEW_BST_IPPG_SHRD_THRESHOLD_CHECK (thres) ||
      BVIEW_BST_IPPG_HDRM_THRESHOLD_CHECK (thres))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 

  /* set threshold for shared buffer stats*/
  rv = bst_ovsdb_threshold_set (asic, port, pg, 
                                SB_OVSDB_BST_STAT_ID_PRI_GROUP_SHARED, thres->umShareThreshold);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    SB_OVSDB_DEBUG_PRINT (
                "BST:ASIC(%d) Ingress Shared (Port, PG) PG(%d) Port(%d):Failed to set Threshold", asic,pg,port);
    return BVIEW_STATUS_FAILURE;
  }
 /* set threshold for headroom buffer stats*/
  rv = bst_ovsdb_threshold_set (asic, port, pg, 
                                SB_OVSDB_BST_STAT_ID_PRI_GROUP_HEADROOM, thres->umHeadroomThreshold);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    SB_OVSDB_DEBUG_PRINT (
                "BST:ASIC(%d) Ingress Headroom (Port, PG) PG(%d) Port(%d):Failed to set Threshold", asic,pg,port);
    return BVIEW_STATUS_FAILURE;
  }

  return BVIEW_STATUS_SUCCESS;
}

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
BVIEW_STATUS sbplugin_ovsdb_bst_ipsp_threshold_set (int asic, int port, int sp, 
                                      BVIEW_BST_INGRESS_PORT_SP_THRESHOLD_t *thres)
{
  BVIEW_STATUS           rv = BVIEW_STATUS_SUCCESS;

   /*validate ASIC*/
  SB_OVSDB_VALID_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (thres == NULL || 
      BVIEW_BST_IPSP_THRESHOLD_CHECK (thres))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
 
  /*Set profile configuration for Ingress Port + Service Pools*/
  rv = bst_ovsdb_threshold_set (asic, port, sp, 
                                SB_OVSDB_BST_STAT_ID_PORT_POOL, thres->umShareThreshold);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    SB_OVSDB_DEBUG_PRINT (
                "BST:ASIC (%d) Ingress(Port, SP) SP(%d) Port(%d):Failed to set Threshold", asic,sp,port);
    return BVIEW_STATUS_FAILURE;
  }
  return BVIEW_STATUS_SUCCESS;
}

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
BVIEW_STATUS sbplugin_ovsdb_bst_isp_threshold_set (int asic, 
                                     int sp, 
                                     BVIEW_BST_INGRESS_SP_THRESHOLD_t *thres)
{
  BVIEW_STATUS           rv = BVIEW_STATUS_SUCCESS;
   /*validate ASIC*/
  SB_OVSDB_VALID_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (thres == NULL || 
      BVIEW_BST_ISP_THRESHOLD_CHECK (thres))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 

 /* The BST_Threshold for the (Ingress Port, SP) UC plus MC shared use-count.*/
  rv = bst_ovsdb_threshold_set (asic, 0, sp, 
                                SB_OVSDB_BST_STAT_ID_ING_POOL, thres->umShareThreshold);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    SB_OVSDB_DEBUG_PRINT (
                "BST:ASIC(%d) Ingress SP(%d):Failed to set Threshold", asic,sp);
    return BVIEW_STATUS_FAILURE;
  }
  return BVIEW_STATUS_SUCCESS;
}

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
BVIEW_STATUS sbplugin_ovsdb_bst_epsp_threshold_set (int asic, 
                                      int port, int sp, 
                                      BVIEW_BST_EGRESS_PORT_SP_THRESHOLD_t *thres)
{
  BVIEW_STATUS           rv = BVIEW_STATUS_SUCCESS;  

   /*validate ASIC*/
  SB_OVSDB_VALID_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (thres == NULL || 
      BVIEW_BST_EPSP_UC_THRESHOLD_CHECK (thres) ||
      BVIEW_BST_EPSP_UM_THRESHOLD_CHECK (thres) || 
      BVIEW_BST_EPSP_MC_THRESHOLD_CHECK (thres))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 

  /* The BST_Threshold for the Egress Per (Port, SP) 
   * UC shared use-count in units of 8 buffers
   */
  rv = bst_ovsdb_threshold_set (asic, port, sp, 
                          SB_OVSDB_BST_STAT_ID_EGR_UCAST_PORT_SHARED, thres->ucShareThreshold);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    SB_OVSDB_DEBUG_PRINT (
                "BST:ASIC(%d) Egress UC(Port,SP) SP(%d) Port(%d):Failed to set Threshold", asic,sp, port);
    return BVIEW_STATUS_FAILURE;
  }

 /* The BST_Threshold for the Egress Per (Port, SP) 
  * MC/UC+MC shared use-count in units of buffers.
  */
  rv = bst_ovsdb_threshold_set (asic, port, sp, 
                               SB_OVSDB_BST_STAT_ID_EGR_PORT_SHARED, thres->umShareThreshold);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    SB_OVSDB_DEBUG_PRINT (
                "BST:ASIC(%d) Egress UC+MC (Port,SP) SP(%d) Port(%d):Failed to set Threshold", asic,sp, port);
    return BVIEW_STATUS_FAILURE;
  }

  return BVIEW_STATUS_SUCCESS;
}

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
BVIEW_STATUS sbplugin_ovsdb_bst_esp_threshold_set (int asic, 
                                     int sp, 
                                     BVIEW_BST_EGRESS_SP_THRESHOLD_t *thres)
{
  BVIEW_STATUS           rv = BVIEW_STATUS_SUCCESS;

   /*validate ASIC*/
  SB_OVSDB_VALID_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (thres == NULL || 
      BVIEW_BST_E_SP_UM_THRESHOLD_CHECK (thres) || 
      BVIEW_BST_E_SP_MC_THRESHOLD_CHECK (thres))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 

  /* BST_Threshold for each of the 4 Egress SPs Shared use-counts in units of buffers.*/
  rv = bst_ovsdb_threshold_set (asic, 0, sp, SB_OVSDB_BST_STAT_ID_EGR_POOL, thres->umShareThreshold);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    SB_OVSDB_DEBUG_PRINT (
                "BST:ASIC(%d) Egress SP (%d):Failed to set Threshold", asic,sp);
    return BVIEW_STATUS_FAILURE;
  }

  /* BST_Threshold for each of the 4 Egress SPs Shared use-counts in units of buffers.*/
  rv = bst_ovsdb_threshold_set (asic, 0, sp, SB_OVSDB_BST_STAT_ID_EGR_MCAST_POOL, thres->mcShareThreshold);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    SB_OVSDB_DEBUG_PRINT (
                "BST:ASIC(%d) Egress SP (%d):Failed to set Threshold", asic,sp);
    return BVIEW_STATUS_FAILURE;
  }

  return BVIEW_STATUS_SUCCESS;
}

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
BVIEW_STATUS sbplugin_ovsdb_bst_eucq_threshold_set (int asic, 
                                      int ucQueue, 
                              BVIEW_BST_EGRESS_UC_QUEUE_THRESHOLD_t *thres)
{
  BVIEW_STATUS           rv = BVIEW_STATUS_SUCCESS;
  int                    port = 0, cosq = 0;

   /*validate ASIC*/
  SB_OVSDB_VALID_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (thres == NULL || 
      BVIEW_BST_E_UC_THRESHOLD_CHECK (thres))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
 
  /*  The BST_Threshold for the Egress UC Queues.*/ 
  rv = bst_ovsdb_threshold_set (asic, port, cosq, 
                                SB_OVSDB_BST_STAT_ID_UCAST, thres->ucBufferThreshold);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    SB_OVSDB_DEBUG_PRINT (
                "BST:ASIC(%d) Ucast Queue (%d):Failed to set Threshold", asic,ucQueue);
    return BVIEW_STATUS_FAILURE;
  }
  
  return BVIEW_STATUS_SUCCESS;
}

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
BVIEW_STATUS sbplugin_ovsdb_bst_eucqg_threshold_set (int asic, 
                                       int ucQueueGrp, 
                                       BVIEW_BST_EGRESS_UC_QUEUEGROUPS_THRESHOLD_t *thres)
{
  BVIEW_STATUS           rv = BVIEW_STATUS_SUCCESS; 

   /*validate ASIC*/
  SB_OVSDB_VALID_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (thres == NULL ||
      BVIEW_BST_E_UC_GRP_THRESHOLD_CHECK (thres))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 

  /*The BST_Threshold for the Egress UC Queue-Group */
  rv = bst_ovsdb_threshold_set (asic, 0, ucQueueGrp, 
                                SB_OVSDB_BST_STAT_ID_UCAST_GROUP, thres->ucBufferThreshold);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    SB_OVSDB_DEBUG_PRINT (
                "BST:ASIC(%d) Ucast Queue Group (%d):Failed to set Threshold", asic,ucQueueGrp);
    return BVIEW_STATUS_FAILURE;
  }
  
  return BVIEW_STATUS_SUCCESS;
}

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
BVIEW_STATUS sbplugin_ovsdb_bst_emcq_threshold_set (int asic, 
                                      int mcQueue, 
                                      BVIEW_BST_EGRESS_MC_QUEUE_THRESHOLD_t *thres)
{
  BVIEW_STATUS           rv = BVIEW_STATUS_SUCCESS; 
  int                    port = 0, cosq = 0;

   /*validate ASIC*/
  SB_OVSDB_VALID_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (thres == NULL || 
      BVIEW_BST_E_MC_THRESHOLD_CHECK (thres) ||
      BVIEW_BST_E_MC_QUEUE_THRESHOLD_CHECK (thres))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 

  rv = bst_ovsdb_threshold_set (asic, port, cosq, 
                                SB_OVSDB_BST_STAT_ID_MCAST, thres->mcBufferThreshold);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    SB_OVSDB_DEBUG_PRINT (
                "BST:ASIC(%d) Mcast Queue (%d):Failed to set Threshold", asic,mcQueue);
    return BVIEW_STATUS_FAILURE;
  }
  return BVIEW_STATUS_SUCCESS;
}

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
BVIEW_STATUS sbplugin_ovsdb_bst_cpuq_threshold_set (int asic, 
                                      int cpuQueue, 
                                      BVIEW_BST_EGRESS_CPU_QUEUE_THRESHOLD_t *thres)
{
  BVIEW_STATUS           rv = BVIEW_STATUS_SUCCESS;
  unsigned int port = 0;

   /*validate ASIC*/
  SB_OVSDB_VALID_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (thres == NULL || 
      BVIEW_BST_E_CPU_THRESHOLD_CHECK (thres))
     /* BVIEW_BST_E_CPU_QUEUE_THRESHOLD_CHECK (thres))*/
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 

  rv = bst_ovsdb_threshold_set (asic, port, cpuQueue, 
                                SB_OVSDB_BST_STAT_ID_CPU_QUEUE, thres->cpuBufferThreshold);
  if (SB_OVSDB_RV_ERROR (rv))
  {
    SB_OVSDB_DEBUG_PRINT (
                "BST:ASIC(%d) CPU Queue (%d):Failed to set Threshold", asic,cpuQueue);
    return BVIEW_STATUS_FAILURE;
  }
  
  return BVIEW_STATUS_SUCCESS;
}

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
BVIEW_STATUS sbplugin_ovsdb_bst_rqeq_threshold_set (int asic, 
                                     int rqeQueue, 
                                     BVIEW_BST_EGRESS_RQE_QUEUE_THRESHOLD_t *thres)
{
  BVIEW_STATUS           rv = BVIEW_STATUS_SUCCESS;

  /*validate ASIC*/
  SB_OVSDB_VALID_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (thres == NULL || 
      BVIEW_BST_E_RQE_THRESHOLD_CHECK (thres)) 
    /*  BVIEW_BST_E_RQE_QUEUE_THRESHOLD_CHECK (thres))*/
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  /* Get Threshold configuration for RQE queues*/ 
  rv = bst_ovsdb_threshold_set (asic, 0, rqeQueue, 
                      SB_OVSDB_BST_STAT_ID_RQE_QUEUE, thres->rqeBufferThreshold);
  if (SB_OVSDB_RV_ERROR (rv))
  {
    SB_OVSDB_DEBUG_PRINT (
                "BST:ASIC(%d) RQE Queue (%d):Failed to set Threshold", asic,rqeQueue);
    return BVIEW_STATUS_FAILURE;
  }
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Clear stats
*           
* @param[in]   asic                                    - unit
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if clear stats is succes.
* @retval BVIEW_STATUS_SUCCESS           if clear stats is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_ovsdb_bst_clear_stats(int asic)
{
  return BVIEW_STATUS_SUCCESS; 
}

/*********************************************************************
* @brief  Restore threshold configuration 
*
* @param   asic                                    - unit
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if restore is succes.
* @retval BVIEW_STATUS_SUCCESS           if restore set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_ovsdb_bst_clear_thresholds  (int asic)
{
  return BVIEW_STATUS_SUCCESS;
}

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
* @notes    callback will be executed in driver thread so post the data 
*           to respective task.
*
*********************************************************************/
BVIEW_STATUS  sbplugin_ovsdb_bst_register_trigger (int asic, 
                                        BVIEW_BST_TRIGGER_CALLBACK_t callback, 
                                        void *cookie)
{
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Get snapshot of all thresholds configured
*
*
* @param  [in]  asic                         - unit
* @param  [out] data            - Threshold snapshot
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
BVIEW_STATUS sbplugin_ovsdb_bst_threshold_get (int asic, 
                              BVIEW_BST_ASIC_SNAPSHOT_DATA_t *data,
                              BVIEW_TIME_t *time)
{
  unsigned int           port = 0;
  unsigned int           index =0;
  BVIEW_STATUS           rv = BVIEW_STATUS_SUCCESS;                   

   /*validate ASIC*/
  SB_OVSDB_VALID_UNIT_CHECK (asic);

  /* Check validity of input data*/
  SB_OVSDB_NULLPTR_CHECK (data, BVIEW_STATUS_INVALID_PARAMETER);

   /* Update current local time*/
  sbplugin_ovsdb_system_time_get (time);

  /* Device wide threshold configuration*/
  rv = bst_ovsdb_threshold_get(asic, 0, 0, 
                               SB_OVSDB_BST_STAT_ID_DEVICE, &data->device.bufferCount);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Get port based thresholds*/ 
  BVIEW_BST_PORT_ITER (asic, port)
  {
    BVIEW_BST_PG_ITER (asic, index)
    { 
      /* Get threshold configuration of The BST_Threshold for the (Ingress Port, PG) 
       * UC plus MC Shared use-count in units of buffers.
       */
      rv = bst_ovsdb_threshold_get (asic, port, index, 
                                    SB_OVSDB_BST_STAT_ID_PRI_GROUP_SHARED, 
                                    &data->iPortPg.data[port - 1][index].umShareBufferCount);
      if (BVIEW_STATUS_SUCCESS != rv)
      {
        return BVIEW_STATUS_FAILURE;
      }
      /* Get threshold for headroom The BST_Threshold for the (Ingress Port, PG) 
       * UC plus MC Headroom use-count in units of buffers
       */
      rv = bst_ovsdb_threshold_get (asic, port, index, 
                                    SB_OVSDB_BST_STAT_ID_PRI_GROUP_HEADROOM, 
                                    &data->iPortPg.data[port - 1][index].umHeadroomBufferCount);
      if (BVIEW_STATUS_SUCCESS != rv)
      {
        return BVIEW_STATUS_FAILURE;
      }
    }
    
    /*Get threshold configuration for The BST_Threshold for the (Ingress Port, SP) 
     * UC plus MC shared use-count in units of buffers.
     */  
    BVIEW_BST_SP_ITER (asic, index)
    {
      rv = bst_ovsdb_threshold_get(asic, port, index, 
                                   SB_OVSDB_BST_STAT_ID_PORT_POOL,
                                   &data->iPortSp.data[port - 1][index].umShareBufferCount);
      if (BVIEW_STATUS_SUCCESS != rv)
      {
        return BVIEW_STATUS_FAILURE;
      }
    }

    BVIEW_BST_SP_ITER (asic, index)
    {
      /* The BST_Threshold for the Egress Per (Port, SP) 
       * UC shared use-count in units of 8 buffers */
      rv = bst_ovsdb_threshold_get (asic, port, index, 
                                    SB_OVSDB_BST_STAT_ID_EGR_UCAST_PORT_SHARED, 
                                    &data->ePortSp.data[port - 1][index].ucShareBufferCount);
      if (BVIEW_STATUS_SUCCESS != rv)
      {
        return BVIEW_STATUS_FAILURE;
      }

      /* Get threshold configuration for egress based port shared buffers*/
      rv = bst_ovsdb_threshold_get (asic, port, index, 
                                    SB_OVSDB_BST_STAT_ID_EGR_PORT_SHARED, 
                                    &data->ePortSp.data[port - 1][index].umShareBufferCount);
      if (BVIEW_STATUS_SUCCESS != rv)
      {
        return BVIEW_STATUS_FAILURE;
      }
    }
  }
 
  /* Get threshold configuration for The BST_Threshold for the Egress MC Queues in units of buffers.*/
  BVIEW_BST_MC_QUEUE_ITER (asic,index)
  {
    rv = bst_ovsdb_threshold_get (asic, port, index,
                                    SB_OVSDB_BST_STAT_ID_MCAST,
                                    &data->eMcQ.data[index].mcBufferCount);
    if (SB_OVSDB_RV_ERROR (rv))
    {
        return BVIEW_STATUS_FAILURE;
    }
    data->eMcQ.data[index].port = 0;
  }
    /* Get threshold configuration for The BST_Threshold for the Egress UC Queues in units of 8 buffers.*/
  BVIEW_BST_UC_QUEUE_ITER (asic,index)
  {
    rv = bst_ovsdb_threshold_get (asic, port, index,
          SB_OVSDB_BST_STAT_ID_UCAST, &data->eUcQ.data[index].ucBufferCount);
    if (BVIEW_STATUS_SUCCESS != rv)
    {
        return BVIEW_STATUS_FAILURE;
    }
    data->eUcQ.data[index].port = 0;
  }
 
  BVIEW_BST_SP_ITER (asic, index)
  {
    /*  BST_Threshold for each of the 4 Egress SPs Shared use-counts in units of buffers.*/
    rv = bst_ovsdb_threshold_get (asic, 0, index, 
            SB_OVSDB_BST_STAT_ID_EGR_POOL, &data->eSp.data[index].umShareBufferCount);
    if (BVIEW_STATUS_SUCCESS != rv)
    {
      return BVIEW_STATUS_FAILURE;
    }

    /*  BST_Threshold for each of the 4 Egress SPs Shared use-counts in units of buffers.*/
    rv = bst_ovsdb_threshold_get (asic, 0, index, 
            SB_OVSDB_BST_STAT_ID_EGR_MCAST_POOL, &data->eSp.data[index].mcShareBufferCount);
    if (BVIEW_STATUS_SUCCESS != rv)
    {
      return BVIEW_STATUS_FAILURE;
    }
  }

  /* Get threshold configuration for The BST_Threshold for the Ingress 
   * SP UC plus MC use-count in units of buffers.
  */
  BVIEW_BST_SP_ITER (asic, index)
  {
    rv = bst_ovsdb_threshold_get (asic, 0, index, 
              SB_OVSDB_BST_STAT_ID_ING_POOL, &data->iSp.data[index].umShareBufferCount);
    if (BVIEW_STATUS_SUCCESS != rv)
    {
      return BVIEW_STATUS_FAILURE;
    }
  }

  /* Get thresholds for The BST_Threshold for the Egress CPU queues in units of buffers*/
  BVIEW_BST_CPU_QUEUE_ITER (asic, index)
  {
    rv = bst_ovsdb_threshold_get (asic, port, index, 
                   SB_OVSDB_BST_STAT_ID_CPU_QUEUE, &data->cpqQ.data[index].cpuBufferCount);
    if (BVIEW_STATUS_SUCCESS != rv)
    {
      return BVIEW_STATUS_FAILURE;
    }
  }

  /* Get Thresholds for BST_Threshold for each of the 11 RQE queues 
   * total use-counts in units of buffers.
  */
  BVIEW_BST_RQE_QUEUE_ITER (asic, index)
  {
    rv = bst_ovsdb_threshold_get (asic, 0, index, SB_OVSDB_BST_STAT_ID_RQE_QUEUE, 
                                  &data->rqeQ.data[index].rqeBufferCount);
    if (SB_OVSDB_RV_ERROR (rv))
    {
      return BVIEW_STATUS_FAILURE;
    }
  } 

  /* Get Thresholds for BST_Threshold for each of the 128 Egress Unicast queue groups*/
  BVIEW_BST_UC_QUEUE_GRP_ITER (asic, index)
  {
    rv = bst_ovsdb_threshold_get (asic, 0, index, 
                                 SB_OVSDB_BST_STAT_ID_UCAST_GROUP, &data->eUcQg.data[index].ucBufferCount);
    if (SB_OVSDB_RV_ERROR (rv))
    {
      return BVIEW_STATUS_FAILURE;
    }
  }

  return BVIEW_STATUS_SUCCESS;
}

#if 0
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
BVIEW_STATUS sbplugin_ovsdb_bst_callback (int asic, SB_OVSDB_SWITCH_EVENT_t event, 
                       int bid, int port, int cosq, void *cookie)
{
  return BVIEW_STATUS_SUCCESS;
}

#endif
/*********************************************************************
* @brief  OVSDB BST feature init
*
* @param[in,out]  ovsdbBstFeat     - BST feature data structure
*
* @retval   BVIEW_STATUS_SUCCESS if BST feature is
*                                initialized successfully.
* @retval   BVIEW_STATUS_FAILURE if initialization is failed.
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_ovsdb_bst_init (BVIEW_SB_BST_FEATURE_t *ovsdbBstFeat)
{
  BVIEW_STATUS      rv  = BVIEW_STATUS_SUCCESS;

  /* NULL Pointer check*/
  SB_OVSDB_NULLPTR_CHECK (ovsdbBstFeat, BVIEW_STATUS_INVALID_PARAMETER);

  /* Initialize BST functions*/
  ovsdbBstFeat->feature.featureId           = BVIEW_FEATURE_BST;
  ovsdbBstFeat->feature.supportedAsicMask   = BVIEW_BST_SUPPORT_MASK;
  ovsdbBstFeat->bst_config_set_cb           = sbplugin_ovsdb_bst_config_set;
  ovsdbBstFeat->bst_config_get_cb           = sbplugin_ovsdb_bst_config_get;
  ovsdbBstFeat->bst_snapshot_get_cb         = sbplugin_ovsdb_bst_snapshot_get;
  ovsdbBstFeat->bst_device_data_get_cb      = sbplugin_ovsdb_bst_device_data_get;
  ovsdbBstFeat->bst_ippg_data_get_cb        = sbplugin_ovsdb_bst_ippg_data_get;
  ovsdbBstFeat->bst_ipsp_data_get_cb        = sbplugin_ovsdb_bst_ipsp_data_get;
  ovsdbBstFeat->bst_isp_data_get_cb         = sbplugin_ovsdb_bst_isp_data_get;
  ovsdbBstFeat->bst_epsp_data_get_cb        = sbplugin_ovsdb_bst_epsp_data_get;
  ovsdbBstFeat->bst_esp_data_get_cb         = sbplugin_ovsdb_bst_esp_data_get;
  ovsdbBstFeat->bst_eucq_data_get_cb        = sbplugin_ovsdb_bst_eucq_data_get;
  ovsdbBstFeat->bst_eucqg_data_get_cb       = sbplugin_ovsdb_bst_eucqg_data_get;
  ovsdbBstFeat->bst_emcq_data_get_cb        = sbplugin_ovsdb_bst_emcq_data_get;
  ovsdbBstFeat->bst_cpuq_data_get_cb        = sbplugin_ovsdb_bst_cpuq_data_get;
  ovsdbBstFeat->bst_rqeq_data_get_cb        = sbplugin_ovsdb_bst_rqeq_data_get;
  ovsdbBstFeat->bst_device_threshold_set_cb = sbplugin_ovsdb_bst_device_threshold_set;
  ovsdbBstFeat->bst_ippg_threshold_set_cb   = sbplugin_ovsdb_bst_ippg_threshold_set;
  ovsdbBstFeat->bst_ipsp_threshold_set_cb   = sbplugin_ovsdb_bst_ipsp_threshold_set;
  ovsdbBstFeat->bst_isp_threshold_set_cb    = sbplugin_ovsdb_bst_isp_threshold_set;
  ovsdbBstFeat->bst_epsp_threshold_set_cb   = sbplugin_ovsdb_bst_epsp_threshold_set;
  ovsdbBstFeat->bst_esp_threshold_set_cb    = sbplugin_ovsdb_bst_esp_threshold_set;
  ovsdbBstFeat->bst_eucq_threshold_set_cb   = sbplugin_ovsdb_bst_eucq_threshold_set;
  ovsdbBstFeat->bst_eucqg_threshold_set_cb  = sbplugin_ovsdb_bst_eucqg_threshold_set;
  ovsdbBstFeat->bst_emcq_threshold_set_cb   = sbplugin_ovsdb_bst_emcq_threshold_set;
  ovsdbBstFeat->bst_cpuq_threshold_set_cb   = sbplugin_ovsdb_bst_cpuq_threshold_set;
  ovsdbBstFeat->bst_rqeq_threshold_set_cb   = sbplugin_ovsdb_bst_rqeq_threshold_set;
  ovsdbBstFeat->bst_threshold_get_cb        = sbplugin_ovsdb_bst_threshold_get;
  ovsdbBstFeat->bst_clear_stats_cb          = sbplugin_ovsdb_bst_clear_stats;
  ovsdbBstFeat->bst_clear_thresholds_cb     = sbplugin_ovsdb_bst_clear_thresholds;
  ovsdbBstFeat->bst_register_trigger_cb     = sbplugin_ovsdb_bst_register_trigger;

  if ((rv = sbplugin_ovsdb_bst_infra_init ())!= BVIEW_STATUS_SUCCESS)
  {
    return rv;
  }
  return BVIEW_STATUS_SUCCESS;
}
