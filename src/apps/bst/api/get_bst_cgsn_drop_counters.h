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

#ifndef INCLUDE_GET_BST_CGSN_DROP_COUNTERS_H 
#define	INCLUDE_GET_BST_BST_CGSN_DROP_COUNTERS_H  

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

#define BVIEW_BST_CGSN_COLLECTION_INTERVAL (1 << 0)
#define BVIEW_BST_CGSN_PORT_LIST (1 << 1)
#define BVIEW_BST_CGSN_COUNT (1 << 2)
#define BVIEW_BST_CGSN_QUEUE_TYPE (1 << 3)
#define BVIEW_BST_CGSN_QUEUE_LIST (1 << 4)
#define BVIEW_BST_CGSN_REQ_TYPE (1 << 5)

#define BVIEW_BST_TOP_PRT_DRP_VALID (BVIEW_BST_CGSN_COUNT | BVIEW_BST_CGSN_REQ_TYPE)
#define BVIEW_BST_TOP_PRT_Q_DRP_VALID (BVIEW_BST_CGSN_COUNT | BVIEW_BST_CGSN_REQ_TYPE | BVIEW_BST_CGSN_QUEUE_TYPE)
#define BVIEW_BST_PRT_DRP_VALID (BVIEW_BST_CGSN_PORT_LIST | BVIEW_BST_CGSN_REQ_TYPE)
#define BVIEW_BST_PRT_Q_DRP_VALID (BVIEW_BST_CGSN_PORT_LIST | BVIEW_BST_CGSN_REQ_TYPE | BVIEW_BST_CGSN_QUEUE_TYPE | BVIEW_BST_CGSN_QUEUE_LIST)

#define BVIEW_BST_CGSN_COUNT_MIN 1
#define BVIEW_BST_CGSN_COUNT_MAX 64

typedef enum _bst_cgsn_ctr_type_
{
    BVIEW_BST_CGSN_UCAST = 1,
    BVIEW_BST_CGSN_MCAST,
    BVIEW_BST_CGSN_ALL,
    BVIEW_BST_CGSN_TOTAL,
} BVIEW_BST_CGSN_CTR_TYPE_t;


typedef enum _bst_cgsn_req_type_
{
    BVIEW_BST_CGSN_TOP_DROPS = 1,
    BVIEW_BST_CGSN_TOP_PRT_Q_DROPS,
    BVIEW_BST_CGSN_PRT_DROPS,
    BVIEW_BST_CGSN_PRT_Q_DROPS
} BVIEW_BST_CGSN_REQ_TYPE_t;


/* structure to map realm to threshold type */
typedef struct _bst_cgsn_drp_string_req_ {
  /* req string */
  char *req_str;
  /* req  type*/
  BVIEW_BST_CGSN_REQ_TYPE_t req;
}BVIEW_BST_CGSN_DROP_REQ_MAP_t;


/* structure to map realm to threshold type */
typedef struct _bst_cgsn_drp_string_queue_ {
  /* queue string */
  char *q_str;
  /* queue  type*/
  BVIEW_BST_CGSN_CTR_TYPE_t queue;
}BVIEW_BST_CGSN_Q_TYPE_MAP_t;


typedef struct _bstjson_get_bst_cgsn_drp_ctrs_
{
  unsigned int count;
  BVIEW_BST_CGSN_REQ_TYPE_t req_type;
  bool all_prts;
  BVIEW_PORT_MASK_t port_list;
  BVIEW_BST_CGSN_CTR_TYPE_t queue_type;
  BVIEW_QUEUE_MASK_t queue_list;
  unsigned int intrvl;
} BSTJSON_GET_BST_CGSN_DROP_CTRS_t;

/******************************************************************
 * @brief  REST API Handler (Generated Code)
 *
 * @param[in]    cookie     Context for the API from Web server
 * @param[in]    jsonBuffer Raw Json Buffer
 * @param[in]    bufLength  Json Buffer length (bytes)
 *                           
 * @retval   BVIEW_STATUS_SUCCESS  JSON Parsed and parameters passed to BST APP
 * @retval   BVIEW_STATUS_INVALID_JSON  JSON is malformatted, or doesn't 
 * 					have necessary data.
 * @retval   BVIEW_STATUS_INVALID_PARAMETER Invalid input parameter
 *
 * @note     See the _impl() function for info passing to BST APP
 *********************************************************************/
BVIEW_STATUS bstjson_get_bst_congestion_drop_counters (void *cookie, char *jsonBuffer, int bufLength);

/*********************************************************************
 * @brief : REST API handler to get the bst congestion drop counters
 *
 * @param[in] cookie : pointer to the cookie
 * @param[in] asicId : asic id
 * @param[in] id     : unit id
 * @param[in] pCommand : pointer to the input command structure
 *
 * @retval  : BVIEW_STATUS_SUCCESS : the message is successfully posted to bst queue.
 * @retval  : BVIEW_STATUS_FAILURE : failed to post the message to bst.
 * @retval  : BVIEW_STATUS_INVALID_PARAMETER : invalid parameter.
 *
 * @note    :
 *
 *********************************************************************/
BVIEW_STATUS bstjson_get_bst_cgsn_drop_counters_impl (void *cookie,
    int asicId, int id,
    BSTJSON_GET_BST_CGSN_DROP_CTRS_t
    *pCommand);


#ifdef	__cplusplus  
}
#endif  

#endif /* INCLUDE_GET_BST_BST_CGSN_DROP_COUNTERS_H */ 

