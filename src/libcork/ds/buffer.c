/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "libcork/core/allocator.h"
#include "libcork/core/types.h"
#include "libcork/ds/buffer.h"
#include "libcork/ds/managed-buffer.h"
#include "libcork/ds/stream.h"


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


void
cork_buffer_clear(cork_buffer_t *buffer)
{
    buffer->size = 0;
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
cork_buffer_append(cork_buffer_t *buffer, const void *src, size_t length)
{
    if (!cork_buffer_ensure_size(buffer, buffer->size + length + 1)) {
        return false;
    }

    memcpy(buffer->buf + buffer->size, src, length);
    buffer->size += length;
    ((char *) buffer->buf)[buffer->size] = '\0';
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


bool
cork_buffer_append_string(cork_buffer_t *buffer, const char *str)
{
    size_t  new_size;
    size_t  dest_offset;
    size_t  length = strlen(str);

    if ((buffer->size > 0) &&
        ((char *) buffer->buf)[buffer->size-1] == '\0') {
        /*
         * Overwrite the existing NUL-terminator
         */
        new_size = buffer->size + length;
        dest_offset = buffer->size - 1;
    } else {
        new_size = buffer->size + length + 1;
        dest_offset = buffer->size;
    }

    if (!cork_buffer_ensure_size(buffer, new_size)) {
        return false;
    }

    memcpy(buffer->buf + dest_offset, str, length+1);
    buffer->size = new_size;
    return true;
}


bool
cork_buffer_append_vprintf(cork_buffer_t *buffer,
                           const char *format, va_list args)
{
    va_list  args1;
    va_copy(args1, args);
    size_t  formatted_length = vsnprintf(NULL, 0, format, args1);
    va_end(args1);

    size_t  new_size;
    size_t  dest_offset;

    if ((buffer->size > 0) &&
        ((char *) buffer->buf)[buffer->size-1] == '\0') {
        /*
         * Overwrite the existing NUL-terminator
         */
        new_size = buffer->size + formatted_length;
        dest_offset = buffer->size - 1;
    } else {
        new_size = buffer->size + formatted_length + 1;
        dest_offset = buffer->size;
    }

    if (!cork_buffer_ensure_size(buffer, new_size)) {
        return false;
    }

    vsnprintf(buffer->buf + dest_offset, formatted_length+1, format, args);
    buffer->size = new_size;
    return true;
}


bool
cork_buffer_vprintf(cork_buffer_t *buffer, const char *format, va_list args)
{
    cork_buffer_clear(buffer);
    return cork_buffer_append_vprintf(buffer, format, args);
}


bool
cork_buffer_append_printf(cork_buffer_t *buffer, const char *format, ...)
{
    va_list  args;
    va_start(args, format);
    bool  result = cork_buffer_append_vprintf(buffer, format, args);
    va_end(args);
    return result;
}


bool
cork_buffer_printf(cork_buffer_t *buffer, const char *format, ...)
{
    va_list  args;
    va_start(args, format);
    bool  result = cork_buffer_vprintf(buffer, format, args);
    va_end(args);
    return result;
}


typedef struct cork_buffer__managed_buffer_t {
    cork_managed_buffer_t  parent;
    cork_buffer_t  *buffer;
} cork_buffer__managed_buffer_t;

static void
cork_buffer__managed_free(cork_managed_buffer_t *vself)
{
    cork_buffer__managed_buffer_t  *self =
        cork_container_of(vself, cork_buffer__managed_buffer_t, parent);
    cork_allocator_t  *alloc = self->buffer->alloc;
    cork_buffer_free(self->buffer);
    cork_delete(alloc, cork_buffer__managed_buffer_t, self);
}

static cork_managed_buffer_iface_t  CORK_BUFFER__MANAGED_BUFFER = {
    cork_buffer__managed_free
};

cork_managed_buffer_t *
cork_buffer_to_managed_buffer(cork_buffer_t *buffer)
{
    cork_buffer__managed_buffer_t  *self =
        cork_new(buffer->alloc, cork_buffer__managed_buffer_t);
    if (self == NULL) {
        cork_buffer_free(buffer);
        return NULL;
    }

    self->parent.buf = buffer->buf;
    self->parent.size = buffer->size;
    self->parent.ref_count = 1;
    self->parent.iface = &CORK_BUFFER__MANAGED_BUFFER;
    self->buffer = buffer;
    return &self->parent;
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


typedef struct cork_buffer__stream_consumer_t {
    cork_stream_consumer_t  consumer;
    cork_buffer_t  *buffer;
} cork_buffer__stream_consumer_t;

static bool
cork_buffer_stream_consumer_data(cork_stream_consumer_t *consumer,
                                 cork_slice_t *slice, bool is_first_chunk)
{
    cork_buffer__stream_consumer_t  *bconsumer =
        cork_container_of(consumer, cork_buffer__stream_consumer_t, consumer);

    if (is_first_chunk) {
        cork_buffer_clear(bconsumer->buffer);
    }

    return cork_buffer_append(bconsumer->buffer, slice->buf, slice->size);
}

static bool
cork_buffer_stream_consumer_eof(cork_stream_consumer_t *consumer)
{
    return true;
}

static void
cork_buffer_stream_consumer_free(cork_stream_consumer_t *consumer)
{
    cork_buffer__stream_consumer_t  *bconsumer =
        cork_container_of(consumer, cork_buffer__stream_consumer_t, consumer);
    cork_allocator_t  *alloc = bconsumer->buffer->alloc;

    cork_delete(alloc, cork_buffer__stream_consumer_t, bconsumer);
}

cork_stream_consumer_t *
cork_buffer_to_stream_consumer(cork_buffer_t *buffer)
{
    cork_buffer__stream_consumer_t  *bconsumer =
        cork_new(buffer->alloc, cork_buffer__stream_consumer_t);
    bconsumer->consumer.data = cork_buffer_stream_consumer_data;
    bconsumer->consumer.eof = cork_buffer_stream_consumer_eof;
    bconsumer->consumer.free = cork_buffer_stream_consumer_free;
    bconsumer->buffer = buffer;
    return &bconsumer->consumer;
}
