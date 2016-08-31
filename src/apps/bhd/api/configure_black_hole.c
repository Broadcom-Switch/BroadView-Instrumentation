/*********************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename configure_black_hole.c 
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
#include "configure_black_hole.h"

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
BVIEW_STATUS bhdjson_configure_black_hole (void *cookie, char *jsonBuffer, int bufLength)
{

    /* Local Variables for JSON Parsing */
    cJSON *json_jsonrpc, *json_method, *json_asicId;
    cJSON *json_id, *json_ports_array, *json_ports;
    cJSON *json_sampling_method = NULL, *json_sampling_params = NULL, *json_watermark = NULL;
    cJSON *root, *params, *json_sample_periodicity = NULL;
    cJSON *json_sample_count = NULL, *json_encapsulation_params = NULL, *json_vlan = NULL;
    cJSON *json_dest_ip = NULL, *json_source_udp_port = NULL, *json_destination_udp_port = NULL;
    cJSON *json_mirror_port = NULL, *json_destination_sample_pool_size = NULL;
    /* Local non-command-parameter JSON variable declarations */
    char jsonrpc[JSON_MAX_NODE_LENGTH] = {0};
    char method[JSON_MAX_NODE_LENGTH] = {0};
    char ports[128] = {0};
    char ipStr[128] = {0};
    char jsonSamplingMethod[128] = {0};
    int asicId = 0, id = 0, iter = 0, sampling_method = 0;
    struct sockaddr_in destIp;
    BVIEW_PORT_MASK_t temp_pbmp;

    /* Local variable declarations */
    BVIEW_STATUS status = BVIEW_STATUS_SUCCESS;
    BHDJSON_CONFIGURE_BLACK_HOLE_t command;

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
    JSON_COMPARE_STRINGS_AND_CLEANUP ("method", &method[0], "configure-black-hole");


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
      /* parse the port list */
      json_ports_array = cJSON_GetObjectItem(params, "port-list");
      JSON_VALIDATE_JSON_POINTER(json_ports_array, "port-list", BVIEW_STATUS_INVALID_JSON);

      memset(&command.black_hole_port_mask, 0, sizeof(BVIEW_PORT_MASK_t));

      for (iter = 0; iter < cJSON_GetArraySize(json_ports_array); iter++)
      {
        memset (ports, 0, 128);
        memset (&temp_pbmp, 0, sizeof(BVIEW_PORT_MASK_t));
        json_ports = cJSON_GetArrayItem(json_ports_array, iter);
        strncpy (&ports[0], json_ports->valuestring, strlen(json_ports->valuestring));
        PT_PORT_LIST_TO_MASK_CONVERT(ports, &temp_pbmp);
        BVIEW_MASKOREQ (command.black_hole_port_mask, temp_pbmp, sizeof(BVIEW_PORT_MASK_t));
      }

      /* parse the sampling-method */
      json_sampling_method = cJSON_GetObjectItem(params, "sampling-method");
      JSON_VALIDATE_JSON_POINTER(json_sampling_method, "sampling-method", BVIEW_STATUS_INVALID_JSON);
      JSON_VALIDATE_JSON_AS_STRING(json_sampling_method, "sampling-method", BVIEW_STATUS_INVALID_JSON);
      /* Copy the string, with a limit on max characters */
      strncpy (&jsonSamplingMethod[0], json_sampling_method->valuestring, JSON_MAX_NODE_LENGTH - 1);

      /* initialize the sampling method to invalid number */
      sampling_method = 0xFF;

      if (0 == (strncmp ("agent", &jsonSamplingMethod[0], strlen("agent"))))
      {
        sampling_method = BVIEW_BHD_AGENT_SAMPLING;
      }

      if (0 == (strncmp ("sflow", &jsonSamplingMethod[0], strlen("sflow"))))
      {
        sampling_method = BVIEW_BHD_SFLOW_SAMPLING;
      }

      /* validate the string */
      if (0xFF == sampling_method)
      {
        /* the string is neither "agent" nor "sflow" */
        /* Free up any allocated resources and return status code */
        if (root != NULL)
        {
          cJSON_Delete(root);
        }
        return BVIEW_STATUS_INVALID_PARAMETER;
      }
      command.sampling_config.sampling_method = sampling_method;

      /* get the sampling params */
      json_sampling_params = cJSON_GetObjectItem(params, "sampling-params");
      JSON_VALIDATE_JSON_POINTER(json_sampling_params, "sampling-params", BVIEW_STATUS_INVALID_JSON);

      if (BVIEW_BHD_AGENT_SAMPLING == sampling_method)
      {
        /* Parsing and Validating 'water-mark' from JSON buffer */
        json_watermark = cJSON_GetObjectItem(json_sampling_params, "water-mark");
        JSON_VALIDATE_JSON_POINTER(json_watermark, "water-mark", BVIEW_STATUS_INVALID_JSON);
        JSON_VALIDATE_JSON_AS_NUMBER(json_watermark, "water-mark");
        /* Copy the value */
        command.sampling_config.sampling_params.agent_sampling_params.water_mark = json_watermark->valueint;
        /* Ensure  that the number 'watermark' is within range */
        JSON_CHECK_VALUE_AND_CLEANUP (command.sampling_config.sampling_params.agent_sampling_params.water_mark, BVIEW_BHD_WATERMARK_MIN, BVIEW_BHD_WATERMARK_MAX);


        /* Parsing and Validating 'sample-periodicity' from JSON buffer */
        json_sample_periodicity = cJSON_GetObjectItem(json_sampling_params, "sample-periodicity");

        if (NULL != json_sample_periodicity)
        {
          JSON_VALIDATE_JSON_POINTER(json_sample_periodicity, "sample-periodicity", BVIEW_STATUS_INVALID_JSON);
          JSON_VALIDATE_JSON_AS_NUMBER(json_sample_periodicity, "sample-periodicity");
          /* Copy the value */
          command.sampling_config.sampling_params.agent_sampling_params.sample_periodicity = json_sample_periodicity->valueint;
          /* Ensure  that the number 'samplePeriodicity' is within range  */
          JSON_CHECK_VALUE_AND_CLEANUP (command.sampling_config.sampling_params.agent_sampling_params.sample_periodicity, 
              BVIEW_BHD_SAMPLE_PERIODICITY_MIN, BVIEW_BHD_SAMPLE_PERIODICITY_MAX);

        }

        /* Parsing and Validating 'sample-count' from JSON buffer */
        json_sample_count = cJSON_GetObjectItem(json_sampling_params, "sample-count");

        if (NULL != json_sample_count)
        {
          JSON_VALIDATE_JSON_POINTER(json_sample_count, "sample-count", BVIEW_STATUS_INVALID_JSON);
          JSON_VALIDATE_JSON_AS_NUMBER(json_sample_count, "sample-count");
          /* Copy the value */
          command.sampling_config.sampling_params.agent_sampling_params.sample_count = json_sample_count->valueint;
          /* Ensure  that the number 'sampleCount' is within range  */
          JSON_CHECK_VALUE_AND_CLEANUP (command.sampling_config.sampling_params.agent_sampling_params.sample_count, 
              BVIEW_BHD_SAMPLE_COUNT_MIN, BVIEW_BHD_SAMPLE_COUNT_MAX);
        }
      }

      if (BVIEW_BHD_SFLOW_SAMPLING == sampling_method)
      {

        /* get the sampling params */
        json_encapsulation_params = cJSON_GetObjectItem(json_sampling_params, "encapsulation-params");
        JSON_VALIDATE_JSON_POINTER(json_encapsulation_params, "encapsulation-params", BVIEW_STATUS_INVALID_JSON);

        /* Parsing and Validating 'vlan-id' from JSON buffer */
        json_vlan = cJSON_GetObjectItem(json_encapsulation_params, "vlan-id");
        if (NULL != json_vlan)
        {
          JSON_VALIDATE_JSON_POINTER(json_vlan, "vlan-id", BVIEW_STATUS_INVALID_JSON);
          JSON_VALIDATE_JSON_AS_NUMBER(json_vlan, "vlan-id");
          /* Copy the value */
          command.sampling_config.sampling_params.sflow_sampling_params.encap_vlan_id = json_vlan->valueint;
          /* Ensure  that the number 'vlan' is within range */
          JSON_CHECK_VALUE_AND_CLEANUP (command.sampling_config.sampling_params.sflow_sampling_params.encap_vlan_id, 
              BVIEW_BHD_VLAN_MIN, BVIEW_BHD_VLAN_MAX);
        }

        /* Parsing and Validating 'destination-ip' from JSON buffer */
        json_dest_ip = cJSON_GetObjectItem(json_encapsulation_params, "destination-ip");
        JSON_VALIDATE_JSON_POINTER(json_dest_ip, "destination-ip", BVIEW_STATUS_INVALID_JSON);
        JSON_VALIDATE_JSON_AS_STRING(json_dest_ip, "destination-ip", BVIEW_STATUS_INVALID_JSON);
        /* Copy the string, with a limit on max characters */
        strncpy (&ipStr[0], json_dest_ip->valuestring, JSON_MAX_NODE_LENGTH - 1);
        if (inet_pton(AF_INET, ipStr, &(destIp.sin_addr)) != 1)
        {
          if (root != NULL)
          {
            cJSON_Delete(root);
          }
          return BVIEW_STATUS_INVALID_PARAMETER;
          
        }
        command.sampling_config.sampling_params.sflow_sampling_params.encap_dest_ip_addr = destIp.sin_addr;

        /* Parsing and Validating 'source-udp-port' from JSON buffer */
        json_source_udp_port = cJSON_GetObjectItem(json_encapsulation_params, "source-udp-port");
        JSON_VALIDATE_JSON_POINTER(json_source_udp_port, "source-udp-port", BVIEW_STATUS_INVALID_JSON);
        JSON_VALIDATE_JSON_AS_NUMBER(json_source_udp_port, "source-udp-port");
        /* Copy the value */
        command.sampling_config.sampling_params.sflow_sampling_params.encap_src_udp_port = json_source_udp_port->valueint;
        /* Ensure  that the number 'sourceUdpPort' is within range */
        if (command.sampling_config.sampling_params.sflow_sampling_params.encap_src_udp_port < BVIEW_BHD_SOURCE_UDP_PORT_MIN)
        {
          cJSON_Delete(root);
        }

        /* Parsing and Validating 'destination-udp-port' from JSON buffer */
        json_destination_udp_port = cJSON_GetObjectItem(json_encapsulation_params, "destination-udp-port");
        JSON_VALIDATE_JSON_POINTER(json_destination_udp_port, "destination-udp-port", BVIEW_STATUS_INVALID_JSON);
        JSON_VALIDATE_JSON_AS_NUMBER(json_destination_udp_port, "destination-udp-port");
        /* Copy the value */
        command.sampling_config.sampling_params.sflow_sampling_params.encap_dest_udp_port = json_destination_udp_port->valueint;
        /* Ensure  that the number 'destUdpPort' is within range */
        if (command.sampling_config.sampling_params.sflow_sampling_params.encap_dest_udp_port < BVIEW_BHD_SOURCE_UDP_PORT_MIN)
        {
          cJSON_Delete(root);
        }

        /* parse mirror-port */
        json_mirror_port = cJSON_GetObjectItem(json_sampling_params, "mirror-port");
        JSON_VALIDATE_JSON_POINTER(json_mirror_port, "mirror-port", BVIEW_STATUS_INVALID_JSON);
        JSON_VALIDATE_JSON_AS_STRING(json_mirror_port, "mirror-port", BVIEW_STATUS_INVALID_JSON);
        /* Copy the string, with a limit on max characters */
        JSON_PORT_MAP_FROM_NOTATION(command.sampling_config.sampling_params.sflow_sampling_params.mirror_port, json_mirror_port->valuestring);

        /* parse sample-pool-size */
        json_destination_sample_pool_size = cJSON_GetObjectItem(json_sampling_params, "sample-pool-size");

          JSON_VALIDATE_JSON_POINTER(json_destination_sample_pool_size, "sample-pool-size", BVIEW_STATUS_INVALID_JSON);
          JSON_VALIDATE_JSON_AS_NUMBER(json_destination_sample_pool_size, "sample-pool-size");
          /* Copy the value */
          command.sampling_config.sampling_params.sflow_sampling_params.sample_pool_size = json_destination_sample_pool_size->valueint;
          /* Ensure  that the number 'samplePoolSize' is within range */
          JSON_CHECK_VALUE_AND_CLEANUP (command.sampling_config.sampling_params.sflow_sampling_params.sample_pool_size, 
              BVIEW_BHD_SAMP_POOL_SIZE_MIN, BVIEW_BHD_SAMP_POOL_SIZE_MAX);

      }


    /* Send the 'command' along with 'asicId' and 'cookie' to the Application thread. */
    status = bhdjson_configure_black_hole_impl (cookie, asicId, id, &command);

    /* Free up any allocated resources and return status code */
    if (root != NULL)
    {
        cJSON_Delete(root);
    }

    return status;
}
