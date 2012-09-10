/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>

#include "libcork/config.h"
#include "libcork/core/allocator.h"
#include "libcork/core/error.h"
#include "libcork/ds/buffer.h"
#include "libcork/threads/basics.h"


/*-----------------------------------------------------------------------
 * Life cycle
 */

struct cork_error {
    cork_error_class  error_class;
    cork_error_code  error_code;
    struct cork_buffer  message;
};

CORK_ATTR_UNUSED
static void
cork_error_init(struct cork_error *err)
{
    memset(err, 0, sizeof(struct cork_error));
}

CORK_ATTR_UNUSED
static void
cork_error_done(struct cork_error *err)
{
    cork_buffer_done(&err->message);
}

cork_tls(struct cork_error, cork_error);


/*-----------------------------------------------------------------------
 * Public error API
 */

bool
cork_error_occurred(void)
{
    struct cork_error  *error = cork_error_get();
    return error->error_class != CORK_ERROR_NONE;
}

cork_error_class
cork_error_get_class(void)
{
    struct cork_error  *error = cork_error_get();
    return error->error_class;
}

cork_error_code
cork_error_get_code(void)
{
    struct cork_error  *error = cork_error_get();
    return error->error_code;
}

const char *
cork_error_message(void)
{
    struct cork_error  *error = cork_error_get();
    return error->message.buf;
}

void
cork_error_set(cork_error_class error_class, cork_error_code error_code,
               const char *format, ...)
{
    struct cork_error  *error = cork_error_get();
    error->error_class = error_class;
    error->error_code = error_code;
    va_list  args;
    va_start(args, format);
    cork_buffer_vprintf(&error->message, format, args);
    va_end(args);
}

void
cork_error_clear(void)
{
    struct cork_error  *error = cork_error_get();
    error->error_class = CORK_ERROR_NONE;
    error->error_code = 0;
}

void
cork_system_error_set(void)
{
    cork_error_set
        (CORK_BUILTIN_ERROR, CORK_SYSTEM_ERROR,
         "%s", strerror(errno));
}

void
cork_unknown_error_set_(const char *location)
{
    cork_error_set
        (CORK_BUILTIN_ERROR, CORK_UNKNOWN_ERROR,
         "Unknown error in %s", location);
}
