/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_DS_SLICE_H
#define LIBCORK_DS_SLICE_H

/**
 * @file
 * @brief Implementation of the @ref slice submodule
 */

#include <libcork/core/allocator.h>
#include <libcork/core/types.h>

/**
 * @addtogroup slice
 *
 * <tt>#%include \<libcork/ds/slice.h\></tt>
 *
 * This section defines an interface for safely referring to the
 * contents of a binary buffer, without needing to know where the buffer
 * came from.  In addition to accessing the contents of the underlying
 * buffer, slices support three operations:
 *
 * <ul>
 *   <li><i>Copying</i> initializes a new slice object to point at the
 *   same underlying buffer as the current slice.  Depending on how the
 *   underlying buffer is implemented, this doesn't necessarily involve
 *   actual copying; for instance, the @ref cork_managed_buffer_t type
 *   implements this operation by incrementing the reference count of
 *   the managed buffer.</li>
 *
 *   <li><i>Slicing</i> updates the current slice to point at a subset
 *   of its current contents.  This doesn't affect the underlying
 *   buffer.</li>
 *
 *   <li><i>Freeing</i> releases the resources used by the slice,
 *   possibly freeing the underlying buffer.</li>
 * </ul>
 *
 * @{
 */


/*-----------------------------------------------------------------------
 * Slices
 */

typedef struct cork_slice_t  cork_slice_t;

/**
 * @brief The interface of methods that slice implementations must
 * provide.
 * @since 0.1-dev
 */

typedef struct cork_slice_iface_t
{
    /**
     * @brief Free the slice.
     *
     * This function pointer can be @c NULL if you don't need to free
     * any underlying buffer.
     */

    void
    (*free)(cork_slice_t *self);

    /**
     * @brief Create a copy of a slice.
     *
     * You can assume that @a offset and @a length refer to a valid
     * subset of the buffer; @ref cork_slice_slice will verify this
     * before calling your method implementation.
     */

    bool
    (*copy)(cork_slice_t *self, cork_slice_t *dest,
            size_t offset, size_t length);

    /**
     * @brief Update the current slice to point at a different subset.
     *
     * You can assume that @a offset and @a length refer to a valid
     * subset of the buffer; @ref cork_slice_slice will verify this
     * before calling your method implementation.
     *
     * This function pointer can be @c NULL if you don't need to do
     * anything special to the underlying buffer; in this case, @ref
     * cork_slice_slice will update the slice's @a buf and @a size
     * fields for you.
     */

    bool
    (*slice)(cork_slice_t *self, size_t offset, size_t length);
} cork_slice_iface_t;


/**
 * @brief A specific window into a portion of an underlying buffer.
 *
 * Note that this class does not need to be allocated on the heap; you
 * can define an instance directly on the stack, or in some other kind
 * of managed storage.
 *
 * Also note that it is <b>very important</b> that you ensure that @ref
 * cork_slice_finish is called whenever you are done with a slice — if
 * you don't, there's a good chance that the underlying buffer will
 * never be freed.  Yes, yes, it's unfortunate that C doesn't have
 * try/finally or RAII, but suck it up and make sure that @ref
 * cork_slice_finish gets called.
 *
 * @since 0.1-dev
 */

struct cork_slice_t
{
    /**
     * @brief The beginning of the sliced portion of the buffer.
     */
    const void  *buf;

    /**
     * @brief The length of the sliced portion of the buffer.
     */
    size_t  size;

    /**
     * @brief The slice implementation of the underlying buffer.
     * @private
     */
    cork_slice_iface_t  *iface;

    /**
     * @brief An opaque pointer used by the slice implementation to
     * refer to the underlying buffer.
     * @private
     */
    void  *user_data;
};

/* end of slice group */
/**
 * @}
 */


/**
 * @brief Clear a slice object.
 *
 * This fills in a slice object so that it's “empty”.  You should not
 * try to call any of the slice methods on an empty slice, nor should
 * you try to dereference the slice's @a buf pointer.  An empty slice is
 * equivalent to a @c NULL pointer.
 *
 * @param [in] slice  The slice instance to clear
 *
 * @public @memberof cork_slice_t
 * @since 0.1-dev
 */

void
cork_slice_clear(cork_slice_t *slice);


/**
 * @brief Return whether a slice is empty.
 * @param [in] slice  A slice instance
 * @returns Whether the slice instance is empty
 * @public @memberof cork_slice_t
 * @since 0.1-dev
 */

#if defined(CORK_DOCUMENTATION)
bool
cork_slice_is_empty(cork_slice_t *slice);
#else
#define cork_slice_is_empty(slice)  ((slice)->buf == NULL)
#endif


/**
 * @brief Initialize a new slice that refers to a subset of an existing
 * slice.
 *
 * The @a offset and @a length parameters identify the subset.  If these
 * parameters don't refer to a valid portion of the slice, we return @c
 * false, and @a dest will be empty.  If the subset is valid, we return
 * @c true.
 *
 * Regardless of whether the new slice is valid, you @b must ensure that
 * you call @ref cork_slice_finish when you are done with the slice.
 *
 * @param [in] dest  The slice to initialize
 * @param [in] slice  The existing slice instance to copy
 * @param [in] offset  The offset into the existing slice where the new
 * slice should begin
 * @param [in] length  The desired length of the new slice
 *
 * @returns @c true if @a buffer, @a offset, and @a length refer to a
 * valid portion of the slice; @c false otherwise.
 *
 * @public @memberof cork_slice_t
 * @since 0.1-dev
 */

bool
cork_slice_copy(cork_slice_t *dest,
                cork_slice_t *slice,
                size_t offset, size_t length);


/**
 * @brief Initialize a new slice that refers to the subset of an
 * existing slice beginning from the given offset.
 *
 * This is equivalent to calling @ref cork_slice_copy with a @a length
 * of <code>slice-\>size - offset</code>.
 *
 * Regardless of whether the new slice is valid, you @b must ensure that
 * you call @ref cork_slice_finish when you are done with the slice.
 *
 * @param [in] dest  The slice to initialize
 * @param [in] slice  The existing slice instance to copy
 * @param [in] offset  The offset into the existing slice where the new
 * slice should begin
 *
 * @returns @c true if @a buffer and @a offset refer to a valid portion
 * of the slice; @c false otherwise.
 *
 * @public @memberof cork_slice_t
 * @since 0.1-dev
 */

bool
cork_slice_copy_offset(cork_slice_t *dest,
                       cork_slice_t *slice,
                       size_t offset);


/**
 * @brief Update a slice to refer to a subset of its contents.
 *
 * The @a offset and @a length parameters identify the subset.  If these
 * parameters don't refer to a valid portion of the slice, we return @c
 * false, and the slice will remain unchanged.
 *
 * @param [in] slice  The slice to update
 * @param [in] offset  The offset into the slice's previous contents
 * where the updated slice should begin
 * @param [in] length  The desired length of the updated slice
 *
 * @returns @c true if @a buffer, @a offset, and @a length refer to a
 * valid portion of the slice; @c false otherwise.
 *
 * @public @memberof cork_slice_t
 * @since 0.1-dev
 */

bool
cork_slice_slice(cork_slice_t *slice,
                 size_t offset, size_t length);


/**
 * @brief Update a slice to refer to the subset of its existing
 * contents, beginning from the given offset.
 *
 * This is equivalent to calling @ref cork_slice_slice with a @a length
 * of <code>slice-\>size - offset</code>.
 *
 * @param [in] slice  The slice to update
 * @param [in] offset  The offset into the slice's previous contents
 * where the updated slice should begin
 *
 * @returns @c true if @a buffer and @a offset refer to a valid portion
 * of the slice; @c false otherwise.
 *
 * @public @memberof cork_slice_t
 * @since 0.1-dev
 */

bool
cork_slice_slice_offset(cork_slice_t *slice,
                        size_t offset);


/**
 * @brief Finalize a slice, freeing the underlying buffer if necessary.
 *
 * @param [in] slice  A slice instance
 *
 * @public @memberof cork_slice_t
 * @since 0.1-dev
 */

void
cork_slice_finish(cork_slice_t *slice);


/**
 * @brief Check the contents of two slices for equality.
 *
 * The slices don't have to refer to the same physical buffers; their
 * contents just need to be identical.
 *
 * @param [in] slice1  A slice instance
 * @param [in] slice2  A slice instance
 * @returns Whether the contents of the two slices are identical.
 *
 * @public @memberof cork_slice_t
 * @since 0.1-dev
 */

bool
cork_slice_equal(const cork_slice_t *slice1,
                 const cork_slice_t *slice2);


#endif /* LIBCORK_DS_SLICE_H */
