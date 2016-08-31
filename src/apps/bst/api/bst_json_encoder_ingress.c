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

#include <inttypes.h>

#include "broadview.h"

#include "cJSON.h"

#include "configure_bst_feature.h"
#include "configure_bst_tracking.h"

#include "bst.h"
#include "common/platform_spec.h"

#include "bst_json_memory.h"
#include "bst_json_encoder.h"

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-bst-report" REST API - ingress-port-port-group.
 *
 *********************************************************************/
static BVIEW_STATUS _jsonencode_report_ingress_ippg ( char *buffer, int asicId,
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
    uint64_t val1 = 0;
    uint64_t val2 = 0;
    uint64_t maxBufVal = 0;
    int sendIncrReport = options->sendIncrementalReport;


    int includePriorityGroups[BVIEW_ASIC_MAX_PRIORITY_GROUPS] = { 0 };
    int port = 0, priGroup = 0;
    char portStr[JSON_MAX_NODE_LENGTH] = { 0 };

    char *ippgTemplate = " { \"realm\": \"ingress-port-priority-group\", \"%s\": [ ";
    char *ippgPortTemplate = " { \"port\": \"%s\", \"data\": [ ";
    char *ippgPortGroupTemplate = " [  %d , %" PRIu64 " , %" PRIu64 " ] ,";

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding INGRESS - IPPG data \n");

    /* copying the header . Pointer and Length adjustments are handled by the macro */
    _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length, ippgTemplate, "data");

    /* For each port, and for each priority group in that port, 
     *  1. attempt to see if this port needs to be reported.
     *  2. create the report.
     */
    for (port = 1; port <= asic->numPorts; port++)
    {
      /* check if the trigger report request should contain snap shot */
        if ((port != options->triggerInfo.port) && 
            (false == options->sendSnapShotOnTrigger) && 
            (true == options->reportTrigger))
        {
          continue;
        }
        includePort = false;
        memset (&includePriorityGroups[0], 0, sizeof (includePriorityGroups));

        /* lets see if this port needs to be included in the report at all */
        for (priGroup = 1; priGroup <= asic->numPriorityGroups; priGroup++)
        {
      /* check if the trigger report request should contain snap shot */
           if ((priGroup-1 != options->triggerInfo.queue) && 
            (false == options->sendSnapShotOnTrigger) && 
            (true == options->reportTrigger))
           {
             continue;
           }
            /* By default, we plan to include the pri-group */
            includePriorityGroups[priGroup - 1] = 1;
   
            if (true == sendIncrReport)
            {
            /* If there is no traffic reported for this priority group, ignore it */
            if ((previous == NULL) && 
                (current->iPortPg.data[port - 1][priGroup - 1].umShareBufferCount == 0) &&
                (current->iPortPg.data[port - 1][priGroup - 1].umHeadroomBufferCount == 0) )
            {
                includePriorityGroups[priGroup - 1] = 0;
                continue;
            }
           }

            /* If this is snapshot report, include the port in the data  */
            if (previous == NULL)
            {
                includePort = true;
                continue;
            }

            /* if there is traffic reported since the last snapshot, we can't ignore this priority group */
            if (previous->iPortPg.data[port - 1][priGroup - 1].umShareBufferCount
                != current->iPortPg.data[port - 1][priGroup - 1].umShareBufferCount)
            {
                includePort = true;
                continue;
            }

            if (previous->iPortPg.data[port - 1][priGroup - 1].umHeadroomBufferCount
                != current->iPortPg.data[port - 1][priGroup - 1].umHeadroomBufferCount)
            {
                includePort = true;
                continue;
            }

            /* since there is no reason to include the group, we can ignore it*/
            includePriorityGroups[priGroup - 1] = 0;
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
          _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length, ippgPortTemplate, &portStr[0]);

        /* for each priority-group, prepare the data */
        for (priGroup = 1; priGroup <= asic->numPriorityGroups; priGroup++)
        {
          /* we ignore if there is no data to be reported */
          if (includePriorityGroups[priGroup - 1] == 0)
            continue;

          val1 = current->iPortPg.data[port - 1][priGroup - 1].umShareBufferCount;
          maxBufVal = options->bst_max_buffers_ptr->iPortPg.data[port - 1][priGroup - 1].umShareMaxBuf;
          
          bst_json_convert_data(options, asic, &val1, maxBufVal);

          maxBufVal = options->bst_max_buffers_ptr->iPortPg.data[port - 1][priGroup - 1].umHeadroomMaxBuf;
          val2 = current->iPortPg.data[port - 1][priGroup - 1].umHeadroomBufferCount;
          bst_json_convert_data(options, asic, &val2, maxBufVal);


          /* add the data to the report */
          _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
              ippgPortGroupTemplate, priGroup-1, val1, val2);
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

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding INGRESS - IPPG data Complete \n");

    return BVIEW_STATUS_SUCCESS;

}

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-bst-report" REST API - ingress-port-service-pool.
 *
 *********************************************************************/
static BVIEW_STATUS _jsonencode_report_ingress_ipsp ( char *buffer, int asicId,
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
    uint64_t val = 0;
    uint64_t maxBufVal = 0;
    int sendIncrReport = options->sendIncrementalReport;

    int includeServicePool[BVIEW_ASIC_MAX_SERVICE_POOLS] = { 0 };
    int port = 0, pool = 0;
    char portStr[JSON_MAX_NODE_LENGTH] = { 0 };


    char *ipspTemplate = " { \"realm\": \"ingress-port-service-pool\", \"%s\": [ ";
    char *ipspPortTemplate = " { \"port\": \"%s\", \"data\": [ ";
    //char *ippgPortGroupTemplate = " { \"pg\" : %d , \"um-share\": %" PRIu64 " , \"um-headroom\": %" PRIu64 " } ,";
    //char *ippgPortGroupTemplate = " { \"pg\" : %d , \"data\": [ %" PRIu64 " , %" PRIu64 " ] } ,";
    char *ipspServicePoolTemplate = " [  %d , %" PRIu64 " ] ,";

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding INGRESS - IPSP data \n");

    /* copying the header . Pointer and Length adjustments are handled by the macro */
    _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length, ipspTemplate, "data");

    /* For each port, and for each priority group in that port, 
     *  1. attempt to see if this port needs to be reported.
     *  2. create the report.
     */
    for (port = 1; port <= asic->numPorts; port++)
    {
      /* check if the trigger report request should contain snap shot */
        if ((port != options->triggerInfo.port) && 
            (false == options->sendSnapShotOnTrigger) && 
            (true == options->reportTrigger))
        {
          continue;
        }
        includePort = false;
        memset (&includeServicePool[0], 0, sizeof (includeServicePool));

        /* lets see if this port needs to be included in the report at all */
        for (pool = 1; pool <= asic->numServicePools; pool++)
        {
      /* check if the trigger report request should contain snap shot */
           if ((pool-1 != options->triggerInfo.queue) && 
            (false == options->sendSnapShotOnTrigger) && 
            (true == options->reportTrigger))
           {
             continue;
           }
            /* By default, we plan to include the pool */
            includeServicePool[pool - 1] = 1;

            if (true == sendIncrReport)
            {
              /* If there is no traffic reported for this priority group, ignore it */
              if ((previous == NULL) &&
                  (current->iPortSp.data[port - 1][pool - 1].umShareBufferCount == 0))
              {
                includeServicePool[pool - 1] = 0;
                continue;
              }
            }

            /* If this is snapshot report, include the port in the data  */
            if (previous == NULL)
            {
                includePort = true;
                continue;
            }

            /* if there is traffic reported since the last snapshot, we can't ignore this pool */
            if (previous->iPortSp.data[port - 1][pool - 1].umShareBufferCount
                != current->iPortSp.data[port - 1][pool - 1].umShareBufferCount)
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
          _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length, ipspPortTemplate,  &portStr[0]);

        /* for each priority-group, prepare the data */
        for (pool = 1; pool <= asic->numServicePools; pool++)
        {
            /* we ignore if there is no data to be reported */
            if (includeServicePool[pool - 1] == 0)
                continue;

            val = current->iPortSp.data[port - 1][pool - 1].umShareBufferCount;
            maxBufVal = options->bst_max_buffers_ptr->iPortSp.data[port - 1][pool - 1].umShareMaxBuf;
            bst_json_convert_data(options, asic, &val, maxBufVal);

            /* add the data to the report */
            _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
                                                          ipspServicePoolTemplate, pool-1,val);
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

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding INGRESS - IPSP data Complete \n");

    return BVIEW_STATUS_SUCCESS;

}

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-bst-report" REST API - ingress-service-pool.
 *
 *********************************************************************/
static BVIEW_STATUS _jsonencode_report_ingress_sp ( char *buffer, int asicId,
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
    uint64_t val = 0;
    uint64_t maxBufVal = 0;
    int sendIncrReport = options->sendIncrementalReport;

    char *ispTemplate = " { \"realm\": \"ingress-service-pool\", \"%s\": [ ";
    char *ispServicePoolTemplate = " [  %d , %" PRIu64 " ] ,";

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding INGRESS - ISP data \n");

    /* copying the header . Pointer and Length adjustments are handled by the macro */
    _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length, ispTemplate, "data");

    /* For each service pool, check if there is a difference, and create the report. */
    for (pool = 1; pool <= asic->numServicePools; pool++)
    {
      /* check if the trigger report request should contain snap shot */
        if ((pool-1 != options->triggerInfo.queue) && 
            (false == options->sendSnapShotOnTrigger) && 
            (true == options->reportTrigger))
        {
          continue;
        }

        if (true == sendIncrReport)
        {
        /* lets see if this pool needs to be included in the report at all */
        /* if this pool needs not be reported, then we move to next pool */
         if ((previous == NULL) &&
            (current->iSp.data[pool-1].umShareBufferCount == 0))
            continue;  
        }

        if ((previous != NULL) &&
            (previous->iSp.data[pool-1].umShareBufferCount == current->iSp.data[pool-1].umShareBufferCount))
            continue;

             val = current->iSp.data[pool-1].umShareBufferCount;
             maxBufVal = options->bst_max_buffers_ptr->iSp.data[pool-1].umShareMaxBuf;
             bst_json_convert_data(options, asic, &val, maxBufVal);
        /* Now that this pool needs to be included in the report, add the data to report */
        _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
                                                      ispServicePoolTemplate, pool-1, val);

    }

    /* adjust the buffer to remove the last ',' */
    buffer = buffer - 1;
    remLength += 1;
    *length -= 1;

    /* add the "] } ," for the next 'realm' */
    _JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
                                                  "] } ," );

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding INGRESS - ISP data Complete \n");

    return BVIEW_STATUS_SUCCESS;

}

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-bst-report" REST API - ingress part.
 *
 *********************************************************************/
BVIEW_STATUS _jsonencode_report_ingress ( char *buffer, 
                                         int asicId,
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

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding INGRESS data \n");

    /* If Port-PriorityGroup realm is asked for, lets encode the corresponding data */
    if (options->includeIngressPortPriorityGroup)
    {
        tempLength = 0;
        status = _jsonencode_report_ingress_ippg(buffer, asicId, previous, current, options, asic, bufLen, &tempLength);
        _JSONENCODE_ASSERT_ERROR((status == BVIEW_STATUS_SUCCESS), status);

        if (tempLength != 0)
        {
            bufLen -= (tempLength);
            buffer += (tempLength);
            *(length) += (tempLength);
        }
    }

    /* If Port-ServicePool realm is asked for, lets encode the corresponding data */
    if (options->includeIngressPortServicePool)
    {
        tempLength = 0;
        status = _jsonencode_report_ingress_ipsp(buffer, asicId, previous, current, options, asic, bufLen, &tempLength);
        _JSONENCODE_ASSERT_ERROR((status == BVIEW_STATUS_SUCCESS), status);
        if (tempLength != 0)
        {
            bufLen -= (tempLength);
            buffer += (tempLength);
            *(length) += (tempLength);
        }
    }

    /* If ServicePool realm is asked for, lets encode the corresponding data */
    if (options->includeIngressServicePool)
    {
        tempLength = 0;
        status = _jsonencode_report_ingress_sp(buffer, asicId, previous, current, options, asic, bufLen, &tempLength);
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
        if (! (options->includeEgressCpuQueue ||
               options->includeEgressMcQueue ||
               options->includeEgressPortServicePool ||
               options->includeEgressRqeQueue ||
               options->includeEgressServicePool ||
               options->includeEgressUcQueue ||
               options->includeEgressUcQueueGroup ))
        {
            *(length) -= 1;
        }
    }

    _JSONENCODE_LOG(_JSONENCODE_DEBUG_TRACE, "BST-JSON-Encoder : (Report) Encoding INGRESS data complete \n");


    return BVIEW_STATUS_SUCCESS;
}
