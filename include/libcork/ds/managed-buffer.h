/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_DS_MANAGED_BUFFER_H
#define LIBCORK_DS_MANAGED_BUFFER_H

/**
 * @file
 * @brief Implementation of the @ref managed_buffer submodule
 */

#include <libcork/core/allocator.h>
#include <libcork/core/types.h>
#include <libcork/ds/slice.h>

/**
 * @addtogroup managed_buffer
 *
 * <tt>#%include \<libcork/ds/managed-buffer.h\></tt>
 *
 * This section defines an interface for handling reference-counted
 * binary buffers.  The @ref cork_managed_buffer_t type wraps a buffer
 * with a simple reference count, and takes care of freeing the
 * necessary resources when the reference count drops to zero.  There
 * should only be a single instance of @ref cork_managed_buffer_t for
 * any given buffer, regardless of how many threads or functions access
 * that buffer.  Each thread or function that uses the buffer does so
 * via a @ref cork_slice_t instance.  This type is meant to be allocated
 * directly on the stack (or in some other managed storage), and keeps a
 * pointer to the @ref cork_managed_buffer_t instance that it slices.
 * As its name implies, a slice can refer to a subset of the buffer.
 *
 * @{
 */

/*-----------------------------------------------------------------------
 * Managed buffers
 */

typedef struct cork_managed_buffer_t  cork_managed_buffer_t;

/**
 * @brief A finalization function for a managed buffer.
 * @since 0.1-dev
 */

typedef void
(*cork_managed_buffer_free_t)(cork_managed_buffer_t *buf);

/**
 * @brief A “managed buffer”, which wraps a buffer with a simple
 * reference count.
 *
 * We take care of freeing the underlying buffer when the reference
 * count drops to zero.
 *
 * There should only be a single instance of @ref cork_managed_buffer_t
 * for any given buffer, regardless of how many threads access that
 * buffer.  One implication of this is that you will not use this class
 * to access the data contained within the buffer.  Instead, each thread
 * or function that accesses data within the buffer will have its own
 * @ref cork_slice_t referring to this buffer.
 *
 * @since 0.1-dev
 */

struct cork_managed_buffer_t
{
    /** @brief The buffer that this instance manages */
    const void  *buf;

    /** @brief The size of @a buf */
    size_t  size;

    /**
     * @brief The function that will be called when the buffer is no
     * longer needed.
     *
     * This function must also free the @c cork_managed_buffer_t
     * instance itself.
     */
    cork_managed_buffer_free_t  free;

    /**
     * @brief A reference count for the buffer.
     *
     * If this drops to 0, the buffer will be finalized.
     */
    volatile int  ref_count;

    /** @brief The allocator uses to create this object */
    cork_allocator_t  *alloc;
};

/* end of managed_buffer group */
/**
 * @}
 */


/**
 * @brief Allocate a new @c cork_managed_buffer_t to manage a copy of a
 * buffer.
 *
 * @param [in] alloc  A custom allocator
 * @param [in] buf  The buffer to copy and manage
 * @param [in] size  The size of @a buf
 *
 * @returns A new buffer, or @c NULL if the buffer couldn't be
 * allocated.
 *
 * @public @memberof cork_managed_buffer_t
 * @since 0.1-dev
 */

cork_managed_buffer_t *
cork_managed_buffer_new_copy(cork_allocator_t *alloc,
                             const void *buf, size_t size);


/**
 * @brief Allocate a new @c cork_managed_buffer_t to manage an existing
 * buffer.
 *
 * The buffer is not copied; when the buffer is no longer needed, the @a
 * free function will be called to finalize it.  This is a helper
 * function for when you don't need to store any additional state in the
 * managed buffer object.
 *
 * @note The @a free function is also responsible for freeing the @c
 * cork_managed_buffer_t instance itself.
 *
 * @param [in] alloc  A custom allocator
 * @param [in] buf  The buffer to manage
 * @param [in] size  The size of @a buf
 * @param [in] free  The function to invoke when the buffer is no longer
 * needed
 *
 * @returns A new buffer, or @c NULL if the buffer couldn't be
 * allocated.
 *
 * @public @memberof cork_managed_buffer_t
 * @since 0.1-dev
 */

cork_managed_buffer_t *
cork_managed_buffer_new(cork_allocator_t *alloc,
                        const void *buf, size_t size,
                        cork_managed_buffer_free_t free);


/**
 * @brief Atomically increase the reference count of a managed buffer.
 * This function is thread-safe.
 * @param [in] buf  A managed buffer
 * @returns A reference to the managed buffer.
 * @public @memberof cork_managed_buffer_t
 * @since 0.1-dev
 */

cork_managed_buffer_t *
cork_managed_buffer_ref(cork_managed_buffer_t *buf);


/**
 * @brief Atomically decrease the reference count of a managed buffer.
 * If the reference count falls to 0, the instance is freed.  This
 * function is thread-safe.
 * @param [in] buf  A managed buffer
 * @public @memberof cork_managed_buffer_t
 * @since 0.1-dev
 */

void
cork_managed_buffer_unref(cork_managed_buffer_t *buf);


/**
 * @brief Initialize a new slice that refers to a subset of a managed
 * buffer.
 *
 * The @a offset and @a length parameters identify the subset.  If these
 * parameters don't refer to a valid portion of the buffer, we return @c
 * false, and you must not try to deference the slice's @a buf pointer.
 * If the slice is valid, we return @c true.
 *
 * Regardless of whether the new slice is valid, you @b must ensure that
 * you call @ref cork_slice_finish when you are done with the slice.
 *
 * @param [in] dest  The slice to initialize
 * @param [in] buffer  The managed buffer to slice
 * @param [in] offset  The offset into the buffer where the new slice
 * should begin
 * @param [in] length  The desired length of the new slice
 *
 * @returns @c true if @a buffer, @a offset, and @a length refer to a
 * valid portion of a managed buffer; @c false otherwise.
 *
 * @public @memberof cork_managed_buffer_t
 * @since 0.1-dev
 */

bool
cork_managed_buffer_slice(cork_slice_t *dest,
                          cork_managed_buffer_t *buffer,
                          size_t offset, size_t length);


/**
 * @brief Initialize a new slice that refers to the subset managed
 * buffer beginning from the given offset.
 *
 * This is equivalent to calling @ref cork_managed_buffer_slice with a
 * @a length of <code>buffer-\>size - offset</code>.
 *
 * Regardless of whether the new slice is valid, you @b must ensure that
 * you call @ref cork_slice_finish when you are done with the slice.
 *
 * @param [in] dest  The slice to initialize
 * @param [in] buffer  The managed buffer to slice
 * @param [in] offset  The offset into the buffer where the new slice
 * should begin
 *
 * @returns @c true if @a buffer and @a offset refer to a valid portion
 * of a managed buffer; @c false otherwise.
 *
 * @public @memberof cork_managed_buffer_t
 * @since 0.1-dev
 */

bool
cork_managed_buffer_slice_offset(cork_slice_t *dest,
                                 cork_managed_buffer_t *buffer,
                                 size_t offset);


#endif /* LIBCORK_DS_MANAGED_BUFFER_H */
