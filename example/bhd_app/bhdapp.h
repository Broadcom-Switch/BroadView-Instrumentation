/*********************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename bhdapp.h
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

#ifndef INCLUDE_BHDAPP_H
#define	INCLUDE_BHDAPP_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <arpa/inet.h>

#define BHD_ENABLE  1
#define BHD_DISABLE 0
#define BHDAPP_WRITE_PCAP_FILE 1

#define ETHERNET_MAC_ADDR_LEN         6
#define ETHERNET_MAC_ADDR_STR_LEN     18

#define BHDAPP_DEFAULT_ASIC_ID      "1"
#define BHDAPP_DEFAULT_BHD_CONFIG    BHD_DISABLE 
#define BHDAPP_DEFAULT_PORT_LIST    "1"
#define BHDAPP_DEFAULT_COLLECTION_INTERVAL  0

#define BHDAPP_MAX_SUPPORTED_PKT_SIZE  (9*1024)
#define BHDAPP_MIN_SUPPORTED_PKT_SIZE  (64)
#define BHDAPP_MAX_PCAP_FILE_SIZE      (BHDAPP_MAX_SUPPORTED_PKT_SIZE + 1024)
#define BHDAPP_MAX_B64_PCAP_FILE_SIZE  ((4 * ((BHDAPP_MAX_PCAP_FILE_SIZE + 2)/3)) + 16)
#define BHDAPP_JSON_BUFF_MAX_SIZE  (BHDAPP_MAX_SUPPORTED_PKT_SIZE + (4*1024))

#define BHDAPP_MAX_STRING_LENGTH      128
#define BHDAPP_MAX_HTTP_BUFFER_LENGTH 2048

#define BHDAPP_MAX_REPORT_LENGTH    (8192*10)

#define BHDAPP_MAX_IP_ADDR_LENGTH    20

/* file from where the configuration properties are read. */
#define BHDAPP_CONFIG_FILE    "bhdapp_config.cfg"

#define BHDAPP_CONFIG_PROPERTY_AGENT_IP   "agent_ip"
#define BHDAPP_CONFIG_PROPERTY_AGENT_IP_DEFAULT   "127.0.0.1"    

#define BHDAPP_CONFIG_PROPERTY_AGENT_PORT   "agent_port"
#define BHDAPP_CONFIG_PROPERTY_AGENT_PORT_DEFAULT   8080

#define BHDAPP_CONFIG_PROPERTY_LOCAL_PORT "bhdapp_port"
#define BHDAPP_CONFIG_PROPERTY_LOCAL_PORT_DEFAULT 9070


#define BHDAPP_COMMUNICATION_LOG_FILE_NEW   "/tmp/bhdapp_communication_new.log"
#define BHDAPP_COMMUNICATION_LOG_FILE_OLD   "/tmp/bhdapp_communication_old.log"
#define BHDAPP_COMMUNICATION_LOG_MAX_FILE_SIZE  (1 * 1024 * 1024)

#define BHDAPP_HTTP_TWIN_CRLF     "\r\n\r\n"

#define BHDAPP_MAX_USER_INPUT_STRING_SIZE 2048

#define BHDAPP_DEFAULT_SAMPLING_METHOD   "agent"
#define BHDAPP_DEFAULT_WATERMARK         1024

#define BHDAPP_DEFAULT_PERIODICITY       10
#define BHDAPP_MIN_SAMPLE_PERIODICITY    1
#define BHDAPP_MAX_SAMPLE_PERIODICITY    60

#define BHDAPP_DEFAULT_SAMPLE_COUNT      2
#define BHDAPP_MIN_SAMPLE_COUNT          0
#define BHDAPP_MAX_SAMPLE_COUNT          30

#define BHDAPP_DEFAULT_VLAN           1
#define BHDAPP_MIN_VLAN               1
#define BHDAPP_MAX_VLAN               4096

#define BHDAPP_DEFAULT_SRC_UDP_PORT      9999
#define BHDAPP_DEFAULT_DEST_UDP_PORT     8888

#define BHDAPP_DEFAULT_MIRROR_PORT       "4"

#define BHDAPP_DEFAULT_SAMPLE_POOL_SIZE  1024
#define BHDAPP_MIN_SAMPLE_POOL_SIZE      1024

#define BHDAPP_DEFAULT_DEST_IP          "10.10.10.10" 



/* Ethernet MAC Address */
typedef struct macAddr_s
{
  char  addr[ETHERNET_MAC_ADDR_LEN];
}macAddr_t;

typedef struct _bhdapp_config_
{
    char agentIp[BHDAPP_MAX_IP_ADDR_LENGTH];
    int agentPort;
    int localPort;
} BHDAPP_CONFIG_t;

typedef enum _bhdapp_json_method_ {
    BHDAPP_JSON_CONFIGURE_BLACK_HOLE_DETECTION = 0,
    BHDAPP_JSON_GET_BLACK_HOLE_DETECTION,
    BHDAPP_JSON_CONFIGURE_BLACK_HOLE,
    BHDAPP_JSON_GET_BLACK_HOLE,
    BHDAPP_JSON_CANCEL_BLACK_HOLE,
    BHDAPP_JSON_GET_SFLOW_SAMPLING_STATUS,
    BHDAPP_JSON_METHOD_LAST,
} BHDAPP_JSON_METHOD_t;


typedef struct _bhdapp_rest_msg_ {
    BHDAPP_JSON_METHOD_t methodId;
    char *httpMethod;
    char *method;
    char *json;
    char *descr;
}BHDAPP_REST_MSG_t;


typedef struct _bhdapp_json_method_info_  {
   BHDAPP_JSON_METHOD_t methodId; 
   char *method;
   char *menuString;
   char *jsonFrmt;
   int (*get_input_for_json)(char *jsonFrmt, char *finalJsonBuf);
} BHDAPP_JSON_METHOD_INFO_t;


/* initialize configurations */
int bhdapp_config_init(BHDAPP_CONFIG_t *config);

/* for communication logging */
int bhdapp_logging_init(void);

int bhdapp_message_log(char *message, int length, bool isFromAgent);

int bhdapp_communicate_with_agent(void *config);

int bhdapp_http_server_run(BHDAPP_CONFIG_t *config);



unsigned int get_new_json_id();

#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_BHDAPP_H */

