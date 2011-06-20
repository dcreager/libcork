/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_DS_DLLIST_H
#define LIBCORK_DS_DLLIST_H

/**
 * @file
 * @brief Implementation of the @ref dllist submodule
 */

#include <libcork/core/types.h>

/**
 * @addtogroup dllist
 *
 * <tt>#%include \<libcork/ds/dllist.h\></tt>
 *
 * This section defines a doubly-linked list data structure that can easily be
 * embedded into other types.
 *
 * @{
 */


/**
 * @brief An element of a doubly-linked list.
 *
 * This type defines the elements of a doubly-linked list.  This type can be
 * embedded into a larger data structure:
 *
 * @code
 * typedef struct person_t {
 *     const char  *first_name;
 *     const char  *last_name;
 *     cork_dllist_item_t  list;
 * } person_t;
 * @endcode
 *
 * You can then use the @ref cork_container_of macro to obtain a pointer to the
 * @c person_t, if you're given a pointer to the @c cork_dllist_item_t.
 *
 * @since 0.1-dev
 */

typedef struct cork_dllist_item_t  cork_dllist_item_t;

struct cork_dllist_item_t {
    /** @brief A pointer to the next element in the list. @private */
    cork_dllist_item_t  *next;
    /** @brief A pointer to the previous element in the list. @private */
    cork_dllist_item_t  *prev;
};


/**
 * @brief A doubly-linked list.
 *
 * The doubly-linked list itself is represented by a sentinel value,
 * representing the empty list.  This sentinel's @a next and @a prev pointers
 * can then be used to iterate through the rest of the list.
 *
 * @since 0.1-dev
 */

typedef struct cork_dllist_t {
    /** @brief The sentinel element for this list. @private */
    cork_dllist_item_t  head;
} cork_dllist_t;

/* end of dllist group */
/**
 * @}
 */


/**
 * @brief Initialize a doubly-linked list.
 *
 * The list will initially be empty.
 *
 * @param[in] list  The list to initialize
 * @public @memberof cork_dllist_t
 * @since 0.1-dev
 */

void
cork_dllist_init(cork_dllist_t *list);


/**
 * @brief A function that can applied to each element in a doubly-linked list.
 * @param[in] element  The current list element
 * @param[in] user_data  A user-data pointer for the function
 * @ingroup dllist
 * @since 0.1-dev
 */

typedef void
(*cork_dllist_map_func_t)(cork_dllist_item_t *element, void *user_data);

/**
 * @brief Apply a function to each element of a doubly-linked list.
 * @param[in] list  A doubly-linked list
 * @param[in] func  The function to apply to each element
 * @param[in] user_data  An additional parameter to pass to the function
 * @public @memberof cork_dllist_t
 * @since 0.1-dev
 */

void
cork_dllist_map(cork_dllist_t *list,
                cork_dllist_map_func_t func, void *user_data);


/**
 * @brief Return the number of elements in a doubly-linked list.
 * @param[in] list  A doubly-linked list
 * @returns The number of elements in the list
 * @public @memberof cork_dllist_t
 * @since 0.1-dev
 */

size_t
cork_dllist_size(const cork_dllist_t *list);


/**
 * @brief Add an element to a list.
 *
 * You are responsible for allocating the list element yourself, most likely by
 * allocating the @c struct that contains the list element.
 *
 * @note This function assumes that the list element isn't already a member of
 * a different list.  You're responsible for calling @ref cork_dllist_remove if
 * this isn't the case.
 *
 * @param[in] list  A doubly-linked list
 * @param[in] element  A list element
 *
 * @public @memberof cork_dllist_t
 * @since 0.1-dev
 */

void
cork_dllist_add(cork_dllist_t *list, cork_dllist_item_t *element);


/**
 * @brief Remove an element from the list it belongs to.
 *
 * You don't have to pass in a pointer to the list itself.
 *
 * @note You shouldn't call this function on the list's sentinel value.
 *
 * @param[in] element  A list element
 *
 * @public @memberof cork_dllist_item_t
 * @since 0.1-dev
 */

void
cork_dllist_remove(cork_dllist_item_t *element);


#endif /* LIBCORK_DS_DLLIST_H */
