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
#include "libcork/ds/managed-buffer.h"
#include "libcork/ds/slice.h"

cork_managed_buffer_t *
cork_managed_buffer_new(cork_allocator_t *alloc,
                        const void *buf, size_t size,
                        cork_managed_buffer_free_t free)
{
    /*
    DEBUG("Creating new cork_managed_buffer_t [%p:%zu], refcount now 1",
          buf, size);
    */

    cork_managed_buffer_t  *mbuf = cork_new(alloc, cork_managed_buffer_t);
    mbuf->buf = buf;
    mbuf->size = size;
    mbuf->free = free;
    mbuf->ref_count = 1;
    mbuf->alloc = alloc;
    return mbuf;
}


static void
cork_managed_buffer_free_copy(cork_managed_buffer_t *mbuf)
{
    cork_free(mbuf->alloc, (void *) mbuf->buf, mbuf->size);
    cork_delete(mbuf->alloc, cork_managed_buffer_t, mbuf);
}

cork_managed_buffer_t *
cork_managed_buffer_new_copy(cork_allocator_t *alloc,
                             const void *buf, size_t size)
{
    cork_managed_buffer_t  *result;
    void  *buf_copy = cork_malloc(alloc, size);
    if (buf_copy == NULL) {
        return NULL;
    }

    memcpy(buf_copy, buf, size);
    result = cork_managed_buffer_new(alloc, buf_copy, size,
                                     cork_managed_buffer_free_copy);
    if (result == NULL) {
        cork_free(alloc, buf_copy, size);
    }
    return result;
}


static void
cork_managed_buffer_free(cork_managed_buffer_t *mbuf)
{
    /*
    DEBUG("Freeing cork_managed_buffer_t [%p:%zu]", mbuf->buf, mbuf->size);
    */

    mbuf->free(mbuf);
}


cork_managed_buffer_t *
cork_managed_buffer_ref(cork_managed_buffer_t *mbuf)
{
    /*
    int  old_count = mbuf->ref_count++;
    DEBUG("Referencing cork_managed_buffer_t [%p:%zu], refcount now %d",
          mbuf->buf, mbuf->size, old_count + 1);
    */

    mbuf->ref_count++;
    return mbuf;
}


void
cork_managed_buffer_unref(cork_managed_buffer_t *mbuf)
{
    /*
    int  old_count = mbuf->ref_count--;
    DEBUG("Dereferencing cork_managed_buffer_t [%p:%zu], refcount now %d",
          mbuf->buf, mbuf->size, old_count - 1);
    */

    if (--mbuf->ref_count == 0) {
        cork_managed_buffer_free(mbuf);
    }
}


static cork_slice_iface_t  CORK_MANAGED_BUFFER__SLICE;

static void
cork_managed_buffer__slice_free(cork_slice_t *self)
{
    cork_managed_buffer_t  *mbuf = self->user_data;
    cork_managed_buffer_unref(mbuf);
}

static bool
cork_managed_buffer__slice_copy(cork_slice_t *self, cork_slice_t *dest,
                                size_t offset, size_t length)
{
    cork_managed_buffer_t  *mbuf = self->user_data;
    dest->buf = self->buf + offset;
    dest->size = length;
    dest->iface = &CORK_MANAGED_BUFFER__SLICE;
    dest->user_data = cork_managed_buffer_ref(mbuf);
    return true;
}

static cork_slice_iface_t  CORK_MANAGED_BUFFER__SLICE = {
    cork_managed_buffer__slice_free,
    cork_managed_buffer__slice_copy,
    NULL
};


bool
cork_managed_buffer_slice(cork_slice_t *dest,
                          cork_managed_buffer_t *buffer,
                          size_t offset, size_t length)
{
    if ((buffer != NULL) &&
        (offset < buffer->size) &&
        ((offset + length) <= buffer->size)) {
        /*
        DEBUG("Slicing [%p:%zu] at %zu:%zu, gives <%p:%zu>",
              buffer->buf, buffer->size,
              offset, length,
              buffer->buf + offset, length);
        */
        dest->buf = buffer->buf + offset;
        dest->size = length;
        dest->iface = &CORK_MANAGED_BUFFER__SLICE;
        dest->user_data = cork_managed_buffer_ref(buffer);
        return true;
    }

    else {
        /*
        DEBUG("Cannot slice [%p:%zu] at %zu:%zu",
              buffer->buf, buffer->size,
              offset, length);
        */
        cork_slice_clear(dest);
        return false;
    }
}


bool
cork_managed_buffer_slice_offset(cork_slice_t *dest,
                                 cork_managed_buffer_t *buffer,
                                 size_t offset)
{
    if (buffer == NULL) {
        cork_slice_clear(dest);
        return false;
    } else {
        return cork_managed_buffer_slice
            (dest, buffer, offset, buffer->size - offset);
    }
}
