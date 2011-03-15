/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CORE_BYTE_ORDER_H
#define LIBCORK_CORE_BYTE_ORDER_H

/**
 * @file
 * @brief Implementation of the @ref byte_order submodule
 */


#include <libcork/config.h>
#include <libcork/core/types.h>

/**
 * @addtogroup byte_order Byte order
 *
 * <tt>#%include \<libcork/core/byte-order.h\></tt>
 *
 * This section contains definitions for determining the endianness of
 * the host system, and for byte-swapping integer values of various
 * sizes.
 *
 * @{
 */


/*---------------------------------------------------------------------*/
/**
 * @defgroup endianness Endianness detection
 * @ingroup byte_order
 *
 * <tt>#%include \<libcork/core/byte-order.h\></tt>
 *
 * The macros in this section can be used to determine the endianness of
 * the current system.  We also provide a C string constant for the
 * host's endianness, as well as these values for the <i>opposite</i>
 * endianness.
 *
 * @{
 */

/**
 * @brief A constant that represents a big-endian system.
 * @since 0.0-dev
 */

#define CORK_BIG_ENDIAN  4321

/**
 * @brief A constant that represents a little-endian system.
 * @since 0.0-dev
 */

#define CORK_LITTLE_ENDIAN  1234

/**
 * @def CORK_HOST_ENDIANNESS
 * @brief A constant macro giving the endianness of the host system.
 *
 * This will be equal to either @ref CORK_BIG_ENDIAN or @ref
 * CORK_LITTLE_ENDIAN.
 *
 * @since 0.0-dev
 */

/**
 * @def CORK_OTHER_ENDIANNESS
 * @brief A constant macro giving the endianness that the host system is
 * <b>not</b>.
 *
 * This will be equal to either @ref CORK_BIG_ENDIAN or @ref
 * CORK_LITTLE_ENDIAN.
 *
 * @since 0.0-dev
 */

/**
 * @def CORK_HOST_ENDIANNESS_NAME
 * @brief A string constant containing the endianness of the host
 * system.
 * @since 0.0-dev
 */

/**
 * @def CORK_OTHER_ENDIANNESS_NAME
 * @brief A string constant containing the endianness that the host
 * system is <b>not</b>.
 * @since 0.0-dev
 */


/*** A cheat for the documentation ***/

#if defined(CORK_DOCUMENTATION)
#define CORK_HOST_ENDIANNESS  CORK_LITTLE_ENDIAN
#define CORK_OTHER_ENDIANNESS  CORK_BIG_ENDIAN
#define CORK_HOST_ENDIANNESS_NAME   "little"
#define CORK_OTHER_ENDIANNESS_NAME  "big"

#elif CORK_CONFIG_IS_BIG_ENDIAN
#define CORK_HOST_ENDIANNESS  CORK_BIG_ENDIAN
#define CORK_OTHER_ENDIANNESS  CORK_LITTLE_ENDIAN
#define CORK_HOST_ENDIANNESS_NAME   "big"
#define CORK_OTHER_ENDIANNESS_NAME  "little"

#elif CORK_CONFIG_IS_LITTLE_ENDIAN
#define CORK_HOST_ENDIANNESS  CORK_LITTLE_ENDIAN
#define CORK_OTHER_ENDIANNESS  CORK_BIG_ENDIAN
#define CORK_HOST_ENDIANNESS_NAME   "little"
#define CORK_OTHER_ENDIANNESS_NAME  "big"

#else
#error "Unknown endianness"
#endif

/* end of byteswap group */
/**
 * @}
 */


/*---------------------------------------------------------------------*/
/**
 * @defgroup byteswap Byte swapping macros
 * @ingroup byte_order
 *
 * <tt>#%include \<libcork/core/byte-order.h\></tt>
 *
 * The macros in this section can be used to swap integer values of
 * various sizes.  There are a couple variations for each integer types:
 * one for converting between host-endianness and either big- or
 * little-endianness; and one for an explicit swap regardless of the
 * underlying endianness.
 *
 * @{
 */


/**
 * @brief Returns the byte-swapped version a 16-bit integer, regardless
 * of the underlying endianness.
 *
 * This macro only requires an rvalue as its parameter (so it can be an
 * expression), and doesn't modify the original contents if it happens
 * to be a variable.
 *
 * @param[in] __u16  a 16-bit integer value
 * @returns  the same integer value, in reversed byte order
 * @since 0.0-dev
 */

#define CORK_SWAP_UINT16(__u16) \
    (((((uint16_t) __u16) & 0xff00u) >> 8) | \
     ((((uint16_t) __u16) & 0x00ffu) << 8))

/**
 * @brief Returns the byte-swapped version of a 32-bit integer,
 * regardless of the underlying endianness.
 *
 * This macro only requires an rvalue as its parameter (so it can be an
 * expression), and doesn't modify the original contents if it happens
 * to be a variable.
 *
 * @param[in] __u32  a 32-bit integer value
 * @returns  the same integer value, in reversed byte order
 * @since 0.0-dev
 */

#define CORK_SWAP_UINT32(__u32) \
    (((((uint32_t) __u32) & 0xff000000u) >> 24) | \
     ((((uint32_t) __u32) & 0x00ff0000u) >>  8) | \
     ((((uint32_t) __u32) & 0x0000ff00u) <<  8) | \
     ((((uint32_t) __u32) & 0x000000ffu) << 24))

/**
 * @brief Returns the byte-swapped version of a 64-bit integer,
 * regardless of the underlying endianness.
 *
 * This macro only requires an rvalue as its parameter (so it can be an
 * expression), and doesn't modify the original contents if it happens
 * to be a variable.
 *
 * @param[in] __u64  a 64-bit integer value
 * @returns  the same integer value, in reversed byte order
 * @since 0.0-dev
 */

#define CORK_SWAP_UINT64(__u64) \
    (((((uint64_t) __u64) & UINT64_C(0xff00000000000000)) >> 56) | \
     ((((uint64_t) __u64) & UINT64_C(0x00ff000000000000)) >> 40) | \
     ((((uint64_t) __u64) & UINT64_C(0x0000ff0000000000)) >> 24) | \
     ((((uint64_t) __u64) & UINT64_C(0x000000ff00000000)) >>  8) | \
     ((((uint64_t) __u64) & UINT64_C(0x00000000ff000000)) <<  8) | \
     ((((uint64_t) __u64) & UINT64_C(0x0000000000ff0000)) << 24) | \
     ((((uint64_t) __u64) & UINT64_C(0x000000000000ff00)) << 40) | \
     ((((uint64_t) __u64) & UINT64_C(0x00000000000000ff)) << 56))

/**
 * @brief Bytes-swaps a 16-bit integer variable in place.
 *
 * This macro requires an lvalue as its parameter; the contents of this
 * variable will be modified by the macro.
 *
 * @param[in,out] __u16  a 16-bit integer variable
 * @returns  nothing
 * @since 0.0-dev
 */

#define CORK_SWAP_IN_PLACE_UINT16(__u16) \
    do { \
        (__u16) = CORK_SWAP_UINT16(__u16); \
    } while (0)

/**
 * @brief Bytes-swaps a 32-bit integer variable in place.
 *
 * This macro requires an lvalue as its parameter; the contents of this
 * variable will be modified by the macro.
 *
 * @param[in,out] __u32  a 32-bit integer variable
 * @returns  nothing
 * @since 0.0-dev
 */

#define CORK_SWAP_IN_PLACE_UINT32(__u32) \
    do { \
        (__u32) = CORK_SWAP_UINT32(__u32); \
    } while (0)

/**
 * @brief Bytes-swaps a 64-bit integer variable in place.
 *
 * This macro requires an lvalue as its parameter; the contents of this
 * variable will be modified by the macro.
 *
 * @param[in,out] __u64  a 64-bit integer variable
 * @returns  nothing
 * @since 0.0-dev
 */

#define CORK_SWAP_IN_PLACE_UINT64(__u64) \
    do { \
        (__u64) = CORK_SWAP_UINT64(__u64); \
    } while (0)


/**
 * @def CORK_UINT16_BIG_TO_HOST(u16)
 * @brief Converts a 16-bit big-endian integer value to host endianness,
 * returning the result.
 *
 * @param[in] __u16  a 16-bit big-endian integer value
 * @returns  the same integer value, in host byte order
 * @since 0.0-dev
 */

/**
 * @def CORK_UINT32_BIG_TO_HOST(u32)
 * @brief Converts a 32-bit big-endian integer value to host endianness,
 * returning the result.
 *
 * @param[in] __u32  a 32-bit big-endian integer value
 * @returns  the same integer value, in host byte order
 * @since 0.0-dev
 */

/**
 * @def CORK_UINT64_BIG_TO_HOST(u64)
 * @brief Converts a 64-bit big-endian integer value to host endianness,
 * returning the result.
 *
 * @param[in] __u64  a 64-bit big-endian integer value
 * @returns  the same integer value, in host byte order
 * @since 0.0-dev
 */

/**
 * @def CORK_UINT16_LITTLE_TO_HOST(u16)
 * @brief Converts a 16-bit little-endian integer value to host
 * endianness, returning the result.
 *
 * @param[in] __u16  a 16-bit little-endian integer value
 * @returns  the same integer value, in host byte order
 * @since 0.0-dev
 */

/**
 * @def CORK_UINT32_LITTLE_TO_HOST(u32)
 * @brief Converts a 32-bit little-endian integer value to host
 * endianness, returning the result.
 *
 * @param[in] __u32  a 32-bit little-endian integer value
 * @returns  the same integer value, in host byte order
 * @since 0.0-dev
 */

/**
 * @def CORK_UINT64_LITTLE_TO_HOST(u64)
 * @brief Converts a 64-bit little-endian integer value to host
 * endianness, returning the result.
 *
 * @param[in] __u64  a 64-bit little-endian integer value
 * @returns  the same integer value, in host byte order
 * @since 0.0-dev
 */


/**
 * @def CORK_UINT16_BIG_TO_HOST_IN_PLACE(u16)
 * @brief Converts a 16-bit big-endian integer value to host endianness
 * in place, replacing the contents of the old value.
 *
 * @param[in,out] __u16  a 16-bit big-endian integer variable
 * @returns  nothing
 * @since 0.0-dev
 */

/**
 * @def CORK_UINT32_BIG_TO_HOST_IN_PLACE(u32)
 * @brief Converts a 32-bit big-endian integer value to host endianness
 * in place, replacing the contents of the old value.
 *
 * @param[in,out] __u32  a 32-bit big-endian integer variable
 * @returns  nothing
 * @since 0.0-dev
 */

/**
 * @def CORK_UINT64_BIG_TO_HOST_IN_PLACE(u64)
 * @brief Converts a 64-bit big-endian integer value to host endianness
 * in place, replacing the contents of the old value.
 *
 * @param[in,out] __u64  a 64-bit big-endian integer variable
 * @returns  nothing
 * @since 0.0-dev
 */

/**
 * @def CORK_UINT16_LITTLE_TO_HOST_IN_PLACE(u16)
 * @brief Converts a 16-bit little-endian integer value to host
 * endianness in place, replacing the contents of the old value.
 *
 * @param[in,out] __u16  a 16-bit little-endian integer variable
 * @returns  nothing
 * @since 0.0-dev
 */

/**
 * @def CORK_UINT32_LITTLE_TO_HOST_IN_PLACE(u32)
 * @brief Converts a 32-bit little-endian integer value to host
 * endianness in place, replacing the contents of the old value.
 *
 * @param[in,out] __u32  a 32-bit little-endian integer variable
 * @returns  nothing
 * @since 0.0-dev
 */

/**
 * @def CORK_UINT64_LITTLE_TO_HOST_IN_PLACE(u64)
 * @brief Converts a 64-bit little-endian integer value to host
 * endianness in place, replacing the contents of the old value.
 *
 * @param[in,out] __u64  a 64-bit little-endian integer variable
 * @returns  nothing
 * @since 0.0-dev
 */

#if CORK_HOST_ENDIANNESS == CORK_BIG_ENDIAN

#define CORK_UINT16_BIG_TO_HOST(__u16) (__u16) /* nothing to do */
#define CORK_UINT16_LITTLE_TO_HOST(__u16)  CORK_SWAP_UINT16(__u16)

#define CORK_UINT32_BIG_TO_HOST(__u32) (__u32) /* nothing to do */
#define CORK_UINT32_LITTLE_TO_HOST(__u32)  CORK_SWAP_UINT32(__u32)

#define CORK_UINT64_BIG_TO_HOST(__u64) (__u64) /* nothing to do */
#define CORK_UINT64_LITTLE_TO_HOST(__u64)  CORK_SWAP_UINT64(__u64)

#define CORK_UINT16_BIG_TO_HOST_IN_PLACE(__u16) /* nothing to do */
#define CORK_UINT16_LITTLE_TO_HOST_IN_PLACE(__u16)  CORK_SWAP_IN_PLACE_UINT16(__u16)

#define CORK_UINT32_BIG_TO_HOST_IN_PLACE(__u32) /* nothing to do */
#define CORK_UINT32_LITTLE_TO_HOST_IN_PLACE(__u32)  CORK_SWAP_IN_PLACE_UINT32(__u32)

#define CORK_UINT64_BIG_TO_HOST_IN_PLACE(__u64) /* nothing to do */
#define CORK_UINT64_LITTLE_TO_HOST_IN_PLACE(__u64)  CORK_SWAP_IN_PLACE_UINT64(__u64)

#elif CORK_HOST_ENDIANNESS == CORK_LITTLE_ENDIAN

#define CORK_UINT16_BIG_TO_HOST(__u16)  CORK_SWAP_UINT16(__u16)
#define CORK_UINT16_LITTLE_TO_HOST(__u16) (__u16) /* nothing to do */

#define CORK_UINT32_BIG_TO_HOST(__u32)  CORK_SWAP_UINT32(__u32)
#define CORK_UINT32_LITTLE_TO_HOST(__u32) (__u32) /* nothing to do */

#define CORK_UINT64_BIG_TO_HOST(__u64)  CORK_SWAP_UINT64(__u64)
#define CORK_UINT64_LITTLE_TO_HOST(__u64) (__u64) /* nothing to do */

#define CORK_UINT16_BIG_TO_HOST_IN_PLACE(__u16)  CORK_SWAP_IN_PLACE_UINT16(__u16)
#define CORK_UINT16_LITTLE_TO_HOST_IN_PLACE(__u16) /* nothing to do */

#define CORK_UINT32_BIG_TO_HOST_IN_PLACE(__u32)  CORK_SWAP_IN_PLACE_UINT32(__u32)
#define CORK_UINT32_LITTLE_TO_HOST_IN_PLACE(__u32) /* nothing to do */

#define CORK_UINT64_BIG_TO_HOST_IN_PLACE(__u64)  CORK_SWAP_IN_PLACE_UINT64(__u64)
#define CORK_UINT64_LITTLE_TO_HOST_IN_PLACE(__u64) /* nothing to do */

#endif


/**
 * @brief Converts a 16-bit host-endian integer value to big endianness,
 * returning the result.
 *
 * @param[in] __u16  a 16-bit host-endian integer value
 * @returns  the same integer value, in big-endian byte order
 * @since 0.0-dev
 */

#define CORK_UINT16_HOST_TO_BIG(__u16)  CORK_UINT16_BIG_TO_HOST(__u16)

/**
 * @brief Converts a 32-bit host-endian integer value to big endianness,
 * returning the result.
 *
 * @param[in] __u32  a 32-bit host-endian integer value
 * @returns  the same integer value, in big-endian byte order
 * @since 0.0-dev
 */

#define CORK_UINT32_HOST_TO_BIG(__u32)  CORK_UINT32_BIG_TO_HOST(__u32)

/**
 * @brief Converts a 64-bit host-endian integer value to big endianness,
 * returning the result.
 *
 * @param[in] __u64  a 64-bit host-endian integer value
 * @returns  the same integer value, in big-endian byte order
 * @since 0.0-dev
 */

#define CORK_UINT64_HOST_TO_BIG(__u64)  CORK_UINT64_BIG_TO_HOST(__u64)

/**
 * @brief Converts a 16-bit host-endian integer value to little
 * endianness, returning the result.
 *
 * @param[in] __u16  a 16-bit host-endian integer value
 * @returns  the same integer value, in little-endian byte order
 * @since 0.0-dev
 */

#define CORK_UINT16_HOST_TO_LITTLE(__u16)  CORK_UINT16_LITTLE_TO_HOST(__u16)

/**
 * @brief Converts a 32-bit host-endian integer value to little
 * endianness, returning the result.
 *
 * @param[in] __u32  a 32-bit host-endian integer value
 * @returns  the same integer value, in little-endian byte order
 * @since 0.0-dev
 */

#define CORK_UINT32_HOST_TO_LITTLE(__u32)  CORK_UINT32_LITTLE_TO_HOST(__u32)

/**
 * @brief Converts a 64-bit host-endian integer value to little
 * endianness, returning the result.
 *
 * @param[in] __u64  a 64-bit host-endian integer value
 * @returns  the same integer value, in little-endian byte order
 * @since 0.0-dev
 */

#define CORK_UINT64_HOST_TO_LITTLE(__u64)  CORK_UINT64_LITTLE_TO_HOST(__u64)


/**
 * @brief Converts a 16-bit host-endian integer value to big endianness
 * in place, replacing the contents of the old value.
 *
 * @param[in,out] __u16  a 16-bit host-endian integer variable
 * @returns  nothing
 * @since 0.0-dev
 */

#define CORK_UINT16_HOST_TO_BIG_IN_PLACE(__u16)  CORK_UINT16_BIG_TO_HOST_IN_PLACE(__u16)

/**
 * @brief Converts a 32-bit host-endian integer value to big endianness
 * in place, replacing the contents of the old value.
 *
 * @param[in,out] __u32  a 32-bit host-endian integer variable
 * @returns  nothing
 * @since 0.0-dev
 */

#define CORK_UINT32_HOST_TO_BIG_IN_PLACE(__u32)  CORK_UINT32_BIG_TO_HOST_IN_PLACE(__u32)

/**
 * @brief Converts a 64-bit host-endian integer value to big endianness
 * in place, replacing the contents of the old value.
 *
 * @param[in,out] __u64  a 64-bit host-endian integer variable
 * @returns  nothing
 * @since 0.0-dev
 */

#define CORK_UINT64_HOST_TO_BIG_IN_PLACE(__u64)  CORK_UINT64_BIG_TO_HOST_IN_PLACE(__u64)

/**
 * @brief Converts a 16-bit host-endian integer value to little
 * endianness in place, replacing the contents of the old value.
 *
 * @param[in,out] __u16  a 16-bit host-endian integer variable
 * @returns  nothing
 * @since 0.0-dev
 */

#define CORK_UINT16_HOST_TO_LITTLE_IN_PLACE(__u16)  CORK_UINT16_LITTLE_TO_HOST_IN_PLACE(__u16)

/**
 * @brief Converts a 32-bit host-endian integer value to little
 * endianness in place, replacing the contents of the old value.
 *
 * @param[in,out] __u32  a 32-bit host-endian integer variable
 * @returns  nothing
 * @since 0.0-dev
 */

#define CORK_UINT32_HOST_TO_LITTLE_IN_PLACE(__u32)  CORK_UINT32_LITTLE_TO_HOST_IN_PLACE(__u32)

/**
 * @brief Converts a 64-bit host-endian integer value to little
 * endianness in place, replacing the contents of the old value.
 *
 * @param[in,out] __u64  a 64-bit host-endian integer variable
 * @returns  nothing
 * @since 0.0-dev
 */

#define CORK_UINT64_HOST_TO_LITTLE_IN_PLACE(__u64)  CORK_UINT64_LITTLE_TO_HOST_IN_PLACE(__u64)


/* end of byteswap group */
/**
 * @}
 */


/* end of byte_order group */
/**
 * @}
 */

#endif /* LIBCORK_CORE_BYTE_ORDER_H */
