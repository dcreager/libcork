/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_DS_RING_BUFFER_H
#define LIBCORK_DS_RING_BUFFER_H

/**
 * @file
 * @brief Implementation of the @ref ring_buffer submodule
 */

#include <libcork/core/allocator.h>
#include <libcork/core/types.h>

/**
 * @addtogroup ring_buffer
 *
 * <tt>#%include \<libcork/ds/ring-buffer.h\></tt>
 *
 * This section defines a ring buffer data structure that can easily be
 * embedded into other types.
 *
 * @{
 */

/**
 * @brief A doubly-linked list.
 *
 * The doubly-linked list itself is represented by a sentinel value,
 * representing the empty list.  This sentinel's @a next and @a prev pointers
 * can then be used to iterate through the rest of the list.
 *
 * @since 0.2-dev
 */

struct cork_ring_buffer {
    /** @brief The elements of the ring buffer */
    void  **elements;

    /** @brief The number of elements that can be stored in this ring
     * buffer. */
    size_t  allocated_size;

    /** @brief The actual number of elements currently in the ring
     * buffer. */
    size_t  size;

    /** @brief The index of the next element to read from the buffer */
    size_t  read_index;

    /** @brief The index of the next element to write into the buffer */
    size_t  write_index;
};

/* end of ring_buffer group */
/**
 * @}
 */


/**
 * @brief Initialize a ring buffer.
 *
 * The buffer will initially be empty.
 *
 * @param[in] buf  The ring buffer to initialize
 * @param[in] size  The capacity of the new ring buffer
 * @public @memberof cork_ring_buffer
 * @since 0.2-dev
 */

int
cork_ring_buffer_init(struct cork_alloc *alloc,
                      struct cork_ring_buffer *buf, size_t size);

/**
 * @brief Finalize a ring buffer.
 *
 * @param[in] buf  A ring buffer
 * @public @memberof cork_ring_buffer
 * @since 0.2-dev
 */

void
cork_ring_buffer_done(struct cork_alloc *alloc,
                      struct cork_ring_buffer *buf);

/**
 * @brief Return whether a ring buffer is empty.
 * @param [in] buf  A ring buffer
 * @returns Whether the ring buffer is empty.
 * @public @memberof cork_ring_buffer
 * @since 0.2-dev
 */

#if defined(CORK_DOCUMENTATION)
bool
cork_ring_buffer_is_empty(struct cork_ring_buffer *buf);
#else
#define cork_ring_buffer_is_empty(buf) ((buf)->size == 0)
#endif

/**
 * @brief Return whether a ring buffer is full.
 * @param [in] buf  A ring buffer
 * @returns Whether the ring buffer is full.
 * @public @memberof cork_ring_buffer
 * @since 0.2-dev
 */

#if defined(CORK_DOCUMENTATION)
bool
cork_ring_buffer_is_full(struct cork_ring_buffer *buf);
#else
#define cork_ring_buffer_is_full(buf) ((buf)->size == (buf)->allocated_size)
#endif

/**
 * @brief Add an element to a ring buffer.
 * @param[in] buf  A ring buffer
 * @param[in] element  The element to add
 * @returns 0 if the element is successfully added; -1 if the ring
 * buffer is full
 * @public @memberof cork_ring_buffer
 * @since 0.2-dev
 */

int
cork_ring_buffer_add(struct cork_ring_buffer *buf, void *element);

/**
 * @brief Remove an element from a ring buffer.
 * @param[in] buf  A ring buffer
 * @returns The first element in the ring buffer, or @c NULL if the ring
 * buffer is empty
 * @public @memberof cork_ring_buffer
 * @since 0.2-dev
 */

void *
cork_ring_buffer_pop(struct cork_ring_buffer *buf);

/**
 * @brief Retrieve an element from a ring buffer without removing it.
 * @param[in] buf  A ring buffer
 * @returns The first element in the ring buffer, or @c NULL if the ring
 * buffer is empty
 * @public @memberof cork_ring_buffer
 * @since 0.2-dev
 */

void *
cork_ring_buffer_peek(struct cork_ring_buffer *buf);


#endif /* LIBCORK_DS_RING_BUFFER_H */
