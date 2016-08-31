
/*********************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename bhdapp_menu.h 
  *
  * @purpose BroadView BHD reference application 
  *
  * @component Black hole detection 
  *
  * @comments
  *
  * @create 3/7/2016
  *
  * @author 
  * @end
  *
  **********************************************************************/
#ifndef INCLUDE_BHDAPP_MENU_H
#define INCLUDE_BHDAPP_MENU_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "bhdapp.h"
#include "bhdapp_json.h"



typedef enum _bhdapp_user_input_status_ {
  USER_INPUT_OK = 0,
  USER_INPUT_NONE,
  USER_INPUT_ERROR
} BHDAPP_USER_INPUT_STATUS_t;


/******************************************************************
 * @brief      Function to provide (main)features menu to user  
 *
 * @param[in]   bhdRestMessages  Buffer that holds the rest message for 
 *                              different methods. And the JSON buffer of
 *                              user selected method would get updated with
 *                              JSON string
 *
 *              restMesgIndex   User selected method  
 *
 * @retval   none   
 *
 * @note     
 *********************************************************************/
void bhdapp_get_user_input(BHDAPP_REST_MSG_t *bhdRestMessages, unsigned int *restMesgIndex);


/******************************************************************
 * @brief      Target function to read input from user to prepare 
 *             JSON buffer for black-hole-detection-enable method. If input is not 
 *             provided for particular fields, default values would be taken 
 *             to prepare JSON 
 *
 * @param[in]   jsonFrmt       Format of the JSON buffer 
 *              finalJsonBuf   Buffer to hold the json message with user inputs 
 *
 * @retval   0    Successfully read user inputs and prepared json buffer
 *
 * @note     
 *********************************************************************/
int config_bhd_feature(char *jsonFrmt, char *finalJsonBuf);

/**************************************************************************************
 * @brief      Target function to read input from user to prepare 
 *             JSON buffer for   get-black-hole-detection-enable method. If input is not 
 *             provided for particular fields, default values would be taken 
 *             to prepare JSON 
 *
 * @param[in]   jsonFrmt       Format of the JSON buffer 
 *              finalJsonBuf   Buffer to hold the json message with user inputs 
 *
 * @retval   0    Successfully read user inputs and prepared json buffer
 *
 * @note     
 ****************************************************************************************/
int get_bhd_feature(char *jsonFrmt, char *finalJsonBuf);

/******************************************************************
 * @brief      Target function to read input from user to prepare 
 *             JSON buffer to configure black hole. 
 *             If input is not provided for particular 
 *             fields, default values would be taken to prepare JSON 
 *
 * @param[in]   jsonFrmt       Format of the JSON buffer 
 *              finalJsonBuf   Buffer to hold the json message with user inputs 
 *
 * @retval   0    Successfully read user inputs and prepared json buffer
 *
 * @note     
 *********************************************************************/
int configure_bhd_black_hole(char *jsonFrmt, char *finalJsonBuf);


/******************************************************************
 * @brief      Target function to read input from user to prepare 
 *             JSON buffer to cancel black hole. 
 *             If input is not provided for particular fields, 
 *             default values would be taken to prepare JSON 
 *
 * @param[in]   jsonFrmt       Format of the JSON buffer 
 *              finalJsonBuf   Buffer to hold the json message with user inputs 
 *
 * @retval   0    Successfully read user inputs and prepared json buffer
 *
 * @note     
 *********************************************************************/
int cancel_bhd_black_hole(char *jsonFrmt, char *finalJsonBuf);

/******************************************************************
 * @brief      Target function to read input from user to prepare 
 *             JSON buffer to get black hole configuration. 
 *             If input is not provided for particular fields, 
 *             default values would be taken to prepare JSON 
 *
 * @param[in]   jsonFrmt       Format of the JSON buffer 
 *              finalJsonBuf   Buffer to hold the json message with user inputs 
 *
 * @retval   0    Successfully read user inputs and prepared json buffer
 *
 * @note     
 *********************************************************************/
int get_bhd_black_hole(char *jsonFrmt, char *finalJsonBuf);

/******************************************************************
 * @brief      Target function to read input from user to prepare 
 *             JSON buffer to get sflow sampling status. 
 *             If input is not provided for particular fields, 
 *             default values would be taken to prepare JSON 
 *
 * @param[in]   jsonFrmt       Format of the JSON buffer 
 *              finalJsonBuf   Buffer to hold the json message with user inputs 
 *
 * @retval   0    Successfully read user inputs and prepared json buffer
 *
 * @note     
 *********************************************************************/
int get_bhd_sflow_sampling_status(char *jsonFrmt, char *finalJsonBuf);

#ifdef	__cplusplus
}
#endif

#endif

