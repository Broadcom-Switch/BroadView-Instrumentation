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

#ifndef INCLUDE_SYSTEM_UTILS_API_H
#define	INCLUDE_SYSTEM_UTILS_API_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include "broadview.h"


typedef BVIEW_STATUS(*BVIEW_SYSTEM_UTILS_CANCEL_API_HANDLER_t) (unsigned int unit, unsigned int id);

/*********************************************************************
 * @brief : Register a feature's cancel api handler for the system 
 *                   cancel command type
 *
 * @param[in] int : feature id
 * @param[in] BVIEW_SYSTEM_UTILS_CANCEL_API_HANDLER_t : Feature's Handler 
 *                                        function for cancel command
 *
 * @retval  : BVIEW_STATUS_SUCCESS : when the function is successfully registered
 * @retval  : BVIEW_STATUS_ALREADY_CONFIGURED: when the hanlder is already configured 
 *                    for the feature
 * @retval  : BVIEW_STATUS_FAILURE : when failed to register the handler.
 *
 * @note :
 *
 *********************************************************************/
BVIEW_STATUS system_utils_cancel_api_request_register(unsigned int featId,
                                                BVIEW_SYSTEM_UTILS_CANCEL_API_HANDLER_t handler);


#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_SYSTEM_UTILS_API_H */

