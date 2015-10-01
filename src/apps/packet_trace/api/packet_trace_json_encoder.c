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

#include "packet_trace.h"
#include "packet_trace_include.h"
#include "packet_trace_util.h"


#include "packet_trace_json_memory.h"
#include "packet_trace_json_encoder.h"

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-pt-feature" REST API.
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
 *           ptjson_memory_free(). Failing to do so leads to memory leaks
 *********************************************************************/
BVIEW_STATUS ptjson_encode_get_pt_feature( int asicId,
                                            int id,
                                            const PTJSON_CONFIGURE_PT_FEATURE_t *pData,
                                            uint8_t **pJsonBuffer
                                            )
{
    char *getPtFeatureTemplate = " {\
\"jsonrpc\": \"2.0\",\
\"method\": \"get-packet-trace-feature\",\
\"asic-id\": \"%s\",\
\"version\": \"%d\",\
\"result\": {\
\"packet-trace-enable\": %d\
},\
\"id\": %d\
}";

    char *jsonBuf;
    char asicIdStr[JSON_MAX_NODE_LENGTH] = { 0 };
    BVIEW_STATUS status;

    _PT_JSONENCODE_LOG(_PT_JSONENCODE_DEBUG_TRACE, "PT-JSON-Encoder : Request for Get-Pt-Feature \n");

    /* Validate Input Parameters */
    _PT_JSONENCODE_ASSERT (pData != NULL);

    /* allocate memory for JSON */
    status = ptjson_memory_allocate(PTJSON_MEMSIZE_RESPONSE, (uint8_t **) & jsonBuf);
    _PT_JSONENCODE_ASSERT (status == BVIEW_STATUS_SUCCESS);

    /* clear the buffer */
    memset(jsonBuf, 0, PTJSON_MEMSIZE_RESPONSE);

    /* convert asicId to external  notation */
    JSON_ASIC_ID_MAP_TO_NOTATION(asicId, &asicIdStr[0]);

    /* encode the JSON */
    snprintf(jsonBuf, PTJSON_MEMSIZE_RESPONSE, getPtFeatureTemplate,
             &asicIdStr[0], BVIEW_JSON_VERSION, pData->ptEnable, id);

    /* setup the return value */
    *pJsonBuffer = (uint8_t *) jsonBuf;

    _PT_JSONENCODE_LOG(_PT_JSONENCODE_DEBUG_TRACE, "PT-JSON-Encoder : Encoding complete [%d bytes] \n", (int)strlen(jsonBuf));

    _PT_JSONENCODE_LOG(_PT_JSONENCODE_DEBUG_DUMPJSON, "PT-JSON-Encoder : %s \n", jsonBuf);

    return BVIEW_STATUS_SUCCESS;
}

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-packet-trace-lag-ecmp-resolution" REST API 
 *
 *********************************************************************/
BVIEW_STATUS _jsonencode_encode_profile_lag_ ( char *buffer,
                                                    const BVIEW_PT_PROFILE_RECORD_t *pData,
                                                    const PTJSON_REPORT_OPTIONS_t *options,
                                                    int bufLen,
                                                    int *length, int asicId, int port)
{
  int remLength = bufLen;
  int actualLength  = 0;
  unsigned int val1 = 0;

  char *lagTemplate = " { \"lag-id\": \"%s\", \"lag-members\" : [ ";
  char *fabricTemplate = "  \"fabric-trunk-id\": \"%s\", \"fabric-trunk-members\" : [ ";
  char *dataTemplate = " \"%s\" ,";
  char *dstMemberTemplate = " \"dst-lag-member\" : \"%s\" ";
  char portStr[JSON_MAX_NODE_LENGTH] = { 0 };
  char lagStr[JSON_MAX_NODE_LENGTH] = { 0 };
  int  member = 0, ii, count = 0;

  _PT_JSONENCODE_LOG(_PT_JSONENCODE_DEBUG_TRACE, "PT-JSON-Encoder : (Report) encoding trace profile data \n");

  val1 = pData->profile[port-1].hashingInfo.lag.trunk;
  /* convert the lag id to an external representation 
   Currently no API is present to convert the same.
   Hence we are just adding the received lag id as string*/
  memset(&lagStr[0], 0, JSON_MAX_NODE_LENGTH);
  sprintf(lagStr, "%d", val1);

  _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
      lagTemplate, lagStr);

       count = 0;
      for (ii = 0; ii < BVIEW_MAX_TRUNK_MEMBERS; ii++)
      {
        member = pData->profile[port-1].hashingInfo.lag.trunk_members[ii];
        if (0 != member) 
        {
          count++;
          memset(&portStr[0], 0, JSON_MAX_NODE_LENGTH);
          JSON_PORT_MAP_TO_NOTATION(member, asicId, &portStr[0]);
          _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
              dataTemplate, &portStr[0]); 
        }
      }


  if (0 != count)
  {
    /* adjust the buffer to remove the last ',' */
    buffer = buffer - 1;
    remLength += 1;
    *length -= 1;
  }

  _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
      "]," );

  member = pData->profile[port-1].hashingInfo.lag.trunk_member;
  memset(&portStr[0], 0, JSON_MAX_NODE_LENGTH);
  JSON_PORT_MAP_TO_NOTATION(member, asicId, &portStr[0]);
  _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length, dstMemberTemplate, &portStr[0]);

  /* add the " ," for the next  */
  _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
      "," );


  val1 = pData->profile[port-1].hashingInfo.lag.fabric_trunk;
  /* convert the lag id to an external representation */
  memset(&lagStr[0], 0, JSON_MAX_NODE_LENGTH);
  sprintf(lagStr, "%d", val1);

  _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
      fabricTemplate, lagStr);

      count = 0;
      for (ii = 0; ii < BVIEW_MAX_TRUNK_MEMBERS; ii++)
      {
        member = pData->profile[port-1].hashingInfo.lag.fabric_trunk_members[ii];
        if (0 != member) 
        {
          count++;
          memset(&portStr[0], 0, JSON_MAX_NODE_LENGTH);
          JSON_PORT_MAP_TO_NOTATION(member, asicId, &portStr[0]);
          _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
              dataTemplate, &portStr[0]); 
        }
      }

    if (0 != count)
    {
      /* adjust the buffer to remove the last ',' */
      buffer = buffer - 1;
      remLength += 1;
      *length -= 1;
    }


  _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
      "]" );

  _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
      "} } " );
  _PT_JSONENCODE_LOG(_PT_JSONENCODE_DEBUG_TRACE, "PT-JSON-Encoder : (Report) profle -lag-resolution Complete \n");

  return BVIEW_STATUS_SUCCESS;
}
/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-packet-trace-lag-ecmp-resolution" REST API 
 *
 *********************************************************************/
static BVIEW_STATUS _jsonencode_encode_profile_ecmp_ ( char *buffer,
                                                    const BVIEW_PT_PROFILE_RECORD_t *pData,
                                                    const PTJSON_REPORT_OPTIONS_t *options,
                                                    int bufLen,
                                                    int *length, int asicId, int port)
{
    int remLength = bufLen;
    int actualLength  = 0, ii, jj;
    unsigned int val1 = 0, val2 = 0, member = 0;
    char *ecmpTemplate = " { \"ecmp-group-id\": \"%s\", \"ecmp-members\" : [";
    char portStr[JSON_MAX_NODE_LENGTH] = { 0 };
    char memberStr[JSON_MAX_NODE_LENGTH] = { 0 };
    char next_hop_str[PT_MAX_IP_ADDR_LENGTH] = { 0 };
  /*  char *dataTemplate = " \"%s\","; */
    char *ecmpMemberTemplate = "[\"%s\",  \"%s\", \"%s\" ],";
    char *ecmpDstTemplate = " \"ecmp-dst-member\" : \"%s\", \"ecmp-dst-port\" : \"%s\", \"ecmp-next-hop-ip\" :\"%s\" }";
    void *ptr;

    _PT_JSONENCODE_LOG(_PT_JSONENCODE_DEBUG_TRACE, "PT-JSON-Encoder : (Report) encoding trace profile data \n");


     for (jj = 0; jj < BVIEW_ECMP_MAX_LEVEL; jj++)
     {
      val1 = pData->profile[port-1].hashingInfo.ecmp[jj].ecmp_group;
      memset(&memberStr[0], 0, JSON_MAX_NODE_LENGTH);
      sprintf(memberStr, "%d", val1);
      _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length, ecmpTemplate, &memberStr[0]);

      for (ii = 0; ii < BVIEW_ECMP_MAX_MEMBERS; ii++)
      {
        member = pData->profile[port-1].hashingInfo.ecmp[jj].ecmp_group_members[ii].member;
       /* if (0 != member) */
        {
          memset(&portStr[0], 0, JSON_MAX_NODE_LENGTH);
          memset(&memberStr[0], 0, JSON_MAX_NODE_LENGTH);
          memset(&next_hop_str[0], 0, PT_MAX_IP_ADDR_LENGTH);
          
          sprintf(memberStr, "%d", member);
          JSON_PORT_MAP_TO_NOTATION(pData->profile[port-1].hashingInfo.ecmp[jj].ecmp_group_members[ii].port, asicId, &portStr[0]);

          ptr = (void *)&pData->profile[port-1].hashingInfo.ecmp[jj].ecmp_group_members[ii].ip;
          inet_ntop(AF_INET, ptr, next_hop_str,  PT_MAX_IP_ADDR_LENGTH);
          _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
              ecmpMemberTemplate, &memberStr[0], &next_hop_str[0], &portStr[0]); 
        }
      }

    /* adjust the buffer to remove the last ',' */
    buffer = buffer - 1;
    remLength += 1;
    *length -= 1;

      _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
                                                  " ]," );

       val1 = pData->profile[port-1].hashingInfo.ecmp[jj].ecmp_egress_info.member;
      memset(&memberStr[0], 0, JSON_MAX_NODE_LENGTH);
      sprintf(memberStr, "%d", val1);

      val2 = pData->profile[port-1].hashingInfo.ecmp[jj].ecmp_egress_info.port;
      memset(&portStr[0], 0, JSON_MAX_NODE_LENGTH);
      sprintf(portStr, "%d", val2);
      ptr = (void *)&pData->profile[port-1].hashingInfo.ecmp[jj].ecmp_egress_info.ip;
      inet_ntop(AF_INET, ptr, next_hop_str,  PT_MAX_IP_ADDR_LENGTH);
      _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length,
                                                      ecmpDstTemplate, memberStr, portStr, next_hop_str);
      if (BVIEW_ECMP_MAX_LEVEL-1 != jj)
      {
        _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, length, ",");
      }
     }
      
    _PT_JSONENCODE_LOG(_PT_JSONENCODE_DEBUG_TRACE, "PT-JSON-Encoder : (Report) profle -ecmp-resolution Complete \n");

    return BVIEW_STATUS_SUCCESS;
}


/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-packet-trace-lag-ecmp-resolution" REST API 
 *
 *********************************************************************/
BVIEW_STATUS _jsonencode_encode_profile_lag ( char *buffer,
                                                    const BVIEW_PT_PROFILE_RECORD_t *pData,
                                                    const PTJSON_REPORT_OPTIONS_t *options,
                                                    int bufLen,
                                                    int *length, int asicId)
{
    int remLength = bufLen;
    int actualLength  = 0;
    int port = 0, tempLength = 0;
    BVIEW_PORT_MASK_t temp_mask;

    char *portTemplate = " { \"port\": \"%s\", \"lag-link-resolution\" :  ";
    char portStr[JSON_MAX_NODE_LENGTH] = { 0 };
    BVIEW_STATUS status;

    *length = 0;
    _PT_JSONENCODE_LOG(_PT_JSONENCODE_DEBUG_TRACE, "PT-JSON-Encoder : (Report) encoding trace profile data \n");


    memset (&temp_mask, 0, sizeof(BVIEW_PORT_MASK_t));
    memcpy(&temp_mask, &pData->port_list, sizeof(BVIEW_PORT_MASK_t));


    /*BVIEW_FHMASKBIT(temp_mask, port, (sizeof(BVIEW_PORT_MASK_t)/(sizeof(unsigned int)))); */
    BVIEW_FLMASKBIT(temp_mask, port, sizeof(BVIEW_PORT_MASK_t));
    while (0 != port) 
    {
      tempLength = 0;
      /* convert the port to an external representation */
      memset(&portStr[0], 0, JSON_MAX_NODE_LENGTH);
      JSON_PORT_MAP_TO_NOTATION(port, asicId, &portStr[0]);

    /* copying the header . Pointer and Length adjustments are handled by the macro */
    _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, &tempLength, portTemplate, &portStr[0]);

        bufLen -= (tempLength);
        *(length) += (tempLength);
               
     tempLength = 0;
     status = _jsonencode_encode_profile_lag_(buffer, pData, options, remLength, &tempLength, asicId, port);
    _PT_JSONENCODE_ASSERT (status == BVIEW_STATUS_SUCCESS);

        bufLen -= (tempLength);
        remLength -= tempLength;
        *(length) += (tempLength);
        buffer += (tempLength);
    _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, &tempLength, ",");
    BVIEW_CLRMASKBIT(temp_mask, port);
    BVIEW_FLMASKBIT(temp_mask, port, sizeof(BVIEW_PORT_MASK_t));
    }

    /* adjust the buffer to remove the last ',' */
    buffer = buffer - 1;
    remLength += 1;
    tempLength -= 1;
    *(length) += (tempLength);
    _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, &tempLength,
                                                  "], \"id\" : %d }", pData->id);
    _PT_JSONENCODE_LOG(_PT_JSONENCODE_DEBUG_TRACE, "PT-JSON-Encoder : (Report) profle -lag-resolution Complete \n");

    return BVIEW_STATUS_SUCCESS;
}
/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-packet-trace-lag-ecmp-resolution" REST API 
 *
 *********************************************************************/
static BVIEW_STATUS _jsonencode_encode_profile_ecmp ( char *buffer,
                                                    const BVIEW_PT_PROFILE_RECORD_t *pData,
                                                    const PTJSON_REPORT_OPTIONS_t *options,
                                                    int bufLen,
                                                    int *length, int asicId)
{
    int remLength = bufLen;
    int actualLength  = 0, tempLength = 0;
    int port = 0;
    BVIEW_PORT_MASK_t temp_mask;
    char portStr[JSON_MAX_NODE_LENGTH] = { 0 };
    BVIEW_STATUS status;

    char *portTemplate = " { \"port\": \"%s\" , \"ecmp-link-resolution\": [ ";

    _PT_JSONENCODE_LOG(_PT_JSONENCODE_DEBUG_TRACE, "PT-JSON-Encoder : (Report) encoding trace profile data \n");


    memset (&temp_mask, 0, sizeof(BVIEW_PORT_MASK_t));
    memcpy(&temp_mask, &pData->port_list, sizeof(BVIEW_PORT_MASK_t));

    /*BVIEW_FHMASKBIT(temp_mask, port, (sizeof(BVIEW_PORT_MASK_t)/(sizeof(unsigned int)))); */
    BVIEW_FLMASKBIT(temp_mask, port, sizeof(BVIEW_PORT_MASK_t));
    while (0 != port) 
    {
      tempLength = 0;
      /* convert the port to an external representation */
      memset(&portStr[0], 0, JSON_MAX_NODE_LENGTH);
      JSON_PORT_MAP_TO_NOTATION(port, asicId, &portStr[0]);

      /* copying the header . Pointer and Length adjustments are handled by the macro */
      _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, &tempLength, portTemplate, &portStr[0]);

      bufLen -= (tempLength);
      *(length) += (tempLength);
      tempLength = 0;
      status = _jsonencode_encode_profile_ecmp_(buffer, pData, options, remLength, &tempLength, asicId, port);
      _PT_JSONENCODE_ASSERT_ERROR((status == BVIEW_STATUS_SUCCESS), status);

      bufLen -= (tempLength);
      remLength -= tempLength;
      *(length) += (tempLength);
      buffer += (tempLength);

      _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, &tempLength,
          "] }" );

      BVIEW_CLRMASKBIT(temp_mask, port);
      BVIEW_FLMASKBIT(temp_mask, port, sizeof(BVIEW_PORT_MASK_t));
      if (0 != port)
      {
        _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, &tempLength, ",");
      }
    }

    *(length) += (tempLength);

    _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, &tempLength,
                                                  "], \"id\" : %d }", pData->id);

    _PT_JSONENCODE_LOG(_PT_JSONENCODE_DEBUG_TRACE, "PT-JSON-Encoder : (Report) profle -ecmp-resolution Complete \n");

    return BVIEW_STATUS_SUCCESS;
}


/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-packet-trace-profile" REST API.
 *
 *********************************************************************/
static BVIEW_STATUS _jsonencode_encode_profile ( char *buffer,
                                                    const BVIEW_PT_PROFILE_RECORD_t *pData,
                                                    const PTJSON_REPORT_OPTIONS_t *options,
                                                    int bufLen,
                                                    int *length, int asicId)
{
    BVIEW_STATUS status;
    int tempLength = 0;
    int remLength = bufLen;
    int actualLength  = 0;

    *length = 0;
    int port = 0;
    BVIEW_PORT_MASK_t temp_mask;

    char *portTemplate = " { \"port\": \"%s\", \"trace-profile\": [";
    char *realmTemplate = " { \"realm\": \"%s\",  \"data\" :  ";
    char portStr[JSON_MAX_NODE_LENGTH] = { 0 };


    _PT_JSONENCODE_LOG(_PT_JSONENCODE_DEBUG_TRACE, "PT-JSON-Encoder : (Report) encoding trace profile data \n");


    memset (&temp_mask, 0, sizeof(BVIEW_PORT_MASK_t));
    memcpy(&temp_mask, &pData->port_list, sizeof(BVIEW_PORT_MASK_t));

    BVIEW_FLMASKBIT(temp_mask, port, sizeof(BVIEW_PORT_MASK_t));
    while (0 != port) 
    {
      /* convert the port to an external representation */
      memset(&portStr[0], 0, JSON_MAX_NODE_LENGTH);
      JSON_PORT_MAP_TO_NOTATION(port, asicId, &portStr[0]);

      /* copying the header . Pointer and Length adjustments are handled by the macro */
      _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, &tempLength, portTemplate, &portStr[0]);

      _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, &tempLength, realmTemplate, "lag-link-resolution");

        bufLen -= (tempLength);
        *(length) += (tempLength);

        tempLength = 0;
     status = _jsonencode_encode_profile_lag_(buffer, pData, options, remLength, &tempLength, asicId, port);
    _PT_JSONENCODE_ASSERT_ERROR((status == BVIEW_STATUS_SUCCESS), status);

        remLength -= tempLength;
        bufLen -= (tempLength);
        buffer += (tempLength);
        *(length) += (tempLength);

      _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, &tempLength, ",");

      if (tempLength != 0)
      {
        bufLen -= (tempLength);
        *(length) += (tempLength);
      }

      remLength = bufLen;
      tempLength = 0;
      _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, &tempLength, realmTemplate, "ecmp-link-resolution");
      _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, &tempLength, "[");
        bufLen -= (tempLength);
        *(length) += (tempLength);
        tempLength = 0;
       status = _jsonencode_encode_profile_ecmp_(buffer, pData, options, remLength, &tempLength, asicId, port);
      _PT_JSONENCODE_ASSERT_ERROR((status == BVIEW_STATUS_SUCCESS), status);
      if (tempLength != 0)
      {
        bufLen -= (tempLength);
        remLength -= tempLength;
        buffer += (tempLength);
        *(length) += (tempLength);
      }



      BVIEW_CLRMASKBIT(temp_mask, port);
      BVIEW_FLMASKBIT(temp_mask, port, sizeof(BVIEW_PORT_MASK_t));
      _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, &tempLength,
          "] } ] }" );
      if (0 != port)
      {
       _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, &tempLength, ",");
      }
    }

    _PT_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, &tempLength,
                                                  "], \"id\" : %d }", pData->id);
    _PT_JSONENCODE_LOG(_PT_JSONENCODE_DEBUG_TRACE, "PT-JSON-Encoder : (Report) Encoding trace profile Complete \n");

    return BVIEW_STATUS_SUCCESS;
}


/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-packet-trace-profile" REST API.
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
 *           ptjson_memory_free(). Failing to do so leads to memory leaks
 *********************************************************************/

BVIEW_STATUS ptjson_encode_get_pt_profile ( int asicId,
                                            const BVIEW_PT_PROFILE_RECORD_t *pData,
                                            const PTJSON_REPORT_OPTIONS_t *options,
                                            uint8_t **pJsonBuffer
                                            )
{
    char *jsonBuf, *start;
    BVIEW_STATUS status;
    int bufferLength = PTJSON_MEMSIZE_REPORT;
    int tempLength = 0;

    time_t report_time;
    struct tm *timeinfo;
    char timeString[64];
    char asicIdStr[JSON_MAX_NODE_LENGTH] = { 0 };

    char *getPtReportStart = " { \
\"jsonrpc\": \"2.0\",\
\"method\": \"%s\",\
\"asic-id\": \"%s\",\
\"version\": \"%d\",\
\"time-stamp\": \"%s\",\
\"report\": [ \
";

    _PT_JSONENCODE_LOG(_PT_JSONENCODE_DEBUG_TRACE, "PT-JSON-Encoder : Request for Get-Pt-profile \n");

    /* Validate Input Parameters */
    _PT_JSONENCODE_ASSERT (options != NULL);
    _PT_JSONENCODE_ASSERT (pData != NULL);

    /* obtain the time */
    memset(&timeString, 0, sizeof (timeString));
    report_time = pData->tv;
    timeinfo = localtime(&report_time);
    strftime(timeString, 64, "%Y-%m-%d - %H:%M:%S ", timeinfo);

    /* allocate memory for JSON */
    status = ptjson_memory_allocate(PTJSON_MEMSIZE_REPORT, (uint8_t **) & jsonBuf);
    _PT_JSONENCODE_ASSERT (status == BVIEW_STATUS_SUCCESS);

    start = jsonBuf;

    /* clear the buffer */
    memset(jsonBuf, 0, PTJSON_MEMSIZE_REPORT);

    /* convert asicId to external  notation */
    JSON_ASIC_ID_MAP_TO_NOTATION(asicId, &asicIdStr[0]);

    /* fill the header */
    /* encode the JSON */
    tempLength = snprintf(jsonBuf, bufferLength, getPtReportStart,
        (options->report_lag_ecmp == false) ? "get-packet-trace-profile" : ((options->report_lag == true) ?
          "get-packet-trace-lag-resolution" : "get-packet-trace-ecmp-resolution"), &asicIdStr[0], 
           BVIEW_JSON_VERSION, timeString);

    jsonBuf += tempLength;
    bufferLength -= tempLength;
    /* get the trace profile */
    if (options->report_lag_ecmp == false)
    {
      status = _jsonencode_encode_profile(jsonBuf, pData, options, bufferLength, &tempLength, asicId);
    }
    else
    {
      if (options->report_lag == true)
      {
        status = _jsonencode_encode_profile_lag(jsonBuf, pData, options, bufferLength, &tempLength, asicId);
    }
    else
    {
        status = _jsonencode_encode_profile_ecmp(jsonBuf, pData, options, bufferLength, &tempLength, asicId);
      }
    }
    _PT_JSONENCODE_ASSERT_ERROR((status == BVIEW_STATUS_SUCCESS), status);

        /* adjust the buffer */
        bufferLength -= (tempLength);
        jsonBuf += (tempLength);

#if 0
    /* finalizing the report */

    bufferLength -= 1;
    jsonBuf -= 1;

    if (jsonBuf[0] == 0)
    {
        bufferLength -= 1;
        jsonBuf--;
    }
#endif
    tempLength = snprintf(jsonBuf, bufferLength, " ] ,\"id\":%d }", pData->id);

    *pJsonBuffer = (uint8_t *) start;

    _PT_JSONENCODE_LOG(_PT_JSONENCODE_DEBUG_TRACE, "PT-JSON-Encoder : Request for Get-Pt-Trace-Profile Complete [%d] bytes \n", (int)strlen(start));

    _PT_JSONENCODE_LOG(_PT_JSONENCODE_DEBUG_DUMPJSON, "PT-JSON-Encoder : %s \n", start);


    return BVIEW_STATUS_SUCCESS;
}


