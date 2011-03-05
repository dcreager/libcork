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
 * @see basic_types
 */

/**
 * @addtogroup basic_types Basic types
 *
 * <tt>#%include &lt;libcork/core/types.h&gt;</tt>
 *
 * The types in this section ensure that the C99 integer types are
 * available, regardless of platform.  We also define some preprocessor
 * macros that give the size of the non-fixed-size standard types.
 *
 * @{
 */

/*** Add documentation for the C99 types ***/

#if defined(CORK_DOCUMENTATION)

/*
 * Note that these typedefs are never evaluated in real code; this is
 * just to trick doxygen into generating an entry for them.
 */

/** @brief A signed 8-bit integer. @since 0.0-dev */
typedef char  int8_t;
/** @brief An unsigned 8-bit integer. @since 0.0-dev */
typedef unsigned char  uint8_t;
/** @brief A signed 16-bit integer. @since 0.0-dev */
typedef short  int16_t;
/** @brief An unsigned 16-bit integer. @since 0.0-dev */
typedef unsigned short  uint16_t;
/** @brief A signed 32-bit integer. @since 0.0-dev */
typedef int  int32_t;
/** @brief An unsigned 32-bit integer. @since 0.0-dev */
typedef unsigned int  uint32_t;
/** @brief A signed 64-bit integer. @since 0.0-dev */
typedef long  int64_t;
/** @brief An unsigned 64-bit integer. @since 0.0-dev */
typedef unsigned long  uint64_t;

/**
 * @brief A signed integer big enough to hold the difference between two
 * pointers.
 * @since 0.0-dev
 */
typedef signed long  ptrdiff_t;

/**
 * @brief An unsigned integer big enough to hold an array index.
 * @since 0.0-dev
 */
typedef unsigned long  size_t;

/**
 * @brief A signed integer big enough to hold a type-cast pointer.
 * @since 0.0-dev
 */
typedef signed long  intptr_t;

/**
 * @brief An unsigned integer big enough to hold a type-case pointer.
 * @since 0.0-dev
 */
typedef unsigned long  uintptr_t;

#endif /* documentation */

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
 * @brief The size, in bytes, of the <tt>short</tt> and <tt>unsigned
 * short</tt> built-in types.
 * @since 0.0-dev
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
 * @brief The size, in bytes, of the <tt>int</tt> and <tt>unsigned
 * int</tt> built-in types.
 * @since 0.0-dev
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
 * @brief The size, in bytes, of the <tt>long</tt> and <tt>unsigned
 * long</tt> built-in types.
 * @since 0.0-dev
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
 * @brief The size, in bytes, of the <tt>void *</tt> built-in type.
 * @since 0.0-dev
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

/* end of basic_types group */
/**
 * @}
 */

#endif /* LIBCORK_CORE_TYPES_H */
