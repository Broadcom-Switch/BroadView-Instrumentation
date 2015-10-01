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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <arpa/inet.h>

#include "ptapp.h"
#include "ptapp_debug.h"


static pthread_mutex_t logLock;

/******************************************************************
 * @brief  Initializes Logging
 *
 * @retval   0  when logging is initialized successfully
 *
 * @note     
 *********************************************************************/
int ptapp_logging_init(void)
{
    int rv = 0;
    FILE *fp;

    /* initialize the mutex*/
    rv = pthread_mutex_init(&logLock, NULL);
    _PTAPP_ASSERT_NET_ERROR( (rv == 0), "PTAPP : Error creating logging mutex \n");

    /* truncate the logging files if already available */
    fp = fopen(PTAPP_COMMUNICATION_LOG_FILE_NEW, "w");
    if (fp != NULL)
    {
        fclose(fp);
    }

    fp = fopen(PTAPP_COMMUNICATION_LOG_FILE_OLD, "w");
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
int ptapp_message_log(char *message, int length, bool isFromAgent)
{
    FILE *fp = NULL;
    char timeString[PTAPP_MAX_STRING_LENGTH] = { 0 };
    time_t logtime;
    struct tm *timeinfo;
    int i = 0;
    struct stat fileStat;

    time(&logtime);
    timeinfo = localtime(&logtime);
    strftime(timeString, PTAPP_MAX_STRING_LENGTH, "%Y-%m-%d %H:%M:%S ", timeinfo);

    pthread_mutex_lock(&logLock);
    fp = fopen(PTAPP_COMMUNICATION_LOG_FILE_NEW, "a");

    if (fp == NULL)
    {
        _PTAPP_LOG(_PTAPP_DEBUG_ERROR, "Log : Unable to open file for logging [%d:%s] \n",
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

    if (stat(PTAPP_COMMUNICATION_LOG_FILE_NEW, &fileStat) == 0)
    {
      /* Check the size of file */
      if (PTAPP_COMMUNICATION_LOG_MAX_FILE_SIZE <= fileStat.st_size)
      {
        /* Remove old log */
        if (0 > remove(PTAPP_COMMUNICATION_LOG_FILE_OLD))
        {
          /* failed to remove the existing file */
        _PTAPP_LOG(_PTAPP_DEBUG_ERROR, "Log : Unable to remove old file for logging [%d:%s] \n",
                    errno, strerror(errno));
        }
        /* Rename old to new */
        if (0 > rename(PTAPP_COMMUNICATION_LOG_FILE_NEW, PTAPP_COMMUNICATION_LOG_FILE_OLD))
        {
          _PTAPP_LOG(_PTAPP_DEBUG_ERROR, "Log : Unable to rename new file to old file for logging [%d:%s] \n",
              errno, strerror(errno));
        }
      }
    }
    pthread_mutex_unlock(&logLock);
    return 0;
}

