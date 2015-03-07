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

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "broadview.h"
#include "rest.h"

/******************************************************************
 * @brief  initialize sessions 
 *
 * @param[in]   rest      REST context for operation
 *
 * @retval   BVIEW_STATUS_SUCCESS on successful initialization
 * @retval   BVIEW_STATUS_FAILURE otherwise
 * 
 * @note     
 *********************************************************************/
BVIEW_STATUS rest_sessions_init(REST_CONTEXT_t *context)
{
    int i = 0;
    REST_SESSION_t *session;

    /* Just basic initialization */

    for (i = 0; i < REST_MAX_SESSIONS; i++)
    {
        session = &context->sessions[i];
        memset(session, 0, sizeof (REST_SESSION_t));
        session->inUse = false;
    }

    return BVIEW_STATUS_SUCCESS;
}

/******************************************************************
 * @brief  allocates an available session (returns the index) 
 *
 * @param[in]   rest      REST context for operation
 *
 * @retval   BVIEW_STATUS_SUCCESS on successful allocation
 * @retval   BVIEW_STATUS_RESOURCE_NOT_AVAILABLE otherwise
 * 
 * @note     
 *********************************************************************/
BVIEW_STATUS rest_allocate_session(REST_CONTEXT_t *context, int *sessionId)
{
    int i = 0;
    for (i = 0; i < REST_MAX_SESSIONS; i++)
    {
        if (context->sessions[i].inUse == false)
        {
            *sessionId = i;
            memset(&context->sessions[i], 0, sizeof (REST_SESSION_t));
            return BVIEW_STATUS_SUCCESS;
        }
    }

    return BVIEW_STATUS_RESOURCE_NOT_AVAILABLE;
}

/******************************************************************
 * @brief  Checks if the supplied session is valid 
 *
 * @param[in]   session      session to be validated
 *
 *********************************************************************/
BVIEW_STATUS rest_session_validate(REST_CONTEXT_t *context, REST_SESSION_t *session)
{
    int i = 0;
    
    for (i = 0; i < REST_MAX_SESSIONS; i++)
    {
        if (&context->sessions[i] == session)
            return BVIEW_STATUS_SUCCESS;
    }
    
    return BVIEW_STATUS_RESOURCE_NOT_AVAILABLE;
}

/******************************************************************
 * @brief  dump a session on console/log
 *
 * @param[in]   session      data to be dumped
 *
 *********************************************************************/
void rest_session_dump(REST_SESSION_t *session)
{
    _REST_LOG(_REST_DEBUG_TRACE, "Session : HTTPMethod : %s - REST Method : %s - URL : %s - Length %d \n",
              session->httpMethod, session->restMethod, session->url, session->length);
}


