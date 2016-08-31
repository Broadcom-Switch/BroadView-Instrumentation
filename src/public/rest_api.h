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

#ifndef INCLUDE_REST_API_H
#define	INCLUDE_REST_API_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include "broadview.h"


/* Initialize REST component */
BVIEW_STATUS rest_init(void);

/* API to send the response buffer back to client. 
 * This function adds HTTP header and sends it to 
 * client 
 */
BVIEW_STATUS rest_response_send(void *cookie, char *pBuf, int size);

/* API to send the response buffer back to client. 
 * This function adds HTTP header along with JSON error code and 
 * sends it to client 
 */

BVIEW_STATUS rest_response_send_error(void *cookie, BVIEW_STATUS rv, int id);

/* API to send the response buffer back to client. 
 * This function adds HTTP header and sends it to 
 * client. No JSON buffer is sent with this API.
 */

BVIEW_STATUS rest_response_send_ok (void *cookie);


#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_REST_API_H */

