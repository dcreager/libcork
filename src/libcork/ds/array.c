/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011-2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>
#include <string.h>

#include "libcork/core/types.h"
#include "libcork/ds/array.h"
#include "libcork/helpers/errors.h"

#ifndef CORK_ARRAY_DEBUG
#define CORK_ARRAY_DEBUG 0
#endif

#if CORK_ARRAY_DEBUG
#include <stdio.h>
#define DEBUG(...) \
    do { \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n"); \
    } while (0)
#else
#define DEBUG(...) /* nothing */
#endif


struct cork_array_private {
    void  *items;
    size_t  size;
    size_t  allocated_size;
};

void
cork_array_ensure_size_(void *varray, size_t desired_count, size_t element_size)
{
    struct cork_array_private  *array = varray;
    size_t  desired_size;

    DEBUG("--- Array %p: Ensure %zu %zu-byte elements",
          array, desired_count, element_size);

    if ((array->allocated_size == 0) &&
        (desired_count <= CORK_INTERNAL_COUNT_FROM_SIZE(element_size))) {
        /* There's already enough space in our internal storage, which
         * we're still using. */
        return;
    }

    /* Otherwise reallocate if there's not enough space in our
     * heap-allocated array. */
    desired_size = desired_count * element_size;

    if (array->allocated_size == 0) {
        void  *old_items = array->items;
        size_t  old_size =
            element_size * CORK_INTERNAL_COUNT_FROM_SIZE(element_size);
        size_t  new_size = CORK_ARRAY_SIZE;
        if (desired_size > new_size) {
            new_size = desired_size;
        }

        DEBUG("--- Array %p: Allocating %zu->%zu bytes",
              array, old_size, new_size);
        array->items = cork_malloc(new_size);
        DEBUG("    Copying %zu bytes from %p to %p",
              old_size, old_items, array->items);
        memcpy(array->items, old_items, old_size);
        array->allocated_size = new_size;
    }

    else if (desired_size > array->allocated_size) {
        size_t  new_size = array->allocated_size * 2;
        if (desired_size > new_size) {
            new_size = desired_size;
        }

        DEBUG("--- Array %p: Reallocating %zu->%zu bytes",
              array, array->allocated_size, new_size);
        array->items = cork_realloc(array->items, new_size);
        array->allocated_size = new_size;
    }
}

void *
cork_array_append_get_(void *varray, size_t element_size)
{
    struct cork_array_private  *array = varray;
    cork_array_ensure_size_(array, array->size+1, element_size);
    return array->items + (element_size * (array->size++));
}
