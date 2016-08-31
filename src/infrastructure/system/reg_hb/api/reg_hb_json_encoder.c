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

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <inttypes.h>
#include "broadview.h"
#include "cJSON.h"
#include "configure_reg_hb_feature.h"
#include "cancel_request.h"
#include "system.h"
#include "system_utils.h"
#include "reg_hb_json_encoder.h"
#include "system_utils_json_memory.h"

 /*********************************************************************
 * creates a JSON buffer using the supplied data for the
 *         "get-system-feature" REST API.
 *
 * @param[in]   method      Method ID (from original request) that needs
 *                          to be encoded in JSON.
 * @param[in]   pData       Data structure holding the required parameters.
 * @param[out]  pJsonBuffer Filled-in JSON buffer
 *
 * @retval   BVIEW_STATUS_SUCCESS  Data is encoded into JSON successfully
 * @retval   BVIEW_STATUS_RESOURCE_NOT_AVAILABLE  Internal Error
 * @retval   BVIEW_STATUS_INVALID_PARAMETER  Invalid input parameter
 * @retval   BVIEW_STATUS_OUTOFMEMORY  No available memory to create JSON buffer
 *
 * @note     The returned json-encoded-buffer should be freed using the
 *           reg_hb_json_memory_free(). Failing to do so leads to memory leaks
 *********************************************************************/
BVIEW_STATUS reg_hb_json_encode_get_feature( int method,
    const REG_HB_JSON_CONFIGURE_FEATURE_t *pData,
    uint8_t **pJsonBuffer
    )
{
  char *getSystemUtilsFeatureTemplate = " {\
                                 \"jsonrpc\": \"2.0\",\
                                 \"method\": \"get-system-feature\",\
                                 \"version\": \"%d\",\
                                 \"result\": {\
                                 \"heartbeat-enable\": %d,\
                                 \"msg-interval\": %d\
},\
  \"id\": %d\
  }";

char *jsonBuf;
BVIEW_STATUS status;

_SYSTEM_UTILS_JSONENCODE_LOG(_SYSTEM_UTILS_JSONENCODE_DEBUG_TRACE, "SYSTEM-UTIL-JSON-Encoder : Request for Get-System-Feature \n");

/* Validate Input Parameters */
_SYSTEM_UTILS_JSONENCODE_ASSERT (pData != NULL);

/* allocate memory for JSON */
status = system_utils_json_memory_allocate(SYSTEM_UTILS_JSON_MEMSIZE_RESPONSE, (uint8_t **) & jsonBuf);
_SYSTEM_UTILS_JSONENCODE_ASSERT (status == BVIEW_STATUS_SUCCESS);

/* clear the buffer */
memset(jsonBuf, 0, SYSTEM_UTILS_JSON_MEMSIZE_RESPONSE);

/* encode the JSON */
snprintf(jsonBuf, SYSTEM_UTILS_JSON_MEMSIZE_RESPONSE, getSystemUtilsFeatureTemplate,
    BVIEW_JSON_VERSION, pData->enable, pData->interval, method);

/* setup the return value */
*pJsonBuffer = (uint8_t *) jsonBuf;

_SYSTEM_UTILS_JSONENCODE_LOG(_SYSTEM_UTILS_JSONENCODE_DEBUG_TRACE, "SYSTEM-JSON-Encoder : Encoding complete [%d bytes] \n", (int)strlen(jsonBuf));

_SYSTEM_UTILS_JSONENCODE_LOG(_SYSTEM_UTILS_JSONENCODE_DEBUG_DUMPJSON, "SYSTEM-JSON-Encoder : %s \n", jsonBuf);

return BVIEW_STATUS_SUCCESS;
}

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the
 *         "get-switch-properties" REST API.
 *
 * @param[in]   method      Method ID (from original request) that needs
 *                          to be encoded in JSON.
 * @param[in]   pData       Data structure holding the required parameters.
 * @param[out]  pJsonBuffer Filled-in JSON buffer
 *
 * @retval   BVIEW_STATUS_SUCCESS  Data is encoded into JSON successfully
 * @retval   BVIEW_STATUS_RESOURCE_NOT_AVAILABLE  Internal Error
 * @retval   BVIEW_STATUS_INVALID_PARAMETER  Invalid input parameter
 * @retval   BVIEW_STATUS_OUTOFMEMORY  No available memory to create JSON buffer
 *
 * @note     The returned json-encoded-buffer should be freed using the
 *           system_utils_json_memory_free(). Failing to do so leads to memory leaks
 *********************************************************************/
BVIEW_STATUS reg_hb_json_encode_get_switch_properties (int method,
    BVIEW_SWITCH_PROPERTIES_t *pData,
    uint8_t **pJsonBuffer
    )
{
  char *getSwitchPropTemplate = "{\
                                 \"jsonrpc\": \"2.0\",\
                                 \"method\": \"get-switch-properties\",\
                                 \"version\": \"%d\",\
                                 \"time-stamp\": \"%s\",\
                                 \"result\": {\"number-of-asics\": %d,\
                                 \"asic-info\":[%s],\
                                 \"supported-features\":[%s],\
                                 \"network-os\":\"%s\",\
                                 \"uid\":\"%s\",\
                                 \"agent-ip\":\"%s\",\
                                 \"agent-port\":\"%d\",\
                                 \"agent-sw-version\":\"%s\"},\
                                 \"id\":%d \
}";
  char *getSwitchPropAsyncTemplate = "{\
                                 \"jsonrpc\": \"2.0\",\
                                 \"method\": \"get-switch-properties\",\
                                 \"version\": \"%d\",\
                                 \"time-stamp\": \"%s\",\
                                 \"result\": {\"number-of-asics\": %d,\
                                 \"asic-info\":[%s],\
                                 \"supported-features\":[%s],\
                                 \"network-os\":\"%s\",\
                                 \"uid\":\"%s\",\
                                 \"agent-ip\":\"%s\",\
                                 \"agent-port\":\"%d\",\
                                 \"agent-sw-version\":\"%s\"}\
}";
  char *asicInfoTemplate = "[\"%s\", \"%s\", %d],";
  char *featureTemplate = "\"%s\"";
  char *jsonBuf;
  BVIEW_STATUS status;
  char asicInfoStr[JSON_MAX_NODE_LENGTH]={0};
  char featureStr[JSON_MAX_NODE_LENGTH]={0};
  int asic =0;
  int len = 0;
  int array_len = 0;
  int totalLen = 0;
  time_t report_time;
  struct tm *timeinfo;
  unsigned int  featureCount = 0;
  char timeString[BVIEW_TIME_BUFFER_SIZE];
  char buf[JSON_MAX_NODE_LENGTH]={0};
  

  _SYSTEM_UTILS_JSONENCODE_LOG(_SYSTEM_UTILS_JSONENCODE_DEBUG_TRACE, "SYSTEM_UTILS-JSON-Encoder : Request for Get-Switch-Properties \n");

  /* Validate Input Parameters */
  _SYSTEM_UTILS_JSONENCODE_ASSERT (pData != NULL);

  /* allocate memory for JSON */
  status = system_utils_json_memory_allocate(SYSTEM_UTILS_JSON_MEMSIZE_RESPONSE, (uint8_t **) & jsonBuf);
  _SYSTEM_UTILS_JSONENCODE_ASSERT (status == BVIEW_STATUS_SUCCESS);

  /* clear the buffer */
  memset(jsonBuf, 0, SYSTEM_UTILS_JSON_MEMSIZE_RESPONSE);

    /* obtain the time */
    memset(&timeString, 0, sizeof (timeString));
    report_time = *(time_t *) &pData->timeStamp;
    timeinfo = localtime(&report_time);
    strftime(timeString, 64, "%Y-%m-%d - %H:%M:%S", timeinfo);

 array_len = JSON_MAX_NODE_LENGTH-1; 
  for (asic = 0; asic < pData->numAsics ; asic++)
  {
    if (pData->asicInfo[asic].asicType == BVIEW_ASIC_TYPE_TD2) 
    {
      sprintf (buf, "%s","BCM56850");
    } 
    else if (pData->asicInfo[asic].asicType == BVIEW_ASIC_TYPE_TH)
    {
      sprintf (buf, "%s","BCM56960");  
    }
    else if (pData->asicInfo[asic].asicType == BVIEW_ASIC_TYPE_QUMRAN)
    {
      sprintf (buf, "%s","BCM88375");
    }
    else 
    {
      sprintf (buf, "%s","");
    }
 
    len  = snprintf (&asicInfoStr[totalLen], array_len, asicInfoTemplate,
        pData->asicInfo[asic].asic_notation,
        buf,
        pData->asicInfo[asic].numPorts);
    totalLen += len;
    array_len = array_len - totalLen;
  }
  /* Remove comma after last element */
  asicInfoStr[totalLen-1] = '\0';

  array_len = JSON_MAX_NODE_LENGTH -1;
  len = 0;
  totalLen = 0;
  if (pData->featureMask & BVIEW_FEATURE_BST)
  {
    len = snprintf (&featureStr[totalLen], array_len,featureTemplate, "BST");
    totalLen += len;
    featureCount++;
    array_len = array_len - totalLen;
  }
  if (pData->featureMask & BVIEW_FEATURE_PACKET_TRACE)
  {
    if (featureCount != 0)
    {
      len =  snprintf (&featureStr[totalLen], array_len, "%s", ",");
      totalLen += len;
    }
    len  = snprintf (&featureStr[totalLen], array_len, featureTemplate, "PT");
    totalLen += len;
    featureCount++;
    array_len = array_len - totalLen;
  }

  if (pData->featureMask & BVIEW_FEATURE_LIVE_PT)
  {
    if (featureCount != 0)
    {
      len =  snprintf (&featureStr[totalLen], array_len, "%s", ",");
      totalLen += len;
    }
    len  = snprintf (&featureStr[totalLen], array_len, featureTemplate, "Live-PT");
    totalLen += len;
    featureCount++;
    array_len = array_len - totalLen;
  }

  if (pData->featureMask & BVIEW_FEATURE_BHD)
  {
    if (featureCount != 0)
    {
      len =  snprintf (&featureStr[totalLen], array_len, "%s", ",");
      totalLen += len;
    }
    len  = snprintf (&featureStr[totalLen], array_len, featureTemplate, "BHD");
    totalLen += len;
    featureCount++;
    array_len = array_len - totalLen;
  }


  featureStr[totalLen] = '\0';

 if (0 == method)
 {
  /* encode the JSON */
  snprintf(jsonBuf, SYSTEM_UTILS_JSON_MEMSIZE_RESPONSE, getSwitchPropAsyncTemplate,
      BVIEW_JSON_VERSION, timeString, pData->numAsics, asicInfoStr, featureStr, pData->networkOs,
      pData->uid, pData->agent_ipaddr, pData->agent_port, pData->agent_sw_version);
 }
 else
 {
  /* encode the JSON */
  snprintf(jsonBuf, SYSTEM_UTILS_JSON_MEMSIZE_RESPONSE, getSwitchPropTemplate,
      BVIEW_JSON_VERSION, timeString, pData->numAsics, asicInfoStr, featureStr, pData->networkOs,
      pData->uid, pData->agent_ipaddr, pData->agent_port, pData->agent_sw_version, method);

 }


  /* setup the return value */
  *pJsonBuffer = (uint8_t *) jsonBuf;

  _SYSTEM_UTILS_JSONENCODE_LOG(_SYSTEM_UTILS_JSONENCODE_DEBUG_TRACE, "SYSTEM-JSON-Encoder : Encoding complete [%d bytes] \n", (int)strlen(jsonBuf));

  _SYSTEM_UTILS_JSONENCODE_LOG(_SYSTEM_UTILS_JSONENCODE_DEBUG_DUMPJSON, "SYSTEM-JSON-Encoder : %s \n", jsonBuf);

  return BVIEW_STATUS_SUCCESS;
}


