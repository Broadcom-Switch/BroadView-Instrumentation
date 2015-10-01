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
#ifndef INCLUDE_BSTAPP_SERVER_H
#define INCLUDE_BSTAPP_SERVER_H

#ifdef  __cplusplus
extern "C"
{
#endif

#include <stdint.h> 
#include <sys/socket.h> 


#define BSTAPP_CONFIG_PORT 9888
#define BSTAPP_MAX_BUFFER_LEN (9*1024)
#define BSTAPP_CONFIG_SERVER "127.0.0.1"
#define MAX_SUPPORTED_METHODS 2
#define JSON_MAX_NODE_LENGTH 128
#define BST_ID_MIN 0
#define BST_ID_MAX 11


typedef int(*bstapp_api_handler_t) (cJSON *buffer);

typedef struct bstapp_api_handler_map_s
{
  char realm[256];
  bstapp_api_handler_t handler;
}REALM_HANDLER_MAP_t;

int bstapp_server_main (void);
int parse_object(char *jsonBuffer);
/******************************************************************
 *  @brief     Function to process the incoming command request 
 *  
 *  @param[in] buffer -  data buffer read from client 
 *  @param[in] length -  number of bytes read  
 *  
 *  @retval int 
 * 
 *  @note
 **********************************************************************/
int process_input(char *buffer, int length);

int extract_device_info(cJSON *json_data);

int extract_i_p_pg_info(cJSON *json_data);

int extract_i_p_sp_info(cJSON *json_data);

int extract_i_sp_info(cJSON *json_data);

int extract_e_p_sp_info(cJSON *json_data);

int extract_e_sp_info(cJSON *json_data);

int extract_e_ucq_info(cJSON *json_data);

int extract_e_ucqg_info(cJSON *json_data);

int extract_e_mcq_info(cJSON *json_data);

int extract_e_rqe_info(cJSON *json_data);

int extract_e_cpu_info(cJSON *json_data);

int bstapp_config_read(BSTAPP_CONFIG_t *config);

int  bstapp_communicate_with_agent(void *param);
#ifdef  __cplusplus
}
#endif

#endif

