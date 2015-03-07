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

#include "broadview.h"
#include "cJSON.h"

#include "configure_bst_feature.h"
#include "configure_bst_tracking.h"

#include "bst.h"

#include "bst_json_memory.h"
#include "bst_json_encoder.h"

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
\"result\": {\
\"bst-enable\": %d,\
\"send-async-reports\": %d,\
\"collection-interval\": %d,\
\"stat-units-in-cells\": %d\
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
             &asicIdStr[0], pData->bstEnable,
             pData->sendAsyncReports, pData->collectionInterval,
             pData->statUnitsInCells, method);

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
             &asicIdStr[0], pData->trackPeakStats,
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
    char *getBstDeviceReportTemplate = " {\
\"realm\": \"device\",\
\"data\": %d \
}";
    /* Since this is an internal function, with all parameters validated already, 
     * we jump to the logic straight-away 
     */
    int data;
    *length = 0;

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

    /* check if we need to convert the data to cells */
    if ((true == options->statUnitsInCells) &&
        (true == options->reportThreshold))
     {
       data = data / (asic->cellToByteConv);
     }
     /* check if we need to convert the data to cells
        the report always comes in cells from asic */
     else if ((false == options->statUnitsInCells) &&
              (false == options->reportThreshold))
     {
       data = data * (asic->cellToByteConv);
     }



    /* encode the JSON */
    *length = snprintf(jsonBuf, bufLen, getBstDeviceReportTemplate, data);

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding device data [%d] complete \n", *length);

    return BVIEW_STATUS_SUCCESS;
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



    char *getBstReportStart = " { \
\"jsonrpc\": \"2.0\",\
\"method\": \"%s\",\
\"asic-id\": \"%s\",\
\"time-stamp\": \"%s\",\
\"report\": [ \
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
    tempLength = snprintf(jsonBuf, bufferLength, getBstReportStart,
                          (options->reportThreshold == true) ? "get-bst-thresholds" :((options->reportTrigger == true)? "trigger-report":"get-bst-report"),
                          &asicIdStr[0], timeString);

    jsonBuf += tempLength;
    bufferLength -= tempLength;

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

