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
 * @brief A copy of struct cork_alloc that lets us assign to the @a alloc
 * field.
 */

struct cork_mutable_alloc {
    cork_alloc_func  alloc;
};


/**
 * @brief A default free function for the struct cork_alloc class.
 */

static void
cork_allocator_default_free(struct cork_alloc *alloc)
{
    cork_delete(alloc, struct cork_alloc, alloc);
}


struct cork_alloc *
cork_allocator_new(cork_alloc_func alloc_func)
{
    struct cork_alloc  *alloc =
        alloc_func(NULL, NULL, 0, sizeof(struct cork_alloc));
    if (alloc == NULL) {
        return NULL;
    }

    struct cork_mutable_alloc  *m_alloc = (struct cork_mutable_alloc *) alloc;
    m_alloc->alloc = alloc_func;
    alloc->free = cork_allocator_default_free;
    return alloc;
}


/**
 * @brief An allocator function that uses the standard @c realloc
 * function.
 */

static void *
cork_malloc_alloc_func(struct cork_alloc *alloc,
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
cork_default_alloc_free(struct cork_alloc *alloc)
{
    /* nothing to do, the object is static! */
}

/**
 * @brief An allocator object that uses the standard @c realloc
 * function.
 */

static struct cork_alloc  CORK_MALLOC_ALLOCATOR =
{
    cork_malloc_alloc_func,
    cork_default_alloc_free
};


struct cork_alloc *
cork_allocator_new_malloc(void)
{
    return &CORK_MALLOC_ALLOCATOR;
}


/**
 * @brief A debugging allocator function that verifies that the @a osize
 * passed in for reallocs and frees matches what was given to the
 * corresponding malloc.
 */

static void *
cork_debug_alloc_func(struct cork_alloc *alloc,
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

static struct cork_alloc  CORK_DEBUG_ALLOCATOR =
{
    cork_debug_alloc_func,
    cork_default_alloc_free
};


struct cork_alloc *
cork_allocator_new_debug(void)
{
    return &CORK_DEBUG_ALLOCATOR;
}


void
cork_allocator_free(struct cork_alloc *alloc)
{
    alloc->free(alloc);
}
