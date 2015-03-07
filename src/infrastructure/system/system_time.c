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

#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "system.h"
#include "openapps_log_api.h"

#define CONVERSION_FACTOR 1000000L

/*********************************************************************
* @brief         Function used to create and arm a timer
*
*
* @param[in]     handler        function pointer to handle the callback
*                               The callback function address is passed
*                               using a void * because the function declared
*                               in linux "sigev_notify_function" has a 
*                               return type "void"  
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


BVIEW_STATUS system_timer_add(void * handler,timer_t * timerId, int timeInMilliSec, TIMER_MODE mode, void * param)
{
  struct sigevent sigTime;
  struct timespec monotonic_time;
  clockid_t clock;
  int ret = BVIEW_STATUS_SUCCESS;
  /*Validate callback function pointers*/
  if (handler == NULL)
  {
    LOG_POST (BVIEW_LOG_ERROR, "Invalid handler during timer registration \r\n");
    return BVIEW_STATUS_FAILURE;
  }
  
  /*Validate pointer which stores timer id*/
  if (timerId == NULL)
  {
    LOG_POST (BVIEW_LOG_ERROR, "Invalid timer id during timer registration\r\n");
    return BVIEW_STATUS_FAILURE;
  }

  /*Populate the structures in-order to create the timer*/
  memset(&sigTime, 0,sizeof(struct sigevent)); 
  sigTime.sigev_notify = SIGEV_THREAD;
  sigTime.sigev_notify_attributes = NULL;
  sigTime.sigev_notify_function = handler;
  sigTime.sigev_value.sival_ptr = param;
  /*Monotonic clock is used if available else realtime clock is used*/ 
  if (!clock_gettime(CLOCK_MONOTONIC, &monotonic_time)) 
   {
     clock = CLOCK_MONOTONIC;
   } 
   else 
   {
     clock = CLOCK_REALTIME;
   }

  /*Timer is created*/
  if (timer_create(clock, &sigTime, timerId) == 0)
   {
     ret = system_timer_set(*timerId,timeInMilliSec,mode); 
   }
   else 
   {
     LOG_POST (BVIEW_LOG_ERROR, "Timer Creation Failed Error no : %d\r\n",errno);
     ret = BVIEW_STATUS_FAILURE;
   }
  return ret;
}

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


BVIEW_STATUS system_timer_delete(timer_t timerId)
{
  int ret = BVIEW_STATUS_SUCCESS;
  if ((timer_delete(timerId)) !=0)
  {
    ret = BVIEW_STATUS_FAILURE;
  }
  return ret;
}

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


BVIEW_STATUS system_timer_set(timer_t timerId,int timeInMilliSec,TIMER_MODE mode)
{
  int ret = BVIEW_STATUS_SUCCESS;
  struct itimerspec timerVal;
  timerVal.it_value.tv_sec = timeInMilliSec / 1000;
  timerVal.it_value.tv_nsec = (long)(timeInMilliSec % 1000) * CONVERSION_FACTOR;
  if (mode == PERIODIC_MODE)/*Timer is triggered periodically*/
  {
    timerVal.it_interval.tv_sec = timerVal.it_value.tv_sec;
    timerVal.it_interval.tv_nsec = timerVal.it_value.tv_nsec;
  }
  else/*Timer is triggered only once, but timer is not destroyed*/
  {
    timerVal.it_interval.tv_sec = 0;
    timerVal.it_interval.tv_nsec = 0;
  }
  /*Timer is set*/
  if (timer_settime(timerId, 0, &timerVal, NULL) != 0)
  {
    LOG_POST (BVIEW_LOG_ERROR, "Timer Initialization Failed, Error no : %d\r\n",errno);
    ret = BVIEW_STATUS_FAILURE;
  }
  return ret;
}
