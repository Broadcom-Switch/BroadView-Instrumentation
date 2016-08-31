/*********************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename configure_black_hole_detection.c 
  *
  * @purpose BroadView BHD JSON decode function 
  *
  * @component Black hole detection 
  *
  * @comments
  *
  * @create 3/7/2016
  *
  * @author 
  * @end
  *
  **********************************************************************/


/* Include Header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "broadview.h"
#include "asic.h"
#include "port_utils.h"
#include "json.h"

#include "cJSON.h"
#include "configure_black_hole_detection.h"

/******************************************************************
 * @brief  REST API Handler (Generated Code)
 *
 * @param[in]    cookie     Context for the API from Web server
 * @param[in]    jsonBuffer Raw Json Buffer
 * @param[in]    bufLength  Json Buffer length (bytes)
 *                           
 * @retval   BVIEW_STATUS_SUCCESS  JSON Parsed and parameters passed to BHD APP
 * @retval   BVIEW_STATUS_INVALID_JSON  JSON is malformatted, or doesn't 
 * 					have necessary data.
 * @retval   BVIEW_STATUS_INVALID_PARAMETER Invalid input parameter
 *
 * @note     See the _impl() function for info passing to BHD APP
 *********************************************************************/
BVIEW_STATUS bhdjson_configure_black_hole_detection (void *cookie, char *jsonBuffer, int bufLength)
{

  /* Local Variables for JSON Parsing */
  cJSON *json_jsonrpc, *json_method, *json_asicId;
  cJSON *json_id, *json_bhdEnable;
  cJSON *root, *params;
  /* Local non-command-parameter JSON variable declarations */
  char jsonrpc[JSON_MAX_NODE_LENGTH] = {0};
  char method[JSON_MAX_NODE_LENGTH] = {0};
  int asicId = 0, id = 0;

  /* Local variable declarations */
  BVIEW_STATUS status = BVIEW_STATUS_SUCCESS;
  BHDJSON_CONFIGURE_BLACK_HOLE_DETECTION_t command;

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
  /* Ensure that 'method' in the JSON equals "configure-black-hole-detection" */
  JSON_COMPARE_STRINGS_AND_CLEANUP ("method", &method[0], "black-hole-detection-enable");


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

  /* Parsing and Validating 'black-hole-detection-enable' from JSON buffer */
  json_bhdEnable = cJSON_GetObjectItem(params, "enable");
  JSON_VALIDATE_JSON_POINTER(json_bhdEnable, "enable", BVIEW_STATUS_INVALID_JSON);
  JSON_VALIDATE_JSON_AS_NUMBER(json_bhdEnable, "enable");
  /* Copy the value */
  command.enable = json_bhdEnable->valueint;
  /* Ensure  that the number 'bst-enable' is within range of [0,1] */
  JSON_CHECK_VALUE_AND_CLEANUP (command.enable, 0, 1);


  /* Send the 'command' along with 'asicId' and 'cookie' to the Application thread. */
  status = bhdjson_configure_black_hole_detection_impl (cookie, asicId, id, &command);

  /* Free up any allocated resources and return status code */
  if (root != NULL)
  {
    cJSON_Delete(root);
  }

  return status;

}
