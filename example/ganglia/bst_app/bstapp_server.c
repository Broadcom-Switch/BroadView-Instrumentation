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

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include "cJSON.h" 
#include "bstapp_http.h"
#include "bstapp_map.h"
#include "bstapp_table.h"
#include "bstapp_cache.h"
#include "bstapp_server_debug.h"
#include "bstapp_server.h"


#define BSTAPP_MAX_BUFFER_LEN ((9*1024) * 32) 
#define BSTAPP_MAX_JSON_LEN (1024)
#define BSTAPP_CONFIG_SERVER "127.0.0.1"
#define MAX_SUPPORTED_METHODS 2
#define JSON_MAX_NODE_LENGTH 128
#define BST_ID_MIN 0
#define BST_ID_MAX 11 
#define BSTAPP_CONNECTION_RETRY_INTERVAL 5

extern char bstapp_conf_file_name[];
static char async_report_buf[BSTAPP_MAX_BUFFER_LEN+1] = {0};
/*********************************************************************
 * @brief : function to return the threshold type for the given realm
 *
 * @param[in] str : realm passed in the input request
 *
 * @retval    : the threshold type value
 *
 * @note : The minumum value of enum type is starts from 1.
 *         if 0 is returned then the given realm didn't match
 *         with any of the supported realm types
 *
 *********************************************************************/
static int realm_handler_get (char *str, bstapp_api_handler_t *handler)
{
  unsigned int i = 0;

  const REALM_HANDLER_MAP_t realm_handler_map[] = {
    {"device", extract_device_info},
    {"ingress-port-priority-group", extract_i_p_pg_info},
    {"ingress-port-service-pool", extract_i_p_sp_info},
    {"ingress-service-pool", extract_i_sp_info},
    {"egress-port-service-pool", extract_e_p_sp_info},
    {"egress-service-pool", extract_e_sp_info},
    {"egress-uc-queue", extract_e_ucq_info},
    {"egress-uc-queue-group", extract_e_ucqg_info},
    {"egress-mc-queue", extract_e_mcq_info},
    {"egress-cpu-queue", extract_e_cpu_info},
    {"egress-rqe-queue", extract_e_rqe_info}
  };
  for (i = BST_ID_MIN; i < BST_ID_MAX; i++)
  {
    if (0 == strcmp (str, realm_handler_map[i].realm))
    {
      *handler = realm_handler_map[i].handler;
      return 0;
    }
  }
  return -1;
}

/******************************************************************
 * @brief  validates a string , whether its a proper HTTP method or not
 *
 * @param[in]   method    string for validation
 *
 * @retval   BVIEW_STATUS_SUCCESS if the string is a valid HTTP method
 * @retval   BVIEW_STATUS_FAILURE otherwise
 *
 * @note
 *********************************************************************/
static int validate_http_method(char *method)
{
  /* Not exhaustive, but minimal methods for functionality */
  static char *supportedMethods[] = { "GET" , "POST" };
  int i = 0;
  for (i = 0; i < MAX_SUPPORTED_METHODS; i++)
  {
    if (strcmp(method, supportedMethods[i]) == 0)
      return 0;
  }

  return -1;
}

/******************************************************************
 * @brief  This function parses http request to extract relevant fields.
 *
 * @param[in]   peer    The connecting peer details
 *
 * @note
 *********************************************************************/
int parse_http_request (char *buffer)
{
  char *buf = buffer;
  char *httpMethod, *url, *json, *restMethod;
  int status;
  int temp = 0, urlLength = 0;

  /* raw http data is available @ session->buffer */
  /* This needs to be parsed into httpMethod, URL and the JSON body */


  /* HTTP method will be the first word of the request */
  httpMethod = strtok(buf, BSTAPP_HTTP_SPACE);
  _BSTAPP_ASSERT_NET_ERROR((httpMethod != NULL), "Invalid HTTP Request \n");

  /* validate the method */
  status = validate_http_method(httpMethod);
  _BSTAPP_ASSERT_NET_ERROR((status == 0), "Unsupported HTTP Method \n");

  /* advance the buf, now it should point to the URL */
  buf += strlen(httpMethod) + 1;

  url = strtok(buf, BSTAPP_HTTP_SPACE);
  _BSTAPP_ASSERT_NET_ERROR((url != NULL), "Invalid HTTP Request \n");

  /* obtain the REST method */
  restMethod = url;
  urlLength = strlen(url);
  for (temp = 0; temp < urlLength; temp++)
  {
    /* method is the string after the last / */
    if (url[temp] == '/')
    {
      if (temp < (urlLength - 1))
        restMethod = &url[temp + 1];
    }
    if(0 != strcmp(restMethod, "agent_response"))
      return -1;
  }
  /* move buf, past the URL, this should now point to HTTP header, */
  buf += urlLength + 1;

  /* look for the twin carriage-return-linefeed sequence, which signals end of header */
  json = strstr(buf, BSTAPP_HTTP_TWIN_CRLF);
  if (NULL == json)
  {
    debug_msg("Invalid HTTP Request \n");
  }
  _BSTAPP_ASSERT_NET_ERROR((json != NULL), "Invalid HTTP Request \n");

  /* move past the end of header, after which, json points to body */
  json += strlen(BSTAPP_HTTP_TWIN_CRLF);

  /* pass the json to extract the data */

  parse_object(json);
  return 0;
}


int parse_object(char *jsonBuffer)
{
  cJSON* json_realm = NULL;
  cJSON* json_data = NULL;
  cJSON *root, *report, *method;

  char realm[128];
  bstapp_api_handler_t handler;

  int i;


  /* Parse JSON to a C-JSON root */
  root = cJSON_Parse(jsonBuffer);
  JSON_VALIDATE_JSON_POINTER(root, "root", -1);

/* parse the JSON method */

  method = cJSON_GetObjectItem(root, "method");
  JSON_VALIDATE_JSON_POINTER(method, "method", -1);
  JSON_VALIDATE_JSON_AS_STRING(method, "method", -1);

  if ( 0 != strcmp(method->valuestring, "get-bst-report"))
  {
    debug_msg ("method %s in the json report is not report \n", method->valuestring);
    return -1;
  }



  report = cJSON_GetObjectItem(root,"report");

  for (i = 0 ; i < cJSON_GetArraySize(report) ; i++)
  {
    /* extract realm */
    memset(realm, 0, 128);
    cJSON * subitem = cJSON_GetArrayItem(report, i);
    json_realm = cJSON_GetObjectItem(subitem, "realm");
    strncpy (&realm[0], json_realm->valuestring,strlen(json_realm->valuestring));
    if (0 == strcmp(realm, "device"))
    {
      json_data = cJSON_GetObjectItem(subitem, "data");
        realm_handler_get(realm, &handler);
        handler(json_data);
        continue;
    }
    json_data = cJSON_GetObjectItem(subitem, "data");
    if(0 == cJSON_GetArraySize(json_data))
    {
      /* no data for this realm */
      continue;
    }
    else
    {
      /* find the appropriate function pointer 
        to populate the data  for this realm */
        realm_handler_get(realm, &handler);
        handler(json_data);
    }
  }
  if (root != NULL)
   {
       cJSON_Delete(root);
   }

  return 0;
}


int extract_device_info(cJSON *json_data)
{
  uint64_t val;

  val = json_data->valuedouble;
  bst_stat_set(0, 0, 0,BSTAPP_STAT_ID_DEVICE, 0, val);
  return 0;
}


int extract_i_p_pg_info(cJSON *json_data)
{
  cJSON *json_port, *json_p_pg_data, *json_p_pg, *json_p_pg_sub_data;
  cJSON *json_pg, *json_um_share, *json_um_headroom;

  int i, j, k,prt, pg;
  char port[128] = {0};
  uint64_t val = 0;

  for (i = 0; i < cJSON_GetArraySize(json_data); i++)
  {
	  json_p_pg = cJSON_GetArrayItem(json_data, i);
	  /* get the port string */
	  json_port = cJSON_GetObjectItem(json_p_pg,"port");


	  strncpy(&port[0], json_port->valuestring, strlen(json_port->valuestring));
	  prt = atoi(port);
	  json_p_pg_data = cJSON_GetObjectItem(json_p_pg,"data");

	  for (j = 0; j < cJSON_GetArraySize(json_p_pg_data); j++)
	  {
		  json_p_pg_sub_data = cJSON_GetArrayItem(json_p_pg_data, j);
		  k = 0;
		  json_pg = cJSON_GetArrayItem(json_p_pg_sub_data, k);
		  pg = json_pg->valueint;
		  k++;

		  json_um_share = cJSON_GetArrayItem(json_p_pg_sub_data, k);
		  val = json_um_share->valuedouble;
		  bst_stat_set(0, prt, pg,BSTAPP_STAT_ID_PRI_GROUP_SHARED, 0, val);
		  k++;

		  json_um_headroom = cJSON_GetArrayItem(json_p_pg_sub_data, k);
		  val = json_um_headroom->valuedouble;

		  bst_stat_set(0, prt, pg,BSTAPP_STAT_ID_PRI_GROUP_HEADROOM, 0, val);
	  }
  }

  return 0;
}


int extract_i_p_sp_info(cJSON *json_data)
{
  cJSON *json_port, *json_p_sp, *json_p_sp_sub_data;
  cJSON *json_pg, *json_p_sp_data, *json_um_share;

  int i, j, k, prt, sp;
  char port[128] = {0};
  uint64_t val = 0;

  for (i = 0; i < cJSON_GetArraySize(json_data); i++)
  {
    json_p_sp = cJSON_GetArrayItem(json_data, i);

    json_port = cJSON_GetObjectItem(json_p_sp,"port");
    strncpy(&port[0], json_port->valuestring, strlen(json_port->valuestring));
    prt = atoi(port);

    json_p_sp_data = cJSON_GetObjectItem(json_p_sp,"data");
     
    for (j = 0; j < cJSON_GetArraySize(json_p_sp_data); j++)
    {
      json_p_sp_sub_data = cJSON_GetArrayItem(json_p_sp_data,j);
      k = 0;
      json_pg = cJSON_GetArrayItem(json_p_sp_sub_data, k);
      sp = json_pg->valueint;
      k++;
      json_um_share = cJSON_GetArrayItem(json_p_sp_sub_data, k);
      val = json_um_share->valuedouble;

      bst_stat_set(0, prt, sp, BSTAPP_STAT_ID_PORT_POOL, 0, val);

    } 
  }

  return 0;
}


int extract_i_sp_info(cJSON *json_data)
{
  cJSON *json_sp_data, *json_spool,*json_um_share;

  int i,  k, sp;
  uint64_t val = 0;

  for (i = 0; i < cJSON_GetArraySize(json_data); i++)
  {
    json_sp_data = cJSON_GetArrayItem(json_data, i);
/*    for (j = 0; j < cJSON_GetArrayItem(json_sp); j++)
    { *
      json_sp_data = cJSON_GetArrayItem(json_sp, j);*/
      k = 0;
      json_spool = cJSON_GetArrayItem(json_sp_data, k);

      sp = json_spool->valueint;
      k++;
      json_um_share = cJSON_GetArrayItem(json_sp_data, k);
      val = json_um_share->valuedouble;
      bst_stat_set(0, 0, sp, BSTAPP_STAT_ID_ING_POOL, 0, val);

  }

  return 0;
}


int extract_e_p_sp_info(cJSON *json_data)
{
  cJSON *json_port, *json_p_sp_data, *json_e_p_sp, *json_sp_sub_data;
  cJSON *json_sp, *json_um_share, *json_uc_share, *json_mc_share;

  int i, j, k, prt, sp;
  char port[128] = {0};
  uint64_t val = 0;


  for (i = 0; i < cJSON_GetArraySize(json_data); i++)
  {
    json_e_p_sp = cJSON_GetArrayItem(json_data, i);

    /* get the port string */
    json_port = cJSON_GetObjectItem(json_e_p_sp,"port");

    strncpy(&port[0], json_port->valuestring, strlen(json_port->valuestring));
    prt = atoi(port);
    json_p_sp_data = cJSON_GetObjectItem(json_e_p_sp,"data");
   
     for (j = 0; j < cJSON_GetArraySize(json_p_sp_data); j++)
     {
       json_sp_sub_data = cJSON_GetArrayItem(json_p_sp_data, j);

       k = 0;
       json_sp = cJSON_GetArrayItem(json_sp_sub_data, k);
       sp = json_sp->valueint;
       k++;

       json_uc_share = cJSON_GetArrayItem(json_sp_sub_data, k);
       val = json_uc_share->valuedouble;
       bst_stat_set(0, prt, sp, BSTAPP_STAT_ID_EGR_UCAST_PORT_SHARED, 0, val);
       k++;


       json_um_share = cJSON_GetArrayItem(json_sp_sub_data, k);
       val = json_um_share->valuedouble;
       bst_stat_set(0, prt, sp, BSTAPP_STAT_ID_EGR_PORT_SHARED, 0, val);
       k++;


       json_mc_share = cJSON_GetArrayItem(json_sp_sub_data, k);
       val = json_mc_share->valuedouble;
       bst_stat_set(0, prt, sp, BSTAPP_STAT_ID_EGR_PORT_MCAST_SHARED, 0, val);
       k++;
     }
        

  }

  return 0;

}


int extract_e_sp_info(cJSON *json_data)
{
  cJSON *json_um_share, *json_mc_share, *json_mc_queue;
  cJSON *json_sp, *json_e_sp_data;

  int i, j, sp;
  uint64_t val=0;

  for (i = 0; i < cJSON_GetArraySize(json_data); i++)
  {
    json_e_sp_data = cJSON_GetArrayItem(json_data, i);

    j = 0;
    json_sp = cJSON_GetArrayItem(json_e_sp_data, j);
    sp = json_sp->valueint;
    j++;

    json_um_share = cJSON_GetArrayItem(json_e_sp_data, j);
    val = json_um_share->valuedouble;
    bst_stat_set(0, 0, sp, BSTAPP_STAT_ID_EGR_POOL, 0, val);

    j++;
    json_mc_share = cJSON_GetArrayItem(json_e_sp_data, j);
    val = json_mc_share->valuedouble;
    bst_stat_set(0, 0, sp, BSTAPP_STAT_ID_EGR_MCAST_POOL, 0, val);

    j++;
    json_mc_queue = cJSON_GetArrayItem(json_e_sp_data, j);
    val = json_mc_queue->valuedouble;
    bst_stat_set(0, 0, sp, BSTAPP_STAT_ID_EGR_MCAST_SHARE_QUEUE, 0, val);

 }

  return 0;
}



int extract_e_ucq_info(cJSON *json_data)
{
  cJSON *json_queue_data, *json_queue, *json_uc_share;
  cJSON *json_port;

  int i, j, queue, prt;
  uint64_t val = 0;
  char port[128] = {0};


  for (i = 0; i < cJSON_GetArraySize(json_data); i++)
  {
    json_queue_data = cJSON_GetArrayItem(json_data, i);
    j = 0;
    json_queue = cJSON_GetArrayItem(json_queue_data, j);
    queue = json_queue->valueint;
    j++;

    json_port = cJSON_GetArrayItem(json_queue_data, j);
    strncpy(&port[0], json_port->valuestring, strlen(json_port->valuestring));
    prt = atoi(port);
    j++;

    json_uc_share = cJSON_GetArrayItem(json_queue_data, j);
    val = json_uc_share->valuedouble;

    bst_stat_set(0, 0, queue, BSTAPP_STAT_ID_UCAST, 0, val);
  }

  return 0;
}

int extract_e_ucqg_info(cJSON *json_data)
{
  cJSON *json_e_ucqg;
  cJSON *json_qg, *json_uc_share;

  int i, j, qg;
  uint64_t val = 0;

  for (i = 0; i < cJSON_GetArraySize(json_data); i++)
  {
    json_e_ucqg = cJSON_GetArrayItem(json_data, i);

    j = 0;
    json_qg = cJSON_GetArrayItem(json_e_ucqg, j);
    qg = json_qg->valueint;
    j++;

    json_uc_share = cJSON_GetArrayItem(json_e_ucqg, j);
    val = json_uc_share->valuedouble;
    bst_stat_set(0, 0, qg, BSTAPP_STAT_ID_UCAST_GROUP, 0, val);

  }

  return 0;
}


int extract_e_mcq_info(cJSON *json_data)
{
  cJSON *json_e_mcq;
  cJSON *json_qg, *json_mc_buf_count, *json_mc_queue_entries;

  int i, j, qg;
  uint64_t val =0;

  for (i = 0; i < cJSON_GetArraySize(json_data); i++)
  {
    json_e_mcq = cJSON_GetArrayItem(json_data, i);
    j = 0;
    json_qg = cJSON_GetArrayItem(json_e_mcq, j);
    qg = json_qg->valueint;
    j++;

    json_qg = cJSON_GetArrayItem(json_e_mcq, j);
    j++;
    
    json_mc_buf_count = cJSON_GetArrayItem(json_e_mcq, j);
    val = json_mc_buf_count->valuedouble;
    bst_stat_set(0, 0, qg, BSTAPP_STAT_ID_MCAST, 0, val);

    j++;
    json_mc_queue_entries = cJSON_GetArrayItem(json_e_mcq, j);
    val = json_mc_queue_entries->valuedouble;
    bst_stat_set(0, 0, qg, BSTAPP_STAT_ID_MCAST_QUEUE_ENTRIES, 0, val);
  }

  return 0;
}

int extract_e_rqe_info(cJSON *json_data)
{
  cJSON *json_e_rqe;
  cJSON *json_pg, *json_rqe_buf_count, *json_rqe_queue_entries;

  int i, j, qg;
  uint64_t val = 0;

  for (i = 0; i < cJSON_GetArraySize(json_data); i++)
  {
    json_e_rqe = cJSON_GetArrayItem(json_data, i);
    j = 0;
    json_pg = cJSON_GetArrayItem(json_e_rqe, j);
    qg = json_pg->valueint;
    j++;
    json_rqe_buf_count = cJSON_GetArrayItem(json_e_rqe, j);
    val = json_rqe_buf_count->valuedouble;
    bst_stat_set(0, 0, qg, BSTAPP_STAT_ID_RQE_QUEUE, 0, val);

    j++;
    json_rqe_queue_entries = cJSON_GetArrayItem(json_e_rqe, j);
    val = json_rqe_queue_entries->valuedouble;
    bst_stat_set(0, 0, qg, BSTAPP_STAT_ID_RQE_POOL, 0, val);
  }

  return 0;
}

int extract_e_cpu_info(cJSON *json_data)
{
  cJSON *json_e_cpu;
  cJSON *json_pg, *json_cpu_buf_count;

  int i, j, qg;
  uint64_t val = 0;

  for (i = 0; i < cJSON_GetArraySize(json_data); i++)
  {
    json_e_cpu = cJSON_GetArrayItem(json_data, i);
    j = 0;
    json_pg = cJSON_GetArrayItem(json_e_cpu, j);
    qg = json_pg->valueint;
    j++;
    json_cpu_buf_count = cJSON_GetArrayItem(json_e_cpu, j);
    val = json_cpu_buf_count->valuedouble;
    bst_stat_set(0, 0, qg, BSTAPP_STAT_ID_CPU_QUEUE, 0, val);
  }

  return 0;
}

/******************************************************************
 * @brief  sends a HTTP 200 message to the client
 *
 * @param[in]   fd    socket for sending message
 *
 * @retval   BVIEW_STATUS_SUCCESS
 *
 * @note
 *********************************************************************/
int rest_send_200(int fd)
{
  int nbytes =0;
  char *response = "HTTP/1.1 200 OK \r\n"
    "Server: Ubuntu 14.0.4 \r\n\r\n";

  nbytes =  write(fd, response, strlen(response));

  if (0 >= nbytes)
  {
    return -1;
  }

  return 0;
}


/******************************************************************
 * @brief      Function to read data received from client using socket 
 *
 * @param[in]  filedes : socket
 *
 * @retval   none
 *
 *  @note
 **********************************************************************/

int read_from_client (int fd, char *buf)
{
    int length = 0;
    int temp = 0;

   struct timeval timeout;
   fd_set cset;
   int retval = 0;



   for (;;)
   {
     FD_ZERO (&cset);
     FD_SET (fd, &cset);

     timeout.tv_sec = 2;
     timeout.tv_usec = 0;

     retval = select (fd+1, &cset, NULL, NULL, &timeout);
     if (retval < 0)
     {
       debug_msg ("Error select of socket failed [%d : %s] \n",
                    errno, strerror(errno));
       return -1;
     }
     else if (retval == 0)
     {
       debug_msg("Socket timed out no data [%d : %s] \n",
           errno, strerror(errno));
       break;
     }

     temp = read(fd, (buf + length), (BSTAPP_MAX_BUFFER_LEN - length));
     if (temp < 0)
     {
       if (errno == EINTR)
       {
         continue; /* perfectly normal; try again */
       }
       else if (errno == EAGAIN)
       {
         continue;
       }
       else
       {
            debug_msg( "Socket read error [%d : %s] \n",
             errno, strerror(errno));
         return -1;
       }
     }
     else if (temp == 0)
     {
       /* the connection has been closed by your peer;  */
           debug_msg("Socket closed by peer, closing socket [%d : %s] \n",
           errno, strerror(errno));
       break;
     }
     else
     {
       /* you got some data; do whatever with it... */
       length += temp;
       continue;
     }
   }
    /* if there is any error reading */
    if ((temp < 0) && (length <= 0))
    {
	    debug_msg("BSTAPP : Error reading data on incoming report, closing socket [%d : %s] \n",
			    errno, strerror(errno));
	    return -1;
    }

   return 0;
}


    /* initialize the app config with default values */
void bstapp_config_init( BSTAPP_CONFIG_t *config)
{
    strcpy(config->agentIp, BSTAPP_CONFIG_PROPERTY_AGENT_IP_DEFAULT);
    config->agentPort = BSTAPP_CONFIG_PROPERTY_AGENT_PORT_DEFAULT;
    config->localPort = BSTAPP_CONFIG_PROPERTY_LOCAL_PORT_DEFAULT;
    config->pollingInterval =  BSTAPP_CONFIG_PROPERTY_POLLING_INTERVAL_DEFAULT;
    config->statType        = BSTAPP_CONFIG_PROPERTY_STAT_TYPE_DEFAULT;
}

int bstapp_server_main (void)
{

    int listenFd, connectionFd;
    int temp;
    struct sockaddr_in serverAddr;
    struct sockaddr_in peerAddr;
    socklen_t peerLen;
    char *buff;
    BSTAPP_CONFIG_t config;
    struct sockaddr_in switchAddr;    
 

    buff = &async_report_buf[0];
    bst_cache_init();

    bstapp_config_init(&config);

    /* read the config file */
    if (-1 == bstapp_config_read(&config))
    {
      debug_msg("failed to read the config file.\n");
    }

    /* configure the bst on the agent */
    bstapp_communicate_with_agent((void *)&config);

    memset(&switchAddr, 0, sizeof (struct sockaddr_in));
    switchAddr.sin_family = AF_INET;
    temp = inet_pton(AF_INET, &config.agentIp[0], &switchAddr.sin_addr);

    if (temp <= 0) {
        debug_msg("Failed to convert switch/client ip address %s \n", strerror(errno));
    }
    
    /* setup listening socket */
    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0)
      debug_msg( "Error Creating server socket");

    /* Initialize the server address and bind to the required port */
    memset(&serverAddr, 0, sizeof (struct sockaddr_in));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(config.localPort);

    /* bind to the socket, */
    temp = bind(listenFd, (struct sockaddr*) &serverAddr, sizeof (serverAddr));
    if (temp < 0)
    debug_msg("Error binding to the port");

    /* Listen for connections */
    temp = listen(listenFd, 1);
    if (temp < 0)
    debug_msg( "Error listening (making socket as passive) ");

    /* Every thing set, start accepting connections */
    while (1)
    {
        peerLen = sizeof (peerAddr);

        /* wait for an incoming connection */
        temp = accept(listenFd, (struct sockaddr*) &peerAddr, &peerLen);
        if (temp == -1)
        {
          continue;
        }

        if (switchAddr.sin_addr.s_addr != peerAddr.sin_addr.s_addr)
        {
          /* Connection is from different ip address than the configured switch address */ 
          /* Close the connection */
          debug_msg("Connection is from %s, where as the configured switch IP is %s, so closing socket\n",
                    inet_ntoa(peerAddr.sin_addr), &config.agentIp[0]);
          close(temp);
          continue;    
        }
 
        /* initialize the connection socket */
        connectionFd = temp;

        /* process the request */
        memset(async_report_buf, 0, sizeof(async_report_buf));
        if (read_from_client(connectionFd, buff) == 0)
        {
           parse_http_request(async_report_buf);
        } 

        close(connectionFd);
    }

    /* execution  shouldn't reach here */
    close(listenFd);
    return 0;
}


/******************************************************************
 * @brief  sends an commands to agent and logs the received responses
 *
 *
 * @note
 *********************************************************************/
int  bstapp_communicate_with_agent(void *param)
{
    BSTAPP_CONFIG_t *config = (BSTAPP_CONFIG_t *) param;
    char *header = "POST /broadview/bst/configure-bst-feature HTTP/1.1\r\n"
            "Host: BroadViewAgent \r\n"
            "User-Agent: BroadView BST App\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml\r\n"
            "Content-Length: %d\r\n"
            "\r\n";

     char *myjsonFormat[] = { "{     \
                       \"jsonrpc\": \"2.0\", \
                       \"method\": \"configure-bst-feature\", \
                       \"asic-id\": \"1\", \
                       \"params\": { \
                       \"bst-enable\": 1, \
                       \"send-async-reports\": 1, \
                       \"collection-interval\": %d, \
                       \"stat-units-in-cells\": 1 ,\
                       \"send-snapshot-on-trigger\": 1, \
                       \"trigger-rate-limit\": 2, \
                       \"trigger-rate-limit-interval\": 10, \
                       \"async-full-reports\": 1, \
                       \"stats-in-percentage\": %d\
                }, \
                \"id\": 1 \
                }",

                "{   \
                       \"jsonrpc\": \"2.0\", \
                       \"method\": \"configure-bst-tracking\", \
                       \"asic-id\": \"1\", \
                       \"params\": { \
                       \"track-peak-stats\" : 0, \
                       \"track-ingress-port-priority-group\" : 1, \
                       \"track-ingress-port-service-pool\" : 1,  \
                       \"track-ingress-service-pool\" : 1, \
                       \"track-egress-port-service-pool\" : 1, \
                       \"track-egress-service-pool\" : 1, \
                       \"track-egress-uc-queue\" : 1, \
                       \"track-egress-uc-queue-group\" : 1, \
                       \"track-egress-mc-queue\" : 1, \
                       \"track-egress-cpu-queue\" : 1, \
                       \"track-egress-rqe-queue\" : 1, \
                       \"track-device\" : 1 \
                       }, \
                       \"id\": 2 \
                 }"
              };

    char myjson[BSTAPP_MAX_JSON_LEN] = { 0 };
    char sendBuf[BSTAPP_MAX_HTTP_BUFFER_LENGTH] = { 0 };
    int clientFd;
    struct sockaddr_in clientAddr;
    int temp = 0;
    char report[BSTAPP_MAX_BUFFER_LEN]  = {0};
    char *buff = &report[0];
    int total_num_of_jsons = 2;
    int current_json_num  = 1;

    _BSTAPP_ASSERT(config != NULL);

    while (current_json_num <= total_num_of_jsons)
    {
      memset(myjson, 0, sizeof (myjson));
      if (current_json_num == 1)
      {
        sprintf(myjson, myjsonFormat[current_json_num-1], config->pollingInterval,
                          ((config->statType == BSTAPP_STAT_TYPE_ABSOLUTE)? 0 : 1)) ;
      }
      else if (current_json_num == 2)
      {
        sprintf(myjson, myjsonFormat[current_json_num-1]);
      }

      current_json_num++;
      memset(sendBuf, 0, sizeof (sendBuf));
      snprintf(sendBuf, BSTAPP_MAX_HTTP_BUFFER_LENGTH, header, strlen(myjson));
      for (; ;)
      {
        /* create socket to send data to */
        clientFd = socket(AF_INET, SOCK_STREAM, 0);
        if (clientFd == -1) {
            debug_msg("Error Creating server socket : %s \n", strerror(errno));
        }
        /* setup the socket */
        memset(&clientAddr, 0, sizeof (struct sockaddr_in));
        clientAddr.sin_family = AF_INET;
        clientAddr.sin_port = htons(config->agentPort);
        temp = inet_pton(AF_INET, &config->agentIp[0], &clientAddr.sin_addr);
        if (temp <= 0) {
            debug_msg("Error Creating server socket %s \n", strerror(errno));
            close(clientFd);
        }

        /* connect to the peer */
        temp = connect(clientFd, (struct sockaddr *) &clientAddr, sizeof (clientAddr));
        if (temp == -1) {
            close(clientFd);
            debug_msg("Failed to connect to BST Agent: Retrying..\n");
            sleep(BSTAPP_CONNECTION_RETRY_INTERVAL);
            continue;
        }

        /* send data */
        temp = send(clientFd, sendBuf, strlen(sendBuf), MSG_MORE);
        if (temp == -1) {
            debug_msg("Error sending data %s \n", strerror(errno));
            close(clientFd);
        }
        temp = send(clientFd, myjson, strlen(myjson), 0);
        if (temp == -1) {
            debug_msg("Error sending data %s \n", strerror(errno));
            close(clientFd);
        }

        /*Now read from the socket for the response */
         read_from_client(clientFd, buff);
         close(clientFd);
         break;
      }
    }
    return 0;
}


/******************************************************************
 * @brief  Reads configuration from a file.
 *
 * @param[in]   config      config to be setup
 *
 * @retval   0  when configuration is initialized successfully
 * @retval  -1  on any error
 *
 * @note
 *********************************************************************/

 int bstapp_config_read(BSTAPP_CONFIG_t *config)
{
    FILE *configFile;
    char line[_BSTAPP_CONFIGFILE_LINE_MAX_LEN] = { 0 };
    int numLinesRead = 0;

    /* dummy structure for validating IP address */
    struct sockaddr_in clientIpAddr;
    int temp;

    /* for string manipulation */
    char *property, *value;


    memset(config, 0, sizeof (BSTAPP_CONFIG_t));

    /* open the file. if file not available/readable, return appropriate error */
    configFile = fopen(bstapp_conf_file_name, _BSTAPP_CONFIGFILE_READ_MODE);

    if (configFile == NULL)
    {
      debug_msg("BSTAPP : Configuration file %s not found:\n",
                    bstapp_conf_file_name);
        return -1;
    }


    /* read the lines one-by-one. if any of the lines is corrupted
     * i.e., doesn't contain valid tokens, return error
     */

    while (numLinesRead < 5)
    {
        memset (&line[0], 0, _BSTAPP_CONFIGFILE_LINE_MAX_LEN);

        /* read one line from the file */
        property = fgets(&line[0], _BSTAPP_CONFIGFILE_LINE_MAX_LEN, configFile);
        if(NULL == property)
        {
          return -1;
        }

        numLinesRead++;

        /* split the line into tokens, based on the file format */
        property = strtok(&line[0], _BSTAPP_CONFIGFILE_DELIMITER);
        if(NULL == property)
        {
          return -1;
        }
        value = property + strlen(property) + 1;

        /* Is this token the agent IP address ?*/
        if (strcmp(property, BSTAPP_CONFIG_PROPERTY_AGENT_IP) == 0)
        {
            /* truncate the newline characters */
            value[strlen(value) - 1] = 0;

            /* is this IP address valid ? */
            temp = inet_pton(AF_INET, value, &(clientIpAddr.sin_addr));
            _BSTAPP_ASSERT_CONFIG_FILE_ERROR(temp > 0);

            /* copy the agent ip address */
            strncpy(&config->agentIp[0], value, BSTAPP_MAX_IP_ADDR_LENGTH - 1);
            continue;
        }


        /* Is this token the agent port number ?*/
        if (strcmp(property, BSTAPP_CONFIG_PROPERTY_AGENT_PORT) == 0)
        {
            /* is this port number valid ? */
            temp = strtol(value, NULL, 10);
            _BSTAPP_ASSERT_CONFIG_FILE_ERROR( errno != ERANGE);

            /* copy the agent port number */
            config->agentPort = temp;
            continue;
        }

        /* Is this token the bst app local port number ?*/
        if (strcmp(property, BSTAPP_CONFIG_PROPERTY_LOCAL_PORT) == 0)
        {
            /* is this port number valid ? */
            temp = strtol(value, NULL, 10);
            _BSTAPP_ASSERT_CONFIG_FILE_ERROR( errno != ERANGE);

            /* copy the agent port number */
            config->localPort = temp;
            continue;
        }

        /* Is this token the bst app polling interval?*/
        if (strcmp(property, BSTAPP_CONFIG_PROPERTY_POLLING_INTERVAL) == 0)
        {
            /* is this port number valid ? */
            temp = strtol(value, NULL, 10);
            _BSTAPP_ASSERT_CONFIG_FILE_ERROR( errno != ERANGE);

            /* copy the agent port number */
            config->pollingInterval = temp;
            continue;
        }

        /* Is this token the bst app stat type?*/
        if (strcmp(property, BSTAPP_CONFIG_PROPERTY_STAT_TYPE) == 0)
        {
           /* is this port number valid ? */
           temp = strtol(value, NULL, 10);
           _BSTAPP_ASSERT_CONFIG_FILE_ERROR( errno != ERANGE);

           /* copy the agent port number */
           config->statType = temp;
           continue;
        }

        /* unknown property */
         debug_msg("BSTAPP : Unknown property in configuration file : %s \n",
                    property);
        fclose(configFile);
        return -1;
    }
    fclose(configFile);
    return 0;
}

