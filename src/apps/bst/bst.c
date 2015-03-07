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
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
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
#include "get_bst_report.h"
#include "bst_json_encoder.h"
#include "bst.h"
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
  {"clear-bst-statistics", bstjson_clear_bst_statistics}
};
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
 
  /* collection interval is maintained in seconds in application.
      while adding the timer, the same should be converted into
       milli seconds and added as the timer api expects 
       interval in milli seconds. */ 
  if (ptr->collectionInterval != msg_data->request.config.collectionInterval)
  {
    /* Collection interval has changed.
       so need to register the modified interval with the timer */
    ptr->collectionInterval = msg_data->request.config.collectionInterval;
    timerUpdateReqd = true;
  }

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
      bst_periodic_collection_timer_add (msg_data->unit);
    }
  }
  else
  {
    ptr->sendAsyncReports = false;
    /* Periodic report collection is turned off...
       so no need for  the timer. 
       delete the timer */
    bst_periodic_collection_timer_delete (msg_data->unit);
  }

  if (ptr->statUnitsInCells != msg_data->request.config.statUnitsInCells)
  {
    /* Store the data is desired in bytes or cells */
    ptr->statUnitsInCells = msg_data->request.config.statUnitsInCells;
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
  rv = sbapi_bst_config_set (msg_data->unit, &bstMode);
  if (BVIEW_STATUS_SUCCESS == rv)
  {
    /* asic is successfully programmed.. now update the config.. */
    ptr->bstEnable = msg_data->request.config.bstEnable;
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

   /* if bst is turned off, 
      no need to send the reports or triggers
      Ignore the same */

  if ((false == config_ptr->bstEnable) &&
      (BVIEW_BST_CMD_API_TRIGGER_REPORT == msg_data->msg_type))
  {
    return BVIEW_STATUS_FAILURE;
  }


  if (((BVIEW_BST_STATS_PERIODIC == msg_data->report_type) ||
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
    if (BVIEW_BST_CMD_API_TRIGGER_REPORT == msg_data->msg_type)
    {
      /* clear thresholds*/
      sbapi_bst_clear_thresholds(msg_data->unit);
      BST_LOCK_TAKE (msg_data->unit);
      /* threshold clear is successful.. clear the record as well */
      memset (ptr->threshold_record_ptr, 0, 
          sizeof (BVIEW_BST_REPORT_SNAPSHOT_t));
      BST_LOCK_GIVE (msg_data->unit);

    } 
    /* collect data.. since the data is huge.. give the current record 
       memory pointer directly so that we can avoid, copy */
    BST_LOCK_TAKE (msg_data->unit);
    ss = ptr->stats_current_record_ptr;
    /* before we collect data..ensure there is no garbage.. 
     */
    memset (ss, 0,
        sizeof (BVIEW_BST_REPORT_SNAPSHOT_t));

    if (true == config_ptr->bstEnable)
    {
      rv = sbapi_bst_snapshot_get (msg_data->unit, &ss->snapshot_data, &ss->tv);

      if (BVIEW_BST_CMD_API_TRIGGER_REPORT == msg_data->msg_type)
      {
        /* Asic would disable the bst on generating the trigger.
           Update the same in the application as well */
        config_ptr->bstEnable = false;
      }
    }
    BST_LOCK_GIVE (msg_data->unit);

    if (BVIEW_STATUS_SUCCESS != rv)
    {
      /* call failed..  log the reason code ..*/
      LOG_POST (BVIEW_LOG_ERROR, "Failed to get bst stats, err %d \r\n", rv);
      /* Since stats collection has failed.. no need to do the rest.
         report the error to the calling function */
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
BVIEW_STATUS bst_periodic_collection_timer_add (unsigned int  unit)
{
  BVIEW_BST_CONFIG_PARAMS_t *ptr;
  BVIEW_BST_DATA_t *bst_data_ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

  bst_data_ptr = BST_UNIT_DATA_PTR_GET (unit);
  ptr = BST_CONFIG_FEATURE_PTR_GET (unit);

  if ((NULL == bst_data_ptr) || (NULL == ptr)) 
    return BVIEW_STATUS_INVALID_PARAMETER;

  /* check if the timer node is already in use.
  */
  if (true == bst_data_ptr->bst_timer.in_use)
  {
    /* the timer is in use. The requester has asked
       to add the timer again.. Remove the old entru
       and add it again.. Reasosn could be that config
       interval would have been changed, In such case,
       delete the one with previous collection timer 
       interval and add the new one */
    rv =  bst_periodic_collection_timer_delete(unit);
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
    rv =  system_timer_add (bst_periodic_collection_cb,
                  &bst_data_ptr->bst_timer.bstCollectionTimer,
                  ptr->collectionInterval*BVIEW_BST_TIME_CONVERSION_FACTOR,
                  PERIODIC_MODE, &bst_data_ptr->bst_timer.unit);

    if (BVIEW_STATUS_SUCCESS == rv)
    {
      bst_data_ptr->bst_timer.in_use = true;
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
*
* @retval  : BVIEW_STATUS_INVALID_PARAMETER -- Inpput paramerts are invalid. 
* @retval  : BVIEW_STATUS_FAILURE -- timer is successfully deleted 
* @retval  : BVIEW_STATUS_SUCCESS -- failed to delete the timer 
*
* @note  : The periodic timer is deleted when send asyncronous reporting
*          is turned off. This timer is per unit.
*
*********************************************************************/
BVIEW_STATUS bst_periodic_collection_timer_delete (int unit)
{
  BVIEW_BST_DATA_t *bst_data_ptr;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

  bst_data_ptr = BST_UNIT_DATA_PTR_GET (unit);

  if (NULL == bst_data_ptr)
    return BVIEW_STATUS_INVALID_PARAMETER;

  if (true == bst_data_ptr->bst_timer.in_use)
  {
    rv = system_timer_delete (bst_data_ptr->bst_timer.bstCollectionTimer);
    if (BVIEW_STATUS_SUCCESS == rv)
    {
      bst_data_ptr->bst_timer.in_use = false;
        LOG_POST (BVIEW_LOG_INFO,
              "bst application: successfully deleted timer for unit %d , timer id %d.\r\n", unit, bst_data_ptr->bst_timer.bstCollectionTimer);
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
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  switch (msg_data->threshold_type)
  {
    /* configure global threshold */
    case BVIEW_BST_DEVICE_THRESHOLD:
      rv =
        sbapi_bst_device_threshold_set (msg_data->unit,
            &msg_data->request.device_threshold);
      break;

    case BVIEW_BST_INGRESS_PORT_PG_THRESHOLD:
         /* configure ingress port pg threshold */
      rv = sbapi_bst_ippg_threshold_set (msg_data->unit,
          msg_data->threshold.port,
          msg_data->threshold.priorityGroup,
          &msg_data->request.
          i_p_pg_threshold);
      break;

    case BVIEW_BST_INGRESS_PORT_SP_THRESHOLD:
         /* configure ingress port + sp threshold */
      rv = sbapi_bst_ipsp_threshold_set (msg_data->unit,
          msg_data->threshold.port,
          msg_data->threshold.servicePool,
          &msg_data->request.
          i_p_sp_threshold);
      break;

    case BVIEW_BST_INGRESS_SP_THRESHOLD:
         /* configure ingress  sp threshold */
      rv = sbapi_bst_isp_threshold_set (msg_data->unit,
          msg_data->threshold.servicePool,
          &msg_data->request.
          i_sp_threshold);
      break;

    case BVIEW_BST_EGRESS_PORT_SP_THRESHOLD:
         /* configure egress port+ sp threshold */
      rv = sbapi_bst_epsp_threshold_set (msg_data->unit,
          msg_data->threshold.port,
          msg_data->threshold.servicePool,
          &msg_data->request.
          ep_sp_threshold);
      break;

    case BVIEW_BST_EGRESS_SP_THRESHOLD:
         /* configure egress sp threshold */
      rv = sbapi_bst_esp_threshold_set (msg_data->unit,
          msg_data->threshold.servicePool,
          &msg_data->request.
          e_sp_threshold);
      break;

    case BVIEW_BST_EGRESS_UC_QUEUE_THRESHOLD:
         /* configure egress unicast queue threshold */
      rv = sbapi_bst_eucq_threshold_set (msg_data->unit,
          msg_data->threshold.queue,
          &msg_data->request.
          e_ucq_threshold);
      break;

    case BVIEW_BST_EGRESS_UC_QUEUEGROUPS_THRESHOLD:
         /* configure egress unicast queuegrp threshold */
      rv = sbapi_bst_eucqg_threshold_set (msg_data->unit,
          msg_data->threshold.queueGroup,
          &msg_data->request.
          e_ucqg_threshold);
      break;

    case BVIEW_BST_EGRESS_MC_QUEUE_THRESHOLD:
         /* configure egress mcast queue threshold */
      rv = sbapi_bst_emcq_threshold_set (msg_data->unit,
          msg_data->threshold.queue,
          &msg_data->request.
          e_mcq_threshold);
      break;

    case BVIEW_BST_EGRESS_CPU_QUEUE_THRESHOLD:
         /* configure egress cpu queue threshold */
      rv = sbapi_bst_cpuq_threshold_set (msg_data->unit,
          msg_data->threshold.queue,
          &msg_data->request.
          cpu_q_threshold);
      break;

    case BVIEW_BST_EGRESS_RQE_QUEUE_THRESHOLD:
         /* configure egress rqe queue threshold */
      rv = sbapi_bst_rqeq_threshold_set (msg_data->unit,
          msg_data->threshold.queue,
          &msg_data->request.
          rqe_q_threshold);
      break;

    default:
      break;
  }

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
  memcpy (bstInfo.restApiList, bst_cmd_api_list,
          (BVIEW_BST_CMD_API_MAX-2) * sizeof (BVIEW_REST_API_t));

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



