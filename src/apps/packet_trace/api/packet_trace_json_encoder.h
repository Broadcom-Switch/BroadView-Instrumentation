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

#ifndef INCLUDE_PACKET_TRACEJSONENCODER_H
#define INCLUDE_PACKET_TRACEJSONENCODER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "broadview.h"
#include "json.h"

#define PT_MAX_B64_SIZE  ((4 * ((BVIEW_PT_MAX_PACKET_SIZE + 2)/3)) + 16)


#define _PT_JSONENCODE_DEBUG
#define _PT_JSONENCODE_DEBUG_LEVEL         _PT_JSONENCODE_DEBUG_ERROR

#define _PT_JSONENCODE_DEBUG_TRACE        (0x1)
#define _PT_JSONENCODE_DEBUG_INFO         (0x01 << 1)
#define _PT_JSONENCODE_DEBUG_ERROR        (0x01 << 2)
#define _PT_JSONENCODE_DEBUG_DUMPJSON     (0x01 << 3)
#define _PT_JSONENCODE_DEBUG_ALL          (0xFF)

#ifdef _PT_JSONENCODE_DEBUG
#define _PT_JSONENCODE_LOG(level, format,args...)   do { \
            if ((level) & _PT_JSONENCODE_DEBUG_LEVEL) { \
                printf(format, ##args); \
            } \
        }while(0)
#else 
#define _PT_JSONENCODE_LOG(level, format,args...)
#endif

/* Utility Macros for parameter validation */
#define _PT_JSONENCODE_ASSERT_ERROR(condition, errcode) do { \
    if (!(condition)) { \
        _PT_JSONENCODE_LOG(_PT_JSONENCODE_DEBUG_ERROR, \
                    "PT JSON Encoder (%s:%d) Invalid Input Parameter  \n", \
                    __func__, __LINE__); \
        return (errcode); \
    } \
} while(0)

#define _PT_JSONENCODE_ASSERT(condition) _PT_JSONENCODE_ASSERT_ERROR((condition), (BVIEW_STATUS_INVALID_PARAMETER))

#define _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actLen, dst, len, lenptr, format, args...) \
    do { \
        int xtemp = *(lenptr); \
        (actLen) = snprintf((dst), (len), format, ##args); \
        *(lenptr) = (xtemp) + (actLen); \
        if ( (len) == (actLen)) { \
            /* Out of buffer here */ \
            _PT_JSONENCODE_LOG(_PT_JSONENCODE_DEBUG_TRACE, "PT-JSON-Encoder : (%s:%d) Out of Json memory while encoding \n", __func__, __LINE__); \
            return BVIEW_STATUS_OUTOFMEMORY; \
        } \
        (dst) += (actLen); \
        (len) -= (actLen); \
    } while(0)


/* Prototypes */

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the
 *         "get-pt-feature" REST API.
 *
 * @param[in]   asicId      ASIC for which this data is being encoded.
 * @param[in]   id          Method ID (from original request) that needs
 *                          to be encoded in JSON.
 * @param[in]   pData       Data structure holding the required parameters.
 * @param[out]  pJsonBuffer Filled-in JSON buffer
 *
 * @retval   BVIEW_STATUS_SUCCESS  Data is encoded into JSON successfully
 * @retval   BVIEW_STATUS_RESOURCE_NOT_AVAILABLE  Internal Error
 * @retval   BVIEW_STATUS_INVALID_PARAMETER  Invalid input parameter
 * @retval   BVIEW_STATUS_OUTOFMEMORY  No available memory to create JSON buffer
 *
 * @note     The returned json-encoded-buffer should be freed using the
 *           ptjson_memory_free(). Failing to do so leads to memory leaks
 *********************************************************************/
BVIEW_STATUS ptjson_encode_get_pt_feature( int asicId,
                                           int id,
                                           const PTJSON_CONFIGURE_PT_FEATURE_t *pData,
                                           uint8_t **pJsonBuffer);


/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the
 *         "get-packet-trace-profile" REST API.
 *
 * @param[in]   asicId      ASIC for which this data is being encoded.
 * @param[in]   pData       Data structure holding the required parameters.
 * @param[in]   options     options required to encode the profile reports.
 * @param[out]  pJsonBuffer Filled-in JSON buffer
 *
 * @retval   BVIEW_STATUS_SUCCESS  Data is encoded into JSON successfully
 * @retval   BVIEW_STATUS_RESOURCE_NOT_AVAILABLE  Internal Error
 * @retval   BVIEW_STATUS_INVALID_PARAMETER  Invalid input parameter
 * @retval   BVIEW_STATUS_OUTOFMEMORY  No available memory to create JSON buffer
 *
 * @note     The returned json-encoded-buffer should be freed using the
 *           ptjson_memory_free(). Failing to do so leads to memory leaks
 *********************************************************************/

BVIEW_STATUS ptjson_encode_get_pt_profile ( int asicId,
                                            const BVIEW_PT_PROFILE_RECORD_t *pData,
                                            const PTJSON_REPORT_OPTIONS_t *options,
                                            uint8_t **pJsonBuffer);


#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_PACKET_TRACE_JSONENCODER_H */
