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

#ifndef INCLUDE_SYSTEM_UTILS_APP_H
#define INCLUDE_SYSTEM_UTILS_APP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************//**
* @brief : function to create the system_utils application thread.
*
* @param[in]  none 
*
* @retval  : BVIEW_STATUS_RESOURCE_NOT_AVAILABLE : Returns when memory is not available
* @retval  : BVIEW_STATUS_FAILURE : failed to create the message queue or thread. 
* @retval  : BVIEW_STATUS_SUCCESS : successful completion of task creation. 
*
* @note : Main function to invoke the system_utils thread. This function allocates the required memory 
*         for system_utils application. Creates the mutexes for the system_utils data. Creates the message queue
*         It assumes that the system is initialized before the system_utils thread is invoked. This api
*        depends on the system variables such as number of units per device. Incase of no such 
*        need, the number of units can be #defined.
*
*********************************************************************/
BVIEW_STATUS system_utils_main ();

/*****************************************************************//**
*  @brief:  function to clean up  system_utils application  
*
* @param[in]  none 
*
* @retval  : none
*
* @note  When in case of any error event, this api cleans up all the resources.
*         i.e it deletes the timer notes, if any are present.
*         destroy the mutexes. clears the memory and destroys the message queue.
*
*********************************************************************/
void system_utils_app_uninit();

/*****************************************************************//**
* @brief : system_utils main application function which does processing of messages
*
* @param[in] : none
*
* @retval  : BVIEW_STATUS_SUCCESS: Fails to register with module manager 
*             or some error happened due to which the system_utils application is
*             unable to process incoming messages.
*
*********************************************************************/
BVIEW_STATUS system_utils_app_main(void);

/*****************************************************************//**
* @brief : function to initialize system_utils structures to default values 
*
* @param[in] : number of units on the agent
*
* @retval  : BVIEW_STATUS_SUCCESS : configuration is successfully initialized.
* @retval  : BVIEW_STATUS_FAILURE : Fail to initialize the SYSTEM UTILS appliation to default.
*
* @note  : Initialization functon to make sure both software and asic are configured with
*          same default values. We are not setting the default values for threshold, but 
*         we are using the same as that of asic.
*
*********************************************************************/
BVIEW_STATUS system_utils_app_config_init(unsigned int num_units);
       
#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_SYSTEM_UTILS_APP_H */

/*!  @}
 * @}
 */
