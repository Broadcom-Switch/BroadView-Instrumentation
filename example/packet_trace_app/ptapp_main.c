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
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "ptapp.h"
#include "ptapp_json.h"
#include "ptapp_debug.h"
#include "version.h" 

PTAPP_CONFIG_t config;

int main(int argc, char** argv)
{
    pthread_t httpThread;
    int rv;

    printf("BroadViewPacketTraceApp Version %s\n",RELEASE_STRING); 

    /* initialize configuration */
    rv = ptapp_config_init(&config);
    _PTAPP_ASSERT(rv == 0);

    /* setup logging */
    rv = ptapp_logging_init();
    _PTAPP_ASSERT(rv == 0);
    
    /* spawn off the agent communicator as a spearate thread */
    rv = pthread_create(&httpThread, NULL, (void *)&ptapp_http_server_run, (void *)&config); 
    _PTAPP_ASSERT(rv == 0);
    
    /* start the report receiver thread */
    ptapp_communicate_with_agent(&config);

    pthread_exit(NULL);
    return (0);
}

