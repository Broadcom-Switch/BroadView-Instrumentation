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

#ifndef INCLUDE_BST_APP_H
#define	INCLUDE_BST_APP_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <arpa/inet.h>

#define BSTAPP_MAX_STRING_LENGTH      128
#define BSTAPP_MAX_HTTP_BUFFER_LENGTH 2048

#define BSTAPP_MAX_REPORT_LENGTH    (8192*10)

#define BSTAPP_MAX_IP_ADDR_LENGTH    20

/* file from where the configuration properties are read. */
#define BSTAPP_CONFIG_FILE    "bstapp_config.cfg"

#define BSTAPP_CONFIG_PROPERTY_AGENT_IP   "agent_ip"
#define BSTAPP_CONFIG_PROPERTY_AGENT_IP_DEFAULT   "127.0.0.1"    

#define BSTAPP_CONFIG_PROPERTY_AGENT_PORT   "agent_port"
#define BSTAPP_CONFIG_PROPERTY_AGENT_PORT_DEFAULT   8080

#define BSTAPP_CONFIG_PROPERTY_LOCAL_PORT "bstapp_port"
#define BSTAPP_CONFIG_PROPERTY_LOCAL_PORT_DEFAULT 9070

#define BSTAPP_CONFIG_PROPERTY_MAX_REPORTS "bstapp_max_reports"
#define BSTAPP_CONFIG_PROPERTY_MAX_REPORTS_DEFAULT 10


#define BSTAPP_COMMUNICATION_LOG_FILE   "/tmp/bstapp_communication.log"   

#define BSTAPP_HTTP_TWIN_CRLF     "\r\n\r\n"

typedef struct _bstapp_config_
{
    char agentIp[BSTAPP_MAX_IP_ADDR_LENGTH];

    int agentPort;

    int localPort;

    int maxReports;
} BSTAPP_CONFIG_t;

typedef struct _bstapp_rest_msg_ {
    char *httpMethod;
    char *method;
    char *json;
    char *descr;
}BSTAPP_REST_MSG_t;

/* initialize configurations */
int bstapp_config_init(BSTAPP_CONFIG_t *config);

/* for communication logging */
int bstapp_logging_init(void);

int bstapp_message_log(char *message, int length, bool isFromAgent);

int bstapp_communicate_with_agent(void *config);

int bstapp_http_server_run(BSTAPP_CONFIG_t *config);



#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_BST_APP_H */

