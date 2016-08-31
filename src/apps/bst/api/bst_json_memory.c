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

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <inttypes.h>

#include "broadview.h"
#include "bst_json_memory.h"

#define _BUFPOOL_DEBUG
#define _BUFPOOL_DEBUG_LEVEL        _BUFPOOL_DEBUG_ERROR

#define _BUFPOOL_DEBUG_TRACE        (0x1)
#define _BUFPOOL_DEBUG_INFO         (0x01 << 1)
#define _BUFPOOL_DEBUG_ERROR        (0x01 << 2)
#define _BUFPOOL_DEBUG_ALL          (0xFF)

#ifdef _BUFPOOL_DEBUG
#define _BUFPOOL_LOG(level, format,args...)   do { \
            if ((level) & _BUFPOOL_DEBUG_LEVEL) { \
                printf(format, ##args); \
            } \
        }while(0)
#else 
#define _BUFPOOL_LOG(level, format,args...)
#endif

/* The following are proportional to number of collectors */

#define _BUFPOOL_MAX_RESPONE_SLICES      20
#define _BUFPOOL_MAX_REPORT_SLICES       4       

/* The following structure represents a memory slice for allocation management */

typedef struct _memory_slice_
{
    uint8_t *buffer;
    BSTJSON_MEMORY_SIZE size;
    bool inUse;
    time_t timeTaken;
    int command;
} _BUFPOOL_MEMORY_SLICE_t;

/* Buffer Pool, will be filled in during initialization */
/* There is no necessity for a 'page'/ 'cacheline' alignment */
/* so, simple static buffers would do just well */
/* we manipulate the index based on the fact that the memory */

/* is allocated in a contiguous way */

static struct _memory_pool_
{
    /* Buffers */

    uint8_t smallBufferPool[_BUFPOOL_MAX_RESPONE_SLICES][BSTJSON_MEMSIZE_RESPONSE];
    uint8_t largeBufferPool[_BUFPOOL_MAX_REPORT_SLICES][BSTJSON_MEMSIZE_REPORT];

    /* Buffer Descriptors */

    _BUFPOOL_MEMORY_SLICE_t smallSlices[_BUFPOOL_MAX_RESPONE_SLICES];
    _BUFPOOL_MEMORY_SLICE_t largeSlices[_BUFPOOL_MAX_REPORT_SLICES];

    /* Internal Buffer Management */
    pthread_mutex_t lock;

    /* timer purposes */
    time_t start;

} bstJsonMemoryPool;

static struct _memory_pool_ *pBufPool;

/* Utility Macros for parameter validation */
#define _BUFPOOL_ASSERT_ERROR(condition, errcode) do { \
    if (!(condition)) { \
        _BUFPOOL_LOG(_BUFPOOL_DEBUG_ERROR, \
                    "BST Buffer Pool (%s:%d) Invalid Input Parameter  \n", \
                    __func__, __LINE__); \
        return (errcode); \
    } \
} while(0)

#define _BUFPOOL_ASSERT(condition) _BUFPOOL_ASSERT_ERROR((condition), (BVIEW_STATUS_INVALID_PARAMETER))

/* Utility Macros */

#define _BUFPOOL_SMALL_POOL_START   ((ptr_to_uint_t) & (pBufPool->smallBufferPool[0][0]))
#define _BUFPOOL_SMALL_POOL_END     (_BUFPOOL_SMALL_POOL_START + ( (_BUFPOOL_MAX_RESPONE_SLICES-1) * BSTJSON_MEMSIZE_RESPONSE))
#define _BUFPOOL_LARGE_POOL_START   ((ptr_to_uint_t) & (pBufPool->largeBufferPool[0][0]))
#define _BUFPOOL_LARGE_POOL_END     (_BUFPOOL_LARGE_POOL_START + ( (_BUFPOOL_MAX_REPORT_SLICES-1) * BSTJSON_MEMSIZE_REPORT))


/* Utility Macros for streamlining the buffer pool access */

#define _BUFPOOL_MUTEX_TAKE(lock) do { \
    int rv; \
    _BUFPOOL_LOG(_BUFPOOL_DEBUG_TRACE, "BST BUffer Pool : Acquiring Mutex \n"); \
    rv = pthread_mutex_lock(&(lock)); \
    if (rv != 0) { \
        _BUFPOOL_LOG(_BUFPOOL_DEBUG_ERROR, "BST BUffer Pool : Acquiring Mutex Failed : %d \n", rv); \
        return BVIEW_STATUS_RESOURCE_NOT_AVAILABLE; \
    } \
 } while(0)

#define _BUFPOOL_MUTEX_RELEASE(lock) do { \
    int rv; \
    _BUFPOOL_LOG(_BUFPOOL_DEBUG_TRACE, "BST BUffer Pool : Releasing Mutex \n"); \
    rv = pthread_mutex_unlock(&(lock)); \
    if (rv != 0) { \
        _BUFPOOL_LOG(_BUFPOOL_DEBUG_ERROR, "BST BUffer Pool : Releasing Mutex Failed : %d \n", rv); \
        return BVIEW_STATUS_RESOURCE_NOT_AVAILABLE; \
    } \
 } while(0)

/*****************************************************************//**
* @brief  Initialize Buffer Pool.
*
*
* @retval   BVIEW_STATUS_SUCCESS    if buffer pool is initialized successfully.
* @retval   BVIEW_STATUS_FAILURE    on any internal error.
*
  *********************************************************************/

BVIEW_STATUS bstjson_memory_init(void)
{
    int index = 0;

    _BUFPOOL_LOG(_BUFPOOL_DEBUG_TRACE, "BST BUffer Pool : Initializing \n");

    /* clear the memory */
    memset(&bstJsonMemoryPool, 0, sizeof (bstJsonMemoryPool));

    /* Initialize the Buffer Descriptors */

    pBufPool = &bstJsonMemoryPool;

    for (index = 0; index < _BUFPOOL_MAX_RESPONE_SLICES; index++)
    {
        pBufPool->smallSlices[index].buffer = &pBufPool->smallBufferPool[index][0];
        pBufPool->smallSlices[index].size = BSTJSON_MEMSIZE_RESPONSE;
        pBufPool->smallSlices[index].inUse = false;
        pBufPool->smallSlices[index].timeTaken = 0;
    }

    for (index = 0; index < _BUFPOOL_MAX_REPORT_SLICES; index++)
    {
        pBufPool->largeSlices[index].buffer = &pBufPool->largeBufferPool[index][0];
        pBufPool->largeSlices[index].size = BSTJSON_MEMSIZE_REPORT;
        pBufPool->largeSlices[index].inUse = false;
        pBufPool->largeSlices[index].timeTaken = 0;
    }

    /* Create the lock */
    pthread_mutex_init(&(pBufPool->lock), NULL);

    time(&pBufPool->start);

    return BVIEW_STATUS_SUCCESS;
}

/******************************************************************
 * @brief  Allocates the desired buffer from the corresponding pool
 *
 * @param[in]    memSize     Memory Size (RESPONSE | REPORT)
 * @param[out]   buffer      Pointer to the allocated buffer.
 *                           
 * @retval   BVIEW_STATUS_SUCCESS  Buffer is allocated successfully
 * @retval   BVIEW_STATUS_OUTOFMEMORY  No free buffers are available
 * @retval   BVIEW_STATUS_RESOURCE_NOT_AVAILABLE  Attempting to take buffer recursively
 * @retval   BVIEW_STATUS_INVALID_PARAMETER  Invalid input parameter
 *
 * @note     Only predefined (two types) sized buffers are supported.
 *           See BSTJSON_MEMORY_SIZE.
 *           The allocated buffer must be freed with a call to 
 *           bstjson_memory_free()
 *********************************************************************/
BVIEW_STATUS bstjson_memory_allocate(BSTJSON_MEMORY_SIZE memSize, uint8_t **buffer)
{
    int index, max;
    _BUFPOOL_MEMORY_SLICE_t *pSlice;

    _BUFPOOL_LOG(_BUFPOOL_DEBUG_TRACE,
                 "BST BUffer Pool : Request for allocation of memory size %d \n",
                 memSize);

    /* Validate Input parameters */

    _BUFPOOL_ASSERT(buffer != NULL);
    _BUFPOOL_ASSERT((memSize == BSTJSON_MEMSIZE_RESPONSE) ||
                    (memSize == BSTJSON_MEMSIZE_REPORT));

    /* setup our data for lookup */

    if (memSize == BSTJSON_MEMSIZE_RESPONSE)
    {
        pSlice = &pBufPool->smallSlices[0];
        max = _BUFPOOL_MAX_RESPONE_SLICES;
    }
    else
    {
        pSlice = &pBufPool->largeSlices[0];
        max = _BUFPOOL_MAX_REPORT_SLICES;
    }

    /* Obtain the lock for lookup */
    _BUFPOOL_MUTEX_TAKE(pBufPool->lock);

    /* The following looks up the buffer list and attempts to find 
     * a buffer descriptor (a slice) which is 'available'. 
     * When an available buffer is found, it returns that pointer to the
     * caller, after necessary house-keeping.
     */

    for (index = 0; index < max; index++)
    {

        if ((pSlice->inUse) == false)
        {
            pSlice->inUse = true;
            pSlice->size = memSize;
            break;
        }

        pSlice++;
    }

    /* Release the mutex */
    _BUFPOOL_MUTEX_RELEASE(pBufPool->lock);

    /* if we found some available buffer, return to caller */
    if (index != max)
    {
        /* update the caller */
        *buffer = pSlice->buffer;
        time(&pSlice->timeTaken);
        _BUFPOOL_LOG(_BUFPOOL_DEBUG_TRACE,
                     "BST BUffer Pool : Allocated memory[ %"PRI_PTR_TO_UINT_FMT" - index=%d] size %d at %d \n",
                     (ptr_to_uint_t)(*buffer), index, memSize, (int) pSlice->timeTaken);
        return BVIEW_STATUS_SUCCESS;
    }

    _BUFPOOL_LOG(_BUFPOOL_DEBUG_ERROR,
                 "BST BUffer Pool : Failed to allocated memory size %d \n",
                 memSize);
    return BVIEW_STATUS_OUTOFMEMORY;
}

/******************************************************************
 * @brief  Returns the passed buffer to the corresponding pool
 *
 * @param[in]   buffer      Pointer to the buffer to be returned to pool.
 *                           
 * @retval   BVIEW_STATUS_SUCCESS  Buffer is returned to pool successfully
 * @retval   BVIEW_STATUS_RESOURCE_NOT_AVAILABLE  Attempting to return buffer recursively
 * @retval   BVIEW_STATUS_INVALID_PARAMETER  Invalid input parameter
 * @retval   BVIEW_STATUS_INVALID_MEMORY  The buffer was not allocated by the Pools
 *
 * @note     The buffer should have been originally allocated by  
 *           bstjson_memory_allocate(). BVIEW_STATUS_INVALID_MEMORY is returned otherwise
 *********************************************************************/
BVIEW_STATUS bstjson_memory_free(uint8_t *buffer)
{
    /* Time for some pointer arithmetic */
    /* We have to ensure that the 'buffer' was actually allocated by us. */
    /* we could easily do a linear search through all slices and confirm it */
    /* But that would be lot of searching */
    /* Instead, we take advantage of the facts that */
    /* 1. The buffer pools are contiguous memory */
    /* 2. And that only two predefined sizes are allocated */
    /* This reduces the search time significantly */

    ptr_to_uint_t pointer = (ptr_to_uint_t) (buffer);
    uint64_t temp = 0;
    int index = 0;
    _BUFPOOL_MEMORY_SLICE_t *pSlice;

    _BUFPOOL_LOG(_BUFPOOL_DEBUG_TRACE, "BST Buffer Pool : Returning %" PRI_PTR_TO_UINT_FMT " to pool \n", pointer);

    /* Validate parameters */
    _BUFPOOL_ASSERT(buffer != NULL);

    /* Check the range. */
    _BUFPOOL_ASSERT( (((pointer >= _BUFPOOL_SMALL_POOL_START) && (pointer <= _BUFPOOL_SMALL_POOL_END)) ||
                      ((pointer >= _BUFPOOL_LARGE_POOL_START) && (pointer <= _BUFPOOL_LARGE_POOL_END))) );


    /* check if this pointer is part of the 'small' pool and return to the pool */
    if ( pointer <= _BUFPOOL_SMALL_POOL_END)
    {
        temp = (pointer - _BUFPOOL_SMALL_POOL_START);

        _BUFPOOL_ASSERT_ERROR ( ((temp % BSTJSON_MEMSIZE_RESPONSE) == 0),
                               BVIEW_STATUS_INVALID_MEMORY);

        index = temp / BSTJSON_MEMSIZE_RESPONSE;

        pSlice = &pBufPool->smallSlices[index];

        if (pSlice->buffer == buffer)
        {
            /* Lock the Pools, update shared mem, release the lock */
            _BUFPOOL_MUTEX_TAKE(pBufPool->lock);
            pSlice->inUse = false;
            pSlice->timeTaken = 0;
            _BUFPOOL_MUTEX_RELEASE(pBufPool->lock);

            _BUFPOOL_LOG(_BUFPOOL_DEBUG_TRACE,
                         "BST Buffer Pool : %" PRI_PTR_TO_UINT_FMT " [index %d] returned to RESPONSE pool \n",
                         pointer, index);
            return BVIEW_STATUS_SUCCESS;
        }

        _BUFPOOL_LOG(_BUFPOOL_DEBUG_ERROR,
                     "BST Buffer Pool : %" PRI_PTR_TO_UINT_FMT " doesn't seem to have been allocated from RESPONSE pool \n",
                     pointer);
        return BVIEW_STATUS_INVALID_MEMORY;
    }

    /* Because of the above assert, the buffer is indeed with in large pool range */
    if ( pointer <= _BUFPOOL_LARGE_POOL_END)
    {
        temp = (pointer - _BUFPOOL_LARGE_POOL_START);

        _BUFPOOL_ASSERT_ERROR ( ((temp % BSTJSON_MEMSIZE_REPORT) == 0),
                               BVIEW_STATUS_INVALID_MEMORY);

        index = temp / BSTJSON_MEMSIZE_REPORT;

        pSlice = &pBufPool->largeSlices[index];

        if (pSlice->buffer == buffer)
        {
            /* Lock the Pools, update shared mem, release the lock */
            _BUFPOOL_MUTEX_TAKE(pBufPool->lock);
            pSlice->inUse = false;
            pSlice->timeTaken = 0;
            _BUFPOOL_MUTEX_RELEASE(pBufPool->lock);

            _BUFPOOL_LOG(_BUFPOOL_DEBUG_TRACE,
                         "BST Buffer Pool : %" PRI_PTR_TO_UINT_FMT " [index %d] returned to REPORT pool \n",
                         pointer, index);
            return BVIEW_STATUS_SUCCESS;
        }

        _BUFPOOL_LOG(_BUFPOOL_DEBUG_ERROR,
                     "BST Buffer Pool : %" PRI_PTR_TO_UINT_FMT " doesn't seem to have been allocated from REPORT pool \n",
                     pointer);
        return BVIEW_STATUS_INVALID_MEMORY;
    }

    _BUFPOOL_LOG(_BUFPOOL_DEBUG_ERROR,
                 "BST Buffer Pool : %" PRI_PTR_TO_UINT_FMT " doesn't seem to have been allocated any of the pools \n",
                 pointer);

    return BVIEW_STATUS_INVALID_MEMORY;
}

/*****************************************************************//**
* @brief  Dump Buffer Pool.
*
*
* @retval   none
*
  *********************************************************************/

void bstjson_memory_dump(void)
{
    int index = 0;
    int inUseCount = 0;

    printf (" BST Buffer Pool Statistics : Total Memory %d bytes \n\n", (int)sizeof (bstJsonMemoryPool));

    for (index = 0; index < _BUFPOOL_MAX_RESPONE_SLICES; index++)
    {
        printf (" [%2d] \t %"PRI_PTR_TO_UINT_FMT" \t %d \t %10s %10d\n",
                index,(ptr_to_uint_t) (pBufPool->smallSlices[index].buffer),
                pBufPool->smallSlices[index].size,
                (pBufPool->smallSlices[index].inUse == true) ? "In Use" : "Available",
                (pBufPool->smallSlices[index].inUse == true) ? (int) ( (pBufPool->smallSlices[index].timeTaken) - pBufPool->start) : 0
                );

        if (pBufPool->smallSlices[index].inUse == true)
            inUseCount++;
    }

    printf("\n RESPONSE Pool - Total %3d -- In Use : %3d -- Available %3d \n\n",
           _BUFPOOL_MAX_RESPONE_SLICES, inUseCount, (_BUFPOOL_MAX_RESPONE_SLICES - inUseCount));

    inUseCount = 0;

    for (index = 0; index < _BUFPOOL_MAX_REPORT_SLICES; index++)
    {
        printf (" [%2d] \t %"PRI_PTR_TO_UINT_FMT" \t %d \t %10s %10d\n",
                index, (ptr_to_uint_t) (pBufPool->largeSlices[index].buffer),
                pBufPool->largeSlices[index].size,
                (pBufPool->largeSlices[index].inUse) ? "In Use" : "Available",
                (pBufPool->largeSlices[index].inUse == true) ? (int) ( (pBufPool->largeSlices[index].timeTaken) - pBufPool->start) : 0
                );
    }

    printf("\n REPORT Pool   - Total %3d -- In Use : %3d -- Available %3d \n\n",
           _BUFPOOL_MAX_REPORT_SLICES, inUseCount, (_BUFPOOL_MAX_REPORT_SLICES - inUseCount));
}
