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

#include "libcork/core/types.h"
#include "libcork/ds/slice.h"


void
cork_slice_clear(cork_slice_t *slice)
{
    slice->buf = NULL;
    slice->size = 0;
    slice->iface = NULL;
    slice->user_data = NULL;
}


bool
cork_slice_copy(cork_slice_t *dest,
                cork_slice_t *slice,
                size_t offset, size_t length)
{
    if ((slice != NULL) &&
        (offset < slice->size) &&
        ((offset + length) <= slice->size)) {
        /*
        DEBUG("Slicing <%p:%zu> at %zu:%zu, gives <%p:%zu>",
              slice->buf, slice->size,
              offset, length,
              slice->buf + offset, length);
        */
        return slice->iface->copy(slice, dest, offset, length);
    }

    else {
        /*
        DEBUG("Cannot slice <%p:%zu> at %zu:%zu",
              slice->buf, slice->size,
              offset, length);
        */
        cork_slice_clear(dest);
        return false;
    }
}


bool
cork_slice_copy_offset(cork_slice_t *dest,
                       cork_slice_t *slice,
                       size_t offset)
{
    if (slice == NULL) {
        cork_slice_clear(dest);
        return false;
    } else {
        return cork_slice_copy
            (dest, slice, offset, slice->size - offset);
    }
}


bool
cork_slice_slice(cork_slice_t *slice,
                 size_t offset, size_t length)
{
    if ((slice != NULL) &&
        (offset < slice->size) &&
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
            return true;
        } else {
            return slice->iface->slice(slice, offset, length);
        }
    }

    else {
        /*
        DEBUG("Cannot slice <%p:%zu> at %zu:%zu",
              slice->buf, slice->size,
              offset, length);
        */
        return false;
    }
}


bool
cork_slice_slice_offset(cork_slice_t *slice,
                        size_t offset)
{
    if (slice == NULL) {
        return false;
    } else {
        return cork_slice_slice
            (slice, offset, slice->size - offset);
    }
}


void
cork_slice_finish(cork_slice_t *slice)
{
    /*
    DEBUG("Finalizing <%p:%zu>", dest->buf, dest->size);
    */

    if (slice->iface->free != NULL) {
        slice->iface->free(slice);
    }

    cork_slice_clear(slice);
}


bool
cork_slice_equal(const cork_slice_t *slice1,
                 const cork_slice_t *slice2)
{
    if (slice1 == slice2) {
        return true;
    }

    if (slice1->size != slice2->size) {
        return false;
    }

    return (memcmp(slice1->buf, slice2->buf, slice1->size) == 0);
}
