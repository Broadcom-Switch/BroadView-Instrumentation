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

#ifndef INCLUDE_BVIEW_H
#define INCLUDE_BVIEW_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/socket.h>
#include <arpa/inet.h>


/** Maximum supported asics on a platform */ 
#define BVIEW_MAX_ASICS_ON_A_PLATFORM   1

/** Maximum supported features */
#define BVIEW_MAX_FEATURES              16

/** Max api cmds supported per feature */
#define BVIEW_MAX_API_CMDS_PER_FEATURE  16

/** Maximum supported sb plugins */
#define BVIEW_MAX_SUPPORTED_SB_PLUGINS   8 

/** Maximum supported features per a sb plug-in (BVIEW_MAX_FEATURES+system feature) */
#define BVIEW_SBPLUGIN_MAX_FEATURES    (BVIEW_MAX_FEATURES+1) 

    /** Indicates various status codes.
    *  This status codes are typically used as return values for function 
    *  calls. However, they can be used elsewhere as well to indicate
    *  execution status.
    */

    typedef enum _bview_status
    {
        BVIEW_STATUS_SUCCESS = 0, /**< Successful completion of request */
        BVIEW_STATUS_FAILURE, /**< Unsuccessful completion of request */
        BVIEW_STATUS_INVALID_PARAMETER,
        BVIEW_STATUS_UNSUPPORTED,
        BVIEW_STATUS_OUTOFMEMORY,
        BVIEW_STATUS_TIMEOUT,
        BVIEW_STATUS_NOTREADY,
        BVIEW_STATUS_TABLE_FULL,
        BVIEW_STATUS_INVALID_COMMAND,
        BVIEW_STATUS_INVALID_JSON,
        BVIEW_STATUS_RESOURCE_NOT_AVAILABLE,
        BVIEW_STATUS_INVALID_MEMORY,
        BVIEW_STATUS_OUTOFRANGE,
        BVIEW_STATUS_INIT_FAILED,
        BVIEW_STATUS_DUPLICATE  /** If the entry is alreadey present */ 
    } BVIEW_STATUS;

    typedef enum _asic_type
    {
      BVIEW_ASIC_TYPE_TD2   =  (1 << 0), 
      BVIEW_ASIC_TYPE_TH    =  (1 << 1),
      BVIEW_ASIC_TYPE_ALL   =  (0xFFFF) 	  
    } BVIEW_ASIC_TYPE;

 /** type define time_t to BVIEW_TIME_t */
    typedef time_t BVIEW_TIME_t;

#if (__STDC_VERSION__ >= 199901L)
  #include <stdint.h>
#else
#ifndef _STDINT_H
    typedef unsigned long long uint64_t;
    typedef unsigned char uint8_t;
#endif
#endif

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_BVIEW_H */
