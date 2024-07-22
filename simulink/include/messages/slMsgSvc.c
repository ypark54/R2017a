/* Copyright 2014-2016 The MathWorks, Inc. */
/* 
 * Copyright 2014-2016 The MathWorks, Inc.
 *
 * File: slMsgSvc.c
 *
 * Abstract:
 *    Simulink Coder implementation of Simulink message, message queues
 *    and other message management APIs
 *
 *    Local switches:
 *    - define SLMSG_USE_STD_MEMCPY to allow use of standard memcpy/memset
 *      implementations - otherwise local implementations are used
 *    - define SLMSG_USE_EXCEPTION to allow run-time exceptions
 *    - define SLMSG_ALLOW_SYSTEM_ALLOC to include malloc/free code
 *      note that this also turns on SLMSG_USE_EXCEPTION
 */

#ifdef SL_INTERNAL

#   include <string.h>
#   define SLMSG_MEMCPY(dst,src,nbytes) (memcpy(dst,src,nbytes))
#   define SLMSG_MEMSET(dst,val,nbytes) (memset(dst,val,nbytes))

#   include "simstruct/sl_types_def.h"
#   include "sl_messages/slMsgSvc.h"

    extern void _slmsg_instrument_send(slMessage *msg, slMsgQueueId queueId, const void *obj);
    extern void _slmsg_instrument_drop(slMessage *msg, slMsgQueueId queueId, const void *obj);
    extern void _slmsg_instrument_pop(slMessage *msg, slMsgQueueId queueId, const void *obj);

#else 

    /* Where to get "memcpy" and "memset" implementations from
     * Define SLMSG_USE_STD_MEMCPY to use standard memcpy/memset implementations
     * otherwise local implementations are used
     */
#   ifdef SLMSG_USE_STD_MEMCPY
#       include <string.h>
#       define SLMSG_MEMCPY(dst,src,nbytes) (memcpy(dst,src,nbytes))
#       define SLMSG_MEMSET(dst,val,nbytes) (memset(dst,val,nbytes))
#   else
        void __slmsg_memcpy_impl(void *dst, const void *src, unsigned long nbytes);
        void __slmsg_memset_impl(void *dst, int val, unsigned long nbytes);
#       define SLMSG_MEMCPY(dst,src,nbytes) (__slmsg_memcpy_impl(dst,src,nbytes))
#       define SLMSG_MEMSET(dst,val,nbytes) (__slmsg_memset_impl(dst,val,nbytes))
#   endif

#   ifndef SLMSG_UNIT_TEST
#       include "builtin_typeid_types.h"
#   else
#       include "simstruct/sl_types_def.h"
#   endif

#   include "slMsgSvc.h"

#   if defined(SLMSG_ALLOW_SYSTEM_ALLOC) || defined(SLMSG_USE_EXCEPTION)
#       define SLMSG_USE_EXCEPTION
#   else
#       define __slmsg_assert(e)
#       define __slmsg_RAISE(e)
#       define __slmsg_Mem_Failed_Address NULL
#       define __slmsg_SYSTEM_ALLOC(s) NULL
#       define __slmsg_SYSTEM_FREE(p)
#   endif

    /* Stub out unused functions */
#   define _slmsg_instrument_send(a,b,c)
#   define _slmsg_instrument_drop(a,b,c)
#   define _slmsg_instrument_pop(a,b,c)

#endif

#define __SLMSG_USE_MEMORY_POOLS__
#ifdef __SLMSG_USE_MEMORY_POOLS__
#   define __slmsg_POOLED_ALLOC(pool, nbytes) __slmsg_MemPool_Alloc(pool, nbytes)
#   define __slmsg_POOLED_FREE(pool, ptr)     __slmsg_MemPool_Free (pool, ptr)
#else
#   define __slmsg_POOLED_ALLOC(pool, nbytes) __slmsg_Mem_alloc((nbytes), __FILE__, __LINE__)
#   define __slmsg_POOLED_FREE(pool, ptr)     ((void)(__slmsg_Mem_free((ptr), __FILE__, __LINE__), (ptr) = 0))
#endif

#define SLMSG_CIRCULAR_INDEX(index, capacity) \
    ( ((capacity) == 0) ? (index) : ((index) % (capacity)) )

#ifndef SLMSG_USE_STD_MEMCPY

/* ------------------------------------------------------------------------
 *                         Utility functions
 * --------------------------------------------------------------------- */

/* Internal memcpy implementation */
void __slmsg_memcpy_impl(void *dst, const void *src, unsigned long nbytes)
{
    unsigned long idx;
    int8_T *dst8 = (int8_T *)dst;
    const int8_T *src8 = (const int8_T *)src;
    for (idx = 0; idx < nbytes; ++idx) {
        *dst8++ = *src8++;
    }
}
/* Internal memset implementation */
void __slmsg_memset_impl(void *dst, int val, unsigned long nbytes)
{
    unsigned long idx;
    int8_T *dst8 = (int8_T *)dst;
    for (idx = 0; idx < nbytes; ++idx) {
        *dst8++ = (int8_T) val;
    }
}

#endif /* SLMSG_USE_STD_MEMCPY */

#ifdef SLMSG_USE_EXCEPTION

/* ------------------------------------------------------------------------
 *                       Runtime exception
 * --------------------------------------------------------------------- */

#include "stdio.h"
#include "stdlib.h"

void __slmsg_Except_raise(__slmsg_Except_Type eType, const char *file, int a_line) 
{
    static const char *static_assert_failed = "Assertion failed";
    static const char *static_bad_alloc = "Memory allocation failed";
    const char *reason;

    if (eType == __slmsg_Except_Assertion_Failed) {
        reason = static_assert_failed;
    } else {
        reason = static_bad_alloc;
    }

    fprintf(stderr, "Uncaught exception %s", reason);
    if (file && (a_line > 0)) {
        fprintf(stderr, " raised at %s:%d\n", file, a_line);
    }
    fprintf(stderr, "aborting...\n");
    fflush(stderr);
    abort();
}

void (__slmsg_assert)(int unused)
{
    (void)unused;
}

#endif /* SLMSG_USE_EXCEPTION */

#ifdef SLMSG_ALLOW_SYSTEM_ALLOC

/* ------------------------------------------------------------------------
 *                     Dynamic memory management
 * --------------------------------------------------------------------- */

#include "stdlib.h"
#include "stddef.h"

void *__slmsg_Mem_alloc(long nbytes, const char *file, int a_line)
{
    void *ptr;
    __slmsg_assert(nbytes > 0);
    ptr = malloc(nbytes);
    if (ptr == NULL) {
        __slmsg_Except_raise(__slmsg_Except_Bad_Alloc, file, a_line);
    }
    return ptr;
}

void *__slmsg_Mem_calloc(long count, long nbytes, const char *file, int a_line) 
{
    void *ptr;
    __slmsg_assert(count > 0);
    __slmsg_assert(nbytes > 0);
    ptr = calloc(count, nbytes);
    if (ptr == NULL) {
        __slmsg_Except_raise(__slmsg_Except_Bad_Alloc, file, a_line);
    }
    return ptr;
}

void __slmsg_Mem_free(void *ptr, const char *file, int a_line) 
{
    (void) (a_line);
    (void) (file);
    if (ptr) {
        free(ptr);
    }
}

void *__slmsg_Mem_resize(void *ptr, long nbytes, const char *file, int a_line) 
{
    __slmsg_assert(ptr != NULL);
    __slmsg_assert(nbytes > 0);
    ptr = realloc(ptr, nbytes);
    if (ptr == NULL) {
        __slmsg_Except_raise(__slmsg_Except_Bad_Alloc, file, a_line);
    }

    return ptr;
}

#endif /* SLMSG_ALLOW_SYSTEM_ALLOC */

/* ------------------------------------------------------------------------
 *                     Memory Pooling for Messages
 * --------------------------------------------------------------------- */

#ifdef SLMSG_MEMPOOL_INSTRUMENT
#include <stdio.h>
#endif

/* Add a new fixed-size memory pool to the memory manager
 * Pool will have numUnits number of memory units, each of size unitSize
 * 
 * This will allocate memory from the system for the pool and create two
 * double linked lists for allocated blocks and free blocks
 */
void __slmsg_MemPool_Add(__slmsg_MemPool *pool, 
                         unsigned long numUnits,
                         unsigned long unitSize,
                         void *memBlock)
{
    unsigned long idx;
    __slmsg_MemChunk *chunk = pool->fMemChunk; 
    __slmsg_assert(pool != NULL);   
    chunk->fNumUnits = numUnits;
    chunk->fBlockSize = numUnits * (unitSize + sizeof(__slmsg_PoolListNode)); 
    chunk->fPrevChunk = NULL;    
   
    /* If memory block is provided then use it */
#ifndef SLMSG_ALLOW_SYSTEM_ALLOC
    __slmsg_assert(memBlock != NULL);
    chunk->fMemBlock = memBlock;
#else
    if (memBlock != NULL) {
        chunk->fMemBlock = memBlock;
    } else {
        __slmsg_assert(pool->fCanMalloc);
        chunk->fMemBlock = __slmsg_SYSTEM_ALLOC(chunk->fBlockSize);
        __slmsg_assert(chunk->fMemBlock != NULL);
    }
#endif

    /* Attach chunk to pool */    
    pool->fUnitSize = unitSize;
  
    /* Initialize pool properties */
    pool->fAllocListHead = NULL;
    pool->fFreeListHead = NULL;
    pool->fNextPool = NULL;
    pool->fStatNumAlloc = 0;
    pool->fStatNumFree = 0;

    /* Link all of the memory units and create the free list */
    for (idx = 0; idx < numUnits; ++idx) {
        __slmsg_PoolListNode *pCurUnit = (__slmsg_PoolListNode *)
            ((char *)chunk->fMemBlock + 
             idx * (unitSize + sizeof(__slmsg_PoolListNode)) );
    
        pCurUnit->pPrev = NULL;
        pCurUnit->pNext = pool->fFreeListHead; /* Insert the new unit at head */
    
        if (NULL != pool->fFreeListHead) {
            pool->fFreeListHead->pPrev = pCurUnit;
        }
        pool->fFreeListHead = pCurUnit;
    }
}

/* Allocate memory of specified size from the pool
 * 
 * Get the next available block from the free list and return. If no more
 * blocks are left then currently we allocate from the system. We should
 * ideally expand the available memory and then allocate (grow the pool).
 */
void *__slmsg_MemPool_Alloc(__slmsg_MemPool* pool, unsigned long nbytes)
{
    __slmsg_PoolListNode *pCurUnit;
    __slmsg_assert(pool != NULL);
    
    /* If pool is fully allocated:
     * - first preference is to grow the pool memory, if allowed
     * - if not, then we have to do an system allocation, if allowed
     * - if not, then we raise an exception
     */
    if (NULL == pool->fFreeListHead) {
#ifdef SLMSG_ALLOW_SYSTEM_ALLOC
        if (pool->fCanMalloc) { /* Grow the memory pool */
            unsigned long idx;

            /* Allocate double the number of units this time */
            unsigned long curNumUnits = pool->fMemChunk->fNumUnits;
            unsigned long newNumUnits = 2 * curNumUnits;
            unsigned long newBlockSize = newNumUnits * 
                (pool->fUnitSize + sizeof(__slmsg_PoolListNode));
        
            /* If the new block size after growing will exceed the maximum
             * allowed block size, then do a system allocation instead
             */
            if (newBlockSize > pool->fMaxBlockSize) {
                return __slmsg_SYSTEM_ALLOC(nbytes);
            } else {
                /* Allocate another chunk */
                __slmsg_MemChunk *chunk = (__slmsg_MemChunk *) 
                    __slmsg_SYSTEM_ALLOC(sizeof(__slmsg_MemChunk));
                __slmsg_assert(chunk != NULL);
	
                chunk->fNumUnits = newNumUnits;
                chunk->fBlockSize = newNumUnits * 
                    (pool->fUnitSize + sizeof(__slmsg_PoolListNode)); 
                chunk->fMemBlock = __slmsg_SYSTEM_ALLOC(chunk->fBlockSize);
                __slmsg_assert(chunk->fMemBlock != NULL);

                /* Link up the new chunk with the previous one */
                chunk->fPrevChunk = pool->fMemChunk;
                pool->fMemChunk = chunk;

                /* Link all of the memory units and create the free list */
                for (idx = 0; idx < newNumUnits; ++idx) {
                    __slmsg_PoolListNode *pCUnit = (__slmsg_PoolListNode *)
                        ((char *)chunk->fMemBlock +
                         idx * (pool->fUnitSize + sizeof(__slmsg_PoolListNode)));

                    pCUnit->pPrev = NULL;
                    pCUnit->pNext = pool->fFreeListHead; /* Insert the new unit at head */
	  
                    if (NULL != pool->fFreeListHead) {
                        pool->fFreeListHead->pPrev = pCUnit;
                    }
                    pool->fFreeListHead = pCUnit;
                }	
            }
        } else {
#endif
            (void) nbytes;
            /* Raise a memory allocation exception */
            __slmsg_RAISE(__slmsg_Except_Bad_Alloc);
#ifdef SLMSG_ALLOW_SYSTEM_ALLOC
        }
#endif
    }
  
    /* If we reached here then the free list is not empty */
    pCurUnit = pool->fFreeListHead;
    pool->fFreeListHead = pCurUnit->pNext; /* Next unit from free list */
    if (NULL != pool->fFreeListHead) {
        pool->fFreeListHead->pPrev = NULL;
    }

    pCurUnit->pNext = pool->fAllocListHead;
    if (NULL != pool->fAllocListHead) {
        pool->fAllocListHead->pPrev = pCurUnit; 
    }
    pool->fAllocListHead = pCurUnit;
    pool->fStatNumAlloc++;
    return (void *)( (char *)pCurUnit + sizeof(__slmsg_PoolListNode) );
}

/* Free previously allocated memory and return to the pool
 * 
 * Determine if this pointer was allocated from any of the chunks in
 * this pool. If yes, then return it to the pool and reset the input
 * pointer so that the caller knows the memory was released.
 * 
 * If the memory is not from this pool, return silently.
 */
void __slmsg_private_MemPool_Free(__slmsg_MemPool *pool, void **ptrIn)
{
    int8_T memFromPool = 0;
    void *ptr = *ptrIn;
    __slmsg_MemChunk *chunk = pool->fMemChunk;

    /* Determine whether this memory was allocated from the pool or
     * from the system - check each memory chunk
     */
    do {
        memFromPool = (
            (chunk->fMemBlock < ptr) && 
            (ptr < (void *)((char *)chunk->fMemBlock + chunk->fBlockSize)));

        chunk = chunk->fPrevChunk;

    } while (!memFromPool && (chunk != NULL));

    /* Return memory to the pool if it was allocated from the pool */
    if (memFromPool) {

        __slmsg_PoolListNode *pCurUnit = (__slmsg_PoolListNode *)
            ((char *) ptr - sizeof(__slmsg_PoolListNode));

        pool->fAllocListHead = pCurUnit->pNext;
        if (NULL != pool->fAllocListHead) {
            pool->fAllocListHead->pPrev = NULL;
        }

        pCurUnit->pNext = pool->fFreeListHead;
        if (NULL != pool->fFreeListHead) {
            pool->fFreeListHead->pPrev = pCurUnit;
        }

        pool->fFreeListHead = pCurUnit;
        pool->fStatNumFree++;

        /* Reset the pointer since we have "freed" this memory */
        *ptrIn = NULL;
    }
}

/* Free previously allocated memory and return to the pool
 * 
 * Starting from the first pool, ask each pool to free this memory if
 * it belongs to the pool. If no pool frees it, then it must have been
 * a system allocation and it can be freed by a system call.
 */
void __slmsg_MemPool_Free(__slmsg_MemPoolMgr *mgr, void *ptr)
{
    int i;

    /* Ask each pool to free the memory 
     * The pool that frees it will set the pointer to NULL
     */
    for (i = 0; (i < mgr->fNumPools) && ptr != NULL; ++i) {
        __slmsg_private_MemPool_Free( &(mgr->fPools[i]), &ptr);
    }

#ifdef SLMSG_ALLOW_SYSTEM_ALLOC
    /* If still not freed by any pool then do a system free 
       because the memory is not associated with any memory 
       pool and memory was system allocated */
    if (ptr != NULL) {
        __slmsg_SYSTEM_FREE(ptr);
    }
#endif
}

/* Recursively destroy all memory chunks starting from input chunk
 */
void __slmsg_private_MemChunk_Destroy(__slmsg_MemChunk **chunk,
                                      boolean_T mallocStatus)
{
    if ((*chunk)->fPrevChunk != NULL) {
        __slmsg_private_MemChunk_Destroy(&(*chunk)->fPrevChunk, mallocStatus);
    }

#ifdef SLMSG_ALLOW_SYSTEM_ALLOC
    if (mallocStatus) { 
        __slmsg_SYSTEM_FREE((*chunk)->fMemBlock);
        __slmsg_SYSTEM_FREE(*chunk);
    }
#endif
    *chunk = NULL;
}

/* Recursively destroy all memory pools starting from the input pool
 * Destroy all memory chunks owned by the input pool
 */
void __slmsg_private_MemPool_Destroy(__slmsg_MemPool * pool)
{
#ifdef SLMSG_MEMPOOL_INSTRUMENT
    /* Report statistics of this pool */
    printf("\nMemory Pool: Fixed block size = %lu\n", (*pool)->fUnitSize);
    printf("-- Number of blocks allocated = %d\n", (*pool)->fStatNumAlloc);
    printf("-- Number of blocks freed     = %d\n", (*pool)->fStatNumFree);
    printf("-- Number of blocks cleaned   = %d\n", (*pool)->fStatNumAlloc - 
           (*pool)->fStatNumFree);
#endif
    /* Destroy all memory chunks owned by this pool */
    __slmsg_private_MemChunk_Destroy(&(pool->fMemChunk), pool->fCanMalloc);
}

/* Destroy the memory pool manager
 * 
 * This will destroy each memory pool, and hence, each memory chunk that
 * is part of the pool before destroying the memory manager.
 */
void __slmsg_MemPool_Destroy(__slmsg_MemPoolMgr *mgr)
{
    int i;
    for (i = 0; i< mgr->fNumPools; ++i) {
        /* destroy the pool */
        __slmsg_private_MemPool_Destroy(&(mgr->fPools[i]));
    }    
}

/* ------------------------------------------------------------------------
 *                        Internal Message APIs
 * --------------------------------------------------------------------- */

/* Destroy a message object */
void _slMsgDestroy(slMsgManager *msgMgr, slMessage *msg)
{
    __slmsg_assert(msg != NULL);

#ifndef SLMSG_PRODUCTION_CODE
    /* Destroy event data that may exist on the message */
    if (msg->fAppDeleter != NULL) {
        (*(msg->fAppDeleter))(msg);
    }
#endif

    __slmsg_POOLED_FREE(&msgMgr->fPoolMgr, msg->fData);
    __slmsg_POOLED_FREE(&msgMgr->fPoolMgr, msg);
}

/* Get the type-casted priority value from a message's data */
real_T _slMsgGetMsgPriorityValWithCast(slMessage *msg, slMsgQueue *q)
{
    real_T priorityVal = 0;
    int_T dataType = q->fDataType;
    const char *dataPtr = (const char *) msg->fData;

    __slmsg_assert(dataType != SLMSG_UNSPECIFIED);

    switch (q->fType) {
    case SLMSG_PRIORITY_QUEUE_ASCENDING:
    case SLMSG_PRIORITY_QUEUE_DESCENDING:
        /* Offset message data by the priority offset
        * Used for structured data where the payload is a field */
        dataPtr += q->fPriorityDataOffset;

        switch (dataType) {
        case SS_DOUBLE:
        {
            const real_T *valT = (const real_T *)dataPtr;
            priorityVal = *valT;
        }
            break;
        case SS_SINGLE:
        {
            const real32_T *valT = (const real32_T *)dataPtr;
            priorityVal = (real_T)*valT;
        }
            break;
        case SS_INT8:
        {
            const int8_T *valT = (const int8_T *)dataPtr;
            priorityVal = (real_T)*valT;
        }
            break;
        case SS_UINT8:
        {
            const uint8_T *valT = (const uint8_T *)dataPtr;
            priorityVal = (real_T)*valT;
        }
            break;
        case SS_INT16:
        {
            const int16_T *valT = (const int16_T *)dataPtr;
            priorityVal = (real_T)*valT;
        }
            break;
        case SS_UINT16:
        {
            const uint16_T *valT = (const uint16_T *)dataPtr;
            priorityVal = (real_T)*valT;
        }
            break;
        case SS_INT32:
        {
            const int32_T *valT = (const int32_T *)dataPtr;
            priorityVal = (real_T)*valT;
        }
            break;
        case SS_UINT32:
        {
            const uint32_T *valT = (const uint32_T *)dataPtr;
            priorityVal = (real_T)*valT;
        }
            break;
        case SS_BOOLEAN:
        {
            const boolean_T *valT = (const boolean_T *)dataPtr;
            priorityVal = (real_T)*valT;
        }
            break;
        default:
            __slmsg_assert(0);
        }
        break;
#ifndef SLMSG_PRODUCTION_CODE        
    case SLMSG_SYSPRIORITY_QUEUE_ASCENDING:
    case SLMSG_SYSPRIORITY_QUEUE_DESCENDING:
        /* if queue is sorted by entity priority, 
        * return entity priority */
        priorityVal = (real_T) msg->fPriority;
        break;
#endif        
    default:
        /* Must be a priority queue */
        __slmsg_assert(0); 
    }
    return priorityVal;
}

/* ------------------------------------------------------------------------
 *  Private Methods of slMsgManager
 *
 *  All methods require an slMsgManager instance.
 *  These methods are called from simulation code as well as serve as the
 *  implementations for the functions called from generated code.
 * --------------------------------------------------------------------- */

/* Create and initialize the message runtime services */
void _slMsgSvcInitMsgManager(slMsgManager *msgMgr)
{
    msgMgr->fQueues = NULL;
    msgMgr->fNumQueues = 0;
    msgMgr->_scratchQueue = SLMSG_UNSPECIFIED;
    msgMgr->fUseGlobalMsgIds = 0;
    msgMgr->fNextMsgId = 1;
}

/* Create and initialize the message runtime services */
void _slMsgSvcInitPool(slMsgMemPool* memPool,
                       int_T numUnits,
                       slMsgDataSize dataSize,
                       void *memBuffer,
                       boolean_T mallocAllowed)
{
#ifdef SLMSG_ALLOW_SYSTEM_ALLOC
    if (mallocAllowed) {
        static int_T max_num_units = 2;        
        if (numUnits < max_num_units) {
            numUnits = max_num_units;
        }
        memPool->fMemChunk = (__slmsg_MemChunk*)
            __slmsg_SYSTEM_ALLOC(sizeof(__slmsg_MemChunk));
    } else {
#endif
        memPool->fMemChunk = &memPool->fNonMallocMemChunk;
#ifdef SLMSG_ALLOW_SYSTEM_ALLOC
    }
#endif

    memPool->fCanMalloc = mallocAllowed;
    memPool->fMaxBlockSize = 2097152;
      
    /* allocate the memory pool */
    __slmsg_MemPool_Add(memPool, numUnits,
        (unsigned long) dataSize, memBuffer);
}

/* Set the number of memory pools */
void _slMsgSvcSetNumMemPools(slMsgManager *msgMgr, int_T numMemPools)
{
    msgMgr->fNumPools = numMemPools;
    msgMgr->fPoolMgr.fNumPools = numMemPools;
}

/* Create a message queue with specified properties */
void _slMsgCreateMsgQueue(slMsgManager *msgMgr,
                          slMsgQueueId id, 
                          slMsgQueueType queueType, 
                          int_T capacity,
                          slMsgQueueDropPolicy dropPolicy, 
                          slMsgDataSize dataSize,
                          int_T dataType,
                          slMsgDataSize priorityDataOffset,
                          slMsgMemPoolId messageMemPoolId,
                          slMsgMemPoolId payloadMemPoolId)
{
    slMsgQueue* q = &(msgMgr->fQueues[id]);

    __slmsg_assert(id != SLMSG_UNSPECIFIED);

    q->fId = id;

    /* One extra for dropping */
    q->fCapacity = (dropPolicy == SLMSG_DROP_NONE) ? capacity : capacity + 1;
    q->fLength = 0;
    q->fType = queueType;
    q->fDropPolicy = dropPolicy;
    q->fDataSize = dataSize;     
    q->fDataType = dataType;
    q->readerMessageMemPoolId = messageMemPoolId;
    q->readerPayloadMemPoolId = payloadMemPoolId;
    q->writerMessageMemPoolId = messageMemPoolId;
    q->writerPayloadMemPoolId = payloadMemPoolId;
    q->fPriorityDataOffset = priorityDataOffset;
    q->fHead = NULL;
    q->fTail = NULL;
    q->_nextMsgId = 0;
    
#ifndef SLMSG_PRODUCTION_CODE
    q->_fNumDropped = 0;
    q->_fComputedNecessaryCapacity = 0;
    q->_fCurrentNecessaryCapacity = 0;
    q->_fInstrumentDropObj = NULL;
    q->_fInstrumentSendObj = NULL;
    q->_fInstrumentPopObj = NULL;
#endif

#ifdef SLMSG_INCLUDE_TASK_TRANSITION_QUEUE
    /* For SRSW lock-free case*/
    q->fSRSWFIFOQueue.fCircularCapacity = 0;
    q->fSRSWFIFOQueue.fCircularChunkSize = 0;
    q->fSRSWFIFOQueue.fCircularHead = 0;
    q->fSRSWFIFOQueue.fCircularTail = 0; 
    q->fSRSWFIFOQueue.fCircularArray = NULL;
#endif
}


/* Create a SRSW lock-free message queue with specified properties */
void _slMsgCreateSRSWMsgQueue(slMsgManager *msgMgr,
                              uint8_T *circularArray,
                              slMsgQueueId id,
                              slMsgQueueType queueType,
                              int_T capacity,
                              slMsgQueueDropPolicy dropPolicy,
                              slMsgDataSize dataSize,
                              int_T dataType,
                              slMsgDataSize priorityDataOffset,
                              slMsgMemPoolId readerMessageMemPoolId,
                              slMsgMemPoolId writerMessageMemPoolId,
                              slMsgMemPoolId readerPayloadMemPoolId,
                              slMsgMemPoolId writerPayloadMemPoolId)
{
    slMsgQueue* q = &(msgMgr->fQueues[id]);
	uint8_T arrayCapacity = (uint8_T)capacity;

    __slmsg_assert(capacity <= MAX_uint8_T);   /* Check max capacity: 255 */
    __slmsg_assert(sizeof(uint8_T) == 1);
    __slmsg_assert(id != SLMSG_UNSPECIFIED);

    q->fId = id;
    q->fCapacity = capacity + 1; /* One extra in the circular array */
    q->fLength = 0;
    q->fType = queueType;
    q->fDropPolicy = dropPolicy;
    q->fDataSize = dataSize;  
    q->fDataType = dataType;
    q->fPriorityDataOffset = priorityDataOffset;
    q->fHead = NULL;
    q->fTail = NULL;
    q->readerMessageMemPoolId = readerMessageMemPoolId;
    q->writerMessageMemPoolId = writerMessageMemPoolId;
    q->readerPayloadMemPoolId = readerPayloadMemPoolId;
    q->writerPayloadMemPoolId = writerPayloadMemPoolId;

#ifndef SLMSG_PRODUCTION_CODE
    q->_fNumDropped = 0;
    q->_fComputedNecessaryCapacity = 0;
    q->_fCurrentNecessaryCapacity = 0;
    q->_fInstrumentDropObj = NULL;
    q->_fInstrumentSendObj = NULL;
    q->_fInstrumentPopObj = NULL;
#endif       

#ifdef SLMSG_INCLUDE_TASK_TRANSITION_QUEUE
    /* to perform mod -- can be 0 which represents 256*/
    q->fSRSWFIFOQueue.fCircularCapacity = arrayCapacity + 1;

    /* all necessary data in one chunk */
    q->fSRSWFIFOQueue.fCircularChunkSize = dataSize + sizeof(slMsgId);
    q->fSRSWFIFOQueue.fCircularHead = 0;
    q->fSRSWFIFOQueue.fCircularTail = 0;  /* initialization */
    q->fSRSWFIFOQueue.fCircularArray = circularArray;
#else
    (void)circularArray;
    (void)arrayCapacity;
#endif
}

/* Set the number of message queues that will be used */
void _slMsgSvcSetNumMsgQueues(slMsgManager *msgMgr, int_T numQueues)
{
     msgMgr->fNumQueues = numQueues;
}

/* Remove a message from the queue that it is present in */
void _slMsgRemoveFromQueue(slMsgManager *msgMgr, slMessage *msg, slMsgQueueId qId)
{
    slMsgQueue *q;
    int numMsg, isDropping;
    slMessage *prev, *next;

    if (qId == SLMSG_UNSPECIFIED) {
        return;
    }

    q = &(msgMgr->fQueues[qId]);
    numMsg = q->fLength;
    isDropping = (numMsg == q->fCapacity);

    prev = msg->fPrev;
    next = msg->fNext;

    if (prev != NULL) {
        prev->fNext = next;
    }

    if (next != NULL) {
        next->fPrev = prev;
    }

    if (msg == q->fHead) {
        q->fHead = next;
    }

    if (msg == q->fTail) {
        q->fTail = prev;
    }

    msg->fNext = NULL;
    msg->fPrev = NULL;

#ifndef SLMSG_PRODUCTION_CODE
    if (!isDropping) {
        q->_fCurrentNecessaryCapacity--;
    }
    msg->fQueueId = SLMSG_UNSPECIFIED;
#else
    (void)isDropping;
#endif
    
    q->fLength--;
}

/* Add a message to the specified queue */
slMessage* _slMsgAddToQueue(slMsgManager *msgMgr, slMessage *msg, slMsgQueueId queueId)
{
    slMsgQueue *q = &(msgMgr->fQueues[queueId]);
    slMsgQueueType qType = q->fType;
    int numInQ;

    /* Empty queue */
    if (q->fTail == NULL) {
        q->fHead = msg;
        q->fTail = msg;
        
    } else {
    
        switch (qType) {
          case SLMSG_FIFO_QUEUE:
            {
                slMessage *oldtail = q->fTail;
                oldtail->fNext = msg;
                msg->fPrev = oldtail;
                q->fTail = msg;
                break;
            }
          case SLMSG_LIFO_QUEUE:
            {
                slMessage *oldhead = q->fHead;
                oldhead->fPrev = msg;
                msg->fNext = oldhead;
                q->fHead = msg;
                break;
            }
          case SLMSG_PRIORITY_QUEUE_ASCENDING:
          case SLMSG_PRIORITY_QUEUE_DESCENDING:
          case SLMSG_SYSPRIORITY_QUEUE_ASCENDING:
          case SLMSG_SYSPRIORITY_QUEUE_DESCENDING:
            {
                real_T newVal;
                int newMsgAdded = 0;
                slMessage *thisMsg = q->fHead;

                /* Iterate over queue messages and insert by priority */
                newVal = _slMsgGetMsgPriorityValWithCast(msg, q);

                while (thisMsg != NULL) {
                    const real_T thisVal = _slMsgGetMsgPriorityValWithCast(
                        thisMsg, q);
                    
                    if (((qType == SLMSG_PRIORITY_QUEUE_DESCENDING ||
                          qType == SLMSG_SYSPRIORITY_QUEUE_DESCENDING) &&
                         (thisVal < newVal )) || 
                        ((qType == SLMSG_PRIORITY_QUEUE_ASCENDING ||
                          qType == SLMSG_SYSPRIORITY_QUEUE_ASCENDING) &&
                         (thisVal > newVal))) {

                        if (thisMsg->fPrev == NULL) {
                            thisMsg->fPrev = msg;
                            msg->fNext = thisMsg;
                            if (thisMsg == q->fHead) {
                                q->fHead = msg;
                            }
                            newMsgAdded = 1;
                        } else {
                            slMessage *prev = thisMsg->fPrev;
                            prev->fNext = msg;
                            msg->fPrev = prev;
                            msg->fNext = thisMsg;
                            thisMsg->fPrev = msg;
                            newMsgAdded = 1;
                        }
                        break;
                    }                    
                    thisMsg = thisMsg->fNext;
                }

                /* If we haven't added it yet then add to the end */
                if (newMsgAdded == 0) {
                    slMessage *oldtail = q->fTail;
                    oldtail->fNext = msg;
                    msg->fPrev = oldtail;
                    q->fTail = msg;
                }
                break;
            }
          default:
            __slmsg_assert(0);
            break;
        }
    }

#ifndef SLMSG_PRODUCTION_CODE
    msg->fQueueId = queueId;

    q->_fCurrentNecessaryCapacity++;
    if (q->_fComputedNecessaryCapacity < q->_fCurrentNecessaryCapacity) {
        q->_fComputedNecessaryCapacity = q->_fCurrentNecessaryCapacity;
    }
#endif

    q->fLength++;

    /* Drop if exceeded capacity */
    numInQ = q->fLength;

    if ((q->fDropPolicy != SLMSG_DROP_NONE) && (numInQ == q->fCapacity)) {
        slMessage *msgToDrop = NULL;

        switch (q->fDropPolicy) {
          case SLMSG_DROP_HEAD_OF_QUEUE:
            msgToDrop = q->fHead;
            break;
          case SLMSG_DROP_TAIL_OF_QUEUE:
            msgToDrop = q->fTail;
            break;
          default:
            __slmsg_assert(0);
            break;
        }

        __slmsg_assert(msgToDrop != NULL);
        
#ifndef SLMSG_PRODUCTION_CODE
        ++q->_fNumDropped;
        if (q->_fInstrumentDropObj != NULL) {
            _slmsg_instrument_drop(msgToDrop, queueId, q->_fInstrumentDropObj);
        }
        msgToDrop->fQueueId = SLMSG_UNSPECIFIED;
#endif

        _slMsgRemoveFromQueue(msgMgr, msgToDrop, queueId);
        return msgToDrop;
    } else {
        return NULL;
    }
}

/* Destroy the specified message */
void _slMsgSvcDestroyMsg(slMsgManager *msgMgr, slMessage *msg)
{
    __slmsg_assert(msg != NULL);

#ifndef SLMSG_PRODUCTION_CODE
    /* Assert that message has been popped or dropped */
    __slmsg_assert(msg->fQueueId == SLMSG_UNSPECIFIED);
#endif

    _slMsgDestroy(msgMgr, msg);
}

/* Create a new message with a specified ID */
slMessage *_slMsgSvcCreateMsgWithId(slMsgManager *msgMgr, 
                                    const void* data, 
                                    slMsgDataSize dataSize, 
                                    slMsgQueueId queueId, 
                                    slMsgId msgId)
{
    slMsgQueue* queue = &(msgMgr->fQueues[queueId]);
    
    /* Which memory pools to use to create this message */
    slMsgMemPool* msgWriterPool = &(msgMgr->fPoolMgr.fPools[queue->writerMessageMemPoolId]);
    slMsgMemPool* msgWriterPayloadPool = &(msgMgr->fPoolMgr.fPools[queue->writerPayloadMemPoolId]);

    slMessage *msg = (slMessage *)__slmsg_POOLED_ALLOC(msgWriterPool, sizeof(slMessage));

    msg->fData = __slmsg_POOLED_ALLOC(msgWriterPayloadPool, (long)dataSize);
    if (data == NULL) {
        SLMSG_MEMSET(msg->fData, 0, dataSize);
    } else {
        SLMSG_MEMCPY(msg->fData, data, dataSize);
    }

    msg->fNext = NULL;
    msg->fPrev = NULL;

#ifndef SLMSG_PRODUCTION_CODE
    msg->fId = msgId;
    msg->fPriority = DEF_EVT_PRIORITY;
    msg->fDataSize = dataSize;
    msg->fQueueId = SLMSG_UNSPECIFIED;
    msg->fAppData = NULL;
    msg->fAppDeleter = NULL;
#else
    (void)msgId;
#endif

    return msg;
}

/* Create a new message */
slMessage *_slMsgSvcCreateMsg(slMsgManager *msgMgr, 
                              const void* data, 
                              slMsgDataSize dataSize, 
                              slMsgQueueId queueId)
{
    slMsgId msgId;
    if (msgMgr->fUseGlobalMsgIds) {
        msgId = msgMgr->fNextMsgId++;
    }
    else {
        msgId = (++(msgMgr->fQueues[queueId]._nextMsgId)) + (queueId << 16);
    }

    return _slMsgSvcCreateMsgWithId(msgMgr, data, dataSize, queueId, msgId);
}

/* Send a message to the specified queue */
slMessage *_slMsgSvcSendMsg(slMsgManager *msgMgr, slMessage *msg, slMsgQueueId queueId)
{
    slMsgQueue *q;
    __slmsg_assert(msg != NULL);

    if (queueId == SLMSG_UNSPECIFIED) {
#ifndef SLMSG_PRODUCTION_CODE
        /* In forwarding: Unconnected sender block - destroy the message */
        _slmsg_instrument_drop(msg, SLMSG_UNSPECIFIED, NULL);
#endif
        return msg;
    }

    q = &(msgMgr->fQueues[queueId]);

#ifndef SLMSG_PRODUCTION_CODE
    /* Message has been popped */
    __slmsg_assert(msg->fQueueId == SLMSG_UNSPECIFIED);
    /* Data size match */
    __slmsg_assert(msg->fDataSize == q->fDataSize);

    if (q->_fInstrumentSendObj != NULL) {
        _slmsg_instrument_send(msg, queueId, q->_fInstrumentSendObj);
        q->_fInstrumentSendObj = NULL;
    }
#else
    (void)q;
#endif

    return _slMsgAddToQueue(msgMgr, msg, queueId);
}

/* Send a message to the specified SRSW FIFO queue */
slMessage *_slMsgSvcSRSWSendMsg(slMsgManager *msgMgr, slMessage *msg, slMsgQueueId queueId)
{
    slMsgQueue *q;
    __slmsg_assert(msg != NULL);

    if (queueId == SLMSG_UNSPECIFIED) {
#ifndef SLMSG_PRODUCTION_CODE    
        /* In forwarding: Unconnected sender block - destroy the message */
        _slmsg_instrument_drop(msg, SLMSG_UNSPECIFIED, NULL);
#endif
        return msg;
    }

    q = &(msgMgr->fQueues[queueId]);
    
#ifndef SLMSG_PRODUCTION_CODE
    __slmsg_assert(msg->fDataSize == q->fDataSize);
    __slmsg_assert(msg->fQueueId == SLMSG_UNSPECIFIED); 
    if (q->_fInstrumentSendObj != NULL) {
        _slmsg_instrument_send(msg, queueId, q->_fInstrumentSendObj);
        q->_fInstrumentSendObj = NULL;
    }
#endif

#ifdef SLMSG_INCLUDE_TASK_TRANSITION_QUEUE
    if (SLMSG_CIRCULAR_INDEX(
            q->fSRSWFIFOQueue.fCircularTail + 1, 
            q->fSRSWFIFOQueue.fCircularCapacity) != 
        q->fSRSWFIFOQueue.fCircularHead) {
        SLMSG_MEMCPY((q->fSRSWFIFOQueue.fCircularArray + 
                (q->fSRSWFIFOQueue.fCircularTail * 
                 q->fSRSWFIFOQueue.fCircularChunkSize)), 
               msg->fData, q->fDataSize);

#ifndef SLMSG_PRODUCTION_CODE
        SLMSG_MEMCPY((q->fSRSWFIFOQueue.fCircularArray + 
                (q->fSRSWFIFOQueue.fCircularTail * 
                 q->fSRSWFIFOQueue.fCircularChunkSize) + q->fDataSize), 
               &(msg->fId), sizeof(slMsgId));
#endif

        q->fSRSWFIFOQueue.fCircularTail = 
            SLMSG_CIRCULAR_INDEX(q->fSRSWFIFOQueue.fCircularTail + 1, 
                                 q->fSRSWFIFOQueue.fCircularCapacity);

        _slMsgDestroy(msgMgr, msg);
        return NULL;
    } 
#else
    (void)q;
#endif
    
    return msg;
}

/* Return number of messages present in specified queue */
int _slMsgSvcGetNumMsgsInQueue(slMsgManager *msgMgr, slMsgQueueId queueId)
{
    __slmsg_assert(queueId != SLMSG_UNSPECIFIED);
    return msgMgr->fQueues[queueId].fLength;
}

/* Peek the message at the specified index of the specified queue */
slMessage *_slMsgSvcPeekMsgFromQueueAtIndex(slMsgManager *msgMgr, slMsgQueueId queueId, int msgIndex)
{
    slMessage *msg = NULL;
    slMsgQueue *q;
    int msgCounter;

    __slmsg_assert(queueId != SLMSG_UNSPECIFIED);
    q = &(msgMgr->fQueues[queueId]);
    
    if (q->fLength > msgIndex) {        
        msg = q->fHead;
        msgCounter = 0;

        while ((msgCounter != msgIndex) && (msg != NULL)) {
            msg = msg->fNext;
            msgCounter++;
        }
        __slmsg_assert(msg != NULL);
    }

    return msg;
}

/* Peek the message at the head of the specified queue */
slMessage *_slMsgSvcPeekMsgFromQueue(slMsgManager *msgMgr, slMsgQueueId queueId)
{
    return _slMsgSvcPeekMsgFromQueueAtIndex(msgMgr, queueId, 0);
}

/* Pop the message at the top of the specified queue */
slMessage *_slMsgSvcPopMsgFromQueue(slMsgManager *msgMgr, slMsgQueueId queueId)
{
    slMessage *msg = NULL;
    slMsgQueue *q;

    __slmsg_assert(queueId != SLMSG_UNSPECIFIED);
    q = &(msgMgr->fQueues[queueId]);

    if (q->fLength > 0) {
        msg = q->fHead;
        _slMsgRemoveFromQueue(msgMgr, msg, queueId);

#ifndef SLMSG_PRODUCTION_CODE
        if (q->_fInstrumentPopObj != NULL) {
            _slmsg_instrument_pop(msg, queueId, q->_fInstrumentPopObj);
        }
    }
    q->_fInstrumentPopObj = NULL;
#else
    }
#endif
    return msg;
}


/* Pop or peek the message at the top of the specified SRSW FIFO queue */
slMessage *_slMsgSvcSRSWReadMsgFromQueue(slMsgManager *msgMgr, slMsgQueueId queueId, boolean_T pop)
{
    slMessage *msg = NULL;
    slMsgQueue *q = NULL;

    __slmsg_assert(queueId != SLMSG_UNSPECIFIED);
    q = &(msgMgr->fQueues[queueId]);

#ifdef SLMSG_INCLUDE_TASK_TRANSITION_QUEUE
    if (q->fSRSWFIFOQueue.fCircularHead != q->fSRSWFIFOQueue.fCircularTail){
        msg = _slMsgSvcCreateMsg(msgMgr, 
                                 (q->fSRSWFIFOQueue.fCircularArray + 
                                  (q->fSRSWFIFOQueue.fCircularHead * 
                                   q->fSRSWFIFOQueue.fCircularChunkSize)), 
                                 q->fDataSize, queueId);

#ifndef SLMSG_PRODUCTION_CODE
        SLMSG_MEMCPY(&(msg->fId), 
               (q->fSRSWFIFOQueue.fCircularArray + 
                (q->fSRSWFIFOQueue.fCircularHead * 
                 q->fSRSWFIFOQueue.fCircularChunkSize) + 
                q->fDataSize), sizeof(slMsgId));
#endif

        if (pop) {
            q->fSRSWFIFOQueue.fCircularHead = 
                SLMSG_CIRCULAR_INDEX(q->fSRSWFIFOQueue.fCircularHead + 1, 
                                     q->fSRSWFIFOQueue.fCircularCapacity);
        }
    }
#else
    (void)q;
    (void)pop;
#endif

    return msg;
}

/* Return the data held by the specified message */
void *_slMsgSvcGetMsgData(slMessage *msg)
{
    __slmsg_assert(msg != NULL);
    return msg->fData;
}

/* ------------------------------------------------------------------------
 *                     Public Implementations
 *                   called from generated code
 * --------------------------------------------------------------------- */

/* Create and initialize the message runtime services */
void slMsgSvcInitMsgManager(void* mgr)
{
    _slMsgSvcInitMsgManager((slMsgManager *)mgr);
}

/* Free runtime memory */
void _slMsgSvcFinalizeMsgManager(slMsgManager *msgMgr)
{
    /* Destroy messages */
    slMessage *msg = NULL;
    slMsgDataSize qIdx = 0;
    slMsgQueue *q;
    slMsgQueueId queueId;

    if (msgMgr == NULL)
        return;

    for (; qIdx < msgMgr->fNumQueues; ++qIdx) {
        q = &(msgMgr->fQueues[qIdx]);
        if (q != NULL) {
            queueId = q->fId;
            if (queueId != SLMSG_UNSPECIFIED) {

#ifndef SLMSG_PRODUCTION_CODE
                q->_fInstrumentDropObj = NULL;
                q->_fInstrumentPopObj = NULL;
                q->_fInstrumentSendObj = NULL;
#endif
                while (_slMsgSvcGetNumMsgsInQueue(msgMgr, queueId) > 0) {
                    msg = _slMsgSvcPopMsgFromQueue(msgMgr, queueId);
                    _slMsgDestroy(msgMgr, msg);
                }
            }
        }
    }
}

/* Terminate message runtime services, delete all messages and queues */
void _slMsgSvcTerminateMsgManager(slMsgManager *msgMgr)
{
    /* Destroy memory pool */
    __slmsg_MemPool_Destroy(&msgMgr->fPoolMgr);
}

/* Finalize message runtime services, delete all messages and queues */
void slMsgSvcFinalizeMsgManager(void *msgMgr)
{
    _slMsgSvcFinalizeMsgManager((slMsgManager *)msgMgr);
}

/* Terminate message services, free memory pool */
void slMsgSvcTerminateMsgManager(void *msgMgr)
{
    _slMsgSvcTerminateMsgManager((slMsgManager *)msgMgr);
}

/* Initialize the memory pool */
void slMsgSvcInitMemPool(void *msgMgr,
    slMsgMemPoolId poolId,
    int_T numUnits, /* number of units */
    slMsgDataSize dataSize,
    void *memBuffer,
    boolean_T mallocAllowed)
{
    slMsgManager *mgr = (slMsgManager *)msgMgr;
    slMsgMemPool* memPool = &(mgr->fPoolMgr.fPools[poolId]);
    __slmsg_assert(memPool != NULL);
    _slMsgSvcInitPool(memPool,
        numUnits,
        dataSize,
        memBuffer,
        mallocAllowed);
}

/* Set the number of message queues that will be used */
void slMsgSvcSetNumMsgQueues(void *msgMgr, int_T num, void *queueArray)
{
    slMsgManager *msgMgrT = (slMsgManager *)msgMgr;
    msgMgrT->fQueues = (slMsgQueue *)queueArray;
    _slMsgSvcSetNumMsgQueues(msgMgrT, num);

}

void slMsgSvcSetNumMemPools(void *msgMgr, int_T num, void *poolArray)
{
    slMsgManager *msgMgrT = (slMsgManager *)msgMgr;
    msgMgrT->fPoolMgr.fPools = (slMsgMemPool *)poolArray;
    _slMsgSvcSetNumMemPools(msgMgrT, num);
}

/* Return number of messages present in specified queue */
int slMsgSvcGetNumMsgsInQueue(void *msgMgr, slMsgQueueId queueId)
{
    return _slMsgSvcGetNumMsgsInQueue((slMsgManager *)msgMgr, queueId);
}

/* Destroy the specified message */
void slMsgSvcDestroyMsg(void *msgMgr, void *msgptr)
{
    _slMsgSvcDestroyMsg((slMsgManager *)msgMgr, (slMessage*)msgptr);
}

/* Create a FIFO message queue with specified properties */
void slMsgSvcCreateFIFOMsgQueue(void *msgMgr, 
                                int_T id,
                                int_T capacity,
                                slMsgDataSize dataSize, 
                                int_T dropPolicy,
                                slMsgMemPoolId messageMemPoolId,
                                slMsgMemPoolId payloadMemPoolId)
{
    slMsgManager *msgMgrT = (slMsgManager *)msgMgr;
    _slMsgCreateMsgQueue(msgMgrT,
                         id, 
                         SLMSG_FIFO_QUEUE, 
                         capacity, 
                         (slMsgQueueDropPolicy) dropPolicy, 
                         dataSize, 
                         SLMSG_UNSPECIFIED, 
                         0,
                         messageMemPoolId,
                         payloadMemPoolId);
}

#ifdef SLMSG_INCLUDE_TASK_TRANSITION_QUEUE
/* Create a SRSW lock-free FIFO message queue with specified properties */
void slMsgSvcCreateSRSWFIFOMsgQueue(void *msgMgr, 
                                    int_T id, 
                                    int_T capacity, 
                                    slMsgDataSize dataSize, 
                                    void* sharedArray,
                                    slMsgMemPoolId readerMessageMemPoolId,
                                    slMsgMemPoolId writerMessageMemPoolId,
                                    slMsgMemPoolId readerPayloadMemPoolId,
                                    slMsgMemPoolId writerPayloadMemPoolId)
{
    slMsgManager *msgMgrT = (slMsgManager *)msgMgr;
    slMsgQueueDropPolicy dropPolicy;    
    dropPolicy = SLMSG_DROP_TAIL_OF_QUEUE;
    __slmsg_assert(sharedArray != NULL);
    __slmsg_assert(capacity > 0);
    _slMsgCreateSRSWMsgQueue(
        msgMgrT,
        (uint8_T*)sharedArray,
        id,
        SLMSG_SRSW_LOCK_FREE_FIFO_QUEUE,
        capacity,
        dropPolicy,
        dataSize,
        SLMSG_UNSPECIFIED,
        0,
        readerMessageMemPoolId,
        writerMessageMemPoolId,
        readerPayloadMemPoolId,
        writerPayloadMemPoolId);
}
#endif

/* Create a LIFO message queue with specified properties */
void slMsgSvcCreateLIFOMsgQueue(void *msgMgr, 
                                int_T id,
                                int_T capacity,
                                slMsgDataSize dataSize,
                                int_T dropPolicy,
                                slMsgMemPoolId messageMemPoolId,
                                slMsgMemPoolId payloadMemPoolId)
{
    slMsgManager *msgMgrT = (slMsgManager *)msgMgr;
    _slMsgCreateMsgQueue(msgMgrT,
                         id,
                         SLMSG_LIFO_QUEUE, 
                         capacity, 
                         (slMsgQueueDropPolicy) dropPolicy, 
                         dataSize, 
                         SLMSG_UNSPECIFIED, 
                         0,
                         messageMemPoolId,
                         payloadMemPoolId);
}

/* Create a Priority message queue with specified properties */
void slMsgSvcCreatePriorityMsgQueue(void *msgMgr, 
                                    int_T id,
                                    int_T capacity,
                                    int_T priorityMode,
                                    slMsgDataSize dataSize,
                                    int_T dataType,
                                    slMsgDataSize priorityDataOffset,
                                    int_T dropPolicy,
                                    slMsgMemPoolId messageMemPoolId,
                                    slMsgMemPoolId payloadMemPoolId)
{
    slMsgManager *msgMgrT = (slMsgManager *)msgMgr;
    slMsgQueueType qType;

    if (priorityMode == SLMSG_ASCENDING_PRIORITY) {
        qType = SLMSG_PRIORITY_QUEUE_ASCENDING;
    } else {
        __slmsg_assert(priorityMode == SLMSG_DESCENDING_PRIORITY);
        qType = SLMSG_PRIORITY_QUEUE_DESCENDING;
    }

    _slMsgCreateMsgQueue(msgMgrT,
                         id, 
                         qType,
                         capacity, 
                         (slMsgQueueDropPolicy) dropPolicy, 
                         dataSize, 
                         dataType, 
                         priorityDataOffset,
                         messageMemPoolId,
                         payloadMemPoolId);
}

/* Send a message to the specified queue */
boolean_T slMsgSvcSendMsgToQueue(void *msgMgr, void *msgptr, slMsgQueueId queueId)
{
    slMessage *droppedMsg;
    slMsgManager *msgMgrT = (slMsgManager *)msgMgr;

    if (msgMgrT->fQueues[queueId].fType == SLMSG_SRSW_LOCK_FREE_FIFO_QUEUE){
        droppedMsg = _slMsgSvcSRSWSendMsg(msgMgrT, (slMessage*)msgptr, queueId);
    } else {
        droppedMsg = _slMsgSvcSendMsg(msgMgrT, (slMessage*)msgptr, queueId);
    }

    if (droppedMsg != NULL){
        slMsgSvcDestroyMsg(msgMgr, droppedMsg);
    }
    return (droppedMsg != msgptr);
}

/* Pop the message at the top of the specified queue */
void *slMsgSvcPopMsgFromQueue(void *msgMgr, slMsgQueueId queueId)
{
    slMsgManager *msgMgrT = (slMsgManager *)msgMgr;
    if (queueId != SLMSG_UNSPECIFIED && msgMgrT->fQueues[queueId].fType == SLMSG_SRSW_LOCK_FREE_FIFO_QUEUE){
        return _slMsgSvcSRSWReadMsgFromQueue(msgMgrT, queueId, 1);
    } else {
        return _slMsgSvcPopMsgFromQueue(msgMgrT, queueId);
    }
}

/* Return the data held by the specified message */
void *slMsgSvcGetMsgData(void *msgptr)
{
    return _slMsgSvcGetMsgData((slMessage*)msgptr);
}

/* Create a new message */
void* slMsgSvcCreateMsg(void *msgMgr, const void* data, slMsgDataSize dataSize, slMsgQueueId queueId)
{
	return _slMsgSvcCreateMsg((slMsgManager *)msgMgr, data, dataSize, queueId);
}

#undef SLMSG_CIRCULAR_INDEX
/* EOF */

#ifdef SL_INTERNAL
#include "sl_messages/sl_messages_c_api.h"
#endif

#ifndef SLMSG_PRODUCTION_CODE     
void _des_slMsgSvcDestroyMsg(slMsgManager *mgr, slMessage *msg)
{
    _slMsgRemoveFromQueue(mgr, msg, msg->fQueueId);
    _slMsgSvcDestroyMsg(mgr, msg);
}

void _des_slMsgSvcAddMsgToQueue(slMsgManager *mgr, slMessage *msg, slMsgQueueId qId)
{
    slMessage *dropped = _slMsgAddToQueue(mgr, msg, qId);
    (void) dropped;
    __slmsg_assert(dropped == NULL);
}

void _des_slMsgSvcRemoveMsgFromQueue(slMsgManager *mgr, slMessage *msg)
{
    _slMsgRemoveFromQueue(mgr, msg, msg->fQueueId);

}

slMessage *_des_slMsgSvcCloneMsg(slMsgManager *mgr, slMessage *msg)
{
    slMessage *new_msg = _slMsgSvcCreateMsgWithId(mgr, msg->fData, msg->fDataSize, msg->fQueueId, msg->fId);
    new_msg->fPriority = msg->fPriority;
    return new_msg;
}

slMessage *_des_slMsgSvcPeekMsgFromQueueAtIndex(slMsgManager *mgr, slMsgQueueId qId, int index)
{
    return _slMsgSvcPeekMsgFromQueueAtIndex(mgr, qId, index);
}

int _des_slMsgSvcGetNumMsgsInQueue(slMsgManager *mgr, slMsgQueueId queueId)
{
    return _slMsgSvcGetNumMsgsInQueue(mgr, queueId);
}

slMessage *_des_slMsgSvcSendMsg(slMsgManager *mgr, slMessage *msg, slMsgQueueId queueId)
{
    return _slMsgSvcSendMsg(mgr, msg, queueId);
}

slMessage *_des_slMsgSvcCreateMsg(slMsgManager *mgr, void *data, slMsgQueueId queueId)
{
    return _slMsgSvcCreateMsg(mgr, data, mgr->fQueues[queueId].fDataSize, queueId);
}

real_T _des_slMsgSvcGetPriorityValWithCast(
    slMsgManager *mgr, slMessage *msg, slMsgQueueId queueId)
{
    real_T val = 0;
    slMsgQueue *q = NULL;
    q = &(mgr->fQueues[queueId]);
    if (q->fType == SLMSG_PRIORITY_QUEUE_ASCENDING ||
        q->fType == SLMSG_PRIORITY_QUEUE_DESCENDING ||
        q->fType == SLMSG_SYSPRIORITY_QUEUE_ASCENDING ||
        q->fType == SLMSG_SYSPRIORITY_QUEUE_DESCENDING) {
        val = _slMsgGetMsgPriorityValWithCast(msg, q);
    }
    return val;
}

void _des_slMsgSvcInitPool(slMsgMemPool* memPool,
    int_T numUnits,
    slMsgDataSize dataSize,
    void *memBuffer,
    boolean_T mallocAllowed)
{
    _slMsgSvcInitPool(memPool, numUnits, dataSize, memBuffer, mallocAllowed);
}

void *_des_slmsg_MemPool_Alloc(__slmsg_MemPool* pool, unsigned long nbytes)
{
    return __slmsg_MemPool_Alloc(pool, nbytes);
}

void _des_slmsg_MemPool_Free(__slmsg_MemPool *pool, void **ptrIn)
{
    __slmsg_private_MemPool_Free(pool, ptrIn);
}

void _des_slmsg_MemPool_Destroy(__slmsg_MemPool *pool)
{
    __slmsg_private_MemPool_Destroy(pool);
}
#endif

