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

#ifndef INCLUDE_SYSTEM_H
#define INCLUDE_SYSTEM_H

#include "broadview.h"


/**Timer mode Periodic or Non Periodic*/
typedef enum
{
  PERIODIC_MODE = 0,
  NON_PERIODIC_MODE = 1,
}TIMER_MODE;

/*********************************************************************
* @brief        Function used to initialize various system components
*
* @param[in]    NA
*
* @retval       NA
*
* @note         NA
*
* @end
*********************************************************************/

void system_init();

/*********************************************************************
* @brief     Function used to deinitialize various system components
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

void system_deinit();

/*********************************************************************
* @brief         Function used to create and arm a timer
*
*
* @param[in]     handler        function pointer to handle the callback
* @param[in,out] timerId        timerId of the timer which is created
* @param[in]     timeInMilliSec Time after which callback is required
* @param[in]     mode           mode specifing if the timer must be
*                               periodic or oneshot
* @param[in]     param          Arguments passed from the calling function
*
* @retval        BVIEW_STATUS_SUCCESS
* @retval        BVIEW_STATUS_FAILURE
*
* @note          NA
*
* @end
*********************************************************************/

BVIEW_STATUS system_timer_add(void * handler,timer_t * timerId, int timeInMilliSec,TIMER_MODE mode,void * param);


/*********************************************************************
* @brief      Function used to destroy a timer
*
*
* @param[in]  timerId Timer id of the timer which needs to be destroyed
*
* @retval     BVIEW_STATUS_SUCCESS
* @retval     BVIEW_STATUS_FAILURE
*
* @note          NA
*
* @end
*********************************************************************/

BVIEW_STATUS system_timer_delete(timer_t timerId);



/*********************************************************************
* @brief      Function used to set/reset a timer
*
*
* @param[in]  timerId        Timer id of the timer which needs to be
*                            set/reset
* @param[in]  timeInMilliSec Time after which callback is required
* @param[in]  mode           mode specifing if the timer must be
*                            periodic or oneshot
*
* @retval     BVIEW_STATUS_SUCCESS
* @retval     BVIEW_STATUS_FAILURE
*
* @note       NA
*
* @end
*********************************************************************/

BVIEW_STATUS system_timer_set(timer_t timerId,int timeInMilliSec,TIMER_MODE mode);

#endif /* INCLUDE_SYSTEM_H */

