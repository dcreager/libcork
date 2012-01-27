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

#include <stdlib.h>

#include <libcork/core/attributes.h>
#include <libcork/core/types.h>


/* Need to forward declare this since there's a circular dependency
 * between allocator.h and error.h */
struct cork_error;


/*-----------------------------------------------------------------------
 * Error handling
 */

/* hash of "libcork/core/allocator.h" */
#define CORK_ALLOC_ERROR  0xa6f0c61b

enum cork_alloc_error {
    /* An error while allocating or reallocating some memory. */
    CORK_CANNOT_ALLOCATE
};

void
cork_cannot_allocate_set(struct cork_error *err, const char *what);


/*-----------------------------------------------------------------------
 * Raw allocation macros
 */

#if CORK_HAVE_REALLOCF
#define cork_realloc  reallocf
#else
void *
cork_realloc(void *ptr, size_t new_size);
#endif

/* type-based macros */
#define cork_new(type)  ((type *) malloc(sizeof(type)))
#define cork_delete(type, instance)  free((instance))

/* string-related functions */

const char *
cork_strdup(const char *str);

void
cork_strfree(const char *str);


#endif /* LIBCORK_CORE_ALLOCATOR_H */
