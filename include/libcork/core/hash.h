/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CORE_HASH_H
#define LIBCORK_CORE_HASH_H

/**
 * @file
 * @brief Implementation of the @ref hashing submodule
 */

#include <libcork/core/types.h>

/**
 * @addtogroup hashing Hash values
 *
 * <tt>#%include \<libcork/core/hash.h\></tt>
 *
 * Provides several functions for producing fast, good hashes.  The
 * implementation provided by these functions can change over time, and
 * doesn't have to be consistent across platforms.  The only guarantee
 * is that hash values will be consistent for the duration of the
 * current process.
 *
 * @{
 */

/**
 * @brief A hash value
 * @since 0.2
 */

typedef uint32_t  cork_hash;

/**
 * @brief Incorporate the contents of the given binary buffer to the
 * hash value.
 * @param[in] seed  The current accumulated hash value
 * @param[in] src  The binary buffer to incorporate into the hash
 * @param[in] len  The length of @a src
 * @returns  a new hash value
 * @since 0.2
 */

cork_hash
cork_hash_buffer(cork_hash seed, const void *src, size_t len);

/**
 * @brief Incorporate the contents of a variable into the hash value.
 *
 * @a val must be an lvalue in the current scope.
 *
 * @param[in] seed  The current accumulated hash value
 * @param[in] val  A variable to add to the hash
 * @returns  a new hash value
 * @since 0.2
 */

#if defined(CORK_DOCUMENTATION)
cork_hash
cork_hash_variable(cork_hash seed, TYPE val);
#else
#define cork_hash_variable(seed, val) \
    (cork_hash_buffer((seed), &(val), sizeof((val))))
#endif

/* end of basic_types group */
/**
 * @}
 */

#endif /* LIBCORK_CORE_HASH_H */
