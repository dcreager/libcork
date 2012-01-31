/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_GC_REFCOUNT_H
#define LIBCORK_GC_REFCOUNT_H


#include <libcork/core/types.h>


/* An internal structure allocated with every garbage-collected object. */
struct cork_gc_header;

/* A garbage collector context. */
struct cork_gc {
    /* The possible roots of garbage cycles.  This is a fixed-size
     * array. */
    struct cork_gc_header  **roots;
    /* The number of used entries in roots. */
    size_t  root_count;
};


/* A callback for recursing through the children of a garbage-collected
 * object. */
typedef void
(*cork_gc_recurser)(struct cork_gc *gc, void *obj, void *ud);

/* An interface that each garbage-collected object must implement. */
struct cork_gc_obj_iface {
    /* Perform additional cleanup; does *NOT* need to deallocate the
     * object itself, or release any child references */
    void
    (*free)(struct cork_gc *gc, void *obj);

    void
    (*recurse)(struct cork_gc *gc, void *self,
               cork_gc_recurser recurser, void *ud);
};


int
cork_gc_init(struct cork_gc *gc);

void
cork_gc_done(struct cork_gc *gc);


void *
cork_gc_alloc(struct cork_gc *gc, size_t instance_size,
              struct cork_gc_obj_iface *iface);

#define cork_gc_new(gc, obj_type, iface) \
    ((obj_type *) \
     (cork_gc_alloc((gc), sizeof(obj_type), (iface))))


void *
cork_gc_incref(struct cork_gc *gc, void *obj);

void
cork_gc_decref(struct cork_gc *gc, void *obj);


#endif /* LIBCORK_GC_REFCOUNT_H */
