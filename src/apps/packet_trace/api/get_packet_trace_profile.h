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

#ifndef INCLUDE_GET_PACKET_TRACE_PROFILE_H 
#define	INCLUDE_GET_PACKET_TRACE_PROFILE_H  

#ifdef	__cplusplus  
extern "C"
{
#endif  


/* Include Header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "broadview.h"
#include "packet_trace.h"
#include "json.h"

#include "cJSON.h"

#define PT_JSON_MAX_PKT_LEN  (4 * ((BVIEW_PT_MAX_PACKET_SIZE + 40 + 2) / 3))

typedef enum _pt_profile_input_s_
{
  PT_PKT = 1,
  PT_5_TUPLE
}PT_PROFILE_INPUT_t;

/* captured packet input params place holder */
typedef struct _pt_packet_params_s_
{
    char packet[PT_JSON_MAX_PKT_LEN];
    unsigned int packet_len;
}PT_PACKET_PARAMS_t;

/* Structure to pass API parameters to the PT APP */
typedef struct _ptjson_get_pt_profile_
{ 
    PT_PROFILE_INPUT_t req_method;
    BVIEW_PORT_MASK_t pbmp;
    int collection_interval;
    int drop_packet;
    int pkt_limit;
    union
    {
      PT_PACKET_PARAMS_t pkt;
      PT_5_TUPLE_PARAMS_t tuple;
    }req_prfl;

} PTJSON_GET_PT_PROFILE_t;


/* Function Prototypes */
BVIEW_STATUS ptjson_get_pt_profile(void *cookie, char *jsonBuffer, int bufLength);
BVIEW_STATUS ptjson_get_pt_profile_impl(void *cookie, int asicId, int id, PTJSON_GET_PT_PROFILE_t *pCommand);

BVIEW_STATUS ptjson_get_pt_lag_resolution (void *cookie, char *jsonBuffer, int bufLength);
BVIEW_STATUS ptjson_get_pt_lag_resolution_impl(void *cookie, int asicId, int id, PTJSON_GET_PT_PROFILE_t *pCommand);

BVIEW_STATUS ptjson_get_pt_ecmp_resolution (void *cookie, char *jsonBuffer, int bufLength);
BVIEW_STATUS ptjson_get_pt_ecmp_resolution_impl(void *cookie, int asicId, int id, PTJSON_GET_PT_PROFILE_t *pCommand);
#ifdef	__cplusplus  
}
#endif  

#endif /* INCLUDE_GET_PT_PROFILE_H */ 

