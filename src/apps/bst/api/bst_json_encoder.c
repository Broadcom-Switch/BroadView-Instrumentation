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
#include "get_bst_cgsn_drop_counters.h"
#include "common/platform_spec.h"
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
\"stat-units-in-cells\": %d,\
\"trigger-rate-limit\": %d,\
\"send-snapshot-on-trigger\": %d,\
\"trigger-rate-limit-interval\": %d,\
\"async-full-reports\": %d,\
\"stats-in-percentage\": %d\
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
             pData->statUnitsInCells, 
             pData->bstMaxTriggers, pData->sendSnapshotOnTrigger,
             pData->triggerTransmitInterval, (pData->sendIncrementalReport == 0)?1:0, 
             pData->statsInPercentage, method);

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

BVIEW_STATUS _jsonencode_report_device ( char *jsonBuf,
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
  uint64_t maxBufVal = 0;

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
  maxBufVal = options->bst_max_buffers_ptr->device.data.maxBuf;
  bst_json_convert_data(options, asic, &data, maxBufVal);

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
    if (status != BVIEW_STATUS_SUCCESS)
    {
      *pJsonBuffer = (uint8_t *) start;
    }
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
      if (status != BVIEW_STATUS_SUCCESS)
      {
        *pJsonBuffer = (uint8_t *) start;
      }
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
      if (status != BVIEW_STATUS_SUCCESS)
      {
        *pJsonBuffer = (uint8_t *) start;
      }
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
                                          uint64_t *value, uint64_t maxBufVal)
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
    if (true == options->statsInPercentage)
    {
      if (0 == maxBufVal)
      {
        data = 0;
      }
      else
      {
        data = data/(asic->cellToByteConv);
        /* we just need the percentage of the configured value */
        percentage = ((double)(data * 100))/((double)(maxBufVal));
        data = round_int(percentage);
        if (100 < data)
        {
          data = 100;
        }
      }
    }
    else
    {

      if (true == options->statUnitsInCells)
      {
        /* threshold comes in bytes from asic */
        data = data / (asic->cellToByteConv);
      }
    }
  }
  else
  {
    /* report is stats */
    if (true == options->statsInPercentage)
    {
      if (0 == maxBufVal)
      {
        data = 0;
      }
      else
      {
        /* we just need the percentage of the configured value */
        percentage = ((double)(data * 100))/((double)(maxBufVal));
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

BVIEW_STATUS bst_json_cgsn_req_str_get(BVIEW_BST_CGSN_REQ_TYPE_t val, char *str)
{
  if (NULL == str)
    return BVIEW_STATUS_INVALID_PARAMETER;

  unsigned int i = 0;

  const BVIEW_BST_CGSN_DROP_REQ_MAP_t cgsn_drp_req_map[] = {
    {"top-drops", BVIEW_BST_CGSN_TOP_DROPS},
    {"top-port-queue-drops", BVIEW_BST_CGSN_TOP_PRT_Q_DROPS},
    {"port-drops", BVIEW_BST_CGSN_PRT_DROPS},
    {"port-queue-drops", BVIEW_BST_CGSN_PRT_Q_DROPS}
  };
  for (i = 0; i < (sizeof(cgsn_drp_req_map)/sizeof(BVIEW_BST_CGSN_DROP_REQ_MAP_t)); i++)
  {
    if (val == cgsn_drp_req_map[i].req)
    {
      strncpy(str, cgsn_drp_req_map[i].req_str, strlen(cgsn_drp_req_map[i].req_str));
      return BVIEW_STATUS_SUCCESS;
    }
  }

  return BVIEW_STATUS_FAILURE;

}


/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-bst-congestion-drop-counters" REST API.
 *
 * @param[in]   asicId      ASIC for which this data is being encoded.
 * @param[in]   pData       Data structure holding the required parameters.
 * @param[in]   asic        pointer to asic capabilities.
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

BVIEW_STATUS bstjson_encode_get_cgsn_drop_ctrs ( int asicId,
                                            const void *in_ptr,
                                            const BVIEW_ASIC_CAPABILITIES_t *asic,
                                            uint8_t **pJsonBuffer
                                            )
{
  char *jsonBuf, *start;
  BVIEW_STATUS status;
  int bufferLength = BSTJSON_MEMSIZE_REPORT;
  int tempLength = 0;
  BVIEW_BST_CGSN_DROPS_t *pData = NULL;

  time_t report_time;
  struct tm *timeinfo;
  char timeString[64];
  char asicIdStr[JSON_MAX_NODE_LENGTH] = { 0 };
  char portStr[256] = {0};
  char queueStr[256] = {0};
  char req_str[256] = {0};
  unsigned int i = 0, prt = 0, queue = 0, begin = 0, end = 0;
  bool first = true;
  bool min_one= false;



  char *getBstCgsnReportStart = " { \
                                 \"jsonrpc\": \"2.0\",\
                                 \"method\": \"%s\",\
                                 \"asic-id\": \"%s\",\
                                 \"version\": \"%d\",\
                                 \"time-stamp\": \"%s\",\
                                 \"report\": [ {\
                                 \"report-type\": \"%s\",\
                                 \"data\": [\
                                 ";

  char *portTemplate = " { \
                        \"port\": \"%s\",\
                        \"data\":  % " PRIu64 " }\
                        ";

  char *portQueueTemplate = " { \
                             \"port\": \"%s\",\
                             \"queue-type\": \"%s\",\
                             \"data\": [ \
                             ";

  char *portQueueDataTemplate = "[ %d, % " PRIu64 "] \
                                 ";


  _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : Request for get-bst-congestion-drop-counters \n");

  pData = (BVIEW_BST_CGSN_DROPS_t *)in_ptr;
  /* Validate Input Parameters */
  _JSONENCODE_ASSERT (pData != NULL);
  _JSONENCODE_ASSERT (asic != NULL);


  /* obtain the time */
  memset(&timeString, 0, sizeof (timeString));
  report_time = pData->tv;
  timeinfo = localtime(&report_time);
  strftime(timeString, 64, "%Y-%m-%d - %H:%M:%S ", timeinfo);

  if (BVIEW_STATUS_SUCCESS != 
      bst_json_cgsn_req_str_get(pData->rcvd_req.req_type, &req_str[0]))
    return BVIEW_STATUS_FAILURE;

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

  tempLength = snprintf(jsonBuf, bufferLength,getBstCgsnReportStart,
      "get-bst-congestion-drop-counters", 
      &asicIdStr[0], BVIEW_JSON_VERSION, timeString, &req_str[0]);

  bufferLength -= tempLength;
  jsonBuf += tempLength;

  /* if the request type is to get 
     top -n records, then sort the data */
  if ((BVIEW_BST_CGSN_TOP_DROPS == pData->rcvd_req.req_type) ||
      (BVIEW_BST_CGSN_TOP_PRT_Q_DROPS == pData->rcvd_req.req_type))
  {
    bst_sort_records(asicId, pData->rcvd_req.count);
  }

  if ((BVIEW_BST_CGSN_TOP_DROPS == pData->rcvd_req.req_type) ||
      (BVIEW_BST_CGSN_PRT_DROPS == pData->rcvd_req.req_type))
  {
    if (BVIEW_BST_CGSN_PRT_DROPS == pData->rcvd_req.req_type)
    {
      begin = BVIEW_BST_TOTAL_DROP_CTR_INDEX_GET(1);
      end = BVIEW_BST_TOTAL_DROP_CTR_INDEX_GET(asic->numPorts);
    }
    else
    {
      begin = 0;
      end = pData->rcvd_req.count;
    }

    first = true;
    for (i = begin; i <= end; i++)
    {
      if (0 != pData->drop_ctrs[i].ctr)
      {
        if (true == first)
        {
          first = false;
        }
        else
        {
          tempLength = snprintf(jsonBuf, bufferLength, ",");
          bufferLength -= tempLength;
          jsonBuf += tempLength;
          
        }
        /* convert the port to an external representation */
        memset(&portStr[0], 0, JSON_MAX_NODE_LENGTH);
        JSON_PORT_MAP_TO_NOTATION(pData->drop_ctrs[i].port, asicId, &portStr[0]);

        tempLength = snprintf(jsonBuf, bufferLength,portTemplate,
            &portStr[0], pData->drop_ctrs[i].ctr);

        bufferLength -= tempLength;
        jsonBuf += tempLength;
      }
    }
  }

  else if (BVIEW_BST_CGSN_TOP_PRT_Q_DROPS == pData->rcvd_req.req_type)
  {
    begin = 0; 
    end = pData->rcvd_req.count;
    for (i = begin; i <= end; i++)
    {
      if (0 != pData->drop_ctrs[i].ctr)
      {
        if (BVIEW_BST_CGSN_UCAST == pData->drop_ctrs[i].type)
        {
          strncpy(&queueStr[0], "ucast", strlen("ucast"));
        }
        else if (BVIEW_BST_CGSN_MCAST == pData->drop_ctrs[i].type)
        {
          strncpy(&queueStr[0], "mcast", strlen("mcast"));
        }
        else
        {
          continue;
        }
        /* convert the port to an external representation */
        memset(&portStr[0], 0, JSON_MAX_NODE_LENGTH);
        JSON_PORT_MAP_TO_NOTATION(pData->drop_ctrs[i].port, asicId, &portStr[0]);

        tempLength = snprintf(jsonBuf, bufferLength,portQueueTemplate,
            &portStr[0], &queueStr[0]);

        bufferLength -= tempLength;
        jsonBuf += tempLength;

        tempLength = snprintf(jsonBuf, bufferLength,portQueueDataTemplate,
            pData->drop_ctrs[i].queue, pData->drop_ctrs[i].ctr);

        bufferLength -= tempLength;
        jsonBuf += tempLength;

        tempLength = snprintf(jsonBuf, bufferLength,"]},");
        bufferLength -= tempLength;
        jsonBuf += tempLength;
      }
    }
    /* adjust the buffer to remove the last ',' */
    jsonBuf = jsonBuf - 1;
    tempLength = tempLength - 1;
    bufferLength = bufferLength + 1;
  }
  else if (BVIEW_BST_CGSN_PRT_Q_DROPS == pData->rcvd_req.req_type)
  {
    first = true;
    for (prt = 1; prt < asic->numPorts; prt++)
    {
      if ((BVIEW_BST_CGSN_ALL == pData->rcvd_req.queue_type) ||
          (BVIEW_BST_CGSN_UCAST == pData->rcvd_req.queue_type))
      {
        /* */
        for (queue = 0; queue < BVIEW_BST_APP_NUM_COS_PORT; queue++)
        {
          i = BVIEW_BST_UCAST_DROP_CTR_INDEX_GET(prt, queue);
          if (0 == pData->drop_ctrs[i].ctr)
            continue;
          {
             min_one = true;
            if (true == first)
            {
              first = false;
            }
            else
            {
              tempLength = snprintf(jsonBuf, bufferLength,",");
              bufferLength -= tempLength;
              jsonBuf += tempLength;

            }
            /* convert the port to an external representation */
            memset(&portStr[0], 0, JSON_MAX_NODE_LENGTH);
            JSON_PORT_MAP_TO_NOTATION(pData->drop_ctrs[i].port, asicId, &portStr[0]);

            tempLength = snprintf(jsonBuf, bufferLength,portQueueTemplate,
                &portStr[0], "ucast");

            bufferLength -= tempLength;
            jsonBuf += tempLength;

            tempLength = snprintf(jsonBuf, bufferLength,portQueueDataTemplate,
                pData->drop_ctrs[i].queue, pData->drop_ctrs[i].ctr);

            bufferLength -= tempLength;
            jsonBuf += tempLength;

          }

          if (true == min_one)
          {
            tempLength = snprintf(jsonBuf, bufferLength,"]}");
            bufferLength -= tempLength;
            jsonBuf += tempLength;
          }
        }
#if 0
        /* adjust the buffer to remove the last ',' */
         jsonBuf = jsonBuf - 1;
         tempLength = tempLength - 1;
         bufferLength = bufferLength + 1;


#endif
      }

      min_one = false;
      if ((BVIEW_BST_CGSN_ALL == pData->rcvd_req.queue_type) ||
          (BVIEW_BST_CGSN_MCAST == pData->rcvd_req.queue_type))
      {
        /* */
        for (queue = 0; queue < BVIEW_BST_APP_NUM_COS_PORT; queue++)
        {
          i = BVIEW_BST_MCAST_DROP_CTR_INDEX_GET(prt, queue);
          if (0 == pData->drop_ctrs[i].ctr)
            continue;
          {
            min_one = true;
            if (true == first)
            {
              first = false;
            }
            else
            {
              tempLength = snprintf(jsonBuf, bufferLength,",");
              bufferLength -= tempLength;
              jsonBuf += tempLength;

            }

            /* convert the port to an external representation */
            memset(&portStr[0], 0, JSON_MAX_NODE_LENGTH);
            JSON_PORT_MAP_TO_NOTATION(pData->drop_ctrs[i].port, asicId, &portStr[0]);

            tempLength = snprintf(jsonBuf, bufferLength,portQueueTemplate,
                &portStr[0], "mcast");

            bufferLength -= tempLength;
            jsonBuf += tempLength;

            tempLength = snprintf(jsonBuf, bufferLength,portQueueDataTemplate,
                pData->drop_ctrs[i].queue, pData->drop_ctrs[i].ctr);

            bufferLength -= tempLength;
            jsonBuf += tempLength;
          }
          if (true  == min_one)
          {
            tempLength = snprintf(jsonBuf, bufferLength,"]}");
            bufferLength -= tempLength;
            jsonBuf += tempLength;
          }
        }
#if 0
        /* adjust the buffer to remove the last ',' */
         jsonBuf = jsonBuf - 1;
         tempLength = tempLength - 1;
         bufferLength = bufferLength + 1;
#endif

      }
    }
#if 0
    /* adjust the buffer to remove the last ',' */
    jsonBuf = jsonBuf - 1;
    tempLength = tempLength - 1;
    bufferLength = bufferLength + 1;i
#endif
  }



  tempLength = snprintf(jsonBuf, bufferLength, "] } ],\"id\": %d }", pData->id);
  bufferLength -= tempLength;
  jsonBuf += tempLength;


  *pJsonBuffer = (uint8_t *) start;

  _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : Request for Get-Bst-Report Complete [%d] bytes \n", (int)strlen(start));

  _JSONENCODE_LOG(_JSONENCODE_DEBUG_DUMPJSON, "BST-JSON-Encoder : %s \n", start);


  return BVIEW_STATUS_SUCCESS;
}
