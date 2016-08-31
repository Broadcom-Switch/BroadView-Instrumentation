
/*********************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename bhdapp_log.c 
  *
  * @purpose BroadView BHD reference application 
  *
  * @component Black hole detection 
  *
  * @comments
  *
  * @create 3/17/2016
  *
  * @author 
  * @end
  *
  **********************************************************************/
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <arpa/inet.h>

#include "bhdapp.h"
#include "bhdapp_debug.h"


static pthread_mutex_t logLock;

/******************************************************************
 * @brief  Initializes Logging
 *
 * @retval   0  when logging is initialized successfully
 *
 * @note     
 *********************************************************************/
int bhdapp_logging_init(void)
{
    int rv = 0;
    FILE *fp;

    /* initialize the mutex*/
    rv = pthread_mutex_init(&logLock, NULL);
    _BHDAPP_ASSERT_NET_ERROR( (rv == 0), "BHDAPP : Error creating logging mutex \n");

    /* truncate the logging files if already available */
    fp = fopen(BHDAPP_COMMUNICATION_LOG_FILE_NEW, "w");
    if (fp != NULL)
    {
        fclose(fp);
    }

    fp = fopen(BHDAPP_COMMUNICATION_LOG_FILE_OLD, "w");
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
int bhdapp_message_log(char *message, int length, bool isFromAgent)
{
    FILE *fp = NULL;
    char timeString[BHDAPP_MAX_STRING_LENGTH] = { 0 };
    time_t logtime;
    struct tm *timeinfo;
    int i = 0;
    struct stat fileStat;

    time(&logtime);
    timeinfo = localtime(&logtime);
    strftime(timeString, BHDAPP_MAX_STRING_LENGTH, "%Y-%m-%d %H:%M:%S ", timeinfo);

    pthread_mutex_lock(&logLock);
    fp = fopen(BHDAPP_COMMUNICATION_LOG_FILE_NEW, "a");

    if (fp == NULL)
    {
        _BHDAPP_LOG(_BHDAPP_DEBUG_ERROR, "Log : Unable to open file for logging [%d:%s] \n",
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

    if (stat(BHDAPP_COMMUNICATION_LOG_FILE_NEW, &fileStat) == 0)
    {
      /* Check the size of file */
      if (BHDAPP_COMMUNICATION_LOG_MAX_FILE_SIZE <= fileStat.st_size)
      {
        /* Remove old log */
        if (0 > remove(BHDAPP_COMMUNICATION_LOG_FILE_OLD))
        {
          /* failed to remove the existing file */
        _BHDAPP_LOG(_BHDAPP_DEBUG_ERROR, "Log : Unable to remove old file for logging [%d:%s] \n",
                    errno, strerror(errno));
        }
        /* Rename old to new */
        if (0 > rename(BHDAPP_COMMUNICATION_LOG_FILE_NEW, BHDAPP_COMMUNICATION_LOG_FILE_OLD))
        {
          _BHDAPP_LOG(_BHDAPP_DEBUG_ERROR, "Log : Unable to rename new file to old file for logging [%d:%s] \n",
              errno, strerror(errno));
        }
      }
    }
    pthread_mutex_unlock(&logLock);
    return 0;
}

