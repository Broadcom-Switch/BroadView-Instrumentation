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

#ifndef INCLUDE_PACKET_TRACE_DEBUG_H
#define INCLUDE_PACKET_TRACE_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include "modulemgr.h"




#define _PT_DEBUG
#define _PT_DEBUG_LEVEL        0x0 

#define _PT_DEBUG_TRACE        (0x1)
#define _PT_DEBUG_INFO         (0x01 << 1)
#define _PT_DEBUG_ERROR        (0x01 << 2)
#define _PT_DEBUG_ALL          (0xFF)

#ifdef _PT_DEBUG
#define _PT_LOG(level, format,args...)   do { \
              if ((level) & _PT_DEBUG_LEVEL) { \
                                printf(format, ##args); \
                            } \
          }while(0)
#else
#define _PT_LOG(level, format,args...)
#endif



#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_PACKET_TRACE_DEBUG_H */

