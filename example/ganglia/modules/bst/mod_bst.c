
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

#include <gm_metric.h>

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "gm_file.h"
#include "libmetrics.h"

#include <apr_tables.h>
#include <apr_strings.h>
#include "bstapp_table.h"

/*
 * Declare ourselves so the configuration routines can find and know us.
 * We'll fill it in at the end of the module.
 */
mmodule bst_module;



static apr_pool_t *pool;

static apr_array_header_t *metric_info = NULL;

extern BSTAPP_BID_PARAMS_t  bid_tab_params[BSTAPP_STAT_ID_MAX_COUNT]; 




/* Initialize the give metric by allocating the per metric data
   structure and inserting a metric definition for each cpu found
*/
static int bst_init_metric (apr_pool_t *p, apr_array_header_t *ar)
{
    int i;
    Ganglia_25metric *gmi;
    unsigned int bid = 0;
    unsigned int row_index = 0;
    unsigned int col_index = 0;
    unsigned int asic  = 0;
    char stat_name[256] = {0};

    for (bid = 0; bid < BSTAPP_STAT_ID_MAX_COUNT; bid++)  
    { 
        for (row_index = 0; row_index < bid_tab_params[bid].num_of_rows; row_index++)
        {

            for (col_index = 0; col_index < bid_tab_params[bid].num_of_columns; col_index++)
            {
                bst_bid_port_index_to_key(asic, bid_tab_params[bid].bid, row_index, col_index,
                                                   stat_name, sizeof(stat_name));       
                gmi = apr_array_push(ar);

                /* gmi->key will be automatically assigned by gmond */
                gmi->name = apr_psprintf (p, "%s", stat_name);
                gmi->tmax = 90;
                gmi->type = GANGLIA_VALUE_DOUBLE;
                gmi->units = apr_pstrdup(p, "KB");
                gmi->slope = apr_pstrdup(p, "zero");
                gmi->fmt = apr_pstrdup(p, "%.0lf");
                gmi->msg_size = UDP_HEADER_SIZE+8;
                gmi->desc = apr_pstrdup(p, gmi->name);        
            }

        }
    }
    
    return 0;
}


static int bst_metric_init (apr_pool_t *p)
{
    Ganglia_25metric *gmi;
    int i = 0;
 
    /* Allocate a pool that will be used by this module */
    apr_pool_create(&pool, p);

    metric_info = apr_array_make(pool, 2, sizeof(Ganglia_25metric));

    /* Initialize each metric */
    bst_init_metric (pool, metric_info); 
    /* Add a terminator to the array and replace the empty static metric definition 
        array with the dynamic array that we just created 
    */
    gmi = apr_array_push(metric_info);
    memset (gmi, 0, sizeof(*gmi));

    bst_module.metrics_info = (Ganglia_25metric *)metric_info->elts;

    for (i = 0; bst_module.metrics_info[i].name != NULL; i++) {
        /* Initialize the metadata storage for each of the metrics and then
         *  store one or more key/value pairs.  The define MGROUPS defines
         *  the key for the grouping attribute. */
        MMETRIC_INIT_METADATA(&(bst_module.metrics_info[i]),p);
        MMETRIC_ADD_METADATA(&(bst_module.metrics_info[i]),MGROUP,"BST");
    }

    
    return 0;
}

static void bst_metric_cleanup ( void )
{
}

/*
static g_val_t bst_metric_handler (int metric_index)*/
g_val_t bst_metric_handler (int metric_index)
{
    g_val_t val;
    int bid;
    int port;
    int index;
    int asic = 0;
    uint64_t stat;
    
    /* get bid, port, index from metric name   */
    bst_key_to_bid_port_index(asic, bst_module.metrics_info[metric_index].name, 
                                         &bid, &port, &index);

    bst_stat_get(asic, port, index, bid, 0xff, &stat);
    val.d = (double)stat;
    return val;
}


/* BST module */
mmodule bst_module =
{
    STD_MMODULE_STUFF,
    bst_metric_init,
    bst_metric_cleanup,
    NULL, /* defined dynamically */
    bst_metric_handler,
};

