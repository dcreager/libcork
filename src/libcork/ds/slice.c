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

#include "libcork/core/checkers.h"
#include "libcork/core/error.h"
#include "libcork/core/types.h"
#include "libcork/ds/slice.h"


/*-----------------------------------------------------------------------
 * Error handling
 */

static void
cork_slice_invalid_slice_set(struct cork_alloc *alloc,
                             struct cork_error *err,
                             size_t buf_size, size_t requested_offset,
                             size_t requested_length)
{
    cork_error_set
        (alloc, err, CORK_SLICE_ERROR, CORK_SLICE_INVALID_SLICE,
         "Cannot slice %zu-byte buffer at %zu:%zu",
         buf_size, requested_offset, requested_length);
}


/*-----------------------------------------------------------------------
 * Slices
 */

void
cork_slice_clear(struct cork_alloc *alloc, struct cork_slice *slice)
{
    slice->buf = NULL;
    slice->size = 0;
    slice->iface = NULL;
    slice->user_data = NULL;
}


int
cork_slice_copy(struct cork_alloc *alloc, struct cork_slice *dest,
                struct cork_slice *slice, size_t offset, size_t length,
                struct cork_error *err)
{
    if ((slice != NULL) &&
        (offset <= slice->size) &&
        ((offset + length) <= slice->size)) {
        /*
        DEBUG("Slicing <%p:%zu> at %zu:%zu, gives <%p:%zu>",
              slice->buf, slice->size,
              offset, length,
              slice->buf + offset, length);
        */
        return slice->iface->copy(alloc, slice, dest, offset, length, err);
    }

    else {
        /*
        DEBUG("Cannot slice <%p:%zu> at %zu:%zu",
              slice->buf, slice->size,
              offset, length);
        */
        cork_slice_clear(alloc, dest);
        cork_slice_invalid_slice_set
            (alloc, err, (slice == NULL)? 0: slice->size, offset, length);
        return -1;
    }
}


int
cork_slice_copy_offset(struct cork_alloc *alloc, struct cork_slice *dest,
                       struct cork_slice *slice, size_t offset,
                       struct cork_error *err)
{
    if (slice == NULL) {
        cork_slice_clear(alloc, dest);
        cork_slice_invalid_slice_set(alloc, err, 0, offset, 0);
        return -1;
    } else {
        return cork_slice_copy
            (alloc, dest, slice, offset, slice->size - offset, err);
    }
}


int
cork_slice_slice(struct cork_alloc *alloc, struct cork_slice *slice,
                 size_t offset, size_t length, struct cork_error *err)
{
    if ((slice != NULL) &&
        (offset <= slice->size) &&
        ((offset + length) <= slice->size)) {
        /*
        DEBUG("Slicing <%p:%zu> at %zu:%zu, gives <%p:%zu>",
              slice->buf, slice->size,
              offset, length,
              slice->buf + offset, length);
        */
        if (slice->iface->slice == NULL) {
            slice->buf += offset;
            slice->size = length;
            return 0;
        } else {
            return slice->iface->slice(alloc, slice, offset, length, err);
        }
    }

    else {
        /*
        DEBUG("Cannot slice <%p:%zu> at %zu:%zu",
              slice->buf, slice->size,
              offset, length);
        */
        cork_slice_invalid_slice_set(alloc, err, slice->size, offset, length);
        return -1;
    }
}


int
cork_slice_slice_offset(struct cork_alloc *alloc, struct cork_slice *slice,
                        size_t offset,
                        struct cork_error *err)
{
    if (slice == NULL) {
        cork_slice_invalid_slice_set(alloc, err, 0, offset, 0);
        return -1;
    } else {
        return cork_slice_slice
            (alloc, slice, offset, slice->size - offset, err);
    }
}


void
cork_slice_finish(struct cork_alloc *alloc, struct cork_slice *slice)
{
    /*
    DEBUG("Finalizing <%p:%zu>", dest->buf, dest->size);
    */

    if (slice->iface->free != NULL) {
        slice->iface->free(alloc, slice);
    }

    cork_slice_clear(alloc, slice);
}


bool
cork_slice_equal(const struct cork_slice *slice1,
                 const struct cork_slice *slice2)
{
    if (slice1 == slice2) {
        return true;
    }

    if (slice1->size != slice2->size) {
        return false;
    }

    return (memcmp(slice1->buf, slice2->buf, slice1->size) == 0);
}
