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

/**
 * @file
 * @brief Implementation of the @ref net_addresses submodule
 */

#include <libcork/core/error.h>
#include <libcork/core/types.h>

/**
 * @defgroup net_addresses Network addresses
 * @ingroup basic_types
 *
 * <tt>#%include \<libcork/core/net-addresses.h\></tt>
 *
 * Defines several types for storing different flavors of network
 * addresses.
 *
 * @{
 */

/*-----------------------------------------------------------------------
 * Error handling
 */

/**
 * @brief The error class for errors defined in this file
 * @since 0.2
 */
/* hash of "libcork/core/net-addresses.h" */
#define CORK_NET_ADDRESS_ERROR  0x1f76fedf

/**
 * @brief Error codes for the errors defined in this file
 * @since 0.2
 */
enum cork_net_address_error_t
{
    /** @brief An unknown error while parsing a network address. */
    CORK_NET_ADDRESS_UNKNOWN_ERROR,
    /** @brief A parse error while parsing a network address. */
    CORK_NET_ADDRESS_PARSE_ERROR
};


/*-----------------------------------------------------------------------
 * IP addresses
 */

/**
 * @brief An IPv4 address.
 * @since 0.2
 */

struct cork_ipv4 {
    /** @brief The individual octets of the address. @private */
    uint8_t  u8[4];
};

/**
 * @brief An IPv6 address.
 * @since 0.2
 */

struct cork_ipv6 {
    /** @brief The individual octets of the address. @private */
    uint8_t  u8[16];
};

/**
 * @brief A generic IP address.
 * @since 0.2
 */

struct cork_ip {
    /** @brief Which version of IP address this is. */
    unsigned int  version;
#if defined(CORK_DOCUMENTATION)
    /** @brief A union defining the possible IP address fields in a @ref
     * cork_ip. */
    union addr {
#else
    union {
#endif
        /** @brief An IPv4 address. */
        struct cork_ipv4  v4;
        /** @brief An IPv6 address. */
        struct cork_ipv6  v6;
    } ip;
    /**< @brief The actual underlying address. */
};

/**
 * @brief The maximum length of an IPv4 string, including NUL
 * terminator.
 * @showinitializer
 * @since 0.2
 */

#define CORK_IPV4_STRING_LENGTH  (sizeof "xxx.xxx.xxx.xxx")

/**
 * @brief The maximum length of an IPv6 string, including NUL
 * terminator.
 * @showinitializer
 * @since 0.2
 */

#define CORK_IPV6_STRING_LENGTH \
    (sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")

/**
 * @brief The maxmimum length of a generic IP string, including NUL
 * terminator.
 * @since 0.2
 */

#define CORK_IP_STRING_LENGTH  CORK_IPV6_STRING_LENGTH

/* end of net_addresses group */
/**
 * @}
 */

/*** IPv4 ***/

/**
 * @brief Initialize an IPv4 address from an existing address in memory.
 * The existing address must be a well-formed IP address: 4 bytes long,
 * in big-endian order.
 * @param [in] addr  The address to initialize
 * @param [in] src  The source memory location
 * @returns Whether the address was successfully initialized.
 * @public @memberof cork_ipv4
 * @since 0.2
 */

bool
cork_ipv4_copy(struct cork_ipv4 *addr, const void *src);

/**
 * @brief Initialize an IPv4 address from a string.
 *
 * If the string doesn't represent a valid IPv4 address, we will leave
 * @c addr unchanged, return @c NULL, and fill in @a error.
 *
 * @param [in] addr  The address to initialize
 * @param [in] str  The string to parse
 * @param [in] error  An error instance
 * @returns Whether the address was successfully initialized.
 * @public @memberof cork_ipv4
 * @since 0.2
 */

bool
cork_ipv4_init(struct cork_ipv4 *addr, const char *str,
               struct cork_error *error);

/**
 * @brief Check two IPv4 addresses for equality.
 * @param [in] addr1  An IPv4 address
 * @param [in] addr2  An IPv4 address
 * @return Whether the two addresses are equal.
 * @public @memberof cork_ipv4
 * @since 0.2
 */

bool
cork_ipv4_equal(const struct cork_ipv4 *addr1, const struct cork_ipv4 *addr2);

/**
 * @brief Return the string representation of an IPv4 address.
 * You provide the @c char array that we'll render the string into.  It
 * must be at least CORK_IPV4_STRING_LENGTH bytes long.
 * @param [in] addr  An IPv4 address
 * @param [out] dest  The @c char array to place the string
 * representation into
 * @public @memberof cork_ipv4
 * @since 0.2
 */

void
cork_ipv4_to_raw_string(const struct cork_ipv4 *addr, char *dest);


/*** IPv6 ***/

/**
 * @brief Initialize an IPv6 address from an existing address in memory.
 * The existing address must be a well-formed IP address: 16 bytes long,
 * in big-endian order.
 * @param [in] addr  The address to initialize
 * @param [in] src  The source memory location
 * @returns Whether the address was successfully initialized.
 * @public @memberof cork_ipv6
 * @since 0.2
 */

bool
cork_ipv6_copy(struct cork_ipv6 *addr, const void *src);

/**
 * @brief Initialize an IPv6 address from a string.
 *
 * If the string doesn't represent a valid IPv4 address, we will leave
 * @c addr unchanged, return @c NULL, and fill in @a error.
 *
 * @param [in] addr  The address to initialize
 * @param [in] str  The string to parse
 * @param [in] error  An error instance
 * @returns Whether the address was successfully initialized.
 * @public @memberof cork_ipv6
 * @since 0.2
 */

bool
cork_ipv6_init(struct cork_ipv6 *addr, const char *str,
               struct cork_error *error);

/**
 * @brief Check two IPv6 addresses for equality.
 * @param [in] addr1  An IPv6 address
 * @param [in] addr2  An IPv6 address
 * @return Whether the two addresses are equal.
 * @public @memberof cork_ipv6
 * @since 0.2
 */

bool
cork_ipv6_equal(const struct cork_ipv6 *addr1, const struct cork_ipv6 *addr2);

/**
 * @brief Return the string representation of an IPv6 address.
 * You provide the @c char array that we'll render the string into.  It
 * must be at least CORK_IPV6_STRING_LENGTH bytes long.
 * @param [in] addr  An IPv6 address
 * @param [out] dest  The @c char array to place the string
 * representation into
 * @public @memberof cork_ipv6
 * @since 0.2
 */

void
cork_ipv6_to_raw_string(const struct cork_ipv6 *addr, char *dest);


/*** Generic IP ***/

/**
 * @brief Initialize a generic IP address from an existing IPv4 address
 * in memory.
 * The existing address must be a well-formed IPv4 address: 4 bytes
 * long, in big-endian order.
 * @param [in] addr  The address to initialize
 * @param [in] src  The source IPv4 memory location
 * @returns Whether the address was successfully initialized.
 * @public @memberof cork_ip
 * @since 0.2
 */

bool
cork_ip_from_ipv4(struct cork_ip *addr, const void *src);

/**
 * @brief Initialize a generic IP address from an existing IPv6 address
 * in memory.
 * The existing address must be a well-formed IPv6 address: 16 bytes
 * long, in big-endian order.
 * @param [in] addr  The address to initialize
 * @param [in] src  The source IPv6 memory location
 * @returns Whether the address was successfully initialized.
 * @public @memberof cork_ip
 * @since 0.2
 */

bool
cork_ip_from_ipv6(struct cork_ip *addr, const void *src);

/**
 * @brief Initialize a generic IP address from a string.
 *
 * If the string doesn't represent a valid IPv4 address, we will leave
 * @c addr unchanged, return @c NULL, and fill in @a error.
 *
 * @param [in] addr  The address to initialize
 * @param [in] str  The string to parse
 * @param [in] error  An error instance
 * @returns Whether the address was successfully initialized.
 * @public @memberof cork_ip
 * @since 0.2
 */

bool
cork_ip_init(struct cork_ip *addr, const char *str, struct cork_error *error);

/**
 * @brief Check two generic IP addresses for equality.
 * @param [in] addr1  An IP address
 * @param [in] addr2  An IP address
 * @return Whether the two addresses are equal.
 * @public @memberof cork_ip
 * @since 0.2
 */

bool
cork_ip_equal(const struct cork_ip *addr1, const struct cork_ip *addr2);

/**
 * @brief Return the string representation of a generic IP address.
 * You provide the @c char array that we'll render the string into.  It
 * must be at least CORK_IP_STRING_LENGTH bytes long.
 * @param [in] addr  An IP address
 * @param [out] dest  The @c char array to place the string
 * representation into
 * @public @memberof cork_ip
 * @since 0.2
 */

void
cork_ip_to_raw_string(const struct cork_ip *addr, char *dest);


#endif /* LIBCORK_CORE_NET_ADDRESSES_H */
