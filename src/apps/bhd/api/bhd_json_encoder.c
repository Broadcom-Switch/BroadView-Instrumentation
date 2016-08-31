/*********************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename bhd_json_encoder.c 
  *
  * @purpose BroadView BHD JSON encoder functions 
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


#include <time.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>

#include "broadview.h"
#include "asic.h"
#include "cJSON.h"
#include "port_utils.h"
#include "bhd.h"
#include "configure_black_hole_detection.h"
#include "configure_black_hole.h"
#include "get_sflow_sampling_status.h"
#include "bhd_include.h"
#include "bhd_json_encoder.h"
#include "bhd_json_memory.h"

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-black-hole-detection" REST API.
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
 *           bhdjson_memory_free(). Failing to do so leads to memory leaks
 *********************************************************************/
BVIEW_STATUS bhdjson_encode_get_black_hole_detection( int asicId,
                                            int method,
                                            const BHDJSON_CONFIGURE_BLACK_HOLE_DETECTION_t *pData,
                                            uint8_t **pJsonBuffer
                                            )

{
  char *getBlackHoleDetectionTemplate = " {\
                                      \"jsonrpc\": \"2.0\",\
                                      \"method\": \"get-black-hole-detection-enable\",\
                                      \"asic-id\": \"%s\",\
                                      \"version\": \"%d\",\
                                      \"result\": {\
                                      \"enable\": %d }, \"id\" : %d }";

  char *jsonBuf, *buffer;
  char asicIdStr[JSON_MAX_NODE_LENGTH] = { 0 };
  BVIEW_STATUS status;
  int actualLength = 0;
  int totalLength = 0;
  int remLength = BHDJSON_MEMSIZE_RESPONSE;

  _BHDJSONENCODE_LOG(_BHDJSONENCODE_DEBUG_TRACE, "BHD-JSON-Encoder : Request for Get-Black-Hole-Detection \n");

  /* Validate Input Parameters */
  _BHDJSONENCODE_ASSERT (pData != NULL);

  /* allocate memory for JSON */
  status = bhdjson_memory_allocate(BHDJSON_MEMSIZE_RESPONSE, (uint8_t **) & jsonBuf);
  _BHDJSONENCODE_ASSERT (status == BVIEW_STATUS_SUCCESS);

  /* clear the buffer */
  memset(jsonBuf, 0, BHDJSON_MEMSIZE_RESPONSE);

  buffer = jsonBuf;

  /* convert asicId to external  notation */
  JSON_ASIC_ID_MAP_TO_NOTATION(asicId, &asicIdStr[0]);

  /* copying the header . Pointer and Length adjustments are handled by the macro */
  _BVIEW_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, 
      &totalLength, getBlackHoleDetectionTemplate, &asicIdStr[0], BVIEW_JSON_VERSION, pData->enable, method);

  /* setup the return value */
  *pJsonBuffer = (uint8_t *) jsonBuf;
  _BHDJSONENCODE_LOG(_BHDJSONENCODE_DEBUG_TRACE, "BHD-JSON-Encoder : Encoding complete [%d bytes] \n", (int)strlen(jsonBuf));

  _BHDJSONENCODE_LOG(_BHDJSONENCODE_DEBUG_DUMPJSON, "BHD-JSON-Encoder : %s \n", jsonBuf);

  return BVIEW_STATUS_SUCCESS;

}


/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-black-hole-detection" REST API.
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
 *           bhdjson_memory_free(). Failing to do so leads to memory leaks
 *********************************************************************/
BVIEW_STATUS bhdjson_encode_get_black_hole( int asicId,
                                            int method,
                                            const BHDJSON_CONFIGURE_BLACK_HOLE_t *pData,
                                            uint8_t **pJsonBuffer
                                            )

{
  char *getBlackHoleDetectionStart = " {\
                                      \"jsonrpc\": \"2.0\",\
                                      \"method\": \"get-black-hole\",\
                                      \"asic-id\": \"%s\",\
                                      \"version\": \"%d\",\
                                      \"result\": {";

  char *portListTemplate = " \"port-list\" : [ ";
  char *portTemplate = " \"%s\" ,";
  char *samplingMethodTemplate = " \"sampling-method\" : \"%s\" , \"sampling-params\" : ";
  char *agentParams = "{ \"water-mark\" : %d, \"sample-periodicity\" : %d, \"sample-count\" : %d }";
  char *sflowParams = "{ \
                       \"encapsulation-params\" : { \
                       \"vlan-id\" : %d ,\
                       \"destination-ip\" : \"%s\",\
                       \"source-udp-port\" : %d, \
                       \"destination-udp-port\" : %d}, \
                       \"mirror-port\" : \"%s\", \
                       \"sample-pool-size\" : %d }";

  char *endTemplate = "}, \"id\" : %d }";
  char *jsonBuf,  *buffer;
  char asicIdStr[JSON_MAX_NODE_LENGTH] = { 0 };
  char portStr[JSON_MAX_NODE_LENGTH] = { 0 };
  char ipStr[JSON_MAX_NODE_LENGTH] = { 0 };
  BVIEW_STATUS status;
  int actualLength = 0;
  int totalLength = 0;
  int remLength = BHDJSON_MEMSIZE_RESPONSE;
  int port = 0;
  BVIEW_PORT_MASK_t local_mask;
  struct in_addr *ptr;

  _BHDJSONENCODE_LOG(_BHDJSONENCODE_DEBUG_TRACE, "BHD-JSON-Encoder : Request for Get-Black-Hole-Detection \n");

  /* Validate Input Parameters */
  _BHDJSONENCODE_ASSERT (pData != NULL);

  /* allocate memory for JSON */
  status = bhdjson_memory_allocate(BHDJSON_MEMSIZE_RESPONSE, (uint8_t **) & jsonBuf);
  _BHDJSONENCODE_ASSERT (status == BVIEW_STATUS_SUCCESS);

  /* clear the buffer */
  memset(jsonBuf, 0, BHDJSON_MEMSIZE_RESPONSE);

  buffer = jsonBuf;

  /* convert asicId to external  notation */
  JSON_ASIC_ID_MAP_TO_NOTATION(asicId, &asicIdStr[0]);

  /* copying the header . Pointer and Length adjustments are handled by the macro */
  _BVIEW_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, 
      &totalLength, getBlackHoleDetectionStart, &asicIdStr[0], BVIEW_JSON_VERSION);

  {

    _BVIEW_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength,
        &totalLength, portListTemplate, NULL);

    memcpy (&local_mask, &pData->black_hole_port_mask, sizeof(BVIEW_PORT_MASK_t));

    /* loop through the port list and encode */
    BVIEW_FLMASKBIT(local_mask, port, sizeof(BVIEW_PORT_MASK_t));

    while (0 != port)
    {
      /* convert the port to an external representation */
      memset(&portStr[0], 0, JSON_MAX_NODE_LENGTH);
      JSON_PORT_MAP_TO_NOTATION(port, asicId, &portStr[0]);

      /* encode the port */
      _BVIEW_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength,
          &totalLength, portTemplate, &portStr[0]);

      BVIEW_CLRMASKBIT(local_mask, port);
      BVIEW_FLMASKBIT(local_mask, port, sizeof(BVIEW_PORT_MASK_t));
    }

    /* adjust the buffer to remove the last ',' */
    buffer = buffer - 1;
    remLength += 1;
    totalLength -= 1;

    _BVIEW_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength,
        &totalLength, "],");

    _BVIEW_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE (actualLength, buffer, remLength,
        &totalLength, samplingMethodTemplate, ((pData->sampling_config.sampling_method==BVIEW_BHD_AGENT_SAMPLING)?"agent":"sflow"));

    if (BVIEW_BHD_AGENT_SAMPLING == pData->sampling_config.sampling_method)
    {
      _BVIEW_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE (actualLength, buffer, remLength,
          &totalLength, agentParams, 
          pData->sampling_config.sampling_params.agent_sampling_params.water_mark,
          pData->sampling_config.sampling_params.agent_sampling_params.sample_periodicity,
          pData->sampling_config.sampling_params.agent_sampling_params.sample_count);
    }

    if (BVIEW_BHD_SFLOW_SAMPLING == pData->sampling_config.sampling_method)
    {
      ptr = (void *)&pData->sampling_config.sampling_params.sflow_sampling_params.encap_dest_ip_addr;
      inet_ntop(AF_INET, ptr, ipStr,  BVIEW_MAX_IP_ADDR_LENGTH);

      port = pData->sampling_config.sampling_params.sflow_sampling_params.mirror_port;
      /* convert the port to an external representation */
      memset(&portStr[0], 0, JSON_MAX_NODE_LENGTH);
      JSON_PORT_MAP_TO_NOTATION(port, asicId, &portStr[0]);


      _BVIEW_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE (actualLength, buffer, remLength,
          &totalLength, sflowParams, 
          pData->sampling_config.sampling_params.sflow_sampling_params.encap_vlan_id,
          &ipStr[0], 
          pData->sampling_config.sampling_params.sflow_sampling_params.encap_src_udp_port,
          pData->sampling_config.sampling_params.sflow_sampling_params.encap_dest_udp_port,
          &portStr[0],
          pData->sampling_config.sampling_params.sflow_sampling_params.sample_pool_size);
    }
  }


  /* append the id */

  _BVIEW_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE (actualLength, buffer, remLength,
      &totalLength, endTemplate, method);


  /* setup the return value */
  *pJsonBuffer = (uint8_t *) jsonBuf;
  _BHDJSONENCODE_LOG(_BHDJSONENCODE_DEBUG_TRACE, "BHD-JSON-Encoder : Encoding complete [%d bytes] \n", (int)strlen(jsonBuf));

  _BHDJSONENCODE_LOG(_BHDJSONENCODE_DEBUG_DUMPJSON, "BHD-JSON-Encoder : %s \n", jsonBuf);

  return BVIEW_STATUS_SUCCESS;

}

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-sflow-sampling-status" REST API.
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
 *           bhdjson_memory_free(). Failing to do so leads to memory leaks
 *********************************************************************/
BVIEW_STATUS bhdjson_encode_get_sampling_status( int asicId,
                                            int method,
                                            const BVIEW_BHD_SFLOW_SAMPLING_STATUS_t *pData,
                                            const BVIEW_TIME_t *time,
                                            uint8_t **pJsonBuffer
                                            )

{
  time_t report_time;
  struct tm *timeinfo;
  char timeString[64];

  char *getReportStart = " {\
                                      \"jsonrpc\": \"2.0\",\
                                      \"method\": \"get-sflow-sampling-status\",\
                                      \"asic-id\": \"%s\",\
                                      \"version\": \"%d\",\
                                      \"time-stamp\": \"%s\",\
                                      \"result\": {\
                                      \"data\":  [ ";

  char * recordTemplate = "{ \
                           \"port\" : \"%s\", \
                           \"sflow-sampling-enabled\" : %d, \
                           \"sampled-packet-count\" : %d, \
                           \"black-holed-packet-count\" : %d },";

  char *endTemplate = "] }, \"id\" : %d }";
  char *jsonBuf,  *buffer;
  char asicIdStr[JSON_MAX_NODE_LENGTH] = { 0 };
  char portStr[JSON_MAX_NODE_LENGTH] = { 0 };
  BVIEW_STATUS status;
  int actualLength = 0;
  int totalLength = 0;
  int remLength = BHDJSON_MEMSIZE_RESPONSE;
  int port = 0, p =0;

  _BHDJSONENCODE_LOG(_BHDJSONENCODE_DEBUG_TRACE, "BHD-JSON-Encoder : Request for Get-sflow-Sampling-Status \n");

  /* Validate Input Parameters */
  _BHDJSONENCODE_ASSERT (pData != NULL);

  /* obtain the time */
  memset(&timeString, 0, sizeof (timeString));
  report_time = *(time_t *) time;
  timeinfo = localtime(&report_time);
  strftime(timeString, 64, "%Y-%m-%d - %H:%M:%S ", timeinfo);

  /* allocate memory for JSON */
  status = bhdjson_memory_allocate(BHDJSON_MEMSIZE_RESPONSE, (uint8_t **) & jsonBuf);
  _BHDJSONENCODE_ASSERT (status == BVIEW_STATUS_SUCCESS);

  /* clear the buffer */
  memset(jsonBuf, 0, BHDJSON_MEMSIZE_RESPONSE);

  buffer = jsonBuf;

  /* convert asicId to external  notation */
  JSON_ASIC_ID_MAP_TO_NOTATION(asicId, &asicIdStr[0]);

  /* copying the header . Pointer and Length adjustments are handled by the macro */
  _BVIEW_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, 
      &totalLength, getReportStart, &asicIdStr[0], BVIEW_JSON_VERSION, timeString);


 for (p = 0; p < pData->num_ports; p++)
 {
   /* convert the port to an external representation */
   memset(&portStr[0], 0, JSON_MAX_NODE_LENGTH);

   port = pData->port_sflow_sampling[p].port;
   JSON_PORT_MAP_TO_NOTATION(port, asicId, &portStr[0]);

  _BVIEW_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, 
      &totalLength, recordTemplate, &portStr[0], 
      pData->port_sflow_sampling[p].sflowSamplingEnabled,
      pData->port_sflow_sampling[p].sampled_pkt_count,
      pData->port_sflow_sampling[p].black_holed_pkt_count);
 }

 /* adjust the buffer to remove the last ',' */
 buffer = buffer - 1;
 remLength += 1;
 totalLength -= 1;


  /* append the id */
  _BVIEW_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, 
      &totalLength, endTemplate, method);


  /* setup the return value */
  *pJsonBuffer = (uint8_t *) jsonBuf;
  _BHDJSONENCODE_LOG(_BHDJSONENCODE_DEBUG_TRACE, "BHD-JSON-Encoder : Encoding complete [%d bytes] \n", (int)strlen(jsonBuf));

  _BHDJSONENCODE_LOG(_BHDJSONENCODE_DEBUG_DUMPJSON, "BHD-JSON-Encoder : %s \n", jsonBuf);

  return BVIEW_STATUS_SUCCESS;

}

/******************************************************************
 * @brief  Creates a JSON buffer using the supplied data for the 
 *         "get-black-hole-event-report" REST API.
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
 *           bhdjson_memory_free(). Failing to do so leads to memory leaks
 *********************************************************************/
BVIEW_STATUS bhdjson_encode_get_black_hole_event_report( int asicId,
                                            int method,
                                            const BHD_BLACK_HOLE_EVENT_REPORT_t *pData,
                                            const BVIEW_TIME_t *time,
                                            uint8_t **pJsonBuffer
                                            )

{
  time_t report_time;
  struct tm *timeinfo;
  char timeString[64];

  char *getReportStart = " { \"jsonrpc\": \"2.0\",\
                                      \"method\": \"get-black-hole-event-report\",\
                                      \"asic-id\": \"%s\",\
                                      \"version\": \"%d\",\
                                      \"time-stamp\": \"%s\",\
                                      \"report\":  ";

  char * recordTemplate = "{ \
                           \"ingress-port\" : \"%s\", \
                           \"egress-port-list\" : [ ";

  char *portTemplate = " \"%s\" ,";
  char *blackHoledPacketTemplate = " \"black-holed-packet-count\" : %" PRIu64 ", \
                                 \"sample-packet\" : ";
  char *samplePacketTemplate = "\"%s\"";

  char *endTemplate = "} }";
  char *jsonBuf, *buffer;
  char asicIdStr[JSON_MAX_NODE_LENGTH] = { 0 };
  char portStr[JSON_MAX_NODE_LENGTH] = { 0 };
  BVIEW_STATUS status;
  int actualLength = 0;
  int totalLength = 0;
  int remLength = BHDJSON_MEMSIZE_RESPONSE;
  int port = 0;
  BVIEW_PORT_MASK_t local_mask;

  _BHDJSONENCODE_LOG(_BHDJSONENCODE_DEBUG_TRACE, "BHD-JSON-Encoder : Request for Get-sflow-Sampling-Status \n");

  /* Validate Input Parameters */
  _BHDJSONENCODE_ASSERT (pData != NULL);

  /* obtain the time */
  memset(&timeString, 0, sizeof (timeString));
  report_time = *(time_t *) time;
  timeinfo = localtime(&report_time);
  strftime(timeString, 64, "%Y-%m-%d - %H:%M:%S ", timeinfo);

  /* allocate memory for JSON */
  status = bhdjson_memory_allocate(BHDJSON_MEMSIZE_RESPONSE, (uint8_t **) & jsonBuf);
  _BHDJSONENCODE_ASSERT (status == BVIEW_STATUS_SUCCESS);

  /* clear the buffer */
  memset(jsonBuf, 0, BHDJSON_MEMSIZE_RESPONSE);

  buffer = jsonBuf;

  /* convert asicId to external  notation */
  JSON_ASIC_ID_MAP_TO_NOTATION(asicId, &asicIdStr[0]);

  /* copying the header . Pointer and Length adjustments are handled by the macro */
  _BVIEW_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, 
      &totalLength, getReportStart, &asicIdStr[0], BVIEW_JSON_VERSION, timeString);
 
    port = pData->ing_port;

   /* convert the port to an external representation */
   memset(&portStr[0], 0, JSON_MAX_NODE_LENGTH);

   port = pData->ing_port;
   JSON_PORT_MAP_TO_NOTATION(port, asicId, &portStr[0]);

  _BVIEW_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, 
      &totalLength, recordTemplate, &portStr[0]); 

  memcpy (&local_mask, &pData->egr_pbmp, sizeof(BVIEW_PORT_MASK_t));

  /* loop through the port list and encode */
  BVIEW_FLMASKBIT(local_mask, port, sizeof(BVIEW_PORT_MASK_t));

  while (0 != port)
  {
    /* convert the port to an external representation */
    memset(&portStr[0], 0, JSON_MAX_NODE_LENGTH);
    JSON_PORT_MAP_TO_NOTATION(port, asicId, &portStr[0]);

    /* encode the port */
    _BVIEW_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength,
        &totalLength, portTemplate, &portStr[0]);

    BVIEW_CLRMASKBIT(local_mask, port);
    BVIEW_FLMASKBIT(local_mask, port, sizeof(BVIEW_PORT_MASK_t));
  }

 /* adjust the buffer to remove the last ',' */
 buffer = buffer - 1;
 remLength += 1;
 totalLength -= 1;

 _BVIEW_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength,
        &totalLength, "],");

 _BVIEW_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength,
        &totalLength, blackHoledPacketTemplate, pData->total_black_holed_pkts);

  /* Encode the packet which is base64 encoded*/
 _BVIEW_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength,
        &totalLength, samplePacketTemplate, pData->json_pkt);

  /* append the id */
  _BVIEW_JSONENCODE_COPY_FORMATTED_STRING_AND_ADVANCE(actualLength, buffer, remLength, 
      &totalLength, endTemplate, NULL);


  /* setup the return value */
  *pJsonBuffer = (uint8_t *) jsonBuf;
  _BHDJSONENCODE_LOG(_BHDJSONENCODE_DEBUG_TRACE, "BHD-JSON-Encoder : Encoding complete [%d bytes] \n", (int)strlen(jsonBuf));

  _BHDJSONENCODE_LOG(_BHDJSONENCODE_DEBUG_DUMPJSON, "BHD-JSON-Encoder : %s \n", jsonBuf);

  return BVIEW_STATUS_SUCCESS;

}

