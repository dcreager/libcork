/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CORE_ALLOCATOR_H
#define LIBCORK_CORE_ALLOCATOR_H

#include <libcork/core/attributes.h>
#include <libcork/core/types.h>


/* Need to forward declare this since there's a circular dependency
 * between allocator.h and error.h */
struct cork_error;

/* Forward declaration */
struct cork_alloc;


/*-----------------------------------------------------------------------
 * Error handling
 */

/* hash of "libcork/core/allocator.h" */
#define CORK_ALLOC_ERROR  0xa6f0c61b

enum cork_alloc_error {
    /* An error while allocating or reallocating some memory. */
    CORK_ALLOC_CANNOT_ALLOCATE
};

void
cork_alloc_cannot_allocate_set(struct cork_alloc *alloc,
                               struct cork_error *err,
                               const char *what);


/*-----------------------------------------------------------------------
 * Custom allocators
 */

/*
 * Should mimic:
 *   malloc() if osize == 0
 *   realloc() if osize != 0 && nsize != 0
 *   free() if osize != 0 && nsize == 0
 *
 * (ptr will always be NULL if osize == 0)
 */
typedef void *
(*cork_alloc_func)(struct cork_alloc *alloc, void *ptr,
                   size_t osize, size_t nsize);

struct cork_alloc {
    const cork_alloc_func  alloc;

    void
    (*free)(struct cork_alloc *alloc);
};


/* Uses standard malloc/realloc/free */
struct cork_alloc *
cork_allocator_new_malloc(void);

/* Verifies sizes when freeing */
struct cork_alloc *
cork_allocator_new_debug(void);

/* Wraps a custom allocation function with no extra state */
struct cork_alloc *
cork_allocator_new(cork_alloc_func alloc_func);

void
cork_allocator_free(struct cork_alloc *alloc);


/*-----------------------------------------------------------------------
 * Raw allocation macros
 */

/* size-based functions */
void *
cork_malloc(struct cork_alloc *alloc, size_t size) CORK_ATTR_MALLOC;

void *
cork_realloc(struct cork_alloc *alloc, void *ptr, size_t osize, size_t nsize);

void
cork_free(struct cork_alloc *alloc, void *ptr, size_t osize);

/* type-based macros */
#define cork_new(alloc, type) \
    ((type *) cork_malloc(alloc, sizeof(type)))
#define cork_delete(alloc, type, instance) \
    cork_free(alloc, instance, sizeof(type))

/* string-related functions */

const char *
cork_strdup(struct cork_alloc *alloc, const char *str);

void
cork_strfree(struct cork_alloc *alloc, const char *str);


#endif /* LIBCORK_CORE_ALLOCATOR_H */
