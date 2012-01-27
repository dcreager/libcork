/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CORE_NET_ADDRESSES_H
#define LIBCORK_CORE_NET_ADDRESSES_H


#include <string.h>

#include <libcork/core/error.h>
#include <libcork/core/types.h>


/*-----------------------------------------------------------------------
 * Error handling
 */

/* hash of "libcork/core/net-addresses.h" */
#define CORK_NET_ADDRESS_ERROR  0x1f76fedf

enum cork_net_address_error {
    /* A parse error while parsing a network address. */
    CORK_NET_ADDRESS_PARSE_ERROR
};


/*-----------------------------------------------------------------------
 * IP addresses
 */

struct cork_ipv4 {
    uint8_t  u8[4];
};

struct cork_ipv6 {
    uint8_t  u8[16];
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
#define cork_ipv4_copy(addr, src) \
    (memcpy((addr), (src), sizeof(struct cork_ipv4)))

int
cork_ipv4_init(struct cork_ipv4 *addr, const char *str,
               struct cork_error *error);

bool
cork_ipv4_equal(const struct cork_ipv4 *addr1, const struct cork_ipv4 *addr2);

void
cork_ipv4_to_raw_string(const struct cork_ipv4 *addr, char *dest);


/*** IPv6 ***/

/* src must be well-formed: 16 bytes, big-endian */
#define cork_ipv6_copy(addr, src) \
    (memcpy((addr), (src), sizeof(struct cork_ipv6)))

int
cork_ipv6_init(struct cork_ipv6 *addr, const char *str,
               struct cork_error *error);

bool
cork_ipv6_equal(const struct cork_ipv6 *addr1, const struct cork_ipv6 *addr2);

void
cork_ipv6_to_raw_string(const struct cork_ipv6 *addr, char *dest);


/*** Generic IP ***/

/* src must be well-formed: 4 bytes, big-endian */
void
cork_ip_from_ipv4(struct cork_ip *addr, const void *src);

/* src must be well-formed: 16 bytes, big-endian */
void
cork_ip_from_ipv6(struct cork_ip *addr, const void *src);

int
cork_ip_init(struct cork_ip *addr, const char *str, struct cork_error *error);

bool
cork_ip_equal(const struct cork_ip *addr1, const struct cork_ip *addr2);

void
cork_ip_to_raw_string(const struct cork_ip *addr, char *dest);


#endif /* LIBCORK_CORE_NET_ADDRESSES_H */
