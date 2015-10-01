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
#include <pthread.h>

#include <arpa/inet.h>

#include "bstapp.h"
#include "bstapp_debug.h"

static pthread_mutex_t logLock;

/******************************************************************
 * @brief  Initializes Logging
 *
 * @retval   0  when logging is initialized successfully
 *
 * @note     
 *********************************************************************/
int bstapp_logging_init(void)
{
    int rv = 0;
    FILE *fp;

    /* initialize the mutex*/
    rv = pthread_mutex_init(&logLock, NULL);
    _BSTAPP_ASSERT_NET_ERROR( (rv == 0), "BSTAPP : Error creating logging mutex \n");

    /* truncate the logging file if already available */
    fp = fopen(BSTAPP_COMMUNICATION_LOG_FILE, "w");
    if (fp != NULL)
    {
        fclose(fp);
    }

    return 0;
}

/******************************************************************
 * @brief  Logs a message
 *
 * @retval   0  when message is logged successfully
 *
 * @note     
 *********************************************************************/
int bstapp_message_log(char *message, int length, bool isFromAgent)
{
    FILE *fp = NULL;
    char timeString[BSTAPP_MAX_STRING_LENGTH] = { 0 };
    time_t logtime;
    struct tm *timeinfo;
    int i = 0;

    time(&logtime);
    timeinfo = localtime(&logtime);
    strftime(timeString, BSTAPP_MAX_STRING_LENGTH, "%Y-%m-%d %H:%M:%S ", timeinfo);

    pthread_mutex_lock(&logLock);
    fp = fopen(BSTAPP_COMMUNICATION_LOG_FILE, "a");

    if (fp == NULL)
    {
        _BSTAPP_LOG(_BSTAPP_DEBUG_ERROR, "Log : Unable to open file for logging [%d:%s] \n",
                    errno, strerror(errno));
        pthread_mutex_unlock(&logLock);
        return -1;
    }

    fputs(timeString, fp);

    if (isFromAgent)
    {
        fputs("Message from Agent \n", fp);
    }
    else
    {
        fputs("Message to Agent \n", fp);
    }

    for (i = 0; i < length; i++)
    {
        fputc(message[i], fp);
    }

    fputs("\n", fp);
    fclose(fp);

    pthread_mutex_unlock(&logLock);
    return 0;
}

