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

#define cork_array_init(arr) \
    do { \
        (arr)->items = (arr)->internal; \
        (arr)->size = 0; \
        (arr)->allocated_size = 0; \
    } while (0)

#define cork_array_done(arr) \
    do { \
        if ((arr)->allocated_size != 0) { \
            free((arr)->items); \
        } \
    } while (0)

#define cork_array_at(arr, i)     ((arr)->items[(i)])
#define cork_array_size(arr)      ((arr)->size)
#define cork_array_is_empty(arr)  ((arr)->size == 0)

#define cork_array_element_size(arr)  (sizeof((arr)->items[0]))

#define cork_array_ensure_size(arr, count) \
    (cork_array_ensure_size_ \
     ((arr), (count), cork_array_element_size(arr)))

int
cork_array_ensure_size_(void *array, size_t desired_count, size_t element_size);

#define cork_array_append(arr, element) \
    (cork_array_ensure_size((arr), (arr)->size+1) || \
     ((arr)->items[(arr)->size++] = (element), 0))

void *
cork_array_append_get_(void *array, size_t element_size);

#define cork_array_append_get(arr) \
    (cork_array_append_get_ \
     ((arr), cork_array_element_size(arr)))


#endif /* LIBCORK_DS_ARRAY_H */
