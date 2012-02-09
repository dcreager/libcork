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

#include <stdio.h>
#include <stdlib.h>

#include <libcork/core/attributes.h>
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

#define cork_abort_(func, file, line, fmt, ...) \
    do { \
        fprintf(stderr, fmt "\n  in %s (%s:%u)\n", \
                __VA_ARGS__, (func), (file), (unsigned int) (line)); \
        abort(); \
    } while (0)

#define cork_abort(fmt, ...) \
    cork_abort_(__func__, __FILE__, __LINE__, fmt, __VA_ARGS__)

CORK_ATTR_UNUSED
static void *
cork_abort_if_null_(void *ptr, const char *msg, const char *func,
                    const char *file, unsigned int line)
{
    if (CORK_UNLIKELY(ptr == NULL)) {
        cork_abort_(func, file, line, "%s", msg);
    } else {
        return ptr;
    }
}

#define cork_abort_if_null(ptr, msg) \
    (cork_abort_if_null_(ptr, msg, __func__, __FILE__, __LINE__))

#define cork_alloc_or_abort(op, ...) \
    (cork_abort_if_null(cork_x##op(__VA_ARGS__), #op " failed"))

#define cork_malloc(size)         cork_alloc_or_abort(malloc, size)
#define cork_calloc(count, size)  cork_alloc_or_abort(calloc, count, size)
#define cork_realloc(ptr, size)   cork_alloc_or_abort(realloc, ptr, size)
#define cork_new(type)            cork_alloc_or_abort(new, type)
#define cork_strdup(str)          cork_alloc_or_abort(strdup, str)


#endif /* LIBCORK_CORE_ALLOCATOR_H */
