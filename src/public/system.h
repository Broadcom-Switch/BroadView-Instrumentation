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

#ifndef INCLUDE_SYSTEM_H
#define INCLUDE_SYSTEM_H

#include "broadview.h"
#include "asic.h"
#include "packet_multiplexer.h"

/**Timer mode Periodic or Non Periodic*/

#define BVIEW_TIME_CONVERSION_FACTOR 1000
#define BVIEW_TIME_BUFFER_SIZE       64

typedef enum
{
  PERIODIC_MODE = 0,
  NON_PERIODIC_MODE = 1,
}TIMER_MODE;

#define SYSTEM_MSGQ_MAX_MSG   10

/* Maximum length of ASIC ID notation*/
#define BVIEW_ASIC_NOTATION_LEN             32

typedef struct _switch_asic_info_
{
  char              asic_notation[BVIEW_ASIC_NOTATION_LEN];
  BVIEW_ASIC_TYPE   asicType;
  int               numPorts;
}  BVIEW_SWITCH_ASIC_INFO_t;

#define BVIEW_MAX_REQUESTS  8

#define BVIEW_MACADDR_LEN  6
#define BVIEW_SYSTEM_TIME_CONVERSION_FACTOR 1000
#define BVIEW_MAX_IP_ADDR_LENGTH 20


/* file from where the configuration properties are read. */
#define SYSTEM_CONFIG_FILE    "agent_config.cfg"

#define SYSTEM_CONFIG_PROPERTY_CLIENT_IP   "bview_client_ip"
#define SYSTEM_CONFIG_PROPERTY_CLIENT_IP_DEFAULT   "127.0.0.1"

#define SYSTEM_CONFIG_PROPERTY_CLIENT_PORT   "bview_client_port"
#define SYSTEM_CONFIG_PROPERTY_CLIENT_PORT_DEFAULT   9070

#define SYSTEM_CONFIG_PROPERTY_LOCAL_PORT "agent_port"
#define SYSTEM_CONFIG_PROPERTY_LOCAL_PORT_DEFAULT 8080

typedef BVIEW_STATUS(*BVIEW_PACKET_RX_CALLBACK_t) (
        void *cookie,
        BVIEW_PACKET_MSG_t *packet_msg);

typedef struct _system_agent_config_
{
  char clientIp[BVIEW_MAX_IP_ADDR_LENGTH];

  int clientPort;

  int localPort;
} BVIEW_SYSTEM_AGENT_CONFIG_t;


#define _SYSTEM_CONFIGFILE_LINE_MAX_LEN   256
#define _SYSTEM_CONFIGFILE_READ_MODE      "r"
#define _SYSTEM_CONFIGFILE_DELIMITER      "="

#define _SYSTEM_ASSERT_CONFIG_FILE_ERROR(condition) do { \
    if (!(condition)) { \
        LOG_POST(BVIEW_LOG_DEBUG, \
                    "SYSTEM (%s:%d) Unrecognized Config File format, may be corrupted. Errno : %s  \n", \
                    __func__, __LINE__, strerror(errno)); \
                        fclose(configFile); \
        return (BVIEW_STATUS_FAILURE); \
    } \
} while(0)

/* Macro to acquire lock */
#define SYSTEM_AGENT_LOCK_TAKE(_mylock)                                                        \
          {                                                                           \
                       if (0 != pthread_mutex_lock (&_mylock))                           \
                       {                                                                        \
                                       LOG_POST (BVIEW_LOG_ERROR,                                            \
                                                             "Failed to take the system agent config lock \r\n");                \
                                       return BVIEW_STATUS_FAILURE;                                          \
                                    }                                                                        \
                     }
/*  to release lock*/
#define SYSTEM_AGENT_LOCK_GIVE(_mylock)                                                        \
           {                                                                          \
                        if (0 != pthread_mutex_unlock(&_mylock))                         \
                        {                                                                        \
                                        LOG_POST (BVIEW_LOG_ERROR,                                            \
                                                          "Failed to Release the sysyem agent config lock.\r\n");                 \
                                         return BVIEW_STATUS_FAILURE;                                         \
                                      }                                                                       \
                       }



/* Max buffers allocated for Egress Port + Service Pools */
typedef struct _system_ep_sp_max_buf_
{
    uint64_t ucShareMaxBuf;
    uint64_t umShareMaxBuf;
    uint64_t mcShareMaxBuf;
} BVIEW_SYSTEM_EGRESS_PORT_SP_MAX_BUF_t;

/* Max buffers allocated for the device level buffers*/
typedef struct _system_device_max_buf_
{
    uint64_t maxBuf;
} BVIEW_SYSTEM_DEVICE_MAX_BUF_t;

/* Max buffers allocated for Ingress Port + Priority Groups */
typedef struct _system_i_p_pg_max_buf_
{
    uint64_t umShareMaxBuf;
    uint64_t umHeadroomMaxBuf;
} BVIEW_SYSTEM_INGRESS_PORT_PG_MAX_BUF_t;

/* Max buffers allocated for Ingress Port + Service Pools */
typedef struct _system_i_p_sp_max_buf_
{
    uint64_t umShareMaxBuf;
} BVIEW_SYSTEM_INGRESS_PORT_SP_MAX_BUF_t;

/* Max buffers allocated for Ingress Service Pools */
typedef struct _system_i_sp_max_buf_
{
    uint64_t umShareMaxBuf;
} BVIEW_SYSTEM_INGRESS_SP_MAX_BUF_t;

/* Max buffers allocated for Egress Service Pools */
typedef struct _system_e_sp_max_buf_
{
    uint64_t umShareMaxBuf;
    uint64_t mcShareMaxBuf;
} BVIEW_SYSTEM_EGRESS_SP_MAX_BUF_t;

/* Max buffers allocated for Egress Unicast Queues */
typedef struct _system_e_ucq_max_buf_
{
    uint64_t ucMaxBuf;
} BVIEW_SYSTEM_EGRESS_UC_QUEUE_MAX_BUF_t;

/* Max buffers allocated for Egress Unicast Queue Groups */
typedef struct _system_e_ucqg_max_buf_
{
    uint64_t ucMaxBuf;
} BVIEW_SYSTEM_EGRESS_UC_QUEUEGROUPS_MAX_BUF_t;

/* Max buffers allocated for Egress Multicast Queues */
typedef struct _system_e_mcq_max_buf_
{
    uint64_t mcMaxBuf;
} BVIEW_SYSTEM_EGRESS_MC_QUEUE_MAX_BUF_t;

/* Max buffers allocated for CPU Queues */
typedef struct _system_cpu_q_max_buf_
{
    uint64_t cpuMaxBuf;
} BVIEW_SYSTEM_EGRESS_CPU_QUEUE_MAX_BUF_t;

/* Max buffers allocated for RQE Queues */
typedef struct _system_rqe_q_max_buf_
{
    uint64_t rqeMaxBuf;
} BVIEW_SYSTEM_EGRESS_RQE_QUEUE_MAX_BUF_t;


typedef struct _system_device_max_buf_data_
{

  BVIEW_SYSTEM_DEVICE_MAX_BUF_t data;
  
} BVIEW_SYSTEM_DEVICE_MAX_BUF_DATA_t;

/* Max Buffers for Ingress Port + Priority Groups */
typedef struct _system_i_p_pg_max_buf_data_
{

  BVIEW_SYSTEM_INGRESS_PORT_PG_MAX_BUF_t data[BVIEW_ASIC_MAX_PORTS][BVIEW_ASIC_MAX_PRIORITY_GROUPS];
  
} BVIEW_SYSTEM_INGRESS_PORT_PG_MAX_BUF_DATA_t;

/* Max Buffers for Ingress Port + Service Pools */
typedef struct _system_i_p_sp_max_buf_data_
{

  BVIEW_SYSTEM_INGRESS_PORT_SP_MAX_BUF_t data[BVIEW_ASIC_MAX_PORTS][BVIEW_ASIC_MAX_INGRESS_SERVICE_POOLS];

} BVIEW_SYSTEM_INGRESS_PORT_SP_MAX_BUF_DATA_t;

/* Max Buffers for Ingress Service Pools */
typedef struct _system_i_sp_max_buf_data_
{

  BVIEW_SYSTEM_INGRESS_SP_MAX_BUF_t data[BVIEW_ASIC_MAX_INGRESS_SERVICE_POOLS];

} BVIEW_SYSTEM_INGRESS_SP_MAX_BUF_DATA_t;

/* Max Buffers for Egress Port + Service Pools */
typedef struct _system_e_p_sp_max_buf_data_
{

  BVIEW_SYSTEM_EGRESS_PORT_SP_MAX_BUF_t data[BVIEW_ASIC_MAX_PORTS][BVIEW_ASIC_MAX_SERVICE_POOLS];

} BVIEW_SYSTEM_EGRESS_PORT_SP_MAX_BUF_DATA_t;

/* Max Buffers for Egress Service Pools */
typedef struct _system_e_sp_max_buf_data_
{

  BVIEW_SYSTEM_EGRESS_SP_MAX_BUF_t data[BVIEW_ASIC_MAX_SERVICE_POOLS];

} BVIEW_SYSTEM_EGRESS_SP_MAX_BUF_DATA_t;

/* Max Buffers for Egress Unicast Queues */
typedef struct _system_e_ucq_max_buf_data_
{

  BVIEW_SYSTEM_EGRESS_UC_QUEUE_MAX_BUF_t data[BVIEW_ASIC_MAX_UC_QUEUES];

} BVIEW_SYSTEM_EGRESS_UC_QUEUE_MAX_BUF_DATA_t;

/* Max Buffers for Egress Unicast Queue Groups */
typedef struct _system_e_ucqg_max_buf_data_
{

  BVIEW_SYSTEM_EGRESS_UC_QUEUEGROUPS_MAX_BUF_t data[BVIEW_ASIC_MAX_UC_QUEUE_GROUPS];

} BVIEW_SYSTEM_EGRESS_UC_QUEUEGROUPS_MAX_BUF_DATA_t;

/* Max Buffers for Egress Multicast Queues */
typedef struct _system_e_mcq_max_buf_data_
{

  BVIEW_SYSTEM_EGRESS_MC_QUEUE_MAX_BUF_t data[BVIEW_ASIC_MAX_MC_QUEUES];

} BVIEW_SYSTEM_EGRESS_MC_QUEUE_MAX_BUF_DATA_t;

/* Max Buffers for CPU Queues */
typedef struct _system_cpu_q_max_buf_data_
{

  BVIEW_SYSTEM_EGRESS_CPU_QUEUE_MAX_BUF_t data[BVIEW_ASIC_MAX_CPU_QUEUES];

} BVIEW_SYSTEM_EGRESS_CPU_QUEUE_MAX_BUF_DATA_t;

/* Max Buffers for RQE Queues */
typedef struct _system_rqe_q_max_buf_data_
{

  BVIEW_SYSTEM_EGRESS_RQE_QUEUE_MAX_BUF_t data[BVIEW_ASIC_MAX_RQE_QUEUES];

} BVIEW_SYSTEM_EGRESS_RQE_QUEUE_MAX_BUF_DATA_t;

/* A Complete Data set for a 'snapshot' */

typedef struct _system_asic_max_buf_snapshot_data_
{
    /* Device Section */
    BVIEW_SYSTEM_DEVICE_MAX_BUF_DATA_t device;

    /* Ingress Section */
    BVIEW_SYSTEM_INGRESS_PORT_PG_MAX_BUF_DATA_t iPortPg;
    BVIEW_SYSTEM_INGRESS_PORT_SP_MAX_BUF_DATA_t iPortSp;
    BVIEW_SYSTEM_INGRESS_SP_MAX_BUF_DATA_t iSp;

    /* Egress Section */
    BVIEW_SYSTEM_EGRESS_PORT_SP_MAX_BUF_DATA_t ePortSp;
    BVIEW_SYSTEM_EGRESS_SP_MAX_BUF_DATA_t eSp;
    BVIEW_SYSTEM_EGRESS_UC_QUEUE_MAX_BUF_DATA_t eUcQ;
    BVIEW_SYSTEM_EGRESS_UC_QUEUEGROUPS_MAX_BUF_DATA_t eUcQg;
    BVIEW_SYSTEM_EGRESS_MC_QUEUE_MAX_BUF_DATA_t eMcQ;
    BVIEW_SYSTEM_EGRESS_CPU_QUEUE_MAX_BUF_DATA_t cpqQ;
    BVIEW_SYSTEM_EGRESS_RQE_QUEUE_MAX_BUF_DATA_t rqeQ;

} BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t;

/*********************************************************************
* @brief        Function used to initialize various system components
*               such as openapps driver and calls phase-2 init
*
* @param[in]    debug     debug mode of openapps driver
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/

void bview_system_init_ph1(bool debug, bool menu);

/*********************************************************************
* @brief        Function used to initialize various system components
*               such as module manager, redirector, agent and sbplugins
*
* @param[in]    NA
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/

void bview_system_init_ph2(void *param);

/*********************************************************************
* @brief     Function used to deinitialize various system components
*
*
* @param[in] NA
*
* @retval    NA
*
* @note      NA
*
* @end
*********************************************************************/

void bview_system_deinit();

/*********************************************************************
* @brief         Function used to create and arm a timer
*
*
* @param[in]     handler        function pointer to handle the callback
* @param[in,out] timerId        timerId of the timer which is created
* @param[in]     timeInMilliSec Time after which callback is required
* @param[in]     mode           mode specifing if the timer must be
*                               periodic or oneshot
* @param[in]     param          Arguments passed from the calling function
*
* @retval        BVIEW_STATUS_SUCCESS
* @retval        BVIEW_STATUS_FAILURE
*
* @note          NA
*
* @end
*********************************************************************/

BVIEW_STATUS system_timer_add(void * handler,timer_t * timerId, int timeInMilliSec,TIMER_MODE mode,void * param);


/*********************************************************************
* @brief      Function used to destroy a timer
*
*
* @param[in]  timerId Timer id of the timer which needs to be destroyed
*
* @retval     BVIEW_STATUS_SUCCESS
* @retval     BVIEW_STATUS_FAILURE
*
* @note          NA
*
* @end
*********************************************************************/

BVIEW_STATUS system_timer_delete(timer_t timerId);



/*********************************************************************
* @brief      Function used to set/reset a timer
*
*
* @param[in]  timerId        Timer id of the timer which needs to be
*                            set/reset
* @param[in]  timeInMilliSec Time after which callback is required
* @param[in]  mode           mode specifing if the timer must be
*                            periodic or oneshot
*
* @retval     BVIEW_STATUS_SUCCESS
* @retval     BVIEW_STATUS_FAILURE
*
* @note       NA
*
* @end
*********************************************************************/

BVIEW_STATUS system_timer_set(timer_t timerId,int timeInMilliSec,TIMER_MODE mode);

/*********************************************************************
* @brief      Function used to get the agent port 
*
*
* @param[in]   localPort pointer to the agent local port 
*
* @retval     BVIEW_STATUS_SUCCESS
* @retval     BVIEW_STATUS_FAILURE
*
* @note          NA
*
* @end
*********************************************************************/
BVIEW_STATUS system_agent_port_get(int *localPort);
/*********************************************************************
* @brief      Function used to get the client ip address 
*
*
* @param[in]   clientIp pointer to the  client ip address 
*
* @retval     BVIEW_STATUS_SUCCESS
* @retval     BVIEW_STATUS_FAILURE
*
* @note          NA
*
* @end
*********************************************************************/
BVIEW_STATUS system_agent_client_ipaddr_get(char *clientIp);
/*********************************************************************
* @brief      Function used to get the client port 
*
*
* @param[in]   clientPort pointer to the  client port 
*
* @retval     BVIEW_STATUS_SUCCESS
* @retval     BVIEW_STATUS_FAILURE
*
* @note          NA
*
* @end
*********************************************************************/
BVIEW_STATUS system_agent_client_port_get(int *clientPort);

/*********************************************************************
* @brief      Function to get current time in %Y-%m-%dT%H:%M:%S:%ms
*
* @param[out]  buf        Pointer to buf which has current time
*
*
* @retval     BVIEW_STATUS_SUCCESS
*
* @note       NA
*
* @end
*********************************************************************/
BVIEW_STATUS   system_dispaly_local_time_get (char *buf);

#endif /* INCLUDE_SYSTEM_H */

