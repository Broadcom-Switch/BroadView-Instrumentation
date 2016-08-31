/*! \file bhd.h
 */

/*! @addtogroup GLUSMDB API Definitions
 *  @{
 *    @addtogroup GLBVIEWBHD BroadView BHD Feature Declarations And Definitions [BVIEW]
 *    @{
 */

/*********************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename bhd.h 
  *
  * @purpose BroadView BHD Application public definitions 
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


#ifndef INCLUDE_BHD_H
#define INCLUDE_BHD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <netinet/in.h>
#include "broadview.h"
#include "sbplugin.h"
#include "asic.h"
#include "port_utils.h"
#include "packet_multiplexer.h"

  /* Max number of ports supported to participate in BHD */
#define BVIEW_BHD_MAX_SUPPORTED_PORTS           BVIEW_ASIC_MAX_PORTS
  /* Max size of the sampled packet reported in asynchronous BHD event reports */ 
#define BVIEW_BHD_MAX_PKT_SIZE                  BVIEW_MAX_PACKET_SIZE

#define BVIEW_BHD_MAX_UNITS                     1


#define BVIEW_BHD_WATERMARK_MIN                 100
#define BVIEW_BHD_WATERMARK_MAX                 10000

#define BVIEW_BHD_SAMPLE_PERIODICITY_MIN        1
#define BVIEW_BHD_SAMPLE_PERIODICITY_MAX        60*60

#define BVIEW_BHD_SAMPLE_COUNT_MIN              0
#define BVIEW_BHD_SAMPLE_COUNT_MAX              30

#define BVIEW_BHD_VLAN_MIN                      1
#define BVIEW_BHD_VLAN_MAX                      4094

#define BVIEW_BHD_SOURCE_UDP_PORT_MIN           1
#define BVIEW_BHD_SOURCE_UDP_PORT_MAX           65535

#define BVIEW_BHD_DEST_UDP_PORT_MIN             1
#define BVIEW_BHD_DEST_UDP_PORT_MAX             65535

#define BVIEW_BHD_SAMP_POOL_SIZE_MIN            1024
#define BVIEW_BHD_SAMP_POOL_SIZE_MAX            100000



  /* Sampled Black holed Packet information */
  typedef struct _bhd_sampled_pkt_info_
  {
    /* Ingress port  */ 
    unsigned int      ing_port;
    /* Bitmap of egress ports */ 
    BVIEW_PORT_MASK_t egr_pbmp; 

    /* Sampled packet data */
    unsigned char     pkt_data[BVIEW_BHD_MAX_PKT_SIZE];
    /* Packet length */
    unsigned int      pkt_len;
#if 0
    /* Total number of Black Holed packets */ 
    int               total_black_holed_pkts;
    /* Time stamp */ 
    BVIEW_TIME_t      report_time;
#endif
  } BVIEW_BHD_SAMPLED_PKT_INFO_t;



  /* Black Holed packet Sampling methods*/
  typedef enum _bhd_pkt_sampling_method_
  {
    /* Agent sampling */	
    BVIEW_BHD_AGENT_SAMPLING = (1 << 0),
    /* sFlow sampling */
    BVIEW_BHD_SFLOW_SAMPLING = (1 << 1),
    BVIEW_BHD_MAX_SAMPLING_METHOD

  } BVIEW_BHD_PKT_SAMPLING_METHOD_t;

#define BVIEW_BHD_DEFAULT_PKT_SAMPLING_METHOD  BVIEW_BHD_AGENT_SAMPLING

  /* Sampling Parameters */
  typedef union _bhd_pkt_sampling_params_
  {
    struct _bhd_agent_sampling_params_
    {
      /* This represents the traffic rate above 
         which traffic is considered as Black holed. 
         Sampling would start only after the water-mark level is crossed  */
      unsigned int water_mark;
      /* Time interval in seconds. */
      unsigned int sample_periodicity;
      /* Number of samples to be sent with in sample_periodicity interval */
      unsigned int sample_count;

    } agent_sampling_params;

    struct _bhd_sflow_sampling_params_
    {
      /* vlan-id  of sflow encapsulation header */
      unsigned int encap_vlan_id;
      /* Destination IP address sflow encapsulation header */
      struct in_addr encap_dest_ip_addr;
      /* Source udp port number of sflow encapsulation header */
      unsigned short encap_src_udp_port;
      /* Destination udp port number of sflow encapsulation header */
      unsigned short encap_dest_udp_port;
      /* Mirror Port number on which sflow encapsulated 
         sample packet is sent out                      */
      unsigned int mirror_port;
      /* Represents the packet pool size for sampling. 
         One packet is sampled for each pool. Minimum is 1024 */
      unsigned int sample_pool_size;

    } sflow_sampling_params;

  } BVIEW_BHD_PKT_SAMPLING_PARAMS_t;


  /* Sampling configuration */
  typedef struct _bhd_pkt_sampling_config_
  {
    /* sampling method */
    BVIEW_BHD_PKT_SAMPLING_METHOD_t sampling_method;
    /* Sampling Parameters */
    BVIEW_BHD_PKT_SAMPLING_PARAMS_t sampling_params; 

  } BVIEW_BHD_PKT_SAMPLING_CONFIG_t;

  typedef struct  _black_hole_config_
  {
    /* List of ports participating in BHD  */
    BVIEW_PORT_MASK_t black_hole_port_mask;
    /* Pkt sampling configuration */
    BVIEW_BHD_PKT_SAMPLING_CONFIG_t sampling_config;

  } BVIEW_BLACK_HOLE_CONFIG_t;

  typedef struct _bhd_config_feature_
  {
    bool enable;
  } BVIEW_CONFIGURE_BHD_FEATURE_t;

  /* Feature Configuration */
  typedef struct _bhd_config_
  {
    /* Fetaure enable/disable */
    BVIEW_CONFIGURE_BHD_FEATURE_t feature;
    /* Black hole is configured */
    bool black_hole_configured;
    /* Black Hole configuration */
    BVIEW_BLACK_HOLE_CONFIG_t bh_config;

  } BVIEW_BHD_CONFIG_t;


  /*sFlow sampling status per port */
  typedef struct _bhd_port_sflow_sampling_status_
  {
    /* Ingress port number */
    int port;
    /* sFlow sampling is enabled or not */ 
    bool sflowSamplingEnabled;
    /* Total number of packets sampled since sflow sampling is enabled */  
    unsigned int sampled_pkt_count;
    /* Total number of packets black holed since sflow sampling is enabled */  
    unsigned int black_holed_pkt_count;

  } BVIEW_BHD_PORT_SFLOW_SAMPLING_STATUS_t;


  /*sFlow sampling status */
  typedef struct _bhd_sflow_sampling_status_
  {
    BVIEW_TIME_t report_time;
    int num_ports;
    BVIEW_BHD_PORT_SFLOW_SAMPLING_STATUS_t port_sflow_sampling[BVIEW_ASIC_MAX_PORTS];

  } BVIEW_BHD_SFLOW_SAMPLING_STATUS_t;

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_BHD_H */


/*!  @}
 * @}
 */

