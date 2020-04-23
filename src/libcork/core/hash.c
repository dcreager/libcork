/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, libcork authors
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include "libcork/core/hash.h"
#include "libcork/core/types.h"

bool
cork_big_hash_equal(const cork_big_hash h1, const cork_big_hash h2);

cork_hash
cork_stable_hash_buffer(cork_hash seed, const void *src, size_t len);

cork_hash
cork_hash_buffer(cork_hash seed, const void *src, size_t len);

cork_big_hash
cork_big_hash_buffer(cork_big_hash seed, const void *src, size_t len);
