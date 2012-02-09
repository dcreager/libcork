/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CORK_MEMPOOL_H
#define LIBCORK_CORK_MEMPOOL_H


#include <libcork/config.h>
#include <libcork/core/attributes.h>
#include <libcork/core/types.h>


#define CORK_MEMPOOL_DEFAULT_BLOCK_SIZE  4096


struct cork_mempool_block;
struct cork_mempool_object;

struct cork_mempool {
    size_t  element_size;
    size_t  block_size;
    struct cork_mempool_object  *free_list;
    /* The number of objects that have been given out by
     * cork_mempool_new but not returned via cork_mempool_free. */
    size_t  allocated_count;
    struct cork_mempool_block  *blocks;

    int
    (*init_object)(void *obj);

    void
    (*done_object)(void *obj);
};

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


void
cork_mempool_init_size_ex(struct cork_mempool *mp, size_t element_size,
                          size_t block_size);

#define cork_mempool_init_size(mp, element_size) \
    (cork_mempool_init_size_ex \
     ((mp), (element_size), CORK_MEMPOOL_DEFAULT_BLOCK_SIZE))

#define cork_mempool_init_ex(mp, type, block_size) \
    (cork_mempool_init_size_ex((mp), sizeof(type), (block_size)))

#define cork_mempool_init(mp, type) \
    (cork_mempool_init_size((mp), sizeof(type)))

void
cork_mempool_done(struct cork_mempool *mp);


int
cork_mempool_new_block(struct cork_mempool *mp);


CORK_ATTR_UNUSED
static void *
cork_mempool_new(struct cork_mempool *mp)
{
    struct cork_mempool_object  *obj;
    void  *ptr;

    if (CORK_UNLIKELY(mp->free_list == NULL)) {
        int  __rc = cork_mempool_new_block(mp);
        if (__rc != 0) {
            return NULL;
        }
    }

    obj = mp->free_list;
    mp->free_list = obj->next_free;
    mp->allocated_count++;
    ptr = cork_mempool_get_object(obj);
    return ptr;
}


void
cork_mempool_free(struct cork_mempool *mp, void *ptr);


#endif /* LIBCORK_CORK_MEMPOOL_H */
