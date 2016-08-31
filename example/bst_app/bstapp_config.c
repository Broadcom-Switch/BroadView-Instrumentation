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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <arpa/inet.h>

#include "bstapp.h"
#include "bstapp_debug.h"

#define _BSTAPP_CONFIGFILE_LINE_MAX_LEN   256
#define _BSTAPP_CONFIGFILE_READ_MODE      "r"
#define _BSTAPP_CONFIGFILE_DELIMITER      "="

/******************************************************************
 * @brief  Sets the configuration, to defaults.
 *
 * @param[in]   config      config to be setup
 *                           
 * @retval   0  when configuration is initialized successfully
 *
 * @note     
 *********************************************************************/

static int bstapp_config_set_defaults(BSTAPP_CONFIG_t *config)
{
    _BSTAPP_LOG(_BSTAPP_DEBUG_INFO, "BSTAPP : Setting configuration to defaults \n");

    memset(config, 0, sizeof (BSTAPP_CONFIG_t));

    /* setup default client IP */
    strncpy(&config->agentIp[0], BSTAPP_CONFIG_PROPERTY_AGENT_IP_DEFAULT, BSTAPP_MAX_IP_ADDR_LENGTH);

    /* setup default client port */
    config->agentPort = BSTAPP_CONFIG_PROPERTY_AGENT_PORT_DEFAULT;

    /* setup default local port */
    config->localPort = BSTAPP_CONFIG_PROPERTY_LOCAL_PORT_DEFAULT;

    /* maximum reports */
    config->maxReports = BSTAPP_CONFIG_PROPERTY_MAX_REPORTS_DEFAULT;

    _BSTAPP_LOG(_BSTAPP_DEBUG_INFO, "BSTAPP : Using default configuration %s:%d <-->local:%d, Max %d Reports \n",
                config->agentIp, config->agentPort, config->localPort, config->maxReports);

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

static int bstapp_config_read(BSTAPP_CONFIG_t *config)
{
    FILE *configFile;
    char line[_BSTAPP_CONFIGFILE_LINE_MAX_LEN] = { 0 };
    int numLinesRead = 0;

    /* dummy structure for validating IP address */
    struct sockaddr_in clientIpAddr;
    int temp;

    /* for string manipulation */
    char *property, *value;

    _BSTAPP_LOG(_BSTAPP_DEBUG_INFO, "BSTAPP : Reading configuration from %s \n", BSTAPP_CONFIG_FILE);

    memset(config, 0, sizeof (BSTAPP_CONFIG_t));

    /* open the file. if file not available/readable, return appropriate error */
    configFile = fopen(BSTAPP_CONFIG_FILE, _BSTAPP_CONFIGFILE_READ_MODE);

    if (configFile == NULL)
    {
        _BSTAPP_LOG(_BSTAPP_DEBUG_ERROR,
                    "BSTAPP : Configuration file %s not found:\n",
                    BSTAPP_CONFIG_FILE);
        return -1;
    }

    /* read the lines one-by-one. if any of the lines is corrupted 
     * i.e., doesn't contain valid tokens, return error 
     */

    while (numLinesRead < 4)
    {
        memset (&line[0], 0, _BSTAPP_CONFIGFILE_LINE_MAX_LEN);

        /* read one line from the file */
        property = fgets(&line[0], _BSTAPP_CONFIGFILE_LINE_MAX_LEN, configFile);
        _BSTAPP_ASSERT_CONFIG_FILE_ERROR(property != NULL);

        numLinesRead++;

        /* split the line into tokens, based on the file format */
        property = strtok(&line[0], _BSTAPP_CONFIGFILE_DELIMITER);
        _BSTAPP_ASSERT_CONFIG_FILE_ERROR(property != NULL);
        value = property + strlen(property) + 1;

        /* Is this token the agent IP address ?*/
        if (strcmp(property, BSTAPP_CONFIG_PROPERTY_AGENT_IP) == 0)
        {
            /* truncate the newline characters */
            value[strlen(value) - 1] = 0;

            /* is this IP address valid ? */
            temp = inet_pton(AF_INET, value, &(clientIpAddr.sin_addr));
            _BSTAPP_ASSERT_CONFIG_FILE_ERROR(temp > 0);

            /* copy the agent ip address */
            strncpy(&config->agentIp[0], value, BSTAPP_MAX_IP_ADDR_LENGTH - 1);
            continue;
        }

        /* Is this token the agent port number ?*/
        if (strcmp(property, BSTAPP_CONFIG_PROPERTY_AGENT_PORT) == 0)
        {
            /* is this port number valid ? */
            temp = strtol(value, NULL, 10);
            _BSTAPP_ASSERT_CONFIG_FILE_ERROR( errno != ERANGE);

            /* copy the agent port number */
            config->agentPort = temp;
            continue;
        }

        /* Is this token the local port number ?*/
        if (strcmp(property, BSTAPP_CONFIG_PROPERTY_LOCAL_PORT) == 0)
        {
            /* is this port number valid ? */
            temp = strtol(value, NULL, 10);
            _BSTAPP_ASSERT_CONFIG_FILE_ERROR( errno != ERANGE);

            /* copy the client port number */
            config->localPort = temp;
            continue;
        }
        
        /* Is this token the Max Reports  ?*/
        if (strcmp(property, BSTAPP_CONFIG_PROPERTY_MAX_REPORTS) == 0)
        {
            /* is this port number valid ? */
            temp = strtol(value, NULL, 10);
            _BSTAPP_ASSERT_CONFIG_FILE_ERROR( errno != ERANGE);

            /* copy the client port number */
            config->maxReports = temp;
            continue;
        }

        /* unknown property */
        _BSTAPP_LOG(_BSTAPP_DEBUG_ERROR,
                    "BSTAPP : Unknown property in configuration file : %s \n",
                    property);
        fclose(configFile);
        return -1;
    }

    _BSTAPP_LOG(_BSTAPP_DEBUG_INFO, "BSTAPP : Using default configuration %s:%d <-->local:%d, Max %d Reports \n",
                config->agentIp, config->agentPort, config->localPort, config->maxReports);

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
int bstapp_config_init(BSTAPP_CONFIG_t *config)
{
    int status;

    /* aim to read */
    _BSTAPP_LOG(_BSTAPP_DEBUG_TRACE, "BSTAPP : Configuring ...");

    status = bstapp_config_read(config);
    if (status != 0)
    {
        bstapp_config_set_defaults(config);
    }

    _BSTAPP_LOG(_BSTAPP_DEBUG_TRACE, "BSTAPP : Configuration Complete");

    return 0;
}


