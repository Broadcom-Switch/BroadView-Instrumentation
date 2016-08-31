/*********************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename cancel_black_hole.h 
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


#ifndef INCLUDE_CANCEL_BLACK_HOLE_H 
#define	INCLUDE_CANCEL_BLACK_HOLE_H  

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
typedef struct _bhdjson_cancel_black_hole_
{
} BHDJSON_CANCEL_BLACK_HOLE_t;


/* Function Prototypes */
BVIEW_STATUS bhdjson_cancel_black_hole(void *cookie, char *jsonBuffer, int bufLength);
BVIEW_STATUS bhdjson_cancel_black_hole_impl(void *cookie, int asicId, int id, BHDJSON_CANCEL_BLACK_HOLE_t *pCommand);


#ifdef	__cplusplus  
}
#endif  

#endif /* INCLUDE_CANCEL_BLACK_HOLE_DETECTION_H */ 

