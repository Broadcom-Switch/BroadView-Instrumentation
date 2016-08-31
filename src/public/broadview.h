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

/** Maximum length of network os string*/
#define BVIEW_NETWORK_OS_LEN_MAX       32

/** Maximum length of feature name string*/
#define BVIEW_MAX_FEATURE_NAME_LEN       32

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

/** json version */
#define BVIEW_JSON_VERSION   3 


/** agent software name */
#define BVIEW_AGENT_SW_NAME   "broadview" 
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
        BVIEW_STATUS_INVALID_ID,
        BVIEW_STATUS_FEATURE_NOT_ENABLED,
        BVIEW_STATUS_NOT_CONFIGURED,
        BVIEW_STATUS_ALREADY_CONFIGURED,
        BVIEW_STATUS_DUPLICATE  /** If the entry is alreadey present */ 
    } BVIEW_STATUS;

    typedef enum _asic_type
    {
      BVIEW_ASIC_TYPE_TD2       =  (1 << 0), 
      BVIEW_ASIC_TYPE_TH        =  (1 << 1),
      BVIEW_ASIC_TYPE_QUMRAN    =  (1 << 2),
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

#if (CPU==gto)||(CPU==xlp)
    #define ENV32BIT
#elif (CPU==x86_64)||(CPU==pclinux)
    #define ENV64BIT
#else 
    #define ENV32BIT  /* Default environment */
#endif
	
#ifdef ENV32BIT
    typedef unsigned long int ptr_to_uint_t;
    #define PRI_PTR_TO_UINT_FMT "lx" 
#else
#ifdef ENV64BIT
	typedef unsigned long long int ptr_to_uint_t;
    #define PRI_PTR_TO_UINT_FMT "llx" 
#endif
#endif

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_BVIEW_H */
