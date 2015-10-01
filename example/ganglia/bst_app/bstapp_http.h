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

#ifndef INCLUDE_BSTAPP_HTTP_H
#define	INCLUDE_BSTAPP_HTTP_H

#ifdef	__cplusplus
extern "C"
{
#endif

#define BSTAPP_HTTP_CRLF          "\r\n"
#define BSTAPP_HTTP_TWIN_CRLF     "\r\n\r\n"
#define BSTAPP_HTTP_SPACE         " "    
#define BSTAPP_MAX_HTTP_BUFFER_LENGTH 2048
#define _BSTAPP_CONFIGFILE_LINE_MAX_LEN   256
#define _BSTAPP_CONFIGFILE_READ_MODE      "r"
#define _BSTAPP_CONFIGFILE_DELIMITER      "="


#define BSTAPP_MAX_IP_ADDR_LENGTH    20

/* file from where the configuration properties are read. */
#define BSTAPP_CONFIG_FILE    "bstapp_config.cfg"

#define BSTAPP_CONFIG_PROPERTY_AGENT_IP   "agent_ip"
#define BSTAPP_CONFIG_PROPERTY_AGENT_IP_DEFAULT   "127.0.0.1"

#define BSTAPP_CONFIG_PROPERTY_AGENT_PORT   "agent_port"
#define BSTAPP_CONFIG_PROPERTY_AGENT_PORT_DEFAULT   8080

#define BSTAPP_CONFIG_PROPERTY_LOCAL_PORT "bstapp_local_port"
#define BSTAPP_CONFIG_PROPERTY_LOCAL_PORT_DEFAULT 9070


#define BSTAPP_CONFIG_PROPERTY_POLLING_INTERVAL "polling_interval"
#define BSTAPP_CONFIG_PROPERTY_POLLING_INTERVAL_DEFAULT  5


typedef struct _bstapp_config_
{
    char agentIp[BSTAPP_MAX_IP_ADDR_LENGTH];

    int agentPort;

    int localPort;

    int pollingInterval;

} BSTAPP_CONFIG_t;

typedef struct _bstapp_rest_msg_ {
    char *httpMethod;
    char *method;
    char *json;
    char *descr;
}BSTAPP_REST_MSG_t;

    

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
            return (-1); \
            } \
}while(0)

#define JSON_COMPARE_STRINGS_AND_CLEANUP(x,y,z)  do { \
      if (strcmp((y), (z)) != 0) { \
            _jsonlog("The JSON contains invalid value for %s (actual %s, required %s) ", (x), (y), (z) ); \
            if (root != NULL) { \
                  cJSON_Delete(root); \
                  } \
            return (-1); \
            } \
}while(0)

#define JSON_COMPARE_VALUE_AND_CLEANUP(x,y,z)  do { \
      if ((y) != (z)) { \
            _jsonlog("The JSON contains invalid value for %s (actual %d, required %d) ", (x), (y), (z) ); \
            if (root != NULL) { \
                  cJSON_Delete(root); \
                  } \
            return (-1); \
            } \
}while(0)


#define JSON_CHECK_VALUE_AND_CLEANUP(x,y,z)  do { \
      if ( ((x) < (y)) || ( (x) > (z)) ) { \
            _jsonlog("The JSON number out of range %d (min %d, max %d) ", (x), (y), (z) ); \
            if (root != NULL) { \
                  cJSON_Delete(root); \
                  } \
            return (-1); \
            } \
}while(0)


#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_BSTAPP_HTTP_H */

