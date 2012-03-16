/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CORE_ERROR_H
#define LIBCORK_CORE_ERROR_H


#include <libcork/core/attributes.h>
#include <libcork/core/types.h>


/* Should be a hash of a string representing the error class. */
typedef uint32_t  cork_error_class;

/* An error class that represents “no error”. */
#define CORK_ERROR_NONE  ((cork_error_class) 0)

typedef unsigned int  cork_error_code;

bool
cork_error_occurred(void);

cork_error_class
cork_error_get_class(void);

cork_error_code
cork_error_get_code(void);

const char *
cork_error_message(void);

void
cork_error_set(cork_error_class error_class, cork_error_code error_code,
               const char *format, ...)
    CORK_ATTR_PRINTF(3,4);

void
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

void
cork_system_error_set(void);

void
cork_unknown_error_set_(const char *location);

#define cork_unknown_error() \
    cork_unknown_error_set_(__func__)


#endif /* LIBCORK_CORE_ERROR_H */
