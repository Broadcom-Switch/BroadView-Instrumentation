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

#include "ptapp.h"

/* Rest mesages for JSON methods */ 
PTAPP_REST_MSG_t ptRestMessages[] = {
    {
     .methodId = PTAPP_JSON_CONFIGURE_PACKET_TRACE_FEATURE,
     .httpMethod = "POST",
     .descr = "Setting up Packet Trace feature",
     .method = "configure-packet-trace-feature",
    },
#ifdef DROP_REASON_SUPPORTED 
    {
     .methodId = PTAPP_JSON_CONFIGURE_PACKET_TRACE_DROP_REASON,
     .httpMethod = "POST",
     .descr = "Setting up Packet Trace drop reason",
     .method = "configure-packet-trace-drop-reason",
    },
#endif
    {
     .methodId = PTAPP_JSON_GET_PACKET_TRACE_FEATURE,
     .httpMethod = "GET",
     .descr = "Obtaining current Packet Trace feature status",
     .method = "get-packet-trace-feature",
    },
    {
     .methodId = PTAPP_JSON_GET_PACKET_TRACE_PROFILE, 
     .httpMethod = "GET",
     .descr = "Setting up Trace request and Obtaining trace profile for a given packet",
     .method = "get-packet-trace-profile",
    },
    {
     .methodId = PTAPP_JSON_GET_PACKET_TRACE_LAG_RESOLUTION, 
     .httpMethod = "GET",
     .descr = "Setting up resolution request and Obtaining LAG resolution for a given packet",
     .method = "get-packet-trace-lag-resolution",
    },
    {
     .methodId = PTAPP_JSON_GET_PACKET_TRACE_ECMP_RESOLUTION, 
     .httpMethod = "GET",
     .descr = "Setting up resolution request and Obtaining ECMP resolution for a given packet",
     .method = "get-packet-trace-ecmp-resolution",
    },
#ifdef DROP_REASON_SUPPORTED 
    {
     .methodId = PTAPP_JSON_GET_PACKET_TRACE_DROP_REASON,
     .httpMethod = "GET",
     .descr = "Obtaining current configured drop reasons",
     .method = "get-packet-trace-drop-reason",
    },
    {
     .methodId = PTAPP_JSON_GET_PACKET_TRACE_DROP_COUNTER_REPORT,
     .httpMethod = "GET",
     .descr = "Obtaining a drop counter report ",
     .method = "get-packet-trace-drop-counter-report",
    },
    {
     .methodId = PTAPP_JSON_GET_PACKET_TRACE_SUPPORTED_DROP_REASONS,
     .httpMethod = "GET",
     .descr = "Obtaining the supported drop reasons on the silicon",
     .method = "get-packet-trace-supported-drop-reasons",
    },
#endif
    {
     .methodId = PTAPP_JSON_CANCEL_PACKET_TRACE_PROFILE,
     .httpMethod = "POST",
     .descr = "Cancelling trace profile request",
     .method = "cancel-packet-trace-profile",
    },
    {
     .methodId = PTAPP_JSON_CANCEL_PACKET_TRACE_LAG_RESOLUTION,
     .httpMethod = "POST",
     .descr = "Cancelling LAG resolution request",
     .method = "cancel-packet-trace-lag-resolution",
    },
    {
     .methodId = PTAPP_JSON_CANCEL_PACKET_TRACE_ECMP_RESOLUTION,
     .httpMethod = "POST",
     .descr = "Cancelling ECMP resolution request",
     .method = "cancel-packet-trace-ecmp-resolution",
    },
#ifdef DROP_REASON_SUPPORTED 
    {
     .methodId = PTAPP_JSON_CANCEL_PACKET_TRACE_SEND_DROP_PACKET,
     .httpMethod = "POST",
     .descr = "Cancelling send dropped packet requests",
     .method = "cancel-packet-trace-send-drop-packet",
    },
    {
     .methodId = PTAPP_JSON_CANCEL_PACKET_TRACE_DROP_COUNTER_REPORT,
     .httpMethod = "POST",
     .descr = "Cancelling send dropped counter reports",
     .method = "cancel-packet-trace-drop-counter-report",
    },
#endif
    {
     .methodId = PTAPP_JSON_GET_LIVE_PACKET_TRACE_LAG_RESOLUTION, 
     .httpMethod = "GET",
     .descr = "Setting up 5 tuple request and Obtaining LAG resolution for captured live packet",
     .method = "get-packet-trace-lag-resolution",
    },
    
};


/* JSON format for different methods */ 
PTAPP_JSON_METHOD_INFO_t ptJsonMethodDetails[PTAPP_JSON_METHOD_LAST] = {
    {
     .methodId = PTAPP_JSON_CONFIGURE_PACKET_TRACE_FEATURE,
     .method = "configure-packet-trace-feature",  
     .menuString = "Configure Packet Trace Feature",
     .get_input_for_json = config_pt_feature,
     .jsonFrmt = "{      \
                     \"jsonrpc\": \"2.0\", \
                     \"method\": \"configure-packet-trace-feature\", \
                     \"asic-id\": \"%s\", \
                     \"params\": {  \
                     \"packet-trace-enable\": %u \
                     }, \
				     \"id\": %u \
                  }",

    },

#ifdef DROP_REASON_SUPPORTED 
    {
     .methodId = PTAPP_JSON_CONFIGURE_PACKET_TRACE_DROP_REASON,
     .method = "configure-packet-trace-drop-reason",  
     .menuString = "Configure Packet Trace drop reasons"
     .get_input_for_json = config_pt_drop_reason,
     .jsonFrmt = "{      \
                     \"jsonrpc\": \"2.0\", \
                     \"method\": \"configure-packet-trace-drop-reason\", \
                     \"asic-id\": \"%s\", \
                     \"params\": {  \
                     \"reason\" : [%s], \
                     \"port-list\" : \"%s\", \
                     \"send-dropped-packet\" : %u, \
                     \"trace-profile\" : %u, \
                     \"packet-count\" : %u, \
                     \"packet-threshold\" : %u \
                     }, \
                     \"id\": %u \
                  }",

    },
#endif

    {
     .methodId = PTAPP_JSON_GET_PACKET_TRACE_FEATURE,
     .method = "get-packet-trace-feature",  
     .menuString = "Get Packet Trace feature status",
     .get_input_for_json = get_pt_feature,
     .jsonFrmt = "{      \
                     \"jsonrpc\": \"2.0\", \
                     \"method\": \"get-packet-trace-feature\", \
                     \"asic-id\": \"%s\", \
                     \"params\": {  \
                     }, \
                     \"id\": %u \
                  }",
    },

    {
     .methodId = PTAPP_JSON_GET_PACKET_TRACE_PROFILE, 
     .method = "get-packet-trace-profile",  
     .menuString = "Trace a packet",
     .get_input_for_json = get_pt_profile,
     .jsonFrmt = "{      \
                     \"jsonrpc\": \"2.0\", \
                     \"method\": \"get-packet-trace-profile\", \
                     \"asic-id\": \"%s\", \
                     \"params\": {  \
                     \"packet\" : \"%s\", \
                     \"port-list\" : [%s], \
                     \"collection-interval\" : %u, \
                     \"drop-packet\" : %u \
                     }, \
				     \"id\": %u \
                  }",

    },

    {
     .methodId = PTAPP_JSON_GET_PACKET_TRACE_LAG_RESOLUTION, 
     .method = "get-packet-trace-lag-resolution",
     .menuString = "Trace LAG resolution for a packet",
     .get_input_for_json = get_pt_profile,
     .jsonFrmt = "{      \
                     \"jsonrpc\": \"2.0\", \
                     \"method\": \"get-packet-trace-lag-resolution\", \
                     \"asic-id\": \"%s\", \
                     \"params\": {  \
                     \"packet\" : \"%s\", \
                     \"port-list\" : [%s], \
                     \"collection-interval\" : %u, \
                     \"drop-packet\" : %u \
                     }, \
				     \"id\": %u \
                  }",

    },
    {
     .methodId = PTAPP_JSON_GET_PACKET_TRACE_ECMP_RESOLUTION, 
     .method = "get-packet-trace-ecmp-resolution",
     .menuString = "Trace ECMP resolution for a packet",
     .get_input_for_json = get_pt_profile,
     .jsonFrmt = "{      \
                     \"jsonrpc\": \"2.0\", \
                     \"method\": \"get-packet-trace-ecmp-resolution\", \
                     \"asic-id\": \"%s\", \
                     \"params\": {  \
                     \"packet\" : \"%s\", \
                     \"port-list\" : [%s], \
                     \"collection-interval\" : %u, \
                     \"drop-packet\" : %u \
                     }, \
				     \"id\": %u \
                  }",

    },
    {
     .methodId = PTAPP_JSON_GET_LIVE_PACKET_TRACE_PROFILE, 
     .method = "get-packet-trace-profile",  
     .menuString = "Capture and Trace a live packet",
     .get_input_for_json = get_live_pt_profile,
	 .jsonFrmt = "{      \
						 \"jsonrpc\": \"2.0\", \
						 \"method\": \"get-packet-trace-profile\", \
						 \"asic-id\": \"%s\", \
							 \"params\": {  \
                             \"src-ip\": \"%s\", \
                             \"dst-ip\": \"%s\", \
                             \"protocol\": \"%s\", \
							 \"l4-src-port\": \"%s\", \
                             \"l4-dst-port\": \"%s\", \
                             \"port-list\": [%s], \
                             \"packet-limit\": %u, \
                             \"collection-interval\": %u, \
                             \"drop-packet\": %u \
						 }, \
						 \"id\": %u \
					  }",

    },
    {
     .methodId = PTAPP_JSON_GET_LIVE_PACKET_TRACE_LAG_RESOLUTION, 
     .method = "get-packet-trace-lag-resolution",
     .menuString = "Capture and Trace LAG resolution for a live packet",
     .get_input_for_json = get_live_pt_profile,
	 .jsonFrmt = "{      \
						 \"jsonrpc\": \"2.0\", \
						 \"method\": \"get-packet-trace-lag-resolution\", \
						 \"asic-id\": \"%s\", \
							 \"params\": {  \
                             \"src-ip\": \"%s\", \
                             \"dst-ip\": \"%s\", \
						     \"protocol\": \"%s\", \
							 \"l4-src-port\": \"%s\", \
                             \"l4-dst-port\": \"%s\", \
                             \"port-list\": [%s], \
                             \"packet-limit\": %u, \
                             \"collection-interval\": %u, \
                             \"drop-packet\": %u \
						 }, \
						 \"id\": %u \
					  }",
    },
    {
     .methodId = PTAPP_JSON_GET_LIVE_PACKET_TRACE_ECMP_RESOLUTION, 
     .method = "get-packet-trace-ecmp-resolution",
     .menuString = "Cature and Trace ECMP resolution for a live capture",
     .get_input_for_json = get_live_pt_profile,
	 .jsonFrmt = "{      \
						 \"jsonrpc\": \"2.0\", \
						 \"method\": \"get-packet-trace-ecmp-resolution\", \
						 \"asic-id\": \"%s\", \
							 \"params\": {  \
                             \"src-ip\": \"%s\", \
                             \"dst-ip\": \"%s\", \
						     \"protocol\": \"%s\", \
							 \"l4-src-port\": \"%s\", \
                             \"l4-dst-port\": \"%s\", \
                             \"port-list\": [%s], \
                             \"packet-limit\": %u, \
                             \"collection-interval\": %u, \
                             \"drop-packet\": %u \
						 }, \
						 \"id\": %u \
					  }",
     },
#if DROP_REASON_SUPPORTED
    {
     .methodId = PTAPP_JSON_GET_PACKET_TRACE_DROP_REASON,
     .method = "get-packet-trace-drop-reason",  
     .menuString = "Get configured drop reasons on the silicon"
     .get_input_for_json = get_pt_drop_reason,
     .jsonFrmt = "{      \
                     \"jsonrpc\": \"2.0\", \
                     \"method\": \"get-packet-trace-drop-reason\", \
                     \"asic-id\": \"%s\", \
                     \"params\": {  \
                     }, \
                     \"id\": %u \
                  }",
 
    },

    {
     .methodId = PTAPP_JSON_GET_PACKET_TRACE_DROP_COUNTER_REPORT,
     .method = "get-packet-trace-drop-counter-report",  
     .menuString = "Get drop counter report"
     .get_input_for_json = get_pt_drop_counter_report,
     .jsonFrmt =  "{      \
                      \"jsonrpc\": \"2.0\", \
                      \"method\": \"get-packet-trace-drop-counter-report\", \
                      \"asic-id\": \"%s\", \
                      \"params\": {  \
                      \"reason\" : [%s], \
                      \"port-list\" : \"%s\", \
                      }, \
                      \"id\": %u \
                   }",

    },

    {
     .methodId = PTAPP_JSON_GET_PACKET_TRACE_SUPPORTED_DROP_REASONS,
     .method = "get-packet-trace-supported-drop-reasons ",  
     .menuString = "Get supported drop reasons on the silicon"
     .get_input_for_json = get_pt_supported_drop_reasons,
     .jsonFrmt =  "{      \
                      \"jsonrpc\": \"2.0\", \
                      \"method\": \"get-packet-trace-supported-drop-reasons\", \
                      \"asic-id\": \"%s\", \
                      \"params\": {  \
                      }, \
                      \"id\": %u \
                   }",
 
    },
#endif

    {
     .methodId = PTAPP_JSON_CANCEL_PACKET_TRACE_PROFILE,
     .method = "cancel-packet-trace-profile",  
     .menuString = "Cancel Packet Trace profile request",
     .get_input_for_json = cancel_pt_profile_request,
     .jsonFrmt =  "{      \
                      \"jsonrpc\": \"2.0\", \
                      \"method\": \"cancel-packet-trace-profile\", \
                      \"asic-id\": \"%s\", \
                      \"params\": {  \
                      \"id\": %u \
                      }, \
                      \"id\": %u \
                   }",

    },
    {
     .methodId = PTAPP_JSON_CANCEL_PACKET_TRACE_LAG_RESOLUTION,
     .method = "cancel-packet-trace-lag-profile",  
     .menuString = "Cancel Packet Trace LAG resolution request",
     .get_input_for_json = cancel_pt_profile_request,
     .jsonFrmt =  "{      \
                      \"jsonrpc\": \"2.0\", \
                      \"method\": \"cancel-packet-trace-lag-resolution\", \
                      \"asic-id\": \"%s\", \
                      \"params\": {  \
                      \"id\": %u \
                      }, \
                      \"id\": %u \
                   }",

    },
    {
     .methodId = PTAPP_JSON_CANCEL_PACKET_TRACE_ECMP_RESOLUTION,
     .method = "cancel-packet-trace-ecmp-profile",  
     .menuString = "Cancel Packet Trace ECMP resolution request",
     .get_input_for_json = cancel_pt_profile_request,
     .jsonFrmt =  "{      \
                      \"jsonrpc\": \"2.0\", \
                      \"method\": \"cancel-packet-trace-ecmp-resolution\", \
                      \"asic-id\": \"%s\", \
                      \"params\": {  \
                      \"id\": %u \
                      }, \
                      \"id\": %u \
                   }",

    },
#if DROP_REASON_SUPPORTED
    {
     .methodId = PTAPP_JSON_CANCEL_PACKET_TRACE_SEND_DROP_PACKET,
     .method = "cancel-packet-trace-send-drop-packet",  
     .menuString = "Cancel Packet Trace send drop packet request"
     .get_input_for_json = cancel_pt_send_drop_pkt_req,
     .jsonFrmt =  "{      \
                      \"jsonrpc\": \"2.0\", \
                      \"method\": \"cancel-packet-trace-send-drop-packet\", \
                      \"asic-id\": \"%s\", \
                      \"params\": {  \
                      \"id\": %u \
                      }, \
                      \"id\": %u \
                   }",

    },

    {
     .methodId = PTAPP_JSON_CANCEL_PACKET_TRACE_DROP_COUNTER_REPORT,
     .method = "cancel-packet-trace-drop-counter-report",  
     .menuString = "Cancel Packet Trace drop counter report collection"
     .get_input_for_json = cancel_pt_drop_counter_report,
     .jsonFrmt =  "{      \
                      \"jsonrpc\": \"2.0\", \
                      \"method\": \"cancel-packet-trace-drop-counter-report\", \
                      \"asic-id\": \"%s\", \
                      \"params\": {  \
                      \"reason\" : [%s], \
                      \"port-list\" : \"%s\", \
                      }, \
                      \"id\": %u \
                   }",
    },
#endif

  
};

/* A global buf to prepare the JSON message */
char jsonBufGlobal[PTAPP_JSON_BUFF_MAX_SIZE ]= {0};
/* JSON id counter */  
static unsigned int jsonIdCnt = 0;

/******************************************************************
 * @brief     Returns a new JSON id.
 *
 * @param[in]   none
 *
 * @retval      A new JSON id
 * @note     
 *********************************************************************/
unsigned int get_new_json_id()
{
  jsonIdCnt++;

  if (jsonIdCnt == 0)  /* Not allowing Id '0' */
  {
    jsonIdCnt++;
  }
  return jsonIdCnt;
}

