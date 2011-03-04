/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CORE_TYPES_H
#define LIBCORK_CORE_TYPES_H

/**
 * @file
 *
 * This file contains definitions for some basic types.
 */

/*
 * For now, we assume that the C99 integer types are available using the
 * standard header.
 */

#include <limits.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

/*
 * Define preprocessor macros that contain the size of several built-in
 * types.  Again, we assume that we have the C99 definitions available.
 */

/**
 * @def CORK_SIZEOF_SHORT
 * The size, in bytes, of the <tt>short</tt> and <tt>unsigned short</tt>
 * built-in types.
 */

#if SHRT_MAX == INT8_MAX
#define CORK_SIZEOF_SHORT  1
#elif SHRT_MAX == INT16_MAX
#define CORK_SIZEOF_SHORT  2
#elif SHRT_MAX == INT32_MAX
#define CORK_SIZEOF_SHORT  4
#elif SHRT_MAX == INT64_MAX
#define CORK_SIZEOF_SHORT  8
#else
#error "Cannot determine size of short"
#endif

/**
 * @def CORK_SIZEOF_INT
 * The size, in bytes, of the <tt>int</tt> and <tt>unsigned int</tt>
 * built-in types.
 */

#if INT_MAX == INT8_MAX
#define CORK_SIZEOF_INT  1
#elif INT_MAX == INT16_MAX
#define CORK_SIZEOF_INT  2
#elif INT_MAX == INT32_MAX
#define CORK_SIZEOF_INT  4
#elif INT_MAX == INT64_MAX
#define CORK_SIZEOF_INT  8
#else
#error "Cannot determine size of int"
#endif

/**
 * @def CORK_SIZEOF_LONG
 * The size, in bytes, of the <tt>long</tt> and <tt>unsigned long</tt>
 * built-in types.
 */

#if LONG_MAX == INT8_MAX
#define CORK_SIZEOF_LONG  1
#elif LONG_MAX == INT16_MAX
#define CORK_SIZEOF_LONG  2
#elif LONG_MAX == INT32_MAX
#define CORK_SIZEOF_LONG  4
#elif LONG_MAX == INT64_MAX
#define CORK_SIZEOF_LONG  8
#else
#error "Cannot determine size of long"
#endif

/**
 * @def CORK_SIZEOF_POINTER
 * The size, in bytes, of the <tt>void *</tt>.
 */

#if INTPTR_MAX == INT8_MAX
#define CORK_SIZEOF_POINTER  1
#elif INTPTR_MAX == INT16_MAX
#define CORK_SIZEOF_POINTER  2
#elif INTPTR_MAX == INT32_MAX
#define CORK_SIZEOF_POINTER  4
#elif INTPTR_MAX == INT64_MAX
#define CORK_SIZEOF_POINTER  8
#else
#error "Cannot determine size of void *"
#endif


#endif /* LIBCORK_CORE_TYPES_H */
