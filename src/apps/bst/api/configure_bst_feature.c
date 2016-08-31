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

/* Include Header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "broadview.h"
#include "json.h"

#include "cJSON.h"
#include "configure_bst_feature.h"

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
BVIEW_STATUS bstjson_configure_bst_feature (void *cookie, char *jsonBuffer, int bufLength)
{

    /* Local Variables for JSON Parsing */
    cJSON *json_jsonrpc, *json_method, *json_asicId;
    cJSON *json_id, *json_bstEnable, *json_sendAsyncReports;
    cJSON *json_collectionInterval, *json_statUnitsInCells,  *root, *params;
    cJSON *json_maxTriggerReports, *json_sendSnapshotTrigger,  *json_triggerTransmitInterval, *json_sendIncrementalReport;
    cJSON *json_statsInPercentage;

    /* Local non-command-parameter JSON variable declarations */
    char jsonrpc[JSON_MAX_NODE_LENGTH] = {0};
    char method[JSON_MAX_NODE_LENGTH] = {0};
    int asicId = 0, id = 0;

    /* Local variable declarations */
    BVIEW_STATUS status = BVIEW_STATUS_SUCCESS;
    BSTJSON_CONFIGURE_BST_FEATURE_t command;

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
    /* Ensure that 'method' in the JSON equals "configure-bst-feature" */
    JSON_COMPARE_STRINGS_AND_CLEANUP ("method", &method[0], "configure-bst-feature");


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

    /* Parsing and Validating 'bst-enable' from JSON buffer */
    json_bstEnable = cJSON_GetObjectItem(params, "bst-enable");
    JSON_VALIDATE_JSON_POINTER(json_bstEnable, "bst-enable", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_bstEnable, "bst-enable");
    /* Copy the value */
    command.bstEnable = json_bstEnable->valueint;
    /* Ensure  that the number 'bst-enable' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.bstEnable, 0, 1);
    command.configMask = (command.configMask | (1 << BST_CONFIG_PARAMS_ENABLE));


    /* Parsing and Validating 'send-async-reports' from JSON buffer */
    json_sendAsyncReports = cJSON_GetObjectItem(params, "send-async-reports");
    JSON_VALIDATE_JSON_POINTER(json_sendAsyncReports, "send-async-reports", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_sendAsyncReports, "send-async-reports");
    /* Copy the value */
    command.sendAsyncReports = json_sendAsyncReports->valueint;
    /* Ensure  that the number 'send-async-reports' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.sendAsyncReports, 0, 1);
    command.configMask = (command.configMask | (1 << BST_CONFIG_PARAMS_SND_ASYNC_REP));


    /* Parsing and Validating 'collection-interval' from JSON buffer */
    json_collectionInterval = cJSON_GetObjectItem(params, "collection-interval");
    JSON_VALIDATE_JSON_POINTER(json_collectionInterval, "collection-interval", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_collectionInterval, "collection-interval");
    /* Copy the value */
    command.collectionInterval = json_collectionInterval->valueint;
    /* Ensure  that the number 'collection-interval' is within range of [0,600] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.collectionInterval, 0, 600);
    command.configMask = (command.configMask | (1 << BST_CONFIG_PARAMS_COLL_INTRVL));


    /* Parsing and Validating 'stat-units-in-cells' from JSON buffer */
    json_statUnitsInCells = cJSON_GetObjectItem(params, "stat-units-in-cells");
    JSON_VALIDATE_JSON_POINTER(json_statUnitsInCells, "stat-units-in-cells", BVIEW_STATUS_INVALID_JSON);
    JSON_VALIDATE_JSON_AS_NUMBER(json_statUnitsInCells, "stat-units-in-cells");
    /* Copy the value */
    command.statUnitsInCells = json_statUnitsInCells->valueint;
    /* Ensure  that the number 'stat-units-in-cells' is within range of [0,1] */
    JSON_CHECK_VALUE_AND_CLEANUP (command.statUnitsInCells, 0, 1);
    command.configMask = (command.configMask | (1 << BST_CONFIG_PARAMS_STATS_UNITS));

    /* Parsing and Validating 'max-trigger-reports' from JSON buffer */
    json_maxTriggerReports = cJSON_GetObjectItem(params, "trigger-rate-limit");
    if (NULL != json_maxTriggerReports)
    {
      JSON_VALIDATE_JSON_POINTER(json_maxTriggerReports, "trigger-rate-limit", BVIEW_STATUS_INVALID_JSON);
      JSON_VALIDATE_JSON_AS_NUMBER(json_maxTriggerReports, "trigger-rate-limit");
      /* Copy the value */
      command.bstMaxTriggers = json_maxTriggerReports->valueint;
      JSON_CHECK_VALUE_AND_CLEANUP (command.bstMaxTriggers, 1, 30);
      command.configMask = (command.configMask | (1 << BST_CONFIG_PARAMS_TGR_RATE_LIMIT));
    }

    /* Parsing and Validating 'send-snapshot-on-trigger' from JSON buffer */
    json_sendSnapshotTrigger = cJSON_GetObjectItem(params, "send-snapshot-on-trigger");
    if (NULL != json_sendSnapshotTrigger)
    {
      JSON_VALIDATE_JSON_POINTER(json_sendSnapshotTrigger, "send-snapshot-on-trigger", BVIEW_STATUS_INVALID_JSON);
      JSON_VALIDATE_JSON_AS_NUMBER(json_sendSnapshotTrigger, "send-snapshot-on-trigger");
      /* Copy the value */
      command.sendSnapshotOnTrigger = json_sendSnapshotTrigger->valueint;
      JSON_CHECK_VALUE_AND_CLEANUP (command.sendSnapshotOnTrigger, 0, 1);
      command.configMask = (command.configMask | (1 << BST_CONFIG_PARAMS_SND_SNAP_TGR));
    }

    /* Parsing and Validating 'trigger-rate-limit-interval' from JSON buffer */
    json_triggerTransmitInterval = cJSON_GetObjectItem(params, "trigger-rate-limit-interval");
    if (NULL != json_triggerTransmitInterval)
    {
      JSON_VALIDATE_JSON_POINTER(json_triggerTransmitInterval, "trigger-rate-limit-interval", BVIEW_STATUS_INVALID_JSON);
      JSON_VALIDATE_JSON_AS_NUMBER(json_triggerTransmitInterval, "trigger-rate-limit-interval");
      /* Copy the value */
      command.triggerTransmitInterval = json_triggerTransmitInterval->valueint;
      JSON_CHECK_VALUE_AND_CLEANUP (command.triggerTransmitInterval, 1, 60);
      command.configMask = (command.configMask | (1 << BST_CONFIG_PARAMS_TGR_RL_INTVL));
    }

    /* Parsing and Validating 'aync-full-reports' from JSON buffer */
    json_sendIncrementalReport = cJSON_GetObjectItem(params, "async-full-reports");
    if (NULL != json_sendIncrementalReport)
    {
      JSON_VALIDATE_JSON_POINTER(json_sendIncrementalReport, "async-full-reports", BVIEW_STATUS_INVALID_JSON);
      JSON_VALIDATE_JSON_AS_NUMBER(json_sendIncrementalReport, "async-full-reports");
      /* Copy the value */
      command.sendIncrementalReport = json_sendIncrementalReport->valueint;
      JSON_CHECK_VALUE_AND_CLEANUP (command.sendIncrementalReport, 0, 1);
      command.configMask = (command.configMask | (1 << BST_CONFIG_PARAMS_ASYNC_FULL_REP));
    }

    /* Parsing and Validating 'stats-in-percentage' from JSON buffer */
    json_statsInPercentage = cJSON_GetObjectItem(params, "stats-in-percentage");
    if (NULL != json_statsInPercentage)
    {
      JSON_VALIDATE_JSON_POINTER(json_statsInPercentage, "stats-in-percentage", BVIEW_STATUS_INVALID_JSON);
      JSON_VALIDATE_JSON_AS_NUMBER(json_statsInPercentage, "stats-in-percentage");
      /* Copy the value */
      command.statsInPercentage = json_statsInPercentage->valueint;
      /* Ensure  that the number 'stats-in-percentage' is within range of [0,1] */
      JSON_CHECK_VALUE_AND_CLEANUP (command.statsInPercentage, 0, 1);
      command.configMask = (command.configMask | (1 << BST_CONFIG_PARAMS_STATS_IN_PERCENT));
    }



    /* Send the 'command' along with 'asicId' and 'cookie' to the Application thread. */
    status = bstjson_configure_bst_feature_impl (cookie, asicId, id, &command);

    /* Free up any allocated resources and return status code */
    if (root != NULL)
    {
        cJSON_Delete(root);
    }

    return status;
}
