/*********************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename bhdapp_config.c
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
#include <stdlib.h>

#include <arpa/inet.h>

#include "bhdapp.h"
#include "bhdapp_debug.h"

#define _BHDAPP_CONFIGFILE_LINE_MAX_LEN   256
#define _BHDAPP_CONFIGFILE_READ_MODE      "r"
#define _BHDAPP_CONFIGFILE_DELIMITER      "="

/******************************************************************
 * @brief  Sets the configuration, to defaults.
 *
 * @param[in]   config      config to be setup
 *                           
 * @retval   0  when configuration is initialized successfully
 *
 * @note     
 *********************************************************************/

static int bhdapp_config_set_defaults(BHDAPP_CONFIG_t *config)
{
    _BHDAPP_LOG(_BHDAPP_DEBUG_INFO, "BHDAPP : Setting configuration to defaults \n");

    memset(config, 0, sizeof (BHDAPP_CONFIG_t));

    /* setup default client IP */
    strncpy(&config->agentIp[0], BHDAPP_CONFIG_PROPERTY_AGENT_IP_DEFAULT, BHDAPP_MAX_IP_ADDR_LENGTH);

    /* setup default client port */
    config->agentPort = BHDAPP_CONFIG_PROPERTY_AGENT_PORT_DEFAULT;

    /* setup default local port */
    config->localPort = BHDAPP_CONFIG_PROPERTY_LOCAL_PORT_DEFAULT;


    _BHDAPP_LOG(_BHDAPP_DEBUG_INFO, "BHDAPP : Using default configuration %s:%d <-->local:%d, \n",
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

static int bhdapp_config_read(BHDAPP_CONFIG_t *config)
{
    FILE *configFile;
    char line[_BHDAPP_CONFIGFILE_LINE_MAX_LEN] = { 0 };
    int numLinesRead = 0;

    /* dummy structure for validating IP address */
    struct sockaddr_in clientIpAddr;
    int temp;

    /* for string manipulation */
    char *property, *value;

    _BHDAPP_LOG(_BHDAPP_DEBUG_INFO, "BHDAPP : Reading configuration from %s \n", BHDAPP_CONFIG_FILE);

    memset(config, 0, sizeof (BHDAPP_CONFIG_t));

    /* open the file. if file not available/readable, return appropriate error */
    configFile = fopen(BHDAPP_CONFIG_FILE, _BHDAPP_CONFIGFILE_READ_MODE);

    if (configFile == NULL)
    {
        _BHDAPP_LOG(_BHDAPP_DEBUG_ERROR,
                    "BHDAPP : Configuration file %s not found:\n",
                    BHDAPP_CONFIG_FILE);
        return -1;
    }

    /* read the lines one-by-one. if any of the lines is corrupted 
     * i.e., doesn't contain valid tokens, return error 
     */

    while (numLinesRead < 3)
    {
        memset (&line[0], 0, _BHDAPP_CONFIGFILE_LINE_MAX_LEN);

        /* read one line from the file */
        property = fgets(&line[0], _BHDAPP_CONFIGFILE_LINE_MAX_LEN, configFile);
        _BHDAPP_ASSERT_CONFIG_FILE_ERROR(property != NULL);

        numLinesRead++;

        /* split the line into tokens, based on the file format */
        property = strtok(&line[0], _BHDAPP_CONFIGFILE_DELIMITER);
        _BHDAPP_ASSERT_CONFIG_FILE_ERROR(property != NULL);
        value = property + strlen(property) + 1;

        /* Is this token the agent IP address ?*/
        if (strcmp(property, BHDAPP_CONFIG_PROPERTY_AGENT_IP) == 0)
        {
            /* truncate the newline characters */
            value[strlen(value) - 1] = 0;

            /* is this IP address valid ? */
            temp = inet_pton(AF_INET, value, &(clientIpAddr.sin_addr));
            _BHDAPP_ASSERT_CONFIG_FILE_ERROR(temp > 0);

            /* copy the agent ip address */
            strncpy(&config->agentIp[0], value, BHDAPP_MAX_IP_ADDR_LENGTH - 1);
            continue;
        }

        /* Is this token the agent port number ?*/
        if (strcmp(property, BHDAPP_CONFIG_PROPERTY_AGENT_PORT) == 0)
        {
            /* is this port number valid ? */
            temp = strtol(value, NULL, 10);
            _BHDAPP_ASSERT_CONFIG_FILE_ERROR( errno != ERANGE);

            /* copy the agent port number */
            config->agentPort = temp;
            continue;
        }

        /* Is this token the local port number ?*/
        if (strcmp(property, BHDAPP_CONFIG_PROPERTY_LOCAL_PORT) == 0)
        {
            /* is this port number valid ? */
            temp = strtol(value, NULL, 10);
            _BHDAPP_ASSERT_CONFIG_FILE_ERROR( errno != ERANGE);

            /* copy the client port number */
            config->localPort = temp;
            continue;
        }
        

        /* unknown property */
        _BHDAPP_LOG(_BHDAPP_DEBUG_ERROR,
                    "BHDAPP : Unknown property in configuration file : %s \n",
                    property);
        fclose(configFile);
        return -1;
    }

    _BHDAPP_LOG(_BHDAPP_DEBUG_INFO, "BHDAPP : Using default configuration %s:%d <-->local:%d \n",
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
int bhdapp_config_init(BHDAPP_CONFIG_t *config)
{
    int status;

    /* aim to read */
    _BHDAPP_LOG(_BHDAPP_DEBUG_TRACE, "BHDAPP : Configuring ...");

    status = bhdapp_config_read(config);
    if (status != 0)
    {
        bhdapp_config_set_defaults(config);
    }

    _BHDAPP_LOG(_BHDAPP_DEBUG_TRACE, "BHDAPP : Configuration Complete");

    return 0;
}


