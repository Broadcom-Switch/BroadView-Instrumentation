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

#ifndef INCLUDE_SYSTEM_UTILS_JSON_ENCODER_H
#define INCLUDE_SYSTEM_UTILS_JSON_ENCODER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "json.h"

/* Prototypes */

BVIEW_STATUS reg_hb_json_encode_get_feature(int method,
                                            const REG_HB_JSON_CONFIGURE_FEATURE_t *pData,
                                            uint8_t **pJsonBuffer
                                            );

BVIEW_STATUS reg_hb_json_encode_get_switch_properties (int method,
                                            BVIEW_SWITCH_PROPERTIES_t *pData,
                                            uint8_t **pJsonBuffer
                                            );

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_SYSTEM_UTILS_JSON_ENCODER_H */
