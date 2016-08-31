/*********************************************************************
  *
  * Copyright © 2016 Broadcom.  The term "Broadcom" refers
  * to Broadcom Limited and/or its subsidiaries.
  *
  ***********************************************************************
  *
  * @filename bhd_json_memory.h 
  *
  * @purpose BroadView BHD JSON memory apis. 
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


#ifndef INCLUDE_BHD_JSON_MEMORY_H
#define	INCLUDE_BHD_JSON_MEMORY_H

#include "broadview.h"
#include "bhd.h"


#ifdef	__cplusplus
extern "C"
{
#endif

/* A small implementation of a memory pool, that offers buffers in two sizes */

typedef enum _bhdjson_memory_size_
{
    BHDJSON_MEMSIZE_RESPONSE = (4*1024),
    BHDJSON_MEMSIZE_REPORT = (8*2048),
} BHDJSON_MEMORY_SIZE;


BVIEW_STATUS bhdjson_memory_init(void);
BVIEW_STATUS bhdjson_memory_allocate(BHDJSON_MEMORY_SIZE memSize, uint8_t **buffer);
BVIEW_STATUS bhdjson_memory_free(uint8_t *buffer);
void bhdjson_memory_dump(void);

#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_BHD_JSON_MEMORY_H */

