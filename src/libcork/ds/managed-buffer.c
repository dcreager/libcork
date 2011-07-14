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


typedef struct cork_managed_buffer_wrapped_t
{
    cork_managed_buffer_t  parent;
    cork_allocator_t  *alloc;
    void  *buf;
    size_t  size;
    cork_managed_buffer_free_t  free;
} cork_managed_buffer_wrapped_t;

static void
cork_managed_buffer_wrapped__free(cork_managed_buffer_t *vself)
{
    cork_managed_buffer_wrapped_t  *self =
        cork_container_of(vself, cork_managed_buffer_wrapped_t, parent);
    self->free(self->alloc, self->buf, self->size);
    cork_delete(self->alloc, cork_managed_buffer_wrapped_t, self);
}

static cork_managed_buffer_iface_t  CORK_MANAGED_BUFFER_WRAPPED = {
    cork_managed_buffer_wrapped__free
};

cork_managed_buffer_t *
cork_managed_buffer_new(cork_allocator_t *alloc,
                        const void *buf, size_t size,
                        cork_managed_buffer_free_t free)
{
    /*
    DEBUG("Creating new cork_managed_buffer_t [%p:%zu], refcount now 1",
          buf, size);
    */

    cork_managed_buffer_wrapped_t  *self =
        cork_new(alloc, cork_managed_buffer_wrapped_t);
    self->parent.buf = buf;
    self->parent.size = size;
    self->parent.ref_count = 1;
    self->parent.iface = &CORK_MANAGED_BUFFER_WRAPPED;
    self->alloc = alloc;
    self->buf = (void *) buf;
    self->size = size;
    self->free = free;
    return &self->parent;
}


typedef struct cork_managed_buffer_copied_t
{
    cork_managed_buffer_t  parent;
    cork_allocator_t  *alloc;
    size_t  allocated_size;
} cork_managed_buffer_copied_t;

#define cork_managed_buffer_copied_data(self) \
    (((void *) (self)) + sizeof(cork_managed_buffer_copied_t))

#define cork_managed_buffer_copied_sizeof(sz) \
    ((sz) + sizeof(cork_managed_buffer_copied_t))

static void
cork_managed_buffer_copied__free(cork_managed_buffer_t *vself)
{
    cork_managed_buffer_copied_t  *self =
        cork_container_of(vself, cork_managed_buffer_copied_t, parent);
    cork_free(self->alloc, self, self->allocated_size);
}

static cork_managed_buffer_iface_t  CORK_MANAGED_BUFFER_COPIED = {
    cork_managed_buffer_copied__free
};

cork_managed_buffer_t *
cork_managed_buffer_new_copy(cork_allocator_t *alloc,
                             const void *buf, size_t size)
{
    size_t  allocated_size = cork_managed_buffer_copied_sizeof(size);
    cork_managed_buffer_copied_t  *self = cork_malloc(alloc, allocated_size);
    if (self == NULL) {
        return NULL;
    }

    self->parent.buf = cork_managed_buffer_copied_data(self);
    self->parent.size = size;
    self->parent.ref_count = 1;
    self->parent.iface = &CORK_MANAGED_BUFFER_COPIED;
    self->alloc = alloc;
    self->allocated_size = allocated_size;
    memcpy((void *) self->parent.buf, buf, size);
    return &self->parent;
}


static void
cork_managed_buffer_free(cork_managed_buffer_t *self)
{
    /*
    DEBUG("Freeing cork_managed_buffer_t [%p:%zu]", self->buf, self->size);
    */

    self->iface->free(self);
}


cork_managed_buffer_t *
cork_managed_buffer_ref(cork_managed_buffer_t *self)
{
    /*
    int  old_count = self->ref_count++;
    DEBUG("Referencing cork_managed_buffer_t [%p:%zu], refcount now %d",
          self->buf, self->size, old_count + 1);
    */

    self->ref_count++;
    return self;
}


void
cork_managed_buffer_unref(cork_managed_buffer_t *self)
{
    /*
    int  old_count = self->ref_count--;
    DEBUG("Dereferencing cork_managed_buffer_t [%p:%zu], refcount now %d",
          self->buf, self->size, old_count - 1);
    */

    if (--self->ref_count == 0) {
        cork_managed_buffer_free(self);
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
