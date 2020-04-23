/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2020, libcork authors
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include "libcork/core/id.h"

/*-----------------------------------------------------------------------
 * Inline declarations
 */

bool
cork_uid_equal(const cork_uid id1, const cork_uid id2);

cork_hash
cork_uid_hash(const cork_uid id);

const char*
cork_uid_name(const cork_uid id);
