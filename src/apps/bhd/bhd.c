/************************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename bhd.c 
  *
  * @purpose BroadView BHD Application command api rocessing functions 
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
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include "json.h"
#include "broadview.h"
#include "bhd.h"
#include "bhd_include.h"
#include "bhd_debug.h"  
#include "packet_multiplexer.h"
#include "system.h"
#include "openapps_log_api.h"
#include "sbplugin_redirect_system.h" 
#include "sbplugin_redirect_bhd.h" 
#include "configure_black_hole_detection.h"  
#include "configure_black_hole.h"  
#include "cancel_black_hole.h"  
#include "get_black_hole_detection.h"
#include "get_black_hole.h"
#include "get_sflow_sampling_status.h"


BVIEW_BHD_CXT_t bhd_info;


static BVIEW_REST_API_t bhd_cmd_api_list[] = {
  {"black-hole-detection-enable", bhdjson_configure_black_hole_detection},
  {"configure-black-hole", bhdjson_configure_black_hole},
  {"cancel-black-hole", bhdjson_cancel_black_hole},
  {"get-black-hole-detection-enable", bhdjson_get_black_hole_detection},
  {"get-black-hole", bhdjson_get_black_hole},
  {"get-sflow-sampling-status", bhdjson_get_sflow_sampling_status}
};

/*********************************************************************
 * @brief : application function to configure the BHD
 *
 * @param[in] msg_data : pointer to the BHD message request.
 *
 * @retval  : BVIEW_STATUS_SUCCESS : when the asic successfully programmed
 * @retval  : BVIEW_STATUS_FAILURE : when the asic is failed to programme.
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER : input parameters are invalid. 
 *
 * @note : This function is invoked in the BHD context and used to 
 *         configure the parameters like
 *         -- bhd enable, sampling-method, sampling-rate, port-list e.t.c
 *
 *********************************************************************/
BVIEW_STATUS bhd_feature_set (BVIEW_BHD_REQUEST_MSG_t * msg_data)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BHD_CONFIG_t *ptr = NULL;
  BVIEW_BHD_INFO_t *oper_ptr = NULL;

  /* check for the null of the input pointer */
  if (NULL == msg_data)
  {
    _BHD_LOG(_BHD_DEBUG_TRACE, "bhd_feature_set: Request message is NULL\n");
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  /* get the configuration structure pointer  for the desired unit */
  ptr = BHD_CONFIG_PTR_GET (msg_data->unit);
  oper_ptr = BHD_OPER_PTR_GET (msg_data->unit);
  if ((NULL == ptr) || (oper_ptr == NULL))
  {
    _BHD_LOG(_BHD_DEBUG_TRACE, "bhd_feature_set:  config ptr=%p, oper ptr=%p \n", ptr, oper_ptr);
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  if (ptr->feature.enable != msg_data->cmd.feature.enable)
  {
    if (msg_data->cmd.feature.enable == false)
    {
      /* Deregister with packet multiplexer */
      if (pkt_mux_deregister(&(oper_ptr->bhd_pkt_mux_hook)) != BVIEW_STATUS_SUCCESS)
      {
        _BHD_LOG(_BHD_DEBUG_TRACE, "BHD: Failed  to deregister with packet multiplexer");
      }
      /* Delete sampling timer */
      if ((rv = bhd_periodic_sampling_timer_delete(msg_data->unit))  
          != BVIEW_STATUS_SUCCESS)
      {
        _BHD_LOG(_BHD_DEBUG_TRACE, "Failed  to delete BHD periodic sampling timer");
      }
      /* Remove Black Hole related configuration */
      sbapi_bhd_cancel_black_hole(msg_data->unit);
      /* Reset configuration */ 
      memset(&(ptr->bh_config), 0x00, sizeof(BVIEW_BLACK_HOLE_CONFIG_t)); 
      ptr->black_hole_configured = false;
      /* Clear operational data */
      oper_ptr->start_sampling = true;
      oper_ptr->current_sample_count = 0;

    }
    ptr->feature = msg_data->cmd.feature;
  }

  _BHD_LOG(_BHD_DEBUG_TRACE, "bhd_feature_set:  successfully configured bhd feature %d \n", msg_data->cmd.feature.enable);
  return rv;
}

/*********************************************************************
 * @brief : application function to get the BHD feature status
 *
 * @param[in] msg_data : pointer to the BHD message request.
 *
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER : input parameters are invalid. 
 * @retval  : BVIEW_STATUS_SUCCESS : when the BHD feature params is 
 *                                   retrieved successfully.
 *
 * @note
 *
 *********************************************************************/
BVIEW_STATUS bhd_feature_get (BVIEW_BHD_REQUEST_MSG_t * msg_data)
{
  BVIEW_BHD_CONFIG_t *ptr;


  if (NULL == msg_data)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  ptr = BHD_CONFIG_PTR_GET (msg_data->unit);
  if (NULL == ptr)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  return  BVIEW_STATUS_SUCCESS;
}


/*********************************************************************
 * @brief : application function to configure Black Hole
 *
 * @param[in] msg_data : pointer to the BHD message request.
 *
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER : input parameters are invalid. 
 * @retval  : BVIEW_STATUS_SUCCESS : when the Black Hole parameters are configured 
 *                                   Correctly
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER : input parameters are invalid. 
 *
 * @note
 *
 *********************************************************************/
BVIEW_STATUS bhd_configure_black_hole (BVIEW_BHD_REQUEST_MSG_t * msg_data)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BHD_CONFIG_t *ptr=NULL;
  BVIEW_BHD_INFO_t *oper_ptr = NULL;
  BVIEW_BHD_PKT_SAMPLING_CONFIG_t *sampling_config=NULL;

  if (NULL == msg_data)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  ptr = BHD_CONFIG_PTR_GET (msg_data->unit);
  oper_ptr = BHD_OPER_PTR_GET (msg_data->unit);
  if ((NULL == ptr) || (oper_ptr == NULL))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  /* black hole can not be configured with out enabling the feature */ 
  if (ptr->feature.enable != true)
  {
    _BHD_LOG(_BHD_DEBUG_TRACE, "Feature is disabled, so failed to configure black hole");
    return BVIEW_STATUS_FEATURE_NOT_ENABLED; 
  } 

  /* Check black hole is already configured or not */
  if (ptr->black_hole_configured == true)
  {
    _BHD_LOG(_BHD_DEBUG_TRACE, "Black hole is already configured, before reconfiguring cancel the existing BH");
    return BVIEW_STATUS_ALREADY_CONFIGURED; 
  }

  sampling_config = &(msg_data->cmd.black_hole_config_params.sampling_config);

  if (!(oper_ptr->sampling_capabilities & sampling_config->sampling_method))
  {
    _BHD_LOG(_BHD_DEBUG_TRACE, "Sampling method is not supported, so failed to configure black hole");
    return BVIEW_STATUS_UNSUPPORTED; 
  }

  /* configure black hole */
  if ((rv = sbapi_bhd_configure_black_hole(msg_data->unit, 
          &(msg_data->cmd.black_hole_config_params))) == BVIEW_STATUS_SUCCESS)
  {
    /* Set  configuration */
    ptr->bh_config = msg_data->cmd.black_hole_config_params;
    /* Add timers and other details if it is agent configuring */
    if (sampling_config->sampling_method == BVIEW_BHD_AGENT_SAMPLING)
    {

      if ((rv = bhd_periodic_sampling_timer_add(msg_data->unit, BVIEW_BHD_CMD_API_EVENT_TIME_PERIOD_START))  
          != BVIEW_STATUS_SUCCESS)
      {
        _BHD_LOG(_BHD_DEBUG_TRACE, "Removing black hole as Failed to add BHD periodic sampling timer ");
        sbapi_bhd_cancel_black_hole(msg_data->unit);
        rv = BVIEW_STATUS_FAILURE;
        return rv;
      }
      else
      {
        /* Add packet multiplexer callback */
        if (pkt_mux_register(&(oper_ptr->bhd_pkt_mux_hook)) == BVIEW_STATUS_SUCCESS)
        {
          /* Start agent sampling related counters */
          oper_ptr->start_sampling = true;
          oper_ptr->current_sample_count = 0;
        }
        else
        {
          _BHD_LOG(_BHD_DEBUG_TRACE, "Removing black hole: Failed to add call back to packet multiplexer");
          bhd_periodic_sampling_timer_delete(msg_data->unit);
          sbapi_bhd_cancel_black_hole(msg_data->unit);
          rv = BVIEW_STATUS_FAILURE;
          return rv;
        }
      } 
    }
    ptr->black_hole_configured = true;
  }
  return rv; 
}


/*********************************************************************
 * @brief : application  to get Black Hole details
 *
 * @param[in] msg_data : pointer to the BHD message request.
 *
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER : Input parameters are invalid. 
 * @retval  : BVIEW_STATUS_SUCCESS :           Input parameters are valied
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER : Input parameters are invalid. 
 *
 * @note
 *
 *********************************************************************/
BVIEW_STATUS bhd_get_black_hole (BVIEW_BHD_REQUEST_MSG_t * msg_data)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BHD_CONFIG_t *ptr;


  if (NULL == msg_data)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  ptr = BHD_CONFIG_PTR_GET (msg_data->unit);
  if (NULL == ptr)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  /* black hole can not be retrieved with out enabling the feature */ 
  if (ptr->feature.enable != true)
  {
    _BHD_LOG(_BHD_DEBUG_TRACE, "Feature is disabled, so failed to get black hole");
    return BVIEW_STATUS_FEATURE_NOT_ENABLED; 
  } 

  /* Check black hole is configured or not */
  if (ptr->black_hole_configured != true)
  {
    _BHD_LOG(_BHD_DEBUG_TRACE, "Black hole is not configured");
    return BVIEW_STATUS_NOT_CONFIGURED; 
  }

  return rv; 
}

/*********************************************************************
 * @brief : application function to cancel Black Hole
 *
 * @param[in] msg_data : pointer to the BHD message request.
 *
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER : input parameters are invalid. 
 * @retval  : BVIEW_STATUS_SUCCESS : when the Black Hole parameters are cancelled 
 *                                   Correctly
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER : input parameters are invalid. 
 *
 * @note
 *
 *********************************************************************/
BVIEW_STATUS bhd_cancel_black_hole (BVIEW_BHD_REQUEST_MSG_t * msg_data)
{
  BVIEW_STATUS               rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BHD_CONFIG_t       *ptr = NULL;
  BVIEW_BHD_INFO_t    *oper_ptr = NULL;


  if (NULL == msg_data)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  ptr = BHD_CONFIG_PTR_GET (msg_data->unit);
  oper_ptr = BHD_OPER_PTR_GET (msg_data->unit);
  if ((NULL == ptr) || (oper_ptr == NULL))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  /* check the feature is enabled or not*/ 
  if (ptr->feature.enable != true)
  {
    _BHD_LOG(_BHD_DEBUG_TRACE, "Feature is disabled, so failed to cancel black hole");
    return BVIEW_STATUS_FEATURE_NOT_ENABLED; 
  } 

  /* Check black hole is configured or not */
  if (ptr->black_hole_configured != true)
  {
    _BHD_LOG(_BHD_DEBUG_TRACE, "Black hole is not configured");
    return BVIEW_STATUS_NOT_CONFIGURED; 
  }

  if ((rv = sbapi_bhd_cancel_black_hole(msg_data->unit)) == BVIEW_STATUS_SUCCESS)
  {
    /* Deregister with packet multiplexer */
    if (pkt_mux_deregister(&(oper_ptr->bhd_pkt_mux_hook)) != BVIEW_STATUS_SUCCESS)
    {
      _BHD_LOG(_BHD_DEBUG_TRACE, "BHD: Failed  to deregister with packet multiplexer");
    }
    /* Delete sampling timer */
    if ((rv = bhd_periodic_sampling_timer_delete(msg_data->unit))  
        != BVIEW_STATUS_SUCCESS)
    {
      _BHD_LOG(_BHD_DEBUG_TRACE, "Failed  to delete BHD periodic sampling timer");
    }
    memset(&(ptr->bh_config), 0x00, sizeof(ptr->bh_config));
    ptr->black_hole_configured = false;
  }

  return rv; 
}


/*********************************************************************
 * @brief : Application function to get sflow sample status
 *
 * @param[in] msg_data : pointer to the BHD message request.
 *
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER : input parameters are invalid. 
 * @retval  : BVIEW_STATUS_SUCCESS :           input parameters are valid. 
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER : input parameters are invalid. 
 *
 * @note
 *
 *********************************************************************/
BVIEW_STATUS bhd_sflow_sample_status_get (BVIEW_BHD_REQUEST_MSG_t * msg_data)
{
  int              index = 0;
  BVIEW_STATUS     rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BHD_INFO_t *ptr;
  BVIEW_PORT_MASK_t local_mask;
  unsigned int port = 0;
  BVIEW_BHD_SFLOW_SAMPLING_STATUS_t *sflow_sampling_status = NULL; 


  if (NULL == msg_data)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  ptr = BHD_OPER_PTR_GET (msg_data->unit);
  if (NULL == ptr)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  /* get the asic sflow sampling capabilities */

  if (!(ptr->sampling_capabilities & BVIEW_BHD_SFLOW_SAMPLING))
  {
    _BHD_LOG(_BHD_DEBUG_TRACE, "Sflow sampling is not supported on this device.");
    return BVIEW_STATUS_UNSUPPORTED;
  }


  memset(&local_mask, 0, sizeof(BVIEW_PORT_MASK_t)); 
  memcpy(&local_mask, &(msg_data->cmd.bhd_sflow_sampling_req.ingress_port_mask),
      sizeof(BVIEW_PORT_MASK_t));

  /* loop through the mask */
  BVIEW_FLMASKBIT(local_mask, port, sizeof(BVIEW_PORT_MASK_t));

  sflow_sampling_status = ptr->sflow_sampling_status;
  memset(sflow_sampling_status, 0x00, sizeof(BVIEW_BHD_SFLOW_SAMPLING_STATUS_t));

  index = 0;
  while ((0 != port) && (BVIEW_STATUS_SUCCESS == rv))
  {
    if ((rv = (sbapi_bhd_sflow_sampling_status_get(msg_data->unit,
              port, &(sflow_sampling_status->port_sflow_sampling[index])))) == BVIEW_STATUS_SUCCESS)  
    {
      sflow_sampling_status->port_sflow_sampling[index].port = port; 
      sflow_sampling_status->num_ports++;
      index++;
    }
    BVIEW_CLRMASKBIT(local_mask, port);
    BVIEW_FLMASKBIT(local_mask, port, sizeof(BVIEW_PORT_MASK_t));
  } 
  sbapi_system_time_get(&(sflow_sampling_status->report_time));
  return rv; 
}

/*********************************************************************
 * @brief : Application function to handle time period start events
 *
 * @param[in] msg_data : pointer to the BHD message request.
 *
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER : input parameters are invalid. 
 * @retval  : BVIEW_STATUS_SUCCESS :  input parameters are valid
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER : input parameters are invalid. 
 *
 * @note
 *
 *********************************************************************/
BVIEW_STATUS bhd_start_time_period(BVIEW_BHD_REQUEST_MSG_t * msg_data)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BHD_INFO_t *ptr;


  if (NULL == msg_data)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  ptr = BHD_OPER_PTR_GET(msg_data->unit);
  if (NULL == ptr)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  ptr->start_sampling = true;
  ptr->current_sample_count = 0;
  return rv; 
}

/*********************************************************************
 * @brief : Application function to handle black holed packets
 *
 * @param[in] msg_data : pointer to the BHD message request.
 *
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER : input parameters are invalid. 
 * @retval  : BVIEW_STATUS_SUCCESS :  input parameters are valid
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER : input parameters are invalid. 
 *
 * @note
 *
 *********************************************************************/
BVIEW_STATUS bhd_process_black_hole_event(BVIEW_BHD_REQUEST_MSG_t * msg_data)
{
  BVIEW_STATUS       rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BHD_INFO_t   *ptr = NULL;
  BVIEW_BHD_CONFIG_t    *cfg_ptr = NULL;
  BVIEW_BLACK_HOLE_CONFIG_t *bh_config_ptr;
  BVIEW_BHD_PKT_SAMPLING_CONFIG_t *sampling_config_ptr;

  if (NULL == msg_data)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  ptr = BHD_OPER_PTR_GET(msg_data->unit);
  cfg_ptr = BHD_CONFIG_PTR_GET (msg_data->unit);
  if ((NULL == ptr) || (NULL == cfg_ptr))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  if (cfg_ptr->feature.enable != true)
  {
    _BHD_LOG(_BHD_DEBUG_TRACE, "BHD feature is disabled: still got black holed packet!!\n");
    return BVIEW_STATUS_FEATURE_NOT_ENABLED;
  }

  /* Check black hole is configured or not */
  if (cfg_ptr->black_hole_configured != true)
  {
    _BHD_LOG(_BHD_DEBUG_TRACE, "Black hole is not configured");
    return BVIEW_STATUS_NOT_CONFIGURED; 
  }

  bh_config_ptr = &(cfg_ptr->bh_config);
  sampling_config_ptr = &(bh_config_ptr->sampling_config);

  /*TBD++  Check ingressport of the packet and egress ports of the packet falls in black hole ports */ 

  if (sampling_config_ptr->sampling_method != BVIEW_BHD_AGENT_SAMPLING)
  {
    _BHD_LOG(_BHD_DEBUG_TRACE, "Agent sampling is not enabled: still got black holed packet!!\n");
    return BVIEW_STATUS_FAILURE;
  }

  /* Check sampling in current period is still enabled */  
  if ((ptr->start_sampling == true) && 
      (ptr->current_sample_count < sampling_config_ptr->sampling_params.agent_sampling_params.sample_count))
  {
    _BHD_LOG(_BHD_DEBUG_TRACE, "Got black holed packet, current_sample_count=%d, configsample_count=%d \n", ptr->current_sample_count, sampling_config_ptr->sampling_params.agent_sampling_params.sample_count);
    rv = BVIEW_STATUS_SUCCESS;
  }
  else
  {
    _BHD_LOG(_BHD_DEBUG_TRACE, "Got black holed packet, but agent sampling in current period is above the configured limit,"
        "so discarding the packet  !!\n");
    rv = BVIEW_STATUS_FAILURE; 
  }
  return rv; 
}

/*********************************************************************
 *  @brief:  callback function to send periodic sampling info  
 *
 * @param[in]   sigval : Data passed with notification after timer expires
 *
 * @retval  : BVIEW_STATUS_SUCCESS : message is successfully posted to pt.
 * @retval  : BVIEW_STATUS_FAILURE : failed to post message to pt.
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameters to function.
 *
 * @note : when the periodic timer expires, this api is invoked in
 *         the timer context. We just post the request for the report, while 
 *        posting we mark the report type as periodic. The parameter returned 
 *       from the callback are the reference to unit id, and the context id. 
 *
 *********************************************************************/
BVIEW_STATUS bhd_periodic_sampling_cb(union sigval sigval)
{
  BVIEW_BHD_REQUEST_MSG_t msg_data;
  BVIEW_STATUS rv;
  BVIEW_BHD_TIMER_CONTEXT_t timer_context;

  memset(&timer_context, 0, sizeof(BVIEW_BHD_TIMER_CONTEXT_t));
  memset(&msg_data, 0, sizeof(BVIEW_BHD_REQUEST_MSG_t));
  timer_context = (*(BVIEW_BHD_TIMER_CONTEXT_t *)sigval.sival_ptr);
  msg_data.unit = timer_context.unit;
  msg_data.msg_type = timer_context.msg_type;
  /* Send the message to the pt application */
  rv = bhd_send_request (&msg_data);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "Failed to send periodic sampling message to BHD application. err = %d\r\n", rv);
    return BVIEW_STATUS_FAILURE;
  } 
  return BVIEW_STATUS_SUCCESS;
}

/*********************************************************************
 * @brief : function to add timer for the periodic profile collection 
 *
 * @param[in] unit : unit for which the periodic profile need to be collected.
 * @param[in] index : Index of the table which maps to the id.
 *
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER -- input parameters are invalid. 
 * @retval  : BVIEW_STATUS_FAILURE -- failed to add the timer 
 * @retval  : BVIEW_STATUS_SUCCESS -- timer is successfully added 
 *
 * @note : this api adds the timer to the linux timer thread, so when the timer 
 *         expires, we receive the callback and post message to the pt application
 *         to collect the profile. this is a periodic timer , whose interval
 *         is equal to the collection interval. Note that collection is per
 *         unit and per request.
 *
 *********************************************************************/
BVIEW_STATUS bhd_periodic_sampling_timer_add (int  unit, 
    long msg_type)
{
  BVIEW_BHD_CONFIG_t *ptr;
  BVIEW_BHD_INFO_t *oper_info = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_BHD_PKT_SAMPLING_PARAMS_t *sampling_params=NULL;

  ptr = BHD_CONFIG_PTR_GET (unit);
  oper_info = BHD_OPER_PTR_GET(unit);

  if ((NULL == oper_info) || (NULL == ptr)) 
    return BVIEW_STATUS_INVALID_PARAMETER;

  sampling_params = &(ptr->bh_config.sampling_config.sampling_params);
  /* check if the timer node is already in use.
   */
  if (true == oper_info->bhd_timer.in_use)
  {
    /* the timer is in use. The requester has asked
       to add the timer again.. Remove the old entru
       and add it again.. Reasosn could be that config
       interval would have been changed, In such case,
       delete the one with previous sampling period timer 
       interval and add the new one */

    if (oper_info->bhd_timer.interval != 
        sampling_params->agent_sampling_params.sample_periodicity)
    {
      rv =  bhd_periodic_sampling_timer_delete(unit);
      if (BVIEW_STATUS_SUCCESS != rv)
      {
        /* timer node add has failed. log the same */
        LOG_POST (BVIEW_LOG_ERROR, 
            "%s Failed to delete periodic sampling time for unit %d, err %d \r\n", __func__, unit, rv);
      }

    }
  }

  oper_info->bhd_timer.context.unit = unit;
  oper_info->bhd_timer.context.msg_type = msg_type;
  oper_info->bhd_timer.interval = sampling_params->agent_sampling_params.sample_periodicity;
  oper_info->bhd_timer.in_use = false;

  /* The timer add function expects the time in milli seconds..
     so convert the time into milli seconds. , before adding
     the timer node */
  rv =  system_timer_add (bhd_periodic_sampling_cb,
      &oper_info->bhd_timer.bhd_sampling_timer,
      oper_info->bhd_timer.interval*BVIEW_TIME_CONVERSION_FACTOR,
      PERIODIC_MODE, (void *) &oper_info->bhd_timer.context);

  if (BVIEW_STATUS_SUCCESS == rv)
  {
    oper_info->bhd_timer.in_use = true;
    LOG_POST (BVIEW_LOG_INFO,
        "bhd application: timer is successfully started for unit %d.\r\n", unit);
  }
  else
  {
    /* timer node add has failed. log the same */
    LOG_POST (BVIEW_LOG_ERROR, 
        "Failed to add periodic sampling time for unit %d, index = %d, err %d \r\n", unit, index, rv);
    memset(&oper_info->bhd_timer, 0, sizeof(BVIEW_BHD_TIMER_t));
  }
  return rv;
}

/*********************************************************************
 * @brief : Deletes the timer node for the given unit
 *
 * @param[in] unit : unit id for which  the timer needs to be deleted.
 *
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER -- input parameters are invalid. 
 * @retval  : BVIEW_STATUS_FAILURE -- timer is successfully deleted 
 * @retval  : BVIEW_STATUS_SUCCESS -- failed to delete the timer 
 *
 * @note  : The periodic timer is deleted when send asyncronous reporting
 *          is turned off. This timer is per unit.
 *
 *********************************************************************/
BVIEW_STATUS bhd_periodic_sampling_timer_delete (int unit)
{
  BVIEW_BHD_INFO_t *oper_info = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

  oper_info = BHD_OPER_PTR_GET(unit);

  if (NULL == oper_info)
    return BVIEW_STATUS_INVALID_PARAMETER;

  if (true == oper_info->bhd_timer.in_use)
  {
    rv = system_timer_delete (oper_info->bhd_timer.bhd_sampling_timer);

    if (BVIEW_STATUS_SUCCESS == rv)
    {
      LOG_POST (BVIEW_LOG_INFO,
          "bhd application: successfully deleted timer for unit %d ,index = %d timer id %d.\r\n", unit, index,
          oper_info->bhd_timer.bhd_sampling_timer);
    }
    else
    {
      /* timer node deletion has failed. log the same */
      LOG_POST (BVIEW_LOG_ERROR, 
          "Failed to delete sample periodic timer for unit %d, err %d \r\n", unit, rv);
    }
    oper_info->bhd_timer.in_use = false;
  }

  memset(&oper_info->bhd_timer, 0, sizeof(BVIEW_BHD_TIMER_t));

  return rv;
}

/*****************************************************************************************
 * @brief :  function to register with module mgr
 *
 * @param[in] : none 
 * 
 * @retval  : BVIEW_STATUS_SUCCESS : registration of BHD with module manager is successful.
 * @retval  : BVIEW_STATUS_FAILURE : BHD failed to register with module manager.
 *
 * @note : BHD need to register with module manager for the below purpose.
 *         When the REST API is invoked, rest queries the module manager for
 *         the suitable function api  for the corresponding request. Once the
 *         api is retieved , posts the request using the retrieved api.
 *
 * @end
 *******************************************************************************************/
BVIEW_STATUS bhd_module_register ()
{
  BVIEW_MODULE_FETAURE_INFO_t bhdInfo;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

  memset (&bhdInfo, 0, sizeof (BVIEW_MODULE_FETAURE_INFO_t));

  bhdInfo.featureId = BVIEW_FEATURE_BHD;
  strncpy(&bhdInfo.featureName[0], "black-hole-detection", strlen("black-hole-detection"));
  memcpy (bhdInfo.restApiList, bhd_cmd_api_list, sizeof(bhd_cmd_api_list)); 

  /* Register with module manager. */
  rv = modulemgr_register (&bhdInfo);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
        "BHD application failed to register with module mgr\r\n");
  }
  else
  {
    LOG_POST (BVIEW_LOG_INFO, 
        "module mgr registration for BHD successful \r\n"); 
  }
  return rv;
}


/*********************************************************************
 * @brief   BHD packet multiplexer callback function
 *
 * @param    pkt_info_ptr @b{(input)} Pointer to packet info
 *
 * @returns  BVIEW_STATUS_SUCCESS    if packet data is valid and posted
 *                                   pkt info to BHD app
 * @returns  BVIEW_STATUS_FAILURE    if packet data is invalid and failed
 *                                   to post to 
 *                                  
 *
 * @notes    This function sends the packet and its info to BHD application 
 *                   through message queue
 *
 * @end
 *********************************************************************/

BVIEW_STATUS bhd_pkt_mux_cb(BVIEW_PACKET_MSG_t *pkt_info_ptr)
{
  BVIEW_BHD_REQUEST_MSG_t   msg_data;
  BVIEW_STATUS              rv = BVIEW_STATUS_SUCCESS;
  struct msqid_ds bhd_msgq_attr;

  
  /* Check the number of messages in msg queue */
  if (msgctl(bhd_info.recvMsgQid, IPC_STAT, &bhd_msgq_attr) == 0)
  {
    if (bhd_msgq_attr.msg_qnum > (SYSTEM_MSGQ_MAX_MSG/2))
    {
      return BVIEW_STATUS_FAILURE;
    }
  }
  memset(&msg_data, 0, sizeof(BVIEW_BHD_REQUEST_MSG_t));

  /* Copy packet related information */  
  msg_data.unit = pkt_info_ptr->packet.asic;
  msg_data.msg_type = BVIEW_BHD_CMD_API_GET_BLACK_HOLE_EVENT_REPORT;
  msg_data.cmd.sample_packet.pkt_len = pkt_info_ptr->packet.pkt_len;
  msg_data.cmd.sample_packet.ing_port = pkt_info_ptr->packet.source_port;
  msg_data.cmd.sample_packet.egr_pbmp = pkt_info_ptr->packet.dst_port_mask;
  /* Check packet length */
  if (pkt_info_ptr->packet.pkt_len > BVIEW_BHD_MAX_PKT_SIZE)
  {
    LOG_POST (BVIEW_LOG_ERROR, 
        "Packet length fiven by packet multiplexer is greater than" 
        "the MAX supported length\r\n"); 

    return BVIEW_STATUS_FAILURE;
  }
  memcpy(msg_data.cmd.sample_packet.pkt_data, pkt_info_ptr->packet.data, pkt_info_ptr->packet.pkt_len);
  rv = bhd_send_request (&msg_data);
  return rv;
}



