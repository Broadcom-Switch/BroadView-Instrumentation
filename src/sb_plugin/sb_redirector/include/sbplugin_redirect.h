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

#ifndef INCLUDE_SBPLUGIN_REDIRECT_H
#define INCLUDE_SBPLUGIN_REDIRECT_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "sbplugin.h"
#include "openapps_log_api.h"


/** South bound plug-in redirect plugin list */
typedef  struct {
    /** Indicates whether this redirect plugin entry is free or not */
    bool isInUse;
    /** South bound plugin data*/ 
    BVIEW_SB_PLUGIN_t  sbPlugin;
} BVIEW_SB_REDIRECT_PLUGIN_t;

/** List of South bound plugin data */
extern BVIEW_SB_REDIRECT_PLUGIN_t sbRedirectPluginList[];

/** Read-Write lock for South bound plug-in redirector */
extern pthread_rwlock_t sbRedirectRWLock;

/** South bound plug-in redirector debug flag */
extern int  sbRedirectDebugFlag;

/** Macro to print the South bound plug-in debug information */
#define SB_REDIRECT_DEBUG_PRINT(severity,format, args...)           \
                                if (sbRedirectDebugFlag)            \
                                {                                   \
                                  log_post(severity,format, ##args);\
                                }

/* Macro to acquire read lock */
#define SB_REDIRECT_RWLOCK_RD_LOCK(lock)                             \
           if (pthread_rwlock_rdlock(&lock) != 0)                    \
           {                                                         \
               SB_REDIRECT_DEBUG_PRINT(BVIEW_LOG_ERROR,              \
			   "(%s:%d) Failed to take Module manager"               \
               "read write lock for read\n", __FILE__, __LINE__);    \
               return BVIEW_STATUS_FAILURE;                          \
           } 


/* Macro to acquire write lock */
#define SB_REDIRECT_RWLOCK_WR_LOCK(lock)                             \
           if (pthread_rwlock_wrlock(&lock) != 0)                    \
           {                                                         \
               SB_REDIRECT_DEBUG_PRINT(BVIEW_LOG_ERROR,              \
			   "(%s:%d) Failed to take Module manager"               \
               "read write lock for write\n", __FILE__, __LINE__);   \
               return BVIEW_STATUS_FAILURE;                          \
           } 

/* Macro to release RW lock */
#define SB_REDIRECT_RWLOCK_UNLOCK(lock)                              \
           if (pthread_rwlock_unlock(&lock) != 0)                    \
           {                                                         \
               SB_REDIRECT_DEBUG_PRINT(BVIEW_LOG_ERROR,              \
			   "(%s:%d) Failed to release Module manager"            \
               "read write lock          \n", __FILE__, __LINE__);   \
           } 



/*********************************************************************
* @brief   Get Function list for a particular feature based on Silicon type.
*          A function list is selected in the following order.
*          1. If an exact match for a silicon is found then that function list 
*                                                                  is returned.
*          2. Else return a function list that has support for 
*                                             all silicon types.
*                               
*
* @param[in]  asicType            Asic type
* @param[in]  featureId           Feature ID
*
* @retval   NULL                   if function fails
* @retval   FunctionListPtr        Function list that matches with 
*                                            the silicon type(/all) 
*
*
* @notes    none
*
***************************************************************************************/
extern void *sb_redirect_feature_handle_get(BVIEW_ASIC_TYPE asicType, BVIEW_FEATURE_ID featureId);

/*********************************************************************
* @brief       Get asic capabilities of a silicon
*
* @param[in]  asicType        Asic type
* @param[out] asicCapPtr      Asic capabilities pointer
*
*
* @retval   BVIEW_STATUS_FAILURE      Due to lock acquistion failure or 
*                                     System feature is not present or
*                                     Asic capabilities for a particular 
*                                                         asic not found
*
* @retval   BVIEW_STATUS_SUCCESS      Asic capabilities are present 
*
* @notes    none
*
*
*********************************************************************/
BVIEW_STATUS  sb_redirect_asic_capabilities_get(BVIEW_ASIC_TYPE asicType, 
                                                BVIEW_ASIC_CAPABILITIES_t **asicCapPtr);
 
/*********************************************************************
* @brief       Set debug flag value for south bound plug-in redirector 
*
* @param[in]   val       Value
*                                                                
*
* @retval   none
*
* @notes    none
*
*********************************************************************/
void sb_redirect_debug_set(int val);

#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_SBPLUGIN_REDIRECT_H */
