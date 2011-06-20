/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CORE_ATTRIBUTES_H
#define LIBCORK_CORE_ATTRIBUTES_H

/**
 * @file
 * @brief Implementation of the @ref compiler_attrs submodule
 */

#include <libcork/config.h>

/**
 * @addtogroup compiler_attrs
 *
 * <tt>#%include \<libcork/core/attributes.h\></tt>
 *
 * The macros in this section define compiler-agnostic versions of
 * several compiler attributes.
 *
 * @{
 */


/**
 * @brief Declare a “const” function.
 *
 * A const function is one whose return value depends only on its
 * parameters.  This is slightly more strict than a “pure” function; a
 * const function is not allowed to read from global variables, whereas
 * a pure function is.
 *
 * @code
 * int square(int x) CORK_ATTR_CONST;
 * @endcode
 *
 * @since 0.1
 */

#if defined(CORK_DOCUMENTATION)
#define CORK_ATTR_CONST
#elif CORK_CONFIG_HAVE_GCC_ATTRIBUTES
#define CORK_ATTR_CONST  __attribute__((const))
#else
#define CORK_ATTR_CONST
#endif


/**
 * @brief Declare a “pure” function.
 *
 * A pure function is one whose return value depends only on its
 * parameters, and global variables.
 *
 * @code
 * int square(int x) CORK_ATTR_PURE;
 * @endcode
 *
 * @since 0.1
 */

#if defined(CORK_DOCUMENTATION)
#define CORK_ATTR_PURE
#elif CORK_CONFIG_HAVE_GCC_ATTRIBUTES
#define CORK_ATTR_PURE  __attribute__((pure))
#else
#define CORK_ATTR_PURE
#endif


/**
 * @brief Declare that a function returns a newly allocated pointer.
 *
 * The compiler can use this information to generate more accurate
 * aliasing information, since it can infer that the result of the
 * function cannot alias any other existing pointer.
 *
 * @since 0.1
 */

#if defined(CORK_DOCUMENTATION)
#define CORK_ATTR_MALLOC
#elif CORK_CONFIG_HAVE_GCC_ATTRIBUTES
#define CORK_ATTR_MALLOC  __attribute__((malloc))
#else
#define CORK_ATTR_MALLOC
#endif


/**
 * @brief Declare an entity that isn't used.
 *
 * This lets you keep <tt>-Wall</tt> activated in several cases where
 * you're obligated to define something that you don't intend to use.
 *
 * @since 0.1
 */

#if defined(CORK_DOCUMENTATION)
#define CORK_ATTR_UNUSED
#elif CORK_CONFIG_HAVE_GCC_ATTRIBUTES
#define CORK_ATTR_UNUSED  __attribute__((unused))
#else
#define CORK_ATTR_UNUSED
#endif


/* end of compiler_attrs group */
/**
 * @}
 */

#endif /* LIBCORK_CORE_ATTRIBUTES_H */
