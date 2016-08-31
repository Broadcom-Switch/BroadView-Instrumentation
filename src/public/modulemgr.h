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

#ifndef INCLUDE_MODULEMGR_H
#define INCLUDE_MODULEMGR_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "broadview.h"
#include "openapps_feature.h"

#define  BVIEW_MAX_MODULES        BVIEW_MAX_FEATURES

    /* Each feature modules information */
    typedef struct _module_feature_info_ {
        BVIEW_FEATURE_ID               featureId;
        char                           featureName[BVIEW_MAX_FEATURE_NAME_LEN];
        BVIEW_REST_API_t               restApiList[BVIEW_MAX_API_CMDS_PER_FEATURE];
    } BVIEW_MODULE_FETAURE_INFO_t;


/*********************************************************************
* @brief       Initialize module manager data with default values
*
*
* @retval   BVIEW_STATUS_FAILURE   if failed to initialize read-write lock
* @retval   BVIEW_STATUS_SUCCESS   if data is successfully initialized 
*                                   to defaults
*
*
* @note    none
*
*********************************************************************/
BVIEW_STATUS modulemgr_init();

/*********************************************************************
* @brief       Register a feature with Module manager, all the modules
*              such as BSt and packet trace need to register api handlers
*              with module manager. 
*
* @param[in]  featureInfo        Feature information
*
* @retval   BVIEW_STATUS_FAILURE     If failed to acquire lock
*
* @retval   BVIEW_STATUS_TABLE_FULL  BVIEW_MAX_MODULES number of modules 
*                                    already registered   
*
* @retval   BVIEW_STATUS_DUPLICATE   If a module is already registered 
*                                    with same feature ID
*
* @retval   BVIEW_STATUS_SUCCESS     Upon registering successfully with 
*                                    Module Manager
*
*
* @note    none
*
*********************************************************************/
BVIEW_STATUS modulemgr_register(BVIEW_MODULE_FETAURE_INFO_t * featureInfoPtr);


/*********************************************************************
* @brief     When a REST API is received, the web server thread obtains 
*            the associated handler using this API 
*
* @param[in]  jsonBuffer       Json Buffer  
* @param[in]  bufLength        Length of the Buffer
* @param[out]  handler         Function handler     
*
* @retval   BVIEW_STATUS_FAILURE     Unable to find function handler
*                                     for the api string in Json buffer
* @retval   BVIEW_STATUS_SUCCESS     Function handler is found
*                                     for the api string in Json buffer
*
* @retval   BVIEW_STATUS_INVALID_JSON    JSON is malformatted, or doesn't
*                                         have necessary data.
* @retval   BVIEW_STATUS_INVALID_PARAMETER  Invalid input parameter
*
* @note    none
*
*********************************************************************/
BVIEW_STATUS modulemgr_rest_api_handler_get(char * jsonBuffer, 
                                            int bufLength, 
                                            BVIEW_REST_API_HANDLER_t *handler);

/*********************************************************************
* @brief     Utility api to get the feature name for the  
*            rest  API  method
*
* @param[in]  apiString       api method string  
* @param[out]  handler          Function handler     
*
* @retval   BVIEW_STATUS_FAILURE     Unable to find feature name 
*                                     for the api string 
* @retval   BVIEW_STATUS_SUCCESS     Feature name is found
*                                     for the api string
*
*
* @retval   BVIEW_STATUS_INVALID_PARAMETER  Invalid input parameter
*
*
*
* @note    none
*
*********************************************************************/
BVIEW_STATUS modulemgr_rest_api_feature_name_get(char * apiString, 
                                            char *featureName);




#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_MODULEMGR_H */
