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


#include <libcork/core/error.h>
#include <libcork/core/types.h>


/*-----------------------------------------------------------------------
 * Error handling
 */

/* hash of "libcork/ds/slice.h" */
#define CORK_SLICE_ERROR  0x960ca750

enum cork_slice_error {
    /* Trying to slice a nonexistent subset of a buffer */
    CORK_SLICE_INVALID_SLICE
};


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
    int
    (*copy)(struct cork_slice *self, struct cork_slice *dest,
            size_t offset, size_t length, struct cork_error *err);

    /* Update the current slice to point at a different subset.  You can
     * assume that offset and length refer to a valid subset of the
     * buffer.  Can be NULL if you don't need to do anything special to
     * the underlying buffer; in this case, we'll update the slice's buf
     * and size fields for you. */
    int
    (*slice)(struct cork_slice *self, size_t offset, size_t length,
             struct cork_error *err);
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


int
cork_slice_copy(struct cork_slice *dest, struct cork_slice *slice,
                size_t offset, size_t length, struct cork_error *err);

int
cork_slice_copy_offset(struct cork_slice *dest, struct cork_slice *slice,
                       size_t offset, struct cork_error *err);


int
cork_slice_slice(struct cork_slice *slice, size_t offset, size_t length,
                 struct cork_error *err);

int
cork_slice_slice_offset(struct cork_slice *slice, size_t offset,
                        struct cork_error *err);


void
cork_slice_finish(struct cork_slice *slice);

bool
cork_slice_equal(const struct cork_slice *slice1,
                 const struct cork_slice *slice2);

void
cork_slice_init_static(struct cork_slice *dest, const void *buf, size_t size);


#endif /* LIBCORK_DS_SLICE_H */
