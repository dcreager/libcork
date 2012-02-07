/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011-2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
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
    fprintf(stderr, "---\nError in %s (%s:%u)\n", \
            (func), (file), (unsigned int) (line));
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


/* The following macros fill in err with a CORK_CANNOT_ALLOCATE
 * error if they fail. */

#define e_check_alloc(call, desc) \
    do { \
        const void  *__result = (call); \
        if (CORK_UNLIKELY(__result == NULL)) { \
            CORK_PRINT_ERROR(); \
            cork_cannot_allocate_set(err, desc); \
            goto error; \
        } \
    } while (0)

#define x_check_alloc(result, call, desc) \
    do { \
        const void  *__result = (call); \
        if (CORK_UNLIKELY(__result == NULL)) { \
            CORK_PRINT_ERROR(); \
            cork_cannot_allocate_set(err, desc); \
            return result; \
        } \
    } while (0)

#define ri_check_alloc(call, desc)  x_check_alloc(-1, call, desc)
#define rp_check_alloc(call, desc)  x_check_alloc(NULL, call, desc)


/* The following macros can be used to allocate a new variable from the
 * heap.  If the allocation fails, we'll automatically fill in your err
 * parameter, and perform some error response action (defined above).
 * We assume you have a variable or parameter named alloc, which is the
 * custom allocator to use. */

#define e_check_new(type, var, desc) \
    e_check_alloc(var = cork_new(type), desc)
#define x_check_new(result, type, var, desc) \
    x_check_alloc(result, var = cork_new(type), desc)

#define ri_check_new(type, var, desc)  x_check_new(-1, type, var, desc)
#define rp_check_new(type, var, desc)  x_check_new(NULL, type, var, desc)


/* The following macros can be used to allocate a new garbage-collected
 * object from the heap.  If the allocation fails, we'll automatically
 * fill in your err parameter, and perform some error response action
 * (defined above).  We assume you have a variable or parameter named
 * alloc, which is the custom allocator to use. */

/* For these variants, you pass in an explicit garbage collection
 * interface name. */

#define e_check_gc_inew(type, iface, var, desc) \
    e_check_alloc(var = cork_gc_new(gc, struct type, iface), desc)
#define x_check_gc_inew(result, type, iface, var, desc) \
    x_check_alloc(result, var = cork_gc_new(gc, struct type, iface), desc)

#define ri_check_gc_inew(type, iface, var, desc) \
    x_check_gc_inew(-1, type, iface, var, desc)
#define rp_check_gc_inew(type, iface, var, desc) \
    x_check_gc_inew(NULL, type, iface, var, desc)

/* For these variants, we assume that your garbage collection interface
 * is called TYPE_NAME_gc */

#define e_check_gc_new(type, var, desc) \
    e_check_gc_inew(type, &type##__gc, var, desc)
#define x_check_gc_new(result, type, var, desc) \
    x_check_gc_inew(result, type, &type##__gc, var, desc)
#define ri_check_gc_new(type, var, desc) \
    ri_check_gc_inew(type, &type##__gc, var, desc)
#define rp_check_gc_new(type, var, desc) \
    rp_check_gc_inew(type, &type##__gc, var, desc)


#endif /* LIBCORK_CORE_CHECKERS_H */
