/*! \file packet_trace.h
 */

/*! @addtogroup GLUSMDB API Definitions
 *  @{
 *    @addtogroup GLBVIEWPT BroadView Packet Trace Feature Declarations And Definitions [BVIEW]
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

#ifndef INCLUDE_PACKET_TRACE_H
#define INCLUDE_PACKET_TRACE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "broadview.h"
#include "sbplugin.h"

#define     BVIEW_PT_MAX_PACKET_SIZE                1558 

/* Packet Including Checksum*/
typedef struct
{
  unsigned char     data[BVIEW_PT_MAX_PACKET_SIZE];
  unsigned int      pkt_len;
} BVIEW_PT_PACKET_t;


/* Drop Reason Types*/
typedef enum _pt_drop_reason_type_
{
   BVIEW_PT_VLAN_XLATE_MISS_DROP,
   BVIEW_PT_BPDU_DROP,
   BVIEW_PT_PROTOCOL_PKT_DROP,
   BVIEW_PT_CML_DROP,
   BVIEW_PT_SRC_ROUTE_DROP,
   BVIEW_PT_L2_SRC_DISCARD_DROP,
   BVIEW_PT_L2SRC_STATIC_MOVE_DROP,
   BVIEW_PT_L2DST_DISCARD_DROP,
   BVIEW_PT_CFI_OR_L3DISABLE_DROP,
   BVIEW_PT_HIGIG_MH_TYPE1_DROP,
   BVIEW_PT_DISC_STAGE_DROP,
   BVIEW_PT_SW1_INVALID_VLAN_DROP,
   BVIEW_PT_HIGIG_HDR_ERROR_DROP,
   BVIEW_PT_LAG_FAILOVER_DROP,
   BVIEW_PT_MACSA0_DROP,
   BVIEW_PT_VFP_DROP,
   BVIEW_PT_VLAN_CC_OR_PBT_DROP,
   BVIEW_PT_CLASS_BASED_SM_DROP,
   BVIEW_PT_MPLS_STAGE_DROP,
   BVIEW_PT_TIME_SYNC_PKT_DROP,
   BVIEW_PT_MY_STATION_DROP,
   BVIEW_PT_BAD_UDP_CHECKSUM_DROP,
   BVIEW_PT_NIV_FORWARDING_DROP,
   BVIEW_PT_NIV_RPF_CHECK_FAIL_DROP,
   BVIEW_PT_PVLAN_VP_EFILTER_DROP,
   BVIEW_PT_TRILL_HEADER_VERSION_NONZERO_DROP,
   BVIEW_PT_TRILL_ADJACENCY_CHECK_FAIL_DROP,
   BVIEW_PT_TRILL_RBRIDGE_LOOKUP_MISS_DROP,
   BVIEW_PT_TRILL_UC_HDR_MC_MACDA_DROP,
   BVIEW_PT_TRILL_SLOWPATH_DROP,
   BVIEW_PT_CORE_IS_IS_PKT_DROP,
   BVIEW_PT_TRILL_RPF_CHECK_FAIL_DROP,
   BVIEW_PT_TRILL_PKT_WITH_SNAP_ENCAP_DROP,
   BVIEW_PT_TRILL_ING_RBRIDGE_EQ_EGR_RBRIDGE_DROP,
   BVIEW_PT_TRILL_HOPCOUNT_CHECK_FAIL_DROP,
   BVIEW_PT_BFD_TERMINATED_DROP,
   BVIEW_PT_L3DST_DISCARD_DROP,
   BVIEW_PT_TUNNEL_DECAP_ECN_DROP,
   BVIEW_PT_L3_PKT_RESOURCE_DROP,
   BVIEW_PT_IPV4_HEADER_ERROR_DROP,
   BVIEW_PT_L3_IPV6_PKT_RESOURCE_DROP,
   BVIEW_PT_IPV6_HEADER_ERROR_DROP,
   BVIEW_PT_INGRESS_PORT_FWD_STATE_DROP,
   BVIEW_PT_FIELD_PROCESSOR_DROP,
   BVIEW_PT_MULTICAST_DROP,
   BVIEW_PT_BITMAP_ZERO_DROP,
   BVIEW_PT_POLICY_DROP,
   BVIEW_PT_DOS_L4_HEADER_ERROR_DROP,
   BVIEW_PT_MTU_ERROR_DROP,
   BVIEW_PT_RECV_TUNNELED_PKT_ERROR_DROP,
   BVIEW_PT_VLAN_ERROR_DROP,
   BVIEW_PT_HIGIG_HEADER_ERROR_DROP,
   BVIEW_PT_MC_IPMC_INDEX_ERROR_DROP,
   BVIEW_PT_PARITY_ERROR_DROP,
   BVIEW_PT_REVERSE_RPF_ERROR_DROP,
   BVIEW_PT_DST_DISCARD_BIT_ERROR_DROP,
   BVIEW_PT_ECMP_CALCULATION_ERROR_DROP,
   BVIEW_PT_MPLS_SEQ_NUM_ERROR_DROP,
   BVIEW_PT_FIELD_PROCESSOR_REDIRECT_ERROR_DROP,
   BVIEW_PT_INVALID_L2MC_ENTRY_DROP,
   BVIEW_PT_NEXTHOP_ZERO_DROP,
   /* This should be last always*/
   BVIEW_PT_MAX_DROP_REASON
} BVIEW_PT_DROP_REASON_TYPE_t;

/*Drop Counters report*/
typedef struct _pt_drop_counter_report_
{
  struct _counter_
  {
      uint64_t  counter;
  } data[BVIEW_PT_MAX_DROP_REASON][BVIEW_ASIC_MAX_PORTS];

} BVIEW_PT_DROP_COUNTER_REPORT_t;


/* Callback Data when packet is droped with configured drop reason*/ 
typedef struct _pt_drop_reason_data_
{
   BVIEW_PT_DROP_REASON_TYPE_t dropReason;
   int                         port;
   BVIEW_PT_PACKET_t           packet; 
} BVIEW_PT_DROP_REASON_DATA_t;

/* The callback for invoking when a packet is droped */
typedef BVIEW_STATUS(*BVIEW_PT_TRIGGER_CALLBACK_t) (int asic,
        void *cookie,
        BVIEW_PT_DROP_REASON_DATA_t dropReasonData);

/* Base type for declarations */
#define     BVIEW_MASK_BASE_UNIT     unsigned int
#define     BVIEW_MASKWID            (8*sizeof(BVIEW_MASK_BASE_UNIT)) 

/* (internal) Number of BVIEW_MASK_BASE_UNITs needed to contain _max bits */
#define     BVIEW_MASK_SIZE(_max)    (((_max) + BVIEW_MASKWID - 1) / BVIEW_MASKWID)

/* Interface storage */
typedef struct
{
  BVIEW_MASK_BASE_UNIT   value[BVIEW_MASK_SIZE(BVIEW_ASIC_MAX_PORTS)];
} BVIEW_PORT_MASK_t;


typedef struct
{
  BVIEW_MASK_BASE_UNIT   value[BVIEW_MASK_SIZE(BVIEW_PT_MAX_DROP_REASON)];
} BVIEW_PT_DROP_REASON_MASK_t;

/* Drop Reason Configuration*/
typedef struct _pt_drop_reason_config_
{
    /* Mask of Drop Reasons */
    BVIEW_PT_DROP_REASON_MASK_t dropReasonMask;
    /* Port Bitmap */
    BVIEW_PORT_MASK_t  pbmp;
    /* Determines whether the send copy of the dropped packets or not.*/
    bool          sendDropedPacket;
    /* Determines whether a trace-profile be sent for each dropped packet. */
    bool          traceProfile;
    /* Number of packets (default 1) to be sent for a given drop reason.*/
    int           packetCount;
    /* A minimum number of packets to be dropped,
     * exceeding which the dropped packet will be sent to the requestor.*/
    int           packetThreshold;

} BVIEW_PT_DROP_REASON_CONFIG_t;



typedef enum _bview_pt_hashing_info_mask_
{
  BVIEW_PT_NO_HASHING_RESOLUTION,
  /* level 1 ecmp hashing resolution done */
  BVIEW_PT_ECMP_1_RESOLUTION,
  /* level 2 ecmp hashing resolution done */
  BVIEW_PT_ECMP_2_RESOLUTION,
  /* trunk hashing resolution done*/
  BVIEW_PT_TRUNK_RESOLUTION,
  /* hg trunk hashing resolution done*/
  BVIEW_PT_FABRIC_TRUNK_RESOLUTION
} BVIEW_PT_HASHING_INFO_MASK_t; 


#define     BVIEW_MAX_TRUNK_MEMBERS         8
typedef struct _bview_pt_lag_hashing_info_
{
   int trunk;                         /* destination trunk group */
   int trunk_member;                  /* destination member port which packet egress. */
   int trunk_members[BVIEW_MAX_TRUNK_MEMBERS];            /* Port Bitmap of trunk members*/
   int fabric_trunk;                  /* destination hg trunk group */
   int fabric_trunk_member;           /* destination member hg port which packet will egress. */
   int fabric_trunk_members[BVIEW_MAX_TRUNK_MEMBERS];     /* Port Bitmap of trunk members*/
} BVIEW_PT_LAG_HASHING_INFO_t;

#define                  BVIEW_ECMP_MAX_MEMBERS       8

typedef struct _bview_ecmp_member_info_
{
  int member;          /* member information */
  int port;            /*  port */
  int ip;              /*  IP */
} BVIEW_ECMP_MEMBER_t; 

typedef struct _bview_ecmp_lag_hashing_info_
{
  int ecmp_group;                  /* multipath egress forwarding object. */
  BVIEW_ECMP_MEMBER_t ecmp_group_members[BVIEW_ECMP_MAX_MEMBERS];           
  BVIEW_ECMP_MEMBER_t ecmp_egress_info;
} BVIEW_PT_ECMP_HASHING_INFO_t;

#define                  BVIEW_ECMP_MAX_LEVEL          2


/* packet hashing resolution information */
typedef struct _bview_pt_hashing_info_ 
{
    BVIEW_PT_HASHING_INFO_MASK_t flags;        
    BVIEW_PT_LAG_HASHING_INFO_t  lag;
    BVIEW_PT_ECMP_HASHING_INFO_t ecmp[BVIEW_ECMP_MAX_LEVEL];
} BVIEW_PT_HASHING_INFO_t;


/* packet trace resolution enums */
typedef enum _pt_resolution_type_t
{
    BVIEW_PT_RESOLUTIONUNKOWN = 0,
    BVIEW_PT_RESOLUTIONCONTROLPKT = 1, /* 1 */
    BVIEW_PT_RESOLUTIONOAMPKT = 2,
    BVIEW_PT_RESOLUTIONBFDPKT = 3,
    BVIEW_PT_RESOLUTIONBPDUPKT = 4,
    BVIEW_PT_RESOLUTION1588PKT = 6,
    BVIEW_PT_RESOLUTIONKNOWNL2UCPKT = 8,
    BVIEW_PT_RESOLUTIONUNKNOWNL2UCPKT = 9,
    BVIEW_PT_RESOLUTIONKNOWNL2MCPKT = 10,
    BVIEW_PT_RESOLUTIONUNKNOWNL2MCPKT = 11,
    BVIEW_PT_RESOLUTIONL2BCPKT = 12,
    BVIEW_PT_RESOLUTIONKNOWNL3UCPKT = 16,
    BVIEW_PT_RESOLUTIONUNKNOWNL3UCPKT = 17,
    BVIEW_PT_RESOLUTIONKNOWNIPMCPKT = 18,
    BVIEW_PT_RESOLUTIONUNKNOWNIPMCPKT = 19,
    BVIEW_PT_RESOLUTIONKNOWNMPLSL2PKT = 24,
    BVIEW_PT_RESOLUTIONUNKNOWNMPLSPKT = 25,
    BVIEW_PT_RESOLUTIONKNOWNMPLSL3PKT = 26,
    BVIEW_PT_RESOLUTIONKNOWNMPLSPKT = 28,
    BVIEW_PT_RESOLUTIONKNOWNMIMPKT = 32,
    BVIEW_PT_RESOLUTIONUNKNOWNMIMPKT = 33,
    BVIEW_PT_RESOLUTIONKNOWNTRILLPKT = 40,
    BVIEW_PT_RESOLUTIONUNKNOWNTRILLPKT = 41,
    BVIEW_PT_RESOLUTIONKNOWNNIVPKT = 48,
    BVIEW_PT_RESOLUTIONUNKNOWNNIVPKT = 49,
    BVIEW_PT_RESOLUTIONKNOWNL2GREPKT = 50,
    BVIEW_PT_RESOLUTIONKNOWNVXLANPKT = 51,
    BVIEW_PT_RESOLUTIONCOUNT = 52
} BVIEW_PT_RESOULTION_t; 

/* packet trace lookup result enums */
typedef enum _pt_lookup_result_type_
{
    BVIEW_PT_LOOKUPINVALID = 0,
    BVIEW_PT_LOOKUPFIRSTVLANTRANSLATIONHIT = 1,
    BVIEW_PT_LOOKUPSECONDVLANTRANSLATIONHIT = 2,
    BVIEW_PT_LOOKUPFORWARDINGVLANVALID = 3,
    BVIEW_PT_LOOKUPL2SRCHIT = 6,
    BVIEW_PT_LOOKUPL2SRCSTATIC = 7,
    BVIEW_PT_LOOKUPL2DSTHIT = 8,
    BVIEW_PT_LOOKUPL2CACHEHIT = 9,
    BVIEW_PT_LOOKUPL3SRCHOSTHIT = 10,
    BVIEW_PT_LOOKUPL3DESTHOSTHIT = 11,
    BVIEW_PT_LOOKUPL3DESTROUTEHIT = 12,
    BVIEW_PT_LOOKUPL2SRCMISS = 13,
    BVIEW_PT_LOOKUPDOSATTACK = 14,
    BVIEW_PT_LOOKUPIPTUNNELHIT = 15,
    BVIEW_PT_LOOKUPMPLSLABEL1HIT = 16,
    BVIEW_PT_LOOKUPMPLSLABEL2HIT = 17,
    BVIEW_PT_LOOKUPMPLSTERMINATED = 18,
    BVIEW_PT_LOOKUPMYSTATIONHIT = 19,
    BVIEW_PT_LOOKUPCOUNT = 20
} BVIEW_PT_LOOKUP_RESULT_TYPE_t;


/* lookup result set bit map */
typedef struct
{
  BVIEW_MASK_BASE_UNIT   value[BVIEW_MASK_SIZE(BVIEW_PT_LOOKUPCOUNT)]; /* bit map for packet trace lookup result set */
} BVIEW_PT_LOOKUP_RESULT_MASK_t;


typedef enum _stp_state_
{
  BVIEW_DISABLED =0,
  BVIEW_FORWARDING,
  BVIEW_DISCRADING,
  BVIEW_LEARNING
} BVIEW_PT_STP_STATE_t;
  
/* Trace Profile */
typedef struct  _pt_trace_profile_
{
    /* port*/
    int                        port;
    /* Packet Including Checksum*/
    BVIEW_PT_PACKET_t          packet;
    /* packet trace lookup status */
    BVIEW_PT_LOOKUP_RESULT_MASK_t   lookupResult;
    /* packet trace resolution result */
    BVIEW_PT_RESOULTION_t      packetResolution;
    /* packet trace hashing information */
    BVIEW_PT_HASHING_INFO_t    hashingInfo;
    /* packet trace ingress stp state */
    BVIEW_PT_STP_STATE_t       stpState;

} BVIEW_PT_TRACE_PROFILE_t;


#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_PACKET_TRACE_H */

