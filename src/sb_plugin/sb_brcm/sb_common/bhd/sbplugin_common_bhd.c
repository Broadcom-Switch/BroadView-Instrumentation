/************************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename sbplugin_common_bhd.c
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

#include <stdbool.h>
#include "feature.h"
#include "sbplugin_common_system.h"
#include "sbplugin_common_bhd.h"
#include "sbplugin_common.h"
#include "bhd.h"
#include "common/platform_spec.h"
#include "sbfeature_bhd.h"
#include "port_utils.h"


/* Black Hold Detection policy information*/
typedef struct _bhd_policy_
{
  SB_BRCM_FIELD_GROUP_CONFIG_t ingressGroupId;
  SB_BRCM_FIELD_GROUP_CONFIG_t egressGroupId;
  SB_BRCM_FIELD_ENTRY_t        entry[BVIEW_ASIC_MAX_PORTS];
  SB_BRCM_FIELD_ENTRY_t        egressEntry;
  SB_BRCM_FIELD_STAT_t         stat_type;
  int                          statId;
  int                          statShareCount;
} BVIEW_BHD_POLICY_t;

typedef struct _sflow_info_
{
  int                          probe_port;
} BVIEW_SFLOW_INFO_t;

typedef struct _bhd_info_
{
  BVIEW_BHD_POLICY_t  policy;
  BVIEW_SFLOW_INFO_t  sflow;
  int                 destClassVal;
  SB_BRCM_PBMP_t      bhdPbmp;
  char                sampling_type;
} BVIEW_BHD_INFO_t;

#define    BVIEW_BHD_SFLOW_MIRROR_SESSION              1
#define    BVIEW_BHD_RULE_CLASS_TAG_ID                 10
#define    BVIEW_SFLOW_SAMPLE_RANDOM_SEED              0x11223344
#define    BVIEW_MAC_ADDR_LEN                          6
#define    BVIEW_BHD_RULE_MATCH_ID                     50
#define    BVIEW_BHD_DEST_CLASS_VALUE                  0x3200
#define    BVIEW_BHD_PORT_CLASS                        11

BVIEW_BHD_INFO_t   bhdInfo;

/*********************************************************************
* @brief  Init Black Hole Detection Policy and Sflow parameters
*
* @param
*
* @retval  
* @retval   
* @retval   
*
* @notes    none
*
*
*********************************************************************/
void   sbplugin_bhd_config_init()
{
  int index =0;
  BVIEW_BHD_POLICY_t   *info;
  BVIEW_SFLOW_INFO_t   *sflow; 

  info  = &bhdInfo.policy;
  sflow = &bhdInfo.sflow;

  info->ingressGroupId = SB_BRCM_FIELD_ENTRY_INVALID;
  info->egressGroupId = SB_BRCM_FIELD_ENTRY_INVALID;
  info->stat_type = SB_BRCM_FIELD_STAT_COUNT;
  info->statId = SB_BRCM_FIELD_ENTRY_INVALID;
  for (index = 0; index < BVIEW_ASIC_MAX_PORTS; index++)
  {
    info->entry[index] = SB_BRCM_FIELD_ENTRY_INVALID;
  }
  info->egressEntry = SB_BRCM_FIELD_ENTRY_INVALID;
  info->statShareCount = 0;
  sflow->probe_port = 0;
  bhdInfo.sampling_type = 0;
  bhdInfo.destClassVal = SB_BRCM_FIELD_ENTRY_INVALID;
  SB_BRCM_PBMP_CLEAR (bhdInfo.bhdPbmp);
}

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
BVIEW_STATUS sbplugin_common_bhd_init (BVIEW_SB_BHD_FEATURE_t *bcmBhd)
{
  /* NULL Pointer check*/
  BVIEW_NULLPTR_CHECK (bcmBhd);

  /* Initialize BST functions*/
  bcmBhd->feature.featureId           = BVIEW_FEATURE_BHD;
  bcmBhd->feature.supportedAsicMask   = BVIEW_BHD_SUPPORT_MASK;
  bcmBhd->bhd_sflow_sampling_status_get_cb           
                     = sbplugin_common_bhd_sflow_status_get;
  bcmBhd->bhd_cancel_black_hole_cb                   
                     = sbplugin_common_bhd_cancel_black_hole;
  bcmBhd->bhd_configure_black_hole_cb                   
                     = sbplugin_common_bhd_config_black_hole;
  bcmBhd->bhd_asic_sampling_capabilties_get_cb       
                     = sbplugin_common_bhd_asic_sampling_cap_get;
  bcmBhd->bhd_black_holed_pkt_count_get_cb           
                     = sbplugin_common_bhd_pkt_count_get;

  sbplugin_bhd_config_init ();
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Create black hole policy and apply.
*
* @param[in]  asic               - unit
* @param[in]  config             - Black Hold Detection configuration structure
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
BVIEW_STATUS snoop_gport_get(SB_BRCM_GPORT_t *snoop_trap_gport_id,
                             int *snoop_command)
{
  static int trap_id = 0;
  int unit = 0;
  int result = SB_BRCM_E_NONE;
  int flags = 0; /* Do not specify any ID for the snoop command / trap */
  int trap_dest_strength = 0; /* No influence on the destination update */
  int trap_snoop_strength = 3; /* Strongest snoop strength for this trap */
  SB_BRCM_RX_SNOOP_CONFIG_t snoop_config; /* Snoop attributes */
  SB_BRCM_RX_TRAP_CONFIG_t trap_config;
  SB_BRCM_GPORT_t trap_gport;

  if (trap_id == 0)
  {
    /*
     * Create a Snoop Command
     * The snoop command defines the attribute of the Snoop
     */
    result = SB_BRCM_RX_SNOOP_CREATE(unit, flags, snoop_command); /* Receive a snoop command handle */
    if (SB_BRCM_E_NONE != result)
    {
      SB_BRCM_RX_SNOOP_DESTROY(unit, flags, *snoop_command);
      return BVIEW_STATUS_FAILURE;
    }

    /*
     * Specify the snoop attributes
     * Here, we set the destination and the probability to 100%
     * Snoop the whole packet
     */
    /* Initialize the structure */
    SB_BRCM_RX_SNOOP_CONFIG_INIT(&snoop_config);
    snoop_config.flags |= SB_BRCM_RX_SNOOP_UPDATE_DEST;

    /* Set the Mirror destination to go to the Destination local port */
    snoop_config.dest_port = SB_BRCM_GPORT_LOCAL_CPU;
    snoop_config.size = -1; /* Full packet snooping */
    snoop_config.probability= 100000; /* 100000 is 100.000% */

    /* Set snoop configuration */
    result = SB_BRCM_RX_SNOOP_SET(unit, *snoop_command, &snoop_config);
    if (result != SB_BRCM_E_NONE)
    {
      SB_BRCM_RX_SNOOP_DESTROY(unit, flags, *snoop_command);
      return BVIEW_STATUS_FAILURE;
    }
    /*
     * Create a User-defined trap for snooping
     */
    result = SB_BRCM_RX_TRAP_TYPE_CREATE(unit, flags, SB_BRCM_RX_TRAP_USER_DEFINE, &trap_id);
    if (result != SB_BRCM_E_NONE)
    {
      SB_BRCM_RX_SNOOP_DESTROY(unit, flags, *snoop_command);
      SB_BRCM_RX_TRAP_TYPE_DESTROY(unit, trap_id);
      return BVIEW_STATUS_FAILURE;
    }

     /*
     * Configure the trap to the snoop command
     */
    SB_BRCM_RX_TRAP_CONFIG_INIT(&trap_config);

    /*for port dest change*/
    trap_config.flags |= SB_BRCM_RX_TRAP_REPLACE;
    trap_config.trap_strength = trap_dest_strength;
    trap_config.snoop_cmnd = *snoop_command; /* Snoop any frame matched by this trap */
    trap_config.snoop_strength = trap_snoop_strength;

    result = SB_BRCM_RX_TRAP_SET(unit, trap_id, &trap_config);
    if (result != SB_BRCM_E_NONE)
    {
      SB_BRCM_RX_SNOOP_DESTROY(unit, flags, *snoop_command);
      SB_BRCM_RX_TRAP_TYPE_DESTROY(unit, trap_id);
      return BVIEW_STATUS_FAILURE;
    }
  }

  /* Get the trap gport to snoop */
  SB_BRCM_GPORT_TRAP_SET(trap_gport, trap_id, trap_dest_strength, trap_snoop_strength);

  *snoop_trap_gport_id = trap_gport;

  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Create black hole policy and apply.
*
* @param[in]  asic               - unit
* @param[in]  config             - Black Hold Detection configuration structure
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
BVIEW_STATUS sbplugin_common_bhd_ingress_policy_dnx (int asic,
                               BVIEW_BLACK_HOLE_CONFIG_t  *config)
{
  SB_BRCM_FIELD_QSET_t             qset;
  SB_BRCM_FIELD_GROUP_CONFIG_t     group;
  SB_BRCM_FIELD_ENTRY_t            entry;
  SB_BRCM_FIELD_ASET_t             aset;
  SB_BRCM_GPORT_t                   snoop_trap_gport_id;
  SB_BRCM_PBMP_t                   InPbmpMask, InPbmp;
  SB_BRCM_FIELD_PRESEL_SET_t       psset;
  SB_BRCM_POLICER_CONFIG_t         pol_cfg;
  SB_BRCM_POLICER_t                policer_id;
  SB_BRCM_FIELD_STAT_t             stat_type = SB_BRCM_FIELD_STATS_PACKETS;
  int                              dst_modid=0,modid_mask =0xffffffff;
  int                              port_mask =0xffffffff;
  int                              rv  =0, port =0, maxPorts;
  int                              presel_id;
  int                              statId;
  int                              snoop_command = 0;
  int                              header0 = 0, header1 =0;
  int                              header2 = 0, header3 =0;

  SB_BRCM_FIELD_QSET_INIT (qset);
  SB_BRCM_FIELD_QSET_ADD (qset, SB_BRCM_QUALIFY_STAGE_INGRESS);
  SB_BRCM_FIELD_QSET_ADD (qset, SB_BRCM_QUALIFY_DSTPORT);

  SB_BRCM_PBMP_CLEAR(InPbmp);
  SB_BRCM_PBMP_CLEAR(InPbmpMask);
 
  maxPorts = 512 ;
  for(port=0; port<maxPorts; port++)
  {
    SB_BRCM_PBMP_PORT_ADD(InPbmpMask, port);
  }

  /* Build the BCM Port Mask*/
  BVIEW_SYSTEM_PORT_ITER (asic, port)
  {
    if (BVIEW_ISMASKBITSET (config->black_hole_port_mask, port))
    {
      SB_BRCM_PBMP_PORT_ADD(InPbmp, port);
    }
  }
  /* Get the user header sizes in bits */
  sbplugin_user_header_size_get (asic, &header0, &header1,
                                 &header2, &header3);
  /* Create Trap */
  if (BVIEW_STATUS_SUCCESS != snoop_gport_get (&snoop_trap_gport_id, 
                                              &snoop_command))
  {
    return BVIEW_STATUS_FAILURE; 
  }

    /* Cretae a presel entity */
  BVIEW_ERROR_RETURN(SB_BRCM_FIELD_PRESEL_CREATE(asic, &presel_id));
    /* Configure the presel */
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_QUALIFY_STAGE(asic, 
                                              presel_id | SB_BRCM_FIELD_QUALIFY_PRESEL, 
                                              SB_BRCM_FIELD_STAGE_INGRESS));
  BVIEW_ERROR_RETURN(SB_BRCM_FIELD_QUALIFY_INPORTS(asic, 
                                              presel_id | SB_BRCM_FIELD_QUALIFY_PRESEL, 
                                              InPbmp,InPbmpMask));

  SB_BRCM_FIELD_PRESEL_INIT(psset);
  SB_BRCM_FIELD_PRESEL_ADD(psset, presel_id);

   /*
     *  Define the ASET Field Group
     */
  SB_BRCM_FIELD_ASET_INIT(aset);
  SB_BRCM_FIELD_ASET_ADD(aset, SB_BRCM_FIELD_ACTION_SNOOP);
  SB_BRCM_FIELD_ASET_ADD(aset, SB_BRCM_FIELD_ACTION_USE_POLICER_RESULT);
  SB_BRCM_FIELD_ASET_ADD(aset, SB_BRCM_FIELD_ACTION_POLICER_LEVEL0);
  SB_BRCM_FIELD_ASET_ADD(aset, SB_BRCM_FIELD_ACTION_CLASS_DEST_SET);
  SB_BRCM_FIELD_ASET_ADD(aset, SB_BRCM_FIELD_ACTION_STAT0);

  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_GROUP_CREATE(asic, qset, 0, &group));
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_GROUP_PRESEL_SET(asic, group, &psset));

   /* Create stat for total packets*/
  stat_type = SB_BRCM_FIELD_STATS_PACKETS;
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_STAT_CREATE (asic, group, 1 , &stat_type, &statId));

    /*  Attach the action set */
  rv = SB_BRCM_FIELD_GROUP_ACTION_SET(asic, group, aset);
  if (SB_BRCM_E_NONE != rv)
  {
    SB_BRCM_FIELD_GROUP_DESTORY(asic, group);
    return BVIEW_STATUS_FAILURE;
  }

  /* Loop through all the ports*/
  BVIEW_SYSTEM_PORT_ITER (asic, port)
  {
    if (BVIEW_ISMASKBITSET (config->black_hole_port_mask, port))
    { 
      rv = SB_BRCM_FIELD_ENTRY_CREATE (asic, group, &entry);
      if (SB_BRCM_E_NONE != rv)
      {
        SB_BRCM_FIELD_GROUP_DESTORY(asic, group);
        return BVIEW_STATUS_FAILURE;
      }

      rv = SB_BRCM_FIELD_QUALIFY_DSTPORT (asic, entry, dst_modid, modid_mask,
                                      port+12, port_mask);
      if (SB_BRCM_E_NONE != rv)
      {
        SB_BRCM_FIELD_GROUP_DESTORY(asic, group);
        return BVIEW_STATUS_FAILURE;
      }
      rv =  SB_BRCM_FIELD_ACTION_ADD(asic, entry, 
                                 SB_BRCM_FIELD_ACTION_SNOOP, 
                                 snoop_trap_gport_id, 0);
      if (SB_BRCM_E_NONE != rv)
      {  
        SB_BRCM_FIELD_GROUP_DESTORY(asic, group);
        return BVIEW_STATUS_FAILURE;
      }
      rv= SB_BRCM_FIELD_ACTION_ADD (asic,entry, 
                                    SB_BRCM_FIELD_ACTION_CLASS_DEST_SET, 
                                    BVIEW_BHD_DEST_CLASS_VALUE,0);
      if (rv != SB_BRCM_E_NONE) 
      {
        SB_BRCM_FIELD_GROUP_DESTORY(asic, group);
        return BVIEW_STATUS_FAILURE;
      }
       /* Attach the same stat for all the entries*/
      rv = SB_BRCM_FIELD_STAT_ATTACH (asic, entry, statId);
      if (rv != SB_BRCM_E_NONE)
      {
        SB_BRCM_FIELD_GROUP_DESTORY(asic, group);
        return BVIEW_STATUS_FAILURE;
      }
      bhdInfo.policy.statShareCount++;
      SB_BRCM_POLICER_CONFIG_INIT (&pol_cfg);
      pol_cfg.flags = (SB_BRCM_POLICER_MODE_PACKETS| SB_BRCM_POLICER_COLOR_BLIND);
      pol_cfg.mode = SB_BRCM_POLICER_MODE_SR_TCM;
      pol_cfg.ckbits_sec = 
          config->sampling_config.sampling_params.agent_sampling_params.water_mark;
      pol_cfg.ckbits_burst = 1000;
      pol_cfg.max_pkbits_sec = 1000000;

      rv = SB_BRCM_POLICER_CREATE (asic, &pol_cfg, &policer_id);
      if (rv != SB_BRCM_E_NONE) 
      {
         SB_BRCM_FIELD_GROUP_DESTORY(asic, group);
         return BVIEW_STATUS_FAILURE;
      }
      rv = SB_BRCM_FIELD_ENTRY_POLICER_ATTACH (asic, entry, 0, policer_id);
      if (rv != SB_BRCM_E_NONE) 
      {
        SB_BRCM_FIELD_GROUP_DESTORY(asic, group);
        return BVIEW_STATUS_FAILURE;
      }
      bhdInfo.policy.entry[port] = entry;  
    }
  }
  /* Policer create*/
  rv = SB_BRCM_FIELD_GROUP_INSTALL(asic, group);
  if (SB_BRCM_E_NONE != rv)
  {
    SB_BRCM_FIELD_GROUP_DESTORY(asic, group);
    return BVIEW_STATUS_FAILURE;
  }

  bhdInfo.policy.stat_type = stat_type;
  bhdInfo.policy.ingressGroupId = group;
  bhdInfo.policy.statId = statId;
  bhdInfo.destClassVal = ((BVIEW_BHD_DEST_CLASS_VALUE >> header2 ) & 0xff);
  SB_BRCM_PBMP_ASSIGN (bhdInfo.bhdPbmp,InPbmp);
  bhdInfo.sampling_type = BVIEW_BHD_AGENT_SAMPLING;
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Create black hole policy and apply.
*
* @param[in]  asic               - unit
* @param[in]  config             - Black Hold Detection configuration structure
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
BVIEW_STATUS sbplugin_common_bhd_egress_policy_dnx (int asic)
                               
{
  SB_BRCM_FIELD_QSET_t                qset;
  SB_BRCM_FIELD_GROUP_CONFIG_t        group;
  SB_BRCM_FIELD_ENTRY_t               entry;
  SB_BRCM_FIELD_ASET_t                aset;
  int                                 rv  =0;
  int                                 header0 = 0, header1 =0;
  int                                 header2 = 0, header3 =0;

  /* Get the user header sizes in bits */
  sbplugin_user_header_size_get (asic, &header0, &header1,
                                 &header2, &header3);
  SB_BRCM_FIELD_QSET_INIT(qset);
  SB_BRCM_FIELD_QSET_ADD (qset, SB_BRCM_QSET_FIELD_QUALIFY_STAGE_EGRESS);
  SB_BRCM_FIELD_QSET_ADD (qset, SB_BRCM_FIELD_QUALIFY_INTERFACE_CLASS_PROCESSING_PORT_ENUM);
  SB_BRCM_FIELD_QSET_ADD (qset, SB_BRCM_FIELD_QUALIFY_COLOR_ENUM);
  SB_BRCM_FIELD_QSET_ADD (qset, SB_BRCM_FIELD_QUALIFY_DST_CLASS_FIELD_ENUM);

  SB_BRCM_FIELD_ASET_INIT(aset);
  SB_BRCM_FIELD_ASET_ADD(aset, SB_BRCM_FIELD_ACTION_DROP);

  rv = SB_BRCM_FIELD_GROUP_CREATE(asic, qset, 0, &group);
  if (SB_BRCM_E_NONE != rv)
  {
    return BVIEW_STATUS_FAILURE;
  }

    /*  Attach the action set */
  rv = SB_BRCM_FIELD_GROUP_ACTION_SET(asic, group, aset);
  if (SB_BRCM_E_NONE != rv)
  {
    SB_BRCM_FIELD_GROUP_DESTORY(asic, group);
    return BVIEW_STATUS_FAILURE;
  }

  rv = SB_BRCM_FIELD_ENTRY_CREATE(asic, group, &entry);
  if (SB_BRCM_E_NONE != rv)
  {
    return BVIEW_STATUS_FAILURE;
  }
  /*  Add CPU to Class ID 2*/
  rv = SB_BRCM_PORT_CLASS_SET(asic, 0, 
                           SB_BRCM_PORT_CLASS_FIELD_EGRESS_PACKET_PROCESSING, BVIEW_BHD_PORT_CLASS);
  if (SB_BRCM_E_NONE != rv)
  {
    return BVIEW_STATUS_FAILURE;
  }
  /* Qualify on Class Id 2*/
  rv = SB_BRCM_FIELD_QUALIFY_INTERFACE_CLASS_PROCESSING_PORT(asic, entry, 
                                                       BVIEW_BHD_PORT_CLASS, 0xff);
  if (SB_BRCM_E_NONE != rv)
  {
    return BVIEW_STATUS_FAILURE;
  }
  /*  Qualify Green Packets*/
  rv = SB_BRCM_FIELD_QUALIFY_COLOR(asic, entry, SB_BRCM_FIELD_COLOR_GREEN);
  if (SB_BRCM_E_NONE != rv)
  {
    return BVIEW_STATUS_FAILURE;
  }
  rv = SB_BRCM_FIELD_QUALIFY_DST_CLASS_FIELD(asic, entry, 
                    ((BVIEW_BHD_DEST_CLASS_VALUE >> header2) & 0xff), 0xff);
  if (SB_BRCM_E_NONE != rv)
  {
    return BVIEW_STATUS_FAILURE;
  }
 
  /* Drop all green packets*/
  rv = SB_BRCM_FIELD_ACTION_ADD (asic,entry,SB_BRCM_FIELD_ACTION_DROP, 0,0);
  if (rv != SB_BRCM_E_NONE) 
  {
    return BVIEW_STATUS_FAILURE;
  }

  rv = SB_BRCM_FIELD_GROUP_INSTALL(asic, group);
  if (SB_BRCM_E_NONE != rv)
  {
    return BVIEW_STATUS_FAILURE;
  }
  bhdInfo.policy.egressGroupId = group;
  bhdInfo.policy.egressEntry = entry;

  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Create black hole policy and apply.
*
* @param[in]  asic               - unit
* @param[in]  config             - Black Hold Detection configuration structure
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
BVIEW_STATUS sbplugin_common_bhd_config_black_hole_dnx (int asic,
                               BVIEW_BLACK_HOLE_CONFIG_t  *config)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

  rv = sbplugin_common_bhd_ingress_policy_dnx (asic, config);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return rv;
  }

  rv = sbplugin_common_bhd_egress_policy_dnx (asic);
  if (rv != BVIEW_STATUS_SUCCESS)
  {
    return rv;
  }
  return rv;
}

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
                       BVIEW_BHD_PORT_SFLOW_SAMPLING_STATUS_t *status)
{
  BVIEW_NULLPTR_CHECK(status);
     /* get stats for flex port */
  SB_BRCM_STAT_GET32 (asic, port, SB_BRCM_SAMPLE_FLEX_SAMPLED_PKTS,
                               &status->sampled_pkt_count);
      /* get stats for flex port */
  SB_BRCM_STAT_GET32 (asic, port, SB_BRCM_SAMPLE_FLEX_PKTS,
                               &status->black_holed_pkt_count);
  if (bhdInfo.sampling_type == BVIEW_BHD_SFLOW_SAMPLING && 
      (SB_BRCM_PBMP_MEMBER(bhdInfo.bhdPbmp, port)))
  {
    status->sflowSamplingEnabled = true;
  }
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Create Black Hold Detection rule
*
* @param[in]   asic                 - unit
* @param[out]  ruleId               - Rule ID
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if rule creation is failed.
* @retval BVIEW_STATUS_SUCCESS           if rule creation is success.
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_bhd_sflow_mirror_delete (int asic, int port)
{
  int rv =0;
  SB_BRCM_GPORT_t   gport =0;
  int flags =0;
  SB_BRCM_MIRROR_DESTINATION_t   mirror_temp_dest;

  SB_BRCM_GPORT_MIRROR_SET(gport, BVIEW_BHD_SFLOW_MIRROR_SESSION);

  flags |= (SB_BRCM_MIRROR_PORT_INGRESS | SB_BRCM_MIRROR_PORT_SFLOW);
  rv =  SB_BRCM_MIRROR_DESTINATION_GET(asic, gport, &mirror_temp_dest);
  if (rv == SB_BRCM_E_NOT_FOUND)
  {
    return BVIEW_STATUS_FAILURE;
  }
  /* Clear MTP index in SFLOW_ING_MIRROR_CONFIG */
  BVIEW_ERROR_RETURN (SB_BRCM_MIRROR_PORT_DEST_DELETE(asic, port,
                                   flags, gport));
  BVIEW_ERROR_RETURN (SB_BRCM_MIRROR_PORT_DESTINATION_DESTROY(asic,gport));

  /* clear stats for flex port */
  SB_BRCM_STAT_CLEAR_SINGLE(asic, port, SB_BRCM_SAMPLE_FLEX_PKTS);
  SB_BRCM_STAT_CLEAR_SINGLE(asic, port, SB_BRCM_SAMPLE_FLEX_SNAPSHOT_PKTS);
  SB_BRCM_STAT_CLEAR_SINGLE(asic, port, SB_BRCM_SAMPLE_FLEX_SAMPLED_PKTS);

  return  BVIEW_STATUS_SUCCESS;
}

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
BVIEW_STATUS sbplugin_common_bhd_cancel_black_hole (int asic)
{
  int rv = SB_BRCM_E_NONE;
  BVIEW_STATUS  rc = BVIEW_STATUS_SUCCESS;
  int ruleIndex  = 0;
  SB_BRCM_POLICER_t            policer_id;
  int                          statId;
  
  for (ruleIndex =0 ; ruleIndex < BVIEW_ASIC_MAX_PORTS; ruleIndex++)
  {
    if (bhdInfo.policy.entry[ruleIndex] == SB_BRCM_FIELD_ENTRY_INVALID)
    {
      continue;
    }
   /* Destroy any attached statId before destroying the entry */
    rv = SB_BRCM_FIELD_ENTRY_STAT_GET(asic, bhdInfo.policy.entry[ruleIndex], &statId);
    if (rv == SB_BRCM_E_NONE)
    {
      SB_BRCM_FIELD_STAT_DETACH(asic, bhdInfo.policy.entry[ruleIndex], statId);
      bhdInfo.policy.statShareCount--;
      if (bhdInfo.policy.statShareCount == 0)
      {
        /* StatId deletion might fail if the statId is being shared */
        (void) SB_BRCM_FIELD_STAT_DESTORY(asic, statId);
      }
    }

    /* Destroy any attached policer before destroying the entry */
    rv = SB_BRCM_FIELD_ENTRY_POLICER_GET(asic, bhdInfo.policy.entry[ruleIndex], 0, &policer_id);
    if (rv == SB_BRCM_E_NONE)
    {
      SB_BRCM_FIELD_ENTRY_POLICER_DETACH(asic, bhdInfo.policy.entry[ruleIndex], 0);

      /* policer deletion might fail if the policer is being shared */
      (void) SB_BRCM_POLICER_DESTROY(asic, policer_id);
    }

    rv = SB_BRCM_FIELD_ENTRY_REMOVE(asic, bhdInfo.policy.entry[ruleIndex]);
    /* If rv is BCM_E_UNAVAIL, it is possible that we are trying to remove
     * a rule that is not installed in the hardware. So, continue with the
     * destroy. */


    rv = SB_BRCM_FIELD_ENTRY_DESTROY(asic, bhdInfo.policy.entry[ruleIndex]);
  }
  /* Delete the Policy */
  if (bhdInfo.policy.ingressGroupId != SB_BRCM_FIELD_ENTRY_INVALID)
  {
    rv = SB_BRCM_FIELD_GROUP_DESTORY (asic, bhdInfo.policy.ingressGroupId);
    if (rv != SB_BRCM_E_NONE)
    {
      return BVIEW_STATUS_FAILURE;
    }
  }

  if (bhdInfo.policy.egressGroupId != SB_BRCM_FIELD_ENTRY_INVALID)
  {
    if (bhdInfo.policy.egressEntry != SB_BRCM_FIELD_ENTRY_INVALID)
    {
       /* Destroy any attached policer before destroying the entry */
      rv = SB_BRCM_FIELD_ENTRY_POLICER_GET(asic, bhdInfo.policy.egressEntry, 0, &policer_id);
      if (rv == SB_BRCM_E_NONE)
      {
        SB_BRCM_FIELD_ENTRY_POLICER_DETACH(asic, bhdInfo.policy.egressEntry, 0);

        /* policer deletion might fail if the policer is being shared */
        (void) SB_BRCM_POLICER_DESTROY(asic, policer_id);
      }

      rv = SB_BRCM_FIELD_ENTRY_REMOVE(asic, bhdInfo.policy.egressEntry);
      rv = SB_BRCM_FIELD_ENTRY_DESTROY(asic, bhdInfo.policy.egressEntry);
    }

    rv = SB_BRCM_FIELD_GROUP_DESTORY (asic, bhdInfo.policy.egressGroupId);
    if (rv != SB_BRCM_E_NONE)
    {
      return BVIEW_STATUS_FAILURE;
    }
  }
  if (bhdInfo.sampling_type == BVIEW_BHD_SFLOW_SAMPLING)
  {
    /* Delete sflow configuration and clear sflow stats*/
    rc = sbplugin_bhd_sflow_mirror_delete (asic, 
                                      bhdInfo.sflow.probe_port);
    if (BVIEW_STATUS_SUCCESS != rc)
    {
      return BVIEW_STATUS_FAILURE;
    }

  }
  /* Intialize local BHD infomation*/
  sbplugin_bhd_config_init();

  return  BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Install Egress Rate limit policy on CPU port.
*
* @param[in]      asic               - unit
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if snapshot get is failed.
* @retval BVIEW_STATUS_SUCCESS           if snapshot get is success.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_bhd_egress_policy (int asic,
                                                SB_BRCM_FIELD_GROUP_CONFIG_t *group,
                                                SB_BRCM_FIELD_ENTRY_t *rule)
{
  SB_BRCM_FIELD_QSET_t         qset;
  SB_BRCM_FIELD_ENTRY_t        entry;
  SB_BRCM_FIELD_GROUP_CONFIG_t bhdGroup;
  SB_BRCM_POLICER_CONFIG_t     policer;
  SB_BRCM_POLICER_t            policer_id;
  int                          port = 0;


  SB_BRCM_FIELD_QSET_INIT(qset);
  SB_BRCM_FIELD_QSET_ADD(qset, SB_BRCM_QSET_FIELD_QUALIFY_STAGE_EGRESS);
  SB_BRCM_FIELD_QSET_ADD(qset, SB_BRCM_QSET_FIELD_QUALIFY_OUT_PORT);
  SB_BRCM_FIELD_QSET_ADD(qset, SB_BRCM_QSET_FIELD_QUALIFY_INGRESS_CLASS_FIELD);

   /* Create the group*/
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_GROUP_CREATE (asic, qset,0, &bhdGroup));

   /*Get the CPU port*/
  SB_BRCM_CPU_PORT_GET(asic, &port);

  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_ENTRY_CREATE (asic, bhdGroup, &entry));
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_QUALIFY_OUTPORT(asic, entry , port, 0xff));
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_QUALIFY_INGRESSCLASSFIELD(0, entry, 
                                      BVIEW_BHD_RULE_CLASS_TAG_ID,0xff));
  /* Attach the same stat for all the entries*/
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_ACTION_ADD (asic, entry,
                  SB_BRCM_FIELD_ACTION_RP_DROP,0,0));

  /* Create the policer and attach to the rule*/
  SB_BRCM_POLICER_CONFIG_INIT(&policer);
  policer.mode = SB_BRCM_POLICER_MODE_COMMITTED ;
  policer.flags |= (SB_BRCM_POLICER_MODE_PACKETS);
  policer.flags |= (SB_BRCM_POLICER_COLOR_BLIND);
  policer.ckbits_sec = 100;
  policer.ckbits_burst = 128;
  BVIEW_ERROR_RETURN (SB_BRCM_POLICER_CREATE (asic, &policer, &policer_id));
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_ENTRY_POLICER_ATTACH (asic, entry, 0, policer_id));
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_GROUP_INSTALL (asic, bhdGroup));

  *group = bhdGroup;
  *rule =  entry;
  return BVIEW_STATUS_SUCCESS;
}
/*********************************************************************
* @brief  Obtain ASIC List of supported drop reasons.
*
* @param[in]      asic               - unit
* @param[out]     mask               - Mask of drop reasons
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if snapshot get is failed.
* @retval BVIEW_STATUS_SUCCESS           if snapshot get is success.
*
* @notes    none
*
*
*********************************************************************/
int     sbplugin_common_bhd_l3_dst_mac_get (int asic,  int index,
                                            SB_BRCM_L3_HOST_t *info,
                                            void *user_data)
{
  SB_BRCM_L3_HOST_t   *tempHostInfo = user_data;

  if (info->l3a_ip_addr == tempHostInfo->l3a_ip_addr)
  {
    memcpy(&tempHostInfo->l3a_nexthop_mac, 
           &info->l3a_nexthop_mac,BVIEW_MAC_ADDR_LEN);
  }
  return 0;
}

/*********************************************************************
* @brief  Create Black Hold Detection rule
*
* @param[in]   asic                 - unit
* @param[out]  ruleId               - Rule ID
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if rule creation is failed.
* @retval BVIEW_STATUS_SUCCESS           if rule creation is success.
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_bhd_sflow_mirror_config (int asic,
                                                int port,
                                  BVIEW_BLACK_HOLE_CONFIG_t  *bhdConfig)
{
  unsigned char localMac[BVIEW_MAC_ADDR_LEN]={0};
  char nullMAC[BVIEW_MAC_ADDR_LEN] = {0};
  int  localIP = 0;
  SB_BRCM_GPORT_t   gport =0;
  BVIEW_BHD_PKT_SAMPLING_PARAMS_t   *config;
  SB_BRCM_MIRROR_DESTINATION_t   mirror_dest, mirror_temp_dest;
  SB_BRCM_L3_HOST_t           l3_host_info;
  SB_BRCM_L3_INFO_t           l3_info;
  int                         rv = 0;
  int                         flags = 0;


  BVIEW_NULLPTR_CHECK(bhdConfig);

  flags |= (SB_BRCM_MIRROR_PORT_SFLOW | SB_BRCM_MIRROR_PORT_INGRESS);
  config = &bhdConfig->sampling_config.sampling_params;

  /* clear the contents */
  SB_BRCM_MIRROR_DESTINATION_INIT(&mirror_dest);

  /* Get MAC and IP address of Local system*/
  if (BVIEW_STATUS_SUCCESS != 
      sbplugin_common_system_mac_get (localMac, BVIEW_MAC_ADDR_LEN))
  {
    return BVIEW_STATUS_FAILURE;
  }
  
  if (BVIEW_STATUS_SUCCESS != 
      sbplugin_common_system_ipv4_get ((unsigned char *)&localIP, sizeof(localIP)))
  {
    return BVIEW_STATUS_FAILURE;
  }
  
  /* Get gport of mirror port*/
  rv = SB_BRCM_API_PORT_GPORT_GET(asic, 
                                  config->sflow_sampling_params.mirror_port, 
                                  &gport);
  if (rv != SB_BRCM_E_NONE)
  {
    return BVIEW_STATUS_FAILURE;
  }

  l3_host_info.l3a_ip_addr = 
        config->sflow_sampling_params.encap_dest_ip_addr.s_addr;
  /* Traverse L3 Host table and get the MAC of interetsed L3 Interface*/
  SB_BRCM_API_L3_INFO (asic, &l3_info);
  rv = SB_BRCM_API_L3_HOST_TRAVERSE (asic, 0, 0, l3_info.l3info_max_host,
                                sbplugin_common_bhd_l3_dst_mac_get,
                                    &l3_host_info);
  if (rv != SB_BRCM_E_NONE)
  {
    return BVIEW_STATUS_FAILURE;
  }
  mirror_dest.gport = gport;
  mirror_dest.flags = (SB_BRCM_MIRROR_DEST_TUNNEL_SFLOW | 
                       SB_BRCM_MIRROR_DEST_WITH_ID);
  mirror_dest.src_addr = localIP;
  mirror_dest.dst_addr = 
        config->sflow_sampling_params.encap_dest_ip_addr.s_addr;

  memcpy(&mirror_dest.src_mac, localMac,BVIEW_MAC_ADDR_LEN);
  memcpy(&mirror_dest.dst_mac, &l3_host_info.l3a_nexthop_mac,BVIEW_MAC_ADDR_LEN);
  if (memcmp (mirror_dest.dst_mac, nullMAC, BVIEW_MAC_ADDR_LEN) ==0)
  {
    return BVIEW_STATUS_FAILURE;
  }
  mirror_dest.vlan_id = config->sflow_sampling_params.encap_vlan_id;
  if (mirror_dest.vlan_id != 0)
  {
    mirror_dest.tpid = 0x8100;
  }
  mirror_dest.udp_src_port = config->sflow_sampling_params.encap_src_udp_port;
  mirror_dest.udp_dst_port = config->sflow_sampling_params.encap_dest_udp_port;
  mirror_dest.version = 4;  /*ipv4 */
  mirror_dest.ttl =  16;
  mirror_dest.tos =  7;
  SB_BRCM_GPORT_MIRROR_SET(mirror_dest.mirror_dest_id, BVIEW_BHD_SFLOW_MIRROR_SESSION);

  /* create the destination only if is not present */
  memcpy(&mirror_temp_dest, &mirror_dest,sizeof(mirror_dest));
  rv =  SB_BRCM_MIRROR_DESTINATION_GET(asic, 
                                   mirror_dest.mirror_dest_id, 
                                   &mirror_temp_dest);
  if (rv == SB_BRCM_E_NOT_FOUND)
  {
    rv = SB_BRCM_MIRROR_PORT_DESTINATION_CREATE(asic, &mirror_dest);
  }

  if (rv == SB_BRCM_E_NONE)
  {
    rv = SB_BRCM_MIRROR_PORT_DEST_ADD(asic, port, 
                                      flags, 
                                      mirror_dest.mirror_dest_id);
  }
  if (rv == SB_BRCM_E_EXISTS)
  {
     rv = SB_BRCM_E_NONE;
  }
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Create Black Hold Detection rule
*
* @param[in]   asic                 - unit
* @param[out]  ruleId               - Rule ID
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if rule creation is failed.
* @retval BVIEW_STATUS_SUCCESS           if rule creation is success.
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_bhd_sflow_sampling_config (int asic,
                                                  int port,
                                                  int sampleSize)
{
  int  data = 0;
  int  rv  =0;

  /* clear stats for flex port */
  SB_BRCM_STAT_CLEAR_SINGLE(asic, port, SB_BRCM_SAMPLE_FLEX_PKTS);
  SB_BRCM_STAT_CLEAR_SINGLE(asic, port, SB_BRCM_SAMPLE_FLEX_SNAPSHOT_PKTS);
  SB_BRCM_STAT_CLEAR_SINGLE(asic, port, SB_BRCM_SAMPLE_FLEX_SAMPLED_PKTS);

  data |= SB_BRCM_PORT_CONTROL_SAMPLE_DEST_MIRROR;

  rv = SB_BRCM_PORT_CONTROL_SET(asic, port,
                            SB_BRCM_PORT_CONTROL_SAMPLE_FLEX_RATE,
                            sampleSize);
  if (SB_BRCM_E_NONE == rv)
  {
    rv = SB_BRCM_PORT_CONTROL_SET(asic, port,
                             SB_BRCM_PORT_CONTROL_SAMPLE_FLEX_DEST, data);
  }
  if (rv != SB_BRCM_E_NONE)
  {
    return BVIEW_STATUS_FAILURE;
  }

  rv = SB_BRCM_API_SWITCH_CONTROL_SET(asic, SB_BRCM_SWITCH_SAMPLE_INGRESS_RANDOM_SEED,
                              BVIEW_SFLOW_SAMPLE_RANDOM_SEED);
  if (rv != SB_BRCM_E_NONE)
  {
    return BVIEW_STATUS_FAILURE;
  }

  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief  Create Black Hold Detection rule 
*
* @param[in]   asic                 - unit
* @param[in]   bhdGroup             - Black Hold Detection policy ID
* @param[in]   statId               - Black Hold Detection stat ID
* @param[in]   InPbmp               - Port bitmap of black holed ports
* @param[in]   dst_port             - Destionation port
* @param[in]   config               - Pointer to Black Hold Detection 
*                                     config structure
* @param[out]  ruleId               - Rule ID
*
* @retval BVIEW_STATUS_INVALID_PARAMETER if input data is invalid.
* @retval BVIEW_STATUS_FAILURE           if rule creation is failed.
* @retval BVIEW_STATUS_SUCCESS           if rule creation is success.
*
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS sbplugin_common_bhd_rule_create (int asic ,
                                              SB_BRCM_FIELD_GROUP_CONFIG_t bhdGroup,
                                              SB_BRCM_FIELD_STAT_t statId,
                                              SB_BRCM_PBMP_t  InPbmp, 
                                              int dst_port,
                                              BVIEW_BLACK_HOLE_CONFIG_t  *config,
                                              SB_BRCM_FIELD_ENTRY_t *ruleId)
{
  SB_BRCM_FIELD_ENTRY_t        entry;
  SB_BRCM_POLICER_CONFIG_t     policer;
  SB_BRCM_POLICER_t            policer_id;
  int                          dst_modid=0,modid_mask =0xff;
  int                          port_mask =0xff;
  SB_BRCM_PORT_CONFIG_t        port_cfg;

  BVIEW_NULLPTR_CHECK(config);
  BVIEW_NULLPTR_CHECK(ruleId);
 
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_ENTRY_CREATE (asic, bhdGroup, &entry));
  
  if (SB_BRCM_RV_ERROR(SB_BRCM_API_PORT_CONFIG_GET(asic,&port_cfg)))
  {
    return BVIEW_STATUS_FAILURE;
  }

  /* Qualifiers: Ingress Ports
   *             DstPort
   */
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_QUALIFY_INPORTS (asic, entry, InPbmp, port_cfg.e));
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_QUALIFY_DSTPORT (asic, entry, 
                                      dst_modid, modid_mask,
                                      dst_port, port_mask));
  
  if (BVIEW_BHD_AGENT_SAMPLING == 
      config->sampling_config.sampling_method)
  {
    /* Create the policer and attach to the rule*/
    SB_BRCM_POLICER_CONFIG_INIT(&policer);
    policer.mode = SB_BRCM_POLICER_MODE_COMMITTED ;
    policer.flags |= (SB_BRCM_POLICER_MODE_PACKETS);
    policer.flags |= (SB_BRCM_POLICER_COLOR_BLIND);
    policer.ckbits_sec = 
       config->sampling_config.sampling_params.agent_sampling_params.water_mark;
    policer.ckbits_burst = 128;

    BVIEW_ERROR_RETURN (SB_BRCM_POLICER_CREATE (asic, &policer, &policer_id));
    BVIEW_ERROR_RETURN (SB_BRCM_FIELD_ENTRY_POLICER_ATTACH (asic, entry, 0, policer_id));
    /* Attach the same stat for all the entries*/
    BVIEW_ERROR_RETURN (SB_BRCM_FIELD_STAT_ATTACH (asic, entry, statId));
    BVIEW_ERROR_RETURN (SB_BRCM_FIELD_ACTION_ADD (asic, entry, 
            SB_BRCM_FIELD_ACTION_RP_COPYTOCPU,1, BVIEW_BHD_RULE_MATCH_ID));
    BVIEW_ERROR_RETURN (SB_BRCM_FIELD_ACTION_ADD (asic, entry,
                                        SB_BRCM_FIELD_ACTION_EGRESS_CLASS_SELECT,
                                        11,0));
    BVIEW_ERROR_RETURN (SB_BRCM_FIELD_ACTION_ADD (asic, entry, 
                                        SB_BRCM_FIELD_ACTION_NEW_CLASS_ID,
                                  BVIEW_BHD_RULE_CLASS_TAG_ID,0));  
  }
  else if (BVIEW_BHD_SFLOW_SAMPLING == 
           config->sampling_config.sampling_method)
  {
    /* Enable Ingress sampling action */
    BVIEW_ERROR_RETURN (SB_BRCM_FIELD_ACTION_ADD (asic, entry, 
                             SB_BRCM_FIELD_ACTION_INGSAMPLEENABLE,1,0));
  }
  *ruleId = entry;
  return BVIEW_STATUS_SUCCESS;
}
/*********************************************************************
* @brief  Create black hole policy and apply.
*
* @param[in]  asic               - unit
* @param[in]  config             - Black Hold Detection configuration structure
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
                                 BVIEW_BLACK_HOLE_CONFIG_t  *config)
{
  int                          rv = SB_BRCM_E_NONE;
  SB_BRCM_FIELD_QSET_t         qset;
  SB_BRCM_FIELD_ENTRY_t        entry;
  SB_BRCM_FIELD_GROUP_CONFIG_t bhdGroup;
  SB_BRCM_FIELD_STAT_t         stat_type = SB_BRCM_FIELD_STATS_PACKETS;
  SB_BRCM_PBMP_t               InPbmp, tempPbmp;
  int                          statId;
  int                          port = 0;
  BVIEW_STATUS                 bview_rc = BVIEW_STATUS_SUCCESS;
  BVIEW_BHD_PKT_SAMPLING_PARAMS_t   *samplingCfg;
  int                          probe_port = 0;
  int                          pool_size = 0;
  BVIEW_ASIC_TYPE              asicType = 0;
  
  BVIEW_NULLPTR_CHECK(config);

  rv = sbplugin_common_system_util_chip_type_get (asic, &asicType);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    return BVIEW_STATUS_FAILURE;
  }
#ifdef BVIEW_CHIP_QUMRAN
  if ((config->sampling_config.sampling_method == BVIEW_BHD_AGENT_SAMPLING) &&
      (asicType == BVIEW_ASIC_TYPE_QUMRAN))
  {
    return sbplugin_common_bhd_config_black_hole_dnx (asic, config);
  }
#endif
  samplingCfg = &config->sampling_config.sampling_params;

  sbplugin_bhd_config_init ();

  SB_BRCM_PBMP_CLEAR(InPbmp);
  SB_BRCM_PBMP_CLEAR (tempPbmp);

  SB_BRCM_FIELD_QSET_INIT(qset);
  SB_BRCM_FIELD_QSET_ADD(qset, SB_BRCM_QUALIFY_STAGE_INGRESS);
  SB_BRCM_FIELD_QSET_ADD(qset, SB_BRCM_QUALIFY_INPORTS);
  SB_BRCM_FIELD_QSET_ADD(qset, SB_BRCM_QUALIFY_DSTPORT );

  /* Build the BCM Port Mask*/
  BVIEW_SYSTEM_PORT_ITER (asic, port)
  {
    if (BVIEW_ISMASKBITSET (config->black_hole_port_mask, port))
    {  
      SB_BRCM_PBMP_PORT_ADD(InPbmp, port);
    }
  }
  /* Create the group*/
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_GROUP_CREATE (asic, qset,0, &bhdGroup));  
  /* Create stat for total packets*/
  stat_type = SB_BRCM_FIELD_STATS_PACKETS;
  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_STAT_CREATE (asic, bhdGroup, 1 , &stat_type, &statId));
  /* Loop through all the ports*/
  BVIEW_SYSTEM_PORT_ITER (asic, port)
  {
    SB_BRCM_PBMP_CLEAR(tempPbmp);
    SB_BRCM_PBMP_ASSIGN (tempPbmp, InPbmp);    
    if (BVIEW_ISMASKBITSET (config->black_hole_port_mask, port))
    {
      /* Remove the port from Ingress port bit map and add as destination port*/
      SB_BRCM_PBMP_PORT_REMOVE(tempPbmp, port);
      bview_rc = sbplugin_common_bhd_rule_create(asic, bhdGroup, 
                                                 statId, tempPbmp, 
                                                 port, config,
                                                 &entry);
      if (BVIEW_STATUS_SUCCESS != bview_rc)
      {
        return BVIEW_STATUS_FAILURE;
      }
      bhdInfo.policy.entry[port] = entry;
    }
  }

  rv = SB_BRCM_FIELD_GROUP_INSTALL (asic, bhdGroup); 
  if (rv != SB_BRCM_E_NONE)
  {
    return BVIEW_STATUS_FAILURE;
  }
  /* Update local table*/
  bhdInfo.policy.stat_type = stat_type;
  bhdInfo.policy.ingressGroupId = bhdGroup;
  bhdInfo.policy.statId = statId;
  SB_BRCM_PBMP_ASSIGN (bhdInfo.bhdPbmp,InPbmp);
  bhdInfo.sampling_type = BVIEW_BHD_AGENT_SAMPLING;
  if (config->sampling_config.sampling_method == BVIEW_BHD_AGENT_SAMPLING)
  {
    if (BVIEW_STATUS_SUCCESS == 
        sbplugin_common_bhd_egress_policy (asic, &bhdGroup, &entry))
    {
      bhdInfo.policy.egressGroupId = bhdGroup;
      bhdInfo.policy.egressEntry = entry;
    }
  }

  /* Configure SFLOW sampling and header properties */
  if (config->sampling_config.sampling_method == BVIEW_BHD_SFLOW_SAMPLING)
  {
    /* Mirroring with Encapsulation Support */
    BVIEW_ERROR_RETURN (SB_BRCM_API_SWITCH_CONTROL_SET(asic,SB_BRCM_SWITCH_DIRECTED_MIRRORING,1));
    BVIEW_ERROR_RETURN (SB_BRCM_API_SWITCH_CONTROL_SET(asic,SB_BRCM_SWITCH_FLEXIBLE_MIRROR_DESTINATIONS,1));
    /* When mirror mode is changed, the mirror module MUST be re-initialized */
    BVIEW_ERROR_RETURN(SB_BRCM_MIRROR_INIT(asic));

    probe_port = 
          samplingCfg->sflow_sampling_params.mirror_port;
    pool_size = 
          samplingCfg->sflow_sampling_params.sample_pool_size;

    bview_rc = sbplugin_bhd_sflow_mirror_config(asic,  
                                                probe_port,
                                                config);
    if (BVIEW_STATUS_SUCCESS != bview_rc)
    {
      return BVIEW_STATUS_FAILURE;
    }
     /* Loop through all the ports and configure sampling parameters*/
    BVIEW_SYSTEM_PORT_ITER (asic, port)
    {
      if (BVIEW_ISMASKBITSET (config->black_hole_port_mask, port))
      {
        bview_rc = sbplugin_bhd_sflow_sampling_config(asic, 
                                                      port,
                                                      pool_size);
        if (BVIEW_STATUS_SUCCESS != bview_rc)
        {
          return BVIEW_STATUS_FAILURE;
        }
      }
    }
    bhdInfo.sflow.probe_port =
          samplingCfg->sflow_sampling_params.mirror_port;
    bhdInfo.sampling_type = BVIEW_BHD_SFLOW_SAMPLING;
  }
  return BVIEW_STATUS_SUCCESS;
}
 
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
                                                int *sampling_cap)
{
  BVIEW_ASIC_TYPE asicType = 0;
  int rv =0;

  BVIEW_NULLPTR_CHECK (sampling_cap);

  rv = sbplugin_common_system_util_chip_type_get (asic, &asicType);  
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    return BVIEW_STATUS_FAILURE;
  }

  *sampling_cap |= BVIEW_BHD_AGENT_SAMPLING;

  if (asicType == BVIEW_ASIC_TYPE_TH)
  {
    *sampling_cap |= BVIEW_BHD_SFLOW_SAMPLING;
  } 
  return BVIEW_STATUS_SUCCESS; 
}

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
BVIEW_STATUS sbplugin_common_bhd_pkt_count_get (int asic, uint64_t *count)
{
  uint64 value;
  BVIEW_NULLPTR_CHECK (count);

  BVIEW_ERROR_RETURN (SB_BRCM_FIELD_STAT_GET (asic, bhdInfo.policy.statId, 
                            bhdInfo.policy.stat_type, &value)); 
  *count = value;
#ifdef BVIEW_CHIP_QUMRAN
  *count = (value + 1)/2;
#endif
  return BVIEW_STATUS_SUCCESS;
} 
  
/*********************************************************************
* @brief   Is packet Black holded
*
* @param[in]  pkt             -    Pointer to Packet structure
* @param[in]  user_header     -    Pointer to User Header
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
                                              unsigned char *user_header)
{
  BVIEW_NULLPTR_CHECK(pkt);

  if (SB_BRCM_PBMP_MEMBER(bhdInfo.bhdPbmp, pkt->src_port))
  {
    if (pkt->rx_matched == BVIEW_BHD_RULE_MATCH_ID ||
        user_header[0] == bhdInfo.destClassVal)
    {
      return BVIEW_STATUS_SUCCESS;
    }
  }
  return BVIEW_STATUS_FAILURE;
}



