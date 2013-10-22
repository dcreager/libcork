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
#include "libcork/os/process.h"
#include "libcork/threads/basics.h"


/*-----------------------------------------------------------------------
 * Life cycle
 */

struct cork_error {
    cork_error_class  error_class;
    cork_error_code  error_code;
    struct cork_buffer  *message;
    struct cork_buffer  *other;
    struct cork_buffer  buf1;
    struct cork_buffer  buf2;
    struct cork_error  *next;
};

static struct cork_error *
cork_error_new(void)
{
    struct cork_error  *error = cork_new(struct cork_error);
    error->error_class = CORK_ERROR_NONE;
    cork_buffer_init(&error->buf1);
    cork_buffer_init(&error->buf2);
    error->message = &error->buf1;
    error->other = &error->buf2;
    return error;
}

static void
cork_error_free(struct cork_error *error)
{
    cork_buffer_done(&error->buf1);
    cork_buffer_done(&error->buf2);
    free(error);
}


static struct cork_error * volatile  errors;

cork_once_barrier(cork_error_list);

static void
cork_error_list_done(void)
{
    struct cork_error  *curr;
    struct cork_error  *next;
    for (curr = errors; curr != NULL; curr = next) {
        next = curr->next;
        cork_error_free(curr);
    }
}

static void
cork_error_list_init(void)
{
    cork_cleanup_at_exit(0, cork_error_list_done);
}


cork_tls(struct cork_error *, cork_error_);

static struct cork_error *
cork_error_get(void)
{
    struct cork_error  **error_ptr = cork_error__get();
    if (CORK_UNLIKELY(*error_ptr == NULL)) {
        struct cork_error  *old_head;
        struct cork_error  *error = cork_error_new();
        cork_once(cork_error_list, cork_error_list_init());
        do {
            old_head = errors;
            error->next = old_head;
        } while (cork_ptr_cas(&errors, old_head, error) != old_head);
        *error_ptr = error;
        return error;
    } else {
        return *error_ptr;
    }
}


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
    return error->message->buf;
}

void
cork_error_set(cork_error_class error_class, cork_error_code error_code,
               const char *format, ...)
{
    va_list  args;
    struct cork_error  *error = cork_error_get();
    error->error_class = error_class;
    error->error_code = error_code;
    va_start(args, format);
    cork_buffer_vprintf(error->message, format, args);
    va_end(args);
}

void
cork_error_prefix(const char *format, ...)
{
    va_list  args;
    struct cork_error  *error = cork_error_get();
    struct cork_buffer  *temp;
    va_start(args, format);
    cork_buffer_vprintf(error->other, format, args);
    va_end(args);
    cork_buffer_append_copy(error->other, error->message);
    temp = error->other;
    error->other = error->message;
    error->message = temp;
}

void
cork_error_clear(void)
{
    struct cork_error  *error = cork_error_get();
    error->error_class = CORK_ERROR_NONE;
    error->error_code = 0;
}

void
cork_system_error_set_explicit(int err)
{
    cork_error_set(CORK_BUILTIN_ERROR, CORK_SYSTEM_ERROR, "%s", strerror(err));
}

void
cork_system_error_set(void)
{
    cork_system_error_set_explicit(errno);
}

void
cork_unknown_error_set_(const char *location)
{
    cork_error_set
        (CORK_BUILTIN_ERROR, CORK_UNKNOWN_ERROR,
         "Unknown error in %s", location);
}
