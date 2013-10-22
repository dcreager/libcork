/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CORE_ERROR_H
#define LIBCORK_CORE_ERROR_H

#include <stdio.h>
#include <stdlib.h>

#include <libcork/core/api.h>
#include <libcork/core/attributes.h>
#include <libcork/core/types.h>


/* Should be a hash of a string representing the error class. */
typedef uint32_t  cork_error_class;

/* An error class that represents “no error”. */
#define CORK_ERROR_NONE  ((cork_error_class) 0)

typedef unsigned int  cork_error_code;

CORK_API bool
cork_error_occurred(void);

CORK_API cork_error_class
cork_error_get_class(void);

CORK_API cork_error_code
cork_error_get_code(void);

CORK_API const char *
cork_error_message(void);

CORK_API void
cork_error_set(cork_error_class error_class, cork_error_code error_code,
               const char *format, ...)
    CORK_ATTR_PRINTF(3,4);

CORK_API void
cork_error_prefix(const char *format, ...)
    CORK_ATTR_PRINTF(1,2);

CORK_API void
cork_error_clear(void);


/*-----------------------------------------------------------------------
 * Built-in errors
 */

/* hash of "libcork/core/error.h" */
#define CORK_BUILTIN_ERROR  0xd178dde5

enum cork_builtin_error {
    /* An error reported by the C library's errno mechanism */
    CORK_SYSTEM_ERROR,
    /* An unknown error */
    CORK_UNKNOWN_ERROR
};

CORK_API void
cork_system_error_set(void);

CORK_API void
cork_system_error_set_explicit(int err);

CORK_API void
cork_unknown_error_set_(const char *location);

#define cork_unknown_error() \
    cork_unknown_error_set_(__func__)


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

#define cork_unreachable() \
    cork_abort("%s", "Code should not be reachable")


#endif /* LIBCORK_CORE_ERROR_H */
