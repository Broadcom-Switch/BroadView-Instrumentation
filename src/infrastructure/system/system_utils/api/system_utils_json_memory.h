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

#ifndef INCLUDE_SYSTEM_UTILS_JSON_MEMORY_H
#define	INCLUDE_SYSTEM_UTILS_JSON_MEMORY_H


#define _SYSTEM_UTILS_JSONENCODE_DEBUG
#define _SYSTEM_UTILS_JSONENCODE_DEBUG_LEVEL         _SYSTEM_UTILS_JSONENCODE_DEBUG_ERROR

#define _SYSTEM_UTILS_JSONENCODE_DEBUG_TRACE        (0x1)
#define _SYSTEM_UTILS_JSONENCODE_DEBUG_INFO         (0x01 << 1)
#define _SYSTEM_UTILS_JSONENCODE_DEBUG_ERROR        (0x01 << 2)
#define _SYSTEM_UTILS_JSONENCODE_DEBUG_DUMPJSON     (0x01 << 3)
#define _SYSTEM_UTILS_JSONENCODE_DEBUG_ALL          (0xFF)

#ifdef _SYSTEM_UTILS_JSONENCODE_DEBUG
#define _SYSTEM_UTILS_JSONENCODE_LOG(level, format,args...)   do { \
            if ((level) & _SYSTEM_UTILS_JSONENCODE_DEBUG_LEVEL) { \
                printf(format, ##args); \
            } \
        }while(0)
#else 
#define _SYSTEM_UTILS_JSONENCODE_LOG(level, format,args...)
#endif

/* Utility Macros for parameter validation */
#define _SYSTEM_UTILS_JSONENCODE_ASSERT_ERROR(condition, errcode) do { \
    if (!(condition)) { \
        _SYSTEM_UTILS_JSONENCODE_LOG(_SYSTEM_UTILS_JSONENCODE_DEBUG_ERROR, \
                    "SYSTEM_UTILS JSON Encoder (%s:%d) Invalid Input Parameter  \n", \
                    __func__, __LINE__); \
        return (errcode); \
    } \
} while(0)

#define _SYSTEM_UTILS_JSONENCODE_ASSERT(condition) _SYSTEM_UTILS_JSONENCODE_ASSERT_ERROR((condition), (BVIEW_STATUS_INVALID_PARAMETER))

#define _SYSTEM_UTILS_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actLen, dst, len, lenptr, format, args...) \
    do { \
        int xtemp = *(lenptr); \
        (actLen) = snprintf((dst), (len), format, ##args); \
        *(lenptr) = (xtemp) + (actLen); \
        if ( (len) == (actLen)) { \
            /* Out of buffer here */ \
            _SYSTEM_UTILS_JSONENCODE_LOG(_SYSTEM_UTILS_JSONENCODE_DEBUG_TRACE, "SYSTEM_UTILS-JSON-Encoder : (%s:%d) Out of Json memory while encoding \n", __func__, __LINE__); \
            return BVIEW_STATUS_OUTOFMEMORY; \
        } \
        (dst) += (actLen); \
        (len) -= (actLen); \
    } while(0)


#ifdef	__cplusplus
extern "C"
{
#endif

/* A small implementation of a memory pool, that offers buffers in two sizes */

typedef enum _system_utils_json_memory_size_
{
    SYSTEM_UTILS_JSON_MEMSIZE_RESPONSE = 1024,
} SYSTEM_UTILS_JSON_MEMORY_SIZE;


BVIEW_STATUS system_utils_json_memory_init(void);
BVIEW_STATUS system_utils_json_memory_allocate(SYSTEM_UTILS_JSON_MEMORY_SIZE memSize, uint8_t **buffer);
BVIEW_STATUS system_utils_json_memory_free(uint8_t *buffer);
void system_utils_json_memory_dump(void);

#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_SYSTEM_UTILS_JSON_MEMORY_H */

