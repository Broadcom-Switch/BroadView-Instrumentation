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

/* Include Header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "broadview.h"
#include "json.h"

#include "cJSON.h"
#include "get_bst_thresholds.h"

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
BVIEW_STATUS bstjson_get_bst_thresholds (void *cookie, char *jsonBuffer, int bufLength)
{

    /* Local Variables for JSON Parsing */
    cJSON *json_jsonrpc, *json_method, *json_asicId;
    cJSON *json_id, *json_includeIngressPortPriorityGroup, *json_includeIngressPortServicePool;
    cJSON *json_includeIngressServicePool, *json_includeEgressPortServicePool, *json_includeEgressServicePool;
    cJSON *json_includeEgressUcQueue, *json_includeEgressUcQueueGroup, *json_includeEgressMcQueue;
    cJSON *json_includeEgressCpuQueue, *json_includeEgressRqeQueue, *json_includeDevice;
    cJSON  *root, *params;

    /* Local non-command-parameter JSON variable declarations */
    char jsonrpc[JSON_MAX_NODE_LENGTH] = {0};
    char method[JSON_MAX_NODE_LENGTH] = {0};
    int asicId = 0, id = 0;

    /* Local variable declarations */
    BVIEW_STATUS status = BVIEW_STATUS_SUCCESS;
    BSTJSON_GET_BST_THRESHOLDS_t command;

    memset(&command, 0, sizeof (command));

    /* Validating input parameters */

    /* Validating 'cookie' */
    JSON_VALIDATE_POINTER(cookie, "cookie", BVIEW_STATUS_INVALID_PARAMETER);

    /* Validating 'jsonBuffer' */
    JSON_VALIDATE_POINTER(jsonBuffer, "jsonBuffer", BVIEW_STATUS_INVALID_PARAMETER);

    /* Validating 'bufLength' */
    if (bufLength > strlen(jsonBuffer))
    {
        _jsonlog("Invalid value for parameter bufLength %d ", bufLength );
        return BVIEW_STATUS_INVALID_PARAMETER;
    }

    /* Parse JSON to a C-JSON root */
    root = cJSON_Parse(jsonBuffer);
    JSON_VALIDATE_JSON_POINTER(root, "root", BVIEW_STATUS_INVALID_JSON);

    /* Obtain command parameters */
    params = cJSON_GetObjectItem(root, "params");
    JSON_VALIDATE_JSON_POINTER(params, "params", BVIEW_STATUS_INVALID_JSON);

    /* Parsing and Validating 'jsonrpc' from JSON buffer */
    json_jsonrpc = cJSON_GetObjectItem(root, "jsonrpc");
    JSON_VALIDATE_JSON_POINTER(json_jsonrpc, "jsonrpc", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_STRING(json_jsonrpc, "jsonrpc", BVIEW_STATUS_INVALID_JSON);
    /* Copy the string, with a limit on max characters */
    strncpy (&jsonrpc[0], json_jsonrpc->valuestring, JSON_MAX_NODE_LENGTH - 1);
    /* Ensure that 'jsonrpc' in the JSON equals "2.0" */
    JSON_COMPARE_STRINGS_AND_CLEANUP ("jsonrpc", &jsonrpc[0], "2.0");


    /* Parsing and Validating 'method' from JSON buffer */
    json_method = cJSON_GetObjectItem(root, "method");
    JSON_VALIDATE_JSON_POINTER(json_method, "method", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_STRING(json_method, "method", BVIEW_STATUS_INVALID_JSON);
    /* Copy the string, with a limit on max characters */
    strncpy (&method[0], json_method->valuestring, JSON_MAX_NODE_LENGTH - 1);
    /* Ensure that 'method' in the JSON equals "get-bst-thresholds" */
    JSON_COMPARE_STRINGS_AND_CLEANUP ("method", &method[0], "get-bst-thresholds");


    /* Parsing and Validating 'asic-id' from JSON buffer */
    json_asicId = cJSON_GetObjectItem(root, "asic-id");
    JSON_VALIDATE_JSON_POINTER(json_asicId, "asic-id", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_STRING(json_asicId, "asic-id", BVIEW_STATUS_INVALID_JSON);
    /* Copy the 'asic-id' in external notation to our internal representation */
    JSON_ASIC_ID_MAP_FROM_NOTATION(asicId, json_asicId->valuestring);

    /* Parsing and Validating 'id' from JSON buffer */
    json_id = cJSON_GetObjectItem(root, "id");
    JSON_VALIDATE_JSON_POINTER(json_id, "id", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_id, "id");
    /* Copy the value */
    id = json_id->valueint;
    /* Ensure  that the number 'id' is within range of [1,100000] */
    JSON_CHECK_VALUE_AND_CLEANUP (id, 1, 100000);


    /* Parsing and Validating 'include-ingress-port-priority-group' from JSON buffer */
    json_includeIngressPortPriorityGroup = cJSON_GetObjectItem(params, "include-ingress-port-priority-group");
    JSON_VALIDATE_JSON_POINTER(json_includeIngressPortPriorityGroup, "include-ingress-port-priority-group", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_includeIngressPortPriorityGroup, "include-ingress-port-priority-group");
    /* Copy the value */
    command.includeIngressPortPriorityGroup = json_includeIngressPortPriorityGroup->valueint;
    /* Ensure  that the number 'include-ingress-port-priority-group' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.includeIngressPortPriorityGroup, 0, 1);


    /* Parsing and Validating 'include-ingress-port-service-pool' from JSON buffer */
    json_includeIngressPortServicePool = cJSON_GetObjectItem(params, "include-ingress-port-service-pool");
    JSON_VALIDATE_JSON_POINTER(json_includeIngressPortServicePool, "include-ingress-port-service-pool", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_includeIngressPortServicePool, "include-ingress-port-service-pool");
    /* Copy the value */
    command.includeIngressPortServicePool = json_includeIngressPortServicePool->valueint;
    /* Ensure  that the number 'include-ingress-port-service-pool' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.includeIngressPortServicePool, 0, 1);


    /* Parsing and Validating 'include-ingress-service-pool' from JSON buffer */
    json_includeIngressServicePool = cJSON_GetObjectItem(params, "include-ingress-service-pool");
    JSON_VALIDATE_JSON_POINTER(json_includeIngressServicePool, "include-ingress-service-pool", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_includeIngressServicePool, "include-ingress-service-pool");
    /* Copy the value */
    command.includeIngressServicePool = json_includeIngressServicePool->valueint;
    /* Ensure  that the number 'include-ingress-service-pool' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.includeIngressServicePool, 0, 1);


    /* Parsing and Validating 'include-egress-port-service-pool' from JSON buffer */
    json_includeEgressPortServicePool = cJSON_GetObjectItem(params, "include-egress-port-service-pool");
    JSON_VALIDATE_JSON_POINTER(json_includeEgressPortServicePool, "include-egress-port-service-pool", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_includeEgressPortServicePool, "include-egress-port-service-pool");
    /* Copy the value */
    command.includeEgressPortServicePool = json_includeEgressPortServicePool->valueint;
    /* Ensure  that the number 'include-egress-port-service-pool' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.includeEgressPortServicePool, 0, 1);


    /* Parsing and Validating 'include-egress-service-pool' from JSON buffer */
    json_includeEgressServicePool = cJSON_GetObjectItem(params, "include-egress-service-pool");
    JSON_VALIDATE_JSON_POINTER(json_includeEgressServicePool, "include-egress-service-pool", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_includeEgressServicePool, "include-egress-service-pool");
    /* Copy the value */
    command.includeEgressServicePool = json_includeEgressServicePool->valueint;
    /* Ensure  that the number 'include-egress-service-pool' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.includeEgressServicePool, 0, 1);


    /* Parsing and Validating 'include-egress-uc-queue' from JSON buffer */
    json_includeEgressUcQueue = cJSON_GetObjectItem(params, "include-egress-uc-queue");
    JSON_VALIDATE_JSON_POINTER(json_includeEgressUcQueue, "include-egress-uc-queue", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_includeEgressUcQueue, "include-egress-uc-queue");
    /* Copy the value */
    command.includeEgressUcQueue = json_includeEgressUcQueue->valueint;
    /* Ensure  that the number 'include-egress-uc-queue' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.includeEgressUcQueue, 0, 1);


    /* Parsing and Validating 'include-egress-uc-queue-group' from JSON buffer */
    json_includeEgressUcQueueGroup = cJSON_GetObjectItem(params, "include-egress-uc-queue-group");
    JSON_VALIDATE_JSON_POINTER(json_includeEgressUcQueueGroup, "include-egress-uc-queue-group", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_includeEgressUcQueueGroup, "include-egress-uc-queue-group");
    /* Copy the value */
    command.includeEgressUcQueueGroup = json_includeEgressUcQueueGroup->valueint;
    /* Ensure  that the number 'include-egress-uc-queue-group' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.includeEgressUcQueueGroup, 0, 1);


    /* Parsing and Validating 'include-egress-mc-queue' from JSON buffer */
    json_includeEgressMcQueue = cJSON_GetObjectItem(params, "include-egress-mc-queue");
    JSON_VALIDATE_JSON_POINTER(json_includeEgressMcQueue, "include-egress-mc-queue", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_includeEgressMcQueue, "include-egress-mc-queue");
    /* Copy the value */
    command.includeEgressMcQueue = json_includeEgressMcQueue->valueint;
    /* Ensure  that the number 'include-egress-mc-queue' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.includeEgressMcQueue, 0, 1);


    /* Parsing and Validating 'include-egress-cpu-queue' from JSON buffer */
    json_includeEgressCpuQueue = cJSON_GetObjectItem(params, "include-egress-cpu-queue");
    JSON_VALIDATE_JSON_POINTER(json_includeEgressCpuQueue, "include-egress-cpu-queue", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_includeEgressCpuQueue, "include-egress-cpu-queue");
    /* Copy the value */
    command.includeEgressCpuQueue = json_includeEgressCpuQueue->valueint;
    /* Ensure  that the number 'include-egress-cpu-queue' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.includeEgressCpuQueue, 0, 1);


    /* Parsing and Validating 'include-egress-rqe-queue' from JSON buffer */
    json_includeEgressRqeQueue = cJSON_GetObjectItem(params, "include-egress-rqe-queue");
    JSON_VALIDATE_JSON_POINTER(json_includeEgressRqeQueue, "include-egress-rqe-queue", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_includeEgressRqeQueue, "include-egress-rqe-queue");
    /* Copy the value */
    command.includeEgressRqeQueue = json_includeEgressRqeQueue->valueint;
    /* Ensure  that the number 'include-egress-rqe-queue' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.includeEgressRqeQueue, 0, 1);


    /* Parsing and Validating 'include-device' from JSON buffer */
    json_includeDevice = cJSON_GetObjectItem(params, "include-device");
    JSON_VALIDATE_JSON_POINTER(json_includeDevice, "include-device", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_includeDevice, "include-device");
    /* Copy the value */
    command.includeDevice = json_includeDevice->valueint;
    /* Ensure  that the number 'include-device' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.includeDevice, 0, 1);


    /* Send the 'command' along with 'asicId' and 'cookie' to the Application thread. */
    status = bstjson_get_bst_thresholds_impl (cookie, asicId, id, &command);

    /* Free up any allocated resources and return status code */
    if (root != NULL)
    {
        cJSON_Delete(root);
    }

    return status;
}
