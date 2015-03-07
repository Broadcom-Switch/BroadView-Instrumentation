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

#ifndef INCLUDE_SBPLUGIN_COMMON_H
#define INCLUDE_SBPLUGIN_COMMON_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "sbplugin.h"
#include "sbplugin_system.h"
#include "openapps_log_api.h"

/* Flag to enable/disable debug */
extern int sbSdkDebugFlag;

/* Macro to print the SDK plug-in debug information */
#define SB_SDK_DEBUG_PRINT(severity,format, args...)                 \
                                if (sbSdkDebugFlag)                  \
                                {                                    \
                                  log_post(severity,format, ##args); \
                                }


#define SB_SDK_LOG(severity,format, args...)               \
                        {                                   \
                          log_post(severity,format, ##args);\
                        }

#define BVIEW_UNIT_CHECK(_asic)                                     \
                         if (SB_BRCM_RV_ERROR(SB_BRCM_API_UNIT_CHECK(_asic)))              \
                         {                                          \
                           return BVIEW_STATUS_INVALID_PARAMETER;   \
                         }
              

/* NULL Pointer Check*/
#define  BVIEW_NULLPTR_CHECK(_p)                \
                if (_p == NULL)                 \
                {                               \
                  return BVIEW_STATUS_FAILURE;  \
                }
   
/*********************************************************************
* @brief    SDK South bound plugin init
*
* @retval   BVIEW_STATUS_SUCCESS if BST feature is
*                                initialized successfully.
* @retval   BVIEW_STATUS_FAILURE if initialization is failed.
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS  sbplugin_common_system_init ();

#ifdef __cplusplus
}
#endif
#endif
