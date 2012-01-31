/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <assert.h>
#include <stdlib.h>

#include "libcork/core/checkers.h"
#include "libcork/core/mempool.h"
#include "libcork/core/types.h"


#if !defined(CORK_DEBUG_MEMPOOL)
#define CORK_DEBUG_MEMPOOL  0
#endif

#if CORK_DEBUG_MEMPOOL
#include <stdio.h>
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG(...) /* no debug messages */
#endif


struct cork_mempool_object {
    /* When this object is unclaimed, it will be in the cork_mempool
     * object's free_list using this pointer. */
    struct cork_mempool_object  *next_free;
};

#define cork_mempool_object_size(mp) \
    (sizeof(struct cork_mempool_object) + (mp)->element_size)

#define cork_mempool_get_header(obj) \
    (((struct cork_mempool_object *) (obj)) - 1)

#define cork_mempool_get_object(hdr) \
    ((void *) (((struct cork_mempool_object *) (hdr)) + 1))

struct cork_mempool_block {
    struct cork_mempool_block  *next_block;
};


void
cork_mempool_init_size_ex(struct cork_mempool *mp, size_t element_size,
                          size_t block_size)
{
    mp->element_size = element_size;
    mp->block_size = block_size;
    mp->free_list = NULL;
    mp->allocated_count = 0;
    mp->blocks = NULL;
}

void
cork_mempool_done(struct cork_mempool *mp)
{
    struct cork_mempool_block  *curr;
    assert(mp->allocated_count == 0);
    for (curr = mp->blocks; curr != NULL; ) {
        struct cork_mempool_block  *next = curr->next_block;
        free(curr);
        /* Do this here instead of in the for statement to avoid
         * accessing the just-freed block. */
        curr = next;
    }
}


/* If this function succeeds, then we guarantee that there will be at
 * least one object in mp->free_list. */
static int
cork_mempool_new_block(struct cork_mempool *mp)
{
    /* Allocate the new block and add it to mp's block list. */
    struct cork_mempool_block  *block;
    void  *vblock;
    DEBUG("Allocating new %zu-byte block\n", mp->block_size);
    rip_check(block = malloc(mp->block_size));
    block->next_block = mp->blocks;
    mp->blocks = block;
    vblock = block;

    /* Divide the block's memory region into a bunch of objects. */
    size_t  index = sizeof(struct cork_mempool_block);
    for (index = sizeof(struct cork_mempool_block);
         (index + cork_mempool_object_size(mp)) <= mp->block_size;
         index += cork_mempool_object_size(mp)) {
        struct cork_mempool_object  *obj = vblock + index;
        DEBUG("  New object at %p[%p]\n", cork_mempool_get_object(obj), obj);
        obj->next_free = mp->free_list;
        mp->free_list = obj;
    }

    return 0;
}


void *
cork_mempool_new(struct cork_mempool *mp)
{
    struct cork_mempool_object  *obj;
    void  *ptr;

    if (CORK_UNLIKELY(mp->free_list == NULL)) {
        rpi_check(cork_mempool_new_block(mp));
    }

    obj = mp->free_list;
    mp->free_list = obj->next_free;
    mp->allocated_count++;
    ptr = cork_mempool_get_object(obj);
    DEBUG("Allocated %p[%p] from memory pool\n", ptr, obj);
    return ptr;
}

void
cork_mempool_free(struct cork_mempool *mp, void *ptr)
{
    struct cork_mempool_object  *obj = cork_mempool_get_header(ptr);
    DEBUG("Returning %p[%p] to memory pool\n", ptr, obj);
    obj->next_free = mp->free_list;
    mp->free_list = obj;
    mp->allocated_count--;
}
