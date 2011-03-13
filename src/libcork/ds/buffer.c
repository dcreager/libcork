/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <string.h>

#include "libcork/core/allocator.h"
#include "libcork/core/types.h"
#include "libcork/ds/buffer.h"
#include "libcork/ds/managed-buffer.h"


void
cork_buffer_init(cork_allocator_t *alloc, cork_buffer_t *buffer)
{
    buffer->buf = NULL;
    buffer->size = 0;
    buffer->allocated_size = 0;
    buffer->alloc = alloc;
}


cork_buffer_t *
cork_buffer_new(cork_allocator_t *alloc)
{
    cork_buffer_t  *buffer = cork_new(alloc, cork_buffer_t);
    if (buffer == NULL) {
        return NULL;
    }

    cork_buffer_init(alloc, buffer);
    return buffer;
}


void
cork_buffer_done(cork_buffer_t *buffer)
{
    if (buffer->buf != NULL) {
        cork_free(buffer->alloc, buffer->buf, buffer->allocated_size);
        buffer->buf = NULL;
        buffer->size = 0;
        buffer->allocated_size = 0;
        buffer->alloc = NULL;
    }
}


void
cork_buffer_free(cork_buffer_t *buffer)
{
    cork_allocator_t  *alloc = buffer->alloc;
    cork_buffer_done(buffer);
    cork_delete(alloc, cork_buffer_t, buffer);
}


bool
cork_buffer_equal(const cork_buffer_t *buffer1, const cork_buffer_t *buffer2)
{
    if (buffer1 == buffer2) {
        return true;
    }

    if (buffer1->size != buffer2->size) {
        return false;
    }

    return (memcmp(buffer1->buf, buffer2->buf, buffer1->size) == 0);
}


bool
cork_buffer_ensure_size(cork_buffer_t *buffer, size_t desired_size)
{
    if (buffer->allocated_size >= desired_size) {
        return true;
    }

    /*
     * Make sure we at least double the old size when reallocating.
     */

    size_t  new_size = buffer->allocated_size * 2;
    if (desired_size > new_size) {
        new_size = desired_size;
    }

    buffer->buf = cork_realloc(buffer->alloc, buffer->buf,
                               buffer->allocated_size, new_size);

    if (buffer->buf == NULL) {
        buffer->allocated_size = 0;
        return false;
    }

    buffer->allocated_size = new_size;
    return true;
}


bool
cork_buffer_set(cork_buffer_t *buffer, const void *src, size_t length)
{
    if (!cork_buffer_ensure_size(buffer, length+1)) {
        return false;
    }

    memcpy(buffer->buf, src, length);
    ((char *) buffer->buf)[length] = '\0';
    buffer->size = length;
    return true;
}


bool
cork_buffer_set_string(cork_buffer_t *buffer, const char *str)
{
    size_t  length = strlen(str);

    if (!cork_buffer_ensure_size(buffer, length+1)) {
        return false;
    }

    memcpy(buffer->buf, str, length+1);
    buffer->size = length+1;
    return true;
}


typedef struct cork_buffer__managed_buffer_t {
    cork_managed_buffer_t  managed_buffer;
    cork_buffer_t  *buffer;
} cork_buffer__managed_buffer_t;

static void
cork_buffer_managed_free(cork_managed_buffer_t *mbuf)
{
    cork_buffer__managed_buffer_t  *bmbuf =
        cork_container_of(mbuf, cork_buffer__managed_buffer_t, managed_buffer);
    cork_allocator_t  *alloc = bmbuf->buffer->alloc;
    cork_buffer_free(bmbuf->buffer);
    cork_delete(alloc, cork_buffer__managed_buffer_t, bmbuf);
}


cork_managed_buffer_t *
cork_buffer_to_managed_buffer(cork_buffer_t *buffer)
{
    cork_buffer__managed_buffer_t  *managed =
        cork_new(buffer->alloc, cork_buffer__managed_buffer_t);
    if (managed == NULL) {
        cork_buffer_free(buffer);
        return NULL;
    }

    managed->managed_buffer.buf = buffer->buf;
    managed->managed_buffer.size = buffer->size;
    managed->managed_buffer.free = cork_buffer_managed_free;
    managed->managed_buffer.ref_count = 1;
    managed->managed_buffer.alloc = buffer->alloc;
    managed->buffer = buffer;
    return &managed->managed_buffer;
}


bool
cork_buffer_to_slice(cork_buffer_t *buffer, cork_slice_t *slice)
{
    cork_managed_buffer_t  *managed = cork_buffer_to_managed_buffer(buffer);

    /*
     * We don't have to check for NULL; cork_managed_buffer_slice_offset
     * will do that for us.
     */

    bool  result = cork_managed_buffer_slice_offset(slice, managed, 0);

    /*
     * Before returning, drop our reference to the managed buffer.  If
     * the slicing succeeded, then there will be one remaining reference
     * in the slice.  If it didn't succeed, this will free the managed
     * buffer for us.
     */

    if (managed != NULL) {
        cork_managed_buffer_unref(managed);
    }

    return result;
}
