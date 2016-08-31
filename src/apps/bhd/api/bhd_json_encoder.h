/*********************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename bhd_json_encoder.h 
  *
  * @purpose BroadView BHD JSON encoder functions 
  *
  * @component Black hole detection 
  *
  * @comments
  *
  * @create 3/7/2016
  *
  * @author 
  * @end
  *
  **********************************************************************/


#ifndef INCLUDE_BHDJSONENCODER_H
#define INCLUDE_BHDJSONENCODER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "broadview.h"
#include "json.h"

#include "bhd.h"

/* reporting options */
typedef struct _bhd_reporting_options_
{
    bool blackHolePresent;
} BHDJSON_REPORT_OPTIONS_t;


#define _BHDJSONENCODE_DEBUG
#define _BHDJSONENCODE_DEBUG_LEVEL         _BHDJSONENCODE_DEBUG_ERROR

#define _BHDJSONENCODE_DEBUG_TRACE        (0x1)
#define _BHDJSONENCODE_DEBUG_INFO         (0x01 << 1)
#define _BHDJSONENCODE_DEBUG_ERROR        (0x01 << 2)
#define _BHDJSONENCODE_DEBUG_DUMPJSON     (0x01 << 3)
#define _BHDJSONENCODE_DEBUG_ALL          (0xFF)

#ifdef _BHDJSONENCODE_DEBUG
#define _BHDJSONENCODE_LOG(level, format,args...)   do { \
            if ((level) & _BHDJSONENCODE_DEBUG_LEVEL) { \
                printf(format, ##args); \
            } \
        }while(0)
#else 
#define _BHDJSONENCODE_LOG(level, format,args...)
#endif

/* Utility Macros for parameter validation */
#define _BHDJSONENCODE_ASSERT_ERROR(condition, errcode) do { \
    if (!(condition)) { \
        _BHDJSONENCODE_LOG(_BHDJSONENCODE_DEBUG_ERROR, \
                    "BHD JSON Encoder (%s:%d) Invalid Input Parameter  \n", \
                    __func__, __LINE__); \
        return (errcode); \
    } \
} while(0)

#define _BHDJSONENCODE_ASSERT(condition) _BHDJSONENCODE_ASSERT_ERROR((condition), (BVIEW_STATUS_INVALID_PARAMETER))

#define _BHDJSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actLen, dst, len, lenptr, format, args...) \
    do { \
        int xtemp = *(lenptr); \
        (actLen) = snprintf((dst), (len), format, ##args); \
        *(lenptr) = (xtemp) + (actLen); \
        if ( (len) == (actLen)) { \
            /* Out of buffer here */ \
            _BHDJSONENCODE_LOG(_BHDJSONENCODE_DEBUG_TRACE, "BHD-JSON-Encoder : (%s:%d) Out of Json memory while encoding \n", __func__, __LINE__); \
            return BVIEW_STATUS_OUTOFMEMORY; \
        } \
        (dst) += (actLen); \
        (len) -= (actLen); \
    } while(0)

/* Prototypes */

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-black-hole-detection" REST API.
 *
 * @param[in]   asicId      ASIC for which this data is being encoded.
 * @param[in]   method      Method ID (from original request) that needs 
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
 *           bhdjson_memory_free(). Failing to do so leads to memory leaks
 *********************************************************************/
BVIEW_STATUS bhdjson_encode_get_black_hole_detection( int asicId,
                                            int method,
                                            const BHDJSON_CONFIGURE_BLACK_HOLE_DETECTION_t *pData,
                                            uint8_t **pJsonBuffer
                                            );

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-black-hole-detection" REST API.
 *
 * @param[in]   asicId      ASIC for which this data is being encoded.
 * @param[in]   method      Method ID (from original request) that needs 
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
 *           bhdjson_memory_free(). Failing to do so leads to memory leaks
 *********************************************************************/
BVIEW_STATUS bhdjson_encode_get_black_hole( int asicId,
                                            int method,
                                            const BHDJSON_CONFIGURE_BLACK_HOLE_t *pData,
                                            uint8_t **pJsonBuffer
                                            );

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-sflow-sampling-status" REST API.
 *
 * @param[in]   asicId      ASIC for which this data is being encoded.
 * @param[in]   method      Method ID (from original request) that needs 
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
 *           bhdjson_memory_free(). Failing to do so leads to memory leaks
 *********************************************************************/
BVIEW_STATUS bhdjson_encode_get_sampling_status( int asicId,
                                            int method,
                                            const BVIEW_BHD_SFLOW_SAMPLING_STATUS_t *pData,
                                            const BVIEW_TIME_t *time,
                                            uint8_t **pJsonBuffer
                                            );

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-black-hole-event-report" REST API.
 *
 * @param[in]   asicId      ASIC for which this data is being encoded.
 * @param[in]   method      Method ID (from original request) that needs 
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
 *           bhdjson_memory_free(). Failing to do so leads to memory leaks
 *********************************************************************/
BVIEW_STATUS bhdjson_encode_get_black_hole_event_report( int asicId,
                                            int method,
                                            const BHD_BLACK_HOLE_EVENT_REPORT_t *pData,
                                            const BVIEW_TIME_t *time,
                                            uint8_t **pJsonBuffer
                                            );

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_BHDJSONENCODER_H */
