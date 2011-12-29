/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_DS_BUFFER_H
#define LIBCORK_DS_BUFFER_H


#include <stdarg.h>

#include <libcork/core/allocator.h>
#include <libcork/core/attributes.h>
#include <libcork/core/types.h>


/* We have to forward declare struct cork_error here, since error.h
 * depends on this file. */
struct cork_error;


struct cork_buffer {
    /* The current contents of the buffer. */
    void  *buf;
    /* The current size of the buffer. */
    size_t  size;
    /* The amount of space allocated for buf. */
    size_t  allocated_size;
};


void
cork_buffer_init(struct cork_alloc *alloc, struct cork_buffer *buffer);

#define CORK_BUFFER_INIT()  { NULL, 0, 0 }

struct cork_buffer *
cork_buffer_new(struct cork_alloc *alloc, struct cork_error *err);

void
cork_buffer_done(struct cork_alloc *alloc, struct cork_buffer *buffer);

void
cork_buffer_free(struct cork_alloc *alloc, struct cork_buffer *buffer);


bool
cork_buffer_equal(const struct cork_buffer *buffer1,
                  const struct cork_buffer *buffer2);


int
cork_buffer_ensure_size(struct cork_alloc *alloc,
                        struct cork_buffer *buffer, size_t desired_size,
                        struct cork_error *err);


void
cork_buffer_clear(struct cork_alloc *alloc, struct cork_buffer *buffer);


/*-----------------------------------------------------------------------
 * A whole bunch of methods for adding data
 */

int
cork_buffer_set(struct cork_alloc *alloc, struct cork_buffer *buffer,
                const void *src, size_t length, struct cork_error *err);

int
cork_buffer_append(struct cork_alloc *alloc, struct cork_buffer *buffer,
                   const void *src, size_t length, struct cork_error *err);


int
cork_buffer_set_string(struct cork_alloc *alloc, struct cork_buffer *buffer,
                       const char *str, struct cork_error *err);

int
cork_buffer_append_string(struct cork_alloc *alloc, struct cork_buffer *buffer,
                          const char *str, struct cork_error *err);


int
cork_buffer_printf(struct cork_alloc *alloc, struct cork_buffer *buffer,
                   struct cork_error *err, const char *format, ...)
    CORK_ATTR_PRINTF(4,5);

int
cork_buffer_append_printf(struct cork_alloc *alloc, struct cork_buffer *buffer,
                          struct cork_error *err, const char *format, ...)
    CORK_ATTR_PRINTF(4,5);

int
cork_buffer_vprintf(struct cork_alloc *alloc, struct cork_buffer *buffer,
                    const char *format, va_list args, struct cork_error *err)
    CORK_ATTR_PRINTF(3,0);

int
cork_buffer_append_vprintf(struct cork_alloc *alloc, struct cork_buffer *buffer,
                           const char *format, va_list args,
                           struct cork_error *err)
    CORK_ATTR_PRINTF(3,0);


/*-----------------------------------------------------------------------
 * Buffer's managed buffer/slice implementation
 */

#include <libcork/ds/managed-buffer.h>
#include <libcork/ds/slice.h>

struct cork_managed_buffer *
cork_buffer_to_managed_buffer(struct cork_alloc *alloc,
                              struct cork_buffer *buffer,
                              struct cork_error *err);

int
cork_buffer_to_slice(struct cork_alloc *alloc, struct cork_buffer *buffer,
                     struct cork_slice *slice,
                     struct cork_error *err);


/*-----------------------------------------------------------------------
 * Buffer's stream consumer implementation
 */

#include <libcork/ds/stream.h>

struct cork_stream_consumer *
cork_buffer_to_stream_consumer(struct cork_alloc *alloc,
                               struct cork_buffer *buffer,
                               struct cork_error *err);


#endif /* LIBCORK_DS_BUFFER_H */
