
/*********************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename bhdapp_main.c 
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
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "bhdapp.h"
#include "bhdapp_json.h"
#include "bhdapp_debug.h"
#include "version.h" 

BHDAPP_CONFIG_t config;

int main(int argc, char** argv)
{
    pthread_t httpThread;
    int rv;

    printf("BroadViewBHDApp Version %s\n",RELEASE_STRING); 

    /* initialize configuration */
    rv = bhdapp_config_init(&config);
    _BHDAPP_ASSERT(rv == 0);

    /* setup logging */
    rv = bhdapp_logging_init();
    _BHDAPP_ASSERT(rv == 0);
    
    /* spawn off the agent communicator as a spearate thread */
    rv = pthread_create(&httpThread, NULL, (void *)&bhdapp_http_server_run, (void *)&config); 
    _BHDAPP_ASSERT(rv == 0);
    
    /* start the report receiver thread */
    bhdapp_communicate_with_agent(&config);

    pthread_exit(NULL);
    return (0);
}

