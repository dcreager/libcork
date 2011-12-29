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


#include <libcork/core/types.h>


typedef uint32_t  cork_hash;

cork_hash
cork_hash_buffer(cork_hash seed, const void *src, size_t len);

#define cork_hash_variable(seed, val) \
    (cork_hash_buffer((seed), &(val), sizeof((val))))


#endif /* LIBCORK_CORE_HASH_H */
