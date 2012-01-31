/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_DS_MANAGED_BUFFER_H
#define LIBCORK_DS_MANAGED_BUFFER_H


#include <libcork/core/error.h>
#include <libcork/core/types.h>
#include <libcork/ds/slice.h>


/*-----------------------------------------------------------------------
 * Managed buffers
 */

struct cork_managed_buffer;

struct cork_managed_buffer_iface {
    /* Free the contents of a managed buffer, and the managed buffer
     * object itself. */
    void
    (*free)(struct cork_managed_buffer *buf);
};


struct cork_managed_buffer {
    /* The buffer that this instance manages */
    const void  *buf;
    /* The size of buf */
    size_t  size;
    /* A reference count for the buffer.  If this drops to 0, the buffer
     * will be finalized. */
    volatile int  ref_count;
    /* The managed buffer implementation for this instance. */
    struct cork_managed_buffer_iface  *iface;
};


struct cork_managed_buffer *
cork_managed_buffer_new_copy(const void *buf, size_t size,
                             struct cork_error *err);


typedef void
(*cork_managed_buffer_freer)(void *buf, size_t size);

struct cork_managed_buffer *
cork_managed_buffer_new(const void *buf, size_t size,
                        cork_managed_buffer_freer free,
                        struct cork_error *err);


struct cork_managed_buffer *
cork_managed_buffer_ref(struct cork_managed_buffer *buf);

void
cork_managed_buffer_unref(struct cork_managed_buffer *buf);


int
cork_managed_buffer_slice(struct cork_slice *dest,
                          struct cork_managed_buffer *buffer,
                          size_t offset, size_t length,
                          struct cork_error *err);

int
cork_managed_buffer_slice_offset(struct cork_slice *dest,
                                 struct cork_managed_buffer *buffer,
                                 size_t offset,
                                 struct cork_error *err);


#endif /* LIBCORK_DS_MANAGED_BUFFER_H */
