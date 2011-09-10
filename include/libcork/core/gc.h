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

/**
 * @file
 * @brief Implementation of the @ref gc submodule
 */

#include <libcork/core/allocator.h>
#include <libcork/core/types.h>

/**
 * @addtogroup gc
 *
 * <tt>#%include \<libcork/gc/refcount.h\></tt>
 *
 * The functions in this section provide garbage collection contexts
 * based on reference counting.  The garbage collector handles reference
 * cycles correctly.  It is <em>not</em> a conservative garbage
 * collector — i.e., we don't assume that every word inside an object
 * might be a pointer.  Instead, each garbage-collected object must
 * provide a function that knows how to recurse into any child objects
 * that it references.
 *
 * The garbage collector defined in this module is <em>not</em>
 * thread-safe.  You should use a single collector per thread.  You can
 * migrate an object from one collector to another, assuming they use
 * the same underlying @ref cork_alloc allocator, using the @ref
 * cork_gc_migrate function.
 *
 * The garbage collection implementation is based on the algorithm
 * describedin §3 of:
 * - Bacon, DF and Rajan, VT.  <i>Concurrent cycle collection in
 *   reference counted systems</i>.  Proc. ECOOP 2001.  LNCS 2072.
 *
 * @{
 */

/**
 * @brief An internal structure allocated with every garbage-collected
 * object.
 * @since 0.2-dev
 */

struct cork_gc_header;

/**
 * @brief A garbage collector context.
 * @since 0.2-dev
 */

struct cork_gc {
    /** @brief The allocator used to allocate objects created by this
     * garbage collector @private */
    struct cork_alloc  *alloc;

    /** @brief The possible roots of garbage cycles @private */
    struct cork_gc_header  **roots;

    /** @brief The number of used entries in @ref roots @private */
    size_t  root_count;
};

/**
 * @brief A callback for recursing through the children of a
 * garbage-collected object.
 * @since 0.2-dev
 */

typedef void
(*cork_gc_recurser)(void *obj, void *ud);

/**
 * @brief An interface that each garbage-collected object must
 * implement.
 * @since 0.2-dev
 */

struct cork_gc_obj_iface {
    /**
     * @brief Free a garbage-collected object once its reference count
     * drops to zero.
     *
     * @note You should @b not decrement the references to any child
     * objects; this will be taken care of for you by the garbage
     * collector.
     *
     * This entry can be @c NULL if there's no additional finalization
     * needed.
     *
     * @since 0.2-dev
     */
    void
    (*free)(struct cork_alloc *alloc, void *obj);

    /**
     * @brief Inform the garbage collector of any child reference in an
     * object.
     *
     * This function will be called by several garbage collection
     * operations that need to recurse into a graph of object
     * references.  You should call @a recurser with each child object
     * that is also garbage collected.  For instance, given a tree
     * object, this method might be defined as:
     *
     * @code
     * static void
     * tree_recurser(void *vself, cork_gc_recurser recurse, void *ud)
     * {
     *     struct tree  *self = vself;
     *     recurse(self->left, ud);
     *     recurse(self->right, ud);
     * }
     * @endcode
     *
     * Note that it's fine to call @a recurser with a @c NULL child
     * pointer; this makes it slightly easier to write this method.
     *
     * @since 0.2-dev
     */
    void
    (*recurse)(void *self, cork_gc_recurser recurser, void *ud);
};


/**
 * @brief Initialize a new garbage collection context.
 * @param [in] gc  A garbage collection context
 * @param [in] alloc  The allocator to use to create and free objects in
 * this context
 * @since 0.2-dev
 */
int
cork_gc_init(struct cork_gc *gc, struct cork_alloc *alloc);

/**
 * @brief Finalize a garbage collection context.
 *
 * @note This also frees any objects still owned by this context.
 *
 * @param [in] gc  A garbage collection context
 * @since 0.2-dev
 */
void
cork_gc_done(struct cork_gc *gc);


/**
 * @brief Allocate a new garbage collected object.
 *
 * @param [in] gc  A garbage collection context
 * @param [in] instance_size  The size of the garbage collected object
 * @param [in] iface  The interface that the garbage collector will use
 * to introspect the object
 * @returns A pointer to the new garbage collected object, or @c NULL if
 * we couldn't allocate a new instance.
 * @since 0.2-dev
 */
void *
cork_gc_alloc(struct cork_gc *gc, size_t instance_size,
              struct cork_gc_obj_iface *iface);

/**
 * @brief Allocate a new garbage collected object of the given type.
 *
 * @param [in] gc  A garbage collection context
 * @param [in] obj_type  The type of object to allocate
 * @param [in] iface  The interface that the garbage collector will use
 * to introspect the object
 * @returns A pointer to the new garbage collected object, or @c NULL if
 * we couldn't allocate a new instance.
 * @since 0.2-dev
 */
#if defined(CORK_DOCUMENTATION)
obj_type *
cork_gc_new(struct cork_gc *gc, TYPE obj_type,
            struct cork_gc_obj_iface *iface);
#else
#define cork_gc_new(gc, obj_type, iface) \
    ((obj_type *) \
     (cork_gc_alloc((gc), sizeof(obj_type), (iface))))
#endif


/**
 * @brief Increment the reference count of a garbage collected object.
 * @param [in] obj  A garbage collected object
 * @since 0.2-dev
 */
void *
cork_gc_incref(struct cork_gc *gc, void *obj);


/**
 * @brief Decrement the reference count of a garbage collected object.
 * @param [in] obj  A garbage collected object
 * @since 0.2-dev
 */
void
cork_gc_decref(struct cork_gc *gc, void *obj);


/* end of gc group */
/**
 * @}
 */


#endif /* LIBCORK_GC_REFCOUNT_H */
