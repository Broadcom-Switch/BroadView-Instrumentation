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

#ifndef INCLUDE_BSTAPP_SERVER_DEBUG_H
#define	INCLUDE_BSTAPP_SERVER_DEBUG_H

#ifdef	__cplusplus
extern "C"
{
#endif
#define _BSTAPP_DEBUG

#define _BSTAPP_DEBUG_LEVEL         0 

#define _BSTAPP_DEBUG_TRACE        (0x1)
#define _BSTAPP_DEBUG_INFO         (0x01 << 1)
#define _BSTAPP_DEBUG_ERROR        (0x01 << 2)
#define _BSTAPP_DEBUG_DUMPJSON     (0x01 << 3)
#define _BSTAPP_DEBUG_ALL          (0xFF)
#ifdef _BSTAPP_DEBUG
#define _BSTAPP_LOG(level, format,args...)   do { \
            if ((level) & _BSTAPP_DEBUG_LEVEL) { \
                printf(format, ##args); \
            } \
        }while(0)
#else 
#define _BSTAPP_LOG(level, format,args...)
#endif

/* Utility Macros for parameter validation */
#define _BSTAPP_ASSERT_ERROR(condition, errcode) do { \
    if (!(condition)) { \
        _BSTAPP_LOG(_BSTAPP_DEBUG_ERROR, \
                    "EXAMPLE (%s:%d) Invalid Input Parameter  \n", \
                    __func__, __LINE__); \
        return (errcode); \
    } \
} while(0)

#define _BSTAPP_ASSERT(condition) _BSTAPP_ASSERT_ERROR((condition), (-1))

/* Utility Logging Macros */

#define _BSTAPP_ASSERT_NET_ERROR(condition, errString) do { \
    if (!(condition)) { \
        _BSTAPP_LOG(_BSTAPP_DEBUG_ERROR, \
                    "EXAMPLE (%s:%d) %s. [ERRNO : %s ] \n", \
                    __func__, __LINE__, (errString), strerror(errno)); \
        return (-1); \
    } \
} while(0)

#define _BSTAPP_PASSIVE_ASSERT_NET_ERROR(condition, errString) do { \
    if (!(condition)) { \
        _BSTAPP_LOG(_BSTAPP_DEBUG_ERROR, \
                    "EXAMPLE (%s:%d) %s. [ERRNO : %s ] \n", \
                    __func__, __LINE__, (errString), strerror(errno)); \
    } \
} while(0)

#define _BSTAPP_ASSERT_NET_SOCKET_ERROR(condition, errString,fd) do { \
    if (!(condition)) { \
        _BSTAPP_LOG(_BSTAPP_DEBUG_ERROR, \
                    "EXAMPLE (%s:%d) %s. [ERRNO : %s ] \n", \
                    __func__, __LINE__, (errString), strerror(errno)); \
        close(fd); \
        return (BVIEW_STATUS_FAILURE); \
    } \
} while(0)

#define _BSTAPP_ASSERT_CONFIG_FILE_ERROR(condition) do { \
    if (!(condition)) { \
        _BSTAPP_LOG(_BSTAPP_DEBUG_ERROR, \
                    "BSTAPP (%s:%d) Unrecognized Config File format, may be corrupted. Errno : %s  \n", \
                    __func__, __LINE__, strerror(errno)); \
        fclose(configFile); \
        return (-1); \
    } \
} while(0)

#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_BSTAPP_DEBUG_H */

