/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include "libcork/core/types.h"
#include "libcork/threads/basics.h"

struct cork_thread_descriptor {
    cork_thread_id  id;
};

static volatile cork_thread_id  last_thread_descriptor = 0;

cork_tls(struct cork_thread_descriptor, cork_thread_descriptor);

cork_thread_id
cork_thread_get_id(void)
{
    struct cork_thread_descriptor  *desc = cork_thread_descriptor_get();
    if (CORK_UNLIKELY(desc->id) == 0) {
        desc->id = cork_uint_atomic_add(&last_thread_descriptor, 1);
    }
    return desc->id;
}
