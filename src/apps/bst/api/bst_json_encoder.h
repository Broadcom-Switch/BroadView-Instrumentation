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

#ifndef INCLUDE_BSTJSONENCODER_H
#define INCLUDE_BSTJSONENCODER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "broadview.h"
#include "json.h"

#include "bst.h"

/* reporting options */
typedef struct _bst_reporting_options_
{
    bool includeIngressPortPriorityGroup;
    bool includeIngressPortServicePool;
    bool includeIngressServicePool;
    bool includeEgressPortServicePool;
    bool includeEgressServicePool;
    bool includeEgressUcQueue;
    bool includeEgressUcQueueGroup;
    bool includeEgressMcQueue;
    bool includeEgressCpuQueue;
    bool includeEgressRqeQueue;
    bool includeDevice;
    bool statUnitsInCells;
    bool reportTrigger;
    bool reportThreshold;
    bool sendSnapShotOnTrigger;
    BVIEW_BST_TRIGGER_INFO_t triggerInfo;
    bool sendIncrementalReport;
    bool statsInPercentage;
    BVIEW_SYSTEM_ASIC_MAX_BUF_SNAPSHOT_DATA_t *bst_max_buffers_ptr;
} BSTJSON_REPORT_OPTIONS_t;


/* structure to map the realms and indices */

typedef struct _bst_realm_index_ {
  char *realm;
  char *index1;
  char *index2;
}BSTJSON_REALM_INDEX_t;

#define _JSONENCODE_DEBUG
#define _JSONENCODE_DEBUG_LEVEL         _JSONENCODE_DEBUG_ERROR

#define _JSONENCODE_DEBUG_TRACE        (0x1)
#define _JSONENCODE_DEBUG_INFO         (0x01 << 1)
#define _JSONENCODE_DEBUG_ERROR        (0x01 << 2)
#define _JSONENCODE_DEBUG_DUMPJSON     (0x01 << 3)
#define _JSONENCODE_DEBUG_ALL          (0xFF)

#ifdef _JSONENCODE_DEBUG
#define _JSONENCODE_LOG(level, format,args...)   do { \
            if ((level) & _JSONENCODE_DEBUG_LEVEL) { \
                printf(format, ##args); \
            } \
        }while(0)
#else 
#define _JSONENCODE_LOG(level, format,args...)
#endif

/* Utility Macros for parameter validation */
#define _JSONENCODE_ASSERT_ERROR(condition, errcode) do { \
    if (!(condition)) { \
        _JSONENCODE_LOG(_JSONENCODE_DEBUG_ERROR, \
                    "BST JSON Encoder (%s:%d) Invalid Input Parameter  \n", \
                    __func__, __LINE__); \
        return (errcode); \
    } \
} while(0)

#define _JSONENCODE_ASSERT(condition) _JSONENCODE_ASSERT_ERROR((condition), (BVIEW_STATUS_INVALID_PARAMETER))

#define _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actLen, dst, len, lenptr, format, args...) \
    do { \
        int xtemp = *(lenptr); \
        (actLen) = snprintf((dst), (len), format, ##args); \
        *(lenptr) = (xtemp) + (actLen); \
        if ( (len) == (actLen)) { \
            /* Out of buffer here */ \
            _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (%s:%d) Out of Json memory while encoding \n", __func__, __LINE__); \
            return BVIEW_STATUS_OUTOFMEMORY; \
        } \
        (dst) += (actLen); \
        (len) -= (actLen); \
    } while(0)

/* Prototypes */

BVIEW_STATUS bstjson_encode_get_bst_feature(int asicId,
                                            int method,
                                            const BSTJSON_CONFIGURE_BST_FEATURE_t *pData,
                                            uint8_t **pJsonBuffer
                                            );


BVIEW_STATUS bstjson_encode_get_bst_tracking(int asicId,
                                             int method,
                                             const BSTJSON_CONFIGURE_BST_TRACKING_t *pData,
                                             uint8_t **pJsonBuffer
                                             );

BVIEW_STATUS bstjson_encode_get_bst_report(int asicId,
                                           int method,
                                           const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *previous,
                                           const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *current,
                                           const BSTJSON_REPORT_OPTIONS_t *options,
                                           const BVIEW_ASIC_CAPABILITIES_t *asic,
                                           const BVIEW_TIME_t *reportTime,
                                           uint8_t **pJsonBuffer
                                           );

BVIEW_STATUS _jsonencode_report_ingress(char *buffer,
                                        int asicId,
                                        const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *previous,
                                        const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *current,
                                        const BSTJSON_REPORT_OPTIONS_t *options,
                                        const BVIEW_ASIC_CAPABILITIES_t *asic,
                                        int bufLen,
                                        int *length
                                        );

BVIEW_STATUS _jsonencode_report_egress(char *buffer,
                                       int asicId,
                                       const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *previous,
                                       const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *current,
                                       const BSTJSON_REPORT_OPTIONS_t *options,
                                       const BVIEW_ASIC_CAPABILITIES_t *asic,
                                       int bufLen,
                                       int *length
                                       );

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-bst-congestion-drop-counters" REST API.
 *
 * @param[in]   asicId      ASIC for which this data is being encoded.
 * @param[in]   pData       Data structure holding the required parameters.
 * @param[in]   asic        pointer to asic capabilities.
 * @param[out]  pJsonBuffer Filled-in JSON buffer
 *                           
 * @retval   BVIEW_STATUS_SUCCESS  Data is encoded into JSON successfully
 * @retval   BVIEW_STATUS_RESOURCE_NOT_AVAILABLE  Internal Error
 * @retval   BVIEW_STATUS_INVALID_PARAMETER  Invalid input parameter
 * @retval   BVIEW_STATUS_OUTOFMEMORY  No available memory to create JSON buffer
 *
 * @note     The returned json-encoded-buffer should be freed using the  
 *           bstjson_memory_free(). Failing to do so leads to memory leaks
 *********************************************************************/

BVIEW_STATUS bstjson_encode_get_cgsn_drop_ctrs ( int asicId,
                                            const void *pData,
                                            const BVIEW_ASIC_CAPABILITIES_t *asic,
                                            uint8_t **pJsonBuffer
                                            );


/******************************************************************* 
   Utility function to convert the data based on config 
********************************************************************/
BVIEW_STATUS bst_json_convert_data(const BSTJSON_REPORT_OPTIONS_t *options,
                                          const BVIEW_ASIC_CAPABILITIES_t *asic,
                                          uint64_t *value, uint64_t defVal);

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_BSTJSONENCODER_H */
