/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011-2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CORE_CHECKERS_H
#define LIBCORK_CORE_CHECKERS_H


/* This header is *not* automatically included when you include
 * libcork/core.h, since we define some macros that don't include a
 * cork_ or CORK_ prefix.  Don't want to pollute your namespace unless
 * you ask for it! */


#include <libcork/core/allocator.h>
#include <libcork/core/attributes.h>


#if !defined(CORK_PRINT_ERRORS)
#define CORK_PRINT_ERRORS 0
#endif

#if CORK_PRINT_ERRORS
#include <stdio.h>
#define CORK_PRINT_ERROR_(func, file, line) \
    fprintf(stderr, "---\nError in %s (%s:%u)\n  %s\n", \
            (func), (file), (unsigned int) (line), \
            cork_error_message());
#define CORK_PRINT_ERROR()  CORK_PRINT_ERROR_(__func__, __FILE__, __LINE__)
#else
#define CORK_PRINT_ERROR()  /* do nothing */
#endif


/* A bunch of macros for calling a function that returns an error.  If
 * an error occurs, it will automatically be propagated out as the
 * result of your own function.  With these macros, you won't have a
 * check to check or modify the error condition; it's returned as-is.
 *
 *   XZ_check
 *
 * where:
 *
 *   X = what happens if an error occurs
 *       "e" = jump to the "error" label
 *       "rY" = return a default error result (Y defined below)
 *       "x" = return an error result that you specify
 *
 *   Y = your return type
 *       "i" = int
 *       "p" = some pointer type
 *
 *   Z = the return type of the function you're calling
 *       "i" = int
 *       "p" = some pointer type
 *
 * In all cases, we assume that your function has a cork_error parameter
 * called "err".
 */


/* jump to "error" label */

#define ei_check(call) \
    do { \
        int  __rc = (call); \
        if (CORK_UNLIKELY(__rc != 0)) { \
            CORK_PRINT_ERROR(); \
            goto error; \
        } \
    } while (0)

#define ep_check(call) \
    do { \
        const void  *__result = (call); \
        if (CORK_UNLIKELY(__result == NULL)) { \
            CORK_PRINT_ERROR(); \
            goto error; \
        } \
    } while (0)


/* return specific error code */

#define xi_check(result, call) \
    do { \
        int  __rc = (call); \
        if (CORK_UNLIKELY(__rc != 0)) { \
            CORK_PRINT_ERROR(); \
            return result; \
        } \
    } while (0)

#define xp_check(result, call) \
    do { \
        const void  *__result = (call); \
        if (CORK_UNLIKELY(__result == NULL)) { \
            CORK_PRINT_ERROR(); \
            return result; \
        } \
    } while (0)


/* return default error code */

#define rii_check(call)  xi_check(__rc, call)
#define rip_check(call)  xp_check(-1, call)
#define rpi_check(call)  xi_check(NULL, call)
#define rpp_check(call)  xp_check(NULL, call)


#endif /* LIBCORK_CORE_CHECKERS_H */
