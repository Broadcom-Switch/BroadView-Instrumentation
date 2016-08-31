
/*********************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename bhdapp_debug.h 
  *
  * @purpose BroadView BHD reference application 
  *
  * @component Black hole detection 
  *
  * @comments
  *
  * @create 3/17/2016
  *
  * @author 
  * @end
  *
  **********************************************************************/

#ifndef INCLUDE_BHDAPP_DEBUG_H
#define	INCLUDE_BHDAPP_DEBUG_H

#ifdef	__cplusplus
extern "C"
{
#endif

#define _BHDAPP_DEBUG
#define _BHDAPP_DEBUG_LEVEL        (0x0)

#define _BHDAPP_DEBUG_TRACE        (0x1)
#define _BHDAPP_DEBUG_INFO         (0x01 << 1)
#define _BHDAPP_DEBUG_ERROR        (0x01 << 2)
#define _BHDAPP_DEBUG_DUMPJSON     (0x01 << 3)
#define _BHDAPP_DEBUG_ALL          (0xFF)

#ifdef _BHDAPP_DEBUG
#define _BHDAPP_LOG(level, format,args...)   do { \
            if ((level) & _BHDAPP_DEBUG_LEVEL) { \
                printf(format, ##args); \
            } \
        }while(0)
#else 
#define _BHDAPP_LOG(level, format,args...)
#endif

/* Utility Macros for parameter validation */
#define _BHDAPP_ASSERT_ERROR(condition, errcode) do { \
    if (!(condition)) { \
        _BHDAPP_LOG(_BHDAPP_DEBUG_ERROR, \
                    "BHDAPP (%s:%d) Invalid Input Parameter  \n", \
                    __func__, __LINE__); \
        return (errcode); \
    } \
} while(0)

#define _BHDAPP_ASSERT(condition) _BHDAPP_ASSERT_ERROR((condition), (-1))

/* Utility Logging Macros */

#define _BHDAPP_ASSERT_NET_ERROR(condition, errString) do { \
    if (!(condition)) { \
        _BHDAPP_LOG(_BHDAPP_DEBUG_ERROR, \
                    "BHDAPP (%s:%d) %s. [ERRNO : %s ] \n", \
                    __func__, __LINE__, (errString), strerror(errno)); \
        return (-1); \
    } \
} while(0)

#define _BHDAPP_PASSIVE_ASSERT_NET_ERROR(condition, errString) do { \
    if (!(condition)) { \
        _BHDAPP_LOG(_BHDAPP_DEBUG_ERROR, \
                    "BHDAPP (%s:%d) %s. [ERRNO : %s ] \n", \
                    __func__, __LINE__, (errString), strerror(errno)); \
    } \
} while(0)

#define _BHDAPP_ASSERT_NET_SOCKET_ERROR(condition, errString,listenFd) do { \
    if (!(condition)) { \
        _BHDAPP_LOG(_BHDAPP_DEBUG_ERROR, \
                    "BHDAPP (%s:%d) %s. [ERRNO : %s ] \n", \
                    __func__, __LINE__, (errString), strerror(errno)); \
        close(listenFd); \
        return (-1); \
    } \
} while(0)

#define _BHDAPP_ASSERT_CONFIG_FILE_ERROR(condition) do { \
    if (!(condition)) { \
        _BHDAPP_LOG(_BHDAPP_DEBUG_ERROR, \
                    "BHDAPP (%s:%d) Unrecognized Config File format, may be corrupted. Errno : %s  \n", \
                    __func__, __LINE__, strerror(errno)); \
        fclose(configFile); \
        return (-1); \
    } \
} while(0)

#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_BHDAPP_DEBUG_H */

