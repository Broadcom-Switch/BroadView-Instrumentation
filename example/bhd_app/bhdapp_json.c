
/*********************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename bhdapp_json.c 
  *
  * @purpose BroadView BHD reference application 
  *
  * @component Black hole detection 
  *
  * @comments
  *
  * @create 3/17/2016
  *
  * @author 
  * @end
  *
  **********************************************************************/
#include "bhdapp.h"
#include "bhdapp_menu.h"


/* Rest mesages for JSON methods */ 
BHDAPP_REST_MSG_t bhdRestMessages[] = {
    {
     .methodId = BHDAPP_JSON_CONFIGURE_BLACK_HOLE_DETECTION,
     .httpMethod = "POST",
     .descr = "Configure Black Hole Detection",
     .method = "black-hole-detection-enable",
    },
    {
     .methodId = BHDAPP_JSON_GET_BLACK_HOLE_DETECTION,
     .httpMethod = "GET",
     .descr = "Obtaining current Black Hole Detection status",
     .method = "get-black-hole-detection-enable",
    },
    {
     .methodId = BHDAPP_JSON_CONFIGURE_BLACK_HOLE,
     .httpMethod = "POST",
     .descr = "Configure Black Hole",
     .method = "configure-black-hole",
    },
    {
     .methodId = BHDAPP_JSON_GET_BLACK_HOLE,
     .httpMethod = "GET",
     .descr = "Obtaining current Black Hole Details",
     .method = "get-black-hole",
    },
    {
     .methodId = BHDAPP_JSON_CANCEL_BLACK_HOLE,
     .httpMethod = "POST",
     .descr = "Cancel Black Hole",
     .method = "cancel-black-hole",
    },
    {
     .methodId = BHDAPP_JSON_GET_SFLOW_SAMPLING_STATUS, 
     .httpMethod = "GET",
     .descr = "Get sFlow sampling status",
     .method = "get-sflow-sampling-status",
    },
};


/* JSON format for different methods */ 
BHDAPP_JSON_METHOD_INFO_t bhdJsonMethodDetails[BHDAPP_JSON_METHOD_LAST] = {
    {
     .methodId = BHDAPP_JSON_CONFIGURE_BLACK_HOLE_DETECTION,
     .menuString = "Configure Black Hole Detection Feature",
     .method = "black-hole-detection-enable",
     .get_input_for_json = config_bhd_feature,
     .jsonFrmt = "{      \
                     \"jsonrpc\": \"2.0\", \
                     \"method\": \"black-hole-detection-enable\", \
                     \"asic-id\": \"%s\", \
                     \"params\": {  \
                     \"enable\": %u \
                     }, \
				     \"id\": %u \
                  }",

    },

    {
     .methodId = BHDAPP_JSON_GET_BLACK_HOLE_DETECTION,
     .method = "get-black-hole-detection-enable",  
     .menuString = "Get Black Hole Detection feature status",
     .get_input_for_json = get_bhd_feature,
     .jsonFrmt = "{      \
                     \"jsonrpc\": \"2.0\", \
                     \"method\": \"get-black-hole-detection-enable\", \
                     \"asic-id\": \"%s\", \
                     \"params\": {  \
                     }, \
                     \"id\": %u \
                  }",
    },

    {
     .methodId = BHDAPP_JSON_CONFIGURE_BLACK_HOLE, 
     .method = "configure-black-hole",  
     .menuString = "Configure Black Hole",
     .get_input_for_json = configure_bhd_black_hole,
     .jsonFrmt = "{      \
                     \"jsonrpc\": \"2.0\", \
                     \"method\": \"configure-black-hole\", \
                     \"asic-id\": \"%s\", \
                     \"params\": {  \
                     \"port-list\" : [%s], \
                     \"sampling-method\": \"%s\", \
                     \"sampling-params\" : %s     \
                     }, \
				     \"id\": %u \
                  }",

    },

    {
     .methodId = BHDAPP_JSON_GET_BLACK_HOLE, 
     .method = "get-black-hole",
     .menuString = "Get Configured Black Hole Info",
     .get_input_for_json = get_bhd_black_hole,
     .jsonFrmt = "{      \
                     \"jsonrpc\": \"2.0\", \
                     \"method\": \"get-black-hole\", \
                     \"asic-id\": \"%s\", \
                     \"params\": {  \
                     }, \
				     \"id\": %u \
                  }",

    },
    {
     .methodId = BHDAPP_JSON_CANCEL_BLACK_HOLE, 
     .method = "cancel-black-hole",
     .menuString = "Cancel Black Hole",
     .get_input_for_json = cancel_bhd_black_hole,
     .jsonFrmt = "{      \
                     \"jsonrpc\": \"2.0\", \
                     \"method\": \"cancel-black-hole\", \
                     \"asic-id\": \"%s\", \
                     \"params\": {  \
                     }, \
				     \"id\": %u \
                  }",

    },

    {
     .methodId = BHDAPP_JSON_GET_SFLOW_SAMPLING_STATUS,
     .method = "get-sflow-sampling-status",  
     .menuString = "Get sFlow sampling status",
     .get_input_for_json = get_bhd_sflow_sampling_status,
     .jsonFrmt =  "{      \
                      \"jsonrpc\": \"2.0\", \
                      \"method\": \"get-sflow-sampling-status\", \
                      \"asic-id\": \"%s\", \
                      \"params\": {  \
                      \"port-list\" : [%s] \
                      }, \
                      \"id\": %u \
                   }",

    },

};

/* A global buf to prepare the JSON message */
char jsonBufGlobal[BHDAPP_JSON_BUFF_MAX_SIZE ]= {0};
/* JSON id counter */  
static unsigned int jsonIdCnt = 0;

/******************************************************************
 * @brief     Returns a new JSON id.
 *
 * @param[in]   none
 *
 * @retval      A new JSON id
 * @note     
 *********************************************************************/
unsigned int get_new_json_id()
{
  jsonIdCnt++;

  if (jsonIdCnt == 0)  /* Not allowing Id '0' */
  {
    jsonIdCnt++;
  }
  return jsonIdCnt;
}

