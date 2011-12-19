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
#include "libcork/core/checkers.h"
#include "libcork/core/error.h"
#include "libcork/core/types.h"
#include "libcork/ds/managed-buffer.h"
#include "libcork/ds/slice.h"


struct cork_managed_buffer_wrapped {
    struct cork_managed_buffer  parent;
    void  *buf;
    size_t  size;
    cork_managed_buffer_freer  free;
};

static void
cork_managed_buffer_wrapped__free(struct cork_alloc *alloc,
                                  struct cork_managed_buffer *vself)
{
    struct cork_managed_buffer_wrapped  *self =
        cork_container_of(vself, struct cork_managed_buffer_wrapped, parent);
    self->free(alloc, self->buf, self->size);
    cork_delete(alloc, struct cork_managed_buffer_wrapped, self);
}

static struct cork_managed_buffer_iface  CORK_MANAGED_BUFFER_WRAPPED = {
    cork_managed_buffer_wrapped__free
};

struct cork_managed_buffer *
cork_managed_buffer_new(struct cork_alloc *alloc,
                        const void *buf, size_t size,
                        cork_managed_buffer_freer free,
                        struct cork_error *err)
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
    self->buf = (void *) buf;
    self->size = size;
    self->free = free;
    return &self->parent;
}


struct cork_managed_buffer_copied {
    struct cork_managed_buffer  parent;
    size_t  allocated_size;
};

#define cork_managed_buffer_copied_data(self) \
    (((void *) (self)) + sizeof(struct cork_managed_buffer_copied))

#define cork_managed_buffer_copied_sizeof(sz) \
    ((sz) + sizeof(struct cork_managed_buffer_copied))

static void
cork_managed_buffer_copied__free(struct cork_alloc *alloc,
                                 struct cork_managed_buffer *vself)
{
    struct cork_managed_buffer_copied  *self =
        cork_container_of(vself, struct cork_managed_buffer_copied, parent);
    cork_free(alloc, self, self->allocated_size);
}

static struct cork_managed_buffer_iface  CORK_MANAGED_BUFFER_COPIED = {
    cork_managed_buffer_copied__free
};

struct cork_managed_buffer *
cork_managed_buffer_new_copy(struct cork_alloc *alloc,
                             const void *buf, size_t size,
                             struct cork_error *err)
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
    self->allocated_size = allocated_size;
    memcpy((void *) self->parent.buf, buf, size);
    return &self->parent;
}


static void
cork_managed_buffer_free(struct cork_alloc *alloc,
                         struct cork_managed_buffer *self)
{
    /*
    DEBUG("Freeing struct cork_managed_buffer [%p:%zu]", self->buf, self->size);
    */

    self->iface->free(alloc, self);
}


struct cork_managed_buffer *
cork_managed_buffer_ref(struct cork_alloc *alloc,
                        struct cork_managed_buffer *self)
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
cork_managed_buffer_unref(struct cork_alloc *alloc,
                          struct cork_managed_buffer *self)
{
    /*
    int  old_count = self->ref_count--;
    DEBUG("Dereferencing struct cork_managed_buffer [%p:%zu], refcount now %d",
          self->buf, self->size, old_count - 1);
    */

    if (--self->ref_count == 0) {
        cork_managed_buffer_free(alloc, self);
    }
}


static struct cork_slice_iface  CORK_MANAGED_BUFFER__SLICE;

static void
cork_managed_buffer__slice_free(struct cork_alloc *alloc,
                                struct cork_slice *self)
{
    struct cork_managed_buffer  *mbuf = self->user_data;
    cork_managed_buffer_unref(alloc, mbuf);
}

static int
cork_managed_buffer__slice_copy(struct cork_alloc *alloc,
                                struct cork_slice *self,
                                struct cork_slice *dest,
                                size_t offset, size_t length,
                                struct cork_error *err)
{
    struct cork_managed_buffer  *mbuf = self->user_data;
    dest->buf = self->buf + offset;
    dest->size = length;
    dest->iface = &CORK_MANAGED_BUFFER__SLICE;
    dest->user_data = cork_managed_buffer_ref(alloc, mbuf);
    return 0;
}

static struct cork_slice_iface  CORK_MANAGED_BUFFER__SLICE = {
    cork_managed_buffer__slice_free,
    cork_managed_buffer__slice_copy,
    NULL
};


int
cork_managed_buffer_slice(struct cork_alloc *alloc,
                          struct cork_slice *dest,
                          struct cork_managed_buffer *buffer,
                          size_t offset, size_t length,
                          struct cork_error *err)
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
        dest->user_data = cork_managed_buffer_ref(alloc, buffer);
        return 0;
    }

    else {
        /*
        DEBUG("Cannot slice [%p:%zu] at %zu:%zu",
              buffer->buf, buffer->size,
              offset, length);
        */
        cork_slice_clear(alloc, dest);
        cork_slice_invalid_slice_set(alloc, err, 0, offset, 0);
        return -1;
    }
}


int
cork_managed_buffer_slice_offset(struct cork_alloc *alloc,
                                 struct cork_slice *dest,
                                 struct cork_managed_buffer *buffer,
                                 size_t offset,
                                 struct cork_error *err)
{
    if (buffer == NULL) {
        cork_slice_clear(alloc, dest);
        cork_slice_invalid_slice_set(alloc, err, 0, offset, 0);
        return -1;
    } else {
        return cork_managed_buffer_slice
            (alloc, dest, buffer, offset, buffer->size - offset, err);
    }
}
