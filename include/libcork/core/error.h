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


#include <libcork/core/allocator.h>
#include <libcork/core/attributes.h>
#include <libcork/core/types.h>
#include <libcork/ds/buffer.h>


/* Need to forward declare this since there's a circular dependency
 * between buffer.h and error.h */
struct cork_buffer;


/* Should be a hash of a string representing the error class. */
typedef uint32_t  cork_error_class;

/* An error class that represents “no error”. */
#define CORK_ERROR_NONE  ((cork_error_class) 0)

typedef unsigned int  cork_error_code;

/* A function that can format a particular error condition. */
typedef int
(*cork_error_printer)(struct cork_alloc *alloc, struct cork_error *err,
                      struct cork_buffer *dest);

/* The amount of space that we reserve inside of each cork_error
 * instance for extra data.  Error classes are free to use more space if
 * they need to, but that will have to be allocated from the heap. */
#define CORK_ERROR_RESERVED_EXTRA_SPACE  64

struct cork_error {
    cork_error_class  error_class;
    cork_error_code  error_code;
    cork_error_printer  printer;
    void  *extra;
    uint8_t  reserved_extra[CORK_ERROR_RESERVED_EXTRA_SPACE];
};

#define cork_error_occurred(error) \
    ((error)->error_class != CORK_ERROR_NONE)
#define cork_error_class(error)  ((error)->error_class)
#define cork_error_code(error)  ((error)->error_code)

#define cork_error_extra(error)  ((error)->extra)


int
cork_error_init(struct cork_alloc *alloc, struct cork_error *error);

#define CORK_ERROR_INIT(alloc)  { CORK_ERROR_NONE, 0, NULL, NULL, {0} }

void
cork_error_done(struct cork_alloc *alloc, struct cork_error *error);


int
cork_error_message(struct cork_alloc *alloc, struct cork_error *error,
                   struct cork_buffer *dest);

int
cork_error_set(struct cork_alloc *alloc, struct cork_error *error,
               cork_error_class eclass, cork_error_code ecode,
               cork_error_printer printer);

int
cork_error_set_extra_raw(struct cork_alloc *alloc, struct cork_error *error,
                         cork_error_class eclass, cork_error_code ecode,
                         cork_error_printer printer,
                         void *extra, size_t extra_size);

#define cork_error_set_extra(alloc, error, eclass, ecode, printer, extra) \
    (cork_error_set_extra_raw((alloc), (error), \
                              (eclass), (ecode), (printer), \
                              &(extra), sizeof(extra)))

void
cork_error_propagate(struct cork_alloc *alloc,
                     struct cork_error *error, struct cork_error *suberror);


#endif /* LIBCORK_CORE_ERROR_H */
