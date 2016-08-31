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

#ifndef INCLUDE_CONFIGURE_REG_HB_FEATURE_H 
#define	INCLUDE_CONFIGURE_REG_HB_FEATURE_H  

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

typedef enum _reg_hb_config_param_mask_pos_
{
  REG_HB_CONFIG_PARAMS_ENABLE = 0,
  REG_HB_CONFIG_PARAMS_INTRVL
}REG_HB_CONFIG_PARAM_MASK_t;

#define BVIEW_REG_HB_TIME_MIN 1
#define BVIEW_REG_HB_TIME_MAX 600

/* Structure to pass API parameters to the REG_HB APP */
typedef struct _reg_hb_json_configure_reg_hb_feature_
{
    int enable;
    int interval;
    int configMask;
} REG_HB_JSON_CONFIGURE_FEATURE_t;


/* Function Prototypes */
BVIEW_STATUS reg_hb_json_configure_feature(void *cookie, char *jsonBuffer, int bufLength);
BVIEW_STATUS reg_hb_json_configure_feature_impl(void *cookie, int id, REG_HB_JSON_CONFIGURE_FEATURE_t *pCommand);


#ifdef	__cplusplus  
}
#endif  

#endif /* INCLUDE_CONFIGURE_REG_HB_FEATURE_H */ 

