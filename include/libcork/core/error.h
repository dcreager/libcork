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

/**
 * @file
 * @brief Implementation of the @ref error submodule
 */

#include <libcork/core/allocator.h>
#include <libcork/core/attributes.h>
#include <libcork/core/types.h>
#include <libcork/ds/buffer.h>

/**
 * @addtogroup error
 *
 * <tt>#%include \<libcork/core/error.h\></tt>
 *
 * This section defines an API for reporting error conditions from
 * functions.  It is modeled on glib's @a GError API.
 *
 * @{
 */

/**
 * @brief An identifier for a class of error conditions
 * @since 0.2
 */

typedef uint32_t  cork_error_class_t;

/**
 * @brief An error class that represents “no error”.
 * @since 0.2
 */

#define CORK_ERROR_NONE  ((cork_error_class_t) 0)

/**
 * @brief An identifier for a particular type of error within a class.
 * @since 0.2
 */

typedef unsigned int  cork_error_code_t;

/**
 * @brief Records information about an error condition.
 *
 * Each possible error condition is identified by a tuple: a @a class,
 * which identifies a broad category of errors, and a @a code, which
 * identifies a particular error within that category.  In addition, an
 * error condition contains a string message that gives more detail.
 *
 * The class is represented by an integer, which should be a hash of a
 * unique string that identifies the error class.  (You can generate a
 * hash using the extras/hashstring.py script that's included in the
 * libcork source.)
 *
 * @since 0.2
 */

typedef struct cork_error_t
{
    /** @brief The class of this error. @private */
    cork_error_class_t  error_class;

    /** @brief The code for this error. @private */
    cork_error_code_t  error_code;

    /** @brief A @ref cork_buffer_t that stores the detailed message for
     * this error. @private */
    cork_buffer_t  message;
} cork_error_t;

/* end of error group */
/**
 * @}
 */

/**
 * @brief Test whether an error condition represents an actual error.
 * @param [in] ctx  A libcork context
 * @param [in] error  An error condition instance
 * @public @memberof cork_error_t
 * @since 0.2
 */

#if defined(CORK_DOCUMENTATION)
bool
cork_error_occurred(cork_context_t *ctx, const cork_error_t *error);
#else
#define cork_error_occurred(ctx, error) \
    ((error)->error_class != CORK_ERROR_NONE)
#endif

/**
 * @brief Retrieve the class from an error condition.
 * @param [in] error  An error condition instance
 * @public @memberof cork_error_t
 * @since 0.2
 */

#if defined(CORK_DOCUMENTATION)
cork_error_class_t
cork_error_class(const cork_error_t *error);
#else
#define cork_error_class(error)  ((error)->error_class)
#endif

/**
 * @brief Retrieve the code from an error condition.
 * @param [in] error  An error condition instance
 * @public @memberof cork_error_t
 * @since 0.2
 */

#if defined(CORK_DOCUMENTATION)
cork_error_code_t
cork_error_code(const cork_error_t *error);
#else
#define cork_error_code(error)  ((error)->error_code)
#endif

/**
 * @brief Retrieve the message from an error condition.
 * @param [in] error  An error condition instance
 * @public @memberof cork_error_t
 * @since 0.2
 */

#if defined(CORK_DOCUMENTATION)
const char *
cork_error_message(const cork_error_t *error);
#else
#define cork_error_message(error)  ((const char *) (error)->message.buf)
#endif

/**
 * @brief Initialize an error condition instance.
 * @param [in] alloc  A custom allocator
 * @param [out] error  The error condition to initialize
 * @public @memberof cork_error_t
 * @since 0.2
 */

bool
cork_error_init(cork_allocator_t *alloc, cork_error_t *error);

/**
 * @brief A static initializer for a new error condition instance.
 * @param [in] alloc  A custom allocator
 * @public @memberof cork_error_t
 * @since 0.2
 */

#if defined(CORK_DOCUMENTATION)
cork_error_t
CORK_ERROR_INIT(cork_allocator_t *alloc);
#else
#define CORK_ERROR_INIT(alloc)  { CORK_ERROR_NONE, 0, CORK_BUFFER_INIT(alloc) }
#endif

/**
 * @brief Finalize an error condition instance.
 * @param [out] error  The error condition to finalize
 * @public @memberof cork_error_t
 * @since 0.2
 */

void
cork_error_done(cork_error_t *error);

/**
 * @brief Fill in an error condition instance.
 *
 * If @a error is @c NULL, then we don't fill in any details about the
 * error condition.
 *
 * @param [out] error  The error condition to fill in
 * @param [in] error_class  The class of the error
 * @param [in] error_code  The code of the error
 * @param [in] format  A <tt>printf</tt>-life format string for the
 * error's detailed message
 * @param [in] ...  Any additional parameters needed by the format
 * string
 * @public @memberof cork_error_t
 * @since 0.2
 */

void
cork_error_set(cork_error_t *error,
               cork_error_class_t error_class,
               cork_error_code_t error_code,
               const char *format, ...)
    CORK_ATTR_PRINTF(4,5);

/**
 * @brief Clear an error condition instance.
 *
 * If @a error is @c NULL, then we don't do anything, since there's no
 * error condition to clear.
 *
 * @param [out] error  The error condition to clear
 * @public @memberof cork_error_t
 * @since 0.2
 */

void
cork_error_clear(cork_error_t *error);

/**
 * @brief Propagate an error condition from one instance to another.
 *
 * If @a error is @c NULL, then we just finalize @a suberror, since
 * there's no error condition to propagate into.  If @a error isn't @c
 * NULL, then we move the contents of @a suberror into @a error.  In
 * either case, @a suberror will be finalized after the function
 * returns.
 *
 * @param [out] error  The error condition to fill in
 * @param [in] suberror  The error condition to propagate
 * @public @memberof cork_error_t
 * @since 0.2
 */

void
cork_error_propagate(cork_error_t *error,
                     cork_error_t *suberror);


#endif /* LIBCORK_CORE_ERROR_H */
