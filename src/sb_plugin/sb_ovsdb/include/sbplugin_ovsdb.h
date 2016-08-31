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

#ifndef INCLUDE_SBPLUGIN_OVSDB_H
#define INCLUDE_SBPLUGIN_OVSDB_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include "sbplugin.h"
#include "sbplugin_bst.h"
#include "sbplugin_system.h"
#include "sbplugin_system_map.h"
#include "sb_redirector_api.h"
#include "openapps_log_api.h"


/* NULL Pointer Check*/
#define  SB_OVSDB_NULLPTR_CHECK(_p,_rv)         \
                if ((_p) == NULL)               \
                {                               \
                  return (_rv);                 \
                }

/* Flag to enable/disable debug */
extern int sbOvsdbDebugFlag;

#define SB_OVSDB_RV_ERROR(_rv)     ((_rv) != BVIEW_STATUS_SUCCESS)

/* Macro to print the OVSDB plug-in debug information */
#define SB_OVSDB_DEBUG_PRINT(format, args...)                                   \
                       if (sbOvsdbDebugFlag)                                    \
                       {                                                        \
                         printf ("(%s:%d) "format, __FILE__, __LINE__,##args);  \
                       }


#define SB_OVSDB_LOG(severity,format, args...)               \
                        {                                   \
                          log_post(severity,format, ##args);\
                        }

#define SB_OVSDB_API_UNIT_CHECK(_unit)                                           \
                        (sbplugin_ovsdb_valid_unit_check(_unit))

#define SB_OVSDB_VALID_UNIT_CHECK(_asic)                                         \
                         if (SB_OVSDB_RV_ERROR(SB_OVSDB_API_UNIT_CHECK(_asic)))  \
                         {                                                       \
                           return BVIEW_STATUS_INVALID_PARAMETER;                \
                         }
              
#ifdef	__cplusplus
}
#endif

#endif /* INCLUDE_SBPLUGIN_OVSDB_H */

