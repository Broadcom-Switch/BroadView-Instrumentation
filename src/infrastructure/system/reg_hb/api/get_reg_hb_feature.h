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

#ifndef INCLUDE_GET_REG_HB_FEATURE_H 
#define	INCLUDE_GET_REG_HB_FEATURE_H  

#ifdef	__cplusplus  
extern "C"
{
#endif  


/* Include Header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "broadview.h"
#include "json.h"

#include "cJSON.h"

/* Structure to pass API parameters to the REG_HB APP */
typedef struct _reg_hb_json_get_feature_
{
} REG_HB_JSON_GET_FEATURE_t;


/* Function Prototypes */
BVIEW_STATUS reg_hb_json_get_feature(void *cookie, char *jsonBuffer, int bufLength);
BVIEW_STATUS reg_hb_json_get_feature_impl(void *cookie, int id, REG_HB_JSON_GET_FEATURE_t *pCommand);


#ifdef	__cplusplus  
}
#endif  

#endif /* INCLUDE_GET_REG_HB_FEATURE_H */ 

