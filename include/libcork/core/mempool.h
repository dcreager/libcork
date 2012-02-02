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


void *
cork_mempool_new(struct cork_mempool *mp) CORK_ATTR_MALLOC;

void
cork_mempool_free(struct cork_mempool *mp, void *ptr);


#endif /* LIBCORK_CORK_MEMPOOL_H */
