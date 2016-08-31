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
#include "json.h"
#include "broadview.h"
#include "packet_trace_debug.h"
#include "packet_trace.h"
#include "packet_trace_util.h"
#include "packet_trace_include.h"
#include "packet_trace_api.h"
#include "packet_trace_json_memory.h"
#include "system.h"
#include "openapps_log_api.h"
#include "sbplugin_redirect_system.h" 
#include "sbplugin_redirect_packet_trace.h" 
#include "cancel_packet_trace_profile.h" 
#include "get_packet_trace_feature.h" 

BVIEW_PT_CXT_t pt_info;

bool pt_debug_pkt_print = false;

static BVIEW_REST_API_t pt_cmd_api_list[] = {

  {"configure-packet-trace-feature", ptjson_configure_pt_feature},
  {"cancel-packet-trace-profile", ptjson_cancel_pt_profile},
  {"cancel-packet-trace-lag-resolution", ptjson_cancel_pt_lag_resolution},
  {"cancel-packet-trace-ecmp-resolution", ptjson_cancel_pt_ecmp_resolution},
  {"get-packet-trace-feature", ptjson_get_pt_feature},
  {"get-packet-trace-lag-resolution", ptjson_get_pt_lag_resolution},
  {"get-packet-trace-ecmp-resolution", ptjson_get_pt_ecmp_resolution},
  {"get-packet-trace-profile", ptjson_get_pt_profile}
};

/*********************************************************************
* @brief : function to find the free available index in the database 
*
* @param[in] *ptr : pointer to the config pointer.
* @param[in] id : id of the message request.
* @param[in] type : request command type.
* @param[in] *avail_index : pointer to the available free index.
*
* @retval  : BVIEW_STATUS_SUCCESS :found free available index 
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
* @retval  : BVIEW_STATUS_ALREADY_CONFIGURED : request id is already configured.
* @retval  : BVIEW_STATUS_TABLE_FULL : Table is full.
*
* @note : 
*
*********************************************************************/
BVIEW_STATUS pt_find_available_index(BVIEW_PT_CFG_t *ptr, 
                                     unsigned int id, 
                                     unsigned int *avail_index)
{
  unsigned int ii = 0xFF;
  bool found_free_slot = false;

  if ((NULL == ptr) ||
      (NULL == avail_index))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  /* find the free slot */
  for (ii = 0; ii < BVIEW_PT_MAX_PROFILE_REQUESTS; ii++)
  {
    if ((false == ptr->profile_list[ii].in_use) ||
        (id == ptr->profile_list[ii].id))
    {
      _PT_LOG(_PT_DEBUG_TRACE,   
          "available index =  %d\r\n", ii);
      found_free_slot = true;
      break;
    }
  }
  if (false == found_free_slot)
  {

    _PT_LOG(_PT_DEBUG_ERROR,   
        "No available index \r\n");
    return BVIEW_STATUS_TABLE_FULL;
  }

  *avail_index = ii;

  if (true == ptr->profile_list[ii].in_use)
  {
    return BVIEW_STATUS_ALREADY_CONFIGURED;
  }
  return BVIEW_STATUS_SUCCESS;
}


/*********************************************************************
* @brief : application function to configure the packet trace
*
* @param[in] msg_data : pointer to the pt message request.
*
* @retval  : BVIEW_STATUS_SUCCESS : when the asic successfully programmed
* @retval  : BVIEW_STATUS_FAILURE : when the asic is failed to programme.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : Inpput paramerts are invalid. 
*
* @note : This function is invoked in the pt context and used to 
*         configure the parameters like
*         -- packet trace enable
*
*********************************************************************/
BVIEW_STATUS pt_feature_set (BVIEW_PT_REQUEST_MSG_t * msg_data)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_PT_CFG_t *ptr;

  /* check for the null of the input pointer */
  if (NULL == msg_data)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  /* get the configuration structure pointer  for the desired unit */
  ptr = PT_CONFIG_PTR_GET (msg_data->unit);
  if (NULL == ptr)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  if (ptr->feature.ptEnable != msg_data->cmd.feature.ptEnable)
  {
    ptr->feature = msg_data->cmd.feature;
  }

  return rv;
}

/*********************************************************************
* @brief : application function to get the pt feature status
*
* @param[in] msg_data : pointer to the pt message request.
*
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : Inpput paramerts are invalid. 
* @retval  : BVIEW_STATUS_SUCCESS : when the pt feature params is 
*                                   retrieved successfully.
*
* @note
*
*********************************************************************/
BVIEW_STATUS pt_feature_get (BVIEW_PT_REQUEST_MSG_t * msg_data)
{
  BVIEW_PT_CFG_t *ptr;


  if (NULL == msg_data)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  ptr = PT_CONFIG_PTR_GET (msg_data->unit);
  if (NULL == ptr)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }
    
  return  BVIEW_STATUS_SUCCESS;
}

BVIEW_STATUS pt_collect_trace_profile(int unit,
                                      BVIEW_PORT_MASK_t *mask,
                                      BVIEW_PT_PACKET_t *packet,
                                      int id)
{
  BVIEW_PT_PACKET_t pkt;
  BVIEW_PORT_MASK_t *temp_mask, local_mask;
  BVIEW_PT_TRACE_PROFILE_t *profile_data;
  BVIEW_PT_INFO_t *oper_info = NULL;
  BVIEW_TIME_t *time;
  BVIEW_PT_PROFILE_RECORD_t *current;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  unsigned int port = 0;
  char *buffer = NULL;

  oper_info = PT_OPER_PTR_GET(unit);
  

  if ((NULL == oper_info) || (NULL == mask) ||
      (NULL == packet) )
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

    /* copy the packet that is required to collect
       trace profile */
  memset(&pkt, 0, sizeof(BVIEW_PT_PACKET_t));
  memcpy(&pkt, packet, sizeof(BVIEW_PT_PACKET_t));


  /* loop through the mask and for every selected port,
     call the redirector api with the given packet and provide
     buffer to collect the response */


    /* take the lock and collect the profile */
  PT_LOCK_TAKE (unit);

  current = oper_info->current;

  memset(current, 0, sizeof(BVIEW_PT_PROFILE_RECORD_t));

  time = &current->tv;

  temp_mask = &current->port_list;
  memcpy (temp_mask, mask, sizeof(BVIEW_PORT_MASK_t));

  current->id = id;

  memset(&local_mask, 0, sizeof(BVIEW_PORT_MASK_t));
  memcpy (&local_mask, mask, sizeof(BVIEW_PORT_MASK_t));
  /* loop through the mask */
  BVIEW_FLMASKBIT(local_mask, port, sizeof(BVIEW_PORT_MASK_t));

  while ((0 != port) && (BVIEW_STATUS_SUCCESS == rv))
  {
    profile_data = &current->profile[port-1];
    /* take the time stamp */
    sbapi_system_time_get(&current->cpu_tv);

     _PT_LOG(_PT_DEBUG_TRACE,   
         " calling sbapi to collect the profile, len = %d, port = %d \n", pkt.pkt_len, port);
     if(pt_debug_pkt_print)
     {
       buffer = (char *) &pkt.data[0];
       BVIEW_PT_PRINT_CHAR_ARRAY(buffer, pkt.pkt_len);
     }
     rv = sbapi_pt_trace_profile_get_cb(unit, port, &pkt, profile_data, time);  

    BVIEW_CLRMASKBIT(local_mask, port);
    BVIEW_FLMASKBIT(local_mask, port, sizeof(BVIEW_PORT_MASK_t));
  }

    /* release the lock */
  PT_LOCK_GIVE (unit);
  return rv;

}

BVIEW_STATUS pt_pcap_header_delete(char *pcap_pkt, 
                                   BVIEW_PT_PACKET_t *orig_pkt)
{
  PT_PCAP_HDR_t *pcap_file_hdr; 
  char *pkt_offset;
  PT_PCAPREC_HDR_t *header;
  unsigned int cap_len;
  unsigned int packet_len, out_len = 0;
  unsigned int in_length;
  char decoded_data[PT_JSON_MAX_PKT_LEN] = {0};

  /* 
     Decode the packet using base-64 */
  in_length = strlen(pcap_pkt);
  if (BVIEW_STATUS_SUCCESS != 
           pt_base64_decode (pcap_pkt, in_length, &decoded_data[0], sizeof(decoded_data), &out_len))
  {
    return BVIEW_STATUS_FAILURE; 
  }

 if (pt_debug_pkt_print)
 {
  _PT_LOG(_PT_DEBUG_TRACE,   
      " The encoded packet len is %d, decoded packet len is %d  and the packet is below \n", in_length, out_len);
   BVIEW_PT_PRINT_CHAR_ARRAY(decoded_data, out_len);
 }
  pkt_offset = decoded_data;
  /* read the pcap file header */
  pcap_file_hdr = (PT_PCAP_HDR_t *)pkt_offset;
  /*  validte the pcap file header 
   */
  _PT_LOG(_PT_DEBUG_TRACE,   
         " The inputted major num  %d  minor numbes %d\r\n",pcap_file_hdr->version_major,
                     pcap_file_hdr->version_minor);

  /* check for endian */
  if (0xD4C3B2A1 == pcap_file_hdr->magic_number)
  {
    pcap_file_hdr->version_major = BVIEW_SWAP_UINT16(pcap_file_hdr->version_major);
    pcap_file_hdr->version_minor = BVIEW_SWAP_UINT16(pcap_file_hdr->version_minor);
  }

  /* currently supported version is 2.4 */
  if (PT_PCAP_MAJOR_NUM < pcap_file_hdr->version_major) 
  {
     _PT_LOG(_PT_DEBUG_ERROR,   
         "unsupported major number in the input"
         " The inputted major numbers %d\r\n",pcap_file_hdr->version_major);
    return BVIEW_STATUS_INVALID_PARAMETER; 
  }

  /* currently supported version is 2.4 */
  if (PT_PCAP_MINOR_NUM < pcap_file_hdr->version_minor)
  {
     _PT_LOG(_PT_DEBUG_ERROR,   
         "unsupported minor number in the input"
         " The inputted minor number %d\r\n",
                     pcap_file_hdr->version_minor);
    return BVIEW_STATUS_INVALID_PARAMETER; 
  }
  pkt_offset = pkt_offset + sizeof(PT_PCAP_HDR_t);
  header = (PT_PCAPREC_HDR_t *)pkt_offset;

  cap_len = header->incl_len;
  packet_len = header->orig_len;

  /* Check for endian */
  if (0xD4C3B2A1 == pcap_file_hdr->magic_number)
  {
    BVIEW_SWAP_UINT32(cap_len, header->incl_len);
    BVIEW_SWAP_UINT32(packet_len, header->orig_len);
  }

  /* since the pcap header is appended to the packet, 
     find the length of the captured packet 
     and the length of the packet on wire */
  /* read  the packet header */
  cap_len = header->incl_len;
  packet_len = header->orig_len;

     _PT_LOG(_PT_DEBUG_TRACE,   
         " The captured packet len =%d, packet length = %d\r\n",cap_len, packet_len);

     _PT_LOG(_PT_DEBUG_TRACE,   
         " The captured packet len =%d, packet length = %d\r\n",cap_len, packet_len);
  if ((packet_len > cap_len) || (PT_MIN_PKT_SIZE > packet_len) ||
      (PT_MAX_PKT_SIZE < packet_len))
  {
     _PT_LOG(_PT_DEBUG_ERROR,   
         "received packet captured length is invalid. Captured packet is" 
          "either smaller than %d or larger than %d bytes"
         ,PT_MIN_PKT_SIZE, PT_MAX_PKT_SIZE);
    return BVIEW_STATUS_INVALID_PARAMETER; 
  }

  if (packet_len > BVIEW_PT_MAX_PACKET_SIZE-1)
  {
     _PT_LOG(_PT_DEBUG_TRACE,   
         " The packet len =%d, is greater than  = %d\r\n",packet_len, BVIEW_PT_MAX_PACKET_SIZE-1);
    return BVIEW_STATUS_INVALID_PARAMETER; 
  }
  pkt_offset = pkt_offset + sizeof(PT_PCAPREC_HDR_t);
  memcpy(&orig_pkt->data[0], pkt_offset, packet_len);
  orig_pkt->pkt_len = packet_len;

     _PT_LOG(_PT_DEBUG_TRACE,   
         "successfully extraced packet from the input"
         " The extracted packet len =%d, \r\n",packet_len);
 if (pt_debug_pkt_print)
 {
   BVIEW_PT_PRINT_CHAR_ARRAY(pkt_offset, packet_len);
 }
  return BVIEW_STATUS_SUCCESS;
}
/*********************************************************************
* @brief : application function to get the trace profile report 
*
* @param[in] msg_data : pointer to the pt message request.
*
* @retval  : BVIEW_STATUS_INVALID_PARAMETER -- Inpput paramerts are invalid. 
* @retval  : BVIEW_STATUS_FAILURE -- report is successfully sent 
* @retval  : BVIEW_STATUS_SUCCESS -- failed to get the report
*
* @note : based on the message type the report is retrieved.
*
*********************************************************************/
BVIEW_STATUS pt_trace_profile_get (BVIEW_PT_REQUEST_MSG_t * msg_data)
{
  BVIEW_PT_CFG_t *ptr;
  BVIEW_PT_PACKET_t packet;
  BVIEW_PORT_MASK_t port_list;
  BVIEW_STATUS rv = BVIEW_STATUS_FAILURE;
  unsigned int ii = 0;
  int id = 0;

  if (NULL == msg_data)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  memset(&port_list, 0, sizeof(BVIEW_PORT_MASK_t));

  /* if pt is turned off, 
     no need to fetch the trace_profile
     Ignore the same */
  ptr = PT_CONFIG_PTR_GET (msg_data->unit);
  if (NULL == ptr)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  if (false == ptr->feature.ptEnable) 
  {
    _PT_LOG(_PT_DEBUG_ERROR,   
        "packet trace is disabled. Not collecting the trace profile\r\n");
    return BVIEW_STATUS_FAILURE;
  }

  _PT_LOG(_PT_DEBUG_TRACE,   
      "collection interval is %d, id is %d msg type = %ld and unit is %d\r\n", 
      msg_data->cmd.profile.collection_interval, msg_data->id, msg_data->msg_type,
      msg_data->unit);

  /* the received message is not a periodic request */
  if (true != msg_data->report_type_periodic)
  {
    _PT_LOG(_PT_DEBUG_TRACE,   
        "get trace profile request is not a periodic request\r\n");
    /* check for the request type */
    if (PT_PKT == msg_data->cmd.profile.req_method)
    {
      /* the request contains the pcap formatted packet.
         Extract the original packet */
      rv = pt_pcap_header_delete(&msg_data->cmd.profile.req_prfl.pkt.packet[0], &packet);
      if (BVIEW_STATUS_SUCCESS != rv)
      {
        _PT_LOG(_PT_DEBUG_ERROR,   
            "Unable to parse the input packet.\r\n");
        return rv;
      }
    }

    memcpy (&port_list, &msg_data->cmd.profile.pbmp, sizeof(BVIEW_PORT_MASK_t));
    id = msg_data->id;

    if (0 != msg_data->cmd.profile.collection_interval)
    {
      /* find the free slot */
      rv = pt_find_available_index(ptr, id,
          &ii);

      if ((BVIEW_STATUS_SUCCESS != rv) &&
          (BVIEW_STATUS_ALREADY_CONFIGURED != rv))
      {
        return rv;
      }

      /* save the request in database */
      ptr->profile_list[ii].in_use = true;
      ptr->profile_list[ii].id = id;
      ptr->profile_list[ii].type = msg_data->msg_type;
      ptr->profile_list[ii].req_method = msg_data->cmd.profile.req_method;
      memcpy (&ptr->profile_list[ii].port_list, &port_list, sizeof(BVIEW_PORT_MASK_t));
      ptr->profile_list[ii].collection_interval = msg_data->cmd.profile.collection_interval;

      if (PT_PKT == msg_data->cmd.profile.req_method)
      {
        memcpy (&ptr->profile_list[ii].prfl.pcap.packet, &packet, sizeof(BVIEW_PT_PACKET_t));
      }
      if(PT_5_TUPLE == msg_data->cmd.profile.req_method)
      {
        ptr->profile_list[ii].prfl.tuple.pkt_limit = msg_data->cmd.profile.pkt_limit;
        ptr->profile_list[ii].prfl.tuple.rcvd_pkts = 0;
        ptr->profile_list[ii].prfl.tuple.tpl = msg_data->cmd.profile.req_prfl.tuple;
      }


      /* Now we copied the context info for trace profile.
         Start a periodic timer */
      _PT_LOG(_PT_DEBUG_TRACE,
          "starting periodic timer for id  %d \r\n", id);
      rv = pt_periodic_collection_timer_add(msg_data->unit, ii, msg_data->msg_type, msg_data->cmd.profile.req_method);
      if (BVIEW_STATUS_SUCCESS != rv)
      {
        /* log error
         */
        _PT_LOG(_PT_DEBUG_ERROR,
            "Adding periodic timer for id  %d failed.\r\n"
            "unit %d, index %d, msg type %ld\r\n", id, msg_data->unit,
            ii, msg_data->msg_type);
      }
    }
    else
    {
      /* 0 is not an allowed collection interval
         for 5 tuple request */
      if (PT_5_TUPLE == msg_data->cmd.profile.req_method)
      {
        return BVIEW_STATUS_INVALID_PARAMETER;
      }
    }

    /* call the sb api */
    if (PT_5_TUPLE == msg_data->cmd.profile.req_method)
    {
      /* call sb api to create the policy for Live Traffic PT */
      rv = sbapi_pt_5_tuple_profile_create(msg_data->unit,
          msg_data->id,
          &port_list,
          &msg_data->cmd.profile.req_prfl.tuple);
    }
    else
    {
      /* call sb api to collect the profile */
      rv = pt_collect_trace_profile(msg_data->unit,
          &port_list,
          &packet, id);
    }
  }
  else
  {
    if (PT_5_TUPLE == ptr->profile_list[msg_data->id].req_method)
    {
      ptr->profile_list[msg_data->id].prfl.tuple.rcvd_pkts = 0;
    }

    if (PT_PKT == ptr->profile_list[ii].req_method)
    {
      /* packet is already present in the config database.
         Copy the same and query for the trace profile */
      _PT_LOG(_PT_DEBUG_TRACE,   
          "Packet is already present in config data base at index location, %d.\r\n"
          "Copying the packet and port list information to query trace profile", msg_data->id);
      memcpy (&packet, &ptr->profile_list[msg_data->id].prfl.pcap.packet, sizeof(BVIEW_PT_PACKET_t));
      memcpy (&port_list, &ptr->profile_list[msg_data->id].port_list, sizeof(BVIEW_PORT_MASK_t));
      id = ptr->profile_list[msg_data->id].id;

      /* Call api to collect the trace profile */
      rv = pt_collect_trace_profile(msg_data->unit,
          &port_list,
          &packet, id);
    }
  }

  return rv;
}

/*********************************************************************
* @brief : function to add timer for the periodic profile collection 
*
* @param[in] unit : unit for which the periodic profile need to be collected.
* @param[in] index : Index of the table which maps to the id.
*
* @retval  : BVIEW_STATUS_INVALID_PARAMETER -- Inpput paramerts are invalid. 
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
BVIEW_STATUS pt_periodic_collection_timer_add (int  unit, 
                                               int index,
                                               long msg_type,
                                               int method)
{
  BVIEW_PT_CFG_t *ptr;
  BVIEW_PT_INFO_t *oper_info = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_PT_TIMER_t *timer_ptr = NULL;
  int collection_interval;

  ptr = PT_CONFIG_PTR_GET (unit);
  oper_info = PT_OPER_PTR_GET(unit);

  if ((NULL == oper_info) || (NULL == ptr)) 
    return BVIEW_STATUS_INVALID_PARAMETER;

    timer_ptr = &oper_info->pt_timer[index];
    collection_interval = ptr->profile_list[index].collection_interval;


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

    if (timer_ptr->interval != collection_interval) 
    {
      rv =  pt_periodic_collection_timer_delete(unit, index);
      if (BVIEW_STATUS_SUCCESS != rv)
      {
        /* timer node add has failed. log the same */
        LOG_POST (BVIEW_LOG_ERROR, 
            "%s Failed to delete periodic collection time for unit %d, err %d \r\n", __func__, unit, rv);
      }
    }
  }

  timer_ptr->context.index = index;
  timer_ptr->context.unit = unit;
  timer_ptr->context.msg_type = msg_type;
  timer_ptr->context.req_method = method;
  timer_ptr->interval = collection_interval;
  timer_ptr->pt_collection_timer = 0;
  timer_ptr->in_use = false;

  /* The timer add function expects the time in milli seconds..
     so convert the time into milli seconds. , before adding
     the timer node */
    rv =  system_timer_add (pt_periodic_collection_cb,
                  &oper_info->pt_timer[index].pt_collection_timer,
                  oper_info->pt_timer[index].interval*BVIEW_TIME_CONVERSION_FACTOR,
                  PERIODIC_MODE, (void *) &oper_info->pt_timer[index].context);

    if (BVIEW_STATUS_SUCCESS == rv)
    {
      timer_ptr->in_use = true;
       LOG_POST (BVIEW_LOG_INFO,
              "pt application: timer is successfully started for unit %d.\r\n", unit);
    }
    else
    {
      /* timer node add has failed. log the same */
      LOG_POST (BVIEW_LOG_ERROR, 
         "Failed to add periodic collection time for unit %d, index = %d, err %d \r\n", unit, index, rv);
      memset(timer_ptr, 0, sizeof(BVIEW_PT_TIMER_t));
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
BVIEW_STATUS pt_periodic_collection_timer_delete (int unit, int index)
{
  BVIEW_PT_INFO_t *oper_info = NULL;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  BVIEW_PT_TIMER_t *timer_ptr = NULL;

  oper_info = PT_OPER_PTR_GET(unit);

  if (NULL == oper_info)
    return BVIEW_STATUS_INVALID_PARAMETER;

    timer_ptr = &oper_info->pt_timer[index];

   if (true == timer_ptr->in_use)
   {
     rv = system_timer_delete (timer_ptr->pt_collection_timer);

     if (BVIEW_STATUS_SUCCESS == rv)
     {
       LOG_POST (BVIEW_LOG_INFO,
           "pt application: successfully deleted timer for unit %d ,index = %d timer id %d.\r\n", unit, index,
           timer_ptr->pt_collection_timer);
     }
     else
     {
       /* timer node deletion has failed. log the same */
       LOG_POST (BVIEW_LOG_ERROR, 
           "Failed to delete periodic collection time for unit %d, err %d \r\n", unit, rv);
     }
      timer_ptr->in_use = false;
   }

    memset(timer_ptr, 0, sizeof(BVIEW_PT_TIMER_t));
  
  return rv;
}
/*********************************************************************
* @brief : function to cancel the trace profile request
*
* @param[in] msg_data : pointer to the pt message request.
*
* @retval  : BVIEW_STATUS_SUCCESS : successfully cancelled the trace profile
* @retval  : BVIEW_STATUS_FAILURE : The cancel of trace profile has failed.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note : 
*
*********************************************************************/
BVIEW_STATUS pt_trace_profile_cancel (BVIEW_PT_REQUEST_MSG_t * msg_data)
{
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  unsigned int ii = 0;
  BVIEW_PT_CFG_t *ptr;

  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  ptr = PT_CONFIG_PTR_GET (msg_data->unit);
  if (NULL == ptr)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  _PT_LOG(_PT_DEBUG_TRACE,   
      "received cancel request prfile request for  id  %d  msg type %ld\r\n", 
      msg_data->cmd.cancel_profile.id, msg_data->msg_type);

  if ((BVIEW_PT_CMD_API_CANCEL_TRACE_PROFILE != msg_data->msg_type) && 
      (BVIEW_PT_CMD_API_CANCEL_LAG_RESOLUTION != msg_data->msg_type) &&
      (BVIEW_PT_CMD_API_CANCEL_ECMP_RESOLUTION != msg_data->msg_type))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  /* get the request  method */
 rv = pt_find_available_index(ptr,
                              msg_data->cmd.cancel_profile.id,
                              &ii);

  /* clear the timer which is running for this index- unit info */

  _PT_LOG(_PT_DEBUG_TRACE,   
      "cancel the timer for the id  %d \r\n", msg_data->cmd.cancel_profile.id);
  rv = pt_periodic_collection_timer_delete(msg_data->unit, ii);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    /* report error */
    _PT_LOG(_PT_DEBUG_ERROR,   
        "timer deletion failed for id %d \r\n"
        "unit  %d index %d\r\n", msg_data->cmd.cancel_profile.id,
        msg_data->unit, ii);
  }

  /* delete the policy */
  if (PT_5_TUPLE == ptr->profile_list[ii].req_method)
  {
    rv = sbapi_pt_5_tuple_profile_delete(msg_data->unit, 
                                         msg_data->cmd.cancel_profile.id);
  }

  /* clear the config info */
  memset(&ptr->profile_list[ii], 0, sizeof(PT_PROFILE_t));
  _PT_LOG(_PT_DEBUG_TRACE,   
      "cancelled the profile request for the id  %d \r\n", msg_data->cmd.cancel_profile.id);
  return rv;
}

/*********************************************************************
* @brief :  function to register with module mgr
*
* @param[in] : none 
* 
* @retval  : BVIEW_STATUS_SUCCESS : registration of PT with module manager is successful.
* @retval  : BVIEW_STATUS_FAILURE : PT failed to register with module manager.
*
* @note : PT need to register with module manager for the below purpose.
*         When the REST API is invoked, rest queries the module manager for
*         the suitable function api  for the corresponding request. Once the
*         api is retieved , posts the request using the retrieved api.
*
* @end
*********************************************************************/
BVIEW_STATUS pt_module_register ()
{
  BVIEW_MODULE_FETAURE_INFO_t ptInfo;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;

  memset (&ptInfo, 0, sizeof (BVIEW_MODULE_FETAURE_INFO_t));

  ptInfo.featureId = BVIEW_FEATURE_PACKET_TRACE;
  strncpy(&ptInfo.featureName[0], "packettrace", strlen("packettrace"));
  ptInfo.featureId = BVIEW_FEATURE_PACKET_TRACE;
  memcpy (ptInfo.restApiList, pt_cmd_api_list,
                              sizeof (pt_cmd_api_list));

  /* Register with module manager. */
  rv = modulemgr_register (&ptInfo);
  if (BVIEW_STATUS_SUCCESS != rv)
  {
    LOG_POST (BVIEW_LOG_ERROR,
              "pt application failed to register with module mgr\r\n");
  }
  else
  {
      LOG_POST (BVIEW_LOG_INFO, 
           "module mgr registration for pt successful \r\n"); 
  }
  return rv;
}
/*********************************************************************
* @brief : function to collect trace profile for 5 tuple 
*
* @param[in] msg_data : pointer to the pt message request.
*
* @retval  : BVIEW_STATUS_SUCCESS : successfully collected the trace profile
* @retval  : BVIEW_STATUS_FAILURE : The trace profile collection has failed.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
*
* @note : 
*
*********************************************************************/
BVIEW_STATUS pt_trace_pkt_rcv_process(BVIEW_PT_REQUEST_MSG_t * msg_data)
{
  BVIEW_PT_CFG_t *ptr;
  BVIEW_PT_PACKET_t packet;
  BVIEW_PORT_MASK_t port_list;
  BVIEW_STATUS rv = BVIEW_STATUS_SUCCESS;
  unsigned int k = 0;


  if (NULL == msg_data)
    return BVIEW_STATUS_INVALID_PARAMETER;

  ptr = PT_CONFIG_PTR_GET (msg_data->unit);
  if (NULL == ptr)
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  /* find the corresponding index */
   rv = pt_find_available_index(ptr, msg_data->id,
          &k);

   if(BVIEW_STATUS_ALREADY_CONFIGURED != rv)
   {
     _PT_LOG(_PT_DEBUG_TRACE,"packet received from sbplugin"
           " Id %d not found in db\r\n", msg_data->id); 
   }

  if ((PT_5_TUPLE != ptr->profile_list[k].req_method) ||
      (false == ptr->profile_list[k].in_use))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  /* increment the recvd packet count for the id */
   ptr->profile_list[k].prfl.tuple.rcvd_pkts++;
   if (ptr->profile_list[k].prfl.tuple.rcvd_pkts <=
       ptr->profile_list[k].prfl.tuple.pkt_limit)
   {
     /* received packet is with in the configured limit.
        Collect the trace profile for the port */
           memcpy (&packet.data[0], msg_data->cmd.profile.req_prfl.pkt.packet, msg_data->cmd.profile.req_prfl.pkt.packet_len);
           packet.pkt_len = msg_data->cmd.profile.req_prfl.pkt.packet_len;
            memcpy (&port_list, &msg_data->cmd.profile.pbmp, sizeof(BVIEW_PORT_MASK_t));

            /* modify the request type so that the response
               goes out with the desired command type */
            msg_data->msg_type = ptr->profile_list[k].type;
            msg_data->cmd.profile.req_method = ptr->profile_list[k].req_method;

     _PT_LOG(_PT_DEBUG_TRACE,   
         "Packet received from sb plugin calling with id %d \r\n"
         "calling sbapi to collect the profile, len = %d \n", msg_data->id, packet.pkt_len);

       return  pt_collect_trace_profile(msg_data->unit, &port_list, &packet, msg_data->id);

   }

   return BVIEW_STATUS_SUCCESS;

}



