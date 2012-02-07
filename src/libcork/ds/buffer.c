/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011-2012, RedJack, LLC.
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
#include "libcork/core/error.h"
#include "libcork/core/types.h"
#include "libcork/ds/buffer.h"
#include "libcork/ds/managed-buffer.h"
#include "libcork/ds/stream.h"
#include "libcork/helpers/errors.h"


void
cork_buffer_init(struct cork_buffer *buffer)
{
    buffer->buf = NULL;
    buffer->size = 0;
    buffer->allocated_size = 0;
}


struct cork_buffer *
cork_buffer_new(struct cork_error *err)
{
    struct cork_buffer  *buffer;
    rp_check_new(struct cork_buffer, buffer, "buffer");
    cork_buffer_init(buffer);
    return buffer;
}


void
cork_buffer_done(struct cork_buffer *buffer)
{
    if (buffer->buf != NULL) {
        free(buffer->buf);
        buffer->buf = NULL;
    }
    buffer->size = 0;
    buffer->allocated_size = 0;
}


void
cork_buffer_free(struct cork_buffer *buffer)
{
    cork_buffer_done(buffer);
    cork_delete(struct cork_buffer, buffer);
}


bool
cork_buffer_equal(const struct cork_buffer *buffer1,
                  const struct cork_buffer *buffer2)
{
    if (buffer1 == buffer2) {
        return true;
    }

    if (buffer1->size != buffer2->size) {
        return false;
    }

    return (memcmp(buffer1->buf, buffer2->buf, buffer1->size) == 0);
}


int
cork_buffer_ensure_size(struct cork_buffer *buffer, size_t desired_size,
                        struct cork_error *err)
{
    if (buffer->allocated_size >= desired_size) {
        return 0;
    }

    /*
     * Make sure we at least double the old size when reallocating.
     */

    size_t  new_size = buffer->allocated_size * 2;
    if (desired_size > new_size) {
        new_size = desired_size;
    }

    buffer->buf = cork_realloc(buffer->buf, new_size);

    if (buffer->buf == NULL) {
        buffer->allocated_size = 0;
        cork_cannot_allocate_set(err, "buffer contents");
        return -1;
    }

    buffer->allocated_size = new_size;
    return 0;
}


void
cork_buffer_clear(struct cork_buffer *buffer)
{
    buffer->size = 0;
}


int
cork_buffer_set(struct cork_buffer *buffer, const void *src, size_t length,
                struct cork_error *err)
{
    rii_check(cork_buffer_ensure_size(buffer, length+1, err));
    memcpy(buffer->buf, src, length);
    ((char *) buffer->buf)[length] = '\0';
    buffer->size = length;
    return 0;
}


int
cork_buffer_append(struct cork_buffer *buffer, const void *src, size_t length,
                   struct cork_error *err)
{
    rii_check(cork_buffer_ensure_size(buffer, buffer->size + length + 1, err));
    memcpy(buffer->buf + buffer->size, src, length);
    buffer->size += length;
    ((char *) buffer->buf)[buffer->size] = '\0';
    return 0;
}


int
cork_buffer_set_string(struct cork_buffer *buffer, const char *str,
                       struct cork_error *err)
{
    return cork_buffer_set(buffer, str, strlen(str), err);
}


int
cork_buffer_append_string(struct cork_buffer *buffer, const char *str,
                          struct cork_error *err)
{
    return cork_buffer_append(buffer, str, strlen(str), err);
}


int
cork_buffer_append_vprintf(struct cork_buffer *buffer, const char *format,
                           va_list args, struct cork_error *err)
{
    size_t  new_size;
    va_list  args1;
    va_copy(args1, args);
    size_t  formatted_length = vsnprintf(NULL, 0, format, args1);
    va_end(args1);

    new_size = buffer->size + formatted_length;
    rii_check(cork_buffer_ensure_size(buffer, new_size+1, err));
    vsnprintf(buffer->buf + buffer->size, formatted_length+1, format, args);
    buffer->size = new_size;
    return 0;
}


int
cork_buffer_vprintf(struct cork_buffer *buffer, const char *format,
                    va_list args, struct cork_error *err)
{
    cork_buffer_clear(buffer);
    return cork_buffer_append_vprintf(buffer, format, args, err);
}


int
cork_buffer_append_printf(struct cork_buffer *buffer, struct cork_error *err,
                          const char *format, ...)
{
    va_list  args;
    va_start(args, format);
    int  rc = cork_buffer_append_vprintf(buffer, format, args, err);
    va_end(args);
    return rc;
}


int
cork_buffer_printf(struct cork_buffer *buffer, struct cork_error *err,
                   const char *format, ...)
{
    va_list  args;
    va_start(args, format);
    int  rc = cork_buffer_vprintf(buffer, format, args, err);
    va_end(args);
    return rc;
}


struct cork_buffer__managed_buffer {
    struct cork_managed_buffer  parent;
    struct cork_buffer  *buffer;
};

static void
cork_buffer__managed_free(struct cork_managed_buffer *vself)
{
    struct cork_buffer__managed_buffer  *self =
        cork_container_of(vself, struct cork_buffer__managed_buffer, parent);
    cork_buffer_free(self->buffer);
    cork_delete(struct cork_buffer__managed_buffer, self);
}

static struct cork_managed_buffer_iface  CORK_BUFFER__MANAGED_BUFFER = {
    cork_buffer__managed_free
};

struct cork_managed_buffer *
cork_buffer_to_managed_buffer(struct cork_buffer *buffer,
                              struct cork_error *err)
{
    struct cork_buffer__managed_buffer  *self;
    e_check_new(struct cork_buffer__managed_buffer, self,
                "managed buffer wrapper for buffer");

    self->parent.buf = buffer->buf;
    self->parent.size = buffer->size;
    self->parent.ref_count = 1;
    self->parent.iface = &CORK_BUFFER__MANAGED_BUFFER;
    self->buffer = buffer;
    return &self->parent;

error:
    cork_buffer_free(buffer);
    return NULL;
}


int
cork_buffer_to_slice(struct cork_buffer *buffer, struct cork_slice *slice,
                     struct cork_error *err)
{
    struct cork_managed_buffer  *managed;
    rip_check(managed = cork_buffer_to_managed_buffer(buffer, err));

    /*
     * We don't have to check for NULL; cork_managed_buffer_slice_offset
     * will do that for us.
     */

    int  rc = cork_managed_buffer_slice_offset(slice, managed, 0, err);

    /*
     * Before returning, drop our reference to the managed buffer.  If
     * the slicing succeeded, then there will be one remaining reference
     * in the slice.  If it didn't succeed, this will free the managed
     * buffer for us.
     */

    cork_managed_buffer_unref(managed);
    return rc;
}


struct cork_buffer__stream_consumer {
    struct cork_stream_consumer  consumer;
    struct cork_buffer  *buffer;
};

static int
cork_buffer_stream_consumer_data(struct cork_stream_consumer *consumer,
                                 struct cork_slice *slice, bool is_first_chunk,
                                 struct cork_error *err)
{
    struct cork_buffer__stream_consumer  *bconsumer = cork_container_of
        (consumer, struct cork_buffer__stream_consumer, consumer);

    if (is_first_chunk) {
        cork_buffer_clear(bconsumer->buffer);
    }

    return cork_buffer_append
        (bconsumer->buffer, slice->buf, slice->size, err);
}

static int
cork_buffer_stream_consumer_eof(struct cork_stream_consumer *consumer,
                                struct cork_error *err)
{
    return 0;
}

static void
cork_buffer_stream_consumer_free(struct cork_stream_consumer *consumer)
{
    struct cork_buffer__stream_consumer  *bconsumer = cork_container_of
        (consumer, struct cork_buffer__stream_consumer, consumer);

    cork_delete(struct cork_buffer__stream_consumer, bconsumer);
}

struct cork_stream_consumer *
cork_buffer_to_stream_consumer(struct cork_buffer *buffer,
                               struct cork_error *err)
{
    struct cork_buffer__stream_consumer  *bconsumer;
    rp_check_new(struct cork_buffer__stream_consumer, bconsumer,
                 "stream consumer wrapper for buffer");
    bconsumer->consumer.data = cork_buffer_stream_consumer_data;
    bconsumer->consumer.eof = cork_buffer_stream_consumer_eof;
    bconsumer->consumer.free = cork_buffer_stream_consumer_free;
    bconsumer->buffer = buffer;
    return &bconsumer->consumer;
}
