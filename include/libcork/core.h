/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CORE_H
#define LIBCORK_CORE_H

/**
 * @file
 * @brief Pulls in all features of the @ref core
 */

/**
 * @defgroup core Core module
 *
 * <tt>#%include \<libcork/core.h\></tt>
 */

/*** order of groups in documentation ***/

/**
 * @defgroup basic_types Basic types
 * @ingroup core
 */

/**
 * @defgroup byte_order Byte order
 * @ingroup core
 */

/**
 * @defgroup compiler_attrs Compiler attributes
 * @ingroup core
 */

/**
 * @defgroup allocator Custom allocation
 * @ingroup core
 */

/**
 * @defgroup error Error conditions
 * @ingroup core
 */

/**
 * @defgroup hier_alloc Hierarchical allocation
 * @ingroup core
 */

/**
 * @defgroup hashing Hash values
 * @ingroup core
 */

/**
 * @defgroup gc Reference-counted garbage collection
 * @ingroup core
 */


/*** include all of the parts ***/

#include <libcork/core/allocator.h>
#include <libcork/core/attributes.h>
#include <libcork/core/byte-order.h>
#include <libcork/core/error.h>
#include <libcork/core/gc.h>
#include <libcork/core/hash.h>
#include <libcork/core/net-addresses.h>
#include <libcork/core/timestamp.h>
#include <libcork/core/types.h>

#endif /* LIBCORK_CORE_H */
