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
 *         "get-bst-report" REST API - egress CPU Queue.
 *
 *********************************************************************/
static BVIEW_STATUS _jsonencode_report_egress_cpuq ( char *buffer, int asicId,
                                                    const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *previous,
                                                    const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *current,
                                                    const BSTJSON_REPORT_OPTIONS_t *options,
                                                    const BVIEW_ASIC_CAPABILITIES_t *asic,
                                                    int bufLen,
                                                    int *length)
{
    int remLength = bufLen;
    int actualLength  = 0;
    int queue = 0;
    uint64_t val = 0;

    char *realmTemplate = " { \"realm\": \"egress-cpu-queue\", \"%s\": [ ";
    char *cpuqTemplate = " [  %d , %" PRIu64 ", %" PRIu64 " ] ,";

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding EGRESS - CPU Queue data \n");

    /* copying the header . Pointer and Length adjustments are handled by the macro */
    _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length, realmTemplate, "data");

    /* For each queue, check if there is a difference, and create the report. */
    for (queue = 1; queue <= asic->numCpuQueues; queue++)
    {
        /* lets see if this queue needs to be included in the report at all */
        /* if this queue needs not be reported, then we move to next queue */
        if ((current->cpqQ.data[queue - 1].cpuBufferCount == 0) &&
            (current->cpqQ.data[queue - 1].cpuQueueEntries == 0) )
            continue;

        if ((previous != NULL) &&
            (previous->cpqQ.data[queue - 1].cpuBufferCount == current->cpqQ.data[queue - 1].cpuBufferCount) &&
            (previous->cpqQ.data[queue - 1].cpuQueueEntries == current->cpqQ.data[queue - 1].cpuQueueEntries))
            continue;

             val = current->cpqQ.data[queue - 1].cpuBufferCount;

           /* check if we need to convert the data to cells */
             if ((true == options->statUnitsInCells) && 
                 (true == options->reportThreshold))
             {
               val = current->cpqQ.data[queue - 1].cpuBufferCount / (asic->cellToByteConv);
             }
             /* check if we need to convert the data to cells
                the report always comes in cells from asic */
             else if ((false == options->statUnitsInCells) &&
                 (false == options->reportThreshold))
             {
               val = current->cpqQ.data[queue - 1].cpuBufferCount * (asic->cellToByteConv);
             }
               
        /* Now that this queue needs to be included in the report, add the data to report */
        _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
                                                      cpuqTemplate, queue-1,
                                                      val,
                                                      current->cpqQ.data[queue - 1].cpuQueueEntries
                                                      );

    }

    /* adjust the buffer to remove the last ',' */
    buffer = buffer - 1;
    remLength += 1;
    *length -= 1;

    /* add the "] } ," for the next 'realm' */
    _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
                                                  "] } ," );

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding EGRESS - CPU Queue data Complete \n");

    return BVIEW_STATUS_SUCCESS;
}

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-bst-report" REST API - egress RQE Queue.
 *
 *********************************************************************/
static BVIEW_STATUS _jsonencode_report_egress_rqeq ( char *buffer, int asicId,
                                                    const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *previous,
                                                    const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *current,
                                                    const BSTJSON_REPORT_OPTIONS_t *options,
                                                    const BVIEW_ASIC_CAPABILITIES_t *asic,
                                                    int bufLen,
                                                    int *length)
{
    int remLength = bufLen;
    int actualLength  = 0;
    int queue = 0;
    uint64_t val = 0;

    char *realmTemplate = " { \"realm\": \"egress-rqe-queue\", \"%s\": [ ";
    char *cpuqTemplate = " [  %d , %" PRIu64 ", %" PRIu64 " ] ,";

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding EGRESS - RQE Queue data \n");

    /* copying the header . Pointer and Length adjustments are handled by the macro */
    _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length, realmTemplate, "data");

    /* For each queue, check if there is a difference, and create the report. */
    for (queue = 1; queue <= asic->numRqeQueues; queue++)
    {
        /* lets see if this queue needs to be included in the report at all */
        /* if this queue needs not be reported, then we move to next queue */
        if ((current->rqeQ.data[queue - 1].rqeBufferCount == 0) &&
            (current->rqeQ.data[queue - 1].rqeQueueEntries == 0) )
            continue;

        if ((previous != NULL) &&
            (previous->rqeQ.data[queue - 1].rqeBufferCount == current->rqeQ.data[queue - 1].rqeBufferCount) &&
            (previous->rqeQ.data[queue - 1].rqeQueueEntries == current->rqeQ.data[queue - 1].rqeQueueEntries))
            continue;

             val = current->rqeQ.data[queue - 1].rqeBufferCount;

           /* check if we need to convert the data to cells */
             if ((true == options->statUnitsInCells) && 
                 (true == options->reportThreshold))
             {
               val = current->rqeQ.data[queue - 1].rqeBufferCount / (asic->cellToByteConv);
             }
             /* check if we need to convert the data to cells
                the report always comes in cells from asic */
             else if ((false == options->statUnitsInCells) &&
                 (false == options->reportThreshold))
             {
               val = current->rqeQ.data[queue - 1].rqeBufferCount * (asic->cellToByteConv);
             }
        /* Now that this queue needs to be included in the report, add the data to report */
        _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
                                                      cpuqTemplate, queue-1,
                                                      val,
                                                      current->rqeQ.data[queue - 1].rqeQueueEntries
                                                      );

    }

    /* adjust the buffer to remove the last ',' */
    buffer = buffer - 1;
    remLength += 1;
    *length -= 1;

    /* add the "] } ," for the next 'realm' */
    _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
                                                  "] } ," );

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding EGRESS - RQE Queue data Complete \n");

    return BVIEW_STATUS_SUCCESS;
}

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-bst-report" REST API - egress Multicast Queue.
 *
 *********************************************************************/
static BVIEW_STATUS _jsonencode_report_egress_mcq ( char *buffer, int asicId,
                                                   const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *previous,
                                                   const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *current,
                                                   const BSTJSON_REPORT_OPTIONS_t *options,
                                                   const BVIEW_ASIC_CAPABILITIES_t *asic,
                                                   int bufLen,
                                                   int *length)
{
    int remLength = bufLen;
    int actualLength  = 0;
    int queue = 0;
    uint64_t val = 0;

    char *realmTemplate = " { \"realm\": \"egress-mc-queue\", \"%s\": [ ";
    char *dataTemplate = " [  %d , \"%s\" ,  %" PRIu64 ", %" PRIu64 " ] ,";
    char portStr[JSON_MAX_NODE_LENGTH] = { 0 };


    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding EGRESS - MC Queue data \n");

    /* copying the header . Pointer and Length adjustments are handled by the macro */
    _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length, realmTemplate, "data");

    /* For each service pool, check if there is a difference, and create the report. */
    for (queue = 1; queue <= asic->numMulticastQueues; queue++)
    {
        /* lets see if this queue needs to be included in the report at all */
        /* if this queue needs not be reported, then we move to next queue */
        if ((current->eMcQ.data[queue - 1].mcBufferCount == 0) &&
            (current->eMcQ.data[queue - 1].mcQueueEntries == 0) )
            continue;

        if ((previous != NULL) &&
            (previous->eMcQ.data[queue - 1].mcBufferCount == current->eMcQ.data[queue - 1].mcBufferCount ) &&
            (previous->eMcQ.data[queue - 1].mcQueueEntries == current->eMcQ.data[queue - 1].mcQueueEntries))
            continue;

        /* convert the port to an external representation */
        memset(&portStr[0], 0, JSON_MAX_NODE_LENGTH);
        JSON_PORT_MAP_TO_NOTATION(current->eMcQ.data[queue - 1].port, asicId, &portStr[0]);

        val = current->eMcQ.data[queue - 1].mcBufferCount;
           /* check if we need to convert the data to cells */
             if ((true == options->statUnitsInCells) && 
                 (true == options->reportThreshold))
             {
               val = current->eMcQ.data[queue - 1].mcBufferCount / (asic->cellToByteConv);
             }
             /* check if we need to convert the data to cells
                the report always comes in cells from asic */
             else if ((false == options->statUnitsInCells) &&
                 (false == options->reportThreshold))
             {
               val = current->eMcQ.data[queue - 1].mcBufferCount * (asic->cellToByteConv);
             }
        /* Now that this pool needs to be included in the report, add the data to report */
        _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
                                                      dataTemplate, queue-1,
                                                      &portStr[0],
                                                      val,
                                                      current->eMcQ.data[queue - 1].mcQueueEntries
                                                      );

    }

    /* adjust the buffer to remove the last ',' */
    buffer = buffer - 1;
    remLength += 1;
    *length -= 1;

    /* add the "] } ," for the next 'realm' */
    _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
                                                  "] } ," );

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding EGRESS - MC Queue data Complete \n");

    return BVIEW_STATUS_SUCCESS;
}

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-bst-report" REST API - egress UC Queue.
 *
 *********************************************************************/
static BVIEW_STATUS _jsonencode_report_egress_ucq ( char *buffer, int asicId,
                                                   const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *previous,
                                                   const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *current,
                                                   const BSTJSON_REPORT_OPTIONS_t *options,
                                                   const BVIEW_ASIC_CAPABILITIES_t *asic,
                                                   int bufLen,
                                                   int *length)
{
    int remLength = bufLen;
    int actualLength  = 0;
    int queue = 0;
    uint64_t val = 0;

    char *realmTemplate = " { \"realm\": \"egress-uc-queue\", \"%s\": [ ";
    char *dataTemplate = " [  %d , \"%s\" , %" PRIu64 " ] ,";
    char portStr[JSON_MAX_NODE_LENGTH] = { 0 };


    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding EGRESS - UC Queue data \n");

    /* copying the header . Pointer and Length adjustments are handled by the macro */
    _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length, realmTemplate, "data");

    /* For each unicast queues, check if there is a difference, and create the report. */
    for (queue = 1; queue <= asic->numUnicastQueues; queue++)
    {
        /* lets see if this queue needs to be included in the report at all */
        /* if this queue needs not be reported, then we move to next queue */
        if (current->eUcQ.data[queue - 1].ucBufferCount == 0)
            continue;

        if ((previous != NULL) &&
            (previous->eUcQ.data[queue - 1].ucBufferCount == current->eUcQ.data[queue - 1].ucBufferCount))
            continue;

        /* convert the port to an external representation */
        memset(&portStr[0], 0, JSON_MAX_NODE_LENGTH);
        JSON_PORT_MAP_TO_NOTATION(current->eUcQ.data[queue - 1].port, asicId, &portStr[0]);

        val = current->eUcQ.data[queue - 1].ucBufferCount;
           /* check if we need to convert the data to cells */
             if ((true == options->statUnitsInCells) && 
                 (true == options->reportThreshold))
             {
               val = current->eUcQ.data[queue - 1].ucBufferCount / (asic->cellToByteConv);
             }
             /* check if we need to convert the data to cells
                the report always comes in cells from asic */
             else if ((false == options->statUnitsInCells) &&
                 (false == options->reportThreshold))
             {
               val = current->eUcQ.data[queue - 1].ucBufferCount * (asic->cellToByteConv);
             }
        /* Now that this ucq needs to be included in the report, add the data to report */
        _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
                                                      dataTemplate, queue-1,
                                                      &portStr[0],
                                                      val 
                                                      );
    }

    /* adjust the buffer to remove the last ',' */
    buffer = buffer - 1;
    remLength += 1;
    *length -= 1;

    /* add the "] } ," for the next 'realm' */
    _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
                                                  "] } ," );

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding EGRESS - UC Queue data Complete \n");

    return BVIEW_STATUS_SUCCESS;
}

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-bst-report" REST API - egress UC Queue Group.
 *
 *********************************************************************/
static BVIEW_STATUS _jsonencode_report_egress_ucqg ( char *buffer, int asicId,
                                                    const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *previous,
                                                    const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *current,
                                                    const BSTJSON_REPORT_OPTIONS_t *options,
                                                    const BVIEW_ASIC_CAPABILITIES_t *asic,
                                                    int bufLen,
                                                    int *length)
{
    int remLength = bufLen;
    int actualLength  = 0;
    int qg = 0;
    uint64_t val = 0;

    char *realmTemplate = " { \"realm\": \"egress-uc-queue-group\", \"%s\": [ ";
    char *dataTemplate = " [  %d , %" PRIu64 " ] ,";

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding EGRESS - UC Queue Group data \n");

    /* copying the header . Pointer and Length adjustments are handled by the macro */
    _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length, realmTemplate, "data");

    /* For each unicast queue groups, check if there is a difference, and create the report. */
    for (qg = 1; qg <= asic->numUnicastQueueGroups; qg++)
    {
        /* lets see if this queue needs to be included in the report at all */
        /* if this queue needs not be reported, then we move to next queue */
        if (current->eUcQg.data[qg - 1].ucBufferCount == 0)
            continue;

        if ((previous != NULL) &&
            (previous->eUcQg.data[qg - 1].ucBufferCount == current->eUcQg.data[qg - 1].ucBufferCount))
            continue;

              val = current->eUcQg.data[qg - 1].ucBufferCount;
           /* check if we need to convert the data to cells */
             if ((true == options->statUnitsInCells) && 
                 (true == options->reportThreshold))
             {
               val = current->eUcQg.data[qg - 1].ucBufferCount / (asic->cellToByteConv);
             }
             /* check if we need to convert the data to cells
                the report always comes in cells from asic */
             else if ((false == options->statUnitsInCells) &&
                 (false == options->reportThreshold))
             {
               val = current->eUcQg.data[qg - 1].ucBufferCount * (asic->cellToByteConv);
             }
        /* Now that this ucqg needs to be included in the report, add the data to report */
        _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
                                                      dataTemplate, qg-1,
                                                      val 
                                                      );
    }

    /* adjust the buffer to remove the last ',' */
    buffer = buffer - 1;
    remLength += 1;
    *length -= 1;

    /* add the "] } ," for the next 'realm' */
    _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
                                                  "] } ," );

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding EGRESS - UC Queue Group data Complete \n");

    return BVIEW_STATUS_SUCCESS;
}

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-bst-report" REST API - egress Service Pools .
 *
 *********************************************************************/
static BVIEW_STATUS _jsonencode_report_egress_sp ( char *buffer, int asicId,
                                                  const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *previous,
                                                  const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *current,
                                                  const BSTJSON_REPORT_OPTIONS_t *options,
                                                  const BVIEW_ASIC_CAPABILITIES_t *asic,
                                                  int bufLen,
                                                  int *length)
{
    int remLength = bufLen;
    int actualLength  = 0;
    int pool = 0;
    uint64_t val1 = 0, val2 = 0;

    char *realmTemplate = " { \"realm\": \"egress-service-pool\", \"%s\": [ ";
    char *dataTemplate = " [  %d , %" PRIu64 " , %" PRIu64 ", %" PRIu64 " ] ,";

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding EGRESS - Service Pool data \n");

    /* copying the header . Pointer and Length adjustments are handled by the macro */
    _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length, realmTemplate, "data");

    /* For each service pool, check if there is a difference, and create the report. */
    for (pool = 1; pool <= asic->numServicePools; pool++)
    {
        /* lets see if this sp needs to be included in the report at all */
        /* if this sp needs not be reported, then we move to next sp */
        if ((current->eSp.data[pool - 1].umShareBufferCount == 0) &&
            (current->eSp.data[pool - 1].mcShareBufferCount == 0)  &&
            (current->eSp.data[pool - 1].mcShareQueueEntries == 0) )
            continue;

        if ((previous != NULL) &&
            (previous->eSp.data[pool - 1].umShareBufferCount == current->eSp.data[pool - 1].umShareBufferCount ) &&
            (previous->eSp.data[pool - 1].mcShareBufferCount == current->eSp.data[pool - 1].mcShareBufferCount ) &&
            (previous->eSp.data[pool - 1].mcShareQueueEntries == current->eSp.data[pool - 1].mcShareQueueEntries ))
            continue;

             val1 = current->eSp.data[pool - 1].umShareBufferCount;
             val2 = current->eSp.data[pool - 1].mcShareBufferCount;

           /* check if we need to convert the data to cells */
             if ((true == options->statUnitsInCells) && 
                 (true == options->reportThreshold))
             {
               val1 = current->eSp.data[pool - 1].umShareBufferCount / (asic->cellToByteConv);
               val2 = current->eSp.data[pool - 1].mcShareBufferCount / (asic->cellToByteConv);
             }
             /* check if we need to convert the data to cells
                the report always comes in cells from asic */
             else if ((false == options->statUnitsInCells) &&
                 (false == options->reportThreshold))
             {
               val1 = current->eSp.data[pool - 1].umShareBufferCount * (asic->cellToByteConv);
               val2 = current->eSp.data[pool - 1].mcShareBufferCount * (asic->cellToByteConv);
             }
        /* Now that this pool needs to be included in the report, add the data to report */
        _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
                                                      dataTemplate, pool-1, val1,val2,
                                                      current->eSp.data[pool - 1].mcShareQueueEntries
                                                      );
    }

    /* adjust the buffer to remove the last ',' */
    buffer = buffer - 1;
    remLength += 1;
    *length -= 1;

    /* add the "] } ," for the next 'realm' */
    _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
                                                  "] } ," );

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding EGRESS - Service Pool data Complete \n");

    return BVIEW_STATUS_SUCCESS;
}

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-bst-report" REST API - egress-port-service-pool.
 *
 *********************************************************************/
static BVIEW_STATUS _jsonencode_report_egress_epsp ( char *buffer, int asicId,
                                                    const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *previous,
                                                    const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *current,
                                                    const BSTJSON_REPORT_OPTIONS_t *options,
                                                    const BVIEW_ASIC_CAPABILITIES_t *asic,
                                                    int bufLen,
                                                    int *length)
{
    int remLength = bufLen;
    int actualLength  = 0;
    bool includePort = false;
    uint64_t val1 = 0, val2 = 0, val3 = 0;

    int includeServicePool[BVIEW_ASIC_MAX_SERVICE_POOLS] = { 0 };
    int port = 0, pool = 0;

    char *epspTemplate = " { \"realm\": \"egress-port-service-pool\", \"%s\": [ ";
    char *epspPortTemplate = " { \"port\": \"%s\", \"data\": [ ";
    char *epspServicePoolTemplate = " [  %d , %" PRIu64 " , %" PRIu64 " , %" PRIu64 " ] ,";
    char portStr[JSON_MAX_NODE_LENGTH] = { 0 };


    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding EGRESS - EPSP data \n");

    /* copying the header . Pointer and Length adjustments are handled by the macro */
    _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length, epspTemplate, "data");

    /* For each port, and for each service pool in that port, 
     *  1. attempt to see if this port needs to be reported.
     *  2. create the report.
     */
    for (port = 1; port <= asic->numPorts; port++)
    {
        includePort = false;
        memset (&includeServicePool[0], 0, sizeof (includeServicePool));

        /* lets see if this port needs to be included in the report at all */
        for (pool = 1; pool <= asic->numServicePools; pool++)
        {
            /* By default, we plan to include the pool */
            includeServicePool[pool - 1] = 1;

            /* If there is no traffic reported for this priority group, ignore it */
            if ( (current->ePortSp.data[port - 1][pool - 1].umShareBufferCount == 0) &&
                (current->ePortSp.data[port - 1][pool - 1].ucShareBufferCount == 0) &&
                (current->ePortSp.data[port - 1][pool - 1].mcShareBufferCount == 0) &&
                (current->ePortSp.data[port - 1][pool - 1].mcShareQueueEntries == 0))
            {
                includeServicePool[pool - 1] = 0;
                continue;
            }

            /* If this is snapshot report, include the port in the data  */
            if (previous == NULL)
            {
                includePort = true;
                continue;
            }

            /* if there is traffic reported since the last snapshot, we can't ignore this pool */
            if ( (previous->ePortSp.data[port - 1][pool - 1].umShareBufferCount
                  != current->ePortSp.data[port - 1][pool - 1].umShareBufferCount) ||
                (previous->ePortSp.data[port - 1][pool - 1].ucShareBufferCount
                 != current->ePortSp.data[port - 1][pool - 1].ucShareBufferCount) ||
                (previous->ePortSp.data[port - 1][pool - 1].mcShareBufferCount
                 != current->ePortSp.data[port - 1][pool - 1].mcShareBufferCount) ||
                (previous->ePortSp.data[port - 1][pool - 1].mcShareQueueEntries
                 != current->ePortSp.data[port - 1][pool - 1].mcShareQueueEntries) )
            {
                includePort = true;
                continue;
            }

            /* since there is no reason to include the pool, we can ignore it*/
            includeServicePool[pool - 1] = 0;
        }

        /* if this port needs not be reported, then we move to next port */
        if (includePort == false)
        {
            continue;
        }

        /* convert the port to an external representation */
        memset(&portStr[0], 0, JSON_MAX_NODE_LENGTH);
        JSON_PORT_MAP_TO_NOTATION(port, asicId, &portStr[0]);

        /* Now that this port needs to be included in the report, copy the header */
        _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length, epspPortTemplate, &portStr[0]);

        /* for each priority-group, prepare the data */
        for (pool = 1; pool <= asic->numServicePools; pool++)
        {
            /* we ignore if there is no data to be reported */
            if (includeServicePool[pool - 1] == 0)
                continue;

            val1 = current->ePortSp.data[port - 1][pool - 1].ucShareBufferCount;
            val2 = current->ePortSp.data[port - 1][pool - 1].umShareBufferCount;
            val3 = current->ePortSp.data[port - 1][pool - 1].mcShareBufferCount;

           /* check if we need to convert the data to cells */
             if ((true == options->statUnitsInCells) && 
                 (true == options->reportThreshold))
             {
               val1 = current->ePortSp.data[port - 1][pool - 1].ucShareBufferCount / (asic->cellToByteConv);
               val2 = current->ePortSp.data[port - 1][pool - 1].umShareBufferCount / (asic->cellToByteConv);
               val3 = current->ePortSp.data[port - 1][pool - 1].mcShareBufferCount / (asic->cellToByteConv);
             }
             /* check if we need to convert the data to cells
                the report always comes in cells from asic */
             else if ((false == options->statUnitsInCells) &&
                 (false == options->reportThreshold))
             {
               val1 = current->ePortSp.data[port - 1][pool - 1].ucShareBufferCount * (asic->cellToByteConv);
               val2 = current->ePortSp.data[port - 1][pool - 1].umShareBufferCount * (asic->cellToByteConv);
               val3 = current->ePortSp.data[port - 1][pool - 1].mcShareBufferCount * (asic->cellToByteConv);
             }
            /* add the data to the report */
            _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
                                                          epspServicePoolTemplate, pool-1,
                                                          val1, val2, val3,
                                                          current->ePortSp.data[port - 1][pool - 1].mcShareQueueEntries
                                                          );
        }

        /* adjust the buffer to remove the last ',' */
        buffer = buffer - 1;
        remLength += 1;
        *length -= 1;

        /* add the "] } ," for the next port */
        _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
                                                      "] } ," );
    }

    /* adjust the buffer to remove the last ',' */
    buffer = buffer - 1;
    remLength += 1;
    *length -= 1;

    /* add the "] } ," for the next 'realm' */
    _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
                                                  "] } ," );

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding EGRESS - EPSP data Complete \n");

    return BVIEW_STATUS_SUCCESS;

}

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-bst-report" REST API - egress part.
 *
 *********************************************************************/
BVIEW_STATUS _jsonencode_report_egress ( char *buffer, int asicId,
                                        const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *previous,
                                        const BVIEW_BST_ASIC_SNAPSHOT_DATA_t *current,
                                        const BSTJSON_REPORT_OPTIONS_t *options,
                                        const BVIEW_ASIC_CAPABILITIES_t *asic,
                                        int bufLen,
                                        int *length)
{
    BVIEW_STATUS status;
    int tempLength = 0;

    *length = 0;

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding EGRESS data \n");

    /* If CPU realm is asked for, lets encode the corresponding data */

    if (options->includeEgressCpuQueue)
    {
        tempLength = 0;
        status = _jsonencode_report_egress_cpuq(buffer, asicId, previous, current, options, asic, bufLen, &tempLength);
        _JSONENCODE_ASSERT_ERROR((status == BVIEW_STATUS_SUCCESS), status);

        if (tempLength != 0)
        {
            bufLen -= (tempLength);
            buffer += (tempLength);
            *(length) += (tempLength);
        }
    }

    /* If Egress Multicast queue realm is asked for, lets encode the corresponding data */
    if (options->includeEgressMcQueue)
    {
        tempLength = 0;
        status = _jsonencode_report_egress_mcq(buffer, asicId, previous, current, options, asic, bufLen, &tempLength);
        _JSONENCODE_ASSERT_ERROR((status == BVIEW_STATUS_SUCCESS), status);

        if (tempLength != 0)
        {
            bufLen -= (tempLength);
            buffer += (tempLength);
            *(length) += (tempLength);
        }
    }

    /* If Egress Port - Service Pool realm is asked for, lets encode the corresponding data */

    if (options->includeEgressPortServicePool)
    {
        tempLength = 0;
        status = _jsonencode_report_egress_epsp(buffer, asicId, previous, current, options, asic, bufLen, &tempLength);
        _JSONENCODE_ASSERT_ERROR((status == BVIEW_STATUS_SUCCESS), status);

        if (tempLength != 0)
        {
            bufLen -= (tempLength);
            buffer += (tempLength);
            *(length) += (tempLength);
        }
    }

    /* If Egress RQE queue realm is asked for, lets encode the corresponding data */
    if (options->includeEgressRqeQueue)
    {
        tempLength = 0;
        status = _jsonencode_report_egress_rqeq(buffer, asicId, previous, current, options, asic, bufLen, &tempLength);
        _JSONENCODE_ASSERT_ERROR((status == BVIEW_STATUS_SUCCESS), status);

        if (tempLength != 0)
        {
            bufLen -= (tempLength);
            buffer += (tempLength);
            *(length) += (tempLength);
        }
    }

    /* If Egress Service Pool realm is asked for, lets encode the corresponding data */
    if (options->includeEgressServicePool)
    {
        tempLength = 0;
        status = _jsonencode_report_egress_sp(buffer, asicId, previous, current, options, asic, bufLen, &tempLength);
        _JSONENCODE_ASSERT_ERROR((status == BVIEW_STATUS_SUCCESS), status);

        if (tempLength != 0)
        {
            bufLen -= (tempLength);
            buffer += (tempLength);
            *(length) += (tempLength);
        }
    }

    /* If Egress Unicast queue realm is asked for, lets encode the corresponding data */
    if (options->includeEgressUcQueue)
    {
        tempLength = 0;
        status = _jsonencode_report_egress_ucq(buffer, asicId, previous, current, options, asic, bufLen, &tempLength);
        _JSONENCODE_ASSERT_ERROR((status == BVIEW_STATUS_SUCCESS), status);

        if (tempLength != 0)
        {
            bufLen -= (tempLength);
            buffer += (tempLength);
            *(length) += (tempLength);
        }
    }

    /* If Egress Unicast queue group realm is asked for, lets encode the corresponding data */
    if (options->includeEgressUcQueueGroup)
    {
        tempLength = 0;
        status = _jsonencode_report_egress_ucqg(buffer, asicId, previous, current, options, asic, bufLen, &tempLength);
        _JSONENCODE_ASSERT_ERROR((status == BVIEW_STATUS_SUCCESS), status);

        if (tempLength != 0)
        {
            bufLen -= (tempLength);
            buffer += (tempLength);
            *(length) += (tempLength);
        }

    }

    if (tempLength != 0)
    {
        *(length) -= 1;
    }

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding EGRESS data complete \n");

    return BVIEW_STATUS_SUCCESS;
}
