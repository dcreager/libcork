/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_DS_ARRAY_H
#define LIBCORK_DS_ARRAY_H


#include <libcork/core/allocator.h>
#include <libcork/core/error.h>
#include <libcork/core/types.h>


#define CORK_ARRAY_SIZE  (sizeof(void *) * 8)
#define CORK_INTERNAL_COUNT_FROM_SIZE(sizeof_T) \
    ((CORK_ARRAY_SIZE - sizeof(void *) - 2*sizeof(size_t)) / sizeof_T)
#define CORK_INTERNAL_COUNT_FROM_TYPE(T) \
    CORK_INTERNAL_COUNT_FROM_SIZE(sizeof(T))


#define cork_array(T) \
    struct { \
        T  *items; \
        size_t  size; \
        size_t  allocated_size; /* in bytes */ \
        T  internal[CORK_INTERNAL_COUNT_FROM_TYPE(T)]; \
    }

#define cork_array_init(alloc, arr) \
    do { \
        (arr)->items = (arr)->internal; \
        (arr)->size = 0; \
        (arr)->allocated_size = 0; \
    } while (0)

#define cork_array_done(alloc, arr) \
    do { \
        if ((arr)->allocated_size != 0) { \
            cork_free((alloc), (arr)->items, (arr)->allocated_size); \
        } \
    } while (0)

#define cork_array_at(arr, i)     ((arr)->items[(i)])
#define cork_array_size(arr)      ((arr)->size)
#define cork_array_is_empty(arr)  ((arr)->size == 0)

#define cork_array_element_size(arr)  (sizeof((arr)->items[0]))

#define cork_array_ensure_size(alloc, arr, count, err) \
    (cork_array_ensure_size_ \
     ((alloc), (arr), (count), cork_array_element_size(arr), (err)))

int
cork_array_ensure_size_(struct cork_alloc *alloc, void *array,
                        size_t desired_count, size_t element_size,
                        struct cork_error *err);

#define cork_array_append(alloc, arr, element, err) \
    (cork_array_ensure_size((alloc), (arr), (arr)->size+1, (err)) || \
     ((arr)->items[(arr)->size++] = (element), 0))


#endif /* LIBCORK_DS_ARRAY_H */
