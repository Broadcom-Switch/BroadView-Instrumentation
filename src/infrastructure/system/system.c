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
extern int openapps_driver_init(bool debug, bool menu);
#define VENDOR_PLATFORM_INIT(debug, menu) openapps_driver_init(debug, menu);
#else
#define VENDOR_PLATFORM_INIT(debug,menu);
#endif

#ifdef FEAT_PT
extern BVIEW_STATUS pt_main ();
extern void pt_app_uninit();
#endif

extern  void driv_app_get_user_input();

/*********************************************************************
* @brief        Function used to initialize various system components
*               such as module manager, redirector, agent and sbplugins
*
* @param[in]    param 
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/
void bview_system_init_ph2(void *param)
{
 

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

#ifdef FEAT_PT
  /*Initialize PT application*/ 
  if (pt_main() != BVIEW_STATUS_SUCCESS)
  {
    LOG_POST (BVIEW_LOG_CRITICAL, "Failed to initialize PT application\r\n");
  } 
#endif
  /*Initialize REST*/ 
  if (rest_init() != BVIEW_STATUS_SUCCESS)
  {
    LOG_POST (BVIEW_LOG_CRITICAL, "Failed to initialize REST \n All components must be De-initialized\r\n");
    bview_system_deinit();
  }  
} 

/*********************************************************************
* @brief        Function used to initialize various system components
*               such as openapps driver and calls phase-2 init
*
* @param[in]    debug     debug mode of openapps driver
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/
void bview_system_init_ph1(bool vendor_debug , bool menu)
{
  /* Initialize platform */
  VENDOR_PLATFORM_INIT(vendor_debug, menu);
  if (false == vendor_debug)
  {
  bview_system_init_ph2(NULL); 
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

void bview_system_deinit()
{
  /*Deinitialize logging, Not handling error as closelog() does not return anything*/ 

  /*Functions used to deinitialize other modules needs to be called from here*/
  /*TBD respective component owners to include appropriate function call*/ 
  logging_deinit();
  bst_app_uninit();
#ifdef FEAT_PT
  pt_app_uninit();
#endif
}  
