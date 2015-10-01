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

#include "ptapp.h"
#include "ptapp_debug.h"

#define _PTAPP_CONFIGFILE_LINE_MAX_LEN   256
#define _PTAPP_CONFIGFILE_READ_MODE      "r"
#define _PTAPP_CONFIGFILE_DELIMITER      "="

/******************************************************************
 * @brief  Sets the configuration, to defaults.
 *
 * @param[in]   config      config to be setup
 *                           
 * @retval   0  when configuration is initialized successfully
 *
 * @note     
 *********************************************************************/

static int ptapp_config_set_defaults(PTAPP_CONFIG_t *config)
{
    _PTAPP_LOG(_PTAPP_DEBUG_INFO, "PTAPP : Setting configuration to defaults \n");

    memset(config, 0, sizeof (PTAPP_CONFIG_t));

    /* setup default client IP */
    strncpy(&config->agentIp[0], PTAPP_CONFIG_PROPERTY_AGENT_IP_DEFAULT, PTAPP_MAX_IP_ADDR_LENGTH);

    /* setup default client port */
    config->agentPort = PTAPP_CONFIG_PROPERTY_AGENT_PORT_DEFAULT;

    /* setup default local port */
    config->localPort = PTAPP_CONFIG_PROPERTY_LOCAL_PORT_DEFAULT;


    _PTAPP_LOG(_PTAPP_DEBUG_INFO, "PTAPP : Using default configuration %s:%d <-->local:%d, \n",
                config->agentIp, config->agentPort, config->localPort);

    return 0;
}

/******************************************************************
 * @brief  Reads configuration from a file.
 *
 * @param[in]   config      config to be setup
 *                           
 * @retval   0  when configuration is initialized successfully
 * @retval  -1  on any error
 *
 * @note     
 *********************************************************************/

static int ptapp_config_read(PTAPP_CONFIG_t *config)
{
    FILE *configFile;
    char line[_PTAPP_CONFIGFILE_LINE_MAX_LEN] = { 0 };
    int numLinesRead = 0;

    /* dummy structure for validating IP address */
    struct sockaddr_in clientIpAddr;
    int temp;

    /* for string manipulation */
    char *property, *value;

    _PTAPP_LOG(_PTAPP_DEBUG_INFO, "PTAPP : Reading configuration from %s \n", PTAPP_CONFIG_FILE);

    memset(config, 0, sizeof (PTAPP_CONFIG_t));

    /* open the file. if file not available/readable, return appropriate error */
    configFile = fopen(PTAPP_CONFIG_FILE, _PTAPP_CONFIGFILE_READ_MODE);

    if (configFile == NULL)
    {
        _PTAPP_LOG(_PTAPP_DEBUG_ERROR,
                    "PTAPP : Configuration file %s not found:\n",
                    PTAPP_CONFIG_FILE);
        return -1;
    }

    /* read the lines one-by-one. if any of the lines is corrupted 
     * i.e., doesn't contain valid tokens, return error 
     */

    while (numLinesRead < 3)
    {
        memset (&line[0], 0, _PTAPP_CONFIGFILE_LINE_MAX_LEN);

        /* read one line from the file */
        property = fgets(&line[0], _PTAPP_CONFIGFILE_LINE_MAX_LEN, configFile);
        _PTAPP_ASSERT_CONFIG_FILE_ERROR(property != NULL);

        numLinesRead++;

        /* split the line into tokens, based on the file format */
        property = strtok(&line[0], _PTAPP_CONFIGFILE_DELIMITER);
        _PTAPP_ASSERT_CONFIG_FILE_ERROR(property != NULL);
        value = property + strlen(property) + 1;

        /* Is this token the agent IP address ?*/
        if (strcmp(property, PTAPP_CONFIG_PROPERTY_AGENT_IP) == 0)
        {
            /* truncate the newline characters */
            value[strlen(value) - 1] = 0;

            /* is this IP address valid ? */
            temp = inet_pton(AF_INET, value, &(clientIpAddr.sin_addr));
            _PTAPP_ASSERT_CONFIG_FILE_ERROR(temp > 0);

            /* copy the agent ip address */
            strncpy(&config->agentIp[0], value, PTAPP_MAX_IP_ADDR_LENGTH - 1);
            continue;
        }

        /* Is this token the agent port number ?*/
        if (strcmp(property, PTAPP_CONFIG_PROPERTY_AGENT_PORT) == 0)
        {
            /* is this port number valid ? */
            temp = strtol(value, NULL, 10);
            _PTAPP_ASSERT_CONFIG_FILE_ERROR( errno != ERANGE);

            /* copy the agent port number */
            config->agentPort = temp;
            continue;
        }

        /* Is this token the local port number ?*/
        if (strcmp(property, PTAPP_CONFIG_PROPERTY_LOCAL_PORT) == 0)
        {
            /* is this port number valid ? */
            temp = strtol(value, NULL, 10);
            _PTAPP_ASSERT_CONFIG_FILE_ERROR( errno != ERANGE);

            /* copy the client port number */
            config->localPort = temp;
            continue;
        }
        

        /* unknown property */
        _PTAPP_LOG(_PTAPP_DEBUG_ERROR,
                    "PTAPP : Unknown property in configuration file : %s \n",
                    property);
        fclose(configFile);
        return -1;
    }

    _PTAPP_LOG(_PTAPP_DEBUG_INFO, "PTAPP : Using default configuration %s:%d <-->local:%d \n",
                config->agentIp, config->agentPort, config->localPort);

    fclose(configFile);
    return 0;
}

/******************************************************************
 * @brief  Initializes configuration, reads it from file or assumes defaults.
 *
 * @param[in]   config      config to be setup
 *                           
 * @retval   0  when configuration is initialized successfully
 *
 * @note     
 *********************************************************************/
int ptapp_config_init(PTAPP_CONFIG_t *config)
{
    int status;

    /* aim to read */
    _PTAPP_LOG(_PTAPP_DEBUG_TRACE, "PTAPP : Configuring ...");

    status = ptapp_config_read(config);
    if (status != 0)
    {
        ptapp_config_set_defaults(config);
    }

    _PTAPP_LOG(_PTAPP_DEBUG_TRACE, "PTAPP : Configuration Complete");

    return 0;
}


