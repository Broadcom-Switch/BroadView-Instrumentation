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
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "bstapp.h"
#include "bstapp_debug.h"

extern BSTAPP_REST_MENU_MSG_t bstRestMenuMessages[];
extern int bstapp_read_from_agent (int fd );

/******************************************************************
 * @brief      Read input string from user
 *
 * @param[in]   usrInput       Buffer to hold the string
 *              sizeOfDest     Max size of the buffer
 *
 * @retval   USER_INPUT_NONE    There is no input from user
 *           USER_INPUT_ERROR   Input is incorrect
 *           USER_INPUT_OK      Input is successfully read.
 * @note
 *********************************************************************/
static int  bstapp_get_string_from_user(char *usrInput, int sizeofinput)
{

  char inputBuf[BSTAPP_MAX_STRING_LENGTH] = { 0 };
  int originalLen = 0;
  
  while(1)
  {
    if(fgets(inputBuf, sizeof(inputBuf), stdin) != NULL)
    {
      if (strlen(inputBuf) == 1)
      {
        return USER_INPUT_NONE;
      }
      originalLen = strlen(inputBuf);
      inputBuf[originalLen-1] = 0;

      if (strlen(inputBuf) > (sizeofinput-1))
      {
        printf("\t\t Maximum allowed is %u characters!\n", (sizeofinput-1));
        fflush(stdin);
        return USER_INPUT_ERROR;
      }
      strncpy(usrInput, inputBuf, strlen(inputBuf)+1);
      fflush(stdin);
      break;
    }
    return USER_INPUT_NONE; 
  }
  return USER_INPUT_OK; 
}


/******************************************************************
 * @brief      Read input string from user and convert it to boolean
 *
 * @param[in]   usrInput        Buffer to hold the integer
 *
 * @retval   USER_INPUT_OK      Input is successfully read and converted to integer
 * @note
 *********************************************************************/

int bstapp_get_bool_from_user(int *usrInput)
{
  char inputBuf[BSTAPP_MAX_STRING_LENGTH] = { 0 };
  int originalLen = 0;

  if (fgets(inputBuf, sizeof(inputBuf), stdin) != NULL)
  { /* fgets reads next line char(\n) as part of string */
    /* only \n is present */
    if(strlen(inputBuf) == 1)
    {
      strncpy(inputBuf, BSTAPP_DEFAULT_ENABLE, sizeof(inputBuf));
      *usrInput = atoi(inputBuf);
    }
    else
    {
      originalLen = strlen(inputBuf);
      inputBuf[originalLen-1] = 0;

      *usrInput = atoi(inputBuf);
      if(*usrInput == 0)
      {
        *usrInput = BSTAPP_DISABLE;
      }
      else 
      {
        *usrInput = BSTAPP_ENABLE;
      }
    }
  }
  return USER_INPUT_OK; 
}
 

/******************************************************************
 * @brief      Read input string from user and convert it to integer
 *
 * @param[in]   usrInput        Buffer to hold the integer
 *
 * @retval   USER_INPUT_NONE    There is no input from user
 *           USER_INPUT_ERROR   Input is incorrect
 *           USER_INPUT_OK      Input is successfully read and converted to integer
 * @note
 *********************************************************************/
static int bstapp_get_uint_from_user(int *usrInput)
{
  char inputBuf[BSTAPP_MAX_STRING_LENGTH] = { 0 };
  int originalLen = 0;
   
  while (1)
  {
    if (fgets(inputBuf, sizeof(inputBuf), stdin) != NULL)
    {
      /* fgets reads next line char(\n) as part of string */
      /* only \n is present */
      if (strlen(inputBuf) == 1)
      {
        return USER_INPUT_NONE;
      }
      originalLen = strlen(inputBuf);
      inputBuf[originalLen-1] = 0;

      if (1 == sscanf(inputBuf, "%u", usrInput))
      {
        fflush(stdin);
        break;
      }
      else
      {
        fflush(stdin);
        return USER_INPUT_ERROR;
      }
    }
    return USER_INPUT_NONE;
  }

  return USER_INPUT_OK;
}


/******************************************************************
 * @brief      Function that provides menu to get parameters from user
 *
 * @param[in]  param 
 *
 * @retval    0         If user input is correct and packet is prepared successfully
 *           -1         If user input is incorrect or failed to prepare packet
 *
 * @note
 *********************************************************************/
int bstapp_debug_menu(void *param)
{
  BSTAPP_CONFIG_t *config = (BSTAPP_CONFIG_t *) param;
  char *header = "%s /broadview/bst/%s HTTP/1.1\r\n"
            "Host: BroadViewAgent \r\n"
            "User-Agent: BroadView BST App\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml\r\n"
            "Content-Length: %d\r\n"
            "\r\n";


  char sendBuf[BSTAPP_MAX_HTTP_BUFFER_LENGTH] = { 0 };
  char jsonBuf[BSTAPP_MAX_HTTP_BUFFER_LENGTH]= { 0 };
  int clientFd;
  struct sockaddr_in clientAddr;
  int temp = 0;
  int input;
  char inputBuf[BSTAPP_MAX_STRING_LENGTH] = { 0 };
  char asic_id[BSTAPP_MAX_ASIC_LENGTH] = { 0 };
  int bst_enable;
  int collection_interval;
  int async_report;
  int stat_units_in_cells;
  int track_peak_stats;
  int track_ingress_port_priority_group;
  int track_ingress_port_service_pool;
  int track_ingress_service_pool;  
  int track_egress_port_service_pool;
  int track_egress_service_pool;
  int track_egress_uc_queue;
  int track_egress_uc_queue_group;
  int track_egress_mc_queue;
  int track_egress_cpu_queue;
  int track_egress_rqe_queue;
  int track_device;
  int threshold;
  char clearBst[BSTAPP_MAX_STRING_LENGTH] = { 0 };
  int retValue;

  BSTAPP_REST_MENU_MSG_t *restMsg;

  _BSTAPP_ASSERT(config != NULL);

  _BSTAPP_LOG(_BSTAPP_DEBUG_TRACE, "Starting communication with agent ");

  while(1)	
  {
    printf("\n 1.Configure BST feature \n 2.Configure BST tracking \n 3.Configure BST threshold \n "
    "4.Clear BST statistics \n 5.Clear BST threshold \n 6.Get BST feature \n 7.Get BST tracking \n "
    "8.Get BST thresholds \n 9.Get BST report \n 10.Exit ");
    printf("\n Select an option from the menu: ");
    bstapp_get_uint_from_user(&input);
 
    if (input > BSTAPP_MAX_INPUT || input <= 0)
    {	
      printf("\nInvalid Selection, Select again:\n");
      continue; 
    }
    if (input == BSTAPP_MAX_INPUT)
    {
      exit(0);
    }		
    restMsg = &bstRestMenuMessages[input-1];

    switch(input)
    {			
      case 1:
      {	
        /* asic id*/
        printf("\n\t Provide asic-id (string)[1]: ");
        retValue = bstapp_get_string_from_user(asic_id, sizeof(asic_id)); 
        if (retValue == USER_INPUT_NONE)
        {
          strncpy(asic_id, BSTAPP_DEFAULT_ASIC_ID, sizeof(asic_id));
        }
 
       /* bst-enable*/	
        printf("\n\t Provide bst-enable (0/1) [1]: ");
        bstapp_get_bool_from_user(&bst_enable);
	
        /* async-reports*/
        printf("\n\t Provide asyn-report (0/1) [1]: ");
        bstapp_get_bool_from_user(&async_report);
					
        /*collection-interval*/
        printf("\n\t Provide collection-interval (0-600) [60]:  ");
        retValue = bstapp_get_uint_from_user(&collection_interval);

        if (retValue == USER_INPUT_NONE)
        {
          strncpy(inputBuf, BSTAPP_DEFAULT_COLLECTION_INTERVAL, sizeof(inputBuf));
          collection_interval = atoi(inputBuf);
        }
    
        /*stat-units-in-cells*/
        printf("\n\t Provide stat-units-in-cells (0/1) [1]: ");
        bstapp_get_bool_from_user(&stat_units_in_cells);

        sprintf(jsonBuf, restMsg->json, asic_id, bst_enable, async_report, collection_interval, stat_units_in_cells);
        break;
      }	
      
      case 2:
      {
        printf("\n\t Provide asic-id(string) [1]: ");
        retValue = bstapp_get_string_from_user(asic_id, sizeof(asic_id));
        if (retValue == USER_INPUT_NONE)
        {
          strncpy(asic_id, BSTAPP_DEFAULT_ASIC_ID, sizeof(asic_id));
        }

        /* track peak_stats */
        printf("\n\t Provide track-peak-stats(0/1) [1]: ");
        bstapp_get_bool_from_user(&track_peak_stats);

        /*track_ingress_port_priority_group */
        printf("\n\t Provide track-ingress-port-priority-group(0/1) [1]: ");
        bstapp_get_bool_from_user(&track_ingress_port_priority_group);

        /*track_ingress_port_service_pool */
        printf("\n\t Provide track-ingress-port-service-pool(0/1) [1]: ");
        bstapp_get_bool_from_user(&track_ingress_port_service_pool);
       
        /*track_ingress_service_pool */
        printf("\n\t Provide track-ingress-service-pool(0/1) [1]: ");
        bstapp_get_bool_from_user(&track_ingress_service_pool);

        /* track_egress_port_service_pool */
        printf("\n\t Provide track-egress-port-service-pool(0/1) [1]: ");
        bstapp_get_bool_from_user(&track_egress_port_service_pool);
        
        /* track_egress_service_pool */
        printf("\n\t Provide track-egres-service-pool(0/1) [1]: ");
        bstapp_get_bool_from_user(&track_egress_service_pool);
     
        /* track_egress_uc_queue  */
        printf("\n\t Provide track-egress-uc-queue(0/1) [1]: ");
        bstapp_get_bool_from_user(&track_egress_uc_queue);

        /* track_egress_uc_queue_group  */
        printf("\n\t Provide track-egress-uc-queue-group(0/1) [1]: ");
        bstapp_get_bool_from_user(&track_egress_uc_queue_group);
        
        /* track_egress_mc_queue  */
        printf("\n\t Provide track-egress-mc-queue(0/1) [1]: ");
        bstapp_get_bool_from_user(&track_egress_mc_queue);
 
        /* track_egress_cpu_queue  */
        printf("\n\t Provide track-egress-cpu-queue(0/1) [1]: ");
        bstapp_get_bool_from_user(&track_egress_cpu_queue);

        /* track_egress_rqe_queue  */
        printf("\n\t Provide track-egres-rqe-queue(0/1) [1]: ");
        bstapp_get_bool_from_user(&track_egress_rqe_queue);

        /* track_device  */
        printf("\n\t Provide track-device(0/1) [1]: ");
        bstapp_get_bool_from_user(&track_device);
        
        sprintf(jsonBuf, restMsg->json, asic_id, track_peak_stats, track_ingress_port_priority_group,
          track_ingress_port_service_pool, track_ingress_service_pool, track_egress_port_service_pool,
          track_egress_service_pool, track_egress_uc_queue, track_egress_uc_queue_group,
          track_egress_mc_queue, track_egress_cpu_queue, track_egress_rqe_queue, track_device);
        break;
      } 
     
      case 3:
      {
        /* asic id*/
        printf("\n\t Provide asic-id(string) [1]: ");
        retValue = bstapp_get_string_from_user(asic_id, sizeof(asic_id));
        
        if (retValue == USER_INPUT_NONE)
        {
          strncpy(asic_id, BSTAPP_DEFAULT_ASIC_ID, sizeof(asic_id));
        }

        /*Threshold*/
        printf("\n\t Provide threshold [1000]: ");
        retValue = bstapp_get_uint_from_user(&threshold);

        if (retValue == USER_INPUT_NONE)
        {
          strncpy(inputBuf, BSTAPP_DEFAULT_THRESHOLD, sizeof(asic_id));
          threshold = atoi(inputBuf);
        }

        sprintf(jsonBuf,restMsg -> json, asic_id, threshold);
        break;
      }
      case 4:
      {  
        printf("\n\t Do you want to Clear-BST-Statistics(yes/no) [yes]: ");
        retValue = bstapp_get_string_from_user(clearBst, sizeof(clearBst));
        
        if(retValue == USER_INPUT_NONE)
        {
          printf("\n\t\t Provide asic-id(string) [1]: ");
          retValue = bstapp_get_string_from_user(asic_id, sizeof(asic_id));
          if (retValue == USER_INPUT_NONE)
          {
            strncpy(asic_id, BSTAPP_DEFAULT_ASIC_ID, sizeof(asic_id));
          }

          sprintf(jsonBuf,restMsg -> json, asic_id);
          break;
        }

        if (strcmp(clearBst, "yes") == 0) 
        {/* asic id*/
          printf("\n\t\t Provide asic-id(string) [1]: ");
          retValue = bstapp_get_string_from_user(asic_id, sizeof(asic_id));
          if (retValue == USER_INPUT_NONE)
          {
            strncpy(asic_id, BSTAPP_DEFAULT_ASIC_ID, sizeof(asic_id));
          }

          sprintf(jsonBuf,restMsg -> json, asic_id);
          break;
        } 
        else
        {
         printf("\t BST-Statistics not cleared");
         continue;
        }  
       
      }
      case 5:
      {
        printf("\n\t Do you want to Clear-BST-Threshold(yes/no) [yes]: ");
        retValue = bstapp_get_string_from_user(clearBst, sizeof(clearBst));
        
        if(retValue == USER_INPUT_NONE)
        {
          printf("\n\t\t Provide asic-id(string) [1]: ");
          retValue = bstapp_get_string_from_user(asic_id, sizeof(asic_id));
          if (retValue == USER_INPUT_NONE)
          {
            strncpy(asic_id, BSTAPP_DEFAULT_ASIC_ID, sizeof(asic_id));
          }

          sprintf(jsonBuf,restMsg -> json, asic_id);
          break;
        }

        if (strcmp(clearBst, "yes") == 0)
        {/* asic id*/
          printf("\n\t\t Provide asic-id(string) [1]: ");
          retValue = bstapp_get_string_from_user(asic_id, sizeof(asic_id));
          if (retValue == USER_INPUT_NONE)
          {
            strncpy(asic_id, BSTAPP_DEFAULT_ASIC_ID, sizeof(asic_id));
          }

          sprintf(jsonBuf,restMsg -> json, asic_id);
          break;
        }
        else
        {
           printf("\t BST-Statistics not cleared");
           continue;
        }
      }

      case 6:
      {
        /* asic id*/
        printf("\n\t Provide asic-id(string) [1]: ");
        retValue = bstapp_get_string_from_user(asic_id, sizeof(asic_id));
        if (retValue == USER_INPUT_NONE)
        {
          strncpy(asic_id, BSTAPP_DEFAULT_ASIC_ID, sizeof(asic_id));
        }

        sprintf(jsonBuf,restMsg -> json, asic_id);
        break;	
      }		
      case 7:
      {
        /* asic id*/
        printf("\n\t Provide asic-id [1]: ");
        retValue = bstapp_get_string_from_user(asic_id, sizeof(asic_id));
        if (retValue == USER_INPUT_NONE)
        {
          strncpy(asic_id, BSTAPP_DEFAULT_ASIC_ID, sizeof(asic_id));
        }

        sprintf(jsonBuf,restMsg -> json, asic_id);
        break;
      }	
      case 8:
      {
        /* asic id*/
        printf("\n\t Provide asic-id [1]: ");
        retValue = bstapp_get_string_from_user(asic_id, sizeof(asic_id));
        if (retValue == USER_INPUT_NONE)
        {
          strncpy(asic_id, BSTAPP_DEFAULT_ASIC_ID, sizeof(asic_id));
        }

        sprintf(jsonBuf,restMsg -> json, asic_id);
        break;
      }
      case 9:
      {
        /* asic id*/
        printf("\n\t Provide asic-id [1]: ");
        retValue = bstapp_get_string_from_user(asic_id, sizeof(asic_id));
        if (retValue == USER_INPUT_NONE)
        {
          strncpy(asic_id, BSTAPP_DEFAULT_ASIC_ID, sizeof(asic_id));
        }

        sprintf(jsonBuf,restMsg -> json, asic_id);
        break;
      }
    }
    _BSTAPP_LOG(_BSTAPP_DEBUG_TRACE, "\n Now attempting : %s ", restMsg->descr);

    memset(sendBuf, 0, sizeof (sendBuf));
    snprintf(sendBuf, BSTAPP_MAX_HTTP_BUFFER_LENGTH, header,
    restMsg->httpMethod, restMsg->method, strlen(jsonBuf));
    
    /* create socket to send data to */
    clientFd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientFd == -1) {
      _BSTAPP_LOG(_BSTAPP_DEBUG_ERROR, "\n Error Creating server socket : %s \n", strerror(errno));
    continue;        
    }
    /* setup the socket */
    memset(&clientAddr, 0, sizeof (struct sockaddr_in));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(config->agentPort);
    temp = inet_pton(AF_INET, &config->agentIp[0], &clientAddr.sin_addr);
    if (temp <= 0) {
      _BSTAPP_LOG(_BSTAPP_DEBUG_ERROR, "Error Creating server socket %s \n", strerror(errno));
      close(clientFd);
      continue;
    }
        
    /* connect to the peer */
    temp = connect(clientFd, (struct sockaddr *) &clientAddr, sizeof (clientAddr));
    if (temp == -1) {
      _BSTAPP_LOG(_BSTAPP_DEBUG_ERROR, "Error connecting to client for sending async reports %s \n", strerror(errno));
      close(clientFd);
      continue;
    }

    /* log what is being sent */
    bstapp_message_log(sendBuf, strlen(sendBuf), false);
    bstapp_message_log(jsonBuf, strlen(jsonBuf), false);
   
    /* send data */
    temp = send(clientFd, sendBuf, strlen(sendBuf), MSG_MORE);
    if (temp == -1) {
      _BSTAPP_LOG(_BSTAPP_DEBUG_ERROR, "Error sending data %s \n", strerror(errno));
      close(clientFd);
      continue;
    }
    temp = send(clientFd, jsonBuf, strlen(jsonBuf), 0);
    if (temp == -1) {
      _BSTAPP_LOG(_BSTAPP_DEBUG_ERROR, "Error sending data %s \n", strerror(errno));
      close(clientFd);
      continue;
    }

   /*Now read from the socket for the response */
   bstapp_read_from_agent(clientFd);
   sleep(2);
   close(clientFd);
 }

  _BSTAPP_LOG(_BSTAPP_DEBUG_TRACE, "Completed communication with agent, exiting ");

  pthread_exit(NULL);

}                       
