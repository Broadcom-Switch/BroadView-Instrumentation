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
} BSTJSON_CONFIGURE_BST_FEATURE_t;


/* Function Prototypes */
BVIEW_STATUS bstjson_configure_bst_feature(void *cookie, char *jsonBuffer, int bufLength);
BVIEW_STATUS bstjson_configure_bst_feature_impl(void *cookie, int asicId, int id, BSTJSON_CONFIGURE_BST_FEATURE_t *pCommand);


#ifdef	__cplusplus  
}
#endif  

#endif /* INCLUDE_CONFIGURE_BST_FEATURE_H */ 

