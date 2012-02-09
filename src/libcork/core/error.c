/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include <libcork/config.h>
#include <libcork/core/allocator.h>
#include <libcork/core/error.h>
#include <libcork/ds/buffer.h>
#include <libcork/threads/basics.h>


/*-----------------------------------------------------------------------
 * Life cycle
 */

struct cork_error {
    cork_error_class  error_class;
    cork_error_code  error_code;
    struct cork_buffer  message;
};

static void
cork_error_init(struct cork_error *err)
{
    memset(err, 0, sizeof(struct cork_error));
}

static void
cork_error_done(struct cork_error *err)
{
    cork_buffer_done(&err->message);
}


/*-----------------------------------------------------------------------
 * Thread-local error struct
 */

/* Prefer, in order:
 *
 * 1) __thread storage class
 * 2) pthread_key_t
 */

#if CORK_CONFIG_HAVE_THREAD_STORAGE_CLASS
static __thread struct cork_error  error =
    { CORK_ERROR_NONE, 0, CORK_BUFFER_INIT() };

static struct cork_error *
cork_error_get(void)
{
    return &error;
}

#elif CORK_HAVE_PTHREADS
#include <pthread.h>

static pthread_key_t  error_key;
cork_once_barrier(error_once);

static void
cork_error_destroy(void *verr)
{
    struct cork_error  *err = verr;
    cork_error_done(err);
    free(err);
}

static void
cork_error_init_key(void)
{
    cork_once
        (error_once,
         assert(pthread_key_create(&error_key, cork_error_destroy) == 0));
}

static struct cork_error *
cork_error_get(void)
{
    struct cork_error  *err;
    cork_error_init_key();
    err = pthread_getspecific(error_key);

    if (CORK_UNLIKELY(err == NULL)) {
        err = cork_new(struct cork_error);
        cork_error_init(err);
        pthread_setspecific(error_key, err);
    }

    return err;
}

#else
#error "No thread-local storage implementation!"
#endif


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
cork_unknown_error_set_(const char *location)
{
    cork_error_set
        (CORK_BUILTIN_ERROR, CORK_UNKNOWN_ERROR,
         "Unknown error in %s", location);
}
