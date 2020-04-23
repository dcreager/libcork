/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013, libcork authors
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CORE_ID_H
#define LIBCORK_CORE_ID_H

#include <libcork/core/attributes.h>
#include <libcork/core/hash.h>


struct cork_uid {
    const char  *name;
};

typedef const struct cork_uid  *cork_uid;

#define CORK_UID_NONE  ((cork_uid) NULL)

#define cork_uid_define_named(c_name, name) \
    static const struct cork_uid  c_name##__id = { name }; \
    static cork_uid  c_name = &c_name##__id;
#define cork_uid_define(c_name) \
    cork_uid_define_named(c_name, #c_name)

CORK_INLINE
bool
cork_uid_equal(const cork_uid id1, const cork_uid id2)
{
    return id1 == id2;
}

CORK_INLINE
cork_hash
cork_uid_hash(const cork_uid id)
{
    return (cork_hash) (uintptr_t) id;
}

CORK_INLINE
const char*
cork_uid_name(const cork_uid id)
{
    return id->name;
}


#endif /* LIBCORK_CORE_ID_H */
