/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_DS_BUFFER_H
#define LIBCORK_DS_BUFFER_H

/**
 * @file
 * @brief Implementation of the @ref buffer submodule
 */

#include <libcork/core/allocator.h>
#include <libcork/core/types.h>

#include <libcork/ds/managed-buffer.h>

/**
 * @addtogroup buffer
 *
 * <tt>#%include \<libcork/ds/buffer.h\></tt>
 *
 * This section defines a resizable binary buffer type.  This class can
 * also be used to construct C strings, when you don't know the size of
 * the string in advance.
 *
 * @note Note that this buffer class is not reference counted, and is
 * intended to be used by only one owner at a time.  If you need a
 * buffer class that can be shared among multiple owners, you need the
 * @ref managed_buffer section.
 *
 * @{
 */


/**
 * @brief A resizable binary buffer.
 *
 * This class is not reference counted; we assume that there's a single
 * owner of the buffer.  The contents of a cork_buffer_t are fully
 * mutable.  If you want to turn the buffer into something that's safe
 * to pass between threads, you can use the @ref cork_buffer_to_slice or
 * @ref cork_buffer_to_managed_buffer functions to create an immutable
 * managed wrapper around the buffer.
 *
 * You can read the contents of the buffer by accessing the @ref buf and
 * @ref size fields directly; however, you should only use the class's
 * method to modify the contents, since they take care of automatically
 * resizing the underlying buffer when necessary.
 *
 * @note Note that this class always creates its own copy of any data
 * added to the buffer; there aren't any methods for wrapping existing
 * buffers without copying.  If you want to do that, you need the @ref
 * cork_managed_buffer_t class.
 *
 * @since 0.1-dev
 */

typedef struct cork_buffer_t
{
    /** @brief The current contents of the buffer. */
    void  *buf;

    /** @brief The current size of the buffer. */
    size_t  size;

    /** @brief The amount of space allocated for @ref buf. @private */
    size_t  allocated_size;

    /** @brief The allocator to use to manage the internal buffer.
     * @private */
    cork_allocator_t  *alloc;
} cork_buffer_t;

/* end of buffer group */
/**
 * @}
 */


/**
 * @brief Initialize a new buffer.
 * @param [in] alloc  A custom allocator
 * @param [in] buffer  An uninitialized buffer
 * @public @memberof cork_buffer_t
 * @since 0.1-dev
 */

void
cork_buffer_init(cork_allocator_t *alloc, cork_buffer_t *buffer);

/**
 * @brief Allocate and initialize a new buffer.
 * @param [in] alloc  A custom allocator
 * @return A new buffer, or @c NULL if the buffer couldn't be allocated.
 * @public @memberof cork_buffer_t
 * @since 0.1-dev
 */

cork_buffer_t *
cork_buffer_new(cork_allocator_t *alloc);


/**
 * @brief Finalize a buffer, freeing any content that it contains.
 * @param [in] buffer  A buffer
 * @public @memberof cork_buffer_t
 * @since 0.1-dev
 */

void
cork_buffer_done(cork_buffer_t *buffer);

/**
 * @brief Finalize and deallocate a buffer, freeing any content that it
 * contains.
 * @param [in] buffer  A buffer
 * @public @memberof cork_buffer_t
 * @since 0.1-dev
 */

void
cork_buffer_free(cork_buffer_t *buffer);


/**
 * @brief Check two buffers for equality.
 * @param [in] buffer1  A buffer
 * @param [in] buffer2  A buffer
 * @returns Whether the contents of the two buffers are identical.
 * @public @memberof cork_buffer_t
 * @since 0.1-dev
 */

bool
cork_buffer_equal(const cork_buffer_t *buffer1, const cork_buffer_t *buffer2);


/**
 * @brief Ensure that a buffer's internal storage is at least a given
 * size.
 *
 * This function won't shrink the size of the buffer's internal storage;
 * if it's larger then @a size, then we don't do anything.
 *
 * @param [in] buffer  A buffer
 * @param [in] desired_size  The desired size of the buffer's internal
 * storage
 *
 * @returns @c true if we can ensure that the internal storage is at
 * least as big as @a size; @c false otherwise.
 *
 * @public @memberof cork_buffer_t
 * @since 0.1-dev
 */

bool
cork_buffer_ensure_size(cork_buffer_t *buffer, size_t desired_size);


/**
 * @brief Copy the contents of the given byte array into a buffer.
 *
 * The buffer's internal storage is reallocated, if necessary, to ensure
 * that it's large enough to hold the contents of @a src.
 *
 * We will add a NUL byte after the contents of @a src in memory, but
 * this NUL byte won't be included in the @ref size of the buffer.
 *
 * @param [in] buffer  A buffer
 * @param [in] src  An external byte array to copy
 * @param [in] length  The size of @a src
 *
 * @returns @c true if the array is successfully copied into the buffer;
 * @c false otherwise.
 *
 * @public @memberof cork_buffer_t
 * @since 0.1-dev
 */

bool
cork_buffer_set(cork_buffer_t *buffer, const void *src, size_t length);


/**
 * @brief Copy the contents of a C string into a buffer.
 *
 * The buffer's internal storage is reallocated, if necessary, to ensure
 * that it's large enough to hold the contents of @a str.
 *
 * @param [in] buffer  A buffer
 * @param [in] str  An external string to copy
 *
 * @returns @c true if the string is successfully copied into the buffer;
 * @c false otherwise.
 *
 * @public @memberof cork_buffer_t
 * @since 0.1-dev
 */

bool
cork_buffer_set_string(cork_buffer_t *buffer, const char *str);


/**
 * @brief Create a new cork_managed_buffer_t to manage the contents of a
 * buffer.
 *
 * @a buffer must have been allocated on the heap (i.e., using
 * @ref cork_buffer_new, not @ref cork_buffer_init).
 *
 * We take ownership of @a buffer, regardless of whether we're able to
 * successfully create a new cork_managed_buffer_t instance.  You should
 * @b not try to free the buffer yourself.
 *
 * @param [in] buffer  A buffer
 *
 * @returns A new managed buffer, or @c NULL if the managed buffer
 * couldn't be allocated.
 *
 * @public @memberof cork_buffer_t
 * @since 0.1-dev
 */

cork_managed_buffer_t *
cork_buffer_to_managed_buffer(cork_buffer_t *buffer);


/**
 * @brief Initialize a new cork_slice_t to manage the contents of a
 * buffer.
 *
 * @a buffer must have been allocated on the heap (i.e., using
 * @ref cork_buffer_new, not @ref cork_buffer_init).
 *
 * We take ownership of @a buffer, regardless of whether we're able to
 * successfully create a new cork_slice_t instance.  You should @b not
 * try to free the buffer yourself.
 *
 * The slice points into the contents of a new managed buffer instance.
 * The managed buffer isn't returned directly, though you can create
 * additional slices into it using the usual cork_slice_t methods.
 *
 * Regardless of whether we can initialize the slice successfully, you
 * @b must ensure that you call @ref cork_slice_finish when you are done
 * with the slice.
 *
 * @param [in] buffer  A buffer
 * @param [in] slice  The slice to initialize
 *
 * @returns @c true if we can initialize the new slice; @c false
 * otherwise.
 *
 * @public @memberof cork_buffer_t
 * @since 0.1-dev
 */

bool
cork_buffer_to_slice(cork_buffer_t *buffer, cork_slice_t *slice);


#endif /* LIBCORK_DS_BUFFER_H */
