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

#ifndef INCLUDE_BST_APP_H
#define INCLUDE_BST_APP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include "modulemgr.h"

#define MSG_QUEUE_ID_TO_BST  0x100
#define MSG_QUEUE_ID_TO_BST_TRIGGER  0x108


#define  BVIEW_BST_APP_NUM_COS_PORT    8

#define _BST_DEBUG
#define _BST_DEBUG_LEVEL        0x0 

#define _BST_DEBUG_TRACE        (0x1)
#define _BST_DEBUG_INFO         (0x01 << 1)
#define _BST_DEBUG_ERROR        (0x01 << 2)
#define _BST_DEBUG_ALL          (0xFF)

#ifdef _BST_DEBUG
#define _BST_LOG(level, format,args...)   do { \
              if ((level) & _BST_DEBUG_LEVEL) { \
                                printf(format, ##args); \
                            } \
          }while(0)
#else
#define _BST_LOG(level, format,args...)
#endif


/* Default values for BST configurations */
  /* bst enable */
#define BVIEW_BST_DEFAULT_ENABLE    false  

#define BVIEW_BST_PERIODIC_REPORT_DEFAULT false
  /* default bst collection interval.
     */
#define BVIEW_BST_DEFAULT_INTERVAL  60
#define BVIEW_BST_DEFAULT_STATS_UNITS  false 
#define BVIEW_BST_DEFAULT_STATS_PERCENTAGE false 
#define BVIEW_BST_DEFAULT_TRACK_INGRESS   true
#define BVIEW_BST_DEFAULT_TRACK_EGRESS    true
#define BVIEW_BST_DEFAULT_TRACK_DEVICE    true

#define BVIEW_BST_DEFAULT_TRACK_IN_P_PG      true
#define BVIEW_BST_DEFAULT_TRACK_IN_P_SP      true
#define BVIEW_BST_DEFAULT_TRACK_IN_SP        true
#define BVIEW_BST_DEFAULT_TRACK_E_P_SP       true
#define BVIEW_BST_DEFAULT_TRACK_E_SP         true
#define BVIEW_BST_DEFAULT_TRACK_E_UC_Q       true
#define BVIEW_BST_DEFAULT_TRACK_E_UC_QG      true
#define BVIEW_BST_DEFAULT_TRACK_E_MC_Q       true
#define BVIEW_BST_DEFAULT_TRACK_E_CPU_Q      true
#define BVIEW_BST_DEFAULT_TRACK_E_RQE_Q      true
#define BVIEW_BST_DEFAULT_TRACK_MODE         BVIEW_BST_MODE_CURRENT

#define BVIEW_BST_DEFAULT_MAX_TRIGGERS 1
#define BVIEW_BST_DEFAULT_SNAPSHOT_TRIGGER true 
#define BVIEW_BST_DEFAULT_TRIGGER_INTERVAL 1 
#define BVIEW_BST_DEFAULT_SEND_INCR_REPORT  1 
#define BVIEW_BST_MAX_UNITS 8
#define BVIEW_BST_TIME_CONVERSION_FACTOR 1000

/* Maximum number of failed Receive messages */
#define BVIEW_BST_MAX_QUEUE_SEND_FAILS      10


/* congestion drop counters changes - start */
#define BVIEW_BST_NUM_UCAST_DROP_CTRS_PORT  BVIEW_BST_APP_NUM_COS_PORT
#define BVIEW_BST_NUM_MCAST_DROP_CTRS_PORT  BVIEW_BST_APP_NUM_COS_PORT
#define BVIEW_BST_NUM_TOTAL_DROP_CTRS_PORT 1

#define BVIEW_BST_MAX_CGSN_CTRS_PORT (BVIEW_BST_NUM_UCAST_DROP_CTRS_PORT + \
                                      BVIEW_BST_NUM_MCAST_DROP_CTRS_PORT + \
                                      BVIEW_BST_NUM_TOTAL_DROP_CTRS_PORT)


#define BVIEW_BST_TOTAL_DROP_CTRS (BVIEW_BST_MAX_CGSN_CTRS_PORT * BVIEW_ASIC_MAX_PORTS)


#define BVIEW_BST_UCAST_DROP_CTR_START  0 
#define BVIEW_BST_MCAST_DROP_CTR_START  (BVIEW_ASIC_MAX_PORTS * BVIEW_BST_NUM_UCAST_DROP_CTRS_PORT) 
#define BVIEW_BST_TOTAL_DROP_CTR_START  (BVIEW_ASIC_MAX_PORTS * (BVIEW_BST_NUM_UCAST_DROP_CTRS_PORT + BVIEW_BST_NUM_MCAST_DROP_CTRS_PORT)) 
  

#define BVIEW_BST_UCAST_DROP_CTR_INDEX_GET(__prt__, __queue_) \
  (BVIEW_BST_UCAST_DROP_CTR_START + ((__prt__-1)*BVIEW_BST_NUM_UCAST_DROP_CTRS_PORT)+ __queue_)


#define BVIEW_BST_MCAST_DROP_CTR_INDEX_GET(__prt__, __queue_) \
  (BVIEW_BST_MCAST_DROP_CTR_START + ((__prt__-1)*BVIEW_BST_NUM_MCAST_DROP_CTRS_PORT)+ __queue_)

#define BVIEW_BST_TOTAL_DROP_CTR_INDEX_GET(__prt__) \
   (BVIEW_BST_TOTAL_DROP_CTR_START + (__prt__-1))


typedef struct _bst_cgsn_drop_ctr_s_
{
  unsigned int port;
  unsigned int queue;
  BVIEW_BST_CGSN_CTR_TYPE_t type;
  uint64_t ctr;
}BVIEW_BST_CGSN_CTR_t;

typedef struct _bst_cgsn_drops_
{
  BVIEW_BST_CGSN_CTR_t  drop_ctrs[BVIEW_BST_TOTAL_DROP_CTRS];
  BVIEW_TIME_t tv;
  unsigned int id;
  BSTJSON_GET_BST_CGSN_DROP_CTRS_t rcvd_req;
}BVIEW_BST_CGSN_DROPS_t;

/* congestion drop counters changes - end */

typedef BSTJSON_CONFIGURE_BST_TRACKING_t  BVIEW_BST_TRACK_PARAMS_t;
typedef BSTJSON_CONFIGURE_BST_FEATURE_t   BVIEW_BST_CONFIG_PARAMS_t;
typedef BSTJSON_REPORT_OPTIONS_t          BVIEW_BST_REPORT_OPTIONS_t;
typedef BSTJSON_GET_BST_REPORT_t          BVIEW_BST_STAT_COLLECT_CONFIG_t;
typedef BSTJSON_CONFIGURE_BST_THRESHOLDS_t BVIEW_BST_THRESHOLD_CONFIG_t;
typedef BSTJSON_GET_BST_CGSN_DROP_CTRS_t   BVIEW_BST_CGSN_DRP_CTRS_t;


typedef enum _bst_report_type_ {
  BVIEW_BST_STATS = 1,
  BVIEW_BST_THRESHOLD,
  BVIEW_BST_PERIODIC,
  BVIEW_BST_STATS_TRIGGER
}BVIEW_BST_REPORT_TYPE_t;

/* threshold types */
typedef enum _bst_threshold_realm_ {
  BVIEW_BST_DEVICE_THRESHOLD = 1,
  BVIEW_BST_EGRESS_PORT_SP_THRESHOLD,
  BVIEW_BST_EGRESS_SP_THRESHOLD,
  BVIEW_BST_EGRESS_UC_QUEUE_THRESHOLD,
  BVIEW_BST_EGRESS_UC_QUEUEGROUPS_THRESHOLD,
  BVIEW_BST_EGRESS_MC_QUEUE_THRESHOLD,
  BVIEW_BST_EGRESS_CPU_QUEUE_THRESHOLD,
  BVIEW_BST_EGRESS_RQE_QUEUE_THRESHOLD,
  BVIEW_BST_INGRESS_PORT_PG_THRESHOLD,
  BVIEW_BST_INGRESS_PORT_SP_THRESHOLD,
  BVIEW_BST_INGRESS_SP_THRESHOLD
}BVIEW_BST_THRESHOLD_TYPE_t;


typedef enum _bst_trigger_index_ {
  BST_ID_DEVICE = 0,
  BST_ID_ING_POOL,
  BST_ID_PORT_POOL,
  BST_ID_PRI_GROUP_SHARED, 
  BST_ID_PRI_GROUP_HEADROOM,
  BST_ID_EGR_POOL,
  BST_ID_EGR_MCAST_POOL, 
  BST_ID_UCAST, 
  BST_ID_MCAST,
  BST_ID_EGR_UCAST_PORT_SHARED,
  BST_ID_EGR_PORT_SHARED, 
  BST_ID_RQE_QUEUE, 
  BST_ID_UCAST_GROUP, 
  BST_ID_MAX
}BST_TRIGGER_INDEX_t;


#define BVIEW_BST_MAX_THRESHOLD_TYPE_MIN BVIEW_BST_DEVICE_THRESHOLD
#define BVIEW_BST_MAX_THRESHOLD_TYPE_MAX BVIEW_BST_INGRESS_SP_THRESHOLD

/* structure to map the trigger index to realm and counter */
typedef struct _bst_trigger_realm_map_ {
    BST_TRIGGER_INDEX_t index;
    char *realm;
    char *counter;
}BST_REALM_COUNTER_INDEX_t;
/* BST command enums */
typedef enum _bst_cmd_ {
  /* Set group */
  BVIEW_BST_CMD_API_SET_TRACK = 1,
  BVIEW_BST_CMD_API_SET_FEATURE,
  BVIEW_BST_CMD_API_SET_THRESHOLD,
  BVIEW_BST_CMD_API_CLEAR_THRESHOLD,
  BVIEW_BST_CMD_API_CLEAR_STATS,
  /* get group */
  BVIEW_BST_CMD_API_GET_REPORT,
  BVIEW_BST_CMD_API_GET_FEATURE,
  BVIEW_BST_CMD_API_GET_TRACK,
  BVIEW_BST_CMD_API_GET_THRESHOLD,
  BVIEW_BST_CMD_API_TRIGGER_REPORT,
  BVIEW_BST_CMD_API_CLEAR_TRIGGER_COUNT,
  BVIEW_BST_CMD_API_ENABLE_BST_ON_TRIGGER,
  BVIEW_BST_CMD_API_TRIGGER_COLLECT,
  BVIEW_BST_CMD_API_GET_CGSN_DRP_CTRS,
  BVIEW_BST_CMD_API_MAX
}BVIEW_FEATURE_BST_CMD_API_t;

#define BST_MAX_REST_API  BVIEW_BST_CMD_API_GET_SWITCH_PROPERTIES

 /* structures to hold the bst related params */
  typedef struct bst_config {
    BVIEW_BST_TRACK_PARAMS_t  track;
    BVIEW_BST_CONFIG_PARAMS_t config;
  } BVIEW_BST_CFG_PARAMS_t;

  typedef struct _bst_report_snapshot_data_ {
    BVIEW_TIME_t tv;
    BVIEW_BST_ASIC_SNAPSHOT_DATA_t snapshot_data;
  }BVIEW_BST_REPORT_SNAPSHOT_t;

  typedef struct _bst_report_respose_ {
    BVIEW_BST_REPORT_SNAPSHOT_t *active;
    BVIEW_BST_REPORT_SNAPSHOT_t *backup;
  }BVIEW_BST_REPORT_RESP_t;

  typedef struct _bst_request_msg_ {
    long msg_type; /* message type */
    int unit; /* variable to hold the asic type */
    void *cookie;
    int id; /* id passed from the request */
    int version; /* json version */
    BVIEW_BST_REPORT_TYPE_t report_type; 
    char realm[JSON_MAX_NODE_LENGTH];
    unsigned int threshold_type;
    BVIEW_BST_THRESHOLD_CONFIG_t threshold;
     /* trigger info */
     BVIEW_BST_TRIGGER_INFO_t triggerInfo;
    union
    {
      /* feature params */
      BVIEW_BST_CONFIG_PARAMS_t config;
      /* params configured to track */
      BVIEW_BST_TRACK_PARAMS_t  track;
      /* params requsted to collect in report */
      BVIEW_BST_STAT_COLLECT_CONFIG_t   collect;
      /* congestion drop counters */
      BSTJSON_GET_BST_CGSN_DROP_CTRS_t drp_ctrs;
      /* params to config threshold */
      BVIEW_BST_DEVICE_THRESHOLD_t                       device_threshold;
      BVIEW_BST_INGRESS_PORT_PG_THRESHOLD_t              i_p_pg_threshold;
      BVIEW_BST_INGRESS_PORT_SP_THRESHOLD_t              i_p_sp_threshold;
      BVIEW_BST_INGRESS_SP_THRESHOLD_t                   i_sp_threshold;
      BVIEW_BST_EGRESS_PORT_SP_THRESHOLD_t               ep_sp_threshold;
      BVIEW_BST_EGRESS_SP_THRESHOLD_t                    e_sp_threshold;
      BVIEW_BST_EGRESS_UC_QUEUE_THRESHOLD_t              e_ucq_threshold;
      BVIEW_BST_EGRESS_UC_QUEUEGROUPS_THRESHOLD_t        e_ucqg_threshold;
      BVIEW_BST_EGRESS_MC_QUEUE_THRESHOLD_t              e_mcq_threshold;
      BVIEW_BST_EGRESS_CPU_QUEUE_THRESHOLD_t             cpu_q_threshold;
      BVIEW_BST_EGRESS_RQE_QUEUE_THRESHOLD_t             rqe_q_threshold;


    }request;
    unsigned int threshold_config_mask;
  }BVIEW_BST_REQUEST_MSG_t;


  typedef struct _bst_response_msg_ {
    long msg_type;
    int unit;
    void *cookie;
    int id;
    BVIEW_BST_REPORT_TYPE_t report_type; 
    BVIEW_ASIC_CAPABILITIES_t  *asic_capabilities;
    BVIEW_BST_REPORT_OPTIONS_t options;
    BVIEW_STATUS rv; /* return value for set request */
    union
    {
      BVIEW_BST_CONFIG_PARAMS_t *config;
      BVIEW_BST_TRACK_PARAMS_t  *track;
      BVIEW_BST_REPORT_RESP_t   report;
      BVIEW_BST_CGSN_DROPS_t   *drp_ctr_report; 
    }response;
  }BVIEW_BST_RESPONSE_MSG_t;

  typedef struct _bst_timer_context_s_ {
    unsigned int unit;
    unsigned int index;
    long cmd;
  }BVIEW_BST_TIMER_CONTEXT_t;

  typedef struct _bst_timer_s_ {
    bool in_use;
    BVIEW_BST_TIMER_CONTEXT_t context;
    timer_t bstTimer;
  }BVIEW_BST_TIMER_t;

  typedef struct _bst_drop_ctrs_elem_s_ {
    bool in_use;
    unsigned int id;
    long type;
    BSTJSON_GET_BST_CGSN_DROP_CTRS_t req;
    BVIEW_BST_TIMER_t cgsn_drop;
  }BVIEW_BST_CGSN_DROP_ELEM_t;


  typedef struct _bst_data_ {
    BVIEW_BST_TIMER_t bst_collection_timer;
    BVIEW_BST_TIMER_t bst_trigger_timer;
    BVIEW_BST_CFG_PARAMS_t bst_config;
    BVIEW_BST_STAT_COLLECT_CONFIG_t  bst_stats_config;
    BVIEW_BST_CGSN_DROP_ELEM_t drop_ctrs_db[BVIEW_MAX_REQUESTS];
  } BVIEW_BST_DATA_t;

typedef BVIEW_STATUS(*BVIEW_BST_THRESHOLD_API_HANDLER_t) (BSTJSON_CONFIGURE_BST_THRESHOLDS_t *cmd,
                                              BVIEW_BST_REQUEST_MSG_t * msg_data);
/* structure to map realm to threshold type */
typedef struct _bst_realm_to_threshold_ {
  /* realm string */
  char *realm;
  /* threshold  type*/
  BVIEW_BST_THRESHOLD_TYPE_t threshold;
  /* threshold validation handler */
  BVIEW_BST_THRESHOLD_API_HANDLER_t handler;
}BVIEW_BST_REALM_THRESHOLD_t;


typedef BVIEW_STATUS(*BVIEW_BST_THRESHOLD_HANDLER_t) (BVIEW_BST_REQUEST_MSG_t * msg_data);
/* structure to map realm to threshold set function */
typedef struct _bst_realm_to_thresholdset_ {
  /* threshold  type*/
  BVIEW_BST_THRESHOLD_TYPE_t threshold;
  /* threshold validation handler */
  BVIEW_BST_THRESHOLD_HANDLER_t handler;
}BVIEW_BST_REALM_THRESHOLD_HANDLER_t;



typedef struct _bst_context_unit_info__
{
  /* stats records */
  BVIEW_BST_REPORT_SNAPSHOT_t *stats_active_record_ptr;
  BVIEW_BST_REPORT_SNAPSHOT_t *stats_backup_record_ptr;
  BVIEW_BST_REPORT_SNAPSHOT_t *stats_current_record_ptr;
  /* threshold records */
  BVIEW_BST_REPORT_SNAPSHOT_t *threshold_record_ptr;

  /* congestion drop counter records */
  BVIEW_BST_CGSN_DROPS_t *cgsn_drp_curr;
  BVIEW_BST_CGSN_DROPS_t *cgsn_drp_active;

  /* place holder to store the bst max buffer settings */
  BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t bst_max_buffers;
  BVIEW_BST_ASIC_SNAPSHOT_DATA_t bst_thresholds_cache;

  /* config data */
  BVIEW_BST_DATA_t *bst_data;
  /* lock for this unit */
  pthread_mutex_t bst_mutex;

  /* Read-Write lock for config local data */
  pthread_rwlock_t bst_configRWLock;


  /* asic capabilities */
  BVIEW_ASIC_CAPABILITIES_t asic_capabilities;

  /* trigger callback cookie */
  int cb_cookie;
  unsigned int bst_trigger_count[BST_ID_MAX];

} BVIEW_BST_UNIT_CXT_t;


typedef struct _bst_context_info__
{
  BVIEW_BST_UNIT_CXT_t unit[BVIEW_BST_MAX_UNITS];
  /* BST Key to Queue Message*/
  key_t key1;
  key_t trigger_key;
  /* message queue id for bst */
  int recvMsgQid;
  int recvTriggerMsgQid;
    /* pthread ID*/
  pthread_t bst_thread;
  pthread_t bst_trigger_thread;
} BVIEW_BST_CXT_t;


typedef BVIEW_STATUS(*BVIEW_BST_API_HANDLER_t) (BVIEW_BST_REQUEST_MSG_t * msg_data);


/** Definition of an BST API */
typedef struct _feature_bst_api_
{
  /** bst command */
  int bst_command;
  /** Handler associated with the corresponding bst command */
  BVIEW_BST_API_HANDLER_t    handler;
}BVIEW_BST_API_t;



#define BST_CONFIG_PTR_GET(id)  &bst_info.unit[id].bst_data->bst_config
#define BST_CONFIG_FEATURE_PTR_GET(id)  &bst_info.unit[id].bst_data->bst_config.config
#define BST_CONFIG_TRACK_PTR_GET(id)  &bst_info.unit[id].bst_data->bst_config.track
#define BST_UNIT_DATA_PTR_GET(id)    bst_info.unit[id].bst_data
#define BST_UNIT_PTR_GET(id)    &bst_info.unit[id]



/* Macro to acquire lock */
#define BST_LOCK_TAKE(_unit)                                                        \
        {                                                                           \
           BVIEW_BST_UNIT_CXT_t *_ptr;                                              \
           _ptr = BST_UNIT_PTR_GET (_unit);                                         \
           if (0 != pthread_mutex_lock (&_ptr->bst_mutex))                          \
           {                                                                        \
              LOG_POST (BVIEW_LOG_ERROR,                                            \
                  "Failed to take the lock for unit %d.\r\n",_unit);                \
              return BVIEW_STATUS_FAILURE;                                          \
           }                                                                        \
         }
/*  to release lock*/
#define BST_LOCK_GIVE(_unit)                                                        \
         {                                                                          \
           BVIEW_BST_UNIT_CXT_t *_ptr;                                              \
           _ptr = BST_UNIT_PTR_GET (_unit);                                         \
           if (0 != pthread_mutex_unlock(&_ptr->bst_mutex))                         \
           {                                                                        \
              LOG_POST (BVIEW_LOG_ERROR,                                            \
              "Failed to Release the lock for unit %d.\r\n",_unit);                 \
               return BVIEW_STATUS_FAILURE;                                         \
            }                                                                       \
          }



/* Macro to acquire read lock */
#define BST_RWLOCK_RD_LOCK(_unit)                             \
{                                                                          \
  BVIEW_BST_UNIT_CXT_t *_ptr;                                              \
_ptr = BST_UNIT_PTR_GET (_unit);                                         \
  if (pthread_rwlock_rdlock(&_ptr->bst_configRWLock) != 0)                    \
  {                                                         \
    LOG_POST (BVIEW_LOG_ERROR,                                            \
        "Failed to take the rw lock for unit %d.\r\n",_unit);                 \
    return BVIEW_STATUS_FAILURE;                                         \
  }                                                                       \
}


/* Macro to acquire write lock */
#define BST_RWLOCK_WR_LOCK(_unit)                             \
{                                                                          \
  BVIEW_BST_UNIT_CXT_t *_ptr;                                              \
  _ptr = BST_UNIT_PTR_GET (_unit);                                         \
  if (pthread_rwlock_wrlock(&_ptr->bst_configRWLock) != 0)                    \
  {                                                         \
    LOG_POST (BVIEW_LOG_ERROR,                                            \
        "Failed to take the write lock for unit %d.\r\n",_unit);                 \
    return BVIEW_STATUS_FAILURE;                          \
  }                                                                       \
}

/* Macro to release RW lock */
#define BST_RWLOCK_UNLOCK(_unit)                              \
{                                                                          \
  BVIEW_BST_UNIT_CXT_t *_ptr;                                              \
  _ptr = BST_UNIT_PTR_GET (_unit);                                         \
  if (pthread_rwlock_unlock(&_ptr->bst_configRWLock) != 0)                    \
  {                                                         \
    LOG_POST (BVIEW_LOG_ERROR,                                            \
        "Failed to release the write lock for unit %d.\r\n",_unit);                 \
    return BVIEW_STATUS_FAILURE;                          \
  }                                                                       \
}



/* Macro to copy track structures to collction structure */
#define BST_COPY_TRACK_TO_COLLECT(_track_ptr, _collect_ptr)                          \
            {                                                                        \
              if ((NULL == (_track_ptr)) || (NULL == (_collect_ptr)))                      \
              {                                                                      \
                return BVIEW_STATUS_FAILURE;                                         \
              }                                                                      \
              (_collect_ptr)->includeDevice = (_track_ptr)->trackDevice;                   \
              (_collect_ptr)->includeIngressPortPriorityGroup =                         \
              (_track_ptr)->trackIngressPortPriorityGroup;                              \
              (_collect_ptr)->includeIngressPortServicePool =                           \
              (_track_ptr)->trackIngressPortServicePool;                                \
              (_collect_ptr)->includeIngressServicePool =                               \
              (_track_ptr)->trackIngressServicePool;                                    \
              (_collect_ptr)->includeEgressPortServicePool =                            \
              (_track_ptr)->trackEgressPortServicePool;                                 \
              (_collect_ptr)->includeEgressServicePool =                                \
              (_track_ptr)->trackEgressServicePool;                                     \
              (_collect_ptr)->includeEgressUcQueue =                                    \
              (_track_ptr)->trackEgressUcQueue;                                         \
              (_collect_ptr)->includeEgressUcQueueGroup =                               \
              (_track_ptr)->trackEgressUcQueueGroup;                                    \
              (_collect_ptr)->includeEgressMcQueue =                                    \
              (_track_ptr)->trackEgressMcQueue;                                         \
              (_collect_ptr)->includeEgressCpuQueue =                                   \
              (_track_ptr)->trackEgressCpuQueue;                                        \
              (_collect_ptr)->includeEgressRqeQueue =                                   \
              (_track_ptr)->trackEgressRqeQueue;                                        \
            }

/* Macro to copy collect structures to response options structure */

#define  BST_COPY_COLLECT_TO_RESP(_collect_ptr, _resp_ptr)                                 \
            {                                                                              \
              if ((NULL == (_resp_ptr)) || (NULL == (_collect_ptr)))                     \
              {                                                                            \
                 LOG_POST (BVIEW_LOG_ERROR, "Failed to copy c-json encoding options to response\r\n");\
                 return BVIEW_STATUS_FAILURE;                                              \
              }                                                                            \
              (_resp_ptr)->includeDevice = (_collect_ptr)->includeDevice;                \
              (_resp_ptr)->includeIngressPortPriorityGroup =                          \
              (_collect_ptr)->includeIngressPortPriorityGroup;                                \
              (_resp_ptr)->includeIngressPortServicePool =                            \
              (_collect_ptr)->includeIngressPortServicePool;                                  \
              (_resp_ptr)->includeIngressServicePool =                                \
              (_collect_ptr)->includeIngressServicePool;                                      \
              (_resp_ptr)->includeEgressPortServicePool =                             \
              (_collect_ptr)->includeEgressPortServicePool;                                   \
              (_resp_ptr)->includeEgressServicePool =                                 \
              (_collect_ptr)->includeEgressServicePool;                                       \
              (_resp_ptr)->includeEgressUcQueue =                                     \
              (_collect_ptr)->includeEgressUcQueue;                                           \
              (_resp_ptr)->includeEgressUcQueueGroup =                                \
              (_collect_ptr)->includeEgressUcQueueGroup;                                      \
              (_resp_ptr)->includeEgressMcQueue =                                     \
              (_collect_ptr)->includeEgressMcQueue;                                           \
              (_resp_ptr)->includeEgressCpuQueue =                                    \
              (_collect_ptr)->includeEgressCpuQueue;                                          \
              (_resp_ptr)->includeEgressRqeQueue =                                    \
              (_collect_ptr)->includeEgressRqeQueue;                                          \
           }

/* Macro to copy response options structure with given value*/

#define  BST_COPY_TO_RESP(_resp_ptr, _val_)                                 \
            {                                                                              \
              if (NULL == (_resp_ptr))                                                  \
              {                                                                            \
                 LOG_POST (BVIEW_LOG_ERROR, "Failed to copy c-json encoding options to response\r\n");\
                 return BVIEW_STATUS_FAILURE;                                              \
              }                                                                            \
              (_resp_ptr)->includeDevice = _val_;                \
              (_resp_ptr)->includeIngressPortPriorityGroup = _val_;                         \
              (_resp_ptr)->includeIngressPortServicePool =  _val_;                          \
              (_resp_ptr)->includeIngressServicePool =  _val_;                              \
              (_resp_ptr)->includeEgressPortServicePool =    _val_;                         \
              (_resp_ptr)->includeEgressServicePool =       _val_;                          \
              (_resp_ptr)->includeEgressUcQueue =   _val_;                                  \
              (_resp_ptr)->includeEgressUcQueueGroup =   _val_;                             \
              (_resp_ptr)->includeEgressMcQueue =   _val_;                                  \
              (_resp_ptr)->includeEgressCpuQueue =  _val_;                                  \
              (_resp_ptr)->includeEgressRqeQueue =  _val_;                                  \
           }



#define _BST_INPUT_PARAMS_CHECK(_type, _capabilities, _p) do { \
      if (BVIEW_BST_INGRESS_PORT_PG_THRESHOLD == _type) { \
         if ((0 > _p->priorityGroup) || \
            (_p->priorityGroup >= _capabilities.numPriorityGroups)) { \
                 return BVIEW_STATUS_INVALID_PARAMETER;                                              \
              }                                                                            \
            } \
         if ((BVIEW_BST_INGRESS_PORT_SP_THRESHOLD == _type) || \
            (BVIEW_BST_EGRESS_PORT_SP_THRESHOLD == _type)) { \
         if ((0 > _p->servicePool) || \
            (_p->servicePool >= _capabilities.numServicePools) ||  \
            (_p->port > _capabilities.numPorts))  { \
                 return BVIEW_STATUS_INVALID_PARAMETER;                                              \
              }                                                                            \
            } \
         if ((BVIEW_BST_INGRESS_SP_THRESHOLD == _type) ||  \
            (BVIEW_BST_EGRESS_SP_THRESHOLD == _type)) { \
         if ((0 > _p->servicePool) || \
            (_p->servicePool >= _capabilities.numServicePools)) {  \
                 return BVIEW_STATUS_INVALID_PARAMETER;                                              \
              }                                                                            \
            } \
      if (BVIEW_BST_EGRESS_UC_QUEUE_THRESHOLD == _type) { \
         if ((0 > _p->queue) || \
            (_p->queue >= _capabilities.numUnicastQueues)) { \
                 return BVIEW_STATUS_INVALID_PARAMETER;                                              \
              }                                                                            \
            } \
      if (BVIEW_BST_EGRESS_UC_QUEUEGROUPS_THRESHOLD == _type) { \
         if ((0 > _p->queueGroup) || \
            (_p->queueGroup >= _capabilities.numUnicastQueueGroups)) { \
                 return BVIEW_STATUS_INVALID_PARAMETER;                                              \
              }                                                                            \
            } \
      if (BVIEW_BST_EGRESS_MC_QUEUE_THRESHOLD == _type) { \
         if ((0 > _p->queue) || \
            (_p->queue >= _capabilities.numMulticastQueues)) { \
                 return BVIEW_STATUS_INVALID_PARAMETER;                                              \
              }                                                                            \
            } \
      if (BVIEW_BST_EGRESS_CPU_QUEUE_THRESHOLD == _type) { \
         if ((0 > _p->queue) || \
            (_p->queue >= _capabilities.numCpuQueues)) { \
                 return BVIEW_STATUS_INVALID_PARAMETER;                                              \
              }                                                                            \
            } \
      if (BVIEW_BST_EGRESS_RQE_QUEUE_THRESHOLD == _type) { \
         if ((0 > _p->queue) || \
            (_p->queue >= _capabilities.numRqeQueues)) { \
                 return BVIEW_STATUS_INVALID_PARAMETER;                                              \
              }                                                                            \
            } \
} while(0)


#define BST_VALIDATE_PERCENTAGE_INPUT(_kk_)                              \
do {                                                                          \
    if ((0 >= _kk_) || \
            (_kk_ > 100)) { \
    return BVIEW_STATUS_FAILURE;                          \
  }                                                                            \
}while (0)


#define BST_CONVERT_CELLS_TO_BYTES(_kk_, __factor__)                              \
do {                                                                          \
    _kk_ = ( _kk_ *__factor__);\
}while (0)


/* functions */
/*********************************************************************
* @brief : application function to configure the bst features
*
* @param[in] msg_data : pointer to the bst message request.
*
* @retval  : BVIEW_STATUS_SUCCESS : when the asic successfully programmed
* @retval  : BVIEW_STATUS_FAILURE : when the asic is failed to programme.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : Inpput paramerts are invalid. 
*
* @note : This function is invoked in the bst context and used to 
*         configure the parameters like
*         -- bst enable
*         -- asyncronous collection of reports
*         -- configuring the collection interval
*         -- option to configure the data in bytes or cells.
*         In case of the underlying  api returns failure, the same error message
*         is received  and sent to the invoking function.
*
*********************************************************************/
BVIEW_STATUS bst_config_feature_set (BVIEW_BST_REQUEST_MSG_t * msg_data);

/*********************************************************************
* @brief : application function to get the bst features
*
* @param[in] msg_data : pointer to the bst message request.
*
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : Inpput paramerts are invalid. 
* @retval  : BVIEW_STATUS_SUCCESS : when the requested data is retrieved successfully.
* @retval  : BVIEW_STATUS_FAILURE : When the application is unable to retrieve the
*                                   requested data.
*
* @note
*
*********************************************************************/
BVIEW_STATUS bst_config_feature_get(BVIEW_BST_REQUEST_MSG_t *msg_data);
/*********************************************************************
* @brief : application function to configure the bst tracking 
*
* @param[in] msg_data : pointer to the bst message request.
*
* @retval  : BVIEW_STATUS_FAILURE : when the requested data fails to get programmed in asic. 
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : Inpput paramerts are invalid. 
* @retval  : BVIEW_STATUS_SUCCESS : when the requested data is successfully programmed.
*
* @note : This function is invokded in the bst context to configure the bst 
*         tracking parameters. BST can enable tracking for below.
*          -- trackPeakStats
*          -- trackIngressPortPriorityGroup
*          -- trackIngressPortServicePool
*          -- trackIngressServicePool
*          -- trackEgressPortServicePool
*          -- trackEgressServicePool
*          -- trackEgressUcQueue
*          -- trackEgressUcQueueGroup
*          -- trackEgressMcQueue
*          -- trackEgressCpuQueue
*          -- trackEgressRqeQueue
*
*********************************************************************/
BVIEW_STATUS bst_config_track_set (BVIEW_BST_REQUEST_MSG_t * msg_data);

/*********************************************************************
* @brief : application function to get the bst tracking 
*
* @param[in] msg_data : pointer to the bst message request.
*
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : Inpput paramerts are invalid. 
* @retval  : BVIEW_STATUS_FAILURE  : unable to retrieve the requested tracking params.
* @retval  : BVIEW_STATUS_SUCCESS  : successfully retrieved the tracking params.
*
* @note
*
*********************************************************************/
BVIEW_STATUS bst_config_track_get(BVIEW_BST_REQUEST_MSG_t *msg_data);

/*********************************************************************
* @brief : application function to get switch properties
*
* @param[in] msg_data : pointer to the bst message request.
*
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : Inpput paramerts are invalid.
* @retval  : BVIEW_STATUS_SUCCESS  : successfully retrieved the switch
*                                    properties.
* @note
*
*********************************************************************/
BVIEW_STATUS system_switch_properties_get (BVIEW_BST_REQUEST_MSG_t * msg_data);

/*********************************************************************
* @brief : application function to get the bst report and thresholds 
*
* @param[in] msg_data : pointer to the bst message request.
*
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : Inpput paramerts are invalid. 
* @retval  : BVIEW_STATUS_FAILURE  : Failed to retrieve the report from the asic.
* @retval  : BVIEW_STATUS_SUCCESS  : Successfully retrieved the stats report.
*
* @note : based on the message type the report is retrieved.
*          if the type is periodic , then the differential data between
*          the active and backup records is sent in the report.
*          If the report is get-bst-report,then the reporte is
*          complete, i.e. the data is not differential.
*          when the report type is trigger report, then report is triggered 
*          using a callback mechanism from the asic.
*
*********************************************************************/
BVIEW_STATUS bst_get_report(BVIEW_BST_REQUEST_MSG_t *msg_data);

/*********************************************************************
* @brief : function to add timer for the periodic stats collection 
*
* @param[in] unit : unit for which the periodic stats need to be collected.
* @param[in] handler :callback handler from timer context 
* @param[in] cmd : command request type 
* @param[in] id : command request id
*
* @retval  : BVIEW_STATUS_INVALID_PARAMETER -- Inpput paramerts are invalid. 
* @retval  : BVIEW_STATUS_FAILURE -- failed to add the timer 
* @retval  : BVIEW_STATUS_SUCCESS -- timer is successfully added 
*
* @note : this api adds the timer to the linux timer thread, so when the timer 
*         expires, we receive the callback and post message to the bst application
*         to collect the stats.. this is a periodic timer , whose interval
*         is equal to the collection interval. Note that collection is per
*         unit and hence we need per timer per unit.
*
*********************************************************************/
BVIEW_STATUS bst_periodic_collection_timer_add (unsigned int  unit,
                                                void * handler, 
                                                BVIEW_FEATURE_BST_CMD_API_t cmd,
                                                unsigned int id);

/*********************************************************************
* @brief : Deletes the timer node for the given unit
*
* @param[in] unit : unit id for which  the timer needs to be deleted.
* @param[in] cmd :  cmd for which  the timer needs to be deleted.
* @param[in] id :  id for which  the timer needs to be deleted.
*
* @retval  : BVIEW_STATUS_INVALID_PARAMETER -- Inpput paramerts are invalid. 
* @retval  : BVIEW_STATUS_FAILURE -- timer is successfully deleted 
* @retval  : BVIEW_STATUS_SUCCESS -- failed to delete the timer 
*
* @note  : The periodic timer is deleted when send asyncronous reporting
*          is turned off. This timer is per unit.
*
*********************************************************************/
BVIEW_STATUS bst_periodic_collection_timer_delete (int unit,
                                                   BVIEW_FEATURE_BST_CMD_API_t cmd,
                                                   unsigned int id);
/*********************************************************************
* @brief : function to send reponse for encoding to cjson and sending 
*          using rest API 
*
* @param[in] reply_data : pointer to the response message
*
* @retval  : BVIEW_STATUS_SUCCESS : message is successfully using rest API. 
* @retval  : BVIEW_STATUS_FAILURE : message is unable to deliver using rest API.
* @retval  : BVIEW_STATUS_OUTOFMEMORY : Unable to allocate json buffer.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter to function.
*
* @note   : This function is invoked by the bst to send the report and 
*           requested get configure params. This function internally
*           calls the encoding api to encode the data, and the memory
*           for the data is allocated. In case of both successful and 
*           unsuccessful send of the data, the memory must be freed.
*           
*********************************************************************/
BVIEW_STATUS bst_send_response (BVIEW_BST_RESPONSE_MSG_t * reply_data);

/*********************************************************************
* @brief : function to prepare the response to the request message  
*
* @param[in] msg_data : pointer to the request message
* @param[out] reply_data : pointer to the response message
*
* @retval  : BVIEW_STATUS_FAILURE : if the copy to response message fails 
* @retval  : BVIEW_STATUS_SUCCESS :  response message is successfully prepred. 
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameters to function.
*
* @note  : This api copies the required information from input request 
*          message to response message. In case of report, it copies the 
*          pointers into the structure The data need to be protected till 
*          the memory is allocated for the report to be copied.
*          If the report is periodic, then we need the references of both
*          active and back up records. If the report is non periodic, i.e 
*          trigger or get report, then only the active record is required. 
*          incse of periodic report, the report contains 
*          incremental/differential changes.. Wher as for get report 
*          the report is complete. Ensure that the lock is taken at this function
*          is released when the buffer for the data is allocated.
*
*********************************************************************/
BVIEW_STATUS bst_copy_reply_params (BVIEW_BST_REQUEST_MSG_t * msg_data,
                            BVIEW_BST_RESPONSE_MSG_t * reply_data);

/*********************************************************************
* @brief :  function to register with module mgr
*
* @param[in] : none 
* 
* @retval  : BVIEW_STATUS_SUCCESS : registration of BST with module manager is successful.
* @retval  : BVIEW_STATUS_FAILURE : BST failed to register with module manager.
*
* @note : BST need to register with module manager for the below purpose.
*         When the REST API is invoked, rest queries the module manager for
*         the suitable function api  for the corresponding request. Once the
*         api is retieved , posts the request using the retrieved api.
*         for this bst need to register with module mgr.
*
* @end
*********************************************************************/
BVIEW_STATUS bst_module_register ();

/*********************************************************************
* @brief   :  function to post message to the bst application  
*
* @param[in]  msg_data : pointer to the message request
*
* @retval  : BVIEW_STATUS_SUCCESS : if the message is successfully posted to BST queue.
* @retval  : BVIEW_STATUS_FAILURE : if the message is failed to send to bst
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameters to function.
*            
* @note  : all the apis and call back functions should use this api
*          to post the message to bst application.
*
*********************************************************************/
BVIEW_STATUS bst_send_request(BVIEW_BST_REQUEST_MSG_t *msg_data);

/*********************************************************************
*  @brief:  callback function to send periodic reports  
*
* @param[in]   sigval : Data passed with notification after timer expires
*
* @retval  : BVIEW_STATUS_SUCCESS : message is successfully posted to bst.
* @retval  : BVIEW_STATUS_FAILURE : failed to post message to bst.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameters to function.
*
* @note : when the periodic collection timer expires, this api is invoked in
*         the timer context. We just post the request for the report, while 
*        posting we mark the report type as periodic. The parameter returned 
*       from the callback is the reference to unit id. If the callback is failed
*        to post to bst application , the error will be logged.
*
*********************************************************************/
BVIEW_STATUS bst_periodic_collection_cb (union sigval sigval);

/*********************************************************************
* @brief : set the threshold for the given realm.
*
* @param[in] msg_data : pointer to the bst message request.
*
* @retval  : BVIEW_STATUS_SUCCESS - threshold is set successfully
* @retval  : BVIEW_STATUS_FAILURE - failed to apply the inputted threshold
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameters to function.
*
* @note    : In case of any threshold set fail, the error is logged
*
*********************************************************************/
BVIEW_STATUS bst_config_threshold_set (BVIEW_BST_REQUEST_MSG_t * msg_data);
/*********************************************************************
* @brief : function to clear the threshold set
*
* @param[in] msg_data : pointer to the bst message request.
*
* @retval  : BVIEW_STATUS_SUCCESS : successfully cleared the threshold values
* @retval  : BVIEW_STATUS_FAILURE : The clearing of thresholds has failed.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameters to function.
*
* @note    : when clear threshold is successful, along with the threshold information,
*            the software information will also be cleared.
*
*********************************************************************/
BVIEW_STATUS bst_clear_threshold_set (BVIEW_BST_REQUEST_MSG_t * msg_data);

/*********************************************************************
* @brief : clear the stats
*
* @param[in] msg_data : pointer to the bst message request.
*
* @retval  : BVIEW_STATUS_SUCCESS : successfully cleared the stats values
* @retval  : BVIEW_STATUS_FAILURE : The clearing of stats has failed.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameters to function.
*
* @note : When clear stats is successful, along with the stats in the asic
*         the cache will also be cleared.
*
*********************************************************************/
BVIEW_STATUS bst_clear_stats_set (BVIEW_BST_REQUEST_MSG_t * msg_data);

/*********************************************************************
* @brief : copies the h/w retrevied to the current record.
*
* @param[in] type : Type of the record , i.e. stats or threshold
* @param[in] unit : unit number for which the data is collected.
*
* @retval  : BVIEW_STATUS_SUCCESS : successfully updates the records
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameters to function.
*
* @note : Once the collection of the stats is completeed, the existing active
*        record becomes back up. The newly collected record become the new active
*        record. Make sure to take the lock while updating..
*
*********************************************************************/
BVIEW_STATUS bst_update_data(BVIEW_BST_REPORT_TYPE_t type,unsigned int unit);

/*************************************************************
*@brief:  Callback function to send the trigger to bst application
*         to send periodic collection
*
* @param[in] unit : unit for which the trigger is generated 
* @param[in] cookie : cookie
* @param[in] type : trigger type
*
* @retval  : BVIEW_STATUS_SUCCESS : successfully posted message to bst
* @retval  : BVIEW_STATUS_FAILURE : message posting to bst queue failed.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameters to function.
*
* @note  : callback function from ASIC to bst application 
*          to send the triggers. This function is invoked 
*          in the asic context, when the congigured thresholds 
*          exceeds the values. In such case, 
*          the trigger is generated by the asic and bst 
*          application take a trigger report and sends the same to collector.
*
*************************************************************/
BVIEW_STATUS bst_trigger_cb(int unit,void *cookie, BVIEW_BST_TRIGGER_INFO_t *triggerInfo);
/*********************************************************************
 * @brief : function to return the api handler for the bst command type
 *
 * @param[in] int : request type
 * @param[out] int : pointer to the handler 
 *
 * @retval  : BVIEW_STATUS_SUCCESS : if the function pointer to the message type is found.
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
 *
 * @note :
 *
 *********************************************************************/
BVIEW_STATUS bst_type_api_get (int type, BVIEW_BST_API_HANDLER_t *handler);


/*********************************************************************
* @brief : Deletes the trigger timer node for the given unit
*
* @param[in] unit : unit id for which  the timer needs to be deleted.
*
* @retval  : BVIEW_STATUS_INVALID_PARAMETER -- Inpput paramerts are invalid. 
* @retval  : BVIEW_STATUS_FAILURE -- timer is successfully deleted 
* @retval  : BVIEW_STATUS_SUCCESS -- failed to delete the timer 
*
* @note  :
*         
*
*********************************************************************/
BVIEW_STATUS bst_trigger_timer_delete (int unit);


/*********************************************************************
* @brief : function to timer for the trigger report rate limit. 
*
* @param[in] unit : unit for which the trigger interval timer need to run.
*
* @retval  : BVIEW_STATUS_INVALID_PARAMETER -- Inpput paramerts are invalid. 
* @retval  : BVIEW_STATUS_FAILURE -- failed to add the timer 
* @retval  : BVIEW_STATUS_SUCCESS -- timer is successfully added 
*
* @note : this api adds the timer to the linux timer thread, so when the timer 
*         expires, we receive the callback and post message to the bst application.
*         Upon receiving the event, bst application clears the trigger reports 
*         counter which it maintains per realm - counter.
*
*********************************************************************/
BVIEW_STATUS bst_trigger_timer_add (unsigned int  unit);
/*********************************************************************
* @brief : clear the trigger counters 
*
* @param[in] msg_data : pointer to the bst message request.
*
* @retval  : BVIEW_STATUS_SUCCESS : successfully cleared the trigger report count 
* @retval  : BVIEW_STATUS_FAILURE : The clearing of count has failed.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note
*
*********************************************************************/
BVIEW_STATUS bst_clear_trigger_count (BVIEW_BST_REQUEST_MSG_t * msg_data);

/*********************************************************************
*  @brief:  callback function to clear the trigger count  
*
* @param[in]   sigval : Data passed with notification after timer expires
*
* @retval  : BVIEW_STATUS_SUCCESS : message is successfully posted to bst.
* @retval  : BVIEW_STATUS_FAILURE : failed to post message to bst.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameters to function.
*
* @note : when the trigger timer expires, this api is invoked in
*         the timer context.
*
*********************************************************************/
BVIEW_STATUS bst_trigger_timer_cb (union sigval sigval);

/*********************************************************************
*  @brief:  function to set the given realm in the include trigger report.  
*
* @param[in]   *realm : pointer to realm 
* @param[in]   *realm : pointer to json encode options 
*
* @retval  : none : 
*
* @note :
*
*********************************************************************/
void bst_set_realm_to_collect(char *realm, BVIEW_BST_REPORT_OPTIONS_t *options);

/*********************************************************************
* @brief : application function to process trigger messages 
*
* @param[in] msg_data : pointer to the bst message request.
*
* @retval  : BVIEW_STATUS_SUCCESS : when the request is successfully processed 
* @retval  : BVIEW_STATUS_FAILURE : when the processing of the request failed. 
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : Inpput paramerts are invalid. 
*
* @note : This function is invoked in the bst thread context and used to 
*         -- bst enable
*         -- send the message to bst thread if the sending of trigger report is allowed. 
*
*********************************************************************/

BVIEW_STATUS bst_process_trigger(BVIEW_BST_REQUEST_MSG_t *msg_data);

/*********************************************************************
* @brief : bst trigger main application function which does processing of 
*          trigger messages
*
* @param[in] : none
*
* @retval  : BVIEW_STATUS_SUCCESS: 
* @retval  : BVIEW_STATUS_FAILURE: Fails to process the trigger messages 
*
* @note  : This api is the processing thread of the bst trigger application. 
*          All the incoming requests are processed. 
*          Currently the assumption is made that if the 
*          thread fails to read continously 10 or more messages,
*          then there is some error and the thread exits.
*
*********************************************************************/

BVIEW_STATUS bst_trigger_main(void);

BVIEW_STATUS bst_enable_on_trigger(BVIEW_BST_REQUEST_MSG_t *msg_data, int bstEnable);


/*********************************************************************
* @brief :  re-enable bst on trigger timer expiry 
*
* @param[in] msg_data : pointer to the bst message request.
*
* @retval  : BVIEW_STATUS_SUCCESS : successfully cleared the trigger report count 
* @retval  : BVIEW_STATUS_FAILURE : The re-enable has failed.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note
*
*********************************************************************/
BVIEW_STATUS bst_enable_on_trigger_timer_expiry (BVIEW_BST_REQUEST_MSG_t * msg_data);

/*********************************************************************
* @brief : application function to get the bst congestion drop counters 
*
* @param[in] msg_data : pointer to the bst message request.
*
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : Inpput paramerts are invalid. 
* @retval  : BVIEW_STATUS_SUCCESS : when the bst feature params is 
*                                   retrieved successfully.
*
* @note
*
*********************************************************************/
BVIEW_STATUS bst_get_cgsn_drp_ctrs(BVIEW_BST_REQUEST_MSG_t * msg_data);

void bst_sort_records(unsigned int unit, unsigned int count);

/* bst set threshold functions */
BVIEW_STATUS bst_device_threshold_set(BVIEW_BST_REQUEST_MSG_t * msg_data);

BVIEW_STATUS bst_ippg_threshold_set(BVIEW_BST_REQUEST_MSG_t * msg_data);

BVIEW_STATUS bst_ipsp_threshold_set(BVIEW_BST_REQUEST_MSG_t * msg_data);
BVIEW_STATUS bst_isp_threshold_set(BVIEW_BST_REQUEST_MSG_t * msg_data);
BVIEW_STATUS bst_epsp_threshold_set(BVIEW_BST_REQUEST_MSG_t * msg_data);
BVIEW_STATUS bst_esp_threshold_set(BVIEW_BST_REQUEST_MSG_t * msg_data);
BVIEW_STATUS bst_euq_threshold_set(BVIEW_BST_REQUEST_MSG_t * msg_data);
BVIEW_STATUS bst_euqg_threshold_set(BVIEW_BST_REQUEST_MSG_t * msg_data);
BVIEW_STATUS bst_emc_threshold_set(BVIEW_BST_REQUEST_MSG_t * msg_data);
BVIEW_STATUS bst_ecpu_threshold_set(BVIEW_BST_REQUEST_MSG_t * msg_data);
BVIEW_STATUS bst_erqe_threshold_set(BVIEW_BST_REQUEST_MSG_t * msg_data);



BVIEW_STATUS bst_device_threshold_input_validate(BSTJSON_CONFIGURE_BST_THRESHOLDS_t *pCommand,
                                                 BVIEW_BST_REQUEST_MSG_t *msg_data);


BVIEW_STATUS bst_ippg_threshold_input_validate(BSTJSON_CONFIGURE_BST_THRESHOLDS_t *pCommand,
                                                 BVIEW_BST_REQUEST_MSG_t *msg_data);


BVIEW_STATUS bst_ipsp_threshold_input_validate(BSTJSON_CONFIGURE_BST_THRESHOLDS_t *pCommand,
                                                 BVIEW_BST_REQUEST_MSG_t *msg_data);


BVIEW_STATUS bst_isp_threshold_input_validate(BSTJSON_CONFIGURE_BST_THRESHOLDS_t *pCommand,
                                                 BVIEW_BST_REQUEST_MSG_t *msg_data);


BVIEW_STATUS bst_epsp_threshold_input_validate(BSTJSON_CONFIGURE_BST_THRESHOLDS_t *pCommand,
                                                 BVIEW_BST_REQUEST_MSG_t *msg_data);


BVIEW_STATUS bst_esp_threshold_input_validate(BSTJSON_CONFIGURE_BST_THRESHOLDS_t *pCommand,
                                                 BVIEW_BST_REQUEST_MSG_t *msg_data);


BVIEW_STATUS bst_euq_threshold_input_validate(BSTJSON_CONFIGURE_BST_THRESHOLDS_t *pCommand,
                                                 BVIEW_BST_REQUEST_MSG_t *msg_data);


BVIEW_STATUS bst_euqg_threshold_input_validate(BSTJSON_CONFIGURE_BST_THRESHOLDS_t *pCommand,
                                                 BVIEW_BST_REQUEST_MSG_t *msg_data);


BVIEW_STATUS bst_emc_threshold_input_validate(BSTJSON_CONFIGURE_BST_THRESHOLDS_t *pCommand,
                                                 BVIEW_BST_REQUEST_MSG_t *msg_data);


BVIEW_STATUS bst_ecpu_threshold_input_validate(BSTJSON_CONFIGURE_BST_THRESHOLDS_t *pCommand,
                                                 BVIEW_BST_REQUEST_MSG_t *msg_data);


BVIEW_STATUS bst_erqe_threshold_input_validate(BSTJSON_CONFIGURE_BST_THRESHOLDS_t *pCommand,
                                                 BVIEW_BST_REQUEST_MSG_t *msg_data);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_BST_APP_H */

