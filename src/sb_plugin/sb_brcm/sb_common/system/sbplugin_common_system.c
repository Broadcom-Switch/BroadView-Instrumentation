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

#include <inttypes.h>
#include <time.h>  
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "feature.h"
#include "sbplugin.h"
#include "system.h"
#include "packet_multiplexer.h"
#include "sbplugin_system.h"
#include "sbplugin_common_system.h"
#include "port_utils.h"
#ifdef FEAT_BHD
#include "sbplugin_common_bhd.h"
#endif
#ifdef FEAT_PT
#include "sbplugin_common_packet_trace.h"
#endif
#include "sbplugin_common.h"
#include "sbplugin_system_map.h"
#include "common/platform_spec.h"
#include "system_utils_crc32.h"

#include "libxml/xmlversion.h"
#include "libxml/parser.h"
#include "libxml/xmlschemas.h"
#include "libxml/xmlschemastypes.h"

/* Array to hold ASIC properties data based for Maximum platforms*/
BVIEW_ASIC_t                 asicDb[BVIEW_MAX_ASICS_ON_A_PLATFORM];
SB_BRCM_PORT_CONFIG_t        sb_brcm_port_config[BVIEW_MAX_ASICS_ON_A_PLATFORM];
BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t sbMaxBufSnapshot[BVIEW_MAX_ASICS_ON_A_PLATFORM];
BVIEW_PACKET_RX_CALLBACK_t   callbackFunc;

SB_SYSTEM_COSQ_GPORTS_MAP_t   cosqGportData[BVIEW_MAX_ASICS_ON_A_PLATFORM];


#define BVIEW_SYSTEM_SETMASKBIT(j, k)                                     \
                ((j).value[((k-1)/(8*sizeof(BVIEW_MASK_BASE_UNIT)))]                \
                            |= 1 << ((k-1) % (8*sizeof(BVIEW_MASK_BASE_UNIT))))

/* Intialize ASIC number Mapping. The Index to table is Applcation ASIC number
 * Application asic numbering starts with '1', so to avoid for loop to get bcm_uint
 * the index to the table is Application asic number.
 */

int  asicMap[BVIEW_MAX_ASICS_ON_A_PLATFORM+1] =
                    /* bcm_asic */
                    /* -------- */
                    {  -1     ,
                        0     };
int  asicAppMap[BVIEW_MAX_ASICS_ON_A_PLATFORM] = {1};

static BVIEW_STATUS extract_device_max_buf(xmlNode *data,
               BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot);

static BVIEW_STATUS extract_i_p_pg_max_buf(xmlNode *data,
               BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot);

static BVIEW_STATUS extract_i_p_sp_max_buf(xmlNode *data,
               BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot);

static BVIEW_STATUS extract_i_sp_max_buf(xmlNode *data,
               BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot);

static BVIEW_STATUS extract_e_p_sp_max_buf(xmlNode *data,
               BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot);

static BVIEW_STATUS extract_e_sp_max_buf(xmlNode *data,
               BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot);

static BVIEW_STATUS extract_e_ucq_max_buf(xmlNode *data,
               BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot);

static BVIEW_STATUS extract_e_ucqg_max_buf(xmlNode *data,
               BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot);

static BVIEW_STATUS extract_e_mcq_max_buf(xmlNode *data,
               BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot);

static BVIEW_STATUS extract_e_cpu_max_buf(xmlNode *data,
               BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot);

static BVIEW_STATUS extract_e_rqe_max_buf(xmlNode *data,
               BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot);

/* Elements of Realm to XML data handler function map */
SB_SYSTEM_REALM_XML_HANDLER_MAP_t realm_xml_handler_map[] = {
    {"realm-device", extract_device_max_buf},
    {"realm-ingress-port-priority-group", extract_i_p_pg_max_buf},
    {"realm-ingress-port-service-pool", extract_i_p_sp_max_buf},
    {"realm-ingress-service-pool", extract_i_sp_max_buf},
    {"realm-egress-port-service-pool", extract_e_p_sp_max_buf},
    {"realm-egress-service-pool", extract_e_sp_max_buf},
    {"realm-egress-uc-queue", extract_e_ucq_max_buf},
    {"realm-egress-uc-queue-group", extract_e_ucqg_max_buf},
    {"realm-egress-mc-queue", extract_e_mcq_max_buf},
    {"realm-egress-cpu-queue", extract_e_cpu_max_buf},
    {"realm-egress-rqe-queue", extract_e_rqe_max_buf}
  };

/*********************************************************************
* @brief  Obtain default max buffers allocated for Device
*
* @param[in]   asic             - unit
* @param[out]  data             - Device data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_SUCCESS           if device max buf get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_system_default_device_max_buf_data_get (int asic, 
                                    BVIEW_SYSTEM_DEVICE_MAX_BUF_DATA_t *data) 
{

 /* Check validity of input data*/
 if (NULL == data)
   return BVIEW_STATUS_INVALID_PARAMETER;

 /*Get the device max buf default values*/
 data->data.maxBuf = SB_BRCM_BST_STAT_ID_DEVICE_DEFAULT; 
 return BVIEW_STATUS_SUCCESS; 
}

/*********************************************************************
* @brief  Obtain Ingress Port + Priority Groups max buf default vals
*
* @param[in]   asic             - unit
* @param[out]  data             - i_p_pg data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if ippg stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if ippg stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_system_default_ippg_max_buf_data_get (int asic, 
                              BVIEW_SYSTEM_INGRESS_PORT_PG_MAX_BUF_DATA_t *data) 
{
  BVIEW_STATUS  rv    = BVIEW_STATUS_SUCCESS;
  unsigned int  port  =0; 
  unsigned int  pg    =0; 

  /* Check validity of input data*/
 if (NULL == data)
   return BVIEW_STATUS_INVALID_PARAMETER;

  /* Loop through all the ports*/
  BVIEW_SYSTEM_PORT_ITER (asic, port)
  {
    /* Loop through all priority groups*/
    BVIEW_SYSTEM_PG_ITER (pg)
    {
      /*BST_Stat for each of the (Ingress Port, PG) UC plus MC 
       * Shared use-counts in units of buffers.
       */
      data->data[port - 1][pg].umShareMaxBuf = SB_BRCM_BST_STAT_ID_PRI_GROUP_SHARED_DEFAULT; 
      
      /* BST_Stat for each of the (Ingress Port, PG) UC plus MC 
       * Headroom use-counts in units of buffers.
       */
      data->data[port - 1][pg].umHeadroomMaxBuf = SB_BRCM_BST_STAT_ID_PRI_GROUP_HEADROOM_DEFAULT;
    } /* for (pg = 0; pg < BVI ....*/
  } /* for (port = 0; port < BVIEW......*/
  return rv;
} 
    
   

/*********************************************************************
* @brief  Obtain Ingress Port + Service Pools max buf
*
* @param[in]   asic             - unit
* @param[out]  data             - i_p_sp data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if ipsp stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if ipsp stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_system_default_ipsp_max_buf_data_get (int asic, 
                                  BVIEW_SYSTEM_INGRESS_PORT_SP_MAX_BUF_DATA_t *data)
{
  unsigned int port =0; 
  unsigned int sp =0;

  /* Check validity of input data*/
  if (NULL == data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  /* Loop through all the ports*/
  BVIEW_SYSTEM_PORT_ITER (asic, port)
  {
    /* BST_Stat for each of the 4 SPs Shared use-counts 
     * associated with this Port in units of buffers.
     */
    BVIEW_SYSTEM_SP_ITER (sp)
    {
      data->data[port - 1][sp].umShareMaxBuf = SB_BRCM_BST_STAT_ID_PORT_POOL_DEFAULT; 
    }
  }
  return BVIEW_STATUS_SUCCESS;
}


/*********************************************************************
* @brief  Obtain Ingress Service Pools max buf
*
* @param[in]   asic             - unit
* @param[out]  data             - i_sp structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if isp stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if isp stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_system_default_isp_max_buf_data_get (int asic, 
                                 BVIEW_SYSTEM_INGRESS_SP_MAX_BUF_DATA_t *data)
{
  int sp = 0;

  /* Check validity of input data*/
  if (NULL == data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  /* BST_Stat for each of the 5 Ingress SPs Shared use-counts in units of buffers*/
  BVIEW_SYSTEM_SP_ITER (sp)
  {
    data->data[sp].umShareMaxBuf = SB_BRCM_BST_STAT_ID_ING_POOL_DEFAULT;
  }
  return BVIEW_STATUS_SUCCESS;
} 

/*********************************************************************
* @brief  Obtain Egress Port + Service Pools max buf
*
* @param[in]   asic             - unit
* @param[out]  data             - e_p_sp data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if epsp stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if epsp stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_system_default_epsp_max_buf_data_get (int asic, 
                                BVIEW_SYSTEM_EGRESS_PORT_SP_MAX_BUF_DATA_t *data)
{
  unsigned int port  =0;
  unsigned int sp =0;

  /* Check validity of input data*/
  if (NULL == data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  /* Loop through all the ports*/
  BVIEW_SYSTEM_PORT_ITER (asic, port)
  {
    BVIEW_SYSTEM_SP_ITER (sp)
    {
      /* Obtain Egress Port + Service Pools max buf - U cast stats*/
      data->data[port - 1][sp].ucShareMaxBuf = SB_BRCM_BST_STAT_ID_EGR_UCAST_PORT_SHARED_DEFAULT;

      /* Obtain Egress Port + Service Pools max buf - Ucast+Mcast cast stats*/
      data->data[port - 1][sp].umShareMaxBuf = SB_BRCM_BST_STAT_ID_EGR_PORT_SHARED_DEFAULT;

      data->data[port - 1][sp].mcShareMaxBuf = SB_BRCM_BST_STAT_ID_EGR_MCAST_PORT_SHARED_DEFAULT;
    }
  }
  return BVIEW_STATUS_SUCCESS;
}
/*********************************************************************
* @brief  Obtain Egress Service Pools max buf
*
* @param[in]   asic             - unit
* @param[out]  data             - e_sp data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if esp stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if esp stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_system_default_esp_max_buf_data_get  (int asic, 
                               BVIEW_SYSTEM_EGRESS_SP_MAX_BUF_DATA_t *data)
{
  unsigned int  sp =0;

  /* Check validity of input data*/
  if (NULL == data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  /* Loop through all the ports*/
  BVIEW_SYSTEM_SP_ITER (sp)
  {
    /* BST_Stat for each of the 4 Egress SPs Shared use-counts in units of buffers.
     * This use-count includes both UC and MC buffers.
     */
    data->data[sp].umShareMaxBuf = SB_BRCM_BST_STAT_ID_EGR_POOL_DEFAULT;

    /*BST_Threshold for each of the 4 Egress SP MC Shared use-counts in units of buffers.*/
    data->data[sp].mcShareMaxBuf = SB_BRCM_BST_STAT_ID_EGR_MCAST_POOL_DEFAULT;
  }
  return BVIEW_STATUS_SUCCESS;
}


/*********************************************************************
* @brief  Obtain Egress Egress Unicast Queues max buf
*
* @param[in]   asic             - unit
* @param[out]  data             - e_uc_q data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if eucq stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if eucq stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_system_default_eucq_max_buf_data_get (int asic, 
                              BVIEW_SYSTEM_EGRESS_UC_QUEUE_MAX_BUF_DATA_t *data)
{
  unsigned int cosq = 0;
  unsigned int port  =0;

  /* Check validity of input data*/
  if (NULL == data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  BVIEW_SYSTEM_PORT_ITER (asic, port)
  {
    /* Iterate COSQ*/
    BVIEW_SYSTEM_ITER (cosq,BVIEW_SYSTEM_NUM_COS_PORT) 
    {
      /*BST_Stat for the UC queue total use-counts in units of buffers.*/
      data->data[((port - 1) * BVIEW_SYSTEM_NUM_COS_PORT) + cosq].ucMaxBuf = SB_BRCM_BST_STAT_ID_UCAST_DEFAULT;
    }
  }
  return BVIEW_STATUS_SUCCESS;
}  
/*********************************************************************
* @brief  Obtain Egress Egress Unicast Queue Groups max buf
*
* @param[in]   asic             - unit
* @param[out]  data             - e_uc_qg data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if eucqg stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if eucqg stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_system_default_eucqg_max_buf_data_get (int asic, 
                        BVIEW_SYSTEM_EGRESS_UC_QUEUEGROUPS_MAX_BUF_DATA_t *data)
{
  unsigned int cosq = 0;

  /* Check validity of input data*/
  if (NULL == data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  /* Loop through all the UC_QUEUE_GROUPS*/
  BVIEW_SYSTEM_ITER (cosq, BVIEW_ASIC_MAX_UC_QUEUE_GROUPS)
  {
    /* BST_Stat for each of the 128 Egress Unicast Queue-Group 
     * Total use-counts in units of buffers.
     */
    data->data[cosq].ucMaxBuf = SB_BRCM_BST_STAT_ID_UCAST_GROUP_DEFAULT;
  }
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Obtain Egress Egress Multicast Queues max buf
*
* @param[in]   asic             - unit
* @param[out]  data             - e_mc_q data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if emcq stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if emcq stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_system_default_emcq_max_buf_data_get (int asic, 
                              BVIEW_SYSTEM_EGRESS_MC_QUEUE_MAX_BUF_DATA_t *data)
{
  unsigned int  cosq =0;
  unsigned int port  =0;

  /* Check validity of input data*/
  if (NULL == data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  /* Loop through all the ports*/
  BVIEW_SYSTEM_PORT_ITER (asic, port)
  {
    /* Loop through cos queue max per port*/
    BVIEW_SYSTEM_ITER (cosq, BVIEW_SYSTEM_NUM_COS_PORT)
    {
      /*BST_Stat for the MC queue total use-counts in units of buffers.*/
      data->data[((port -1) * BVIEW_SYSTEM_NUM_COS_PORT) + cosq].mcMaxBuf = SB_BRCM_BST_STAT_ID_MCAST_DEFAULT;
    }
  }
  return BVIEW_STATUS_SUCCESS;
}


/*********************************************************************
* @brief  Obtain Egress Egress CPU Queues max buf
*
* @param[in]   asic             - unit
* @param[out]  data             - CPU queue data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if CPU stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if CPU stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_system_default_cpuq_max_buf_data_get (int asic, 
                             BVIEW_SYSTEM_EGRESS_CPU_QUEUE_MAX_BUF_DATA_t *data)
{
  unsigned int cosq = 0;

  /* Check validity of input data*/
  if (NULL == data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  /* iterate through Maximum CPU cosqs*/
  BVIEW_SYSTEM_ITER (cosq, BVIEW_ASIC_MAX_CPU_QUEUES)
  {
    /*The BST_Threshold for the Egress CPU queues in units of buffers.*/
    data->data[cosq].cpuMaxBuf = SB_BRCM_BST_STAT_ID_CPU_DEFAULT;
  }
  return BVIEW_STATUS_SUCCESS;
}


/*********************************************************************
* @brief  Obtain Egress Egress RQE Queues max buf 
*
* @param[in]   asic             - unit
* @param[out]  data             - RQE data data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if RQE stat get is failed.
* @retval BVIEW_STATUS_SUCCESS           if RQE stat get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_system_default_rqeq_max_buf_data_get (int asic, 
                                   BVIEW_SYSTEM_EGRESS_RQE_QUEUE_MAX_BUF_DATA_t *data)
{
  unsigned int cosq = 0;

  /* Check validity of input data*/
  if (NULL == data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  /* Loop through all the RQE queues*/
  BVIEW_SYSTEM_ITER (cosq, BVIEW_ASIC_MAX_RQE_QUEUES)
  {
    /* BST_Stat for each of the 11 RQE queues total use-counts in units of buffers.*/
    data->data[cosq].rqeMaxBuf = SB_BRCM_BST_STAT_ID_RQE_QUEUE_DEFAULT;
  }
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Obtain default values of max bufs  
*
* @param[in]   asic             - unit
* @param[out]  snapshot         - Max bufs snapshot data structure
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if max bufs get is failed.
* @retval BVIEW_STATUS_SUCCESS           if max bufs get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_system_max_buf_defaults_get( int asic,
                      BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

  /* Check validity of input data*/
  BVIEW_NULLPTR_CHECK (snapshot); 

  /* Obtain Device max buf */ 
  rv = sbplugin_system_default_device_max_buf_data_get (asic, &snapshot->device);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Ingress Port + Priority Groups max buf */
  rv = sbplugin_system_default_ippg_max_buf_data_get (asic, &snapshot->iPortPg);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }
  /* Obtain Ingress Port + Service Pools max buf */
  rv = sbplugin_system_default_ipsp_max_buf_data_get (asic, &snapshot->iPortSp);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Ingress Service Pools max buf */
  rv = sbplugin_system_default_isp_max_buf_data_get (asic, &snapshot->iSp);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Egress Port + Service Pools max buf */
  rv = sbplugin_system_default_epsp_max_buf_data_get (asic, &snapshot->ePortSp);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Egress Service Pools max buf */
  rv = sbplugin_system_default_esp_max_buf_data_get (asic, &snapshot->eSp);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }
 
  /* Obtain Egress Egress Unicast Queues max buf */
  rv = sbplugin_system_default_eucq_max_buf_data_get (asic, &snapshot->eUcQ);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Egress Egress Unicast Queue Groups max buf */
  rv = sbplugin_system_default_eucqg_max_buf_data_get (asic, &snapshot->eUcQg);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Egress Egress Multicast Queues max buf */
  rv = sbplugin_system_default_emcq_max_buf_data_get (asic, &snapshot->eMcQ);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Egress Egress CPU Queues max buf */
  rv = sbplugin_system_default_cpuq_max_buf_data_get (asic, &snapshot->cpqQ);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Obtain Egress Egress RQE Queues max buf */
  rv = sbplugin_system_default_rqeq_max_buf_data_get (asic, &snapshot->rqeQ);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return BVIEW_STATUS_FAILURE;
  }
  return BVIEW_STATUS_SUCCESS;
}


/*********************************************************************
* @brief  Update snapshot of max bufs with the values in XML file 
*
* @param[in]   unit             - unit 
* @param[in]   root             - root of XML tree
* @param[out]  snapshot         - Snapshot of max bufs
*
* @retval none.
*
* @notes    none
*
*
*********************************************************************/
static void sbplugin_system_max_buf_snapshot_update_from_xml(int unit, 
                       xmlNode *root, BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot)
{
  xmlNode *unit_node = NULL;
  xmlNode *lvl1_node = NULL;
  xmlChar *content   = NULL;
  int index = 0;
  int num_of_realm_handlers = 0; 
  int unit_num = 0;

  num_of_realm_handlers = 
                  (sizeof(realm_xml_handler_map)/sizeof(SB_SYSTEM_REALM_XML_HANDLER_MAP_t));
				  
  /* root is pointing to bstmaxbufs */
  if ( (root == NULL) || (strcmp((char *)root->name, "bstmaxbufs")))
  {
    SB_LOG (BVIEW_LOG_ERROR, "XML is not in the specified format\n");
    return;
  }

  unit_node = root->children;
  
  for (lvl1_node = unit_node; lvl1_node; lvl1_node = lvl1_node->next) 
  {
    if (lvl1_node->type == XML_ELEMENT_NODE) 
    {
      xmlNode *lvl2_node = lvl1_node->children;

      /* Check whether the unit number is correct */
      if (!strcmp((char *)lvl1_node->name, "unit"))
      {
        content = xmlGetProp(lvl1_node, (xmlChar *)"num");
        if (content == NULL)
        {
          continue;
        }
        unit_num = atoi((char *)content);
        xmlFree(content);
        /* unit number did not match, continue to next element */
        if (unit != (unit_num-1))
        {
          continue;
        }
      } 
      else
      {
        continue;
      } /* end of if (!strcmp(lvl1_node->name, "unit" .. */
	  
      for ( ; lvl2_node; lvl2_node = lvl2_node->next) 
      {
        if (lvl2_node->type == XML_ELEMENT_NODE)  
        {
          xmlNode *lvl3_node = lvl2_node->children;

          for (index = 0; index < num_of_realm_handlers; index++)
          {
            if (strcmp(realm_xml_handler_map[index].realm, (char *)lvl2_node->name) == 0) 
            {
              /* Call handler to extract data from xml for this realm */
              realm_xml_handler_map[index].handler(lvl3_node, snapshot);
              break; 
            }
          }
        } 
      }
    }
  } 
}

/*********************************************************************
* @brief  Extract device max buf from xml and update correpsonding 
*         elements in snapshot structure
*
* @param[in]   data             - Pointer to the begining of 
*                                 data nodes in XML tree
* @param[out]  snapshot         - snapshot of maxbufs updated with 
*                                 device data
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if the xml tree data format is wrong.
* @retval BVIEW_STATUS_SUCCESS           if extracting value from xml tree
*                                        and updating snapshot is success.
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS extract_device_max_buf(xmlNode *data,
               BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot)
{
  xmlNode *lvl3_node = NULL;
  xmlNode *lvl4_node = NULL;
  xmlChar *content   = NULL;
  uint64_t device_data_max  = 0; 

  /* Validate input */  	
  BVIEW_NULLPTR_CHECK (data);
  BVIEW_NULLPTR_CHECK (snapshot);  
  
  lvl3_node = data;
  for (; lvl3_node; lvl3_node = lvl3_node->next)    
  {
    /* Check if the node is data node */  
    if (strcmp((char *)lvl3_node->name, "data") != 0)
    {
      continue;
    }
    else
    {
      lvl4_node = lvl3_node->children;
      /* Go through the elements in data node */
      for ( ; lvl4_node; lvl4_node = lvl4_node->next) 
      {
        /* Only handle element nodes */
        if (lvl4_node->type == XML_ELEMENT_NODE) 
        {
          if (!strcmp((char *)lvl4_node->name, "data-max"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
            } 
            device_data_max = strtoull((char *)content,
                                       (char **) NULL, SB_XML_CONTENT_BASE); 
            /*  Set the snapshot*/
            snapshot->device.data.maxBuf = device_data_max;

			/* Free the content */
            xmlFree(content);
          }
        }
      }
    }
  }
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Extract ingress port priority group max buf from xml and update correpsonding 
*         elements in snapshot structure
*
* @param[in]   data             - Pointer to the begining of 
*                                 data nodes in XML tree
* @param[out]  snapshot         - snapshot of maxbufs updated with 
*                                 ingress port priority group
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if the xml tree data format is wrong.
* @retval BVIEW_STATUS_SUCCESS           if extracting value from xml tree
*                                        and updating snapshot is success.
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS extract_i_p_pg_max_buf(xmlNode *data,
               BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot)
{
  xmlNode *lvl3_node = NULL;
  xmlNode *lvl4_node = NULL;
  xmlChar *content   = NULL;
  unsigned int port    = 0;
  unsigned int pg      = 0;
  uint64_t um_s_b_max  = 0;
  uint64_t um_h_b_max       = 0; 
  bool port_set      = false;
  bool pg_set        = false; 
  bool um_s_b_max_set  = false;
  bool um_h_b_max_set  = false;

  /* Validate input */  	
  BVIEW_NULLPTR_CHECK (data);
  BVIEW_NULLPTR_CHECK (snapshot);  
  
  lvl3_node = data;
  for (; lvl3_node; lvl3_node = lvl3_node->next)    
  {
    /* Check if the node is data node */  
    if (strcmp((char *)lvl3_node->name, "data") != 0)
    {
      continue;
    }
    else
    {
      lvl4_node = lvl3_node->children;
      port_set = false;
      pg_set   = false;
      um_s_b_max_set = false;
      um_h_b_max_set = false;

      /* Go through the elements in data node */
      for ( ; lvl4_node; lvl4_node = lvl4_node->next) 
      {
        /* Only handle element nodes */
        if (lvl4_node->type == XML_ELEMENT_NODE) 
        {
          if (!strcmp((char *)lvl4_node->name, "port"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            port = atoi((char *)content); 
            /* Free the content */
            xmlFree(content);
            port_set = true;
          }

          if (!strcmp((char *)lvl4_node->name, "priority-group"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            pg = atoi((char *)content); 
            /* Free the content */
            xmlFree(content);
            pg_set = true;
          }
          if (!strcmp((char *)lvl4_node->name, "um-share-buffer-max"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            um_s_b_max = strtoull((char *)content,
			                       (char **) NULL, SB_XML_CONTENT_BASE);

            /* Free the content */
            xmlFree(content);
            um_s_b_max_set = true;
          }

          if (!strcmp((char *)lvl4_node->name, "um-headroom-buffer-max"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            um_h_b_max = strtoull((char *)content,
			                       (char **) NULL, SB_XML_CONTENT_BASE);
            /* Free the content */
            xmlFree(content);
            um_h_b_max_set = true;
          }
        }
      }
      if ((port_set == true) && (pg_set == true))
      {
        if ((port > BVIEW_ASIC_MAX_PORTS) || (port == 0) ||
            (pg   >= BVIEW_ASIC_MAX_PRIORITY_GROUPS))
        {
            SB_LOG (BVIEW_LOG_ERROR, "SYSTEM: port=%d or pg = %d is out of range\n", port, pg);    
            return BVIEW_STATUS_FAILURE;
        }

        if (true == um_s_b_max_set)
        {
          /*  set snapshot */
          snapshot->iPortPg.data[port-1][pg].umShareMaxBuf = um_s_b_max;
        }
        if (true == um_h_b_max_set)
        {
          /*  set snapshot */
          snapshot->iPortPg.data[port-1][pg].umHeadroomMaxBuf = um_h_b_max;
        }
      }
    }
  }
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief   Get the gport of a unicast cosQ
*
* @param[in]   asic              - unit
* @param[in]   port              - port number
* @param[in]   cosq              - cosq number
* @param[out]  gport             - cosq gport number
*
* @retval BVIEW_STATUS_SUCCESS            if the gport get is successful .
* @retval BVIEW_STATUS_INVALID_PARAMETER  if input parameters are invalid.
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_ucastq_gport_get(int asic, int port, int cosq,
                                             SB_BRCM_GPORT_t *gport)
{
   if ((port < 0) || (port >= BVIEW_ASIC_MAX_PORTS) || 
       (cosq < 0) || (cosq >= BVIEW_SYSTEM_PORT_MAX_UCAST_QUEUES))
   {
      SB_LOG (BVIEW_LOG_ERROR, "SYSTEM: UCAST portNum = %d or cosQ = %d is out of range\n", 
                                                                  port, cosq);    
      return  BVIEW_STATUS_FAILURE;
   }

   *gport = cosqGportData[asic].portInfo[port].ucastGport[cosq];

   return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief   Get the gport of a multicast cosQ
*
* @param[in]   asic              - unit
* @param[in]   port              - port number
* @param[in]   cosq              - cosq number
* @param[out]  gport             - cosq gport number
*
* @retval BVIEW_STATUS_SUCCESS            if the gport get is successful .
* @retval BVIEW_STATUS_INVALID_PARAMETER  if input parameters are invalid.
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_mcastq_gport_get(int asic, int port, int cosq,
                                             SB_BRCM_GPORT_t *gport)
{
   if ((port < 0) || (port >= BVIEW_ASIC_MAX_PORTS) || 
       (cosq < 0) || (cosq >= BVIEW_SYSTEM_PORT_MAX_MCAST_QUEUES))
   {
      SB_LOG (BVIEW_LOG_ERROR, "SYSTEM: MCAST portNum = %d or cosQ = %d is out of range\n", 
                                                                  port, cosq);    
      return  BVIEW_STATUS_FAILURE;
   }

   *gport = cosqGportData[asic].portInfo[port].mcastGport[cosq];
   return BVIEW_STATUS_SUCCESS;
}

static int sbplugin_system_ucast_qid_to_cosq_convert(unsigned int port, unsigned int qId)
{
   return (qId % BVIEW_SYSTEM_PORT_MAX_UCAST_QUEUES);
}

static int sbplugin_system_mcast_qid_to_cosq_convert(unsigned int port, unsigned int qId)
{
   return (qId % BVIEW_SYSTEM_PORT_MAX_MCAST_QUEUES);
}

/*********************************************************************
* @brief  Call back function that is called by traverse function with 
*         with gport and cosq info. This funciton saves the gports locally. 
*          
*
* @param[in]   asic             - asic number 
* @param[in]   port             - port number 
* @param[in]   numq             - Number of queues
* @param[in]   flags            - gport type
* @param[in]   gport            - gport number
* @param[out]  user_data        - pointer to user provided data. 
*
* @retval -1          if the cosq number is out of range.
* @retval  0          if the gport is successfully saved
*
* @notes    none
*
*
*********************************************************************/
static int sbplugin_sytem_cosq_gport_traverse_cb(int asic, SB_BRCM_GPORT_t port,
                                                 int numq, uint32 flags,
                                                 SB_BRCM_GPORT_t gport, void *user_data)
{
   unsigned int portNum;
   unsigned int qId;
   unsigned int cosQnum;
   
   if ((flags & SB_BRCM_COSQ_GPORT_UCAST_QUEUE_GROUP))
   {    
      portNum = SB_BRCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
      qId     = SB_BRCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(gport);
      cosQnum = sbplugin_system_ucast_qid_to_cosq_convert(portNum, qId);
      if ((cosQnum >= BVIEW_SYSTEM_PORT_MAX_UCAST_QUEUES) || (portNum >= BVIEW_ASIC_MAX_PORTS)) 
      {
         SB_LOG (BVIEW_LOG_ERROR, "SYSTEM: UCAST portNum = %d or cosQ = %d is out of range\n", 
                                                                      portNum, cosQnum);    
         return -1;
      }
      cosqGportData[asic].portInfo[portNum].ucastGport[cosQnum] = gport;
      
   }
   else if ((flags & SB_BRCM_COSQ_GPORT_MCAST_QUEUE_GROUP))
   {    
      portNum = SB_BRCM_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
      qId     = SB_BRCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);
      cosQnum = sbplugin_system_mcast_qid_to_cosq_convert(portNum, qId);
      if ((cosQnum >= BVIEW_SYSTEM_PORT_MAX_MCAST_QUEUES) || (portNum >= BVIEW_ASIC_MAX_PORTS)) 
      {
         SB_LOG (BVIEW_LOG_ERROR, "SYSTEM: MCAST portNum = %d or cosQ = %d is out of range\n", 
                                                                      portNum, cosQnum);    
         return -1;
      }
      cosqGportData[asic].portInfo[portNum].mcastGport[cosQnum] = gport;
   }
   return 0;
}

/*********************************************************************
* @brief   Update cosq gports information from SDK to local buff  
*
* @param[in]   unit              - unit
*
* @retval none
*
*
* @notes    none
*
*
*********************************************************************/
static void sbplugin_system_cosq_gport_info_update(int unit)
{
   int  dummy;
   void *user_data = &dummy;

   SB_BRCM_COSQ_GPORT_TRAVERSE(unit, sbplugin_sytem_cosq_gport_traverse_cb, user_data);
   return; 
} 
/*********************************************************************
* @brief  Extract ingress port serive pool max buf from xml and update correpsonding 
*         elements in snapshot structure
*
* @param[in]   data             - Pointer to the begining of 
*                                 data nodes in XML tree
* @param[out]  snapshot         - snapshot of maxbufs updated with 
*                                 ingress port serive pool data
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if the xml tree data format is wrong.
* @retval BVIEW_STATUS_SUCCESS           if extracting value from xml tree
*                                        and updating snapshot is success.
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS extract_i_p_sp_max_buf(xmlNode *data,
               BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot)
{
  xmlNode *lvl3_node = NULL;
  xmlNode *lvl4_node = NULL;
  xmlChar *content   = NULL;
  unsigned int port  = 0;
  unsigned int sp    = 0;
  uint64_t um_s_b_max= 0;
  bool port_set      = false;
  bool sp_set        = false; 
  bool um_s_b_max_set  = false;

  /* Validate input */  	
  BVIEW_NULLPTR_CHECK (data);
  BVIEW_NULLPTR_CHECK (snapshot);  
  
  lvl3_node = data;
  for (; lvl3_node; lvl3_node = lvl3_node->next)    
  {
    /* Check if the node is data node */  
    if (strcmp((char *)lvl3_node->name, "data") != 0)
    {
      continue;
    }
    else
    {
      lvl4_node = lvl3_node->children;
      port_set = false;
      sp_set   = false;
      um_s_b_max_set = false;

      /* Go through the elements in data node */
      for ( ; lvl4_node; lvl4_node = lvl4_node->next) 
      {
        /* Only handle element nodes */
        if (lvl4_node->type == XML_ELEMENT_NODE) 
        {
          if (!strcmp((char *)lvl4_node->name, "port"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            port = atoi((char *)content); 
            /* Free the content */
            xmlFree(content);
            port_set = true;
          }

          if (!strcmp((char *)lvl4_node->name, "service-pool"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            sp = atoi((char *)content); 
            /* Free the content */
            xmlFree(content);
            sp_set = true;
          }
          if (!strcmp((char *)lvl4_node->name, "um-share-buffer-max"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            um_s_b_max = strtoull((char *)content,
			                       (char **) NULL, SB_XML_CONTENT_BASE);

            /* Free the content */
            xmlFree(content);
            um_s_b_max_set = true;
          }
        }
      }
      if ((port_set == true) && (sp_set == true))
      {
        if ((port > BVIEW_ASIC_MAX_PORTS) || (port == 0) ||
            (sp   >= BVIEW_ASIC_MAX_INGRESS_SERVICE_POOLS))
        {
            SB_LOG (BVIEW_LOG_ERROR, "SYSTEM: port=%d or sp = %d is out of range\n", port, sp);    
            return BVIEW_STATUS_FAILURE;
        }
        if (true == um_s_b_max_set)
        {
          /*  set snapshot */
          snapshot->iPortSp.data[port-1][sp].umShareMaxBuf = um_s_b_max; 
        }
      }
    }
  }
  return BVIEW_STATUS_SUCCESS;
}


/*********************************************************************
* @brief  Extract ingress serive pool max buf from xml and update correpsonding 
*         elements in snapshot structure
*
* @param[in]   data             - Pointer to the begining of 
*                                 data nodes in XML tree
* @param[out]  snapshot         - snapshot of maxbufs updated with 
*                                 ingress serive pool data
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if the xml tree data format is wrong.
* @retval BVIEW_STATUS_SUCCESS           if extracting value from xml tree
*                                        and updating snapshot is success.
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS extract_i_sp_max_buf(xmlNode *data,
                    BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot)
{
  xmlNode *lvl3_node = NULL;
  xmlNode *lvl4_node = NULL;
  xmlChar *content   = NULL;
  unsigned int sp     = 0;
  uint64_t um_s_b_max = 0;
  bool sp_set        = false; 
  bool um_s_b_max_set  = false;

  /* Validate input */  	
  BVIEW_NULLPTR_CHECK (data);
  BVIEW_NULLPTR_CHECK (snapshot);  
  
  lvl3_node = data;
  for (; lvl3_node; lvl3_node = lvl3_node->next)    
  {
    /* Check if the node is data node */  
    if (strcmp((char *)lvl3_node->name, "data") != 0)
    {
      continue;
    }
    else
    {
      lvl4_node = lvl3_node->children;
      sp_set   = false;
      um_s_b_max_set = false;

      /* Go through the elements in data node */
      for ( ; lvl4_node; lvl4_node = lvl4_node->next) 
      {
        /* Only handle element nodes */
        if (lvl4_node->type == XML_ELEMENT_NODE) 
        {

          if (!strcmp((char *)lvl4_node->name, "service-pool"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            sp = atoi((char *)content); 
            /* Free the content */
            xmlFree(content);
            sp_set = true;
          }
          if (!strcmp((char *)lvl4_node->name, "um-share-buffer-max"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            um_s_b_max = strtoull((char *)content,
			                       (char **) NULL, SB_XML_CONTENT_BASE);

            /* Free the content */
            xmlFree(content);
            um_s_b_max_set = true;
          }
        }
      }
      if (true == sp_set)
      {
        if ((sp >= BVIEW_ASIC_MAX_INGRESS_SERVICE_POOLS))
        {
            SB_LOG (BVIEW_LOG_ERROR, "SYSTEM: sp = %d is out of range\n",  sp);    
            return BVIEW_STATUS_FAILURE;
        }
        if (true == um_s_b_max_set)
        {
          /*  set snapshot */
          snapshot->iSp.data[sp].umShareMaxBuf = um_s_b_max;
        }
      }
    }
  }
  return BVIEW_STATUS_SUCCESS;
}


/*********************************************************************
* @brief  Extract egress port serive pool max buf from xml and update correpsonding 
*         elements in snapshot structure
*
* @param[in]   data             - Pointer to the begining of 
*                                 data nodes in XML tree
* @param[out]  snapshot         - snapshot of maxbufs updated with 
*                                 egress port serive pool data
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if the xml tree data format is wrong.
* @retval BVIEW_STATUS_SUCCESS           if extracting value from xml tree
*                                        and updating snapshot is success.
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS extract_e_p_sp_max_buf(xmlNode *data,
               BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot)
{
  xmlNode *lvl3_node = NULL;
  xmlNode *lvl4_node = NULL;
  xmlChar *content   = NULL;
  unsigned int port           = 0;
  unsigned int sp             = 0;
  uint64_t um_s_b_max     = 0;
  uint64_t uc_s_b_max     = 0;
  uint64_t mc_s_b_max     = 0;
  bool port_set      = false;
  bool sp_set        = false; 
  bool um_s_b_max_set  = false;
  bool uc_s_b_max_set  = false;
  bool mc_s_b_max_set  = false;

  /* Validate input */  	
  BVIEW_NULLPTR_CHECK (data);
  BVIEW_NULLPTR_CHECK (snapshot);  
  
  lvl3_node = data;
  for (; lvl3_node; lvl3_node = lvl3_node->next)    
  {
    /* Check if the node is data node */  
    if (strcmp((char *)lvl3_node->name, "data") != 0)
    {
      continue;
    }
    else
    {
      lvl4_node = lvl3_node->children;
      port_set = false;
      sp_set   = false;
      um_s_b_max_set = false;
      uc_s_b_max_set = false;
      mc_s_b_max_set = false;

      /* Go through the elements in data node */
      for ( ; lvl4_node; lvl4_node = lvl4_node->next) 
      {
        /* Only handle element nodes */
        if (lvl4_node->type == XML_ELEMENT_NODE) 
        {
          if (!strcmp((char *)lvl4_node->name, "port"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            port = atoi((char *)content); 
            /* Free the content */
            xmlFree(content);
            port_set = true;
          }

          if (!strcmp((char *)lvl4_node->name, "service-pool"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            sp = atoi((char *)content); 
            /* Free the content */
            xmlFree(content);
            sp_set = true;
          }
          if (!strcmp((char *)lvl4_node->name, "um-share-buffer-max"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            um_s_b_max = strtoull((char *)content,
			                       (char **) NULL, SB_XML_CONTENT_BASE);

            /* Free the content */
            xmlFree(content);
            um_s_b_max_set = true;
          }
          if (!strcmp((char *)lvl4_node->name, "uc-share-buffer-max"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            uc_s_b_max = strtoull((char *)content,
			                       (char **) NULL, SB_XML_CONTENT_BASE);

            /* Free the content */
            xmlFree(content);
            uc_s_b_max_set = true;
          }
          if (!strcmp((char *)lvl4_node->name, "mc-share-buffer-max"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            mc_s_b_max = strtoull((char *)content,
			                       (char **) NULL, SB_XML_CONTENT_BASE);

            /* Free the content */
            xmlFree(content);
            mc_s_b_max_set = true;
          }
        }
      }
      if ((port_set == true) && (sp_set == true))
      {
        if ((port > BVIEW_ASIC_MAX_PORTS) || (port == 0) ||
            (sp   >= BVIEW_ASIC_MAX_INGRESS_SERVICE_POOLS))
        {
            SB_LOG (BVIEW_LOG_ERROR, "SYSTEM: port=%d or sp = %d is out of range\n", port, sp);    
            return BVIEW_STATUS_FAILURE;
        }
        if (true == um_s_b_max_set)
        {
          /*  set snapshot */
          snapshot->ePortSp.data[port-1][sp].umShareMaxBuf = um_s_b_max;  
        }
        if (true == uc_s_b_max_set)
        {
          /*  set snapshot */
          snapshot->ePortSp.data[port-1][sp].ucShareMaxBuf = uc_s_b_max;  
        }
        if (true == mc_s_b_max_set)
        {
          /*  set snapshot */
          snapshot->ePortSp.data[port-1][sp].mcShareMaxBuf = mc_s_b_max;  
        }
      }
    }
  }
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Extract egress serive pool max buf from xml and update correpsonding 
*         elements in snapshot structure
*
* @param[in]   data             - Pointer to the begining of 
*                                 data nodes in XML tree
* @param[out]  snapshot         - snapshot of maxbufs updated with 
*                                 egress serive pool data
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if the xml tree data format is wrong.
* @retval BVIEW_STATUS_SUCCESS           if extracting value from xml tree
*                                        and updating snapshot is success.
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS extract_e_sp_max_buf(xmlNode *data,
                    BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot)
{
  xmlNode *lvl3_node = NULL;
  xmlNode *lvl4_node = NULL;
  xmlChar *content   = NULL;
  unsigned int sp         = 0;
  uint64_t um_s_b_max     = 0;
  uint64_t mc_s_b_max     = 0;
  bool sp_set        = false; 
  bool um_s_b_max_set  = false;
  bool mc_s_b_max_set  = false;

  /* Validate input */  	
  BVIEW_NULLPTR_CHECK (data);
  BVIEW_NULLPTR_CHECK (snapshot);  
  
  lvl3_node = data;
  for (; lvl3_node; lvl3_node = lvl3_node->next)    
  {
    /* Check if the node is data node */  
    if (strcmp((char *)lvl3_node->name, "data") != 0)
    {
      continue;
    }
    else
    {
      lvl4_node = lvl3_node->children;
      sp_set = false;
      um_s_b_max_set = false;
      mc_s_b_max_set = false;

      /* Go through the elements in data node */
      for ( ; lvl4_node; lvl4_node = lvl4_node->next) 
      {
        /* Only handle element nodes */
        if (lvl4_node->type == XML_ELEMENT_NODE) 
        {

          if (!strcmp((char *)lvl4_node->name, "service-pool"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            sp = atoi((char *)content); 
            /* Free the content */
            xmlFree(content);
            sp_set = true;
          }
          if (!strcmp((char *)lvl4_node->name, "um-share-buffer-max"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            um_s_b_max = strtoull((char *)content,
			                       (char **) NULL, SB_XML_CONTENT_BASE);

            /* Free the content */
            xmlFree(content);
            um_s_b_max_set = true;
          }
          if (!strcmp((char *)lvl4_node->name, "mc-share-buffer-max"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            mc_s_b_max = strtoull((char *)content,
			                       (char **) NULL, SB_XML_CONTENT_BASE);

            /* Free the content */
            xmlFree(content);
            mc_s_b_max_set = true;
          }
        }
      }
      if (true == sp_set)
      {
        if ((sp >= BVIEW_ASIC_MAX_INGRESS_SERVICE_POOLS))
        {
            SB_LOG (BVIEW_LOG_ERROR, "SYSTEM: sp = %d is out of range\n",  sp);    
            return BVIEW_STATUS_FAILURE;
        }
        if (true == um_s_b_max_set)
        {
          /*  set snapshot */
          snapshot->eSp.data[sp].umShareMaxBuf = um_s_b_max;  
        }
        if (true == mc_s_b_max_set)
        {
          /*  set snapshot */
          snapshot->eSp.data[sp].mcShareMaxBuf = mc_s_b_max;  
        }
      }
    }
  }
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Extract egress unicast queues max buf from xml and update correpsonding 
*         elements in snapshot structure
*
* @param[in]   data             - Pointer to the begining of 
*                                 data nodes in XML tree
* @param[out]  snapshot         - snapshot of maxbufs updated with 
*                                 egress unicast queues data
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if the xml tree data format is wrong.
* @retval BVIEW_STATUS_SUCCESS           if extracting value from xml tree
*                                        and updating snapshot is success.
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS extract_e_ucq_max_buf(xmlNode *data,
                    BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot)
{
  xmlNode *lvl3_node = NULL;
  xmlNode *lvl4_node = NULL;
  xmlChar *content   = NULL;
  unsigned int queue      = 0;
  uint64_t uc_b_max       = 0;
  bool queue_set     = false; 
  bool uc_b_max_set  = false;

  /* Validate input */  	
  BVIEW_NULLPTR_CHECK (data);
  BVIEW_NULLPTR_CHECK (snapshot);  
  
  lvl3_node = data;
  for (; lvl3_node; lvl3_node = lvl3_node->next)    
  {
    /* Check if the node is data node */  
    if (strcmp((char *)lvl3_node->name, "data") != 0)
    {
      continue;
    }
    else
    {
      lvl4_node = lvl3_node->children;
      queue_set = false;
      uc_b_max_set = false;

      /* Go through the elements in data node */
      for ( ; lvl4_node; lvl4_node = lvl4_node->next) 
      {
        /* Only handle element nodes */
        if (lvl4_node->type == XML_ELEMENT_NODE) 
        {

          if (!strcmp((char *)lvl4_node->name, "queue"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            queue = atoi((char *)content); 
            /* Free the content */
            xmlFree(content);
            queue_set = true;
          }
          if (!strcmp((char *)lvl4_node->name, "uc-buffer-max"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            uc_b_max = strtoull((char *)content,
			                       (char **) NULL, SB_XML_CONTENT_BASE);

            /* Free the content */
            xmlFree(content);
            uc_b_max_set = true;
          }
        }
      }
      if (true == queue_set)
      {
        if ((queue >= BVIEW_ASIC_MAX_UC_QUEUES))
        {
            SB_LOG (BVIEW_LOG_ERROR, "SYSTEM: queue = %d is out of range\n", queue);    
            return BVIEW_STATUS_FAILURE;
        }
        if (true == uc_b_max_set)
        {
          /*  set snapshot */
          snapshot->eUcQ.data[queue].ucMaxBuf = uc_b_max;
        }
      }
    }
  }
  return BVIEW_STATUS_SUCCESS;
}


/*********************************************************************
* @brief  Extract egress unicast queue groups max buf from xml and update correpsonding 
*         elements in snapshot structure
*
* @param[in]   data             - Pointer to the begining of 
*                                 data nodes in XML tree
* @param[out]  snapshot         - snapshot of maxbufs updated with 
*                                 egress unicast queue groups data
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if the xml tree data format is wrong.
* @retval BVIEW_STATUS_SUCCESS           if extracting value from xml tree
*                                        and updating snapshot is success.
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS extract_e_ucqg_max_buf(xmlNode *data,
                    BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot)
{
  xmlNode *lvl3_node = NULL;
  xmlNode *lvl4_node = NULL;
  xmlChar *content   = NULL;
  unsigned int qg      = 0;
  uint64_t uc_b_max    = 0;
  bool qg_set     = false; 
  bool uc_b_max_set  = false;

  /* Validate input */  	
  BVIEW_NULLPTR_CHECK (data);
  BVIEW_NULLPTR_CHECK (snapshot);  
  
  lvl3_node = data;
  for (; lvl3_node; lvl3_node = lvl3_node->next)    
  {
    /* Check if the node is data node */  
    if (strcmp((char *)lvl3_node->name, "data") != 0)
    {
      continue;
    }
    else
    {
      lvl4_node = lvl3_node->children;
      qg_set = false;
      uc_b_max_set = false;

      /* Go through the elements in data node */
      for ( ; lvl4_node; lvl4_node = lvl4_node->next) 
      {
        /* Only handle element nodes */
        if (lvl4_node->type == XML_ELEMENT_NODE) 
        {

          if (!strcmp((char *)lvl4_node->name, "queue-group"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            qg = atoi((char *)content); 
            /* Free the content */
            xmlFree(content);
            qg_set = true;
          }
          if (!strcmp((char *)lvl4_node->name, "uc-buffer-max"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            uc_b_max = strtoull((char *)content,
			                       (char **) NULL, SB_XML_CONTENT_BASE);

            /* Free the content */
            xmlFree(content);
            uc_b_max_set = true;
          }
        }
      }
      if (true == qg_set)
      {
        if ((qg >= BVIEW_ASIC_MAX_UC_QUEUE_GROUPS))
        {
            SB_LOG (BVIEW_LOG_ERROR, "SYSTEM: queue group = %d is out of range\n", qg);    
            return BVIEW_STATUS_FAILURE;
        }
        if (true == uc_b_max_set)
        {
          /*  set snapshot */
          snapshot->eUcQg.data[qg].ucMaxBuf = uc_b_max;
        }
      }
    }
  }
  return BVIEW_STATUS_SUCCESS;
}


/*********************************************************************
* @brief  Extract egress multicast queue max buf from xml and update correpsonding 
*         elements in snapshot structure
*
* @param[in]   data             - Pointer to the begining of 
*                                 data nodes in XML tree
* @param[out]  snapshot         - snapshot of maxbufs updated with 
*                                 egress multicast queue data
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if the xml tree data format is wrong.
* @retval BVIEW_STATUS_SUCCESS           if extracting value from xml tree
*                                        and updating snapshot is success.
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS extract_e_mcq_max_buf(xmlNode *data,
                    BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot)
{
  xmlNode *lvl3_node = NULL;
  xmlNode *lvl4_node = NULL;
  xmlChar *content   = NULL;
  unsigned int queue = 0;
  uint64_t mc_b_max  = 0;
  bool queue_set     = false; 
  bool mc_b_max_set  = false;

  /* Validate input */  	
  BVIEW_NULLPTR_CHECK (data);
  BVIEW_NULLPTR_CHECK (snapshot);  
  
  lvl3_node = data;
  for (; lvl3_node; lvl3_node = lvl3_node->next)    
  {
    /* Check if the node is data node */  
    if (strcmp((char *)lvl3_node->name, "data") != 0)
    {
      continue;
    }
    else
    {
      lvl4_node = lvl3_node->children;
      queue_set = false;
      mc_b_max_set = false;

      /* Go through the elements in data node */
      for ( ; lvl4_node; lvl4_node = lvl4_node->next) 
      {
        /* Only handle element nodes */
        if (lvl4_node->type == XML_ELEMENT_NODE) 
        {

          if (!strcmp((char *)lvl4_node->name, "queue"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            queue = atoi((char *)content); 
            /* Free the content */
            xmlFree(content);
            queue_set = true;
          }
          if (!strcmp((char *)lvl4_node->name, "mc-buffer-max"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            mc_b_max = strtoull((char *)content,
			                       (char **) NULL, SB_XML_CONTENT_BASE);

            /* Free the content */
            xmlFree(content);
            mc_b_max_set = true;
          }
        }
      }
      if (true == queue_set)
      {
        if ((queue >= BVIEW_ASIC_MAX_MC_QUEUES))
        {
            SB_LOG (BVIEW_LOG_ERROR, "SYSTEM: queue = %d is out of range\n", queue);    
            return BVIEW_STATUS_FAILURE;
        }
        if (true == mc_b_max_set)
        {
          /*  set snapshot */
          snapshot->eMcQ.data[queue].mcMaxBuf = mc_b_max;
        }
      }
    }
  }
  return BVIEW_STATUS_SUCCESS;
}


/*********************************************************************
* @brief  Extract CPU queues max buf from xml and update correpsonding 
*         elements in snapshot structure
*
* @param[in]   data             - Pointer to the begining of 
*                                 data nodes in XML tree
* @param[out]  snapshot         - snapshot of maxbufs updated with 
*                                 CPU queues data
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if the xml tree data format is wrong.
* @retval BVIEW_STATUS_SUCCESS           if extracting value from xml tree
*                                        and updating snapshot is success.
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS extract_e_cpu_max_buf(xmlNode *data,
                    BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot)
{
  xmlNode *lvl3_node = NULL;
  xmlNode *lvl4_node = NULL;
  xmlChar *content   = NULL;
  unsigned int queue  = 0;
  uint64_t cpu_b_max  = 0;
  bool queue_set     = false; 
  bool cpu_b_max_set  = false;

  /* Validate input */  	
  BVIEW_NULLPTR_CHECK (data);
  BVIEW_NULLPTR_CHECK (snapshot);  
  
  lvl3_node = data;
  for (; lvl3_node; lvl3_node = lvl3_node->next)    
  {
    /* Check if the node is data node */  
    if (strcmp((char *)lvl3_node->name, "data") != 0)
    {
      continue;
    }
    else
    {
      lvl4_node = lvl3_node->children;
      queue_set = false;
      cpu_b_max_set = false;

      /* Go through the elements in data node */
      for ( ; lvl4_node; lvl4_node = lvl4_node->next) 
      {
        /* Only handle element nodes */
        if (lvl4_node->type == XML_ELEMENT_NODE) 
        {

          if (!strcmp((char *)lvl4_node->name, "queue"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            queue = atoi((char *)content); 
            /* Free the content */
            xmlFree(content);
            queue_set = true;
          }
          if (!strcmp((char *)lvl4_node->name, "cpu-buffer-max"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            cpu_b_max = strtoull((char *)content,
			                       (char **) NULL, SB_XML_CONTENT_BASE);

            /* Free the content */
            xmlFree(content);
            cpu_b_max_set = true;
          }
        }
      }
      if (true == queue_set)
      {
        if ((queue >= BVIEW_ASIC_MAX_CPU_QUEUES))
        {
            SB_LOG (BVIEW_LOG_ERROR, "SYSTEM: queue = %d is out of range\n", queue);    
            return BVIEW_STATUS_FAILURE;
        }
        if (true == cpu_b_max_set)
        {
          /*  set snapshot */
          snapshot->cpqQ.data[queue].cpuMaxBuf = cpu_b_max; 
        }
      }
    }
  }
  return BVIEW_STATUS_SUCCESS;
}



/*********************************************************************
* @brief  Extract RQE queues max buf from xml and update correpsonding 
*         elements in snapshot structure
*
* @param[in]   data             - Pointer to the begining of 
*                                 data nodes in XML tree
* @param[out]  snapshot         - snapshot of maxbufs updated with 
*                                 RQE queues data
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if the xml tree data format is wrong.
* @retval BVIEW_STATUS_SUCCESS           if extracting value from xml tree
*                                        and updating snapshot is success.
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS extract_e_rqe_max_buf(xmlNode *data,
                    BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot)
{
  xmlNode *lvl3_node = NULL;
  xmlNode *lvl4_node = NULL;
  xmlChar *content   = NULL;
  unsigned int queue  = 0;
  uint64_t rqe_b_max  = 0;
  bool queue_set     = false; 
  bool rqe_b_max_set  = false;

  /* Validate input */  	
  BVIEW_NULLPTR_CHECK (data);
  BVIEW_NULLPTR_CHECK (snapshot);  
  
  lvl3_node = data;
  for (; lvl3_node; lvl3_node = lvl3_node->next)    
  {
    /* Check if the node is data node */  
    if (strcmp((char *)lvl3_node->name, "data") != 0)
    {
      continue;
    }
    else
    {
      lvl4_node = lvl3_node->children;
      queue_set = false;
      rqe_b_max_set = false;

      /* Go through the elements in data node */
      for ( ; lvl4_node; lvl4_node = lvl4_node->next) 
      {
        /* Only handle element nodes */
        if (lvl4_node->type == XML_ELEMENT_NODE) 
        {

          if (!strcmp((char *)lvl4_node->name, "queue"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            queue = atoi((char *)content); 
            /* Free the content */
            xmlFree(content);
            queue_set = true;
          }
          if (!strcmp((char *)lvl4_node->name, "rqe-buffer-max"))
          {
            /* Get content for the element */
            content = xmlNodeGetContent(lvl4_node);
            if (content == NULL)
            {
              continue;
			} 
            rqe_b_max = strtoull((char *)content,
			                       (char **) NULL, SB_XML_CONTENT_BASE);

            /* Free the content */
            xmlFree(content);
            rqe_b_max_set = true;
          }
        }
      }
      if (true == queue_set)
      {
        if ((queue >= BVIEW_ASIC_MAX_RQE_QUEUES))
        {
            SB_LOG (BVIEW_LOG_ERROR, "SYSTEM: queue = %d is out of range\n", queue);    
            return BVIEW_STATUS_FAILURE;
        }
        if (true == rqe_b_max_set)
        {
          /* set snapshot */
          snapshot->rqeQ.data[queue].rqeMaxBuf =  rqe_b_max;
        }
      }
    }
  }
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief   Update system max bufs with the values provided in XML file 
*
* @param[in]   unit              - unit number for which the snapshot needs 
* @param[in]   xmlFileName       - XML file name
* @param[in]   XSDFileName       - XML schema definition file name
* @param[out]  snapshot          - snapshot structure that holds the maxbufs 
*
* @retval none.
*
*
* @notes    none
*
*
*********************************************************************/
static void sbplugin_system_max_buf_get_from_xml(int unit, char *XMLFileName, 
                                                 char *XSDFileName, 
                          BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *snapshot)
{
  xmlNode *root_element = NULL;
  xmlDoc *doc = NULL;
  xmlSchemaPtr schema = NULL;
  xmlSchemaParserCtxtPtr ctxt = NULL;

  xmlLineNumbersDefault(1);

  ctxt = xmlSchemaNewParserCtxt(XSDFileName);

  if (ctxt == NULL)
  {
    SB_LOG (BVIEW_LOG_ERROR,"Failed to get new parser context\n");
    goto free_resources;
  }
   
  xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, 
                           (xmlSchemaValidityWarningFunc) fprintf, stderr);

  schema = xmlSchemaParse(ctxt);
  xmlSchemaFreeParserCtxt(ctxt);
  /*
   * this initialize the library and check potential ABI mismatches
   * between the version it was compiled for and the actual shared
   * library used.
   */
  LIBXML_TEST_VERSION

  /*parse the file and get the DOM */
  doc = xmlReadFile(XMLFileName, NULL, 0);

  if (doc == NULL) {
    SB_LOG (BVIEW_LOG_ERROR,"Could not find/parse xml file %s\n", XMLFileName);
    goto free_resources;
  }
  else
  {
    xmlSchemaValidCtxtPtr ctxt;
    int ret;

    ctxt = xmlSchemaNewValidCtxt(schema);
    xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    ret = xmlSchemaValidateDoc(ctxt, doc);
    if (ret == 0)
    {
      SB_LOG (BVIEW_LOG_INFO, "%s validates\n", XMLFileName);
      /*Get the root element node */
      root_element = xmlDocGetRootElement(doc);
      sbplugin_system_max_buf_snapshot_update_from_xml(unit, root_element, snapshot);
    }
    else if (ret > 0)
    {
      SB_LOG (BVIEW_LOG_ERROR, "%s fails to validate\n", XMLFileName);
    }
    else
    {
      SB_LOG (BVIEW_LOG_ERROR, "%s validation generated an internal error\n", XMLFileName);
    }

    xmlSchemaFreeValidCtxt(ctxt);
    /*free the document */
    xmlFreeDoc(doc);
  }

  free_resources:
  /* free the resource */
  if(schema != NULL)
  {
    xmlSchemaFree(schema);
  }

  xmlSchemaCleanupTypes();
  /*
   *Free the global variables that may
   *have been allocated by the parser.
   */
  xmlCleanupParser();
}

/*********************************************************************
* @brief   Update system max bufs either with default values or 
*           values provided in XML file
*
* @param[in]   unit              - unit
*
* @retval none
*
*
* @notes    none
*
*
*********************************************************************/
static void sbplugin_system_max_buf_update(int unit)
{
  
  FILE *maxBufXMLFile = NULL;
  FILE *maxBufXMLSchemaFile = NULL;

  /* Update max bufs with default values */ 
  sbplugin_system_max_buf_defaults_get(unit, &sbMaxBufSnapshot[unit]);  
  
  /* Check if the XML file exists */
  maxBufXMLFile = fopen(BST_MAX_BUF_CFG_FILE_NAME, "r");

  if (maxBufXMLFile == NULL)
  {
     SB_LOG (BVIEW_LOG_INFO,
               "SYSTEM: Failed to open XML file %s", BST_MAX_BUF_CFG_FILE_NAME);
     return ;
  }
  fclose(maxBufXMLFile);

  /* Check if the XML file exists */
  maxBufXMLSchemaFile = fopen(BST_MAX_BUF_CFG_SCHEMA_FILE_NAME, "r");

  if (maxBufXMLSchemaFile == NULL)
  {
     SB_LOG (BVIEW_LOG_INFO,
               "SYSTEM: Failed to open XML Schema file %s", BST_MAX_BUF_CFG_SCHEMA_FILE_NAME);
     return ;
  }
  fclose(maxBufXMLSchemaFile);

  /* Update max bufs with the values in xml file */
  sbplugin_system_max_buf_get_from_xml(unit, BST_MAX_BUF_CFG_FILE_NAME, 
                                       BST_MAX_BUF_CFG_SCHEMA_FILE_NAME, &sbMaxBufSnapshot[unit]);  
}

/*********************************************************************
* @brief   Get the chip type
*
* @param[in]   unit              - unit
* @param[out]  asicType          - ASIC type
*
* @retval BVIEW_STATUS_SUCCESS            if chip get is success.
* @retval BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_util_chip_type_get (unsigned int unit,
                                                BVIEW_ASIC_TYPE *asicType)
{
  SB_BRCM_UNIT_INFO_t info;

  if (asicType == NULL)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  if(SB_BRCM_RV_ERROR(SB_BRCM_API_UNIT_INFO_GET(unit, &info)))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
  if(SB_BRCM_IS_CHIP_TD2(info) || SB_BRCM_IS_CHIP_TD2_PLUS(info))
  {
    *asicType = BVIEW_ASIC_TYPE_TD2;
  }
  else if (SB_BRCM_IS_CHIP_TH(info))
  {
    *asicType = BVIEW_ASIC_TYPE_TH;
  }
  else if (SB_BRCM_IS_CHIP_QUMRAN(info))
  {
    *asicType = BVIEW_ASIC_TYPE_QUMRAN;
  }
  else 
  {
    return BVIEW_STATUS_FAILURE;
  }
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Init TD2 scaling parametrs.
*
* @param [in,out]  asic               - ASIC specific info data
*
* @retval BVIEW_STATUS_SUCCESS            if init is success.
* @retval BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
*
* @notes    none
*
*
*********************************************************************/
static BVIEW_STATUS sbplugin_common_system_util_td2_init (BVIEW_ASIC_t *asic)
{
  if (asic == NULL)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  asic->scalingParams.numUnicastQueues = BVIEW_TD2_NUM_UC_QUEUE;
  asic->scalingParams.numUnicastQueueGroups = BVIEW_TD2_NUM_UC_QUEUE_GRP;
  asic->scalingParams.numMulticastQueues = BVIEW_TD2_NUM_MC_QUEUE;
  asic->scalingParams.numServicePools = BVIEW_TD2_NUM_SP;
  asic->scalingParams.numCommonPools = BVIEW_TD2_NUM_COMMON_SP;
  asic->scalingParams.numCpuQueues = BVIEW_TD2_CPU_COSQ;
  asic->scalingParams.numRqeQueues = BVIEW_TD2_NUM_RQE;
  asic->scalingParams.numRqeQueuePools = BVIEW_TD2_NUM_RQE_POOL;
  asic->scalingParams.numPriorityGroups = BVIEW_TD2_NUM_PG;
  asic->scalingParams.cellToByteConv = BVIEW_TD2_CELL_TO_BYTE;

  asic->scalingParams.support1588 = BVIEW_TD2_1588_SUPPORT;

  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  SYSTEM feature init
*
* @param[in,out]   bcmSystem   -  system data structure
*
* @returns BVIEW_STATUS_SUCCESS  if intialization is success
*          BVIEW_STATUS_FAILURE  if intialization is fail
*
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_init (BVIEW_SB_SYSTEM_FEATURE_t    *bcmSystem)
{
  unsigned int     num_front_panel_ports = 0;
  unsigned int     index = 0;
  unsigned int     unit = 0;
  unsigned int     num_ports = 0;
  unsigned int     max_units = 0;
  SB_BRCM_PORT_CONFIG_t port_cfg;

  if (bcmSystem == NULL)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  memset (bcmSystem, 0x00, sizeof (BVIEW_SB_SYSTEM_FEATURE_t));
  bcmSystem->feature.featureId           = BVIEW_FEATURE_SYSTEM;
  bcmSystem->feature.supportedAsicMask   = BVIEW_SYSTEM_SUPPORT_MASK;
  bcmSystem->numSupportedAsics = 0;

  SOC_BRCM_API_ATTACH_MAX_UNITS_GET(&max_units);

  if (BVIEW_MAX_ASICS_ON_A_PLATFORM < max_units)
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Find out the attached ASIC and fill scaling parametrs*/
  for (unit = 0; ((unit < max_units) && (!SB_BRCM_RV_ERROR(SB_BRCM_API_UNIT_CHECK(unit)))) ; 
       unit++)
  {
    bcmSystem->numSupportedAsics++;

    if (SB_BRCM_RV_ERROR(SB_BRCM_API_PORT_CONFIG_GET(unit,&port_cfg)))
    {
      continue;
    }
    sb_brcm_port_config[unit] = port_cfg;
    SB_BRCM_PBMP_COUNT(port_cfg.ge, num_ports);
    num_front_panel_ports = num_ports;

    SB_BRCM_PBMP_COUNT(port_cfg.xe, num_ports);
    num_front_panel_ports += num_ports;

    asicDb[index].scalingParams.numPorts  = num_front_panel_ports;

    asicDb[index].unit = unit;
    sbplugin_common_system_util_chip_type_get (unit, &asicDb[index].asicType);

    /* Based on the CHIP call appropriate init function*/
    if (BVIEW_ASIC_TYPE_TD2 == asicDb[index].asicType || 
        BVIEW_ASIC_TYPE_TH  == asicDb[index].asicType)
    {
      sbplugin_common_system_util_td2_init (&asicDb[index]);
    }
    /*else if (BVIEW_ASIC_TYPE_TH == asic.asicType)
    {


    }*/
      
    bcmSystem->asicList[index] = &asicDb[index];
    sbplugin_system_max_buf_update(unit);
    sbplugin_system_cosq_gport_info_update(unit);
    index++;
  }    

    
  bcmSystem->system_name_get_cb     = sbplugin_common_system_name_get;
  bcmSystem->system_mac_get_cb      = sbplugin_common_system_mac_get;
  bcmSystem->system_ip4_get_cb      = sbplugin_common_system_ipv4_get;
  bcmSystem->system_time_get_cb     = sbplugin_common_system_time_get;
  bcmSystem->system_asic_translate_from_notation_cb      = sbplugin_common_system_asic_translate_from_notation;
  bcmSystem->system_port_translate_from_notation_cb      = sbplugin_common_system_port_translate_from_notation;
  bcmSystem->system_asic_translate_to_notation_cb        = sbplugin_common_system_asic_translate_to_notation;
  bcmSystem->system_port_translate_to_notation_cb        = sbplugin_common_system_port_translate_to_notation;
  bcmSystem->system_network_os_get_cb                    = sbplugin_common_system_network_os_get;
  bcmSystem->system_uid_get_cb                           = sbplugin_common_system_uid_get;
  bcmSystem->system_lag_translate_to_notation_cb        = sbplugin_common_system_lag_translate_to_notation;
  bcmSystem->system_max_buf_snapshot_get_cb              = sbplugin_common_system_max_buf_snapshot_get;
  bcmSystem->system_packet_rx_register_cb                = sbplugin_common_system_packet_rx_register;
  return BVIEW_STATUS_SUCCESS;
} 

/*********************************************************************
* @brief  Get the system name
*
* @param[out] buffer                         - buffer
* @param[in]  length                         - length of the buffer
*
* @retval  BVIEW_STATUS_SUCCESS            if Name get is success.
* @retval  BVIEW_STATUS_FAILURE            if Name get is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_common_system_name_get (char *buffer, int length)
{
  if (buffer == NULL)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  strncpy (buffer, "BCM-TRIDENT2", length);
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Get the MAC address of the system
*
* @param[out] buffer                         - buffer
* @param[in]  length                         - length of the buffer
*
* @retval  BVIEW_STATUS_SUCCESS            if MAC get is success.
* @retval  BVIEW_STATUS_FAILURE            if MAC get is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes    Get MAC address of the service port.
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_common_system_mac_get (unsigned char *buffer, 
                                           int length)
{
  int fd;
  struct ifreq ifr;
  BVIEW_STATUS  rv = BVIEW_STATUS_FAILURE;

  if (buffer == NULL)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  fd = socket(AF_INET, SOCK_DGRAM, 0);

  if (fd > -1)
  {
    ifr.ifr_addr.sa_family = AF_INET;
    snprintf(ifr.ifr_name, IFNAMSIZ-1, "%s", "eth0");

    if (ioctl(fd, SIOCGIFHWADDR, &ifr) != -1)
    {
      memcpy(buffer, ifr.ifr_hwaddr.sa_data, length);
      rv = BVIEW_STATUS_SUCCESS;
    }
    close(fd);
  }

  return rv;
}

/*********************************************************************
* @brief  Get the IP address of system
*
* @param[out] buffer                         - buffer
* @param[in]  length                         - length of the buffer
*
* @retval  BVIEW_STATUS_SUCCESS            if IP get is success.
* @retval  BVIEW_STATUS_FAILURE            if IP get is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes    Get IP address of service port.
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_ipv4_get (unsigned char *buffer, 
                                           int length)
{
  int fd;
  struct ifreq ifr;
  BVIEW_STATUS  rv  = BVIEW_STATUS_FAILURE;

  if (buffer == NULL)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  } 
  fd = socket(AF_INET, SOCK_DGRAM, 0);

  if (fd > -1)
  {
    /* Get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* IP address attached to "eth0" */
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
    if (ioctl(fd, SIOCGIFADDR, &ifr) != -1)
    {
      memcpy (buffer, &((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr, length); 
      rv = BVIEW_STATUS_SUCCESS;
    }
    close(fd);
  }

  return rv;
}

/*********************************************************************
* @brief  Get Current local time.
*
* @param[out] tm                          - Pointer to tm structure
*
* @retval  BVIEW_STATUS_SUCCESS            if time get is success.
* @retval  BVIEW_STATUS_FAILURE            if time get is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes    Get IP address of service port.
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_common_system_time_get (time_t *ptime)
{
  /* NULL pointer check*/
  BVIEW_NULLPTR_CHECK (ptime);

  time (ptime);
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Translate ASIC String notation to ASIC Number.
*
* @param[in]  src                         - ASIC ID String
* @param[out] asic                        - ASIC Number
*
* @retval  BVIEW_STATUS_SUCCESS            if ASIC Translation is success.
* @retval  BVIEW_STATUS_FAILURE            if ASIC Translation is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes   
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_asic_translate_from_notation (char *src, 
                                                               int *asic) 
{
  BVIEW_NULLPTR_CHECK (src);
  BVIEW_NULLPTR_CHECK (asic);

  /* Convert to Interger*/
  *asic = atoi(src);
 
  if (*asic > (BVIEW_MAX_ASICS_ON_A_PLATFORM))
     return BVIEW_STATUS_INVALID_PARAMETER;
 
  /* Get BCM ASIC ID from mapping table*/
  BVIEW_BCM_ASIC_GET (*asic)

  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Translate Port String notation to Port Number.
*
* @param[in]   src                         - Port ID String
* @param[out]  port                        - PortId
*
* @retval  BVIEW_STATUS_SUCCESS            if Port Tranlate is success.
* @retval  BVIEW_STATUS_FAILURE            if Port  is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes  
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_port_translate_from_notation (char *src, 
                                                               int *port) 
{
  BVIEW_NULLPTR_CHECK (src);
  BVIEW_NULLPTR_CHECK (port);
  
  /* Convert to Interger*/
  *port = atoi(src); 
  
  /* Get BCM port*/
  BVIEW_BCM_PORT_GET (*port);

  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Translate ASIC number to ASIC string notation.
*
* @param[in]   asic                         - ASIC ID
* @param[out]  dst                          - ASIC ID String
*
* @retval  BVIEW_STATUS_SUCCESS            if ASIC ID Tranlate is success.
* @retval  BVIEW_STATUS_FAILURE            if ASIC ID Tranlate is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes 
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_asic_translate_to_notation (int asic, 
                                                             char *dst) 
{
  BVIEW_NULLPTR_CHECK (dst);

  if (asic >= (BVIEW_MAX_ASICS_ON_A_PLATFORM))
     return BVIEW_STATUS_INVALID_PARAMETER;

  /* Get BCM ASIC ID from mapping table*/
  BVIEW_BCM_APP_ASIC_GET (asic)
  
  /* Convert to String*/
  sprintf(dst, "%d", asic); 

  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief   Translate port number to port string notation.
*
* @param[in]   asic                         - ASIC 
* @param[in]   port                         - Port Number
* @param[out]  dst                          - ASIC String
*
* @retval  BVIEW_STATUS_SUCCESS            if Port Tranlate is success.
* @retval  BVIEW_STATUS_FAILURE            if Port Tranlate is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes  
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_port_translate_to_notation (int asic, 
                                                             int port, 
                                                             char *dst) 
{
  BVIEW_NULLPTR_CHECK (dst);
   
  /* Get BCM port*/
  BVIEW_BCM_PORT_GET (port);
  
  /* Convert to String*/
  sprintf(dst, "%d", port); 

  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief       Get Network OS
*
* @param[out]  buffer                 Pointer to network OS String
* @param[in]   length                 length of the buffer
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure
*                                     Failed to get network os
*
* @retval   BVIEW_STATUS_SUCCESS      Network OS is successfully
*                                     queried
*
* @notes    none
*
*********************************************************************/
BVIEW_STATUS  sbplugin_common_system_network_os_get (uint8_t *buffer, int length)
{
  BVIEW_NULLPTR_CHECK (buffer);
  
  memcpy (buffer, SBPLUGIN_NETWORK_OS, length);
  return BVIEW_STATUS_SUCCESS;
}


/*********************************************************************
* @brief  Get the UID of the system
*
* @param[out] buffer                         - buffer
* @param[in]  length                         - length of the buffer
*
* @retval  BVIEW_STATUS_SUCCESS            if UID get is success.
* @retval  BVIEW_STATUS_FAILURE            if UID get is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes   get the UID of the system 
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_uid_get (unsigned char *buffer, 
                                           int length)
{
  unsigned char mac[BVIEW_MACADDR_LEN];

  if (buffer == NULL) 
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  memset(mac, 0, BVIEW_MACADDR_LEN);

  if (BVIEW_STATUS_SUCCESS != sbplugin_common_system_mac_get(&mac[0], BVIEW_MACADDR_LEN))
  {
    return BVIEW_STATUS_FAILURE;
  }

  snprintf((char *)buffer, length, "%02x%02x%02x%02x%02x%02x%02x%02x", 0,0, mac[0],mac[1], mac[2], mac[3], mac[4], mac[5]); 
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief   Translate lag number to lag string notation.
*
* @param[in]   asic                         - ASIC 
* @param[in]   lag                          - lag Number
* @param[out]  dst                          - ASIC String
*
* @retval  BVIEW_STATUS_SUCCESS            if lag Tranlate is success.
* @retval  BVIEW_STATUS_FAILURE            if lag Tranlate is failed.
* @retval  BVIEW_STATUS_INVALID_PARAMETER  if input parameter is invalid.
*
* @notes  
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_lag_translate_to_notation (int asic, 
                                                             int lag, 
                                                             char *dst) 
{
  BVIEW_NULLPTR_CHECK (dst);
   
  /* Get BCM lag*/
  BVIEW_BCM_LAG_GET (lag);
  
  /* Convert to String*/
  sprintf(dst, "%d", lag); 

  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Get snapshot of max buffers allocated  
*
*
* @param  [in]  asic                         - unit
* @param[out]  maxBufSnapshot                - Max buffers snapshot
* @param[out]  time                          - time
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if snapshot is succes.
* @retval BVIEW_STATUS_SUCCESS           if snapshot set is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_system_max_buf_snapshot_get (int asic, 
                              BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *maxBufSnapshot,
                              BVIEW_TIME_t * time)
{
   /*validate ASIC*/
  BVIEW_UNIT_CHECK (asic);

  /* Check validity of input data*/
  BVIEW_NULLPTR_CHECK (maxBufSnapshot);

   /* Update current local time*/
  sbplugin_common_system_time_get (time);

  *maxBufSnapshot = sbMaxBufSnapshot[asic];
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Register with silicon for CPU bound packets
*
*
* @param  [in]  asic                - unit
* @param  [in]  callback            - Function to be called when packet
*                                     is received
* @param  [in]  name                - Name of the function
* @param  [in]  cookie              - Cookie
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if registration is failed
* @retval BVIEW_STATUS_SUCCESS           if snapshot set is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS   sbplugin_common_system_packet_rx_register (int asic,
                                BVIEW_PACKET_RX_CALLBACK_t callback,
                                char *name,
                                void *cookie)
                                                          
{
  int priority = 10;
  int rv = SB_BRCM_E_NONE;
  unsigned int flags = SB_BRCM_RCO_F_ALL_COS;

   /*validate ASIC*/
  BVIEW_UNIT_CHECK (asic);
  /* Check validity of input data*/
  BVIEW_NULLPTR_CHECK (cookie);
  
  callbackFunc = callback;                                                   
 
  rv = SB_BRCM_API_SWITCH_RX_REGISTER (asic, name, 
                                       sbplugin_common_system_receive, 
                                       priority, cookie, flags);
  if (rv != SB_BRCM_E_NONE)
  {
     return BVIEW_STATUS_FAILURE;
  }

  rv = SB_BRCM_RX_START (asic, NULL);
  if (rv != SB_BRCM_E_NONE)
  {
    /* incase the rx start is already started,
       the call would return e_busy.
       Hence ignore if the same is already
       running */

    if (rv == SB_BRCM_E_BUSY)
    {
      return BVIEW_STATUS_SUCCESS;
    }
     return BVIEW_STATUS_FAILURE;
  }
  
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Get the sizes of user headers in bits
*
* @param  [in]  asic                - unit
* @param  [out  user_header0        - Length of User Header 0
* @param  [out] user_header1        - Length of User Header 1
* @param  [out] user_header2        - Length of User Header 2
* @param  [out] user_header3        - Length of User Header 3
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_SUCCESS           if snapshot set is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_user_header_size_get (int asic,
                                            int *user_header0,
                                            int *user_header1,
                                            int *user_header2,
                                            int *user_header3)
{

  /* Check validity of input data*/
  BVIEW_NULLPTR_CHECK (user_header0);
  /* Check validity of input data*/
  BVIEW_NULLPTR_CHECK (user_header1);
  /* Check validity of input data*/
  BVIEW_NULLPTR_CHECK (user_header2);
/* Check validity of input data*/
  BVIEW_NULLPTR_CHECK (user_header3);

#ifdef BVIEW_CHIP_QUMRAN
  *user_header0 = 
      soc_property_port_get((asic), 0, spn_FIELD_CLASS_ID_SIZE, 0);
  *user_header1 = 
      soc_property_port_get((asic), 1, spn_FIELD_CLASS_ID_SIZE, 0);
  *user_header2 = 
      soc_property_port_get((asic), 2, spn_FIELD_CLASS_ID_SIZE, 0);
  *user_header3 = 
      soc_property_port_get((asic), 3, spn_FIELD_CLASS_ID_SIZE, 0);
#else
  *user_header0 = 0;
  *user_header1 = 0;
  *user_header2 = 0;
  *user_header3 = 0;
#endif
  return BVIEW_STATUS_SUCCESS;
}

#ifdef BVIEW_CHIP_QUMRAN
/*********************************************************************
* @brief  Strip FTMH and User Header etc and get the start ethernet 
*         packet 
*
* @param  [in]  asic                - unit
* @param  [in]  pkt                 - Pointer to packet data
* @param  [in]  len                 - Length of the packet
* @param  [out] user_header         - Pointer to User Header start 
* @param  [out] offset              - Offset to ethernet header
*                                     length of FTMH+ User Header etc
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_SUCCESS           if snapshot set is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_pkt_start_get (unsigned int   asic,
                                     unsigned char  *pkt,
                                     int   len,
                                     unsigned char *user_header,
                                     unsigned int *offset)
{
  unsigned char        pph_type;
  unsigned int         pph_offset;
  char                *propkey, *propval;
  static unsigned int  lb_ext_offset = (-1);
  unsigned char        eei_ext_present;
  unsigned char        lrn_ext_present;
  unsigned char        fhei_size;
  int                  header0 =0, header1 = 0;
  int                  header2 =0, header3 = 0;
  int                  user_header_size = 0;
  
  /* Check validity of input data*/
  BVIEW_NULLPTR_CHECK (pkt);
  /* Check validity of input data*/
  BVIEW_NULLPTR_CHECK (user_header);
  /* Check validity of input data*/
  BVIEW_NULLPTR_CHECK (offset);

  /* Initialize offset to account for the FTMH base header size */
  *offset = 9;

  /* Only need to read the soc property once, it will not change */
  if (lb_ext_offset == -1)
  {
    propkey = spn_SYSTEM_FTMH_LOAD_BALANCING_EXT_MODE;
    propval = soc_property_get_str(asic, propkey);

    if ((propval) && (sal_strcmp(propval, "ENABLED") == 0))
    {
      lb_ext_offset = 1;
    }
    else
    {
      lb_ext_offset = 0;
    }
  }

  *offset += lb_ext_offset;

  /*
   * Need to parse the FTMH Base header to determine if Dest System Port Ext(16bits) is included
   * This is bit 3 of the 72bit FTMH base header.
   */
  if (pkt[8] & 0x08)
  {
    *offset += 2;
  }
  
   /* TODO - how do we determine is stacking Extension(16bits) is included */

  /* Parse the PPH_TYPE in the FTMH header to see how much, if any, PPH to parse */
  pph_type = (pkt[5] & 0x06) >> 1;
  switch(pph_type)
  {
    case 0: /* No PPH */
      pph_offset = 0;
      break;

    case 1: /* PPH Base */
      /* PPH Base size = 56bits */
      pph_offset = 7;

      /* Is EEI-Extension-Present 24bits */
      eei_ext_present = (pkt[*offset+0] & 0x80) >> 7;
      if (eei_ext_present)
      {
        pph_offset += 3;
      }

      /* Is Learn-Extension-Present 40bits */
      lrn_ext_present = (pkt[*offset+0] & 0x40) >> 6;
      if (lrn_ext_present)
      {
        pph_offset += 5;
      }

      /* FHEI-Size? */
      fhei_size = (pkt[*offset+0] & 0x30) >> 4;
      switch(fhei_size)
      {
        case 0:
          /* No fhei header */
          break;
        case 1:
          /* 3 Byte fhei header */
          pph_offset += 3;
          break;
        case 2:
          /* 5 Byte fhei header */
          pph_offset += 5;
          break;
        case 3:
          /* 8 Byte fhei header */
          pph_offset += 8;
          break;
      }
      break;

    case 2: /* PPH OAM-TS only */
      pph_offset = 0;
      break;

    case 3: /* PPH Base + PAM-TS */
      pph_offset = 0;
      break;
   }

  *offset += pph_offset;
  /* Get the user header size */
  sbplugin_user_header_size_get (asic, &header0, &header1, &header2, &header3);
  /* Convert to Bytes*/
  user_header_size = (header0 + header1 + header2 + header3) /8;

  /* Copy User header*/
  memcpy(user_header, &pkt[*offset], user_header_size);
  *offset += user_header_size;
  return BVIEW_STATUS_SUCCESS;
}
#endif

/*********************************************************************
* @brief  Packet Receive and notify the applciation
*
*
* @param  [in]  asic                   - unit
* @param  [in]  bcm_pkt                - Pointer to ASIC Packet structure
* @param  [in]  cookie                 - Cookie
*
* @retval SB_BRCM_RX_HANDLED     Packet is handled.
*
* @notes    none
*
*
*********************************************************************/
SB_BRCM_RX_t   sbplugin_common_system_receive (int asic, 
                                               SB_BRCM_PKT_t *bcm_pkt, 
                                               void *cookie)
{
  static BVIEW_PACKET_MSG_t     pktMsg;
  BVIEW_STATUS rc = BVIEW_STATUS_SUCCESS;
#ifdef FEAT_PT
  int requestId;
#endif
#ifdef BVIEW_CHIP_QUMRAN
  SB_BRCM_PKT_t pkt;
  unsigned int offset = 0;
  unsigned int crc = 0;
#endif
  unsigned char user_header[BVIEW_MAX_USER_HEADER_SIZE];

  memset (&pktMsg, 0x00, sizeof (pktMsg));
#ifdef BVIEW_CHIP_QUMRAN
  memset (&pkt, 0x00, sizeof(pkt));
#endif
  memset (user_header, 0x00, BVIEW_MAX_USER_HEADER_SIZE);

  /* Check validity of input data*/
  BVIEW_NULLPTR_CHECK (cookie);
  /* Check validity of input data*/
  BVIEW_NULLPTR_CHECK (bcm_pkt);

#ifdef BVIEW_CHIP_QUMRAN
  sbplugin_pkt_start_get (asic, bcm_pkt->pkt_data->data,
                          bcm_pkt->tot_len,
                          user_header,
                          &offset);
  memcpy (&pkt, bcm_pkt, sizeof(pkt));
  pkt.pkt_data->data = &bcm_pkt->pkt_data->data[offset];
  pkt.tot_len =  bcm_pkt->tot_len - offset;
  bcm_pkt = &pkt;
  bcm_pkt->src_port = bcm_pkt->src_port -12;
#endif
  /* Get Source port and destination ports*/
  pktMsg.packet.source_port = bcm_pkt->src_port;
  pktMsg.packet.asic = bcm_pkt->unit;

  
  /* check if the length of the packet is 
     in the supported range */
  if (BVIEW_MAX_PACKET_SIZE < bcm_pkt->tot_len)
  {
    bcm_pkt->tot_len = BVIEW_MAX_PACKET_SIZE;
  }

  pktMsg.packet.pkt_len = bcm_pkt->tot_len;
 
  if (bcm_pkt->dest_port != 0)
  {
    BVIEW_SETMASKBIT (pktMsg.packet.dst_port_mask, bcm_pkt->dest_port); 
  }
  memcpy (&pktMsg.packet.data[0], &bcm_pkt->pkt_data->data[0], bcm_pkt->tot_len);
#ifdef BVIEW_CHIP_QUMRAN
  crc =crc32_calculate(&pktMsg.packet.data[0], pktMsg.packet.pkt_len);
  memcpy (&pktMsg.packet.data[pktMsg.packet.pkt_len], &crc, sizeof(crc));
  pktMsg.packet.pkt_len += sizeof(crc);
#endif
  /* Build appMask information */
#ifdef FEAT_BHD
  if (BVIEW_STATUS_SUCCESS == 
      sbplugin_common_bhd_build_info(bcm_pkt, user_header))
  {
    pktMsg.appMask |= BVIEW_FEATURE_BHD;
  }
#endif
#ifdef FEAT_PT
  if (BVIEW_STATUS_SUCCESS == 
      sbplugin_common_pt_build_info(bcm_pkt, &requestId))
  {
    pktMsg.appMask |= BVIEW_FEATURE_LIVE_PT;
    pktMsg.ltcRequestId = requestId;
  }
#endif

  if (pktMsg.appMask == 0)
  {
    return SB_BRCM_RX_HANDLED;
  }
  else
  {  
    /* Notify through callback function registered*/
    rc = callbackFunc (cookie, &pktMsg);
    if (BVIEW_STATUS_SUCCESS != rc)
    {
    }  
  }   
  return SB_BRCM_RX_HANDLED;
}




