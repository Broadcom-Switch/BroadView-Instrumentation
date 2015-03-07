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

#ifndef INCLUDE_REST_DEBUG_H
#define	INCLUDE_REST_DEBUG_H

#ifdef	__cplusplus
extern "C"
{
#endif
#include  "openapps_log_api.h"
#define _REST_DEBUG
#define _REST_DEBUG_LEVEL        _REST_DEBUG_ALL

#define _REST_DEBUG_TRACE        (0x1)
#define _REST_DEBUG_INFO         (0x01 << 1)
#define _REST_DEBUG_ERROR        (0x01 << 2)
#define _REST_DEBUG_DUMPJSON     (0x01 << 3)
#define _REST_DEBUG_ALL          (0xFF)
extern void log_post(BVIEW_SEVERITY severity, char * format, ...);
#ifdef _REST_DEBUG
#define _REST_LOG(level, format,args...)   do { \
            if ((level) & _REST_DEBUG_LEVEL) { \
                log_post(level,format, ##args); \
            } \
        }while(0)
#else 
#define _REST_LOG(level, format,args...)
#endif

/* Utility Macros for parameter validation */
#define _REST_ASSERT_ERROR(condition, errcode) do { \
    if (!(condition)) { \
        _REST_LOG(_REST_DEBUG_ERROR, \
                    "REST (%s:%d) Invalid Input Parameter  \n", \
                    __func__, __LINE__); \
        return (errcode); \
    } \
} while(0)

#define _REST_ASSERT(condition) _REST_ASSERT_ERROR((condition), (BVIEW_STATUS_INVALID_PARAMETER))

/* Utility Logging Macros */

#define _REST_ASSERT_NET_ERROR(condition, errString) do { \
    if (!(condition)) { \
        _REST_LOG(_REST_DEBUG_ERROR, \
                    "REST (%s:%d) %s. [ERRNO : %s ] \n", \
                    __func__, __LINE__, (errString), strerror(errno)); \
        return (BVIEW_STATUS_FAILURE); \
    } \
} while(0)

#define _REST_PASSIVE_ASSERT_NET_ERROR(condition, errString) do { \
    if (!(condition)) { \
        _REST_LOG(_REST_DEBUG_ERROR, \
                    "REST (%s:%d) %s. [ERRNO : %s ] \n", \
                    __func__, __LINE__, (errString), strerror(errno)); \
    } \
} while(0)

#define _REST_ASSERT_NET_SOCKET_ERROR(condition, errString,fd) do { \
    if (!(condition)) { \
        _REST_LOG(_REST_DEBUG_ERROR, \
                    "REST (%s:%d) %s. [ERRNO : %s ] \n", \
                    __func__, __LINE__, (errString), strerror(errno)); \
        close(fd); \
        return (BVIEW_STATUS_FAILURE); \
    } \
} while(0)


#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_REST_DEBUG_H */

