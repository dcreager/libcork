/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#include "libcork/core/net-addresses.h"
#include "libcork/core/types.h"


/*** IPv4 ***/

bool
cork_ipv4_copy(cork_ipv4_t *addr, const void *src)
{
    memcpy(addr, src, sizeof(cork_ipv4_t));
    return true;
}

bool
cork_ipv4_init(cork_ipv4_t *addr, const char *str)
{
    return (inet_pton(AF_INET, str, addr) == 1);
}

bool
cork_ipv4_equal(const cork_ipv4_t *addr1, const cork_ipv4_t *addr2)
{
    return (memcmp(addr1, addr2, sizeof(cork_ipv4_t)) == 0);
}

void
cork_ipv4_to_raw_string(const cork_ipv4_t *addr, char *dest)
{
    snprintf(dest, CORK_IPV4_STRING_LENGTH, "%u.%u.%u.%u",
             addr->u8[0], addr->u8[1], addr->u8[2], addr->u8[3]);
}

/*** IPv6 ***/

bool
cork_ipv6_copy(cork_ipv6_t *addr, const void *src)
{
    memcpy(addr, src, sizeof(cork_ipv6_t));
    return true;
}

bool
cork_ipv6_init(cork_ipv6_t *addr, const char *str)
{
    return (inet_pton(AF_INET6, str, addr) == 1);
}

bool
cork_ipv6_equal(const cork_ipv6_t *addr1,
                const cork_ipv6_t *addr2)
{
    return (memcmp(addr1, addr2, sizeof(cork_ipv6_t)) == 0);
}

#define NS_IN6ADDRSZ 16
#define NS_INT16SZ 2

void
cork_ipv6_to_raw_string(const cork_ipv6_t *addr, char *dest)
{
    const uint8_t  *src = addr->u8;

    /*
     * Note that int32_t and int16_t need only be "at least" large enough
     * to contain a value of the specified size.  On some systems, like
     * Crays, there is no such thing as an integer variable with 16 bits.
     * Keep this in mind if you think this function should have been coded
     * to use pointer overlays.  All the world's not a VAX.
     */
    char *tp;
    struct { int base, len; } best, cur;
    unsigned int words[NS_IN6ADDRSZ / NS_INT16SZ];
    int i;

    /*
     * Preprocess:
     *      Copy the input (bytewise) array into a wordwise array.
     *      Find the longest run of 0x00's in src[] for :: shorthanding.
     */
    memset(words, '\0', sizeof words);
    for (i = 0; i < NS_IN6ADDRSZ; i++)
        words[i / 2] |= (src[i] << ((1 - (i % 2)) << 3));
    best.base = -1;
    best.len = 0;
    cur.base = -1;
    cur.len = 0;
    for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
        if (words[i] == 0) {
            if (cur.base == -1)
                cur.base = i, cur.len = 1;
            else
                cur.len++;
        } else {
            if (cur.base != -1) {
                if (best.base == -1 || cur.len > best.len)
                    best = cur;
                cur.base = -1;
            }
        }
    }
    if (cur.base != -1) {
        if (best.base == -1 || cur.len > best.len)
            best = cur;
    }
    if (best.base != -1 && best.len < 2)
        best.base = -1;

    /*
     * Format the result.
     */
    tp = dest;
    for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
        /* Are we inside the best run of 0x00's? */
        if (best.base != -1 && i >= best.base &&
            i < (best.base + best.len)) {
            if (i == best.base)
                *tp++ = ':';
            continue;
        }
        /* Are we following an initial run of 0x00s or any real hex? */
        if (i != 0)
            *tp++ = ':';
        /* Is this address an encapsulated IPv4? */
        if (i == 6 && best.base == 0 &&
            (best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
            tp += sprintf(tp, "%u.%u.%u.%u",
                          src[12], src[13], src[14], src[15]);
            break;
        }
        tp += sprintf(tp, "%x", words[i]);
    }
    /* Was it a trailing run of 0x00's? */
    if (best.base != -1 && (best.base + best.len) ==
        (NS_IN6ADDRSZ / NS_INT16SZ))
        *tp++ = ':';
    *tp++ = '\0';
}


/*** IP ***/

bool
cork_ip_from_ipv4(cork_ip_t *addr, const void *src)
{
    addr->version = 4;
    return cork_ipv4_copy(&addr->ip.v4, src);
}

bool
cork_ip_from_ipv6(cork_ip_t *addr, const void *src)
{
    addr->version = 6;
    return cork_ipv6_copy(&addr->ip.v6, src);
}

bool
cork_ip_init(cork_ip_t *addr, const char *str)
{
    if (cork_ipv4_init(&addr->ip.v4, str)) {
        addr->version = 4;
        return true;
    }

    if (cork_ipv6_init(&addr->ip.v6, str)) {
        addr->version = 6;
        return true;
    }

    return false;
}

bool
cork_ip_equal(const cork_ip_t *addr1,
              const cork_ip_t *addr2)
{
    if (addr1 == addr2) {
        return true;
    }

    if (!addr1 || !addr2) {
        return false;
    }

    if (addr1->version != addr2->version) {
        return false;
    }

    switch (addr1->version) {
        case 4:
            return cork_ipv4_equal(&addr1->ip.v4, &addr2->ip.v4);

        case 6:
            return cork_ipv6_equal(&addr1->ip.v6, &addr2->ip.v6);

        default:
            return false;
    }
}

void
cork_ip_to_raw_string(const cork_ip_t *addr, char *dest)
{
    switch (addr->version) {
        case 4:
            cork_ipv4_to_raw_string(&addr->ip.v4, dest);
            return;

        case 6:
            cork_ipv6_to_raw_string(&addr->ip.v6, dest);
            return;

        default:
            strncpy(dest, "<INVALID>", CORK_IP_STRING_LENGTH);
            return;
    }
}
