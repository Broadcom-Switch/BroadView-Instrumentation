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

#ifndef INCLUDE_BST_JSON_MEMORY_H
#define	INCLUDE_BST_JSON_MEMORY_H

#include "broadview.h"
#include "bst.h"


#ifdef	__cplusplus
extern "C"
{
#endif

/* A small implementation of a memory pool, that offers buffers in two sizes */

typedef enum _bstjson_memory_size_
{
    BSTJSON_MEMSIZE_RESPONSE = 1024,
    BSTJSON_MEMSIZE_REPORT = (sizeof(BVIEW_BST_ASIC_SNAPSHOT_DATA_t)+ (32*2048)),
} BSTJSON_MEMORY_SIZE;


BVIEW_STATUS bstjson_memory_init(void);
BVIEW_STATUS bstjson_memory_allocate(BSTJSON_MEMORY_SIZE memSize, uint8_t **buffer);
BVIEW_STATUS bstjson_memory_free(uint8_t *buffer);
void bstjson_memory_dump(void);

#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_BST_JSON_MEMORY_H */

