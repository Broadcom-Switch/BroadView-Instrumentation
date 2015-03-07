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

#include "rest_api.h"
#include "system.h"
#include "openapps_log_api.h"
#include "modulemgr.h"
#include "sb_redirector_api.h"

#include "broadview.h"
#include "feature.h"
#include "bst.h"
#include "sbplugin_api.h"

#ifdef FEAT_VENDOR_INIT
extern int driverInit();
#define VENDOR_PLATFORM_INIT()  driverInit();
#else
#define VENDOR_PLATFORM_INIT()  ;
#endif

/*********************************************************************
* @brief        Function used to initialize various system components
*               Each component is initialized using a proper API
*
* @param[in]    NA
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/
void system_init()
{
 
  /* Initialize platform */
  VENDOR_PLATFORM_INIT(); 

  /*Initialize logging, Not handling error as openlog() does not return anything*/ 
  logging_init();

  /*Initialize Module manager*/ 
  if (modulemgr_init() != BVIEW_STATUS_SUCCESS)
  {
    LOG_POST (BVIEW_LOG_CRITICAL, "Failed to initialize Module Manager\r\n");
  }  
  /*Initialize south-bound plugin*/ 
  if (sb_redirector_init() != BVIEW_STATUS_SUCCESS)
  {
    LOG_POST (BVIEW_LOG_CRITICAL, "Failed to initialize south-bound plugin\r\n");
  }  
  /*Initialize south-bound BST plugin*/ 
  if (sbplugin_common_init() != BVIEW_STATUS_SUCCESS)
  {
    LOG_POST (BVIEW_LOG_CRITICAL, "Failed to initialize south-bound BST plugin r\n");
  }  
  /*Initialize BST application*/ 
  if (bst_main() != BVIEW_STATUS_SUCCESS)
  {
    LOG_POST (BVIEW_LOG_CRITICAL, "Failed to initialize BST application\r\n");
  }  
  /*Initialize REST*/ 
  if (rest_init() != BVIEW_STATUS_SUCCESS)
  {
    LOG_POST (BVIEW_LOG_CRITICAL, "Failed to initialize REST \n All components must be De-initialized\r\n");
    system_deinit();
  }  
} 

/*********************************************************************
* @brief     Function used to deinitialize various system components
*            Individual components are deinitialized using proper
*            function calls  
*
*
* @param[in] NA
*
* @retval    NA
*
* @note      NA
*
* @end
*********************************************************************/

void system_deinit()
{
  /*Deinitialize logging, Not handling error as closelog() does not return anything*/ 

  /*Functions used to deinitialize other modules needs to be called from here*/
  /*TBD respective component owners to include appropriate function call*/ 
  logging_deinit();
  bst_app_uninit();
}  
