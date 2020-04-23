/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, libcork authors
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CORE_NET_ADDRESSES_H
#define LIBCORK_CORE_NET_ADDRESSES_H


#include <string.h>

#include <libcork/core/api.h>
#include <libcork/core/attributes.h>
#include <libcork/core/error.h>
#include <libcork/core/types.h>


/*-----------------------------------------------------------------------
 * IP addresses
 */

struct cork_ipv4 {
    union {
        uint8_t  u8[4];
        uint16_t  u16[2];
        uint32_t  u32;
    } _;
};

struct cork_ipv6 {
    union {
        uint8_t  u8[16];
        uint16_t  u16[8];
        uint32_t  u32[4];
        uint64_t  u64[2];
    } _;
};

struct cork_ip {
    /* Which version of IP address this is. */
    unsigned int  version;
    union {
        struct cork_ipv4  v4;
        struct cork_ipv6  v6;
    } ip;
};


#define CORK_IPV4_STRING_LENGTH  (sizeof "xxx.xxx.xxx.xxx")
#define CORK_IPV6_STRING_LENGTH \
    (sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")
#define CORK_IP_STRING_LENGTH  CORK_IPV6_STRING_LENGTH


/*** IPv4 ***/

/* src must be well-formed: 4 bytes, big-endian */
CORK_INLINE
void
cork_ipv4_copy(struct cork_ipv4* addr, const void* src)
{
    memcpy(addr, src, sizeof(struct cork_ipv4));
}

CORK_INLINE
bool
cork_ipv4_equal(const struct cork_ipv4* addr1, const struct cork_ipv4* addr2)
{
    return addr1->_.u32 == addr2->_.u32;
}

CORK_API int
cork_ipv4_init(struct cork_ipv4 *addr, const char *str);

CORK_API bool
cork_ipv4_equal_(const struct cork_ipv4 *addr1, const struct cork_ipv4 *addr2);

CORK_API void
cork_ipv4_to_raw_string(const struct cork_ipv4 *addr, char *dest);

CORK_API bool
cork_ipv4_is_valid_network(const struct cork_ipv4 *addr,
                           unsigned int cidr_prefix);


/*** IPv6 ***/

/* src must be well-formed: 16 bytes, big-endian */
CORK_INLINE
void
cork_ipv6_copy(struct cork_ipv6* addr, const void* src)
{
    memcpy(addr, src, sizeof(struct cork_ipv6));
}

CORK_INLINE
bool
cork_ipv6_equal(const struct cork_ipv6* addr1, const struct cork_ipv6* addr2)
{
    return addr1->_.u64[0] == addr2->_.u64[0] &&
        addr1->_.u64[1] == addr2->_.u64[1];
}

CORK_API int
cork_ipv6_init(struct cork_ipv6 *addr, const char *str);

CORK_API bool
cork_ipv6_equal_(const struct cork_ipv6 *addr1, const struct cork_ipv6 *addr2);

CORK_API void
cork_ipv6_to_raw_string(const struct cork_ipv6 *addr, char *dest);

CORK_API bool
cork_ipv6_is_valid_network(const struct cork_ipv6 *addr,
                           unsigned int cidr_prefix);


/*** Generic IP ***/

CORK_INLINE
bool
cork_ip_equal(const struct cork_ip* addr1, const struct cork_ip* addr2)
{
    if (addr1->version != addr2->version)
        return false;
    else if (addr1->version == 4) {
        return cork_ipv4_equal(&addr1->ip.v4, &addr2->ip.v4);
    } else {
        return cork_ipv6_equal(&addr1->ip.v6, &addr2->ip.v6);
    }
}

/* src must be well-formed: 4 bytes, big-endian */
CORK_INLINE
void
cork_ip_from_ipv4(struct cork_ip* addr, const void* src)
{
    addr->version = 4;
    cork_ipv4_copy(&addr->ip.v4, src);
}

/* src must be well-formed: 16 bytes, big-endian */
CORK_INLINE
void
cork_ip_from_ipv6(struct cork_ip* addr, const void* src)
{
    addr->version = 6;
    cork_ipv6_copy(&addr->ip.v6, src);
}

CORK_API int
cork_ip_init(struct cork_ip *addr, const char *str);

CORK_API void
cork_ip_to_raw_string(const struct cork_ip *addr, char *dest);

CORK_API bool
cork_ip_is_valid_network(const struct cork_ip *addr, unsigned int cidr_prefix);


#endif /* LIBCORK_CORE_NET_ADDRESSES_H */
