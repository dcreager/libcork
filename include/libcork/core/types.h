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
 * @brief Implementation of the @ref basic_types submodule
 */

/**
 * @addtogroup basic_types Basic types
 *
 * <tt>#%include \<libcork/core/types.h\></tt>
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

/** @brief A boolean. @since 0.1 */
typedef int  bool;
/** @brief The boolean “true” value. @since 0.1 */
#define true  1
/** @brief The boolean “false” value. @since 0.1 */
#define false  0

/** @brief A signed 8-bit integer. @since 0.1 */
typedef char  int8_t;
/** @brief An unsigned 8-bit integer. @since 0.1 */
typedef unsigned char  uint8_t;
/** @brief A signed 16-bit integer. @since 0.1 */
typedef short  int16_t;
/** @brief An unsigned 16-bit integer. @since 0.1 */
typedef unsigned short  uint16_t;
/** @brief A signed 32-bit integer. @since 0.1 */
typedef int  int32_t;
/** @brief An unsigned 32-bit integer. @since 0.1 */
typedef unsigned int  uint32_t;
/** @brief A signed 64-bit integer. @since 0.1 */
typedef long  int64_t;
/** @brief An unsigned 64-bit integer. @since 0.1 */
typedef unsigned long  uint64_t;

/**
 * @brief A signed integer big enough to hold the difference between two
 * pointers.
 * @since 0.1
 */
typedef signed long  ptrdiff_t;

/**
 * @brief An unsigned integer big enough to hold an array index.
 * @since 0.1
 */
typedef unsigned long  size_t;

/**
 * @brief A signed integer big enough to hold a type-cast pointer.
 * @since 0.1
 */
typedef signed long  intptr_t;

/**
 * @brief An unsigned integer big enough to hold a type-case pointer.
 * @since 0.1
 */
typedef unsigned long  uintptr_t;

#endif /* documentation */

/*
 * For now, we assume that the C99 integer types are available using the
 * standard headers.
 */

#include <limits.h>
#include <inttypes.h>
#include <stdbool.h>
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
 * @since 0.1
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
 * @since 0.1
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
 * @since 0.1
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
 * @since 0.1
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


/**
 * @brief Return a pointer to a @c struct, given a pointer to one of its
 * fields.
 * @param[in] field  A pointer to @a field_name within a @c struct of
 * type @a struct_type
 * @param[in] struct_type  The type of the @c struct to return
 * @param[in] field_name  The name of the field within @a struct_type
 * @since 0.2
 */

#if defined(CORK_DOCUMENTATION)
struct_type *
cork_container_of(field_type *field, TYPE struct_type, FIELD field_name);
#else
#define cork_container_of(field, struct_type, field_name) \
    ((struct_type *) (- offsetof(struct_type, field_name) + \
                      (void *) (field)))
#endif

/* end of basic_types group */
/**
 * @}
 */

#endif /* LIBCORK_CORE_TYPES_H */
