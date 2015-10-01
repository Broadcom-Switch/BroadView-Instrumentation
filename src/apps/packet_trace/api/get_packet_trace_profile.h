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

/* Structure to pass API parameters to the PT APP */
typedef struct _ptjson_get_pt_profile_
{
    char packet[PT_JSON_MAX_PKT_LEN];
    BVIEW_PORT_MASK_t pbmp;
    int collection_interval;
    int drop_packet;
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

