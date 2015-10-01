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

#include <time.h>
#include <inttypes.h>
#include <math.h>

#include "broadview.h"
#include "cJSON.h"

#include "configure_bst_feature.h"
#include "configure_bst_tracking.h"
#include "get_bst_report.h"
#include "configure_bst_thresholds.h"
#include "clear_bst_statistics.h"
#include "clear_bst_thresholds.h"

#include "bst.h"

#include "bst_json_memory.h"
#include "bst_json_encoder.h"
#include "bst_app.h"

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-bst-feature" REST API.
 *
 * @param[in]   asicId      ASIC for which this data is being encoded.
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
 *           bstjson_memory_free(). Failing to do so leads to memory leaks
 *********************************************************************/
BVIEW_STATUS bstjson_encode_get_bst_feature( int asicId,
                                            int method,
                                            const BSTJSON_CONFIGURE_BST_FEATURE_t *pData,
                                            uint8_t **pJsonBuffer
                                            )
{
    char *getBstFeatureTemplate = " {\
\"jsonrpc\": \"2.0\",\
\"method\": \"get-bst-feature\",\
\"asic-id\": \"%s\",\
\"version\": \"%d\",\
\"result\": {\
\"bst-enable\": %d,\
\"send-async-reports\": %d,\
\"collection-interval\": %d,\
\"stats-in-percentage\": %d,\
\"stat-units-in-cells\": %d,\
\"trigger-rate-limit\": %d,\
\"send-snapshot-on-trigger\": %d,\
\"trigger-rate-limit-interval\": %d,\
\"async-full-reports\": %d\
},\
\"id\": %d\
}";

    char *jsonBuf;
    char asicIdStr[JSON_MAX_NODE_LENGTH] = { 0 };
    BVIEW_STATUS status;

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : Request for Get-Bst-Feature \n");

    /* Validate Input Parameters */
    _JSONENCODE_ASSERT (pData != NULL);

    /* allocate memory for JSON */
    status = bstjson_memory_allocate(BSTJSON_MEMSIZE_RESPONSE, (uint8_t **) & jsonBuf);
    _JSONENCODE_ASSERT (status == BVIEW_STATUS_SUCCESS);

    /* clear the buffer */
    memset(jsonBuf, 0, BSTJSON_MEMSIZE_RESPONSE);

    /* convert asicId to external  notation */
    JSON_ASIC_ID_MAP_TO_NOTATION(asicId, &asicIdStr[0]);

    /* encode the JSON */
    snprintf(jsonBuf, BSTJSON_MEMSIZE_RESPONSE, getBstFeatureTemplate,
             &asicIdStr[0], BVIEW_JSON_VERSION, pData->bstEnable,
             pData->sendAsyncReports, pData->collectionInterval,
             pData->statsInPercentage, pData->statUnitsInCells, 
             pData->bstMaxTriggers, pData->sendSnapshotOnTrigger,
             pData->triggerTransmitInterval, (pData->sendIncrementalReport == 0)?1:0, method);

    /* setup the return value */
    *pJsonBuffer = (uint8_t *) jsonBuf;

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : Encoding complete [%d bytes] \n", (int)strlen(jsonBuf));

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_DUMPJSON, "BST-JSON-Encoder : %s \n", jsonBuf);

    return BVIEW_STATUS_SUCCESS;
}

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the
 *         "get-switch-properties" REST API.
 *
 * @param[in]   asicId      ASIC for which this data is being encoded.
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
 *           bstjson_memory_free(). Failing to do so leads to memory leaks
 *********************************************************************/
BVIEW_STATUS bstjson_encode_get_switch_properties ( int asicId,
                                            int method,
                                            BVIEW_SWITCH_PROPERTIES_t *pData,
                                            uint8_t **pJsonBuffer
                                            )
{
    char *getSwitchPropTemplate = "{\
\"jsonrpc\": \"2.0\",\
\"method\": \"get-switch-properties\",\
\"version\": \"%d\",\
\"result\": {\"number-of-asics\": %d,\
\"asic-info\":[%s],\
\"supported-features\":[%s],\
\"network-os\":\"%s\"},\
\"id\":%d }";

   char *asicInfoTemplate = "[\"%s\", \"%s\", %d],";
   char *featureTemplate = "\"%s\"";
   char *jsonBuf;
   BVIEW_STATUS status;
   char asicInfoStr[JSON_MAX_NODE_LENGTH]={0}; 
   char featureStr[JSON_MAX_NODE_LENGTH]={0}; 
   int asic =0;
   int len = 0;
   int totalLen = 0;

   _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : Request for Get-Switch-Properties \n");

    /* Validate Input Parameters */
   _JSONENCODE_ASSERT (pData != NULL);

    /* allocate memory for JSON */
   status = bstjson_memory_allocate(BSTJSON_MEMSIZE_RESPONSE, (uint8_t **) & jsonBuf);
   _JSONENCODE_ASSERT (status == BVIEW_STATUS_SUCCESS);

   /* clear the buffer */
   memset(jsonBuf, 0, BSTJSON_MEMSIZE_RESPONSE);
   
   for (asic = 0; asic < pData->numAsics ; asic++)
   {
     len  = snprintf (&asicInfoStr[totalLen],JSON_MAX_NODE_LENGTH,asicInfoTemplate, 
            pData->asicInfo[asic].asic_notation, 
            (pData->asicInfo[asic].asicType == BVIEW_ASIC_TYPE_TD2) ? "BCM56850" : "BCM56960", 
            pData->asicInfo[asic].numPorts);
     totalLen += len;
   }
   /* Remove comma after last element */
   asicInfoStr[totalLen -1] = '\0';
   
   len = 0;
   totalLen = 0;
   if (pData->featureMask & BVIEW_FEATURE_BST)
   {
     len = snprintf (&featureStr[totalLen], JSON_MAX_NODE_LENGTH,featureTemplate, "BST");
     totalLen += len;
   }
   if (pData->featureMask & BVIEW_FEATURE_PACKET_TRACE)
   {
     len =  snprintf (&featureStr[totalLen], JSON_MAX_NODE_LENGTH, "%s", ",");
     totalLen += len;
     len  = snprintf (&featureStr[totalLen], JSON_MAX_NODE_LENGTH, featureTemplate, "PT");
   } 
   
   /* encode the JSON */
   snprintf(jsonBuf, BSTJSON_MEMSIZE_RESPONSE, getSwitchPropTemplate,
            BVIEW_JSON_VERSION, pData->numAsics, asicInfoStr, featureStr, pData->networkOs,
            method);            

    /* setup the return value */
   *pJsonBuffer = (uint8_t *) jsonBuf;

   _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : Encoding complete [%d bytes] \n", (int)strlen(jsonBuf));

   _JSONENCODE_LOG(_JSONENCODE_DEBUG_DUMPJSON, "BST-JSON-Encoder : %s \n", jsonBuf);

   return BVIEW_STATUS_SUCCESS;
}

   
/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-bst-tracking" REST API.
 *
 * @param[in]   asicId      ASIC for which this data is being encoded.
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
 *           bstjson_memory_free(). Failing to do so leads to memory leaks
 *********************************************************************/

BVIEW_STATUS bstjson_encode_get_bst_tracking( int asicId,
                                             int method,
                                             const BSTJSON_CONFIGURE_BST_TRACKING_t *pData,
                                             uint8_t **pJsonBuffer
                                             )
{
    char *getBstTrackingTemplate = " {\
\"jsonrpc\": \"2.0\",\
\"method\": \"get-bst-tracking\",\
\"asic-id\": \"%s\",\
\"version\": \"%d\",\
\"result\": {\
\"track-peak-stats\" : %d, \
\"track-ingress-port-priority-group\" : %d, \
\"track-ingress-port-service-pool\" : %d, \
\"track-ingress-service-pool\" : %d, \
\"track-egress-port-service-pool\" : %d, \
\"track-egress-service-pool\" : %d, \
\"track-egress-uc-queue\" : %d, \
\"track-egress-uc-queue-group\" : %d, \
\"track-egress-mc-queue\" : %d, \
\"track-egress-cpu-queue\" : %d, \
\"track-egress-rqe-queue\" : %d, \
\"track-device\" : %d \
},\
\"id\": %d\
}";

    char *jsonBuf;
    BVIEW_STATUS status;
    char asicIdStr[JSON_MAX_NODE_LENGTH] = { 0 };

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : Request for Get-Bst-Tracking \n");

    /* Validate Input Parameters */
    _JSONENCODE_ASSERT (pData != NULL);

    /* allocate memory for JSON */
    status = bstjson_memory_allocate(BSTJSON_MEMSIZE_RESPONSE, (uint8_t **) & jsonBuf);
    _JSONENCODE_ASSERT (status == BVIEW_STATUS_SUCCESS);

    /* clear the buffer */
    memset(jsonBuf, 0, BSTJSON_MEMSIZE_RESPONSE);

    /* convert asicId to external  notation */
    JSON_ASIC_ID_MAP_TO_NOTATION(asicId, &asicIdStr[0]);

    /* encode the JSON */
    snprintf(jsonBuf, BSTJSON_MEMSIZE_RESPONSE, getBstTrackingTemplate,
             &asicIdStr[0], BVIEW_JSON_VERSION, pData->trackPeakStats,
             pData->trackIngressPortPriorityGroup,
             pData->trackIngressPortServicePool,
             pData->trackIngressServicePool,
             pData->trackEgressPortServicePool,
             pData->trackEgressServicePool,
             pData->trackEgressUcQueue,
             pData->trackEgressUcQueueGroup,
             pData->trackEgressMcQueue,
             pData->trackEgressCpuQueue,
             pData->trackEgressRqeQueue,
             pData->trackDevice, method);

    /* setup the return value */
    *pJsonBuffer = (uint8_t *) jsonBuf;

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : Encoding complete [%d bytes] \n", (int)strlen(jsonBuf));

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_DUMPJSON, "BST-JSON-Encoder : %s \n", jsonBuf);

    return BVIEW_STATUS_SUCCESS;
}

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-bst-report" REST API - device part.
 *
 *********************************************************************/

static BVIEW_STATUS _jsonencode_report_device ( char *jsonBuf,
                                               const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *previous,
                                               const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *current,
                                               const BSTJSON_REPORT_OPTIONS_t *options,
                                               const BVIEW_ASIC_CAPABILITIES_t *asic,
                                               int bufLen,
                                               int *length)
{

  char *getBstDeviceReportTemplate = "{ \"realm\" : \"device\", \"data\" : % " PRIu64 "}";
  /* Since this is an internal function, with all parameters validated already, 
   * we jump to the logic straight-away 
   */
  uint64_t data;
  *length = 0;
  uint64_t defaultVal = 0;

  _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding device data \n");

  /* if collector is not interested in device stats, ignore it*/
  if (options->includeDevice == false)
  {
    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Device data not needed \n");
    return BVIEW_STATUS_SUCCESS;
  }

  /* if there is no change in stats since we reported last time, ignore it*/
  if  ((previous != NULL) && (current->device.bufferCount == previous->device.bufferCount))
  {
    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Device data %" PRIu64 " has not changed since last reading \n",
        current->device.bufferCount);
    return BVIEW_STATUS_SUCCESS;
  }
  /* data to be sent to collector */
  data = current->device.bufferCount;
  defaultVal = options->bst_defaults_ptr->device.bufferCount;

  bst_json_convert_data(options, asic, &data, defaultVal);

  /* encode the JSON */
  *length = snprintf(jsonBuf, bufLen, getBstDeviceReportTemplate, data);
  _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding device data [%d] complete \n", *length);

  return BVIEW_STATUS_SUCCESS;
}

static BVIEW_STATUS bstjson_realm_to_indices_get(char *realm, char *index1, char *index2)
{
  static BSTJSON_REALM_INDEX_t bst_realm_indices_map [] = {
    {"device" ,NULL, NULL},
    {"ingress-service-pool", "service-pool", NULL},
    {"ingress-port-service-pool", "port", "service-pool" },
    {"ingress-port-priority-group", "port", "priority-group"},
    {"egress-port-service-pool", "port", "service-pool"},
    {"egress-service-pool", "service-pool", NULL},
    {"egress-uc-queue", "queue", NULL},
    {"egress-uc-queue-group", "queue-group", NULL},
    {"egress-mc-queue", "queue", NULL},
    {"egress-cpu-queue", "queue", NULL},
    {"egress-rqe-queue", "queue", NULL}
  };

  unsigned int i;

  if ((NULL == realm)||
      (NULL == index1) ||
      (NULL == index2))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  for (i = 0; i < 11; i++)
  {
    if (0 == strcmp(realm, bst_realm_indices_map[i].realm))
    {
      if (NULL != bst_realm_indices_map[i].index1)
      {
        strncpy(index1, bst_realm_indices_map[i].index1, strlen(bst_realm_indices_map[i].index1));
      }

      if (NULL != bst_realm_indices_map[i].index2)
      {
        strncpy(index2, bst_realm_indices_map[i].index2, strlen(bst_realm_indices_map[i].index2));
      }

      return BVIEW_STATUS_SUCCESS;
    }
  }

  return BVIEW_STATUS_FAILURE;

}

BVIEW_STATUS bstjson_encode_trigger_realm_index_info(char *buffer, int asicId,
                                                     int bufLen,int *length,
                                                     char *index, int port, int queue)
{
  char portStr[JSON_MAX_NODE_LENGTH] = { 0 };

  char * portTemplate = "\"port\" : \"%s\",";
  char * indexTemplate = "\"%s\" : %d,";

  if (index != NULL)
  {
    if (0 == strcmp("port", index))
    {
      /* convert the port to an external representation */
      memset(&portStr[0], 0, JSON_MAX_NODE_LENGTH);
      JSON_PORT_MAP_TO_NOTATION(port, asicId, &portStr[0]);
       *length = snprintf(buffer, bufLen, portTemplate, &portStr[0]);
    }
    else
    {
       *length = snprintf(buffer, bufLen, indexTemplate, index, queue);
    }
    return BVIEW_STATUS_SUCCESS;
  }
    return BVIEW_STATUS_FAILURE;

}
/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-bst-report" REST API.
 *
 * @param[in]   asicId      ASIC for which this data is being encoded.
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
 *           bstjson_memory_free(). Failing to do so leads to memory leaks
 *********************************************************************/

BVIEW_STATUS bstjson_encode_get_bst_report ( int asicId,
                                            int method,
                                            const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *previous,
                                            const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *current,
                                            const BSTJSON_REPORT_OPTIONS_t *options,
                                            const BVIEW_ASIC_CAPABILITIES_t *asic,
                                            const BVIEW_TIME_t *time,
                                            uint8_t **pJsonBuffer
                                            )
{
    char *jsonBuf, *start;
    BVIEW_STATUS status;
    int bufferLength = BSTJSON_MEMSIZE_REPORT;
    int tempLength = 0;

    time_t report_time;
    struct tm *timeinfo;
    char timeString[64];
    char asicIdStr[JSON_MAX_NODE_LENGTH] = { 0 };
    char index1[256];
    char index2[256];



    char *getBstReportStart = " { \
\"jsonrpc\": \"2.0\",\
\"method\": \"%s\",\
\"asic-id\": \"%s\",\
\"version\": \"%d\",\
\"time-stamp\": \"%s\",\
\"report\": [ \
";

    char *getBstTriggerReportStart = " { \
\"jsonrpc\": \"2.0\",\
\"method\": \"%s\",\
\"asic-id\": \"%s\",\
\"version\": \"%d\",\
\"time-stamp\": \"%s\",\
\"realm\": \"%s\",\
\"counter\": \"%s\",\
";


    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : Request for Get-Bst-Report \n");

    /* Validate Input Parameters */
    _JSONENCODE_ASSERT (options != NULL);
    _JSONENCODE_ASSERT (current != NULL);
    _JSONENCODE_ASSERT (time != NULL);
    _JSONENCODE_ASSERT (asic != NULL);

    /* obtain the time */
    memset(&timeString, 0, sizeof (timeString));
    report_time = *(time_t *) time;
    timeinfo = localtime(&report_time);
    strftime(timeString, 64, "%Y-%m-%d - %H:%M:%S ", timeinfo);

    /* allocate memory for JSON */
    status = bstjson_memory_allocate(BSTJSON_MEMSIZE_REPORT, (uint8_t **) & jsonBuf);
    _JSONENCODE_ASSERT (status == BVIEW_STATUS_SUCCESS);

    start = jsonBuf;

    /* clear the buffer */
    memset(jsonBuf, 0, BSTJSON_MEMSIZE_REPORT);

    /* convert asicId to external  notation */
    JSON_ASIC_ID_MAP_TO_NOTATION(asicId, &asicIdStr[0]);

    /* fill the header */
    /* encode the JSON */

    if (options->reportTrigger == false)
    {
      tempLength = snprintf(jsonBuf, bufferLength, getBstReportStart,
          (options->reportThreshold == true) ? "get-bst-thresholds" :"get-bst-report",
          &asicIdStr[0], BVIEW_JSON_VERSION, timeString);
      bufferLength -= tempLength;
      jsonBuf += tempLength;
    }
    else
    {
      memset(index1, 0, sizeof(index1));
      memset(index2, 0, sizeof(index2));
      if (BVIEW_STATUS_SUCCESS != bstjson_realm_to_indices_get((char *)options->triggerInfo.realm, &index1[0], &index2[0]))
      {
        return BVIEW_STATUS_INVALID_PARAMETER;
      }
      tempLength = snprintf(jsonBuf, bufferLength, getBstTriggerReportStart,
          "trigger-report",
          &asicIdStr[0], BVIEW_JSON_VERSION, timeString, options->triggerInfo.realm, options->triggerInfo.counter);

      jsonBuf += tempLength;
      bufferLength -= tempLength;
      if (0 != index1[0])
      {
        status = bstjson_encode_trigger_realm_index_info(jsonBuf, asicId, bufferLength, &tempLength, &index1[0], 
            options->triggerInfo.port, options->triggerInfo.queue);
        _JSONENCODE_ASSERT_ERROR((status == BVIEW_STATUS_SUCCESS), status);
        bufferLength -= tempLength;
        jsonBuf += tempLength;
      }


      if (0 != index2[0])
      {
        status = bstjson_encode_trigger_realm_index_info(jsonBuf, asicId, bufferLength, &tempLength, &index2[0], 
            options->triggerInfo.port, options->triggerInfo.queue);
        _JSONENCODE_ASSERT_ERROR((status == BVIEW_STATUS_SUCCESS), status);
        bufferLength -= tempLength;
        jsonBuf += tempLength;
      }

      tempLength = snprintf(jsonBuf, bufferLength, "\"report\" : [" ); 
      bufferLength -= tempLength;
      jsonBuf += tempLength; 
    }

    /* get the device report */
    status = _jsonencode_report_device(jsonBuf, previous, current, options, asic, bufferLength, &tempLength);
    _JSONENCODE_ASSERT_ERROR((status == BVIEW_STATUS_SUCCESS), status);

    if (tempLength)
    {
        bufferLength -= tempLength;
        jsonBuf += tempLength;

        tempLength = snprintf(jsonBuf, bufferLength, " ,");

        bufferLength -= tempLength;
        jsonBuf += tempLength;
    }

    /* if any of the ingress encodings are required, add them to report */
    if (options->includeIngressPortPriorityGroup ||
        options->includeIngressPortServicePool ||
        options->includeIngressServicePool)
    {
        status = _jsonencode_report_ingress(jsonBuf, asicId, previous, current, options, asic, bufferLength, &tempLength);
        _JSONENCODE_ASSERT_ERROR((status == BVIEW_STATUS_SUCCESS), status);

        /* adjust the buffer */
        bufferLength -= (tempLength);
        jsonBuf += (tempLength);

    }

    /* if any of the egress encodings are required, add them to report */
    if (options->includeEgressCpuQueue ||
        options->includeEgressMcQueue ||
        options->includeEgressPortServicePool ||
        options->includeEgressRqeQueue ||
        options->includeEgressServicePool ||
        options->includeEgressUcQueue ||
        options->includeEgressUcQueueGroup )
    {
        status = _jsonencode_report_egress(jsonBuf, asicId, previous, current, options, asic, bufferLength, &tempLength);
        _JSONENCODE_ASSERT_ERROR((status == BVIEW_STATUS_SUCCESS), status);

        /* adjust the buffer */
        bufferLength -= (tempLength);
        jsonBuf += (tempLength);

    }

    /* finalizing the report */

    bufferLength -= 1;
    jsonBuf -= 1;

    if (jsonBuf[0] == 0)
    {
        bufferLength -= 1;
        jsonBuf--;
    }

    tempLength = snprintf(jsonBuf, bufferLength, " ] } ");

    *pJsonBuffer = (uint8_t *) start;

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : Request for Get-Bst-Report Complete [%d] bytes \n", (int)strlen(start));

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_DUMPJSON, "BST-JSON-Encoder : %s \n", start);


    return BVIEW_STATUS_SUCCESS;
}

uint64_t round_int( double r ) {
      return (r > 0.0) ? (r + 0.5) : (r - 0.5); 
}
/******************************************************************* 
   Utility function to convert the data based on config 
********************************************************************/
BVIEW_STATUS bst_json_convert_data(const BSTJSON_REPORT_OPTIONS_t *options,
                                          const BVIEW_ASIC_CAPABILITIES_t *asic,
                                          uint64_t *value, uint64_t defVal)
{
  double percentage =0;
  uint64_t data = 0;

  if ((NULL == options) ||
      (NULL == asic) ||
      (NULL == value))
  {
    return BVIEW_STATUS_INVALID_PARAMETER;
  }

  data = *value;


  /* Report is threshold. */
  if (true == options->reportThreshold)
  {
    if (true == options->statUnitsInCells)
    {
      /* threshold comes in bytes from asic */
      data = data / (asic->cellToByteConv);
    }
  }
  else
  {
    /* report is stats */
    if (true == options->statsInPercentage)
    {
      if (0 == defVal)
      {
        data = 0;
      }
      else
      {
        /* we just need the percentage of the configured value */
        percentage = ((double)(data * 100))/((double)(defVal));
        data = round_int(percentage);
      }
    }
    else
    {
      /* conversion to bytes or cells based on config */
      if (false == options->statUnitsInCells)
      {
     /* check if we need to convert the data to cells
        the report always comes in cells from asic */
       data = data * (asic->cellToByteConv);
      }
    }
  }

  *value = data;

  return BVIEW_STATUS_SUCCESS;
}
