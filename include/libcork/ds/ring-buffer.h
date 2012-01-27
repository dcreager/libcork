/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_DS_RING_BUFFER_H
#define LIBCORK_DS_RING_BUFFER_H


#include <libcork/core/types.h>


struct cork_ring_buffer {
    /* The elements of the ring buffer */
    void  **elements;
    /* The number of elements that can be stored in this ring
     * buffer. */
    size_t  allocated_size;
    /* The actual number of elements currently in the ring buffer. */
    size_t  size;
    /* The index of the next element to read from the buffer */
    size_t  read_index;
    /* The index of the next element to write into the buffer */
    size_t  write_index;
};


int
cork_ring_buffer_init(struct cork_ring_buffer *buf, size_t size);

void
cork_ring_buffer_done(struct cork_ring_buffer *buf);


#define cork_ring_buffer_is_empty(buf) ((buf)->size == 0)
#define cork_ring_buffer_is_full(buf) ((buf)->size == (buf)->allocated_size)


int
cork_ring_buffer_add(struct cork_ring_buffer *buf, void *element);

void *
cork_ring_buffer_pop(struct cork_ring_buffer *buf);

void *
cork_ring_buffer_peek(struct cork_ring_buffer *buf);


#endif /* LIBCORK_DS_RING_BUFFER_H */
