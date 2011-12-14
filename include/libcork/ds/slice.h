/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_DS_SLICE_H
#define LIBCORK_DS_SLICE_H


#include <libcork/core/allocator.h>
#include <libcork/core/types.h>


/*-----------------------------------------------------------------------
 * Slices
 */

struct cork_slice;

struct cork_slice_iface {
    /* Free the slice.  Can be NULL if you don't need to free any
     * underlying buffer. */
    void
    (*free)(struct cork_slice *self);

    /* Create a copy of a slice.  You can assume that offset and length
     * refer to a valid subset of the buffer. */
    bool
    (*copy)(struct cork_slice *self, struct cork_slice *dest,
            size_t offset, size_t length);

    /* Update the current slice to point at a different subset.  You can
     * assume that offset and length refer to a valid subset of the
     * buffer.  Can be NULL if you don't need to do anything special to
     * the underlying buffer; in this case, we'll update the slice's buf
     * and size fields for you. */
    bool
    (*slice)(struct cork_slice *self, size_t offset, size_t length);
};


struct cork_slice {
    /* The beginning of the sliced portion of the buffer. */
    const void  *buf;
    /* The length of the sliced portion of the buffer. */
    size_t  size;
    /* The slice implementation of the underlying buffer. */
    struct cork_slice_iface  *iface;
    /* An opaque pointer used by the slice implementation to refer to
     * the underlying buffer. */
    void  *user_data;
};


void
cork_slice_clear(struct cork_slice *slice);

#define cork_slice_is_empty(slice)  ((slice)->buf == NULL)


bool
cork_slice_copy(struct cork_slice *dest,
                struct cork_slice *slice,
                size_t offset, size_t length);

bool
cork_slice_copy_offset(struct cork_slice *dest,
                       struct cork_slice *slice,
                       size_t offset);


bool
cork_slice_slice(struct cork_slice *slice,
                 size_t offset, size_t length);

bool
cork_slice_slice_offset(struct cork_slice *slice,
                        size_t offset);


void
cork_slice_finish(struct cork_slice *slice);

bool
cork_slice_equal(const struct cork_slice *slice1,
                 const struct cork_slice *slice2);


#endif /* LIBCORK_DS_SLICE_H */