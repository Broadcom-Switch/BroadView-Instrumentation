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

#include <stdbool.h>
#include "bst.h"
#include "sbplugin_common_bst.h"
#include "sbplugin_common_system.h"
#include "sbfeature_bst.h"
#include "sbplugin_common.h"
#include "sbplugin_bst_map.h"
#include "common/platform_spec.h"

/* HW Trigger Callback handle from Applciation*/
static BVIEW_BST_TRIGGER_CALLBACK_t        bst_hw_trigger_cb;
 
/*********************************************************************
* @brief  BCM BST feature init
*
* @param[in,out]  bcmBst     - BST feature data structure
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
BVIEW_STATUS sbplugin_common_bst_init (BVIEW_SB_BST_FEATURE_t *bcmBst)
{
  /* NULL Pointer check*/
  BVIEW_NULLPTR_CHECK (bcmBst);

  /* Initialize BST functions*/
  bcmBst->feature.featureId           = BVIEW_FEATURE_BST;
  bcmBst->feature.supportedAsicMask   = BVIEW_BST_SUPPORT_MASK;
  bcmBst->bst_config_set_cb           = sbplugin_common_bst_config_set;
  bcmBst->bst_config_get_cb           = sbplugin_common_bst_config_get;
  bcmBst->bst_snapshot_get_cb         = sbplugin_common_bst_snapshot_get;
  bcmBst->bst_device_data_get_cb      = sbplugin_common_bst_device_data_get;
  bcmBst->bst_ippg_data_get_cb        = sbplugin_common_bst_ippg_data_get;
  bcmBst->bst_ipsp_data_get_cb        = sbplugin_common_bst_ipsp_data_get;
  bcmBst->bst_isp_data_get_cb         = sbplugin_common_bst_isp_data_get;
  bcmBst->bst_epsp_data_get_cb        = sbplugin_common_bst_epsp_data_get;
  bcmBst->bst_esp_data_get_cb         = sbplugin_common_bst_esp_data_get;
  bcmBst->bst_eucq_data_get_cb        = sbplugin_common_bst_eucq_data_get;
  bcmBst->bst_eucqg_data_get_cb       = sbplugin_common_bst_eucqg_data_get;
  bcmBst->bst_emcq_data_get_cb        = sbplugin_common_bst_emcq_data_get;
  bcmBst->bst_cpuq_data_get_cb        = sbplugin_common_bst_cpuq_data_get;
  bcmBst->bst_rqeq_data_get_cb        = sbplugin_common_bst_rqeq_data_get;
  bcmBst->bst_device_threshold_set_cb = sbplugin_common_bst_device_threshold_set;
  bcmBst->bst_ippg_threshold_set_cb   = sbplugin_common_bst_ippg_threshold_set;
  bcmBst->bst_ipsp_threshold_set_cb   = sbplugin_common_bst_ipsp_threshold_set;
  bcmBst->bst_isp_threshold_set_cb    = sbplugin_common_bst_isp_threshold_set;
  bcmBst->bst_epsp_threshold_set_cb   = sbplugin_common_bst_epsp_threshold_set;
  bcmBst->bst_esp_threshold_set_cb    = sbplugin_common_bst_esp_threshold_set;
  bcmBst->bst_eucq_threshold_set_cb   = sbplugin_common_bst_eucq_threshold_set;
  bcmBst->bst_eucqg_threshold_set_cb  = sbplugin_common_bst_eucqg_threshold_set;
  bcmBst->bst_emcq_threshold_set_cb   = sbplugin_common_bst_emcq_threshold_set;
  bcmBst->bst_cpuq_threshold_set_cb   = sbplugin_common_bst_cpuq_threshold_set;
  bcmBst->bst_rqeq_threshold_set_cb   = sbplugin_common_bst_rqeq_threshold_set;
  bcmBst->bst_threshold_get_cb        = sbplugin_common_bst_threshold_get;
  bcmBst->bst_clear_stats_cb          = sbplugin_common_bst_clear_stats;
  bcmBst->bst_clear_thresholds_cb     = sbplugin_common_bst_clear_thresholds;
  bcmBst->bst_register_trigger_cb     = sbplugin_common_bst_register_trigger;
  bcmBst->bst_port_ucast_cgs_drop_get_cb = sbplugin_common_bst_port_ucast_cgs_drop_get;
  bcmBst->bst_port_mcast_cgs_drop_get_cb = sbplugin_common_bst_port_mcast_cgs_drop_get;
  bcmBst->bst_port_total_cgs_drop_get_cb = sbplugin_common_bst_port_total_cgs_drop_get;

  return BVIEW_STATUS_SUCCESS;
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
BVIEW_STATUS sbplugin_common_bst_config_set (int asic, BVIEW_BST_CONFIG_t *data)
{
 int          currentTrackMode = 0;
 int          currentBstMode   = 0;
 unsigned int trackMode        = 0;
 unsigned int rv =0;

 /* Check validity of input data and asic validity check*/
 BVIEW_BST_INPUT_VALID_CHECK (asic, data);

 /* enableStatsMonitoring can be either 'true' or 'false'*/
 if (data->enableStatsMonitoring != true &&
     data->enableStatsMonitoring != false) 
 {
   return BVIEW_STATUS_INVALID_PARAMETER;
 }  
 
 /* Check the validity of tracking mode*/
 if (BVIEW_BST_MODE_CURRENT == data->mode)
 {
   trackMode = false;
 }
 else if (BVIEW_BST_MODE_PEAK == data->mode)
 {
   trackMode = true;
 }
 else 
 {
   return BVIEW_STATUS_INVALID_PARAMETER;
 }
 
 /* set mode only if it is different from cureent mode*/
 rv = SB_BRCM_API_SWITCH_CONTROL_GET(asic, SB_BRCM_SWITCH_BST_ENABLE, &currentBstMode);
 if (rv != SB_BRCM_E_NONE)
 {
   return BVIEW_STATUS_FAILURE;
 }
/* If trigger is generated for any realm in the ingress group, then bst is disabled only
   for that group, i.e for device and egress group the bst is still enabled. Hence in this case
   the bst returns enabled. Hence want to set the bstEnable irresptective of the underlying state
   Hence the below check is commented for now */

/* if (currentBstMode != data->enableStatsMonitoring) */
 {
   rv = SB_BRCM_API_SWITCH_CONTROL_SET(asic, SB_BRCM_SWITCH_BST_ENABLE, data->enableStatsMonitoring);

   if (rv != SB_BRCM_E_NONE)
   {
     return BVIEW_STATUS_FAILURE;
   }
 }

 /* set tracking mode only if it is different from current tracking mode*/
 rv = SB_BRCM_API_SWITCH_CONTROL_GET(asic, SB_BRCM_SWITCH_BST_TRACKING_MODE, &currentTrackMode);
 if (rv != SB_BRCM_E_NONE)
 {
   return BVIEW_STATUS_FAILURE;
 }

 if (currentTrackMode != trackMode)
 {
   rv = SB_BRCM_API_SWITCH_CONTROL_SET(asic, SB_BRCM_SWITCH_BST_TRACKING_MODE, trackMode);
   if (rv != SB_BRCM_E_NONE)
   {
     return BVIEW_STATUS_FAILURE;
   }
 }
 return  BVIEW_STATUS_SUCCESS;
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
BVIEW_STATUS sbplugin_common_bst_config_get (int asic, BVIEW_BST_CONFIG_t *data)
{
 unsigned int rv  =0;
 int temp = 0;

  /* Check validity of input data*/
 if (data == NULL)
 {
   return BVIEW_STATUS_INVALID_PARAMETER;
 }
  /*validate ASIC*/
 if (SB_BRCM_RV_ERROR(SB_BRCM_API_UNIT_CHECK(asic)))
 {
   return BVIEW_STATUS_INVALID_PARAMETER;
 }
 /* Get BST mode*/
 rv = SB_BRCM_API_SWITCH_CONTROL_GET(asic, SB_BRCM_SWITCH_BST_ENABLE, &temp);
 if (rv != SB_BRCM_E_NONE)
 {
   return BVIEW_STATUS_FAILURE;
 }
 /* Get BST tracking Mode*/
 data->enableStatsMonitoring = temp;
 rv = SB_BRCM_API_SWITCH_CONTROL_GET(asic, SB_BRCM_SWITCH_BST_TRACKING_MODE, &temp);
 if (rv != SB_BRCM_E_NONE)
 {
   return BVIEW_STATUS_FAILURE;
 }
 /* Convert HW trackMode value to APP trackMode*/
 data->mode = ((temp == true) ? BVIEW_BST_MODE_PEAK : BVIEW_BST_MODE_CURRENT);
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
BVIEW_STATUS sbplugin_common_bst_snapshot_get (int asic, 
                                 BVIEW_BST_ASIC_SNAPSHOT_DATA_t *snapshot, 
                                 BVIEW_TIME_t *time)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;


  /* Check validity of input data*/
  BVIEW_BST_INPUT_VALIDATE (asic, snapshot, time);

  /* Obtain Device Statistics */ 
  rv = sbplugin_common_bst_device_data_get (asic, &snapshot->device, time);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Ingress Port + Priority Groups Statistics */
  rv = sbplugin_common_bst_ippg_data_get (asic, &snapshot->iPortPg, time);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }
  /* Obtain Ingress Port + Service Pools Statistics */
  rv = sbplugin_common_bst_ipsp_data_get (asic, &snapshot->iPortSp, time);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Ingress Service Pools Statistics */
  rv = sbplugin_common_bst_isp_data_get (asic, &snapshot->iSp,time);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Egress Port + Service Pools Statistics */
  rv = sbplugin_common_bst_epsp_data_get (asic, &snapshot->ePortSp, time);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Egress Service Pools Statistics */
  rv = sbplugin_common_bst_esp_data_get (asic, &snapshot->eSp, time);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }
 
  /* Obtain Egress Egress Unicast Queues Statistics */
  rv = sbplugin_common_bst_eucq_data_get (asic, &snapshot->eUcQ, time);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Egress Egress Unicast Queue Groups Statistics */
  rv = sbplugin_common_bst_eucqg_data_get (asic, &snapshot->eUcQg ,time);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Egress Egress Multicast Queues Statistics */
  rv = sbplugin_common_bst_emcq_data_get (asic, &snapshot->eMcQ, time);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Egress Egress CPU Queues Statistics */
  rv = sbplugin_common_bst_cpuq_data_get (asic, &snapshot->cpqQ,time);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Egress Egress RQE Queues Statistics */
  rv = sbplugin_common_bst_rqeq_data_get (asic, &snapshot->rqeQ, time);
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
BVIEW_STATUS sbplugin_common_bst_device_data_get (int asic, 
                                    BVIEW_BST_DEVICE_DATA_t *data, 
                                    BVIEW_TIME_t *time)
{
 int b_rv         = SB_BRCM_E_NONE;
 BVIEW_STATUS rv  = BVIEW_STATUS_SUCCESS;  

 /* Check validity of input data*/
 BVIEW_BST_INPUT_VALIDATE (asic, data, time); 

 /*call sync function to copy HW stats*/
 BVIEW_BST_STAT_SYNC (asic, SB_BRCM_BST_STAT_ID_DEVICE);

 /* Update current local time*/
 sbplugin_common_system_time_get (time);

 /*Get total use-count is expressed in terms of buffers used in the device*/ 
 b_rv = SB_BRCM_API_COSQ_BST_STAT_GET(asic, 0, 0, SB_BRCM_BST_STAT_ID_DEVICE, 0, &data->bufferCount);
 if (b_rv != SB_BRCM_E_NONE)
 {
   SB_LOG (BVIEW_LOG_ERROR,"Failed to get Device level Use-count");
   return BVIEW_STATUS_FAILURE;
 }
 return rv; 
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
BVIEW_STATUS sbplugin_common_bst_ippg_data_get (int asic, 
                              BVIEW_BST_INGRESS_PORT_PG_DATA_t *data, 
                              BVIEW_TIME_t *time)
{
  SB_BRCM_GPORT_t   gport =0;
  int           b_rv  = SB_BRCM_E_NONE;
  unsigned int  port  =0; 
  unsigned int  pg    =0; 
  BVIEW_STATUS  rv    = BVIEW_STATUS_SUCCESS;

  /* Check validity of input data*/
  BVIEW_BST_INPUT_VALIDATE (asic, data, time);

  /*call sync to copy HW stats*/
  BVIEW_BST_STAT_SYNC (asic, SB_BRCM_BST_STAT_ID_PRI_GROUP_SHARED);

  /*call sync to copy HW stats*/
  BVIEW_BST_STAT_SYNC (asic, SB_BRCM_BST_STAT_ID_PRI_GROUP_HEADROOM); 

  /* Update current local time*/
  sbplugin_common_system_time_get (time);

  /* Loop through all the ports*/
  BVIEW_BST_PORT_ITER (asic, port)
  {
   if (!SB_BRCM_IS_XE_PORT (asic, port))
     continue;

    b_rv = SB_BRCM_API_PORT_GPORT_GET(asic, port, &gport);
    if (b_rv != SB_BRCM_E_NONE)
    {
      return BVIEW_STATUS_FAILURE;
    }
     
    /* Loop through all priority groups*/
    BVIEW_BST_PG_ITER (pg)
    {
      /*BST_Stat for each of the (Ingress Port, PG) UC plus MC 
       * Shared use-counts in units of buffers.
       */
      b_rv = SB_BRCM_API_COSQ_BST_STAT_GET (asic, gport, pg, SB_BRCM_BST_STAT_ID_PRI_GROUP_SHARED, 
                         0, &(data->data[port - 1][pg].umShareBufferCount));
      if (b_rv != SB_BRCM_E_NONE)
      {
         SB_LOG (BVIEW_LOG_ERROR,
                           "Failed to get Ingress (Port,PG) Shared buffer \
                            Use-Count for port(%d) PG(%d)",
                            port,pg);
         return BVIEW_STATUS_FAILURE;
      }

      /* BST_Stat for each of the (Ingress Port, PG) UC plus MC 
       * Headroom use-counts in units of buffers.
       */
      b_rv = SB_BRCM_API_COSQ_BST_STAT_GET (asic, gport,pg, SB_BRCM_BST_STAT_ID_PRI_GROUP_HEADROOM, 
                         0, &(data->data[port - 1][pg].umHeadroomBufferCount));
      if (b_rv != SB_BRCM_E_NONE)
      {
         SB_LOG (BVIEW_LOG_ERROR,
                           "Failed to get Ingress (Port,PG) Headroom \
                            buffer Use-Count for port(%d) PG(%d)", 
                            port,pg);
         return BVIEW_STATUS_FAILURE;
      }
    } /* for (pg = 0; pg < BVI ....*/
  } /* for (port = 0; port < BVIEW......*/
  return rv;
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
BVIEW_STATUS sbplugin_common_bst_ipsp_data_get (int asic, 
                                  BVIEW_BST_INGRESS_PORT_SP_DATA_t *data, 
                                  BVIEW_TIME_t *time)
{
 SB_BRCM_GPORT_t gport =0;
 int         rv    =0; 
 unsigned int port =0; 
 unsigned int sp =0;
 
 /* Check validity of input data*/
 BVIEW_BST_INPUT_VALIDATE (asic, data, time);

 /*call sync to copy HW stats*/
 BVIEW_BST_STAT_SYNC (asic, SB_BRCM_BST_STAT_ID_PORT_POOL);
 
 /* Update current local time*/
 sbplugin_common_system_time_get (time);

 /* Loop through all the ports*/
 BVIEW_BST_PORT_ITER (asic, port)
 {
   if (!SB_BRCM_IS_XE_PORT (asic, port))
     continue;

   rv = SB_BRCM_API_PORT_GPORT_GET (asic, port, &gport);
   if (SB_BRCM_RV_ERROR(rv))
   {
     return BVIEW_STATUS_FAILURE;
   }
   /* BST_Stat for each of the 4 SPs Shared use-counts 
    * associated with this Port in units of buffers.
    */
   BVIEW_BST_SP_ITER (sp)
   { 
     rv = SB_BRCM_API_COSQ_BST_STAT_GET (asic, gport,sp, 
                                 SB_BRCM_BST_STAT_ID_PORT_POOL,
                                 0,
                                 &data->data[port - 1][sp].umShareBufferCount);
     if (SB_BRCM_RV_ERROR(rv))
     {
       SB_LOG (BVIEW_LOG_ERROR,"Failed to get Ingress (Port,SP) \
                           buffer Use-Count for port(%d) SP(%d)", port,sp);
       return BVIEW_STATUS_FAILURE;
     }
   }
 }
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
BVIEW_STATUS sbplugin_common_bst_isp_data_get (int asic, 
                                 BVIEW_BST_INGRESS_SP_DATA_t *data, 
                                 BVIEW_TIME_t *time)
{
 int     rv = 0, sp =0;

 /* Check validity of input data*/
 BVIEW_BST_INPUT_VALIDATE (asic, data, time);

 /*call sync to copy HW stats*/
 BVIEW_BST_STAT_SYNC (asic, SB_BRCM_BST_STAT_ID_ING_POOL);

 /* Update current local time*/
 sbplugin_common_system_time_get (time);

 /* BST_Stat for each of the 5 Ingress SPs Shared use-counts in units of buffers*/
 BVIEW_BST_SP_ITER (sp)
 {
   rv = SB_BRCM_API_COSQ_BST_STAT_GET (asic, 0 ,sp,SB_BRCM_BST_STAT_ID_ING_POOL,
                0,&data->data[sp].umShareBufferCount);
   if (SB_BRCM_RV_ERROR(rv))
   {
     SB_LOG (BVIEW_LOG_ERROR,"Failed to get Ingress (SP) buffer Use-Count \
                           for SP(%d)", sp);
     return BVIEW_STATUS_FAILURE;
   } 
 }
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
BVIEW_STATUS sbplugin_common_bst_epsp_data_get (int asic, 
                                BVIEW_BST_EGRESS_PORT_SP_DATA_t *data, 
                                BVIEW_TIME_t *time)
{
 SB_BRCM_GPORT_t  gport =0;
 int          rv    =0; 
 unsigned int port  =0;
 unsigned int sp =0;

 
 /* Check validity of input data*/
 BVIEW_BST_INPUT_VALIDATE (asic, data, time);

 /*call sync to copy HW stats*/
 BVIEW_BST_STAT_SYNC (asic, SB_BRCM_BST_STAT_ID_EGR_PORT_SHARED);

 /*call sync to copy HW stats*/
 BVIEW_BST_STAT_SYNC (asic, SB_BRCM_BST_STAT_ID_EGR_UCAST_PORT_SHARED);

 /* Update current local time*/
 sbplugin_common_system_time_get (time);

 /* Loop through all the ports*/
 BVIEW_BST_PORT_ITER (asic, port)
 {
   if (!SB_BRCM_IS_XE_PORT (asic, port))
     continue;

   rv = SB_BRCM_API_PORT_GPORT_GET (asic, port, &gport);
   if (SB_BRCM_RV_ERROR(rv))
   {
     return BVIEW_STATUS_FAILURE;
   }
   BVIEW_BST_SP_ITER (sp)
   {
     /* Obtain Egress Port + Service Pools Statistics - U cast stats*/
     rv = SB_BRCM_API_COSQ_BST_STAT_GET (asic, gport,sp, 
                                 SB_BRCM_BST_STAT_ID_EGR_UCAST_PORT_SHARED,
                                 0,
                                 &data->data[port - 1][sp].ucShareBufferCount);
     if (SB_BRCM_RV_ERROR(rv))
     {
       SB_LOG (BVIEW_LOG_ERROR,"Failed to get Egress Ucast (Port,SP) buffer \
                           Use-Count for Port (%d) SP(%d)", port,sp);

       return BVIEW_STATUS_FAILURE;
     }
     /* Obtain Egress Port + Service Pools Statistics - Ucast+Mcast cast stats*/
     rv = SB_BRCM_API_COSQ_BST_STAT_GET (asic, gport,sp, 
                                 SB_BRCM_BST_STAT_ID_EGR_PORT_SHARED,
                                 0,
                                 &data->data[port - 1][sp].umShareBufferCount);
     if (SB_BRCM_RV_ERROR(rv))
     {
       SB_LOG (BVIEW_LOG_ERROR,"Failed to get Egress Ucast + Mcast (Port,SP) \
                           buffer Use-Count for Port (%d) SP(%d)", port,sp);
       return BVIEW_STATUS_FAILURE;
     }
   }
 }
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
BVIEW_STATUS sbplugin_common_bst_esp_data_get  (int asic, 
                               BVIEW_BST_EGRESS_SP_DATA_t *data, 
                               BVIEW_TIME_t *time)
{
 int           rv = 0; 
 unsigned int  sp =0;

 /* Check validity of input data*/
 BVIEW_BST_INPUT_VALIDATE (asic, data, time);

 /*call sync to copy HW stats*/
 BVIEW_BST_STAT_SYNC (asic, SB_BRCM_BST_STAT_ID_EGR_POOL);

  /*call sync to copy HW stats*/
 BVIEW_BST_STAT_SYNC (asic, SB_BRCM_BST_STAT_ID_EGR_MCAST_POOL);

 /* Update current local time*/
 sbplugin_common_system_time_get (time);

  /* Loop through all the ports*/
 BVIEW_BST_SP_ITER (sp)
 {
   /* BST_Stat for each of the 4 Egress SPs Shared use-counts in units of buffers.
    * This use-count includes both UC and MC buffers.
    */

   rv = SB_BRCM_API_COSQ_BST_STAT_GET (asic, 0 ,sp, SB_BRCM_BST_STAT_ID_EGR_POOL,
          0,&data->data[sp].umShareBufferCount);
   if (SB_BRCM_RV_ERROR(rv))
   {
     SB_LOG (BVIEW_LOG_ERROR,"Failed to get Egress Ucast+Mcast (SP) buffer \
                           Use-Count for SP(%d)", sp);

     return BVIEW_STATUS_FAILURE;
   }
   /*BST_Threshold for each of the 4 Egress SP MC Shared use-counts in units of buffers.*/
   rv = SB_BRCM_API_COSQ_BST_STAT_GET (asic, 0 ,sp, SB_BRCM_BST_STAT_ID_EGR_MCAST_POOL,
         0,&data->data[sp].mcShareBufferCount);
   if (SB_BRCM_RV_ERROR(rv))
   {
     SB_LOG (BVIEW_LOG_ERROR,"Failed to get Egress Ucast (SP) buffer \
                           Use-Count for SP(%d)", sp);

     return BVIEW_STATUS_FAILURE;
   }
 }
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
BVIEW_STATUS sbplugin_common_bst_eucq_data_get (int asic, 
                              BVIEW_BST_EGRESS_UC_QUEUE_DATA_t *data, 
                              BVIEW_TIME_t *time)
{
 int          rv   = 0; 
 unsigned int cosq = 0;
 SB_BRCM_GPORT_t  gport =0;
 unsigned int port  =0;

  /* Check validity of input data*/
 BVIEW_BST_INPUT_VALIDATE (asic, data, time);

 /*call sync to copy HW stats*/
 BVIEW_BST_STAT_SYNC (asic, SB_BRCM_BST_STAT_ID_UCAST);

 /* Update current local time*/
 sbplugin_common_system_time_get (time);

 BVIEW_BST_PORT_ITER (asic, port)
 {
   if (!SB_BRCM_IS_XE_PORT (asic, port))
     continue;

   rv = SB_BRCM_API_PORT_GPORT_GET(asic, port , &gport);
   if (SB_BRCM_RV_ERROR(rv))
   {
     return BVIEW_STATUS_FAILURE;
   }

   /* Iterate COSQ*/
   BVIEW_BST_ITER (cosq,BVIEW_BST_NUM_COS_PORT) 
   {
     /*BST_Stat for the UC queue total use-counts in units of buffers.*/
     rv = SB_BRCM_API_COSQ_BST_STAT_GET (asic, gport, cosq, SB_BRCM_BST_STAT_ID_UCAST,
              0,&data->data[((port - 1) * BVIEW_BST_NUM_COS_PORT) + cosq].ucBufferCount);
     data->data[((port -1) * BVIEW_BST_NUM_COS_PORT) + cosq].port = port;
     if (SB_BRCM_RV_ERROR(rv))
     {
       SB_LOG (BVIEW_LOG_ERROR,
                 "BST:Unit(%d) Ucast queue(%d) Port(%d):\
                  Failed to get Buffer Use-Count.", asic, cosq, port);
       return BVIEW_STATUS_FAILURE;
     }
   }
 }
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
BVIEW_STATUS sbplugin_common_bst_eucqg_data_get (int asic, 
                        BVIEW_BST_EGRESS_UC_QUEUEGROUPS_DATA_t *data, 
                        BVIEW_TIME_t *time)
{
 int          rv   = 0; 
 unsigned int cosq = 0;

 /* Check validity of input data*/
 BVIEW_BST_INPUT_VALIDATE (asic, data, time);

 /*call sync to copy HW stats*/
 BVIEW_BST_STAT_SYNC (asic, SB_BRCM_BST_STAT_ID_UCAST_GROUP);

 /* Update current local time*/
 sbplugin_common_system_time_get (time);

 /* Loop through all the UC_QUEUE_GROUPS*/
 BVIEW_BST_ITER (cosq, BVIEW_ASIC_MAX_UC_QUEUE_GROUPS)
 {
   /* BST_Stat for each of the 128 Egress Unicast Queue-Group 
    * Total use-counts in units of buffers.
    */
   rv = SB_BRCM_API_COSQ_BST_STAT_GET(asic, 0 , cosq, SB_BRCM_BST_STAT_ID_UCAST_GROUP,
             0, &data->data[cosq].ucBufferCount);
   if (SB_BRCM_RV_ERROR(rv))
   {
     SB_LOG (BVIEW_LOG_ERROR,
                 "BST:Unit(%d) Ucast queue Group(%d):\
                  Failed to get Buffer Use-Count.", asic, cosq);
     return BVIEW_STATUS_FAILURE;
   }
 }
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
BVIEW_STATUS sbplugin_common_bst_emcq_data_get (int asic, 
                              BVIEW_BST_EGRESS_MC_QUEUE_DATA_t *data, 
                              BVIEW_TIME_t *time)
{
 int           rv = 0;
 unsigned int  cosq =0;
 SB_BRCM_GPORT_t  gport =0;
 unsigned int port  =0;

 /* Check validity of input data*/
 BVIEW_BST_INPUT_VALIDATE (asic, data, time);

 /*call sync to copy HW stats*/
 BVIEW_BST_STAT_SYNC (asic, SB_BRCM_BST_STAT_ID_MCAST);

 /* Update current local time*/
 sbplugin_common_system_time_get (time);

 /* Loop through all the ports*/
 BVIEW_BST_PORT_ITER (asic, port)
 {
   if (!SB_BRCM_IS_XE_PORT (asic, port))
     continue;

   rv = SB_BRCM_API_PORT_GPORT_GET(asic, port, &gport);
   if (SB_BRCM_RV_ERROR(rv))
   {
     return BVIEW_STATUS_FAILURE;
   }
   /* Loop through cos queue max per port*/
   BVIEW_BST_ITER (cosq, BVIEW_BST_NUM_COS_PORT)
   {
     /*BST_Stat for the MC queue total use-counts in units of buffers.*/
     rv = SB_BRCM_API_COSQ_BST_STAT_GET (asic, gport, cosq, SB_BRCM_BST_STAT_ID_MCAST,
          0, &data->data[((port -1) * BVIEW_BST_NUM_COS_PORT) + cosq].mcBufferCount);

     data->data[((port-1) * BVIEW_BST_NUM_COS_PORT) + cosq].port = port;
     if (SB_BRCM_RV_ERROR(rv))
     {
       SB_LOG (BVIEW_LOG_ERROR,
                 "BST:Unit(%d) Mcast queue(%d) Port(%d):\
                  Failed to get Buffer Use-Count.", asic, cosq, port);

       return BVIEW_STATUS_FAILURE;
     }
   }
 }
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
BVIEW_STATUS sbplugin_common_bst_cpuq_data_get (int asic, 
                             BVIEW_BST_EGRESS_CPU_QUEUE_DATA_t *data, 
                             BVIEW_TIME_t *time)
{
 int          rv   = 0; 
 unsigned int cosq = 0;
 unsigned int port = 0;
 SB_BRCM_GPORT_t gport = 0;

 /* Check validity of input data*/
 BVIEW_BST_INPUT_VALIDATE (asic, data, time);

 /*call sync to copy HW stats*/
 BVIEW_BST_STAT_SYNC (asic, SB_BRCM_BST_STAT_ID_CPU);

 /*Get the CPU port*/
 SB_BRCM_CPU_PORT_GET(asic, &port);

 rv = SB_BRCM_API_PORT_GPORT_GET(asic, port, &gport);
 if (SB_BRCM_RV_ERROR(rv))
 {
   return BVIEW_STATUS_FAILURE;
 }

 /* Update current local time*/
 sbplugin_common_system_time_get (time);

 /* iterate through Maximum CPU cosqs*/
 BVIEW_BST_ITER (cosq, BVIEW_ASIC_MAX_CPU_QUEUES)
 {
   /*The BST_Threshold for the Egress CPU queues in units of buffers.*/
   rv = SB_BRCM_API_COSQ_BST_STAT_GET (asic, gport, cosq, SB_BRCM_BST_STAT_ID_CPU,
                    0, &data->data[cosq].cpuBufferCount);

   if (SB_BRCM_RV_ERROR(rv))
   {
     SB_LOG (BVIEW_LOG_ERROR,
                 "BST:ASIC(%d) CPU queue (%d):Failed to get Buffer Use-Count.", asic, cosq);
     return BVIEW_STATUS_FAILURE;
   }
 }
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
BVIEW_STATUS sbplugin_common_bst_rqeq_data_get (int asic, 
                                   BVIEW_BST_EGRESS_RQE_QUEUE_DATA_t *data, 
                                   BVIEW_TIME_t *time) 
{
 int          rv   = 0; 
 unsigned int cosq = 0;

 /* Check validity of input data*/
 BVIEW_BST_INPUT_VALIDATE (asic, data, time);

 /*call sync to copy HW stats*/
 BVIEW_BST_STAT_SYNC (asic, SB_BRCM_BST_STAT_ID_RQE_QUEUE);

 /* Update current local time*/
 sbplugin_common_system_time_get (time);

 /* Loop through all the RQE queues*/
 BVIEW_BST_ITER (cosq, BVIEW_ASIC_MAX_RQE_QUEUES)
 {
   /* BST_Stat for each of the 11 RQE queues total use-counts in units of buffers.*/
   rv = SB_BRCM_API_COSQ_BST_STAT_GET (asic, 0 , cosq, SB_BRCM_BST_STAT_ID_RQE_QUEUE,
             0,&data->data[cosq].rqeBufferCount);
   if (SB_BRCM_RV_ERROR(rv))
   {
     SB_LOG (BVIEW_LOG_ERROR,
                 "BST:ASIC(%d) RQE queue (%d):Failed to get Buffer Use-Count.", asic, cosq);
     return BVIEW_STATUS_FAILURE;
   }
 }
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
BVIEW_STATUS sbplugin_common_bst_device_threshold_set (int asic, 
                                          BVIEW_BST_DEVICE_THRESHOLD_t *thres)
{
  SB_BRCM_COSQ_BST_PROFILE_t profile;
  int rv = 0;

   /*validate ASIC*/
  BVIEW_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (thres == NULL || 
      BVIEW_BST_DEVICE_THRESHOLD_CHECK (thres))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 

  profile.byte = thres->threshold;
  rv = SB_BRCM_COSQ_BST_PROFILE_SET(asic, 0, 0, SB_BRCM_BST_STAT_ID_DEVICE, &profile);
  if (SB_BRCM_RV_ERROR(rv))
  {
    SB_LOG (BVIEW_LOG_ERROR,
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
BVIEW_STATUS sbplugin_common_bst_ippg_threshold_set (int asic, int port, int pg, 
                                     BVIEW_BST_INGRESS_PORT_PG_THRESHOLD_t *thres)
{
  SB_BRCM_COSQ_BST_PROFILE_t profile;
  int rv = 0;
  SB_BRCM_GPORT_t gport=0;

   /*validate ASIC*/
  BVIEW_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (thres == NULL || 
      BVIEW_BST_IPPG_SHRD_THRESHOLD_CHECK (thres) ||
      BVIEW_BST_IPPG_HDRM_THRESHOLD_CHECK (thres))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 

  /* Get the GPORT */
  /* Temp fix: Application port numbering starts with 1 but the system expects it from 0 */
  rv = SB_BRCM_API_PORT_GPORT_GET (asic, port, &gport);
  if (SB_BRCM_RV_ERROR(rv))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  /* set threshold for shared buffer stats*/
  profile.byte = thres->umShareThreshold;  
  rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, gport, pg, SB_BRCM_BST_STAT_ID_PRI_GROUP_SHARED, &profile);
  if (SB_BRCM_RV_ERROR(rv))
  {
    SB_LOG (BVIEW_LOG_ERROR,
                "BST:ASIC(%d) Ingress Shared (Port, PG) PG(%d) Port(%d):Failed to set Threshold", asic,pg,port);
    return BVIEW_STATUS_FAILURE;
  }
 /* set threshold for headroom buffer stats*/
  profile.byte = thres->umHeadroomThreshold;
  rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, gport, pg, SB_BRCM_BST_STAT_ID_PRI_GROUP_HEADROOM, &profile);
  if (SB_BRCM_RV_ERROR(rv))
  {
    SB_LOG (BVIEW_LOG_ERROR,
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
BVIEW_STATUS sbplugin_common_bst_ipsp_threshold_set (int asic, int port, int sp, 
                                      BVIEW_BST_INGRESS_PORT_SP_THRESHOLD_t *thres)
{
  SB_BRCM_COSQ_BST_PROFILE_t profile;
  int rv = 0;
  SB_BRCM_GPORT_t gport=0;

   /*validate ASIC*/
  BVIEW_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (thres == NULL || 
      BVIEW_BST_IPSP_THRESHOLD_CHECK (thres))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
 
  /* Get the GPORT*/
  /* Temp fix: Application port numbering starts with 1 but the system expects it from 0 */
  rv = SB_BRCM_API_PORT_GPORT_GET (asic, port, &gport);
  if (SB_BRCM_RV_ERROR(rv))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  /*Set profile configuration for Ingress Port + Service Pools*/
  profile.byte = thres->umShareThreshold;  
  rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, gport, sp, SB_BRCM_BST_STAT_ID_PORT_POOL, &profile);
  if (SB_BRCM_RV_ERROR(rv))
  {
    SB_LOG (BVIEW_LOG_ERROR,
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
BVIEW_STATUS sbplugin_common_bst_isp_threshold_set (int asic, 
                                     int sp, 
                                     BVIEW_BST_INGRESS_SP_THRESHOLD_t *thres)
{
  SB_BRCM_COSQ_BST_PROFILE_t profile;
  int rv = 0;

   /*validate ASIC*/
  BVIEW_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (thres == NULL || 
      BVIEW_BST_ISP_THRESHOLD_CHECK (thres))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 

 /* The BST_Threshold for the (Ingress Port, SP) UC plus MC shared use-count.*/
  profile.byte = thres->umShareThreshold;  
  rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, 0, sp, SB_BRCM_BST_STAT_ID_ING_POOL, &profile);
  if (SB_BRCM_RV_ERROR(rv))
  {
    SB_LOG (BVIEW_LOG_ERROR,
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
BVIEW_STATUS sbplugin_common_bst_epsp_threshold_set (int asic, 
                                      int port, int sp, 
                                      BVIEW_BST_EGRESS_PORT_SP_THRESHOLD_t *thres)
{
  SB_BRCM_COSQ_BST_PROFILE_t profile;
  int rv = 0;
  SB_BRCM_GPORT_t gport=0;

   /*validate ASIC*/
  BVIEW_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (thres == NULL || 
      BVIEW_BST_EPSP_UC_THRESHOLD_CHECK (thres) ||
      BVIEW_BST_EPSP_UM_THRESHOLD_CHECK (thres)) 
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 

  /* Get the GPORT*/  
  /* Temp fix: Application port numbering starts with 1 but the system expects it from 0 */
  rv = SB_BRCM_API_PORT_GPORT_GET (asic, port, &gport);
  if (SB_BRCM_RV_ERROR(rv))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  /* The BST_Threshold for the Egress Per (Port, SP) 
   * UC shared use-count in units of 8 buffers
   */
  profile.byte = thres->ucShareThreshold;  
  rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, gport, sp, 
                          SB_BRCM_BST_STAT_ID_EGR_UCAST_PORT_SHARED, &profile);
  if (SB_BRCM_RV_ERROR(rv))
  {
    SB_LOG (BVIEW_LOG_ERROR,
                "BST:ASIC(%d) Egress UC(Port,SP) SP(%d) Port(%d):Failed to set Threshold", asic,sp, port);
    return BVIEW_STATUS_FAILURE;
  }

 /* The BST_Threshold for the Egress Per (Port, SP) 
  * MC/UC+MC shared use-count in units of buffers.
  */
  profile.byte = thres->umShareThreshold;
  rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, gport, sp, 
                               SB_BRCM_BST_STAT_ID_EGR_PORT_SHARED, &profile);
  if (SB_BRCM_RV_ERROR(rv))
  {
    SB_LOG (BVIEW_LOG_ERROR,
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
BVIEW_STATUS sbplugin_common_bst_esp_threshold_set (int asic, 
                                     int sp, 
                                     BVIEW_BST_EGRESS_SP_THRESHOLD_t *thres)
{
  SB_BRCM_COSQ_BST_PROFILE_t profile;
  int rv = 0;

   /*validate ASIC*/
  BVIEW_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (thres == NULL || 
      BVIEW_BST_E_SP_UM_THRESHOLD_CHECK (thres) || 
      BVIEW_BST_E_SP_MC_THRESHOLD_CHECK (thres))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 

  /* BST_Threshold for each of the 4 Egress SPs Shared use-counts in units of buffers.*/
  profile.byte = thres->umShareThreshold;  
  rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, 0, sp, SB_BRCM_BST_STAT_ID_EGR_POOL, &profile);
  if (SB_BRCM_RV_ERROR(rv))
  {
    SB_LOG (BVIEW_LOG_ERROR,
                "BST:ASIC(%d) Egress SP (%d):Failed to set Threshold", asic,sp);
    return BVIEW_STATUS_FAILURE;
  }

  /* BST_Threshold for each of the 4 Egress SPs Shared use-counts in units of buffers.*/
  profile.byte = thres->mcShareThreshold;
  rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, 0, sp, SB_BRCM_BST_STAT_ID_EGR_MCAST_POOL, &profile);
  if (SB_BRCM_RV_ERROR(rv))
  {
    SB_LOG (BVIEW_LOG_ERROR,
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
BVIEW_STATUS sbplugin_common_bst_eucq_threshold_set (int asic, 
                                      int ucQueue, 
                              BVIEW_BST_EGRESS_UC_QUEUE_THRESHOLD_t *thres)
{
  SB_BRCM_COSQ_BST_PROFILE_t profile;
  int rv = 0, port = 0, cosq = 0;
  SB_BRCM_GPORT_t gport = 0;

   /*validate ASIC*/
  BVIEW_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (thres == NULL || 
      BVIEW_BST_E_UC_THRESHOLD_CHECK (thres))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
 
  cosq = ucQueue % BVIEW_BST_NUM_COS_PORT;
  port = ucQueue/BVIEW_BST_NUM_COS_PORT;
  rv = SB_BRCM_API_PORT_GPORT_GET (asic, (port+1), &gport);
  if (SB_BRCM_RV_ERROR(rv))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  /*  The BST_Threshold for the Egress UC Queues.*/ 
  profile.byte = thres->ucBufferThreshold;  
  rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, gport, cosq, SB_BRCM_BST_STAT_ID_UCAST, &profile);
  if (SB_BRCM_RV_ERROR(rv))
  {
    SB_LOG (BVIEW_LOG_ERROR,
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
BVIEW_STATUS sbplugin_common_bst_eucqg_threshold_set (int asic, 
                                       int ucQueueGrp, 
                                       BVIEW_BST_EGRESS_UC_QUEUEGROUPS_THRESHOLD_t *thres)
{
  SB_BRCM_COSQ_BST_PROFILE_t profile;
  int rv = 0;

   /*validate ASIC*/
  BVIEW_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (thres == NULL ||
      BVIEW_BST_E_UC_GRP_THRESHOLD_CHECK (thres))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 

  /*The BST_Threshold for the Egress UC Queue-Group */
  profile.byte = thres->ucBufferThreshold;  
  rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, 0, ucQueueGrp, SB_BRCM_BST_STAT_ID_UCAST_GROUP, &profile);
  if (SB_BRCM_RV_ERROR(rv))
  {
    SB_LOG (BVIEW_LOG_ERROR,
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
BVIEW_STATUS sbplugin_common_bst_emcq_threshold_set (int asic, 
                                      int mcQueue, 
                                      BVIEW_BST_EGRESS_MC_QUEUE_THRESHOLD_t *thres)
{
  SB_BRCM_COSQ_BST_PROFILE_t profile;
  int rv = 0, port = 0, cosq = 0;
  SB_BRCM_GPORT_t gport = 0;

   /*validate ASIC*/
  BVIEW_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (thres == NULL || 
      BVIEW_BST_E_MC_THRESHOLD_CHECK (thres))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 

  cosq = mcQueue % BVIEW_BST_NUM_COS_PORT;
  port = mcQueue/BVIEW_BST_NUM_COS_PORT;
  rv = SB_BRCM_API_PORT_GPORT_GET (asic, (port+1), &gport);

  profile.byte = thres->mcBufferThreshold;  
  rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, gport, cosq, SB_BRCM_BST_STAT_ID_MCAST, &profile);
  if (SB_BRCM_RV_ERROR(rv))
  {
    SB_LOG (BVIEW_LOG_ERROR,
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
BVIEW_STATUS sbplugin_common_bst_cpuq_threshold_set (int asic, 
                                      int cpuQueue, 
                                      BVIEW_BST_EGRESS_CPU_QUEUE_THRESHOLD_t *thres)
{
  SB_BRCM_COSQ_BST_PROFILE_t profile;
  int rv = 0;
  unsigned int port = 0;
  SB_BRCM_GPORT_t gport = 0;

   /*validate ASIC*/
  BVIEW_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (thres == NULL || 
      BVIEW_BST_E_CPU_THRESHOLD_CHECK (thres))
     /* BVIEW_BST_E_CPU_QUEUE_THRESHOLD_CHECK (thres))*/
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 

  /* get CPU port*/
  SB_BRCM_CPU_PORT_GET(asic, &port);
  rv = SB_BRCM_API_PORT_GPORT_GET(asic, port, &gport);
  if (SB_BRCM_RV_ERROR(rv))
  {
    return BVIEW_STATUS_FAILURE;
  } 
  /* CPU queues are saved in the same table as the Egress Multicast queues 
   * (Egress MC Queue BST Table). The entry for the CPU queues starts 
   * from 520 to 567 for 48 CPU queues) */

  profile.byte = thres->cpuBufferThreshold;  
  rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, gport, cpuQueue, SB_BRCM_BST_STAT_ID_CPU, &profile);
  if (SB_BRCM_RV_ERROR (rv))
  {
    SB_LOG (BVIEW_LOG_ERROR,
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
BVIEW_STATUS sbplugin_common_bst_rqeq_threshold_set (int asic, 
                                     int rqeQueue, 
                                     BVIEW_BST_EGRESS_RQE_QUEUE_THRESHOLD_t *thres)
{
  SB_BRCM_COSQ_BST_PROFILE_t profile;
  int rv = 0;

  /*validate ASIC*/
  BVIEW_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (thres == NULL || 
      BVIEW_BST_E_RQE_THRESHOLD_CHECK (thres)) 
    /*  BVIEW_BST_E_RQE_QUEUE_THRESHOLD_CHECK (thres))*/
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  /* Get Threshold configuration for RQE queues*/ 
  profile.byte = thres->rqeBufferThreshold;  
  rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, 0, rqeQueue, SB_BRCM_BST_STAT_ID_RQE_QUEUE, &profile);
  if (SB_BRCM_RV_ERROR (rv))
  {
    SB_LOG (BVIEW_LOG_ERROR,
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
BVIEW_STATUS  sbplugin_common_bst_clear_stats(int asic)
{
  int rv = 0;
  unsigned int bid =0; 
  unsigned int port = 0;
  SB_BRCM_GPORT_t  gport = 0;
  unsigned int cosq =0; 
  unsigned int num = 0;

   /*validate ASIC*/
  BVIEW_UNIT_CHECK (asic);

  /* Clear all the stats*/
  BVIEW_BST_ITER (bid, SB_BRCM_BST_STAT_ID_MAX_COUNT)
  {
    if (bid == SB_BRCM_BST_STAT_ID_EGR_POOL||
        bid == SB_BRCM_BST_STAT_ID_EGR_MCAST_POOL ||
        bid == SB_BRCM_BST_STAT_ID_EGR_UCAST_PORT_SHARED ||
        bid == SB_BRCM_BST_STAT_ID_EGR_PORT_SHARED ||
        bid == SB_BRCM_BST_STAT_ID_UCAST_GROUP||
        bid == SB_BRCM_BST_STAT_ID_PORT_POOL ||
        bid == SB_BRCM_BST_STAT_ID_ING_POOL)
    {
        num = 4;
    }
    else if (bid == SB_BRCM_BST_STAT_ID_PRI_GROUP_SHARED ||
              bid == SB_BRCM_BST_STAT_ID_PRI_GROUP_HEADROOM ||
              bid == SB_BRCM_BST_STAT_ID_UCAST ||
              bid == SB_BRCM_BST_STAT_ID_MCAST)
    {
       num = 8;
    }
    else if (bid == SB_BRCM_BST_STAT_ID_RQE_QUEUE)
    {
        num = BVIEW_ASIC_MAX_RQE_QUEUES;
    }
    else 
    {
       num = 1;
    }
   
    if (bid == SB_BRCM_BST_STAT_ID_ING_POOL ||
        bid == SB_BRCM_BST_STAT_ID_EGR_MCAST_POOL ||
        bid == SB_BRCM_BST_STAT_ID_EGR_POOL ||
        bid == SB_BRCM_BST_STAT_ID_RQE_QUEUE ||
        bid == SB_BRCM_BST_STAT_ID_UCAST_GROUP||
        bid == SB_BRCM_BST_STAT_ID_DEVICE)
    {
       gport = 0;
       BVIEW_BST_ITER (cosq, num)
       {
         rv = SB_BRCM_COSQ_BST_STAT_CLEAR (asic, gport, cosq, bid);
         if (SB_BRCM_RV_ERROR (rv))
         {
            return BVIEW_STATUS_FAILURE;
         }
       } /* Iter ... Cosq*/
    }
    else if (bid == SB_BRCM_BST_STAT_ID_PRI_GROUP_SHARED ||
             bid == SB_BRCM_BST_STAT_ID_PRI_GROUP_HEADROOM ||
             bid == SB_BRCM_BST_STAT_ID_UCAST ||
             bid == SB_BRCM_BST_STAT_ID_MCAST ||
             bid == SB_BRCM_BST_STAT_ID_PORT_POOL ||
             bid == SB_BRCM_BST_STAT_ID_EGR_PORT_SHARED ||
             bid == SB_BRCM_BST_STAT_ID_EGR_UCAST_PORT_SHARED)
    {
      BVIEW_BST_PORT_ITER (asic, port)
      {
        if (!SB_BRCM_IS_XE_PORT (asic, port))
          continue;
        /* Get GPORT*/
        rv = SB_BRCM_API_PORT_GPORT_GET (asic, port, &gport);
        if (SB_BRCM_RV_ERROR (rv))
        {
           return BVIEW_STATUS_FAILURE;
        }

        BVIEW_BST_ITER (cosq, num)
        {
          rv = SB_BRCM_COSQ_BST_STAT_CLEAR (asic, gport, cosq, bid);
          if (SB_BRCM_RV_ERROR (rv))
          {
             return BVIEW_STATUS_FAILURE;
          }
        } /* Iter ... Cosq*/
      } /* Inter ...Port*/
    }
  } /* Iter ...SB_BRCM_BST_STAT_ID_MAX_COUNT*/
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
BVIEW_STATUS  sbplugin_common_bst_clear_thresholds  (int asic)
{
  SB_BRCM_COSQ_BST_PROFILE_t profile;
  unsigned int           port = 0;
  unsigned int           index =0;
  SB_BRCM_GPORT_t            gport = 0;
  int                    rv = 0;                   
  
   /*validate ASIC*/
  BVIEW_UNIT_CHECK (asic);

  /*Configure Default The BST_Threshold for Device Use-Countin units of buffers.*/
  profile.byte = BVIEW_BST_DEVICE_THRES_DEFAULT;
  rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, 0, 0, SB_BRCM_BST_STAT_ID_DEVICE, &profile);
  if (SB_BRCM_RV_ERROR(rv))
  {
    return BVIEW_STATUS_FAILURE;
  }
 
  /* Configure default threshold for port based stats*/
  BVIEW_BST_PORT_ITER (asic, port)
  {
    if (!SB_BRCM_IS_XE_PORT (asic, port))
      continue;

    rv = SB_BRCM_API_PORT_GPORT_GET (asic, port, &gport); 
    if (SB_BRCM_RV_ERROR(rv))
    {
      return BVIEW_STATUS_FAILURE;
    }

    BVIEW_BST_PG_ITER (index)
    { 
      /*  Configure Default The BST_Threshold for the 
       * (Ingress Port, PG) UC plus MC Shared use-count in units of buffers.
       */
      profile.byte = BVIEW_BST_I_P_PG_UCMC_SHARED_THRES_DEFAULT;
      rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, gport, index, SB_BRCM_BST_STAT_ID_PRI_GROUP_SHARED, &profile);
      if (SB_BRCM_RV_ERROR(rv))
      {
        return BVIEW_STATUS_FAILURE;
      }

      /*  Configure Default The BST_Threshold for the 
       * (Ingress Port, PG) UC plus MC Headroom use-count in units of buffers.
       */
      profile.byte =  BVIEW_BST_I_P_PG_UCMC_HDRM_THRES_DEFAULT;
      rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, gport, index, SB_BRCM_BST_STAT_ID_PRI_GROUP_HEADROOM, &profile);
      if (SB_BRCM_RV_ERROR(rv))
      {
        return BVIEW_STATUS_FAILURE;
      }
    }
    
    /* Configure Default The BST_Threshold for the 
     * (Ingress Port, SP) UC plus MC shared use-count in units of buffers.
     */
    profile.byte = BVIEW_BST_I_P_SP_UCMC_SHARED_THRES_DEFAULT;
    BVIEW_BST_SP_ITER (index) 
    {
      rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, gport, index, SB_BRCM_BST_STAT_ID_PORT_POOL, &profile);
      if (SB_BRCM_RV_ERROR(rv))
      {
        return BVIEW_STATUS_FAILURE;
      }
    }

    BVIEW_BST_SP_ITER (index) 
    {
      /* Configure default The BST_Threshold for the Egress Per (Port, SP) UC shared.*/
      profile.byte = BVIEW_BST_E_P_SP_UC_THRES_DEFAULT;
      rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, gport, index, SB_BRCM_BST_STAT_ID_EGR_UCAST_PORT_SHARED, &profile);
      if (SB_BRCM_RV_ERROR(rv))
      {
        return BVIEW_STATUS_FAILURE;
      }
      /* Configure Default BST_Threshold for the Egress Per (Port, SP) MC/UC+MC shared*/
      profile.byte = BVIEW_BST_E_P_SP_UCMC_THRES_DEFAULT ;
      rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, gport, index, SB_BRCM_BST_STAT_ID_EGR_PORT_SHARED, &profile);
      if (SB_BRCM_RV_ERROR(rv))
      {
        return BVIEW_STATUS_FAILURE;
      }
    }
    /* Configure default threshold for Mcast queues*/
    BVIEW_BST_ITER (index, BVIEW_BST_NUM_COS_PORT)
    {
      profile.byte = BVIEW_BST_MCAST_THRES_DEFAULT;
      rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, gport, index, SB_BRCM_BST_STAT_ID_MCAST, &profile);
      if (SB_BRCM_RV_ERROR (rv))
      {
        return BVIEW_STATUS_FAILURE;
      }
    }
    /* Configure default threshold for Ucast queues*/
    profile.byte = BVIEW_BST_UCAST_THRES_DEFAULT;
    BVIEW_BST_ITER (index, BVIEW_BST_NUM_COS_PORT)
    {
      rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, gport, index, SB_BRCM_BST_STAT_ID_UCAST, &profile);
      if (SB_BRCM_RV_ERROR(rv))
      {
        return BVIEW_STATUS_FAILURE;
      }    
    }

  }

  BVIEW_BST_SP_ITER (index)
  {
    /*  Configure Default BST_Threshold for each of the 4 Egress 
     *  SPs Shared use-counts (UC+MC) in units of buffers.
     */
    profile.byte = BVIEW_BST_E_SP_UCMC_THRES_DEFAULT;
    rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, 0, index, SB_BRCM_BST_STAT_ID_EGR_POOL, &profile);
    if (SB_BRCM_RV_ERROR(rv))
    {
      return BVIEW_STATUS_FAILURE;
    }
    
    profile.byte = BVIEW_BST_E_SP_MC_THRES_DEFAULT;
     /*  BST_Threshold for each of the 4 Egress SPs Shared use-counts (MC) in units of buffers.*/
    rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, 0, index, SB_BRCM_BST_STAT_ID_EGR_MCAST_POOL, &profile);
    if (SB_BRCM_RV_ERROR(rv))
    {
      return BVIEW_STATUS_FAILURE;
    }

  }
  /*Configure Default The BST_Threshold for the Ingress SP 
   * UC plus MC use-count in units of buffers.
   */
  profile.byte = BVIEW_BST_I_SP_UCMC_SHARED_THRES_DEFAULT;
  BVIEW_BST_SP_ITER (index)
  {
    rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, 0, index, SB_BRCM_BST_STAT_ID_ING_POOL, &profile);
    if (SB_BRCM_RV_ERROR(rv))
    {
      return BVIEW_STATUS_FAILURE;
    }
  }
  /* Get CPU port*/
  SB_BRCM_CPU_PORT_GET(asic, &port);
  rv = SB_BRCM_API_PORT_GPORT_GET(asic, port, &gport);
  if (SB_BRCM_RV_ERROR(rv))
  {
   return BVIEW_STATUS_FAILURE;
  }
  /* Configure Default The BST_Threshold for the Egress CPU Queues 
   * in units of egress queue entries.
   */
  profile.byte = BVIEW_BST_E_CPU_UCMC_THRES_DEFAULT;
  BVIEW_BST_ITER (index, BVIEW_ASIC_MAX_CPU_QUEUES)
  {
    rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, gport, index, SB_BRCM_BST_STAT_ID_CPU, &profile);
    if (SB_BRCM_RV_ERROR(rv))
    {
      return BVIEW_STATUS_FAILURE;
    }
  }

  profile.byte = BVIEW_BST_E_RQE_THRES_DEFAULT;
  /* Configure Default BST_Threshold for each of the 11 RQE queues 
   * total use-counts in units of buffers.
   */
  BVIEW_BST_ITER (index, BVIEW_ASIC_MAX_RQE_QUEUES)
  {
    rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, 0, index, SB_BRCM_BST_STAT_ID_RQE_QUEUE, &profile);
    if (SB_BRCM_RV_ERROR (rv))
    {
      return BVIEW_STATUS_FAILURE;
    }
  } 

  /* Configure Thresholds for BST_Threshold for each of the 128 Egress Unicast queue groups*/
  profile.byte = BVIEW_BST_UCAST_QUEUE_GROUP_DEFAULT; 
  BVIEW_BST_ITER (index, BVIEW_ASIC_MAX_UC_QUEUE_GROUPS)
  {
    rv = SB_BRCM_COSQ_BST_PROFILE_SET (asic, 0, index, SB_BRCM_BST_STAT_ID_UCAST_GROUP, &profile);
    if (SB_BRCM_RV_ERROR (rv))
    {
      return BVIEW_STATUS_FAILURE;
    }
  }

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
BVIEW_STATUS  sbplugin_common_bst_register_trigger (int asic, 
                                        BVIEW_BST_TRIGGER_CALLBACK_t callback, 
                                        void *cookie)
{
  int rv  = 0;

   /*validate ASIC*/
  BVIEW_UNIT_CHECK (asic);
   
  bst_hw_trigger_cb = callback;

  /* Register Hw trigger callback*/
  rv = SB_BRCM_SWITCH_EVENT_REGISTER (asic, 
                                      (SB_BRCM_SWITCH_EVENT_CB_t)sbplugin_common_bst_callback,
                                      cookie); 
  if (SB_BRCM_RV_ERROR (rv))
  {
    return BVIEW_STATUS_FAILURE;
  }
  return BVIEW_STATUS_SUCCESS;
}

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
                              BVIEW_TIME_t *time)
{
  SB_BRCM_COSQ_BST_PROFILE_t profile;
  unsigned int           port = 0;
  unsigned int           index =0;
  SB_BRCM_GPORT_t        gport = 0;
  int                    rv = 0;                   

   /*validate ASIC*/
  BVIEW_UNIT_CHECK (asic);

  /* Check validity of input data*/
  BVIEW_NULLPTR_CHECK (thresholdSnapshot);

   /* Update current local time*/
  sbplugin_common_system_time_get (time);

  /* Device wide threshold configuration*/
  rv = SB_BRCM_COSQ_BST_PROFILE_GET(asic, 0, 0, SB_BRCM_BST_STAT_ID_DEVICE, &profile);
  if (SB_BRCM_RV_ERROR(rv))
  {
    return BVIEW_STATUS_FAILURE;
  }
  thresholdSnapshot->device.bufferCount = profile.byte;

  /* Get port based thresholds*/ 
  BVIEW_BST_PORT_ITER (asic, port)
  {
    if (!SB_BRCM_IS_XE_PORT (asic, port))
     continue;

    /* Get GPORT */
    rv = SB_BRCM_API_PORT_GPORT_GET(asic, port, &gport); 
    if (SB_BRCM_RV_ERROR(rv))
    {
      return BVIEW_STATUS_FAILURE;
    }

    BVIEW_BST_PG_ITER (index)
    { 
      /* Get threshold configuration of The BST_Threshold for the (Ingress Port, PG) 
       * UC plus MC Shared use-count in units of buffers.
       */
      rv = SB_BRCM_COSQ_BST_PROFILE_GET (asic, gport, index, 
                                     SB_BRCM_BST_STAT_ID_PRI_GROUP_SHARED, &profile);
      if (SB_BRCM_RV_ERROR(rv))
      {
        return BVIEW_STATUS_FAILURE;
      }
      thresholdSnapshot->iPortPg.data[port - 1][index].umShareBufferCount = profile.byte;

      /* Get threshold for headroom The BST_Threshold for the (Ingress Port, PG) 
       * UC plus MC Headroom use-count in units of buffers
       */
      rv = SB_BRCM_COSQ_BST_PROFILE_GET (asic, gport, index, 
                                     SB_BRCM_BST_STAT_ID_PRI_GROUP_HEADROOM, &profile);
      if (SB_BRCM_RV_ERROR(rv))
      {
        return BVIEW_STATUS_FAILURE;
      }
      thresholdSnapshot->iPortPg.data[port - 1][index].umHeadroomBufferCount = profile.byte;
    }
    
    /*Get threshold configuration for The BST_Threshold for the (Ingress Port, SP) 
     * UC plus MC shared use-count in units of buffers.
     */  
    BVIEW_BST_SP_ITER (index)
    {
      rv = SB_BRCM_COSQ_BST_PROFILE_GET(asic, gport, index, 
                                     SB_BRCM_BST_STAT_ID_PORT_POOL, &profile);
      if (SB_BRCM_RV_ERROR(rv))
      {
        return BVIEW_STATUS_FAILURE;
      }
      thresholdSnapshot->iPortSp.data[port - 1][index].umShareBufferCount = profile.byte;
    }

    BVIEW_BST_SP_ITER (index)
    {
      /* The BST_Threshold for the Egress Per (Port, SP) 
       * UC shared use-count in units of 8 buffers */
      rv = SB_BRCM_COSQ_BST_PROFILE_GET (asic, gport, index, 
                                     SB_BRCM_BST_STAT_ID_EGR_UCAST_PORT_SHARED, &profile);
      if (SB_BRCM_RV_ERROR(rv))
      {
        return BVIEW_STATUS_FAILURE;
      }
      thresholdSnapshot->ePortSp.data[port - 1][index].ucShareBufferCount = profile.byte;

      /* Get threshold configuration for egress based port shared buffers*/
      rv = SB_BRCM_COSQ_BST_PROFILE_GET (asic, gport, index, 
                                     SB_BRCM_BST_STAT_ID_EGR_PORT_SHARED, &profile);
      if (SB_BRCM_RV_ERROR(rv))
      {
        return BVIEW_STATUS_FAILURE;
      }
      thresholdSnapshot->ePortSp.data[port - 1][index].umShareBufferCount = profile.byte;
    }
  
    /* Get threshold configuration for The BST_Threshold for the Egress MC Queues in units of buffers.*/
    BVIEW_BST_ITER (index, BVIEW_BST_NUM_COS_PORT)
    {
      rv = SB_BRCM_COSQ_BST_PROFILE_GET (asic, gport, index, SB_BRCM_BST_STAT_ID_MCAST, &profile);
      if (SB_BRCM_RV_ERROR (rv))
      {
        return BVIEW_STATUS_FAILURE;
      }
      thresholdSnapshot->eMcQ.data[(port -1)*BVIEW_BST_NUM_COS_PORT+index].mcBufferCount = profile.byte;
      thresholdSnapshot->eMcQ.data[(port -1)*BVIEW_BST_NUM_COS_PORT+index].port = port;
    }
    /* Get threshold configuration for The BST_Threshold for the Egress UC Queues in units of 8 buffers.*/
    BVIEW_BST_ITER (index, BVIEW_BST_NUM_COS_PORT)
    {
      rv = SB_BRCM_COSQ_BST_PROFILE_GET (asic, gport, index, SB_BRCM_BST_STAT_ID_UCAST, &profile);
      if (SB_BRCM_RV_ERROR(rv))
      {
        return BVIEW_STATUS_FAILURE;
      }    
      thresholdSnapshot->eUcQ.data[(port -1)*BVIEW_BST_NUM_COS_PORT+index].ucBufferCount = profile.byte;
      thresholdSnapshot->eUcQ.data[(port -1)*BVIEW_BST_NUM_COS_PORT+index].port = port;
    }
  }
  
  BVIEW_BST_SP_ITER (index)
  {
    /*  BST_Threshold for each of the 4 Egress SPs Shared use-counts in units of buffers.*/
    rv = SB_BRCM_COSQ_BST_PROFILE_GET (asic, 0, index, SB_BRCM_BST_STAT_ID_EGR_POOL, &profile);
    if (SB_BRCM_RV_ERROR(rv))
    {
      return BVIEW_STATUS_FAILURE;
    }
    thresholdSnapshot->eSp.data[index].umShareBufferCount = profile.byte;

    /*  BST_Threshold for each of the 4 Egress SPs Shared use-counts in units of buffers.*/
    rv = SB_BRCM_COSQ_BST_PROFILE_GET (asic, 0, index, SB_BRCM_BST_STAT_ID_EGR_MCAST_POOL, &profile);
    if (SB_BRCM_RV_ERROR(rv))
    {
      return BVIEW_STATUS_FAILURE;
    }
    thresholdSnapshot->eSp.data[index].mcShareBufferCount = profile.byte;
  }

  /* Get threshold configuration for The BST_Threshold for the Ingress 
   * SP UC plus MC use-count in units of buffers.
  */
  BVIEW_BST_SP_ITER (index)
  {
    rv = SB_BRCM_COSQ_BST_PROFILE_GET (asic, 0, index, SB_BRCM_BST_STAT_ID_ING_POOL, &profile);
    if (SB_BRCM_RV_ERROR(rv))
    {
      return BVIEW_STATUS_FAILURE;
    }
    thresholdSnapshot->iSp.data[index].umShareBufferCount = profile.byte;
  }

  /* Get CPU queues threshold configuration*/
  SB_BRCM_CPU_PORT_GET(asic, &port);
  rv = SB_BRCM_API_PORT_GPORT_GET(asic, port, &gport);
  if (SB_BRCM_RV_ERROR(rv))
  {
   return BVIEW_STATUS_FAILURE;
  }

  /* Get thresholds for The BST_Threshold for the Egress CPU queues in units of buffers*/
  BVIEW_BST_ITER (index, BVIEW_ASIC_MAX_CPU_QUEUES)
  {
    rv = SB_BRCM_COSQ_BST_PROFILE_GET (asic, gport, index, SB_BRCM_BST_STAT_ID_CPU, &profile);
    if (SB_BRCM_RV_ERROR(rv))
    {
      return BVIEW_STATUS_FAILURE;
    }
    thresholdSnapshot->cpqQ.data[index].cpuBufferCount = profile.byte;
  }

  /* Get Thresholds for BST_Threshold for each of the 11 RQE queues 
   * total use-counts in units of buffers.
  */
  BVIEW_BST_ITER (index, BVIEW_ASIC_MAX_RQE_QUEUES)
  {
    rv = SB_BRCM_COSQ_BST_PROFILE_GET (asic, 0, index, SB_BRCM_BST_STAT_ID_RQE_QUEUE, &profile);
    if (SB_BRCM_RV_ERROR (rv))
    {
      return BVIEW_STATUS_FAILURE;
    }
    thresholdSnapshot->rqeQ.data[index].rqeBufferCount = profile.byte;
  } 

  /* Get Thresholds for BST_Threshold for each of the 128 Egress Unicast queue groups*/
  BVIEW_BST_ITER (index, BVIEW_ASIC_MAX_UC_QUEUE_GROUPS)
  {
    rv = SB_BRCM_COSQ_BST_PROFILE_GET (asic, 0, index, SB_BRCM_BST_STAT_ID_UCAST_GROUP, &profile);
    if (SB_BRCM_RV_ERROR (rv))
    {
      return BVIEW_STATUS_FAILURE;
    }
    thresholdSnapshot->eUcQg.data[index].ucBufferCount = profile.byte;
  }

  return BVIEW_STATUS_SUCCESS;
}

static BVIEW_STATUS sbplugin_common_bst_bid_to_realm_get( int bid, char *realm, char *counter)
{
  unsigned int i;

  static SB_BRCM_BID_REALM_t bid_realm_counter_map [] = {
    {SB_BRCM_BST_STAT_ID_DEVICE, "device" ,"data"},
    {SB_BRCM_BST_STAT_ID_ING_POOL, "ingress-service-pool", "um-share-buffer-count"},
    {SB_BRCM_BST_STAT_ID_PORT_POOL, "ingress-port-service-pool", "um-share-buffer-count" },
    {SB_BRCM_BST_STAT_ID_PRI_GROUP_SHARED, "ingress-port-priority-group", "um-share-buffer-count"},
    {SB_BRCM_BST_STAT_ID_PRI_GROUP_HEADROOM, "ingress-port-priority-group", "um-headroom-buffer-count"},
    {SB_BRCM_BST_STAT_ID_EGR_POOL, "egress-service-pool", "um-share-buffer-count"},
    {SB_BRCM_BST_STAT_ID_EGR_MCAST_POOL, "egress-service-pool", "mc-share-buffer-count"},
    {SB_BRCM_BST_STAT_ID_UCAST, "egress-uc-queue", "uc-buffer-count"},
    {SB_BRCM_BST_STAT_ID_MCAST, "egress-mc-queue", "mc-buffer-count"},
    {SB_BRCM_BST_STAT_ID_EGR_UCAST_PORT_SHARED, "egress-port-service-pool", "uc-share-buffer-count"},
    {SB_BRCM_BST_STAT_ID_EGR_PORT_SHARED, "egress-port-service-pool", "um-share-buffer-count"},
    {SB_BRCM_BST_STAT_ID_RQE_QUEUE, "egress-rqe-queue", "rqe-buffer-count"},
    {SB_BRCM_BST_STAT_ID_UCAST_GROUP, "egress-uc-queue-group", "uc-buffer-count"}
  }; 

    if ((NULL == realm) ||
       (NULL == counter))
    {
      return BVIEW_STATUS_INVALID_PARAMETER;
    }

    for (i = 0; i < SB_BRCM_BST_STAT_ID_MAX; i++)
    {
      if (bid == bid_realm_counter_map[i].bid)
      {
        strncpy(realm, bid_realm_counter_map[i].realm, strlen(bid_realm_counter_map[i].realm));
        strncpy(counter, bid_realm_counter_map[i].counter, strlen(bid_realm_counter_map[i].counter));
        return BVIEW_STATUS_SUCCESS;
      }
    }
      return BVIEW_STATUS_INVALID_PARAMETER;

}
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
                       int bid, int port, int cosq, void *cookie)
{
  BVIEW_BST_TRIGGER_INFO_t triggerInfo;

  memset(&triggerInfo, 0, sizeof(BVIEW_BST_TRIGGER_INFO_t));

  /* Call application Callback registered with plugin*/ 
  if (event == SB_BRCM_SWITCH_EVENT_MMU_BST_TRIGGER) 
  {
    /* get the realm and the counter from the BID */
    if (BVIEW_STATUS_SUCCESS != sbplugin_common_bst_bid_to_realm_get(bid, &triggerInfo.realm[0], &triggerInfo.counter[0]))
    {
      return BVIEW_STATUS_INVALID_PARAMETER;
    }

    if ((SB_BRCM_BST_STAT_ID_UCAST == bid) ||
       (SB_BRCM_BST_STAT_ID_MCAST == bid))
    {
      /* derive the inputted queue */
      cosq = ((port-1)*BVIEW_BST_NUM_COS_PORT) + cosq;
    }

    triggerInfo.port = port;
    triggerInfo.queue = cosq;
    #ifdef BST_DEBUG_METRICS
    char buf[BVIEW_TIME_BUFFER_SIZE];
    system_dispaly_local_time_get (buf);
    printf ("\r\n%s: Trigger event for buffer (%d) realm (%s) counter (%s) index1 (%d) index2 (%d)",
             buf, bid,triggerInfo.realm, triggerInfo.counter, port, cosq);
    #endif
    /* Call application callback routine*/
    bst_hw_trigger_cb ((asic), cookie, &triggerInfo); 
  }
  return BVIEW_STATUS_SUCCESS;
}


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
                                                int queue, uint64_t *dropCount)
{
  int rv = 0; 
  SB_BRCM_GPORT_t gport = 0;
  SB_BRCM_GPORT_t cosqGport = 0;
  uint64 value;
    
   /*validate ASIC*/
  BVIEW_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if ((dropCount == NULL)  ||
      (queue >= BVIEW_BST_NUM_COS_PORT))
  {
    SB_LOG (BVIEW_LOG_ERROR,
                "BST UCAST congestion drops:ASIC(%d) port(%d): queue(%d) or dropCount(%p) " 
                "are invalid params\n", asic, port, queue, dropCount);
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 

  if (!SB_BRCM_IS_XE_PORT (asic, port))
  {
    SB_LOG (BVIEW_LOG_ERROR,
                "BST UCAST congestion drops:ASIC(%d): Not a vlid port(%d) \n", 
                 asic, port);
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 
     
  rv = SB_BRCM_API_PORT_GPORT_GET(asic, port , &gport);
  if (SB_BRCM_RV_ERROR(rv))
  {
    SB_LOG (BVIEW_LOG_ERROR,
                "BST UCAST congestion drops:ASIC(%d) port(%d) "
                "Failed to get gport for port\n", asic, port);

    return BVIEW_STATUS_FAILURE;
  }

  if (sbplugin_common_system_ucastq_gport_get(asic, port, queue, &cosqGport) 
                                                        != BVIEW_STATUS_SUCCESS)
  {
    SB_LOG (BVIEW_LOG_ERROR,
                "BST UCAST congestion drops:ASIC(%d) port(%d) queue(%d) "
                "Failed to get gport for cos queue\n", asic, port, queue);
    return BVIEW_STATUS_FAILURE; 
  }

  rv = SB_BRCM_COSQ_STAT_GET(asic, cosqGport, 0, SB_BRCM_COSQ_STAT_DROPPED_PACKETS, 
                             &value);   
  if (SB_BRCM_RV_ERROR(rv))
  {
    SB_LOG (BVIEW_LOG_ERROR,
                "BST UCAST congestion drops:ASIC(%d) port(%d) cosqGport(0x%x)"
                "Failed to get congestion drop counter\n", asic, port, cosqGport);

    return BVIEW_STATUS_FAILURE;
  }
  *dropCount = value;
  return BVIEW_STATUS_SUCCESS;
}


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
                                              int queue, uint64_t *dropCount)
{
  int rv = 0; 
  SB_BRCM_GPORT_t gport = 0;
  SB_BRCM_GPORT_t cosqGport = 0;
  uint64  value;
    
   /*validate ASIC*/
  BVIEW_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if ((dropCount == NULL)  ||
      (queue >= BVIEW_BST_NUM_COS_PORT))
  {
    SB_LOG (BVIEW_LOG_ERROR,
                "BST MCAST congestion drops:ASIC(%d) port(%d): queue(%d) or dropCount(%p) " 
                "are invalid params\n", asic, port, queue, dropCount);
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 

  if (!SB_BRCM_IS_XE_PORT (asic, port))
  {
    SB_LOG (BVIEW_LOG_ERROR,
                "BST MCAST congestion drops:ASIC(%d): Not a vlid port(%d) \n", 
                 asic, port);
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 
     
  rv = SB_BRCM_API_PORT_GPORT_GET(asic, port , &gport);
  if (SB_BRCM_RV_ERROR(rv))
  {
    SB_LOG (BVIEW_LOG_ERROR,
                "BST MCAST congestion drops:ASIC(%d) port(%d) "
                "Failed to get gport for port\n", asic, port);

    return BVIEW_STATUS_FAILURE;
  }

  if (sbplugin_common_system_mcastq_gport_get(asic, port, queue, &cosqGport) 
                                                        != BVIEW_STATUS_SUCCESS)
  {
    SB_LOG (BVIEW_LOG_ERROR,
                "BST MCAST congestion drops:ASIC(%d) port(%d) queue(%d) "
                "Failed to get gport for cos queue\n", asic, port, queue);
    return BVIEW_STATUS_FAILURE; 
  }

  rv = SB_BRCM_COSQ_STAT_GET(asic, cosqGport, 0, SB_BRCM_COSQ_STAT_DROPPED_PACKETS, 
                             &value);   
  if (SB_BRCM_RV_ERROR(rv))
  {
    SB_LOG (BVIEW_LOG_ERROR,
                "BST MCAST congestion drops:ASIC(%d) port(%d) cosqGport(0x%x)"
                "Failed to get congestion drop counter\n", asic, port, cosqGport);

    return BVIEW_STATUS_FAILURE;
  }
  *dropCount = value;
  return BVIEW_STATUS_SUCCESS;
}


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
                                                         uint64_t *dropCount)
{
  int rv = 0; 
  SB_BRCM_GPORT_t gport = 0;
  uint64 value;
    
   /*validate ASIC*/
  BVIEW_UNIT_CHECK (asic);

  /* Check validity of input data*/
  if (dropCount == NULL)  
  {
    SB_LOG (BVIEW_LOG_ERROR,
                "BST Total congestion drops:ASIC(%d) port(%d): dropCount(%p) " 
                "is invalid param\n", asic, port, dropCount);
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 

  if (!SB_BRCM_IS_XE_PORT (asic, port))
  {
    SB_LOG (BVIEW_LOG_ERROR,
                "BST Total congestion drops:ASIC(%d): Not a vlid port(%d) \n", 
                 asic, port);
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 
     
  rv = SB_BRCM_API_PORT_GPORT_GET(asic, port , &gport);
  if (SB_BRCM_RV_ERROR(rv))
  {
    SB_LOG (BVIEW_LOG_ERROR,
                "BST Total congestion drops:ASIC(%d) port(%d) "
                "Failed to get gport for port\n", asic, port);

    return BVIEW_STATUS_FAILURE;
  }

  /* Send -1 to collect total of all the queues */
  rv = SB_BRCM_COSQ_STAT_GET(asic, gport, -1, SB_BRCM_COSQ_STAT_DROPPED_PACKETS, 
                             &value);   
  if (SB_BRCM_RV_ERROR(rv))
  {
    SB_LOG (BVIEW_LOG_ERROR,
                "BST Total congestion drops:ASIC(%d) port(%d) "
                "Failed to get congestion drop counter\n", asic, port);

    return BVIEW_STATUS_FAILURE;
  }
  *dropCount = value;
  return BVIEW_STATUS_SUCCESS;
}


