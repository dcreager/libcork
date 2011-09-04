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


struct cork_managed_buffer_wrapped {
    struct cork_managed_buffer  parent;
    struct cork_alloc  *alloc;
    void  *buf;
    size_t  size;
    cork_managed_buffer_freer  free;
};

static void
cork_managed_buffer_wrapped__free(struct cork_managed_buffer *vself)
{
    struct cork_managed_buffer_wrapped  *self =
        cork_container_of(vself, struct cork_managed_buffer_wrapped, parent);
    self->free(self->alloc, self->buf, self->size);
    cork_delete(self->alloc, struct cork_managed_buffer_wrapped, self);
}

static struct cork_managed_buffer_iface  CORK_MANAGED_BUFFER_WRAPPED = {
    cork_managed_buffer_wrapped__free
};

struct cork_managed_buffer *
cork_managed_buffer_new(struct cork_alloc *alloc,
                        const void *buf, size_t size,
                        cork_managed_buffer_freer free)
{
    /*
    DEBUG("Creating new struct cork_managed_buffer [%p:%zu], refcount now 1",
          buf, size);
    */

    struct cork_managed_buffer_wrapped  *self =
        cork_new(alloc, struct cork_managed_buffer_wrapped);
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


struct cork_managed_buffer_copied {
    struct cork_managed_buffer  parent;
    struct cork_alloc  *alloc;
    size_t  allocated_size;
};

#define cork_managed_buffer_copied_data(self) \
    (((void *) (self)) + sizeof(struct cork_managed_buffer_copied))

#define cork_managed_buffer_copied_sizeof(sz) \
    ((sz) + sizeof(struct cork_managed_buffer_copied))

static void
cork_managed_buffer_copied__free(struct cork_managed_buffer *vself)
{
    struct cork_managed_buffer_copied  *self =
        cork_container_of(vself, struct cork_managed_buffer_copied, parent);
    cork_free(self->alloc, self, self->allocated_size);
}

static struct cork_managed_buffer_iface  CORK_MANAGED_BUFFER_COPIED = {
    cork_managed_buffer_copied__free
};

struct cork_managed_buffer *
cork_managed_buffer_new_copy(struct cork_alloc *alloc,
                             const void *buf, size_t size)
{
    size_t  allocated_size = cork_managed_buffer_copied_sizeof(size);
    struct cork_managed_buffer_copied  *self = cork_malloc(alloc, allocated_size);
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
cork_managed_buffer_free(struct cork_managed_buffer *self)
{
    /*
    DEBUG("Freeing struct cork_managed_buffer [%p:%zu]", self->buf, self->size);
    */

    self->iface->free(self);
}


struct cork_managed_buffer *
cork_managed_buffer_ref(struct cork_managed_buffer *self)
{
    /*
    int  old_count = self->ref_count++;
    DEBUG("Referencing struct cork_managed_buffer [%p:%zu], refcount now %d",
          self->buf, self->size, old_count + 1);
    */

    self->ref_count++;
    return self;
}


void
cork_managed_buffer_unref(struct cork_managed_buffer *self)
{
    /*
    int  old_count = self->ref_count--;
    DEBUG("Dereferencing struct cork_managed_buffer [%p:%zu], refcount now %d",
          self->buf, self->size, old_count - 1);
    */

    if (--self->ref_count == 0) {
        cork_managed_buffer_free(self);
    }
}


static struct cork_slice_iface  CORK_MANAGED_BUFFER__SLICE;

static void
cork_managed_buffer__slice_free(struct cork_slice *self)
{
    struct cork_managed_buffer  *mbuf = self->user_data;
    cork_managed_buffer_unref(mbuf);
}

static bool
cork_managed_buffer__slice_copy(struct cork_slice *self,
                                struct cork_slice *dest,
                                size_t offset, size_t length)
{
    struct cork_managed_buffer  *mbuf = self->user_data;
    dest->buf = self->buf + offset;
    dest->size = length;
    dest->iface = &CORK_MANAGED_BUFFER__SLICE;
    dest->user_data = cork_managed_buffer_ref(mbuf);
    return true;
}

static struct cork_slice_iface  CORK_MANAGED_BUFFER__SLICE = {
    cork_managed_buffer__slice_free,
    cork_managed_buffer__slice_copy,
    NULL
};


bool
cork_managed_buffer_slice(struct cork_slice *dest,
                          struct cork_managed_buffer *buffer,
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
cork_managed_buffer_slice_offset(struct cork_slice *dest,
                                 struct cork_managed_buffer *buffer,
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
