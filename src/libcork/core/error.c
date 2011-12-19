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


static void
cork_error_free_extra(struct cork_alloc *alloc, struct cork_error *error)
{
    /* An error's extra field will point at the reserved_extra field if all
     * of the error's extra data can fit in there.  In this case, we don't
     * need to free the extra field, since it's not pointing at a
     * distinct heap object.  If it's pointing at some other pointer,
     * then we assume that the extra pointer is heap allocated.  It's
     * size will be stored in reserved_extra. */

    if (error->extra != NULL && error->extra != error->reserved_extra) {
        union {
            size_t  *s;
            uint8_t  *u8;
        } strict_cast;

        strict_cast.u8 = error->reserved_extra;
        cork_free(alloc, error->extra, *(strict_cast.s));
    }
}

int
cork_error_init(struct cork_alloc *alloc, struct cork_error *error)
{
    memset(error, 0, sizeof(error));
    return 0;
}


void
cork_error_done(struct cork_alloc *alloc, struct cork_error *error)
{
    cork_error_free_extra(alloc, error);
    memset(error, 0, sizeof(error));
}


int
cork_error_message(struct cork_alloc *alloc, struct cork_error *error,
                   struct cork_buffer *dest)
{
    if (error->printer == NULL) {
        return cork_buffer_printf
            (alloc, dest, NULL, "Unknown error: class=0x%08" PRIx32 ", code=%u",
             error->error_class, error->error_code);
    }

    else {
        return error->printer(alloc, error, dest);
    }
}


int
cork_error_set(struct cork_alloc *alloc, struct cork_error *error,
               cork_error_class eclass, cork_error_code ecode,
               cork_error_printer printer)
{
    if (error != NULL) {
        /* TODO: Assert that there isn't already an error */
        error->error_class = eclass;
        error->error_code = ecode;
        error->printer = printer;
        error->extra = NULL;
    }

    return 0;
}

int
cork_error_set_extra_raw(struct cork_alloc *alloc, struct cork_error *error,
                         cork_error_class eclass, cork_error_code ecode,
                         cork_error_printer printer,
                         void *extra, size_t extra_size)
{
    if (error != NULL) {
        /* TODO: Assert that there isn't already an error */
        error->error_class = eclass;
        error->error_code = ecode;
        error->printer = printer;

        if (extra_size <= CORK_ERROR_RESERVED_EXTRA_SPACE) {
            error->extra = error->reserved_extra;
        } else {
            error->extra = cork_malloc(alloc, extra_size);
            if (error->extra == NULL) {
                /* Can't use the real printer, since it's allowed to
                 * assume that extra is non-NULL. */
                error->printer = NULL;
                return 1;
            } else {
                /* Save the size of extra into reserved_extra, so that
                 * we can free the extra space later. */
                union {
                    size_t  *s;
                    uint8_t  *u8;
                } strict_cast;

                strict_cast.u8 = error->reserved_extra;
                *(strict_cast.s) = extra_size;
            }
        }

        memcpy(error->extra, extra, extra_size);
    }

    return 0;
}


void
cork_error_propagate(struct cork_alloc *alloc,
                     struct cork_error *error, struct cork_error *suberror)
{
    if (error == NULL) {
        cork_error_done(alloc, suberror);
    } else {
        /* TODO: Assert that there isn't already an error */
        memcpy(error, suberror, sizeof(struct cork_error));
        cork_error_init(alloc, suberror);
    }
}
