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

#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <inttypes.h>
#include "json.h"
#include "bst_json_memory.h"
#include "clear_bst_statistics.h"
#include "clear_bst_thresholds.h"
#include "configure_bst_thresholds.h"
#include "configure_bst_feature.h"
#include "configure_bst_tracking.h"
#include "get_bst_tracking.h"
#include "get_bst_feature.h"
#include "get_bst_thresholds.h"
#include "get_bst_cgsn_drop_counters.h"
#include "get_bst_report.h"
#include "bst_json_encoder.h"
#include "bst.h"
#include "common/platform_spec.h"
#include "broadview.h"
#include "bst_app.h"
#include "system.h"
#include "openapps_log_api.h"
#include "sbplugin_redirect_bst.h"
#include "sbplugin_redirect_system.h" 

BVIEW_BST_CXT_t bst_info;
int recvMsgQid;

static BVIEW_REST_API_t bst_cmd_api_list[] = {

  {"configure-bst-tracking", bstjson_configure_bst_tracking},
  {"configure-bst-feature", bstjson_configure_bst_feature},
  {"configure-bst-thresholds", bstjson_configure_bst_thresholds},
  {"get-bst-report", bstjson_get_bst_report},
  {"get-bst-feature", bstjson_get_bst_feature},
  {"get-bst-tracking", bstjson_get_bst_tracking},
  {"get-bst-thresholds", bstjson_get_bst_thresholds},
  {"clear-bst-thresholds", bstjson_clear_bst_thresholds},
  {"clear-bst-statistics", bstjson_clear_bst_statistics},
  {"get-bst-congestion-drop-counters", bstjson_get_bst_congestion_drop_counters}
};

/*********************************************************************
  * @brief : function to return the threshold type for the given realm
  *
  * @param[in] str : realm passed in the input request
  *
  * @retval    : the threshold type value
  *
  * @note : The minumum value of enum type is starts from 1.
  *         if 0 is returned then the given realm didn't match
  *         with any of the supported realm types 
  *
  *********************************************************************/
static BVIEW_STATUS bst_realm_handler_get (unsigned int type, 
                      BVIEW_BST_THRESHOLD_HANDLER_t *handler)
{
  unsigned int i = 0;

  const BVIEW_BST_REALM_THRESHOLD_HANDLER_t realm_threshold_handler_map[] = {
    {BVIEW_BST_DEVICE_THRESHOLD, bst_device_threshold_set},
    {BVIEW_BST_INGRESS_PORT_PG_THRESHOLD, bst_ippg_threshold_set},
    {BVIEW_BST_INGRESS_PORT_SP_THRESHOLD, bst_ipsp_threshold_set},
    {BVIEW_BST_INGRESS_SP_THRESHOLD, bst_isp_threshold_set},
    {BVIEW_BST_EGRESS_PORT_SP_THRESHOLD, bst_epsp_threshold_set},
    {BVIEW_BST_EGRESS_SP_THRESHOLD, bst_esp_threshold_set},
    {BVIEW_BST_EGRESS_UC_QUEUE_THRESHOLD, bst_euq_threshold_set},
    {BVIEW_BST_EGRESS_UC_QUEUEGROUPS_THRESHOLD, bst_euqg_threshold_set},
    {BVIEW_BST_EGRESS_MC_QUEUE_THRESHOLD, bst_emc_threshold_set},
    {BVIEW_BST_EGRESS_CPU_QUEUE_THRESHOLD, bst_ecpu_threshold_set},
    {BVIEW_BST_EGRESS_RQE_QUEUE_THRESHOLD, bst_erqe_threshold_set}
  };
  for (i = 0; i <(sizeof(realm_threshold_handler_map)/sizeof(BVIEW_BST_REALM_THRESHOLD_HANDLER_t)); i++)
  {
    if (type == realm_threshold_handler_map[i].threshold)
    {
       _BST_LOG(_BST_DEBUG_TRACE, "Found handler match for the realm type %d\n", realm_threshold_handler_map[i].threshold);
       *handler = realm_threshold_handler_map[i].handler;
      return BVIEW_STATUS_SUCCESS;
    }
  }
  _BST_LOG(_BST_DEBUG_ERROR, "requested realm %d not found match for the realm type \n ", type);
  return BVIEW_STATUS_FAILURE;
}


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
BVIEW_STATUS bst_config_feature_set (BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BST_CONFIG_t bstMode;
  BVIEW_BST_CONFIG_PARAMS_t *ptr;
  bool timerUpdateReqd = false;
  int tmpMask = 0;

  /* check for the null of the input pointer */
  if (NULL == msg_data)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  /* get the configuration structure pointer  for the desired unit */
  ptr = BST_CONFIG_FEATURE_PTR_GET (msg_data->unit);
  if (NULL == ptr)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
 
  BST_RWLOCK_WR_LOCK(msg_data->unit);
  /* collection interval is maintained in seconds in application.
      while adding the timer, the same should be converted into
       milli seconds and added as the timer api expects 
       interval in milli seconds. */ 
  tmpMask = msg_data->request.config.configMask;

  if ((tmpMask & (1 << BST_CONFIG_PARAMS_COLL_INTRVL)) &&
      (ptr->collectionInterval != msg_data->request.config.collectionInterval))
  {
    /* Collection interval has changed.
       so need to register the modified interval with the timer */
    ptr->collectionInterval = msg_data->request.config.collectionInterval;
    timerUpdateReqd = true;
  }

  if ((tmpMask & (1 << BST_CONFIG_PARAMS_SND_SNAP_TGR)) &&
      (ptr->sendSnapshotOnTrigger != msg_data->request.config.sendSnapshotOnTrigger))
  {
    ptr->sendSnapshotOnTrigger = msg_data->request.config.sendSnapshotOnTrigger;
  }
  
  if ((tmpMask & (1 << BST_CONFIG_PARAMS_TGR_RATE_LIMIT)) &&
       (ptr->bstMaxTriggers != msg_data->request.config.bstMaxTriggers))
  {
    ptr->bstMaxTriggers = msg_data->request.config.bstMaxTriggers;
  }

  if ((tmpMask & (1 << BST_CONFIG_PARAMS_TGR_RL_INTVL)) &&
      (ptr->triggerTransmitInterval != msg_data->request.config.triggerTransmitInterval))
  {
    ptr->triggerTransmitInterval = msg_data->request.config.triggerTransmitInterval;
  }

  /* request is always the negation of the variable. Hence checking 
     for equality. If same then change the variable */
  if (tmpMask & (1 << BST_CONFIG_PARAMS_ASYNC_FULL_REP)) 
  {
    ptr->sendIncrementalReport = (msg_data->request.config.sendIncrementalReport == 0)?1:0;
  }

 if (tmpMask & (1 << BST_CONFIG_PARAMS_SND_ASYNC_REP))
 { 
   if (true == msg_data->request.config.sendAsyncReports)
   {
     /* request contains sendAsyncReports = true */
     if (true != ptr->sendAsyncReports)
     {
       /* old config is not enabled for sending async reports.
          now it is enabled.. so need to register with timer*/
       ptr->sendAsyncReports = true;
       timerUpdateReqd = true;
     }
     /*
        Register with the timer for periodic callbacks */
     if (true == timerUpdateReqd)
     {
       bst_periodic_collection_timer_add (msg_data->unit,
                                          bst_periodic_collection_cb,
                                          BVIEW_BST_CMD_API_GET_REPORT, 0);
     }
   }
   else
   {
     ptr->sendAsyncReports = false;
     /* Periodic report collection is turned off...
        so no need for  the timer. 
        delete the timer */
     bst_periodic_collection_timer_delete (msg_data->unit,
                                           BVIEW_BST_CMD_API_GET_REPORT, 0);
   }
 }

  if ((tmpMask & (1 << BST_CONFIG_PARAMS_STATS_UNITS)) && 
      (ptr->statUnitsInCells != msg_data->request.config.statUnitsInCells))
  {
    /* Store the data is desired in bytes or cells */
    ptr->statUnitsInCells = msg_data->request.config.statUnitsInCells;
  }


  if ((tmpMask & (1 << BST_CONFIG_PARAMS_STATS_IN_PERCENT)) &&
      (ptr->statsInPercentage != msg_data->request.config.statsInPercentage))
  {
    /* Store the data is desired in percentage */
    ptr->statsInPercentage = msg_data->request.config.statsInPercentage;
  }


  BST_RWLOCK_UNLOCK(msg_data->unit);

  if (!(tmpMask & (1 << BST_CONFIG_PARAMS_ENABLE)))
  {
    /* bst not enabled in config.
       just return */
      return rv;
  }

  /* till now we have not checked if the same is enabled in h/w.
      Now check if the bst is enabled in asic.. 
     want to check from s/w .. but set can happen directly and get 
     may be through this app.. so checking with asic only..*/
  memset (&bstMode, 0, sizeof (BVIEW_BST_CONFIG_t));
  rv = sbapi_bst_config_get (msg_data->unit, &bstMode);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    /* Why the h/w call has failed.. post the error with the error reason. */
    LOG_POST (BVIEW_LOG_ERROR,
        "Unable to extract the bst mode from asic. err: %d \r\n", rv);
    return rv;
  }
 
   /* now check if the h/w is set to the same state as requested.
      if not, then only program the asic.. else not required */

  if (bstMode.enableStatsMonitoring == msg_data->request.config.bstEnable)
  {
    /* every thing is fine. both are in sync.. just return */
    return rv;
  }

 /* Set the asic with the desired config to control bst */  
  bstMode.enableStatsMonitoring = msg_data->request.config.bstEnable;
  bstMode.enablePeriodicCollection = ptr->sendAsyncReports;
  bstMode.collectionPeriod = ptr->collectionInterval;
  rv = sbapi_bst_config_set (msg_data->unit, &bstMode);
  if (BVIEW_STATUS_SUCCESS == rv)
  {
    /* asic is successfully programmed.. now update the config.. */
    BST_RWLOCK_WR_LOCK(msg_data->unit);
    ptr->bstEnable = msg_data->request.config.bstEnable;
    BST_RWLOCK_UNLOCK(msg_data->unit);
    LOG_POST (BVIEW_LOG_INFO,
              "bst application: setting bst feature is successful for unit %d.\r\n", msg_data->unit);
  }
  return rv;
}

/*********************************************************************
* @brief : application function to get the bst features
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
BVIEW_STATUS bst_config_feature_get (BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  BVIEW_BST_CONFIG_PARAMS_t *ptr;


  if (NULL == msg_data)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  ptr = BST_CONFIG_FEATURE_PTR_GET (msg_data->unit);
  if (NULL == ptr)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
  return  BVIEW_STATUS_SUCCESS;
}

/* Track set and get apis */
/*********************************************************************
* @brief : application function to configure the bst tracking 
*
* @param[in] msg_data : pointer to the bst message request.
*
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : Inpput paramerts are invalid. 
* @retval  : BVIEW_STATUS_FAILURE : unable to set the bst tracking params
* @retval  : BVIEW_STATUS_SUCCESS : successfully configured bst params.
*
* @note
*
*********************************************************************/
BVIEW_STATUS bst_config_track_set (BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BST_CONFIG_t bstMode;
  BVIEW_BST_TRACK_PARAMS_t *ptr;
  BVIEW_BST_CONFIG_PARAMS_t *config_ptr;
  bool config_changed = false;
  bool trackIngress = true;
  bool trackEgress = true;
  bool trackDevice = true;
  int trackPeakStats = BVIEW_BST_MODE_CURRENT;

  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;


  ptr = BST_CONFIG_TRACK_PTR_GET (msg_data->unit);
  config_ptr = BST_CONFIG_FEATURE_PTR_GET (msg_data->unit);

   if ((NULL == ptr) || (NULL == config_ptr))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
   /* tracking for current or peak mode is changed.. mark the same */
  if (ptr->trackPeakStats != msg_data->request.track.trackPeakStats)
  {
    config_changed = true;
  }

  if (ptr->trackIngressPortPriorityGroup != msg_data->request.track.trackIngressPortPriorityGroup)
  {
   /* tracking for ingress port pri grp has  changed.. mark the same */
    config_changed = true;
  }

  if (ptr->trackIngressPortServicePool != msg_data->request.track.trackIngressPortServicePool)
  {
   /* tracking for ingress port service pool has  changed */
    config_changed = true;
  }

  if (ptr->trackIngressServicePool != msg_data->request.track.trackIngressServicePool)
  {
   /* tracking for ingress service pool has  changed */
    config_changed = true;
  }

  if (ptr->trackEgressPortServicePool != msg_data->request.track.trackEgressPortServicePool)
  {
   /* tracking for egress port service pool has  changed */
    config_changed = true;
  }

  if (ptr->trackEgressServicePool != msg_data->request.track.trackEgressServicePool)
  {
   /* tracking for egress service pool has  changed */
    config_changed = true;
  }

  if (ptr->trackEgressUcQueue != msg_data->request.track.trackEgressUcQueue)
  {
   /* tracking for egress unicat queue has  changed */
    config_changed = true;
  }

  if (ptr->trackEgressUcQueueGroup != msg_data->request.track.trackEgressUcQueueGroup)
  {
   /* tracking for egress unicast queuei grp has  changed */
    config_changed = true;
  }

  if (ptr->trackEgressMcQueue != msg_data->request.track.trackEgressMcQueue)
  {
   /* tracking for egress multicast queue has  changed */
    config_changed = true;
  }

  if (ptr->trackEgressCpuQueue != msg_data->request.track.trackEgressCpuQueue)
  {
   /* tracking for egress cpu queue has  changed */
    config_changed = true;
  }

  if (ptr->trackEgressRqeQueue != msg_data->request.track.trackEgressRqeQueue)
  {
   /* tracking for egress rqe queue has  changed */
    config_changed = true;
  }

  if (ptr->trackDevice != msg_data->request.track.trackDevice)
  {
   /* tracking for global data  changed */
    config_changed = true;
  }

  if (false == config_changed)
  {
    /*no config change. so just return. 
      what ever is desired.. is already in place .. */
    return rv;
  }
 /* request has additonal than what is currently there.
    program the asic */
  memset (&bstMode, 0, sizeof (BVIEW_BST_CONFIG_t));
 
 if (true == msg_data->request.track.trackPeakStats)
 {
   trackPeakStats = BVIEW_BST_MODE_PEAK;
 }
 else
 {
   trackPeakStats = BVIEW_BST_MODE_CURRENT;
 }
  bstMode.enableStatsMonitoring = config_ptr->bstEnable;
  bstMode.enableDeviceStatsMonitoring = trackDevice;
  bstMode.enableIngressStatsMonitoring = trackIngress;
  bstMode.enableEgressStatsMonitoring = trackEgress;
  bstMode.mode = trackPeakStats;
/* program the asic. */
  rv = sbapi_bst_config_set (msg_data->unit, &bstMode);
  if (BVIEW_STATUS_SUCCESS == rv)
  {
      /*successfully programed the asic. store the config */
    *ptr = msg_data->request.track;
    LOG_POST (BVIEW_LOG_INFO,
              "bst application: setting bst tracking is successful"
               " for unit %d.\r\n", msg_data->unit);
  }
  return rv;
}

/*********************************************************************
* @brief : application function to get the bst tracking 
*
* @param[in] msg_data : pointer to the bst message request.
*
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : Inpput paramerts are invalid. 
* @retval  : BVIEW_STATUS_SUCCESS  : successfully retrieved the tracking params.
*
* @note
*
*********************************************************************/
BVIEW_STATUS bst_config_track_get (BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BST_TRACK_PARAMS_t *ptr;

  if (NULL == msg_data)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  /* get the tracking config */
  ptr = BST_CONFIG_TRACK_PTR_GET (msg_data->unit);

  if (NULL == ptr)
    return BVIEW_STATUS_INVALID_PARAMETER;

  return rv;
}

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
BVIEW_STATUS system_switch_properties_get (BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  if (NULL == msg_data)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
* @brief : application function to get the bst report and thresholds 
*
* @param[in] msg_data : pointer to the bst message request.
*
* @retval  : BVIEW_STATUS_INVALID_PARAMETER -- Inpput paramerts are invalid. 
* @retval  : BVIEW_STATUS_FAILURE -- report is successfully sent 
* @retval  : BVIEW_STATUS_SUCCESS -- failed to get the report
*
* @note : based on the message type the report is retrieved.
*
*********************************************************************/
BVIEW_STATUS bst_get_report (BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  BVIEW_BST_REPORT_SNAPSHOT_t *ss;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BST_UNIT_CXT_t *ptr;
  BVIEW_BST_TRACK_PARAMS_t *track_ptr;
  BVIEW_BST_CONFIG_PARAMS_t *config_ptr;
  BVIEW_TIME_t  curr_time;   
 
  if (NULL == msg_data)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  track_ptr = BST_CONFIG_TRACK_PTR_GET (msg_data->unit);
  config_ptr = BST_CONFIG_FEATURE_PTR_GET (msg_data->unit);
  ptr = BST_UNIT_PTR_GET (msg_data->unit);

  if ((NULL == ptr) || (NULL == track_ptr) || (NULL == config_ptr))
    return BVIEW_STATUS_INVALID_PARAMETER;

  /* request is to get the report.
   */

  if (((BVIEW_BST_PERIODIC == msg_data->report_type) ||
        (BVIEW_BST_STATS_TRIGGER == msg_data->report_type)) &&
      (NULL != track_ptr))
  {
    BVIEW_BST_STAT_COLLECT_CONFIG_t *pCollect = &msg_data->request.collect;

    /* copy the track params to the collection request */
    BST_COPY_TRACK_TO_COLLECT (track_ptr, 
        pCollect);
  }


  if ((BVIEW_BST_CMD_API_GET_REPORT == msg_data->msg_type) ||
      (BVIEW_BST_CMD_API_TRIGGER_REPORT == msg_data->msg_type))
  {
    /* collect data.. since the data is huge.. give the current record 
       memory pointer directly so that we can avoid, copy */
    BST_LOCK_TAKE (msg_data->unit);
    ss = ptr->stats_current_record_ptr;
    /* before we collect data..ensure there is no garbage.. 
     */
    memset (ss, 0,
        sizeof (BVIEW_BST_REPORT_SNAPSHOT_t));
    rv = sbapi_bst_snapshot_get (msg_data->unit, &ss->snapshot_data, &ss->tv);
    BST_LOCK_GIVE (msg_data->unit);

    if (BVIEW_STATUS_SUCCESS != rv)
    {
      /* call failed..  log the reason code ..*/
      LOG_POST (BVIEW_LOG_ERROR, "Failed to get bst stats, err %d \r\n", rv);
      /* Since stats collection has failed.. no need to do the rest.
         report the error to the calling function */
    }

	/* check if stats are requested in percentage format.
	    if yes, then retrieve the default/max buffers allocated from ASIC */
    if (true == config_ptr->statsInPercentage)
	{
          sbapi_system_max_buf_snapshot_get (msg_data->unit, &ptr->bst_max_buffers,
                                          &curr_time);
	}

    if (BVIEW_BST_CMD_API_TRIGGER_REPORT == msg_data->msg_type)
    {
    rv = bst_enable_on_trigger(msg_data, true);
    }
  }

  /* request is to get the thresholds..
   */
  if (BVIEW_BST_CMD_API_GET_THRESHOLD == msg_data->msg_type)
  {
    /* here we need to take the lock, since we maintain only one record for
       thresholds. assumption is that threshold get is called very sparingly,
       where as report is quite often.. so one record is enough.. 
       since we have single record, don't want a read while collecting the  information.
       so protect the same */
    BST_LOCK_TAKE (msg_data->unit);
    /* make sure no garbage.. */
    memset (&ptr->threshold_record_ptr->snapshot_data, 0, sizeof(BVIEW_BST_ASIC_SNAPSHOT_DATA_t));

    /* check if stats are requested in percentage format.
       if yes, then retrieve the default/max buffers allocated from ASIC */
    if (true == config_ptr->statsInPercentage)
    {
      sbapi_system_max_buf_snapshot_get (msg_data->unit, &ptr->bst_max_buffers,
          &curr_time);
    }

    rv = sbapi_bst_threshold_get (msg_data->unit, &ptr->threshold_record_ptr->snapshot_data, 
        &ptr->threshold_record_ptr->tv);
    /* Release  the lock . */
    BST_LOCK_GIVE (msg_data->unit);
    if (BVIEW_STATUS_SUCCESS != rv)
    {
      /* call failed..  log the reason code ..*/
      LOG_POST (BVIEW_LOG_ERROR, "Failed to get bst stats, err %d \r\n", rv);
      /* Since threshold get has failed.. no need to do the rest.
         report the error to the calling function */
    }
  }
  return rv;
}

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
                                                unsigned int id)
{
  BVIEW_BST_CONFIG_PARAMS_t *ptr;
  BVIEW_BST_DATA_t *bst_data_ptr;
  BVIEW_BST_TIMER_t *timer_ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  unsigned interval = 0;

  bst_data_ptr = BST_UNIT_DATA_PTR_GET (unit);
  ptr = BST_CONFIG_FEATURE_PTR_GET (unit);

  if ((NULL == bst_data_ptr) || 
      (NULL == ptr) ||
      (NULL == handler)) 
    return BVIEW_STATUS_INVALID_PARAMETER;

  /* get the appropriate timer context */
  switch (cmd)
  {
    case BVIEW_BST_CMD_API_GET_REPORT:
    timer_ptr = &bst_data_ptr->bst_collection_timer;
    /* fill the context */
    timer_ptr->context.unit = unit;
    timer_ptr->context.cmd = cmd;
    interval = ptr->collectionInterval;
    break;

    case BVIEW_BST_CMD_API_GET_CGSN_DRP_CTRS:
    timer_ptr = &bst_data_ptr->drop_ctrs_db[id].cgsn_drop;
    /* fill the context */
    timer_ptr->context.index = bst_data_ptr->drop_ctrs_db[id].id;
    timer_ptr->context.unit = unit;
    timer_ptr->context.cmd = cmd;
    interval = bst_data_ptr->drop_ctrs_db[id].req.intrvl;
    break;

    default:
    return BVIEW_STATUS_SUCCESS;
  }

  /* check if the timer node is already in use.
  */
  if (true == timer_ptr->in_use)
  {
    /* the timer is in use. The requester has asked
       to add the timer again.. Remove the old entru
       and add it again.. Reasosn could be that config
       interval would have been changed, In such case,
       delete the one with previous collection timer 
       interval and add the new one */
    rv =  bst_periodic_collection_timer_delete(unit,
                                           cmd, id);
    if (BVIEW_STATUS_SUCCESS != rv)
  {
      /* timer node add has failed. log the same */
      LOG_POST (BVIEW_LOG_ERROR, 
          "%s Failed to delete periodic collection time for unit %d, err %d \r\n", __func__, unit, rv);
    }
  }

  /* The timer add function expects the time in milli seconds..
     so convert the time into milli seconds. , before adding
     the timer node */
    rv =  system_timer_add (handler,
                  &timer_ptr->bstTimer, interval*BVIEW_BST_TIME_CONVERSION_FACTOR,
                  PERIODIC_MODE, &timer_ptr->context);

    if (BVIEW_STATUS_SUCCESS == rv)
    {
      timer_ptr->in_use = true;
       LOG_POST (BVIEW_LOG_INFO,
              "bst application: timer is successfully started for unit %d.\r\n", unit);
    }
    else
    {
      /* timer node add has failed. log the same */
      LOG_POST (BVIEW_LOG_ERROR, 
         "Failed to add periodic collection time for unit %d, err %d \r\n", unit, rv);
    }
  return rv;
}

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
                                                   unsigned int id)
{
  BVIEW_BST_DATA_t *bst_data_ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BST_TIMER_t *timer_ptr;

  bst_data_ptr = BST_UNIT_DATA_PTR_GET (unit);

  
  if (NULL == bst_data_ptr)
    return BVIEW_STATUS_INVALID_PARAMETER;

  /* get the appropriate timer context */
  switch (cmd)
  {
    case BVIEW_BST_CMD_API_GET_REPORT:
    timer_ptr = &bst_data_ptr->bst_collection_timer;
    break;

    case BVIEW_BST_CMD_API_GET_CGSN_DRP_CTRS:
    timer_ptr = &bst_data_ptr->drop_ctrs_db[id].cgsn_drop;
    break;

    default:
    return BVIEW_STATUS_SUCCESS;
  }

  if (true == timer_ptr->in_use)
  {
    rv = system_timer_delete (timer_ptr->bstTimer);
    if (BVIEW_STATUS_SUCCESS == rv)
    {
      timer_ptr->in_use = false;
      /* Clear the context */
      memset(&timer_ptr->context, 0, sizeof(BVIEW_BST_TIMER_CONTEXT_t));
        LOG_POST (BVIEW_LOG_INFO,
              "bst application: successfully deleted timer for unit %d , timer id %d.\r\n", unit, timer_ptr->bstTimer);
    }
    else
    {
      /* timer node add has failed. log the same */
      LOG_POST (BVIEW_LOG_ERROR, 
           "Failed to delete periodic collection time for unit %d, err %d \r\n", unit, rv);
    }
  }
  
  return rv;
}

uint64_t round_value( double r ) {
        return (r > 0.0) ? (r + 0.5) : (r - 0.5);
}

/*************************************************************
  Utility function to convert percentage to value 
  ***********************************************************/
void bst_calculate_value_from_percentage(uint64_t maxVal, 
                                         int percentage,
                                         uint64_t *data)
{
  double val;
  val = (double) ((maxVal * percentage)/100);
  *data = round_value(val);

  return;
}
/*********************************************************************
* @brief : set the threshold for the given realm.
*
* @param[in] msg_data : pointer to the bst message request.
*
* @retval  : BVIEW_STATUS_SUCCESS - threshold is set successfully
* @retval  : BVIEW_STATUS_FAILURE - failed to apply the inputted threshold
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note    : In case of any threshold set fail, the error is logged
*
*********************************************************************/
BVIEW_STATUS bst_config_threshold_set (BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  BVIEW_BST_THRESHOLD_HANDLER_t threshold_fn;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

  if (NULL == msg_data)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  if (BVIEW_STATUS_SUCCESS != bst_realm_handler_get(msg_data->threshold_type, &threshold_fn))
  {
    return BVIEW_STATUS_FAILURE;
  }

  rv = threshold_fn(msg_data);

  /* check if the threshold set is successful */ 
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    _BST_LOG(_BST_DEBUG_ERROR,"threshold set failed for the threshold type. %d, err %d \r\n",msg_data->threshold_type, rv); 
    LOG_POST (BVIEW_LOG_ERROR, 
        "threshold set failed for the threshold type. %d, err %d \r\n", 
        msg_data->threshold_type, rv);
  }
  else
  {
    _BST_LOG(_BST_DEBUG_TRACE,   
        "threshold set successful for the threshold type. %d\r\n", 
        msg_data->threshold_type);
    LOG_POST (BVIEW_LOG_INFO, 
        "threshold set successful for the threshold type. %d\r\n", 
        msg_data->threshold_type);
  }

  return rv;
}

/*********************************************************************
* @brief : function to clear the threshold set
*
* @param[in] msg_data : pointer to the bst message request.
*
* @retval  : BVIEW_STATUS_SUCCESS : successfully cleared the threshold values
* @retval  : BVIEW_STATUS_FAILURE : The clearing of thresholds has failed.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note : 
*
*********************************************************************/
BVIEW_STATUS bst_clear_threshold_set (BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BST_UNIT_CXT_t *ptr;
  BVIEW_TIME_t  tv;   

  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  ptr = BST_UNIT_PTR_GET (msg_data->unit);

  rv = sbapi_bst_clear_thresholds (msg_data->unit);
  if (BVIEW_STATUS_SUCCESS == rv)
  {
    BST_LOCK_TAKE (msg_data->unit);
    /* threshold clear is successful.. clear the record as well */
    memset (ptr->threshold_record_ptr, 0, 
        sizeof (BVIEW_BST_REPORT_SNAPSHOT_t));
    /* populate the default thresholds in cache */
    memset(&ptr->bst_thresholds_cache, 0, sizeof(BVIEW_BST_ASIC_SNAPSHOT_DATA_t));
    rv = sbapi_bst_threshold_get (msg_data->unit,
               &ptr->bst_thresholds_cache,
               &tv);

    BST_LOCK_GIVE (msg_data->unit);

      LOG_POST (BVIEW_LOG_INFO, 
           "threshold clear successful for the unit. %d \r\n", 
            msg_data->unit);
  }
  else
  {
    /* log why the clear failed ..*/
      LOG_POST (BVIEW_LOG_ERROR, 
           "threshold clear failed for the unit. %d, err %d \r\n", 
            msg_data->unit, rv);
  }
  return rv;
}

/*********************************************************************
* @brief : clear the stats
*
* @param[in] msg_data : pointer to the bst message request.
*
* @retval  : BVIEW_STATUS_SUCCESS : successfully cleared the stats values
* @retval  : BVIEW_STATUS_FAILURE : The clearing of stats has failed.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note
*
*********************************************************************/
BVIEW_STATUS bst_clear_stats_set (BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BST_UNIT_CXT_t *ptr;

  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  ptr = BST_UNIT_PTR_GET (msg_data->unit);
  /* take the lock */
  BST_LOCK_TAKE (msg_data->unit);
  /* stats clear  */
  memset (ptr->stats_backup_record_ptr, 0,
      sizeof (BVIEW_BST_REPORT_SNAPSHOT_t));
  memset (ptr->stats_active_record_ptr, 0,
      sizeof (BVIEW_BST_REPORT_SNAPSHOT_t));
  memset (ptr->stats_current_record_ptr, 0,
      sizeof (BVIEW_BST_REPORT_SNAPSHOT_t));
  /* release the lock */
  BST_LOCK_GIVE (msg_data->unit);

  /* clear in asic as well*/

  rv = sbapi_bst_clear_stats (msg_data->unit);
  if (BVIEW_STATUS_SUCCESS != rv)
    /* log why the stats clear failed ..*/
    LOG_POST (BVIEW_LOG_ERROR, 
        "stats clear failed for the unit. %d, err %d \r\n", msg_data->unit, rv);
  return rv;
}

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
BVIEW_STATUS bst_update_data (BVIEW_BST_REPORT_TYPE_t type, unsigned int unit)
{
  BVIEW_BST_UNIT_CXT_t *ptr;
  BVIEW_BST_REPORT_SNAPSHOT_t *temp;

  ptr = BST_UNIT_PTR_GET (unit);

  switch (type)
  {
  case BVIEW_BST_STATS:
    BST_LOCK_TAKE (unit);
    /* take the lock */
    /* copy the backup pointer in a temporary variable */
    temp = ptr->stats_backup_record_ptr;
    /* make the active  as backup */
    ptr->stats_backup_record_ptr = ptr->stats_active_record_ptr;

    /* make the current as active */
    ptr->stats_active_record_ptr = ptr->stats_current_record_ptr;
    /* now make the old backup as current */
    ptr->stats_current_record_ptr = temp;
    /* release the lock */
    BST_LOCK_GIVE (unit);
    break;

  default:
    break;
  }

  /* log an information that the records are updated */
      LOG_POST (BVIEW_LOG_INFO, 
           "stat records are updated \r\n"); 

  return BVIEW_STATUS_SUCCESS;
}

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
BVIEW_STATUS bst_module_register ()
{
  BVIEW_MODULE_FETAURE_INFO_t bstInfo;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

  memset (&bstInfo, 0, sizeof (BVIEW_MODULE_FETAURE_INFO_t));

  bstInfo.featureId = BVIEW_FEATURE_BST;
  strncpy (&bstInfo.featureName[0], "bst", strlen("bst"));
  memcpy (bstInfo.restApiList, bst_cmd_api_list,
          sizeof(bst_cmd_api_list));

  /* Register with module manager. */
  rv = modulemgr_register (&bstInfo);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
              "bst application failed to register with module mgr\r\n");
  }
  else
  {
      LOG_POST (BVIEW_LOG_INFO, 
           "module mgr registration for bst successful \r\n"); 
  }
  return rv;
}

/*********************************************************************
* @brief : function to find the free available index in the database 
*
* @param[in] *ptr : pointer to the config pointer.
* @param[in] *avail_index : pointer to the available free index.
*
* @retval  : BVIEW_STATUS_SUCCESS :found free available index 
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
* @retval  : BVIEW_STATUS_TABLE_FULL : Table is full.
*
* @note : 
*
*********************************************************************/
BVIEW_STATUS bst_drop_ctrs_db_get_free_index(BVIEW_BST_DATA_t *ptr, 
                                          unsigned int *avail_index)
{
  unsigned int ii = 0;

  if ((NULL == ptr) ||
      (NULL == avail_index))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  /* find the free slot */
  for (ii = 0; ii < BVIEW_MAX_REQUESTS; ii++)
  {
    if (false == ptr->drop_ctrs_db[ii].in_use) 
    {
      _BST_LOG(_BST_DEBUG_TRACE,   
          "available index =  %d\r\n", ii);
      *avail_index = ii;
      return BVIEW_STATUS_SUCCESS;
    }
  }

  _BST_LOG(_BST_DEBUG_ERROR,   
        "No available index \r\n");
  return BVIEW_STATUS_TABLE_FULL;
}


/*********************************************************************
* @brief : function to get a particular record in drop counters db 
*
* @param[in] *ptr : pointer to the config pointer.
* @param[in]  id:   Id of the record to be found.
* @param[out] *index : pointer to the index of the record in DB.
*
* @retval  : BVIEW_STATUS_SUCCESS :found the record 
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
* @retval  : BVIEW_STATUS_FAILURE : Record is not found.
*
* @note : 
*
*********************************************************************/
BVIEW_STATUS bst_drop_ctrs_db_get_rec_index(BVIEW_BST_DATA_t *ptr, 
                                       unsigned int id, unsigned int *index)
{
  unsigned int ii = 0xFF;

  if ((NULL == ptr) ||
      (NULL == index))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  for (ii = 0; ii < BVIEW_MAX_REQUESTS; ii++)
  {
    if (id == ptr->drop_ctrs_db[ii].id)
    {
      *index = ii;
      return BVIEW_STATUS_SUCCESS;
    }
  }
  /* Failed to find the record */ 
  return BVIEW_STATUS_FAILURE;
}




BVIEW_STATUS bst_cgsn_drop_ctr_get(unsigned int unit,
                                   unsigned int type,
                                   int port,
                                   int queue)
{
   BVIEW_BST_UNIT_CXT_t *ptr;
   unsigned int index = 0;
   uint64_t *stat = NULL;
   BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

   ptr = BST_UNIT_PTR_GET (unit);

    if (NULL == ptr)
      return BVIEW_STATUS_INVALID_PARAMETER;
    /* take the lock */
    BST_LOCK_TAKE (unit);

  switch (type)
  {
    case BVIEW_BST_CGSN_TOTAL:
      index = BVIEW_BST_TOTAL_DROP_CTR_INDEX_GET(port);
      ptr->cgsn_drp_curr->drop_ctrs[index].port = port;
      ptr->cgsn_drp_curr->drop_ctrs[index].type = type;
      stat = &ptr->cgsn_drp_curr->drop_ctrs[index].ctr;
      if (NULL == stat)
      {
        return BVIEW_STATUS_INVALID_PARAMETER;
      }
      rv = sbapi_bst_port_total_cgs_drop_get(unit, port, stat); 
    break;

    case BVIEW_BST_CGSN_UCAST:
      index = BVIEW_BST_UCAST_DROP_CTR_INDEX_GET(port, queue);
      ptr->cgsn_drp_curr->drop_ctrs[index].port = port;
      ptr->cgsn_drp_curr->drop_ctrs[index].queue = queue;
      ptr->cgsn_drp_curr->drop_ctrs[index].type = type;
      stat = &ptr->cgsn_drp_curr->drop_ctrs[index].ctr;
      if (NULL == stat)
        return BVIEW_STATUS_INVALID_PARAMETER;
      rv = sbapi_bst_port_ucast_cgs_drop_get(unit, port, queue, stat);
    break;

    case BVIEW_BST_CGSN_MCAST:
      index = BVIEW_BST_MCAST_DROP_CTR_INDEX_GET(port, queue);
      ptr->cgsn_drp_curr->drop_ctrs[index].port = port;
      ptr->cgsn_drp_curr->drop_ctrs[index].queue = queue;
      ptr->cgsn_drp_curr->drop_ctrs[index].type = type;
      stat = &ptr->cgsn_drp_curr->drop_ctrs[index].ctr;
      if (NULL == stat)
        return BVIEW_STATUS_INVALID_PARAMETER;
      rv = sbapi_bst_port_mcast_cgs_drop_get(unit, port, queue, stat); 
    break;

    default:
    break;
  }

    /* release the lock */
    BST_LOCK_GIVE (unit);
  return rv;
}

BVIEW_STATUS bst_collect_cgsn_drop_counters(unsigned int unit,
                                          BSTJSON_GET_BST_CGSN_DROP_CTRS_t *input,
                                          unsigned int id)
{
  BVIEW_BST_UNIT_CXT_t *ptr;
  BVIEW_PORT_MASK_t port_list;
  BVIEW_QUEUE_MASK_t queue_list, tmp_queue_list;
  unsigned int prt = 0, que = 0, max_prts = 0;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

   ptr = BST_UNIT_PTR_GET (unit);

    if (NULL == ptr)
      return BVIEW_STATUS_INVALID_PARAMETER;

  memset(&port_list, 0, sizeof(BVIEW_PORT_MASK_t));
  memset(&queue_list, 0, sizeof(BVIEW_QUEUE_MASK_t));
  memset(&tmp_queue_list, 0, sizeof(BVIEW_QUEUE_MASK_t));

    ptr = BST_UNIT_PTR_GET (unit);
    /* take the lock */
    BST_LOCK_TAKE (unit);
  /* loop through all the ports */

    /* stats clear  */
    memset (ptr->cgsn_drp_curr, 0,
        sizeof (BVIEW_BST_CGSN_DROPS_t));

    max_prts = ptr->asic_capabilities.numPorts;

    BST_LOCK_GIVE (unit);


  if ((BVIEW_BST_CGSN_TOP_DROPS == input->req_type) ||
      (BVIEW_BST_CGSN_TOP_PRT_Q_DROPS == input->req_type) ||
      (true == input->all_prts))
  {
    for (prt = 1; prt <= max_prts; prt++)
    {
      /* set the pos mask */
      BVIEW_SETMASKBIT(port_list, prt);
    }

   if(BVIEW_BST_CGSN_TOP_PRT_Q_DROPS == input->req_type)
   {
    for (que = 1; que <= BVIEW_BST_APP_NUM_COS_PORT; que++)
    {
      /* set the pos mask */
      BVIEW_SETMASKBIT(queue_list, que);
    }
   }
  }
  else
  {
    memcpy(&port_list, &input->port_list, sizeof(BVIEW_PORT_MASK_t));
    memcpy(&queue_list, &input->queue_list, sizeof(BVIEW_QUEUE_MASK_t));
  }

  prt = 0;
  BVIEW_FLMASKBIT(port_list, prt, sizeof(BVIEW_PORT_MASK_t));

   while (0 != prt)
   {
 
    if ((BVIEW_BST_CGSN_TOP_DROPS == input->req_type)||
        (BVIEW_BST_CGSN_PRT_DROPS == input->req_type))
    {
        /* queue list is not present */
         bst_cgsn_drop_ctr_get(unit, BVIEW_BST_CGSN_TOTAL, prt, 0);
    }
    else
    {

      memcpy(&tmp_queue_list, &queue_list, sizeof(BVIEW_QUEUE_MASK_t));
      que = 0;
       BVIEW_FLMASKBIT(tmp_queue_list, que, sizeof(BVIEW_QUEUE_MASK_t));
       while ((0 != que))
       {
         if(BVIEW_BST_CGSN_ALL == input->queue_type)
         {
           bst_cgsn_drop_ctr_get(unit, BVIEW_BST_CGSN_UCAST, prt, que-1);
           bst_cgsn_drop_ctr_get(unit, BVIEW_BST_CGSN_MCAST, prt, que-1);
         }
         else if (BVIEW_BST_CGSN_UCAST == input->queue_type)
         {
           bst_cgsn_drop_ctr_get(unit, BVIEW_BST_CGSN_UCAST, prt, que-1);
         }
         else if (BVIEW_BST_CGSN_MCAST == input->queue_type)
         {
           bst_cgsn_drop_ctr_get(unit, BVIEW_BST_CGSN_MCAST, prt, que-1);
         }
         BVIEW_CLRMASKBIT(tmp_queue_list, que);
         BVIEW_FLMASKBIT(tmp_queue_list, que, sizeof(BVIEW_QUEUE_MASK_t));
       }
    }

     BVIEW_CLRMASKBIT(port_list, prt);
     BVIEW_FLMASKBIT(port_list, prt, sizeof(BVIEW_PORT_MASK_t));
   }

   /* get the time stamp */
   sbapi_system_time_get(&ptr->cgsn_drp_curr->tv);
   /* copy the req to the record */
   ptr->cgsn_drp_curr->rcvd_req = *input;
   /* copy the request id to record */
   ptr->cgsn_drp_curr->id = id;
   return rv;

}


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
BVIEW_STATUS bst_get_cgsn_drp_ctrs(BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  BVIEW_BST_DATA_t *bst_data_ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  unsigned int ii = 0;
  BSTJSON_GET_BST_CGSN_DROP_CTRS_t req;

  memset(&req, 0, sizeof(BSTJSON_GET_BST_CGSN_DROP_CTRS_t));
  unsigned int id =0;

  if(NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  bst_data_ptr = BST_UNIT_DATA_PTR_GET (msg_data->unit);

  if (NULL == bst_data_ptr) 
    return BVIEW_STATUS_INVALID_PARAMETER;

  {
    if (BVIEW_BST_PERIODIC != msg_data->report_type)
    {
      rv = bst_drop_ctrs_db_get_rec_index(bst_data_ptr, 
           msg_data->id, 
           &ii);
      if (rv == BVIEW_STATUS_SUCCESS)
      {
        /* Record is already present, return ALREADY_CONFIGURED */
        return  BVIEW_STATUS_ALREADY_CONFIGURED;
      }

      rv =  bst_drop_ctrs_db_get_free_index(bst_data_ptr, 
                      &ii);

      if (rv != BVIEW_STATUS_SUCCESS)
      {
        return BVIEW_STATUS_FAILURE;
      }

      if ((0 != msg_data->request.drp_ctrs.intrvl))
      {
        /* add the request to the cache */
        bst_data_ptr->drop_ctrs_db[ii].req = msg_data->request.drp_ctrs;
        bst_data_ptr->drop_ctrs_db[ii].id = msg_data->id;
        bst_data_ptr->drop_ctrs_db[ii].type = msg_data->msg_type;
        bst_data_ptr->drop_ctrs_db[ii].in_use = true;

        /* Start the timer */
        bst_periodic_collection_timer_add(msg_data->unit,
            bst_periodic_collection_cb,
            msg_data->msg_type, ii);
      }

      req = msg_data->request.drp_ctrs;
      id = msg_data->id;
    }
    else
    {
      rv = bst_drop_ctrs_db_get_rec_index(bst_data_ptr, 
           msg_data->id, 
           &ii);
      if (rv == BVIEW_STATUS_SUCCESS)
      { 
      /* retrieve the request from the cache */
         req = bst_data_ptr->drop_ctrs_db[ii].req;
         id = bst_data_ptr->drop_ctrs_db[ii].id;
      }
      else
      {
         return rv;
      }
    }
  }

  {
    /* Call the api to collect the report */
    rv = bst_collect_cgsn_drop_counters(msg_data->unit, &req, id);
  }

  return rv;
}


void bst_sort_records(unsigned int unit, unsigned int count)
{
  BVIEW_BST_UNIT_CXT_t *ptr;
  unsigned int i, j;
  BVIEW_BST_CGSN_CTR_t temp;
  
  ptr = BST_UNIT_PTR_GET (unit);
  if (NULL == ptr)
    return;

  memset(&temp, 0, sizeof(BVIEW_BST_CGSN_CTR_t));

   for (i = 0; i < count; i++)
   {
     for (j = i+1; j < BVIEW_BST_TOTAL_DROP_CTRS; j++)
     {
       if(ptr->cgsn_drp_active->drop_ctrs[i].ctr < ptr->cgsn_drp_active->drop_ctrs[j].ctr)
       {
         temp = ptr->cgsn_drp_active->drop_ctrs[i];
         ptr->cgsn_drp_active->drop_ctrs[i] = ptr->cgsn_drp_active->drop_ctrs[j];
         ptr->cgsn_drp_active->drop_ctrs[j] = temp;
       }
     }
   }

   return;
}


BVIEW_STATUS bst_cancel_request(unsigned int unit, unsigned int id)
{
  BVIEW_BST_DATA_t *bst_data_ptr;
  unsigned int index =0;
  BVIEW_STATUS rv;

  bst_data_ptr = BST_UNIT_DATA_PTR_GET (unit);

  if (NULL == bst_data_ptr) 
    return BVIEW_STATUS_INVALID_PARAMETER;

    /* check if the record already exists */
    rv = bst_drop_ctrs_db_get_rec_index(bst_data_ptr,
                                        id,
                                        &index);

    
    if (BVIEW_STATUS_SUCCESS == rv)
    {
      /* take lock */
      BST_LOCK_TAKE (unit);

      /* clear the timer, if any */
      bst_periodic_collection_timer_delete (unit,
                                            BVIEW_BST_CMD_API_GET_CGSN_DRP_CTRS,
                                            index);

      memset(&bst_data_ptr->drop_ctrs_db[index],0 , sizeof(BVIEW_BST_CGSN_DROP_ELEM_t));

      /* release lock */
      BST_LOCK_GIVE (unit);
      rv = BVIEW_STATUS_SUCCESS;
    }
    else
    {
      rv = BVIEW_STATUS_NOT_CONFIGURED;
    }
    return rv;
}


/* threshold set functions */

BVIEW_STATUS bst_device_threshold_set(BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  BVIEW_BST_UNIT_CXT_t *unit_ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BST_CONFIG_PARAMS_t *config_ptr;
  BVIEW_ASIC_CAPABILITIES_t *asic;
  BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *ptr;
  BVIEW_BST_ASIC_SNAPSHOT_DATA_t *cache_ptr;
  BVIEW_TIME_t  curr_time;   

  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  unit_ptr = BST_UNIT_PTR_GET (msg_data->unit);

  config_ptr = BST_CONFIG_FEATURE_PTR_GET (msg_data->unit);
  asic = &unit_ptr->asic_capabilities;
  ptr = &unit_ptr->bst_max_buffers;
  cache_ptr = &unit_ptr->bst_thresholds_cache;

  if (msg_data->threshold_config_mask & BVIEW_BST_DEVICE_MASK)
  {
    if (true == config_ptr->statsInPercentage)
    {
      BST_VALIDATE_PERCENTAGE_INPUT(msg_data->request.device_threshold.threshold);
      /* convert the percentage to data */
      sbapi_system_max_buf_snapshot_get (msg_data->unit, ptr,
                                          &curr_time);
      bst_calculate_value_from_percentage(ptr->device.data.maxBuf,
          (int) msg_data->request.device_threshold.threshold,
          &msg_data->request.device_threshold.threshold);

      /* threshold is in bytes. convert the same to bytes from cells. */
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.device_threshold.threshold, 
          asic->cellToByteConv);
    }
    else if (true == config_ptr->statUnitsInCells)
    {
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.device_threshold.threshold, 
          asic->cellToByteConv);
    }
    rv =
      sbapi_bst_device_threshold_set (msg_data->unit,
          &msg_data->request.device_threshold);

    if (BVIEW_STATUS_SUCCESS == rv)
    {
      cache_ptr->device.bufferCount = msg_data->request.device_threshold.threshold;
    }
  }
  return rv;
}

BVIEW_STATUS bst_ippg_threshold_set(BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  BVIEW_BST_UNIT_CXT_t *unit_ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BST_CONFIG_PARAMS_t *config_ptr;
  BVIEW_ASIC_CAPABILITIES_t *asic;
  BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *ptr;
  BVIEW_BST_ASIC_SNAPSHOT_DATA_t *cache_ptr;
  BVIEW_TIME_t  curr_time;   

  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  unit_ptr = BST_UNIT_PTR_GET (msg_data->unit);

  config_ptr = BST_CONFIG_FEATURE_PTR_GET (msg_data->unit);
  asic = &unit_ptr->asic_capabilities;
  ptr = &unit_ptr->bst_max_buffers;
  cache_ptr = &unit_ptr->bst_thresholds_cache;


  sbapi_system_max_buf_snapshot_get (msg_data->unit, ptr,
      &curr_time);
  if (msg_data->threshold_config_mask & BVIEW_BST_UMSHARE_MASK)
  {
    if (true == config_ptr->statsInPercentage)
    {
      BST_VALIDATE_PERCENTAGE_INPUT(msg_data->request.i_p_pg_threshold.umShareThreshold);
      /* convert the percentage to data */
      bst_calculate_value_from_percentage(ptr->iPortPg.
          data[msg_data->threshold.port-1]
          [msg_data->threshold.priorityGroup].
          umShareMaxBuf,
          (int) msg_data->request.i_p_pg_threshold.umShareThreshold,
          &msg_data->request.i_p_pg_threshold.umShareThreshold);

      /* threshold is in bytes. convert the same to bytes from cells. */
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.i_p_pg_threshold.umShareThreshold,
          asic->cellToByteConv);
    }
    else if (true == config_ptr->statUnitsInCells)
    {
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.i_p_pg_threshold.umShareThreshold,
          asic->cellToByteConv);
    }
  }
  else
  {
    msg_data->request.i_p_pg_threshold.umShareThreshold = 
      cache_ptr->iPortPg.data[msg_data->threshold.port-1]
      [msg_data->threshold.priorityGroup].umShareBufferCount;
  }


  if (msg_data->threshold_config_mask & BVIEW_BST_UMHEADROOM_MASK)
  {
    if (true == config_ptr->statsInPercentage)
    {
      BST_VALIDATE_PERCENTAGE_INPUT(msg_data->request.i_p_pg_threshold.umHeadroomThreshold);
      /* convert the percentage to data */
      bst_calculate_value_from_percentage(ptr->iPortPg.
          data[msg_data->threshold.port-1]
          [msg_data->threshold.priorityGroup].
          umHeadroomMaxBuf,
          (int) msg_data->request.i_p_pg_threshold.umHeadroomThreshold,
          &msg_data->request.i_p_pg_threshold.umHeadroomThreshold);

      /* threshold is in bytes. convert the same to bytes from cells. */
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.i_p_pg_threshold.umHeadroomThreshold,
          asic->cellToByteConv);
    }
    else if (true == config_ptr->statUnitsInCells)
    {
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.i_p_pg_threshold.umHeadroomThreshold,
          asic->cellToByteConv);
    }
  }
  else
  {
    msg_data->request.i_p_pg_threshold.umHeadroomThreshold = 
      cache_ptr->iPortPg.data[msg_data->threshold.port-1]
      [msg_data->threshold.priorityGroup].umHeadroomBufferCount;
  }

  /* configure ingress port pg threshold */
  rv = sbapi_bst_ippg_threshold_set (msg_data->unit,
      msg_data->threshold.port,
      msg_data->threshold.priorityGroup,
      &msg_data->request.
      i_p_pg_threshold);

  if (BVIEW_STATUS_SUCCESS == rv)
  {
    if (msg_data->threshold_config_mask & BVIEW_BST_UMSHARE_MASK)
    {
      cache_ptr->iPortPg.data[msg_data->threshold.port-1]
        [msg_data->threshold.priorityGroup].umShareBufferCount = 
        msg_data->request.i_p_pg_threshold.umShareThreshold; 
    }
    if (msg_data->threshold_config_mask & BVIEW_BST_UMHEADROOM_MASK)
    {
      cache_ptr->iPortPg.data[msg_data->threshold.port-1]
      [msg_data->threshold.priorityGroup].umHeadroomBufferCount = 
        msg_data->request.i_p_pg_threshold.umHeadroomThreshold; 
    }
  }

  return rv;
}

BVIEW_STATUS bst_ipsp_threshold_set(BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  BVIEW_BST_UNIT_CXT_t *unit_ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BST_CONFIG_PARAMS_t *config_ptr;
  BVIEW_ASIC_CAPABILITIES_t *asic;
  BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *ptr;
  BVIEW_BST_ASIC_SNAPSHOT_DATA_t *cache_ptr;
  BVIEW_TIME_t  curr_time;   

  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  unit_ptr = BST_UNIT_PTR_GET (msg_data->unit);

  config_ptr = BST_CONFIG_FEATURE_PTR_GET (msg_data->unit);
  asic = &unit_ptr->asic_capabilities;
  ptr = &unit_ptr->bst_max_buffers;
  cache_ptr = &unit_ptr->bst_thresholds_cache;


  sbapi_system_max_buf_snapshot_get (msg_data->unit, ptr,
      &curr_time);
  if (msg_data->threshold_config_mask & BVIEW_BST_UMSHARE_MASK)
  {
    if (true == config_ptr->statsInPercentage)
    {
      BST_VALIDATE_PERCENTAGE_INPUT(msg_data->request.i_p_sp_threshold.umShareThreshold);
      /* convert the percentage to data */
      bst_calculate_value_from_percentage(ptr->iPortSp.
          data[msg_data->threshold.port-1]
          [msg_data->threshold.servicePool].
          umShareMaxBuf,
          (int) msg_data->request.i_p_sp_threshold.umShareThreshold,
          &msg_data->request.i_p_sp_threshold.umShareThreshold);

      /* threshold is in bytes. convert the same to bytes from cells. */
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.i_p_sp_threshold.umShareThreshold,
          asic->cellToByteConv);
    }
    else if (true == config_ptr->statUnitsInCells)
    {
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.i_p_sp_threshold.umShareThreshold,
          asic->cellToByteConv);
    }
  }
  else
  {
    msg_data->request.i_p_sp_threshold.umShareThreshold = 
      cache_ptr->iPortSp.data[msg_data->threshold.port-1]
      [msg_data->threshold.servicePool].umShareBufferCount;
  }

  rv = sbapi_bst_ipsp_threshold_set (msg_data->unit,
      msg_data->threshold.port,
      msg_data->threshold.servicePool,
      &msg_data->request.
      i_p_sp_threshold);

  if (BVIEW_STATUS_SUCCESS == rv)
  {
    cache_ptr->iPortSp.data[msg_data->threshold.port-1]
      [msg_data->threshold.servicePool].umShareBufferCount = 
      msg_data->request.i_p_sp_threshold.umShareThreshold;
  }

  return rv;

}

BVIEW_STATUS bst_isp_threshold_set(BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  BVIEW_BST_UNIT_CXT_t *unit_ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BST_CONFIG_PARAMS_t *config_ptr;
  BVIEW_ASIC_CAPABILITIES_t *asic;
  BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *ptr;
  BVIEW_BST_ASIC_SNAPSHOT_DATA_t *cache_ptr;
  BVIEW_TIME_t  curr_time;   

  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  unit_ptr = BST_UNIT_PTR_GET (msg_data->unit);

  config_ptr = BST_CONFIG_FEATURE_PTR_GET (msg_data->unit);
  asic = &unit_ptr->asic_capabilities;
  ptr = &unit_ptr->bst_max_buffers;
  cache_ptr = &unit_ptr->bst_thresholds_cache;


  if (msg_data->threshold_config_mask & BVIEW_BST_UMSHARE_MASK)
  {
    if (true == config_ptr->statsInPercentage)
    {
      BST_VALIDATE_PERCENTAGE_INPUT(msg_data->request.i_sp_threshold.umShareThreshold);
      sbapi_system_max_buf_snapshot_get (msg_data->unit, ptr,
                                          &curr_time);
      /* convert the percentage to data */
      bst_calculate_value_from_percentage(ptr->iSp.
          data[msg_data->threshold.servicePool].
          umShareMaxBuf,
          (int) msg_data->request.i_sp_threshold.umShareThreshold,
          &msg_data->request.i_sp_threshold.umShareThreshold);

      /* threshold is in bytes. convert the same to bytes from cells. */
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.i_sp_threshold.umShareThreshold,
          asic->cellToByteConv);

    }
    else if (true == config_ptr->statUnitsInCells)
    {
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.i_sp_threshold.umShareThreshold,
          asic->cellToByteConv);
    }

  }
  else
  {
    msg_data->request.i_sp_threshold.umShareThreshold = 
      cache_ptr->iSp.
      data[msg_data->threshold.servicePool].
      umShareBufferCount; 
  }

  rv = sbapi_bst_isp_threshold_set (msg_data->unit,
      msg_data->threshold.servicePool,
      &msg_data->request.
      i_sp_threshold);

  if (BVIEW_STATUS_SUCCESS == rv)
  {
    /* update cache */
    cache_ptr->iSp.
      data[msg_data->threshold.servicePool].
      umShareBufferCount =
      msg_data->request.i_sp_threshold.umShareThreshold;

  }

  return rv;
}


BVIEW_STATUS bst_epsp_threshold_set(BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  BVIEW_BST_UNIT_CXT_t *unit_ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BST_CONFIG_PARAMS_t *config_ptr;
  BVIEW_ASIC_CAPABILITIES_t *asic;
  BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *ptr;
  BVIEW_BST_ASIC_SNAPSHOT_DATA_t *cache_ptr;
  BVIEW_TIME_t  curr_time;   

  if (NULL == msg_data)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  unit_ptr = BST_UNIT_PTR_GET (msg_data->unit);

  config_ptr = BST_CONFIG_FEATURE_PTR_GET (msg_data->unit);
  asic = &unit_ptr->asic_capabilities;
  ptr = &unit_ptr->bst_max_buffers;
  cache_ptr = &unit_ptr->bst_thresholds_cache;

 
      sbapi_system_max_buf_snapshot_get (msg_data->unit, ptr,
                                          &curr_time);
  if (msg_data->threshold_config_mask & BVIEW_BST_UCSHARE_MASK)
  {
    if (true == config_ptr->statsInPercentage)
    {
      BST_VALIDATE_PERCENTAGE_INPUT(msg_data->request.ep_sp_threshold.ucShareThreshold);
      /* convert the percentage to data */
      bst_calculate_value_from_percentage(ptr->ePortSp.
          data[msg_data->threshold.port-1]
          [msg_data->threshold.servicePool].
          ucShareMaxBuf,
          (int) msg_data->request.ep_sp_threshold.ucShareThreshold,
          &msg_data->request.ep_sp_threshold.ucShareThreshold);

      /* threshold is in bytes. convert the same to bytes from cells. */
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.ep_sp_threshold.ucShareThreshold,
          asic->cellToByteConv);

    }
    else if (true == config_ptr->statUnitsInCells)
    {
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.ep_sp_threshold.ucShareThreshold,
          asic->cellToByteConv);
    }
  }
  else
  {
    msg_data->request.ep_sp_threshold.ucShareThreshold =
      cache_ptr->ePortSp.data[msg_data->threshold.port-1]
      [msg_data->threshold.servicePool].ucShareBufferCount;
  }


  if (msg_data->threshold_config_mask & BVIEW_BST_UMSHARE_MASK)
  {
    if (true == config_ptr->statsInPercentage)
    {
      BST_VALIDATE_PERCENTAGE_INPUT(msg_data->request.ep_sp_threshold.umShareThreshold);
      /* convert the percentage to data */
      bst_calculate_value_from_percentage(ptr->ePortSp.
          data[msg_data->threshold.port-1]
          [msg_data->threshold.servicePool].
          umShareMaxBuf,
          (int) msg_data->request.ep_sp_threshold.umShareThreshold,
          &msg_data->request.ep_sp_threshold.umShareThreshold);

      /* threshold is in bytes. convert the same to bytes from cells. */
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.ep_sp_threshold.umShareThreshold,
          asic->cellToByteConv);

    }
    else if (true == config_ptr->statUnitsInCells)
    {
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.ep_sp_threshold.umShareThreshold,
          asic->cellToByteConv);
    }

  }
  else
  {
    msg_data->request.ep_sp_threshold.umShareThreshold =
      cache_ptr->ePortSp.data[msg_data->threshold.port-1]
      [msg_data->threshold.servicePool].umShareBufferCount;
  }


  if (msg_data->threshold_config_mask & BVIEW_BST_MCSHARE_MASK)
  {
    if (true == config_ptr->statsInPercentage)
    {
      BST_VALIDATE_PERCENTAGE_INPUT(msg_data->request.ep_sp_threshold.mcShareThreshold);
      /* convert the percentage to data */
      bst_calculate_value_from_percentage(ptr->ePortSp.
          data[msg_data->threshold.port-1]
          [msg_data->threshold.servicePool].
          mcShareMaxBuf,
          (int) msg_data->request.ep_sp_threshold.mcShareThreshold,
          &msg_data->request.ep_sp_threshold.mcShareThreshold);

      /* threshold is in bytes. convert the same to bytes from cells. */
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.ep_sp_threshold.mcShareThreshold,
          asic->cellToByteConv);

    }
    else if (true == config_ptr->statUnitsInCells)
    {
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.ep_sp_threshold.mcShareThreshold,
          asic->cellToByteConv);
    }

  }
  else
  {
    msg_data->request.ep_sp_threshold.mcShareThreshold =
      cache_ptr->ePortSp.data[msg_data->threshold.port-1]
      [msg_data->threshold.servicePool].mcShareBufferCount;
  }

  rv = sbapi_bst_epsp_threshold_set (msg_data->unit,
      msg_data->threshold.port,
      msg_data->threshold.servicePool,
      &msg_data->request.
      ep_sp_threshold);

  if (BVIEW_STATUS_SUCCESS == rv)
  {
    /* update the cahche */

    if (msg_data->threshold_config_mask & BVIEW_BST_UCSHARE_MASK)
    {
      cache_ptr->ePortSp.data[msg_data->threshold.port-1]
        [msg_data->threshold.servicePool].ucShareBufferCount = 
        msg_data->request.ep_sp_threshold.ucShareThreshold;
    }


    if (msg_data->threshold_config_mask & BVIEW_BST_UMSHARE_MASK)
    {
      cache_ptr->ePortSp.data[msg_data->threshold.port-1]
        [msg_data->threshold.servicePool].umShareBufferCount = 
        msg_data->request.ep_sp_threshold.umShareThreshold;
    }

    if (msg_data->threshold_config_mask & BVIEW_BST_MCSHARE_MASK)
    {
      cache_ptr->ePortSp.data[msg_data->threshold.port-1]
        [msg_data->threshold.servicePool].mcShareBufferCount = 
        msg_data->request.ep_sp_threshold.mcShareThreshold;
    }

  }
  return rv;
}


BVIEW_STATUS bst_esp_threshold_set(BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  BVIEW_BST_UNIT_CXT_t *unit_ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BST_CONFIG_PARAMS_t *config_ptr;
  BVIEW_ASIC_CAPABILITIES_t *asic;
  BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *ptr;
  BVIEW_BST_ASIC_SNAPSHOT_DATA_t *cache_ptr;
  BVIEW_TIME_t  curr_time;   

  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  unit_ptr = BST_UNIT_PTR_GET (msg_data->unit);

  config_ptr = BST_CONFIG_FEATURE_PTR_GET (msg_data->unit);
  asic = &unit_ptr->asic_capabilities;
  ptr = &unit_ptr->bst_max_buffers;
  cache_ptr = &unit_ptr->bst_thresholds_cache;


  sbapi_system_max_buf_snapshot_get (msg_data->unit, ptr,
                                          &curr_time);
  if (msg_data->threshold_config_mask & BVIEW_BST_UMSHARE_MASK)
  {
    if (true == config_ptr->statsInPercentage)
    {
      BST_VALIDATE_PERCENTAGE_INPUT(msg_data->request.e_sp_threshold.umShareThreshold);
      /* convert the percentage to data */
      bst_calculate_value_from_percentage(ptr->eSp.
          data[msg_data->threshold.servicePool].
          umShareMaxBuf,
          (int) msg_data->request.e_sp_threshold.umShareThreshold,
          &msg_data->request.e_sp_threshold.umShareThreshold);

      /* threshold is in bytes. convert the same to bytes from cells. */
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.e_sp_threshold.umShareThreshold,
          asic->cellToByteConv);

    }
    else if (true == config_ptr->statUnitsInCells)
    {
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.e_sp_threshold.umShareThreshold,
          asic->cellToByteConv);
    }

  }
  else
  {
    msg_data->request.e_sp_threshold.umShareThreshold =
      cache_ptr->eSp.data[msg_data->threshold.servicePool].
      umShareBufferCount;
  }


  if (msg_data->threshold_config_mask & BVIEW_BST_MCSHARE_MASK)
  {
    if (true == config_ptr->statsInPercentage)
    {
      BST_VALIDATE_PERCENTAGE_INPUT(msg_data->request.e_sp_threshold.mcShareThreshold);
      /* convert the percentage to data */
      bst_calculate_value_from_percentage(ptr->eSp.
          data[msg_data->threshold.servicePool].
          mcShareMaxBuf,
          (int) msg_data->request.e_sp_threshold.mcShareThreshold,
          &msg_data->request.e_sp_threshold.mcShareThreshold);

      /* threshold is in bytes. convert the same to bytes from cells. */
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.e_sp_threshold.mcShareThreshold,
          asic->cellToByteConv);

    }
    else if (true == config_ptr->statUnitsInCells)
    {
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.e_sp_threshold.mcShareThreshold,
          asic->cellToByteConv);
    }

  }
  else
  {
    msg_data->request.e_sp_threshold.mcShareThreshold =
      cache_ptr->eSp.data[msg_data->threshold.servicePool].
      mcShareBufferCount;
  }


  /* configure egress sp threshold */
  rv = sbapi_bst_esp_threshold_set (msg_data->unit,
      msg_data->threshold.servicePool,
      &msg_data->request.
      e_sp_threshold);


  if (BVIEW_STATUS_SUCCESS == rv)
  {
    /* update the cahche */

    if (msg_data->threshold_config_mask & BVIEW_BST_UMSHARE_MASK)
    {
      cache_ptr->eSp.data[msg_data->threshold.servicePool].
        umShareBufferCount = 
        msg_data->request.e_sp_threshold.umShareThreshold;
    }

    if (msg_data->threshold_config_mask & BVIEW_BST_MCSHARE_MASK)
    {
      cache_ptr->eSp.data[msg_data->threshold.servicePool].
        mcShareBufferCount = 
        msg_data->request.e_sp_threshold.mcShareThreshold;
    }
  }

  return rv;

}

BVIEW_STATUS bst_euq_threshold_set(BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  BVIEW_BST_UNIT_CXT_t *unit_ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BST_CONFIG_PARAMS_t *config_ptr;
  BVIEW_ASIC_CAPABILITIES_t *asic;
  BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *ptr;
  BVIEW_BST_ASIC_SNAPSHOT_DATA_t *cache_ptr;
  BVIEW_TIME_t  curr_time;   

  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  unit_ptr = BST_UNIT_PTR_GET (msg_data->unit);

  config_ptr = BST_CONFIG_FEATURE_PTR_GET (msg_data->unit);
  asic = &unit_ptr->asic_capabilities;
  ptr = &unit_ptr->bst_max_buffers;
  cache_ptr = &unit_ptr->bst_thresholds_cache;


  if (msg_data->threshold_config_mask & BVIEW_BST_UC_MASK)
  {
    if (true == config_ptr->statsInPercentage)
    {
      BST_VALIDATE_PERCENTAGE_INPUT(msg_data->request.e_ucq_threshold.ucBufferThreshold);
      /* convert the percentage to data */
      sbapi_system_max_buf_snapshot_get (msg_data->unit, ptr,
                                          &curr_time);
      bst_calculate_value_from_percentage(ptr->eUcQ.data[msg_data->threshold.queue].ucMaxBuf,
          (int) msg_data->request.e_ucq_threshold.ucBufferThreshold,
          &msg_data->request.e_ucq_threshold.ucBufferThreshold);

      /* threshold is in bytes. convert the same to bytes from cells. */
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.e_ucq_threshold.ucBufferThreshold,
          asic->cellToByteConv);

    }
    else if (true == config_ptr->statUnitsInCells)
    {
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.e_ucq_threshold.ucBufferThreshold,
          asic->cellToByteConv);

    }
  }
  else
  {
    msg_data->request.e_ucq_threshold.ucBufferThreshold = 
      cache_ptr->eUcQ.data[msg_data->threshold.queue].ucBufferCount;
  }

  /* configure egress unicast queue threshold */
  rv = sbapi_bst_eucq_threshold_set (msg_data->unit,
      msg_data->threshold.queue,
      &msg_data->request.
      e_ucq_threshold);

  if (BVIEW_STATUS_SUCCESS == rv)
  {
    if (msg_data->threshold_config_mask & BVIEW_BST_UC_MASK)
    {
      cache_ptr->eUcQ.data[msg_data->threshold.queue].ucBufferCount = 
      msg_data->request.e_ucq_threshold.ucBufferThreshold; 

    }
  }

  return rv;

}

BVIEW_STATUS bst_euqg_threshold_set(BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  BVIEW_BST_UNIT_CXT_t *unit_ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BST_CONFIG_PARAMS_t *config_ptr;
  BVIEW_ASIC_CAPABILITIES_t *asic;
  BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *ptr;
  BVIEW_BST_ASIC_SNAPSHOT_DATA_t *cache_ptr;
  BVIEW_TIME_t  curr_time;   

  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  unit_ptr = BST_UNIT_PTR_GET (msg_data->unit);

  config_ptr = BST_CONFIG_FEATURE_PTR_GET (msg_data->unit);
  asic = &unit_ptr->asic_capabilities;
  ptr = &unit_ptr->bst_max_buffers;
  cache_ptr = &unit_ptr->bst_thresholds_cache;


  if (msg_data->threshold_config_mask & BVIEW_BST_UC_MASK)
  {
    if (true == config_ptr->statsInPercentage)
    {
      BST_VALIDATE_PERCENTAGE_INPUT(msg_data->request.e_ucqg_threshold.ucBufferThreshold);
      sbapi_system_max_buf_snapshot_get (msg_data->unit, ptr,
                                          &curr_time);
      /* convert the percentage to data */
      bst_calculate_value_from_percentage(ptr->eUcQg.data[msg_data->threshold.queueGroup].ucMaxBuf,
          (int) msg_data->request.e_ucqg_threshold.ucBufferThreshold,
          &msg_data->request.e_ucqg_threshold.ucBufferThreshold);

      /* threshold is in bytes. convert the same to bytes from cells. */
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.e_ucqg_threshold.ucBufferThreshold,
          asic->cellToByteConv);

    }
    else if (true == config_ptr->statUnitsInCells)
    {
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.e_ucqg_threshold.ucBufferThreshold,
          asic->cellToByteConv);
    }

  }
  else
  {
    msg_data->request.e_ucqg_threshold.ucBufferThreshold = 
      cache_ptr->eUcQg.data[msg_data->threshold.queueGroup].ucBufferCount;
  }
  /* configure egress unicast queuegrp threshold */
  rv = sbapi_bst_eucqg_threshold_set (msg_data->unit,
      msg_data->threshold.queueGroup,
      &msg_data->request.
      e_ucqg_threshold);

  if (BVIEW_STATUS_SUCCESS == rv)
  {
    if (msg_data->threshold_config_mask & BVIEW_BST_UC_MASK)
    {
      cache_ptr->eUcQg.data[msg_data->threshold.queueGroup].ucBufferCount = 
        msg_data->request.e_ucq_threshold.ucBufferThreshold;
    }
  }

  return rv;

}

BVIEW_STATUS bst_emc_threshold_set(BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  BVIEW_BST_UNIT_CXT_t *unit_ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BST_CONFIG_PARAMS_t *config_ptr;
  BVIEW_ASIC_CAPABILITIES_t *asic;
  BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *ptr;
  BVIEW_BST_ASIC_SNAPSHOT_DATA_t *cache_ptr;
  BVIEW_TIME_t  curr_time;   

  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  unit_ptr = BST_UNIT_PTR_GET (msg_data->unit);

  config_ptr = BST_CONFIG_FEATURE_PTR_GET (msg_data->unit);
  asic = &unit_ptr->asic_capabilities;
  ptr = &unit_ptr->bst_max_buffers;
  cache_ptr = &unit_ptr->bst_thresholds_cache;


  if (msg_data->threshold_config_mask & BVIEW_BST_MC_MASK)
  {
    if (true == config_ptr->statsInPercentage)
    {
       BST_VALIDATE_PERCENTAGE_INPUT(msg_data->request.e_mcq_threshold.mcBufferThreshold);
      /* convert the percentage to data */
      sbapi_system_max_buf_snapshot_get (msg_data->unit, ptr,
                                          &curr_time);
      bst_calculate_value_from_percentage(ptr->eMcQ.data[msg_data->threshold.queue].mcMaxBuf,
          (int) msg_data->request.e_mcq_threshold.mcBufferThreshold,
          &msg_data->request.e_mcq_threshold.mcBufferThreshold);

      /* threshold is in bytes. convert the same to bytes from cells. */
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.e_mcq_threshold.mcBufferThreshold,
          asic->cellToByteConv);

    }
    else if (true == config_ptr->statUnitsInCells)
    {
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.e_mcq_threshold.mcBufferThreshold,
          asic->cellToByteConv);
    }

  }
  else
  {
    msg_data->request.e_mcq_threshold.mcBufferThreshold = 
      cache_ptr->eMcQ.data[msg_data->threshold.queue].mcBufferCount;
  }

  /* configure egress mcast queue threshold */
  rv = sbapi_bst_emcq_threshold_set (msg_data->unit,
      msg_data->threshold.queue,
      &msg_data->request.
      e_mcq_threshold);

  if (BVIEW_STATUS_SUCCESS == rv)
  {
    if (msg_data->threshold_config_mask & BVIEW_BST_MC_MASK)
    {
      cache_ptr->eMcQ.data[msg_data->threshold.queue].mcBufferCount = 
        msg_data->request.e_mcq_threshold.mcBufferThreshold;
    }
  }

  return rv;
}

BVIEW_STATUS bst_ecpu_threshold_set(BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  BVIEW_BST_UNIT_CXT_t *unit_ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BST_CONFIG_PARAMS_t *config_ptr;
  BVIEW_ASIC_CAPABILITIES_t *asic;
  BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *ptr;
  BVIEW_BST_ASIC_SNAPSHOT_DATA_t *cache_ptr;
  BVIEW_TIME_t  curr_time;   

  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  unit_ptr = BST_UNIT_PTR_GET (msg_data->unit);

  config_ptr = BST_CONFIG_FEATURE_PTR_GET (msg_data->unit);
  asic = &unit_ptr->asic_capabilities;
  ptr = &unit_ptr->bst_max_buffers;
  cache_ptr = &unit_ptr->bst_thresholds_cache;


  if (msg_data->threshold_config_mask & BVIEW_BST_CPU_MASK)
  {
    if (true == config_ptr->statsInPercentage)
    {
      BST_VALIDATE_PERCENTAGE_INPUT(msg_data->request.cpu_q_threshold.cpuBufferThreshold);
      sbapi_system_max_buf_snapshot_get (msg_data->unit, ptr,
                                          &curr_time);
      /* convert the percentage to data */
      bst_calculate_value_from_percentage(ptr->cpqQ.data[msg_data->threshold.queue].cpuMaxBuf,
          (int) msg_data->request.cpu_q_threshold.cpuBufferThreshold,
          &msg_data->request.cpu_q_threshold.cpuBufferThreshold);

      /* threshold is in bytes. convert the same to bytes from cells. */
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.cpu_q_threshold.cpuBufferThreshold, 
          asic->cellToByteConv);

    }
    else if (true == config_ptr->statUnitsInCells)
    {
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.cpu_q_threshold.cpuBufferThreshold, 
          asic->cellToByteConv);
    }


  }
  else
  {
    msg_data->request.cpu_q_threshold.cpuBufferThreshold =
      cache_ptr->cpqQ.data[msg_data->threshold.queue].cpuBufferCount;
  }

  /* configure egress cpu queue threshold */
  rv = sbapi_bst_cpuq_threshold_set (msg_data->unit,
      msg_data->threshold.queue,
      &msg_data->request.
      cpu_q_threshold);

  if (BVIEW_STATUS_SUCCESS == rv)
  {
    if (msg_data->threshold_config_mask & BVIEW_BST_CPU_MASK)
    {
      cache_ptr->cpqQ.data[msg_data->threshold.queue].cpuBufferCount =
        msg_data->request.cpu_q_threshold.cpuBufferThreshold;
    }
  }

  return rv;

}

BVIEW_STATUS bst_erqe_threshold_set(BVIEW_BST_REQUEST_MSG_t * msg_data)
{
  BVIEW_BST_UNIT_CXT_t *unit_ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BST_CONFIG_PARAMS_t *config_ptr;
  BVIEW_ASIC_CAPABILITIES_t *asic;
  BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *ptr;
  BVIEW_BST_ASIC_SNAPSHOT_DATA_t *cache_ptr;
  BVIEW_TIME_t  curr_time;   

  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  unit_ptr = BST_UNIT_PTR_GET (msg_data->unit);

  config_ptr = BST_CONFIG_FEATURE_PTR_GET (msg_data->unit);
  asic = &unit_ptr->asic_capabilities;
  ptr = &unit_ptr->bst_max_buffers;
  cache_ptr = &unit_ptr->bst_thresholds_cache;

  if (msg_data->threshold_config_mask & BVIEW_BST_RQE_MASK)
  {
    if (true == config_ptr->statsInPercentage)
    {
       sbapi_system_max_buf_snapshot_get (msg_data->unit, ptr, 
                                          &curr_time);
      BST_VALIDATE_PERCENTAGE_INPUT(msg_data->request.rqe_q_threshold.rqeBufferThreshold);
      /* convert the percentage to data */
      bst_calculate_value_from_percentage(ptr->rqeQ.data[msg_data->threshold.queue].rqeMaxBuf,
          (int) msg_data->request.rqe_q_threshold.rqeBufferThreshold,
          &msg_data->request.rqe_q_threshold.rqeBufferThreshold);

      /* threshold is in bytes. convert the same to bytes from cells. */
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.rqe_q_threshold.rqeBufferThreshold, 
          asic->cellToByteConv);

    }
    else if (true == config_ptr->statUnitsInCells)
    {
      BST_CONVERT_CELLS_TO_BYTES(msg_data->request.rqe_q_threshold.rqeBufferThreshold, 
          asic->cellToByteConv);
    }
  }
  else
  {
    msg_data->request.rqe_q_threshold.rqeBufferThreshold =
      cache_ptr->rqeQ.data[msg_data->threshold.queue].rqeBufferCount;
  }

  /* configure egress rqe queue threshold */
  rv = sbapi_bst_rqeq_threshold_set (msg_data->unit,
      msg_data->threshold.queue,
      &msg_data->request.
      rqe_q_threshold);

  if (BVIEW_STATUS_SUCCESS == rv)
  {
    if (msg_data->threshold_config_mask & BVIEW_BST_RQE_MASK)
    {
      cache_ptr->rqeQ.data[msg_data->threshold.queue].rqeBufferCount =
        msg_data->request.rqe_q_threshold.rqeBufferThreshold;
    }
  }

  return rv;
}
