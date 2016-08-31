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

#ifndef INCLUDE_PTAPP_JSON_H
#define INCLUDE_PTAPP_JSON_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include "ptapp.h"


/* Rest mesages for JSON methods */ 
extern PTAPP_REST_MSG_t ptRestMessages[]; 

/* JSON format for different methods */ 
extern PTAPP_JSON_METHOD_INFO_t ptJsonMethodDetails[PTAPP_JSON_METHOD_LAST];

/* A global buf to prepare the JSON message */
extern char jsonBufGlobal[PTAPP_JSON_BUFF_MAX_SIZE];

/******************************************************************
 * @brief     Returns a new JSON id.
 *
 * @param[in]   none
 *
 * @retval      A new JSON id
 * @note     
 *********************************************************************/
extern unsigned int get_new_json_id();

#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_PTAPP_JSON_H */

