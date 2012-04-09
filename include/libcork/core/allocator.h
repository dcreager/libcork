/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011-2012, RedJack, LLC.
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
#include <libcork/core/error.h>
#include <libcork/core/types.h>


/*-----------------------------------------------------------------------
 * Recoverable
 */

#define cork_xmalloc  malloc
#define cork_xcalloc  calloc
#define cork_xfree    free

#if CORK_HAVE_REALLOCF
#define cork_xrealloc  reallocf
#else
void *
cork_xrealloc(void *ptr, size_t new_size) CORK_ATTR_MALLOC;
#endif

/* type-based macros */
#define cork_xnew(type)  ((type *) cork_xmalloc(sizeof(type)))

/* string-related functions */

const char *
cork_xstrdup(const char *str);

void
cork_strfree(const char *str);


/*-----------------------------------------------------------------------
 * Abort on failure
 */

#define cork_alloc_or_abort(op, ...) \
    (cork_abort_if_null(cork_x##op(__VA_ARGS__), #op " failed"))

#define cork_malloc(size)         cork_alloc_or_abort(malloc, size)
#define cork_calloc(count, size)  cork_alloc_or_abort(calloc, count, size)
#define cork_realloc(ptr, size)   cork_alloc_or_abort(realloc, ptr, size)
#define cork_new(type)            cork_alloc_or_abort(new, type)
#define cork_strdup(str) \
    ((const char *) cork_abort_if_null \
     ((void *) cork_xstrdup((str)), "strdup failed"))


#endif /* LIBCORK_CORE_ALLOCATOR_H */
