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

#ifndef INCLUDE_PACKET_TRACE_JSON_MEMORY_H
#define	INCLUDE_PACKET_TRACE_JSON_MEMORY_H

#include "broadview.h"

#ifdef	__cplusplus
extern "C"
{
#endif

/* A small implementation of a memory pool, that offers buffers in two sizes */

typedef enum _ptjson_memory_size_
{
    PTJSON_MEMSIZE_RESPONSE = 1024,
    PTJSON_MEMSIZE_REPORT = (sizeof(BVIEW_PT_PROFILE_RECORD_t)+ 2048),
} PTJSON_MEMORY_SIZE;


BVIEW_STATUS ptjson_memory_init(void);
BVIEW_STATUS ptjson_memory_allocate(PTJSON_MEMORY_SIZE memSize, uint8_t **buffer);
BVIEW_STATUS ptjson_memory_free(uint8_t *buffer);
void ptjson_memory_dump(void);

#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_PACKET_TRACE_JSON_MEMORY_H */

