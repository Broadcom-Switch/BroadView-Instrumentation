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

#ifndef INCLUDE_PTAPP_H
#define	INCLUDE_PTAPP_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <arpa/inet.h>

#define PT_ENABLE  1
#define PT_DISABLE 0
#define TRACE_PKT_DROP      1
#define TRACE_PKT_FORWARD   0
#define PTAPP_WRITE_PCAP_FILE 0

#define PTAPP_DEFAULT_ASIC_ID      "1"
#define PTAPP_DEFAULT_PT_CONFIG    PT_DISABLE 
#define PTAPP_DEFAULT_PORT_LIST    "1"
#define PTAPP_DEFAULT_COLLECTION_INTERVAL  0
#define PTAPP_DEFAULT_TRACE_PKT_DROP_CONFIG   TRACE_PKT_DROP
#define PTAPP_DEFAULT_TRACE_PKT_DROP_CONFIG_STR  "yes"

#define PTAPP_MAX_SUPPORTED_PKT_SIZE  (9*1024)
#define PTAPP_MIN_SUPPORTED_PKT_SIZE  (64)
#define PTAPP_MAX_PCAP_FILE_SIZE      (PTAPP_MAX_SUPPORTED_PKT_SIZE + 1024)
#define PTAPP_MAX_B64_PCAP_FILE_SIZE  ((4 * ((PTAPP_MAX_PCAP_FILE_SIZE + 2)/3)) + 16)
#define PTAPP_JSON_BUFF_MAX_SIZE  (PTAPP_MAX_SUPPORTED_PKT_SIZE + (4*1024))

#define PTAPP_MAX_STRING_LENGTH      128
#define PTAPP_MAX_HTTP_BUFFER_LENGTH 2048

#define PTAPP_MAX_REPORT_LENGTH    (8192*10)

#define PTAPP_MAX_IP_ADDR_LENGTH    20

/* file from where the configuration properties are read. */
#define PTAPP_CONFIG_FILE    "ptapp_config.cfg"

#define PTAPP_CONFIG_PROPERTY_AGENT_IP   "agent_ip"
#define PTAPP_CONFIG_PROPERTY_AGENT_IP_DEFAULT   "127.0.0.1"    

#define PTAPP_CONFIG_PROPERTY_AGENT_PORT   "agent_port"
#define PTAPP_CONFIG_PROPERTY_AGENT_PORT_DEFAULT   8080

#define PTAPP_CONFIG_PROPERTY_LOCAL_PORT "ptapp_port"
#define PTAPP_CONFIG_PROPERTY_LOCAL_PORT_DEFAULT 9070


#define PTAPP_COMMUNICATION_LOG_FILE_NEW   "/tmp/ptapp_communication_new.log"
#define PTAPP_COMMUNICATION_LOG_FILE_OLD   "/tmp/ptapp_communication_old.log"
#define PTAPP_COMMUNICATION_LOG_MAX_FILE_SIZE  (1 * 1024 * 1024)

#define PTAPP_HTTP_TWIN_CRLF     "\r\n\r\n"

#define PTAPP_MAX_USER_INPUT_STRING_SIZE 2048

#define PTAPP_DEFAULT_PKT_LEN         1024
#define PTAPP_MAX_SUPPORTED_OFFSETVALUE_LEN 2048 


#define PTAPP_DEFAULT_MPLS_PKT_STR      "no"
#define PTAPP_MPLS_LABEL_DEF_PRESENCE   "no"
#define PTAPP_MPLS_MAX_LABELS_NUM       (2)
#define PTAPP_MPLS_MAX_LABEL_VAL        (0xFFFFF) 
#define PTAPP_MPLS_MAX_EXP_VAL          (0x7)

typedef struct _ptapp_config_
{
    char agentIp[PTAPP_MAX_IP_ADDR_LENGTH];
    int agentPort;
    int localPort;
} PTAPP_CONFIG_t;

typedef enum _ptapp_json_method_ {
    PTAPP_JSON_CONFIGURE_PACKET_TRACE_FEATURE = 0,
#ifdef DROP_REASON_SUPPORTED
    PTAPP_JSON_CONFIGURE_PACKET_TRACE_DROP_REASON,
#endif
    PTAPP_JSON_GET_PACKET_TRACE_FEATURE,
    PTAPP_JSON_GET_PACKET_TRACE_PROFILE,
    PTAPP_JSON_GET_PACKET_TRACE_LAG_RESOLUTION,
    PTAPP_JSON_GET_PACKET_TRACE_ECMP_RESOLUTION,
#ifdef DROP_REASON_SUPPORTED
    PTAPP_JSON_GET_PACKET_TRACE_DROP_REASON,
    PTAPP_JSON_GET_PACKET_TRACE_DROP_COUNTER_REPORT,
    PTAPP_JSON_GET_PACKET_TRACE_SUPPORTED_DROP_REASONS,
#endif
    PTAPP_JSON_CANCEL_PACKET_TRACE_PROFILE,
    PTAPP_JSON_CANCEL_PACKET_TRACE_LAG_RESOLUTION,
    PTAPP_JSON_CANCEL_PACKET_TRACE_ECMP_RESOLUTION,
#ifdef DROP_REASON_SUPPORTED
    PTAPP_JSON_CANCEL_PACKET_TRACE_SEND_DROP_PACKET,
    PTAPP_JSON_CANCEL_PACKET_TRACE_DROP_COUNTER_REPORT, 
#endif
    PTAPP_JSON_METHOD_LAST,
} PTAPP_JSON_METHOD_t;


typedef struct _ptapp_rest_msg_ {
    PTAPP_JSON_METHOD_t methodId;
    char *httpMethod;
    char *method;
    char *json;
    char *descr;
}PTAPP_REST_MSG_t;


typedef struct _ptapp_json_method_info_  {
   PTAPP_JSON_METHOD_t methodId; 
   char *method;
   char *menuString;
   char *jsonFrmt;
   int (*get_input_for_json)(char *jsonFrmt, char *finalJsonBuf);
} PTAPP_JSON_METHOD_INFO_t;

/* initialize configurations */
int ptapp_config_init(PTAPP_CONFIG_t *config);

/* for communication logging */
int ptapp_logging_init(void);

int ptapp_message_log(char *message, int length, bool isFromAgent);

int ptapp_communicate_with_agent(void *config);

int ptapp_http_server_run(PTAPP_CONFIG_t *config);


int config_pt_feature(char *jsonFrmt, char *finalJsonBuf);

int get_pt_feature(char *jsonFrmt, char *finalJsonBuf);

int get_pt_profile(char *jsonFrmt, char *finalJsonBuf);

int cancel_pt_profile_request(char *jsonFrmt, char *finalJsonBuf);

unsigned int get_new_json_id();

#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_PTAPP_H */

