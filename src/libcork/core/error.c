/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdarg.h>
#include <string.h>

#include <libcork/core/allocator.h>
#include <libcork/core/error.h>
#include <libcork/ds/buffer.h>


bool
cork_error_init(struct cork_alloc *alloc, struct cork_error *error)
{
    error->error_class = CORK_ERROR_NONE;
    error->error_code = 0;
    cork_buffer_init(alloc, &error->message);
    return true;
}


void
cork_error_done(struct cork_error *error)
{
    error->error_class = CORK_ERROR_NONE;
    error->error_code = 0;
    cork_buffer_done(&error->message);
}


void
cork_error_set(struct cork_error *error,
               cork_error_class error_class,
               cork_error_code error_code,
               const char *format, ...)
{
    if (error != NULL) {
        /* TODO: Assert that there isn't already an error */
        error->error_class = error_class;
        error->error_code = error_code;
        va_list  args;
        va_start(args, format);
        cork_buffer_vprintf(&error->message, format, args);
        va_end(args);
    }
}


void
cork_error_clear(struct cork_error *error)
{
    if (error != NULL) {
        error->error_class = CORK_ERROR_NONE;
        error->error_code = 0;
        cork_buffer_clear(&error->message);
    }
}


void
cork_error_propagate(struct cork_error *error,
                     struct cork_error *suberror)
{
    if (error == NULL) {
        cork_error_done(suberror);
    } else {
        /* TODO: Assert that there isn't already an error */
        memcpy(error, suberror, sizeof(struct cork_error));
        cork_error_init(error->message.alloc, suberror);
    }
}
