/*********************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename get_black_hole_detection.h 
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


#ifndef INCLUDE_GET_BLACK_HOLE_DETECTION_H 
#define	INCLUDE_GET_BLACK_HOLE_DETECTION_H  

#ifdef	__cplusplus  
extern "C"
{
#endif  


/* Include Header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "broadview.h"
#include "json.h"

#include "cJSON.h"
#include "bhd.h"

/* Structure to pass API parameters to the BHD APP */
typedef struct _bhdjson_get_black_hole_detection_
{
} BHDJSON_GET_BLACK_HOLE_DETECTION_t;


/* Function Prototypes */
BVIEW_STATUS bhdjson_get_black_hole_detection(void *cookie, char *jsonBuffer, int bufLength);
BVIEW_STATUS bhdjson_get_black_hole_detection_impl(void *cookie, int asicId, int id, BHDJSON_GET_BLACK_HOLE_DETECTION_t *pCommand);


#ifdef	__cplusplus  
}
#endif  

#endif /* INCLUDE_GET_BLACK_HOLE_DETECTION_H */ 

