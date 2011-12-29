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
#include "libcork/core/error.h"
#include "libcork/core/types.h"


/*-----------------------------------------------------------------------
 * Error handling
 */

void
cork_alloc_cannot_allocate_set(struct cork_alloc *alloc,
                               struct cork_error *err,
                               const char *kind)
{
    cork_error_set
        (alloc, err, CORK_ALLOC_ERROR, CORK_ALLOC_CANNOT_ALLOCATE,
         "Error allocating %s", kind);
}


/*-----------------------------------------------------------------------
 * Custom allocators
 */

/* A default free function for allocators that don't maintain extra
 * state.  (So we know that the actual type of the allocator is struct
 * cork_alloc) */

static void
cork_allocator_default_free(struct cork_alloc *alloc)
{
    cork_delete(alloc, struct cork_alloc, alloc);
}


/* A free function for static allocator objects */

static void
cork_default_alloc_free(struct cork_alloc *alloc)
{
    /* nothing to do, the object is static! */
}


struct cork_alloc *
cork_allocator_new(cork_alloc_func alloc_func)
{
    struct cork_alloc  *alloc =
        alloc_func(NULL, NULL, 0, sizeof(struct cork_alloc));
    if (alloc == NULL) {
        return NULL;
    }

    /* We need this trick since the alloc field is const in struct
     * cork_alloc.  (And by using a union, we don't violate any strict
     * aliasing rules.) */
    union {
        struct cork_alloc  *original;
        cork_alloc_func  *alloc;
    } mutable;
    mutable.original = alloc;
    *mutable.alloc = alloc_func;

    alloc->free = cork_allocator_default_free;
    return alloc;
}


/* A custom allocator that uses the standard realloc function. */

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


/* A debugging allocator function that verifies that the osize passed in
 * for realloc and free matches what was given to the corresponding
 * malloc. */

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


const char *
cork_strdup(struct cork_alloc *alloc, const char *str)
{
    size_t  len = strlen(str);
    size_t  allocated_size = len + sizeof(size_t) + 1;
    size_t  *new_str = cork_malloc(alloc, allocated_size);
    if (new_str == NULL) {
        return NULL;
    }

    *new_str = allocated_size;
    char  *dest = (char *) (void *) (new_str + 1);
    strncpy(dest, str, len + 1);
    return dest;
}


void
cork_strfree(struct cork_alloc *alloc, const char *str)
{
    size_t  *base = ((size_t *) str) - 1;
    size_t  allocated_size = *base;
    cork_free(alloc, base, allocated_size);
}
