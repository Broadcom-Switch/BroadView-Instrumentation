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

#ifndef INCLUDE_OPENAPPS_FEATURE_H
#define	INCLUDE_OPENAPPS_FEATURE_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include "broadview.h"

    /** List of Instrumentation features */

    typedef enum _bview_feature_id_
    {
        /** enum to represent system feature */
        BVIEW_FEATURE_SYSTEM = (0x1 << 0),
        /** enum to represent BST feature */
        BVIEW_FEATURE_BST = (0x1 << 1),
        /** enum to represent Packet Trace feature */
        BVIEW_FEATURE_PACKET_TRACE = (0x1 << 2),
        /** enum to represent Packet Trace feature */
        BVIEW_FEATURE_BHD = (0x1 << 3),
        /** enum to represent Live Packet Trace feature */
        BVIEW_FEATURE_LIVE_PT = (0x1 << 4)

    } BVIEW_FEATURE_ID;

    /** the web server invokes the handler associated with the incoming REST API  *
      * the incoming API is containing in the jsonBuffer.                         *
      * The cookie is used by the web server to hold any context associated with   *
      * the request. It is passed back the handler when some data needs to be sent back */
    typedef BVIEW_STATUS(*BVIEW_REST_API_HANDLER_t) (void *cookie,
        char *jsonBuffer,
        int bufLength);

    /** Definition of an REST API */
    typedef struct _feature_rest_api_ 
    {
        /** Api string in the Json buffer */  
        char *apiString;
        /** Handler associated with the corresponding api string */ 
        BVIEW_REST_API_HANDLER_t    handler;
    }BVIEW_REST_API_t;

#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_OPENAPPS_FEATURE_H */

