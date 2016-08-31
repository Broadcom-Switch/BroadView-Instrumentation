
/*********************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename bhdapp_json.h 
  *
  * @purpose BroadView BHD reference Application  
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

#ifndef INCLUDE_BHDAPP_JSON_H
#define INCLUDE_BHDAPP_JSON_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include "bhdapp.h"


/* Rest mesages for JSON methods */ 
extern BHDAPP_REST_MSG_t bhdRestMessages[]; 

/* JSON format for different methods */ 
extern BHDAPP_JSON_METHOD_INFO_t bhdJsonMethodDetails[BHDAPP_JSON_METHOD_LAST];

/* A global buf to prepare the JSON message */
extern char jsonBufGlobal[BHDAPP_JSON_BUFF_MAX_SIZE];

/******************************************************************
 * @brief     Returns a new JSON id.
 *
 * @param[in]   none
 *
 * @retval      A new JSON id
 * @note     
 *********************************************************************/
extern unsigned int get_new_json_id();

#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_BHDAPP_JSON_H */

