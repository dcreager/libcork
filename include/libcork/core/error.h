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
#include <libcork/ds/buffer.h>


/* Should be a hash of a string representing the error class. */
typedef uint32_t  cork_error_class;

/* An error class that represents “no error”. */
#define CORK_ERROR_NONE  ((cork_error_class) 0)

typedef unsigned int  cork_error_code;


struct cork_error {
    cork_error_class  error_class;
    cork_error_code  error_code;
    struct cork_buffer  message;
};

#define cork_error_occurred(error) \
    ((error)->error_class != CORK_ERROR_NONE)
#define cork_error_class(error)  ((error)->error_class)
#define cork_error_code(error)  ((error)->error_code)
#define cork_error_message(error)  ((const char *) (error)->message.buf)


void
cork_error_init(struct cork_error *error);

#define CORK_ERROR_INIT()  { CORK_ERROR_NONE, 0, CORK_BUFFER_INIT() }

void
cork_error_done(struct cork_error *error);


void
cork_error_set(struct cork_error *error, cork_error_class error_class,
               cork_error_code error_code, const char *format, ...)
    CORK_ATTR_PRINTF(4,5);

void
cork_error_propagate(struct cork_error *error, struct cork_error *suberror);


/*-----------------------------------------------------------------------
 * Built-in errors
 */

/* hash of "libcork/core/error.h" */
#define CORK_BUILTIN_ERROR  0xd178dde5

enum cork_builtin_error {
    /* An unknown error */
    CORK_UNKNOWN_ERROR
};

void
cork_unknown_error_set_(struct cork_error *err, const char *location);

#define cork_unknown_error_set(err) \
    cork_unknown_error_set_((err), __func__)


#endif /* LIBCORK_CORE_ERROR_H */
