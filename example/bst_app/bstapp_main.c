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
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "bstapp.h"
#include "bstapp_debug.h"
#include "version.h"

BSTAPP_CONFIG_t config;

int main(int argc, char** argv)
{
    pthread_t httpThread;
    int rv;

    printf("BroadViewBstApp Version %s\n",RELEASE_STRING);

    /* initialize configuration */
    rv = bstapp_config_init(&config);
    _BSTAPP_ASSERT(rv == 0);

    /* setup logging */
    rv = bstapp_logging_init();
    _BSTAPP_ASSERT(rv == 0);
    
    /* spawn off the agent communicator as a spearate thread */
    rv = pthread_create(&httpThread, NULL, (void *)&bstapp_communicate_with_agent, (void *)&config);
    _BSTAPP_ASSERT(rv == 0);
    
    /* start the report receiver thread */
    bstapp_http_server_run(&config);

    pthread_exit(NULL);
    return (0);
}

