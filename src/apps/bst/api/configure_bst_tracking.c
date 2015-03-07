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
#include "configure_bst_tracking.h"

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
BVIEW_STATUS bstjson_configure_bst_tracking (void *cookie, char *jsonBuffer, int bufLength)
{

    /* Local Variables for JSON Parsing */
    cJSON *json_jsonrpc, *json_method, *json_asicId;
    cJSON *json_id, *json_trackPeakStats, *json_trackIngressPortPriorityGroup;
    cJSON *json_trackIngressPortServicePool, *json_trackIngressServicePool, *json_trackEgressPortServicePool;
    cJSON *json_trackEgressServicePool, *json_trackEgressUcQueue, *json_trackEgressUcQueueGroup;
    cJSON *json_trackEgressMcQueue, *json_trackEgressCpuQueue, *json_trackEgressRqeQueue;
    cJSON *json_trackDevice,  *root, *params;

    /* Local non-command-parameter JSON variable declarations */
    char jsonrpc[JSON_MAX_NODE_LENGTH] = {0};
    char method[JSON_MAX_NODE_LENGTH] = {0};
    int asicId = 0, id = 0;

    /* Local variable declarations */
    BVIEW_STATUS status = BVIEW_STATUS_SUCCESS;
    BSTJSON_CONFIGURE_BST_TRACKING_t command;

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
    /* Ensure that 'method' in the JSON equals "configure-bst-tracking" */
    JSON_COMPARE_STRINGS_AND_CLEANUP ("method", &method[0], "configure-bst-tracking");


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


    /* Parsing and Validating 'track-peak-stats' from JSON buffer */
    json_trackPeakStats = cJSON_GetObjectItem(params, "track-peak-stats");
    JSON_VALIDATE_JSON_POINTER(json_trackPeakStats, "track-peak-stats", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_trackPeakStats, "track-peak-stats");
    /* Copy the value */
    command.trackPeakStats = json_trackPeakStats->valueint;
    /* Ensure  that the number 'track-peak-stats' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.trackPeakStats, 0, 1);


    /* Parsing and Validating 'track-ingress-port-priority-group' from JSON buffer */
    json_trackIngressPortPriorityGroup = cJSON_GetObjectItem(params, "track-ingress-port-priority-group");
    JSON_VALIDATE_JSON_POINTER(json_trackIngressPortPriorityGroup, "track-ingress-port-priority-group", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_trackIngressPortPriorityGroup, "track-ingress-port-priority-group");
    /* Copy the value */
    command.trackIngressPortPriorityGroup = json_trackIngressPortPriorityGroup->valueint;
    /* Ensure  that the number 'track-ingress-port-priority-group' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.trackIngressPortPriorityGroup, 0, 1);


    /* Parsing and Validating 'track-ingress-port-service-pool' from JSON buffer */
    json_trackIngressPortServicePool = cJSON_GetObjectItem(params, "track-ingress-port-service-pool");
    JSON_VALIDATE_JSON_POINTER(json_trackIngressPortServicePool, "track-ingress-port-service-pool", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_trackIngressPortServicePool, "track-ingress-port-service-pool");
    /* Copy the value */
    command.trackIngressPortServicePool = json_trackIngressPortServicePool->valueint;
    /* Ensure  that the number 'track-ingress-port-service-pool' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.trackIngressPortServicePool, 0, 1);


    /* Parsing and Validating 'track-ingress-service-pool' from JSON buffer */
    json_trackIngressServicePool = cJSON_GetObjectItem(params, "track-ingress-service-pool");
    JSON_VALIDATE_JSON_POINTER(json_trackIngressServicePool, "track-ingress-service-pool", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_trackIngressServicePool, "track-ingress-service-pool");
    /* Copy the value */
    command.trackIngressServicePool = json_trackIngressServicePool->valueint;
    /* Ensure  that the number 'track-ingress-service-pool' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.trackIngressServicePool, 0, 1);


    /* Parsing and Validating 'track-egress-port-service-pool' from JSON buffer */
    json_trackEgressPortServicePool = cJSON_GetObjectItem(params, "track-egress-port-service-pool");
    JSON_VALIDATE_JSON_POINTER(json_trackEgressPortServicePool, "track-egress-port-service-pool", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_trackEgressPortServicePool, "track-egress-port-service-pool");
    /* Copy the value */
    command.trackEgressPortServicePool = json_trackEgressPortServicePool->valueint;
    /* Ensure  that the number 'track-egress-port-service-pool' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.trackEgressPortServicePool, 0, 1);


    /* Parsing and Validating 'track-egress-service-pool' from JSON buffer */
    json_trackEgressServicePool = cJSON_GetObjectItem(params, "track-egress-service-pool");
    JSON_VALIDATE_JSON_POINTER(json_trackEgressServicePool, "track-egress-service-pool", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_trackEgressServicePool, "track-egress-service-pool");
    /* Copy the value */
    command.trackEgressServicePool = json_trackEgressServicePool->valueint;
    /* Ensure  that the number 'track-egress-service-pool' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.trackEgressServicePool, 0, 1);


    /* Parsing and Validating 'track-egress-uc-queue' from JSON buffer */
    json_trackEgressUcQueue = cJSON_GetObjectItem(params, "track-egress-uc-queue");
    JSON_VALIDATE_JSON_POINTER(json_trackEgressUcQueue, "track-egress-uc-queue", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_trackEgressUcQueue, "track-egress-uc-queue");
    /* Copy the value */
    command.trackEgressUcQueue = json_trackEgressUcQueue->valueint;
    /* Ensure  that the number 'track-egress-uc-queue' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.trackEgressUcQueue, 0, 1);


    /* Parsing and Validating 'track-egress-uc-queue-group' from JSON buffer */
    json_trackEgressUcQueueGroup = cJSON_GetObjectItem(params, "track-egress-uc-queue-group");
    JSON_VALIDATE_JSON_POINTER(json_trackEgressUcQueueGroup, "track-egress-uc-queue-group", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_trackEgressUcQueueGroup, "track-egress-uc-queue-group");
    /* Copy the value */
    command.trackEgressUcQueueGroup = json_trackEgressUcQueueGroup->valueint;
    /* Ensure  that the number 'track-egress-uc-queue-group' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.trackEgressUcQueueGroup, 0, 1);


    /* Parsing and Validating 'track-egress-mc-queue' from JSON buffer */
    json_trackEgressMcQueue = cJSON_GetObjectItem(params, "track-egress-mc-queue");
    JSON_VALIDATE_JSON_POINTER(json_trackEgressMcQueue, "track-egress-mc-queue", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_trackEgressMcQueue, "track-egress-mc-queue");
    /* Copy the value */
    command.trackEgressMcQueue = json_trackEgressMcQueue->valueint;
    /* Ensure  that the number 'track-egress-mc-queue' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.trackEgressMcQueue, 0, 1);


    /* Parsing and Validating 'track-egress-cpu-queue' from JSON buffer */
    json_trackEgressCpuQueue = cJSON_GetObjectItem(params, "track-egress-cpu-queue");
    JSON_VALIDATE_JSON_POINTER(json_trackEgressCpuQueue, "track-egress-cpu-queue", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_trackEgressCpuQueue, "track-egress-cpu-queue");
    /* Copy the value */
    command.trackEgressCpuQueue = json_trackEgressCpuQueue->valueint;
    /* Ensure  that the number 'track-egress-cpu-queue' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.trackEgressCpuQueue, 0, 1);


    /* Parsing and Validating 'track-egress-rqe-queue' from JSON buffer */
    json_trackEgressRqeQueue = cJSON_GetObjectItem(params, "track-egress-rqe-queue");
    JSON_VALIDATE_JSON_POINTER(json_trackEgressRqeQueue, "track-egress-rqe-queue", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_trackEgressRqeQueue, "track-egress-rqe-queue");
    /* Copy the value */
    command.trackEgressRqeQueue = json_trackEgressRqeQueue->valueint;
    /* Ensure  that the number 'track-egress-rqe-queue' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.trackEgressRqeQueue, 0, 1);


    /* Parsing and Validating 'track-device' from JSON buffer */
    json_trackDevice = cJSON_GetObjectItem(params, "track-device");
    JSON_VALIDATE_JSON_POINTER(json_trackDevice, "track-device", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_trackDevice, "track-device");
    /* Copy the value */
    command.trackDevice = json_trackDevice->valueint;
    /* Ensure  that the number 'track-device' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.trackDevice, 0, 1);


    /* Send the 'command' along with 'asicId' and 'cookie' to the Application thread. */
    status = bstjson_configure_bst_tracking_impl (cookie, asicId, id, &command);

    /* Free up any allocated resources and return status code */
    if (root != NULL)
    {
        cJSON_Delete(root);
    }

    return status;
}
