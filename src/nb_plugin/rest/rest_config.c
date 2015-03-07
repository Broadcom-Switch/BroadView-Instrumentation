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
#include <stdlib.h>

#include <arpa/inet.h>

#include "broadview.h"

#include "rest.h"

#define _REST_CONFIGFILE_LINE_MAX_LEN   256
#define _REST_CONFIGFILE_READ_MODE      "r"
#define _REST_CONFIGFILE_DELIMITER      "="

#define _REST_ASSERT_CONFIG_FILE_ERROR(condition) do { \
    if (!(condition)) { \
        _REST_LOG(_REST_DEBUG_ERROR, \
                    "REST (%s:%d) Unrecognized Config File format, may be corrupted. Errno : %s  \n", \
                    __func__, __LINE__, strerror(errno)); \
                        fclose(configFile); \
        return (BVIEW_STATUS_FAILURE); \
    } \
} while(0)

/******************************************************************
 * @brief  Sets the configuration, to defaults.
 *
 * @param[in]   rest      REST context for operation
 *                           
 * @retval   BVIEW_STATUS_SUCCESS  when configuration is initialized successfully
 *
 * @note     
 *********************************************************************/

static BVIEW_STATUS rest_config_set_defaults(REST_CONTEXT_t *rest)
{
    _REST_LOG(_REST_DEBUG_INFO, "REST : Setting configuration to defaults \n");

    memset(&rest->config, 0, sizeof (REST_CONFIG_t));

    /* setup default client IP */
    strncpy(&rest->config.clientIp[0], REST_CONFIG_PROPERTY_CLIENT_IP_DEFAULT, REST_MAX_IP_ADDR_LENGTH);

    /* setup default client port */
    rest->config.clientPort = REST_CONFIG_PROPERTY_CLIENT_PORT_DEFAULT;

    /* setup default local port */
    rest->config.localPort = REST_CONFIG_PROPERTY_LOCAL_PORT_DEFAULT;

    _REST_LOG(_REST_DEBUG_INFO, "REST : Using default configuration %s:%d <-->local:%d \n",
              rest->config.clientIp, rest->config.clientPort, rest->config.localPort);

    return BVIEW_STATUS_SUCCESS;
}

/******************************************************************
 * @brief  Reads configuration from a file.
 *
 * @param[in]   rest      REST context for operation
 *                           
 * @retval   BVIEW_STATUS_SUCCESS  when configuration is initialized successfully
 * @retval   BVIEW_STATUS_RESOURCE_NOT_AVAILABLE  if config file is not readable
 * @retval   BVIEW_STATUS_FAILURE if config file contents are invalid
 *
 * @note     
 *********************************************************************/

static BVIEW_STATUS rest_config_read(REST_CONTEXT_t *rest)
{
    FILE *configFile;
    char line[_REST_CONFIGFILE_LINE_MAX_LEN] = { 0 };
    int numLinesRead = 0;

    /* dummy structure for validating IP address */
    struct sockaddr_in clientIpAddr;
    int temp;

    /* for string manipulation */
    char *property, *value;

    _REST_LOG(_REST_DEBUG_INFO, "REST : Reading configuration from %s \n", REST_CONFIG_FILE);

    memset(&rest->config, 0, sizeof (REST_CONFIG_t));

    /* open the file. if file not available/readable, return appropriate error */
    configFile = fopen(REST_CONFIG_FILE, _REST_CONFIGFILE_READ_MODE);

    if (configFile == NULL)
    {
        _REST_LOG(_REST_DEBUG_ERROR,
                  "REST : Configuration file %s not found: \n",
                  REST_CONFIG_FILE);
        return BVIEW_STATUS_RESOURCE_NOT_AVAILABLE;
    }

    /* read the lines one-by-one. if any of the lines is corrupted 
     * i.e., doesn't contain valid tokens, return error 
     */

    while (numLinesRead < 3)
    {
        memset (&line[0], 0, _REST_CONFIGFILE_LINE_MAX_LEN);

        /* read one line from the file */
        property = fgets(&line[0], _REST_CONFIGFILE_LINE_MAX_LEN, configFile);
        _REST_ASSERT_CONFIG_FILE_ERROR(property != NULL);

        numLinesRead++;

        /* split the line into tokens, based on the file format */
        property = strtok(&line[0], _REST_CONFIGFILE_DELIMITER);
        _REST_ASSERT_CONFIG_FILE_ERROR(property != NULL);
        value = property + strlen(property) + 1;

        /* Is this token the client IP address ?*/
        if (strcmp(property, REST_CONFIG_PROPERTY_CLIENT_IP) == 0)
        {
            /* truncate the newline characters */
            value[strlen(value) - 1] = 0;

            /* is this IP address valid ? */
            temp = inet_pton(AF_INET, value, &(clientIpAddr.sin_addr));
            _REST_ASSERT_CONFIG_FILE_ERROR(temp > 0);

            /* copy the client ip address */
            strncpy(&rest->config.clientIp[0], value, REST_MAX_IP_ADDR_LENGTH - 1);
            continue;
        }

        /* Is this token the client port number ?*/
        if (strcmp(property, REST_CONFIG_PROPERTY_CLIENT_PORT) == 0)
        {
            /* is this port number valid ? */
            temp = strtol(value, NULL, 10);
            _REST_ASSERT_CONFIG_FILE_ERROR( errno != ERANGE);

            /* copy the client port number */
            rest->config.clientPort = temp;
            continue;
        }

        /* Is this token the local port number ?*/
        if (strcmp(property, REST_CONFIG_PROPERTY_LOCAL_PORT) == 0)
        {
            /* is this port number valid ? */
            temp = strtol(value, NULL, 10);
            _REST_ASSERT_CONFIG_FILE_ERROR( errno != ERANGE);

            /* copy the client port number */
            rest->config.localPort = temp;
            continue;
        }

        /* unknown property */
        _REST_LOG(_REST_DEBUG_ERROR,
                  "REST : Unknown property in configuration file : %s \n",
                  property);

        fclose(configFile);
        return BVIEW_STATUS_FAILURE;
    }

    _REST_LOG(_REST_DEBUG_INFO, "REST : Using configuration %s:%d <-->local:%d \n",
              rest->config.clientIp, rest->config.clientPort, rest->config.localPort);

    fclose(configFile);

    return BVIEW_STATUS_SUCCESS;
}

/******************************************************************
 * @brief  Initializes configuration, reads it from file or assumes defaults.
 *
 * @param[in]   rest      REST context for operation
 *                           
 * @retval   BVIEW_STATUS_SUCCESS  when configuration is initialized successfully
 *
 * @note     
 *********************************************************************/
BVIEW_STATUS rest_config_init(REST_CONTEXT_t *rest)
{
    BVIEW_STATUS status;

    /* aim to read */
    _REST_LOG(_REST_DEBUG_TRACE, "REST : Configuring ...");

    status = rest_config_read(rest);
    if (status != BVIEW_STATUS_SUCCESS)
    {
        rest_config_set_defaults(rest);
    }

    _REST_LOG(_REST_DEBUG_TRACE, "REST : Configuration Complete");

    return BVIEW_STATUS_SUCCESS;
}



