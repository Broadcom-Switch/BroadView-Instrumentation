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

#ifndef INCLUDE_DRIVER_UTIL_MENU_H
#define INCLUDE_DRIVER_UTIL_MENU_H

#ifdef  __cplusplus
extern "C"
{
#endif

#include <netinet/in.h>

#define EXAMPLE_APP_MAX_TABLE_SIZE        20 
#define ETHERNET_MAC_ADDR_LEN       6
#define EXAMPLE_APP_MAX_LAG       4
#define EXAMPLE_APP_MAX_USER_INPUT_STRING_SIZE       1024
#define ETHERNET_MAC_ADDR_STR_LEN       128
#define EXAMPLE_APP_MIN_VLAN  1
#define EXAMPLE_APP_MAX_VLAN 4094
#define EXAMPLE_APP_MIN_PORT 1
#define EXAMPLE_APP_MAX_PORT 48
#define EXAMPLE_APP_MAX_MULTIPATHS 8


#define _EXAMPLE_APP_DEBUG
#define _EXAMPLE_APP_DEBUG_LEVEL        (0x0)

#define _EXAMPLE_APP_DEBUG_TRACE        (0x1)
#define _EXAMPLE_APP_DEBUG_INFO         (0x01 << 1)
#define _EXAMPLE_APP_DEBUG_ERROR        (0x01 << 2)
#define _EXAMPLE_APP_DEBUG_ALL          (0xFF)

#ifdef _EXAMPLE_APP_DEBUG
#define _EXAMPLE_APP_LOG(level, format,args...)   do { \
              if ((level) & _EXAMPLE_APP_DEBUG_LEVEL) { \
                                printf(format, ##args); \
                            } \
          }while(0)
#else
#define _EXAMPLE_APP_LOG(level, format,args...)
#endif

  typedef enum l3_table_type
  {
    L3_INTF = 0,
    L3_ROUTING,
    L3_ECMP_ROUTING,
    L3_ARP
  }EXAMPLE_L3_TABLE_TYPE_t;



#ifdef  __cplusplus
}
#endif

#endif

