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

#ifndef INCLUDE_CONFIGURE_BST_FEATURE_H 
#define	INCLUDE_CONFIGURE_BST_FEATURE_H  

#ifdef	__cplusplus  
extern "C"
{
#endif  


/* Include Header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "broadview.h"
#include "json.h"

#include "cJSON.h"

typedef enum _bst_config_param_mask_pos_
{
  BST_CONFIG_PARAMS_ENABLE = 0,
  BST_CONFIG_PARAMS_SND_ASYNC_REP,
  BST_CONFIG_PARAMS_COLL_INTRVL,
  BST_CONFIG_PARAMS_STATS_UNITS,
  BST_CONFIG_PARAMS_TGR_RATE_LIMIT,
  BST_CONFIG_PARAMS_SND_SNAP_TGR,
  BST_CONFIG_PARAMS_TGR_RL_INTVL,
  BST_CONFIG_PARAMS_ASYNC_FULL_REP,
  BST_CONFIG_PARAMS_STATS_IN_PERCENT
}BST_CONFIG_PARAM_MASK_t;

/* Structure to pass API parameters to the BST APP */
typedef struct _bstjson_configure_bst_feature_
{
    int bstEnable;
    int sendAsyncReports;
    int collectionInterval;
    int statsInPercentage;
    int statUnitsInCells;
    int bstMaxTriggers;
    int sendSnapshotOnTrigger;
    int triggerTransmitInterval;
    int sendIncrementalReport;
    int configMask;
} BSTJSON_CONFIGURE_BST_FEATURE_t;


/* Function Prototypes */
BVIEW_STATUS bstjson_configure_bst_feature(void *cookie, char *jsonBuffer, int bufLength);
BVIEW_STATUS bstjson_configure_bst_feature_impl(void *cookie, int asicId, int id, BSTJSON_CONFIGURE_BST_FEATURE_t *pCommand);


#ifdef	__cplusplus  
}
#endif  

#endif /* INCLUDE_CONFIGURE_BST_FEATURE_H */ 

