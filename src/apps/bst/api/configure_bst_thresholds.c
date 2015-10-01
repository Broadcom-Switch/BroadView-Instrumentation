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
#include "configure_bst_thresholds.h"

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
BVIEW_STATUS bstjson_configure_bst_thresholds (void *cookie, char *jsonBuffer, int bufLength)
{

    /* Local Variables for JSON Parsing */
    cJSON *json_jsonrpc, *json_method, *json_asicId;
    cJSON *json_id, *json_realm, *json_port;
    cJSON *json_priorityGroup, *json_servicePool, *json_queue;
    cJSON *json_queueGroup, *json_threshold, *json_umShareThreshold;
    cJSON *json_umHeadroomThreshold, *json_ucShareThreshold, *json_mcShareThreshold;
    cJSON *json_mcShareQueueEntriesThreshold, *json_ucThreshold, *json_mcThreshold;
    cJSON *json_mcQueueEntriesThreshold, *json_cpuThreshold, *json_rqeThreshold;
    cJSON  *root, *params;

    /* Local non-command-parameter JSON variable declarations */
    char jsonrpc[JSON_MAX_NODE_LENGTH] = {0};
    char method[JSON_MAX_NODE_LENGTH] = {0};
    int asicId = 0, id = 0;

    /* Local variable declarations */
    BVIEW_STATUS status = BVIEW_STATUS_SUCCESS;
    BSTJSON_CONFIGURE_BST_THRESHOLDS_t command;

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
    /* Ensure that 'method' in the JSON equals "configure-bst-thresholds" */
    JSON_COMPARE_STRINGS_AND_CLEANUP ("method", &method[0], "configure-bst-thresholds");


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


    /* Parsing and Validating 'realm' from JSON buffer */
    json_realm = cJSON_GetObjectItem(params, "realm");
    JSON_VALIDATE_JSON_POINTER(json_realm, "realm", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_STRING(json_realm, "realm", BVIEW_STATUS_INVALID_JSON);
    /* Copy the string, with a limit on max characters */
    strncpy (&command.realm[0], json_realm->valuestring, JSON_MAX_NODE_LENGTH - 1);


    /* Parsing and Validating 'port' from JSON buffer */
    json_port = cJSON_GetObjectItem(params, "port");
    /* The node 'port' is an optioanl one, ignore if not present in JSON */
    if (json_port != NULL)
    {
        JSON_VALIDATE_JSON_POINTER(json_port, "port", BVIEW_STATUS_INVALID_JSON);
        JSON_VALIDATE_JSON_AS_STRING(json_port, "port", BVIEW_STATUS_INVALID_JSON);
        /* Copy the 'port' in external notation to our internal representation */
        JSON_PORT_MAP_FROM_NOTATION(command.port, json_port->valuestring);
    } /* if optional */


    /* Parsing and Validating 'priority-group' from JSON buffer */
    json_priorityGroup = cJSON_GetObjectItem(params, "priority-group");
    /* The node 'priority-group' is an optioanl one, ignore if not present in JSON */
    if (json_priorityGroup != NULL)
    {
        JSON_VALIDATE_JSON_POINTER(json_priorityGroup, "priority-group", BVIEW_STATUS_INVALID_JSON);
        JSON_VALIDATE_JSON_AS_NUMBER(json_priorityGroup, "priority-group");
        /* Copy the value */
        command.priorityGroup = json_priorityGroup->valueint;
        /* Ensure  that the number 'priority-group' is within range of [0,7] */
        JSON_CHECK_VALUE_AND_CLEANUP (command.priorityGroup, 0, 7);
    } /* if optional */


    /* Parsing and Validating 'service-pool' from JSON buffer */
    json_servicePool = cJSON_GetObjectItem(params, "service-pool");
    /* The node 'service-pool' is an optioanl one, ignore if not present in JSON */
    if (json_servicePool != NULL)
    {
        JSON_VALIDATE_JSON_POINTER(json_servicePool, "service-pool", BVIEW_STATUS_INVALID_JSON);
        JSON_VALIDATE_JSON_AS_NUMBER(json_servicePool, "service-pool");
        /* Copy the value */
        command.servicePool = json_servicePool->valueint;
        /* Ensure  that the number 'service-pool' is within range of [0,3] */
        JSON_CHECK_VALUE_AND_CLEANUP (command.servicePool, 0, 3);
    } /* if optional */


    /* Parsing and Validating 'queue' from JSON buffer */
    json_queue = cJSON_GetObjectItem(params, "queue");
    /* The node 'queue' is an optioanl one, ignore if not present in JSON */
    if (json_queue != NULL)
    {
        JSON_VALIDATE_JSON_POINTER(json_queue, "queue", BVIEW_STATUS_INVALID_JSON);
        JSON_VALIDATE_JSON_AS_NUMBER(json_queue, "queue");
        /* Copy the value */
        command.queue = json_queue->valueint;
        /* Ensure  that the number 'queue' is within range of [0,4095] */
        JSON_CHECK_VALUE_AND_CLEANUP (command.queue, 0, 4095);
    } /* if optional */


    /* Parsing and Validating 'queue-group' from JSON buffer */
    json_queueGroup = cJSON_GetObjectItem(params, "queue-group");
    /* The node 'queue-group' is an optioanl one, ignore if not present in JSON */
    if (json_queueGroup != NULL)
    {
        JSON_VALIDATE_JSON_POINTER(json_queueGroup, "queue-group", BVIEW_STATUS_INVALID_JSON);
        JSON_VALIDATE_JSON_AS_NUMBER(json_queueGroup, "queue-group");
        /* Copy the value */
        command.queueGroup = json_queueGroup->valueint;
        /* Ensure  that the number 'queue-group' is within range of [0,7] */
        JSON_CHECK_VALUE_AND_CLEANUP (command.queueGroup, 0, 127);
    } /* if optional */


    /* Parsing and Validating 'threshold' from JSON buffer */
    json_threshold = cJSON_GetObjectItem(params, "threshold");
    /* The node 'threshold' is an optioanl one, ignore if not present in JSON */
    if (json_threshold != NULL)
    {
        JSON_VALIDATE_JSON_POINTER(json_threshold, "threshold", BVIEW_STATUS_INVALID_JSON);
        JSON_VALIDATE_JSON_AS_NUMBER(json_threshold, "threshold");
        /* Copy the value */
        command.threshold = json_threshold->valueint;
    } /* if optional */


    /* Parsing and Validating 'um-share-threshold' from JSON buffer */
    json_umShareThreshold = cJSON_GetObjectItem(params, "um-share-threshold");
    /* The node 'um-share-threshold' is an optioanl one, ignore if not present in JSON */
    if (json_umShareThreshold != NULL)
    {
        JSON_VALIDATE_JSON_POINTER(json_umShareThreshold, "um-share-threshold", BVIEW_STATUS_INVALID_JSON);
        JSON_VALIDATE_JSON_AS_NUMBER(json_umShareThreshold, "um-share-threshold");
        /* Copy the value */
        command.umShareThreshold = json_umShareThreshold->valueint;
    } /* if optional */


    /* Parsing and Validating 'um-headroom-threshold' from JSON buffer */
    json_umHeadroomThreshold = cJSON_GetObjectItem(params, "um-headroom-threshold");
    /* The node 'um-headroom-threshold' is an optioanl one, ignore if not present in JSON */
    if (json_umHeadroomThreshold != NULL)
    {
        JSON_VALIDATE_JSON_POINTER(json_umHeadroomThreshold, "um-headroom-threshold", BVIEW_STATUS_INVALID_JSON);
        JSON_VALIDATE_JSON_AS_NUMBER(json_umHeadroomThreshold, "um-headroom-threshold");
        /* Copy the value */
        command.umHeadroomThreshold = json_umHeadroomThreshold->valueint;
    } /* if optional */


    /* Parsing and Validating 'uc-share-threshold' from JSON buffer */
    json_ucShareThreshold = cJSON_GetObjectItem(params, "uc-share-threshold");
    /* The node 'uc-share-threshold' is an optioanl one, ignore if not present in JSON */
    if (json_ucShareThreshold != NULL)
    {
        JSON_VALIDATE_JSON_POINTER(json_ucShareThreshold, "uc-share-threshold", BVIEW_STATUS_INVALID_JSON);
        JSON_VALIDATE_JSON_AS_NUMBER(json_ucShareThreshold, "uc-share-threshold");
        /* Copy the value */
        command.ucShareThreshold = json_ucShareThreshold->valueint;
    } /* if optional */


    /* Parsing and Validating 'mc-share-threshold' from JSON buffer */
    json_mcShareThreshold = cJSON_GetObjectItem(params, "mc-share-threshold");
    /* The node 'mc-share-threshold' is an optioanl one, ignore if not present in JSON */
    if (json_mcShareThreshold != NULL)
    {
        JSON_VALIDATE_JSON_POINTER(json_mcShareThreshold, "mc-share-threshold", BVIEW_STATUS_INVALID_JSON);
        JSON_VALIDATE_JSON_AS_NUMBER(json_mcShareThreshold, "mc-share-threshold");
        /* Copy the value */
        command.mcShareThreshold = json_mcShareThreshold->valueint;
    } /* if optional */


    /* Parsing and Validating 'mc-share-queue-entries-threshold' from JSON buffer */
    json_mcShareQueueEntriesThreshold = cJSON_GetObjectItem(params, "mc-share-queue-entries-threshold");
    /* The node 'mc-share-queue-entries-threshold' is an optioanl one, ignore if not present in JSON */
    if (json_mcShareQueueEntriesThreshold != NULL)
    {
        JSON_VALIDATE_JSON_POINTER(json_mcShareQueueEntriesThreshold, "mc-share-queue-entries-threshold", BVIEW_STATUS_INVALID_JSON);
        JSON_VALIDATE_JSON_AS_NUMBER(json_mcShareQueueEntriesThreshold, "mc-share-queue-entries-threshold");
        /* Copy the value */
        command.mcShareQueueEntriesThreshold = json_mcShareQueueEntriesThreshold->valueint;
    } /* if optional */


    /* Parsing and Validating 'uc-threshold' from JSON buffer */
    json_ucThreshold = cJSON_GetObjectItem(params, "uc-threshold");
    /* The node 'uc-threshold' is an optioanl one, ignore if not present in JSON */
    if (json_ucThreshold != NULL)
    {
        JSON_VALIDATE_JSON_POINTER(json_ucThreshold, "uc-threshold", BVIEW_STATUS_INVALID_JSON);
        JSON_VALIDATE_JSON_AS_NUMBER(json_ucThreshold, "uc-threshold");
        /* Copy the value */
        command.ucThreshold = json_ucThreshold->valueint;
    } /* if optional */


    /* Parsing and Validating 'mc-threshold' from JSON buffer */
    json_mcThreshold = cJSON_GetObjectItem(params, "mc-threshold");
    /* The node 'mc-threshold' is an optioanl one, ignore if not present in JSON */
    if (json_mcThreshold != NULL)
    {
        JSON_VALIDATE_JSON_POINTER(json_mcThreshold, "mc-threshold", BVIEW_STATUS_INVALID_JSON);
        JSON_VALIDATE_JSON_AS_NUMBER(json_mcThreshold, "mc-threshold");
        /* Copy the value */
        command.mcThreshold = json_mcThreshold->valueint;
    } /* if optional */


    /* Parsing and Validating 'mc-queue-entries-threshold' from JSON buffer */
    json_mcQueueEntriesThreshold = cJSON_GetObjectItem(params, "mc-queue-entries-threshold");
    /* The node 'mc-queue-entries-threshold' is an optioanl one, ignore if not present in JSON */
    if (json_mcQueueEntriesThreshold != NULL)
    {
        JSON_VALIDATE_JSON_POINTER(json_mcQueueEntriesThreshold, "mc-queue-entries-threshold", BVIEW_STATUS_INVALID_JSON);
        JSON_VALIDATE_JSON_AS_NUMBER(json_mcQueueEntriesThreshold, "mc-queue-entries-threshold");
        /* Copy the value */
        command.mcQueueEntriesThreshold = json_mcQueueEntriesThreshold->valueint;
    } /* if optional */


    /* Parsing and Validating 'cpu-threshold' from JSON buffer */
    json_cpuThreshold = cJSON_GetObjectItem(params, "cpu-threshold");
    /* The node 'cpu-threshold' is an optioanl one, ignore if not present in JSON */
    if (json_cpuThreshold != NULL)
    {
        JSON_VALIDATE_JSON_POINTER(json_cpuThreshold, "cpu-threshold", BVIEW_STATUS_INVALID_JSON);
        JSON_VALIDATE_JSON_AS_NUMBER(json_cpuThreshold, "cpu-threshold");
        /* Copy the value */
        command.cpuThreshold = json_cpuThreshold->valueint;
    } /* if optional */


    /* Parsing and Validating 'rqe-threshold' from JSON buffer */
    json_rqeThreshold = cJSON_GetObjectItem(params, "rqe-threshold");
    /* The node 'rqe-threshold' is an optioanl one, ignore if not present in JSON */
    if (json_rqeThreshold != NULL)
    {
        JSON_VALIDATE_JSON_POINTER(json_rqeThreshold, "rqe-threshold", BVIEW_STATUS_INVALID_JSON);
        JSON_VALIDATE_JSON_AS_NUMBER(json_rqeThreshold, "rqe-threshold");
        /* Copy the value */
        command.rqeThreshold = json_rqeThreshold->valueint;
    } /* if optional */


    /* Send the 'command' along with 'asicId' and 'cookie' to the Application thread. */
    status = bstjson_configure_bst_thresholds_impl (cookie, asicId, id, &command);

    /* Free up any allocated resources and return status code */
    if (root != NULL)
    {
        cJSON_Delete(root);
    }

    return status;
}
