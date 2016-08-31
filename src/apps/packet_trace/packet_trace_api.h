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

#ifndef INCLUDE_PACKET_TRACE_API_H
#define INCLUDE_PACKET_TRACE_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include "modulemgr.h"

#if 0
typedef enum _pt_config_tuple_mask_ {
  PT_CONFIG_TUPLE_SRC_IP = (1 << 0),
  PT_CONFIG_TUPLE_DST_IP = (1 << 1),
  PT_CONFIG_TUPLE_PROTOCOL = (1 << 2),
  PT_CONFIG_TUPLE_SRC_PORT = (1 << 3),
  PT_CONFIG_TUPLE_DST_PORT = (1 << 4)
}PT_CONFIG_TUPLE_MASK_t;

/* 5-tuple information place holder */
typedef struct _pt_5_tuple_params_s_
{
  unsigned int src_ip;
  unsigned int dst_ip;
  unsigned int protocol;
  unsigned int src_port;
  unsigned int dst_port;
  unsigned int 5_tuple_mask;
}PT_5_TUPLE_PARAMS_t;
#endif
 
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
BVIEW_STATUS pt_feature_set (BVIEW_PT_REQUEST_MSG_t * msg_data);


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
BVIEW_STATUS pt_feature_get (BVIEW_PT_REQUEST_MSG_t * msg_data);


/*********************************************************************
* @brief : application function to collect the trace profile 
*
* @param[in] unit : unit number for which the trace profile neess to be collected. 
* @param[in] mask : pointer to the port list mask.
* @param[in] packet : pointer to the packet.
* @param[in] length : length of the packet.
* @param[in] id : id for the corresponding trace profile.
*
* @retval  : BVIEW_STATUS_SUCCESS : when the profile is successfully retrieved.
* @retval  : BVIEW_STATUS_FAILURE : when the profile collection failed.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : Input paramerts are invalid. 
*
*
*********************************************************************/

BVIEW_STATUS pt_collect_trace_profile(int unit,
                                      BVIEW_PORT_MASK_t *mask,
                                      BVIEW_PT_PACKET_t *packet,
                                      int id);


/*********************************************************************
* @brief : application function to remove the pcap header from packet 
*
* @param[in] pcap_pkt : pointer to the packet which contains pcap header. 
* @param[out] orig_pkt : pointer to port after removing the pcap header.
* @param[in] length : length of the packet.
*
* @retval  : BVIEW_STATUS_SUCCESS : when the header is successfully removed. 
* @retval  : BVIEW_STATUS_FAILURE : Removing pcap header failed.
* @retval  : BVIEW_STATUS_INVALID_PARAMETER : Inpput paramerts are invalid. 
*
*
*********************************************************************/
BVIEW_STATUS pt_pcap_header_delete(char *pcap_pkt, 
                                   BVIEW_PT_PACKET_t *orig_pkt);

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
BVIEW_STATUS pt_trace_profile_get (BVIEW_PT_REQUEST_MSG_t * msg_data);


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
                                               int method);


/*********************************************************************
* @brief : Deletes the timer node for the given unit
*
* @param[in] unit : unit id for which  the timer needs to be deleted.
* @param[in] index : Index of the table which maps to the id.
*
* @retval  : BVIEW_STATUS_INVALID_PARAMETER -- Inpput paramerts are invalid. 
* @retval  : BVIEW_STATUS_FAILURE -- timer is successfully deleted 
* @retval  : BVIEW_STATUS_SUCCESS -- failed to delete the timer 
*
* @note  : The periodic timer is deleted when send asyncronous reporting
*          is turned off. This timer is per unit.
*
*********************************************************************/
BVIEW_STATUS pt_periodic_collection_timer_delete (int unit, int index);

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
BVIEW_STATUS pt_trace_profile_cancel (BVIEW_PT_REQUEST_MSG_t * msg_data);


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
BVIEW_STATUS pt_module_register ();


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_PACKET_TRACE_API_H */

