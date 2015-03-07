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

#ifndef INCLUDE_JSON_H
#define INCLUDE_JSON_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "sbplugin_redirect_system.h"

#define JSON_MAX_NODE_LENGTH    32

#define _jsonlog(format,args...)              printf(format, ##args)  

#define JSON_VALIDATE_POINTER(x,y,z)  do { \
    if ((x) == NULL) { \
    _jsonlog("Invalid (NULL) value for parameter %s ", (y) ); \
    return (z); \
    } \
} while(0)

#define JSON_VALIDATE_JSON_POINTER(x,y,z) do { \
    if ((x) == NULL) { \
    _jsonlog("Error parsing JSON %s ", (y) ); \
    return (z); \
    } \
}while(0) 

#define JSON_VALIDATE_POINTER_AND_CLEANUP(x,y,z)  do { \
    if ((x) == NULL) { \
    _jsonlog("Invalid (NULL) value for parameter %s ", (y) ); \
    if (root != NULL) { \
    cJSON_Delete(root); \
    } \
    return (z); \
    } \
} while(0)

#define JSON_VALIDATE_JSON_POINTER_AND_CLEANUP(x,y,z)  do { \
    if ((x) == NULL) { \
    _jsonlog("Error parsing JSON %s ", (y) ); \
    if (root != NULL) { \
    cJSON_Delete(root); \
    } \
    return (z); \
    } \
}while(0)

#define JSON_VALIDATE_JSON_AS_STRING(x,y,z)  do { \
    if ((x)->type != cJSON_String) { \
    _jsonlog("Error parsing JSON, %s not a string ", (y) ); \
    if (root != NULL)  { \
    cJSON_Delete(root); \
    } \
    return (z); \
    } \
    if((x)->valuestring == NULL) { \
    _jsonlog("Error parsing JSON, %s not a valid string ", (y) ); \
    if (root != NULL) { \
    cJSON_Delete(root); \
    } \
    return (z); \
    } \
}while(0)

#define JSON_VALIDATE_JSON_AS_NUMBER(x,y)   do { \
    if ((x)->type != cJSON_Number) { \
    _jsonlog("Error parsing JSON, %s not a integer ", (y) ); \
    if (root != NULL)  { \
    cJSON_Delete(root); \
    } \
    return (BVIEW_STATUS_INVALID_JSON); \
    } \
}while(0)

#define JSON_COMPARE_STRINGS_AND_CLEANUP(x,y,z)  do { \
    if (strcmp((y), (z)) != 0) { \
    _jsonlog("The JSON contains invalid value for %s (actual %s, required %s) ", (x), (y), (z) ); \
    if (root != NULL) { \
    cJSON_Delete(root); \
    } \
    return (BVIEW_STATUS_INVALID_JSON); \
    } \
}while(0)

#define JSON_COMPARE_VALUE_AND_CLEANUP(x,y,z)  do { \
    if ((y) != (z)) { \
    _jsonlog("The JSON contains invalid value for %s (actual %d, required %d) ", (x), (y), (z) ); \
    if (root != NULL) { \
    cJSON_Delete(root); \
    } \
    return (BVIEW_STATUS_INVALID_JSON); \
    } \
}while(0)

#define JSON_CHECK_VALUE_AND_CLEANUP(x,y,z)  do { \
    if ( ((x) < (y)) || ( (x) > (z)) ) { \
    _jsonlog("The JSON number out of range %d (min %d, max %d) ", (x), (y), (z) ); \
    if (root != NULL) { \
    cJSON_Delete(root); \
    } \
    return (BVIEW_STATUS_INVALID_JSON); \
    } \
}while(0)

#define JSON_ASIC_ID_MAP_FROM_NOTATION(_aId, _asicStr) do { \
    int _asicId = 0; \
    BVIEW_STATUS rv = sbapi_system_asic_translate_from_notation( (_asicStr), &_asicId); \
    if (rv != BVIEW_STATUS_SUCCESS) { \
            _jsonlog("The JSON string can't be converted to asicId %s ", (_asicStr)); \
            return BVIEW_STATUS_INVALID_JSON; \
    } \
    (_aId) = _asicId; \
    } while(0)
    
#define JSON_PORT_MAP_FROM_NOTATION(_port, _portStr) do { \
    int _pId = 0; \
    BVIEW_STATUS rv = sbapi_system_port_translate_from_notation( (_portStr), &_pId); \
    if (rv != BVIEW_STATUS_SUCCESS) { \
            _jsonlog("The JSON string can't be converted to Port# %s ", (_portStr)); \
            return BVIEW_STATUS_INVALID_JSON; \
    } \
    (_port) = _pId; \
    } while(0)
        

#define JSON_ASIC_ID_MAP_TO_NOTATION(_aId, _asicStr) do { \
    BVIEW_STATUS rv = sbapi_system_asic_translate_to_notation( (_aId), (_asicStr)); \
    if (rv != BVIEW_STATUS_SUCCESS) { \
            _jsonlog("The asicId can't be converted to external notation %d ", (_aId)); \
            return BVIEW_STATUS_INVALID_JSON; \
    } \
    } while(0)
    
#define JSON_PORT_MAP_TO_NOTATION(_port, _asic, _portStr) do { \
    BVIEW_STATUS rv = sbapi_system_port_translate_to_notation((_asic), (int)(_port), (_portStr)); \
    if (rv != BVIEW_STATUS_SUCCESS) { \
            _jsonlog("The port can't be converted to external notation %d ", (int)(_port)); \
            return BVIEW_STATUS_INVALID_JSON; \
    } \
    } while(0)
    
#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_JSON_H */

