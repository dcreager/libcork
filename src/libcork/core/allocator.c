/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libcork/core/allocator.h"
#include "libcork/core/types.h"


/**
 * @brief A copy of cork_allocator_t that lets us assign to the @a alloc
 * field.
 */

typedef struct cork_mutable_allocator_t
{
    cork_alloc_func_t  alloc;
} cork_mutable_allocator_t;


/**
 * @brief A default free function for the cork_allocator_t class.
 */

static void
cork_allocator_default_free(cork_allocator_t *alloc)
{
    cork_delete(alloc, cork_allocator_t, alloc);
}


cork_allocator_t *
cork_allocator_new(cork_alloc_func_t alloc_func)
{
    cork_allocator_t  *alloc =
        alloc_func(NULL, NULL, 0, sizeof(cork_allocator_t));
    if (alloc == NULL) {
        return NULL;
    }

    cork_mutable_allocator_t  *m_alloc = (cork_mutable_allocator_t *) alloc;
    m_alloc->alloc = alloc_func;
    alloc->free = cork_allocator_default_free;
    return alloc;
}


/**
 * @brief An allocator function that uses the standard @c realloc
 * function.
 */

static void *
cork_malloc_alloc_func(cork_allocator_t *alloc,
                       void *ptr, size_t osize, size_t nsize)
{
    if (nsize == 0) {
        free(ptr);
        return NULL;
    } else {
        return realloc(ptr, nsize);
    }
}

/**
 * @brief A free function for @ref cork_default_allocator.
 */

static void
cork_default_alloc_free(cork_allocator_t *alloc)
{
    /* nothing to do, the object is static! */
}

/**
 * @brief An allocator object that uses the standard @c realloc
 * function.
 */

static cork_allocator_t  cork_malloc_allocator =
{
    cork_malloc_alloc_func,
    cork_default_alloc_free
};


cork_allocator_t *
cork_allocator_new_malloc(void)
{
    return &cork_malloc_allocator;
}


/**
 * @brief A debugging allocator function that verifies that the @a osize
 * passed in for reallocs and frees matches what was given to the
 * corresponding malloc.
 */

static void *
cork_debug_alloc_func(cork_allocator_t *alloc,
                      void *ptr, size_t osize, size_t nsize)
{
    if (nsize == 0) {
        size_t  *size = ((size_t *) ptr) - 1;
        if (osize != *size) {
            fprintf(stderr,
                    "Error freeing %p:\n"
                    "Size passed to cork_free (%zu) "
                    "doesn't match size passed to "
                    "cork_malloc (%zu)\n",
                    ptr, osize, *size);
            abort();
        }
        free(size);
        return NULL;
    } else {
        size_t  real_nsize = nsize + sizeof(size_t);
        size_t  *real_ptr = (ptr == NULL)? NULL: ((size_t *) ptr)-1;

        if ((real_ptr != NULL) && (*real_ptr != osize)) {
            fprintf(stderr,
                    "Error resizing %p:\n"
                    "Size passed to cork_realloc (%zu) "
                    "doesn't match size passed to "
                    "cork_malloc (%zu)\n",
                    ptr, osize, *real_ptr);
            abort();
        }

        size_t  *new_ptr = realloc(real_ptr, real_nsize);
        *new_ptr = nsize;
        return (new_ptr + 1);
    }
}

/**
 * @brief An debug allocator object.
 */

static cork_allocator_t  cork_debug_allocator =
{
    cork_debug_alloc_func,
    cork_default_alloc_free
};


cork_allocator_t *
cork_allocator_new_debug(void)
{
    return &cork_debug_allocator;
}


void
cork_allocator_free(cork_allocator_t *alloc)
{
    alloc->free(alloc);
}
