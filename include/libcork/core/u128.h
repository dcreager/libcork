/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013, libcork authors
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CORE_U128_H
#define LIBCORK_CORE_U128_H

#include <stdlib.h>

#include <libcork/config.h>
#include <libcork/core/api.h>
#include <libcork/core/attributes.h>
#include <libcork/core/byte-order.h>
#include <libcork/core/types.h>

typedef struct {
    union {
        uint8_t  u8[16];
        uint16_t  u16[8];
        uint32_t  u32[4];
        uint64_t  u64[2];
#if CORK_HOST_ENDIANNESS == CORK_BIG_ENDIAN
        struct { uint64_t hi; uint64_t lo; } be64;
#else
        struct { uint64_t lo; uint64_t hi; } be64;
#endif
#if CORK_CONFIG_HAVE_GCC_INT128
#define CORK_U128_HAVE_U128  1
        unsigned __int128  u128;
#elif CORK_CONFIG_HAVE_GCC_MODE_ATTRIBUTE
#define CORK_U128_HAVE_U128  1
        unsigned int  u128 __attribute__((mode(TI)));
#else
#define CORK_U128_HAVE_U128  0
#endif
    } _;
} cork_u128;


/* i0-3 are given in big-endian order, regardless of host endianness */
CORK_INLINE
cork_u128
cork_u128_from_32(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3)
{
    cork_u128  value;
#if CORK_HOST_ENDIANNESS == CORK_BIG_ENDIAN
    value._.u32[0] = i0;
    value._.u32[1] = i1;
    value._.u32[2] = i2;
    value._.u32[3] = i3;
#else
    value._.u32[3] = i0;
    value._.u32[2] = i1;
    value._.u32[1] = i2;
    value._.u32[0] = i3;
#endif
    return value;
}

/* i0-1 are given in big-endian order, regardless of host endianness */
CORK_INLINE
cork_u128
cork_u128_from_64(uint64_t i0, uint64_t i1)
{
    cork_u128  value;
#if CORK_HOST_ENDIANNESS == CORK_BIG_ENDIAN
    value._.u64[0] = i0;
    value._.u64[1] = i1;
#else
    value._.u64[1] = i0;
    value._.u64[0] = i1;
#endif
    return value;
}

CORK_INLINE
cork_u128
cork_u128_zero(void)
{
    return cork_u128_from_64(0, 0);
}


#if CORK_HOST_ENDIANNESS == CORK_BIG_ENDIAN
#define cork_u128_be8(val, idx)   ((val)._.u8[(idx)])
#define cork_u128_be16(val, idx)  ((val)._.u16[(idx)])
#define cork_u128_be32(val, idx)  ((val)._.u32[(idx)])
#define cork_u128_be64(val, idx)  ((val)._.u64[(idx)])
#else
#define cork_u128_be8(val, idx)   ((val)._.u8[15 - (idx)])
#define cork_u128_be16(val, idx)  ((val)._.u16[7 - (idx)])
#define cork_u128_be32(val, idx)  ((val)._.u32[3 - (idx)])
#define cork_u128_be64(val, idx)  ((val)._.u64[1 - (idx)])
#endif

#define CORK_SWAP_UINT128(__u128)                                              \
  (cork_u128_from_64(CORK_SWAP_UINT64(cork_u128_be64((__u128), 1)),            \
                     CORK_SWAP_UINT64(cork_u128_be64((__u128), 0))))

#define CORK_SWAP_IN_PLACE_UINT128(__u128) \
    do { \
        (__u128) = CORK_SWAP_UINT128(__u128); \
    } while (0)

#if CORK_HOST_ENDIANNESS == CORK_BIG_ENDIAN
#define CORK_UINT128_BIG_TO_HOST(__u128) (__u128) /* nothing to do */
#define CORK_UINT128_LITTLE_TO_HOST(__u128)  CORK_SWAP_UINT128(__u128)
#define CORK_UINT128_BIG_TO_HOST_IN_PLACE(__u128) /* nothing to do */
#define CORK_UINT128_LITTLE_TO_HOST_IN_PLACE(__u128)                           \
  CORK_SWAP_IN_PLACE_UINT128(__u128)
#elif CORK_HOST_ENDIANNESS == CORK_LITTLE_ENDIAN
#define CORK_UINT128_BIG_TO_HOST(__u128)  CORK_SWAP_UINT128(__u128)
#define CORK_UINT128_LITTLE_TO_HOST(__u128) (__u128) /* nothing to do */
#define CORK_UINT128_BIG_TO_HOST_IN_PLACE(__u128)                              \
  CORK_SWAP_IN_PLACE_UINT128(__u128)
#define CORK_UINT128_LITTLE_TO_HOST_IN_PLACE(__u128) /* nothing to do */
#endif

#define CORK_UINT128_HOST_TO_BIG(__u128) CORK_UINT128_BIG_TO_HOST(__u128)
#define CORK_UINT128_HOST_TO_LITTLE(__u128) CORK_UINT128_LITTLE_TO_HOST(__u128)
#define CORK_UINT128_HOST_TO_BIG_IN_PLACE(__u128)                              \
  CORK_UINT128_BIG_TO_HOST_IN_PLACE(__u128)
#define CORK_UINT128_HOST_TO_LITTLE_IN_PLACE(__u128)                           \
  CORK_UINT128_LITTLE_TO_HOST_IN_PLACE(__u128)

CORK_INLINE
bool
cork_u128_eq(cork_u128 a, cork_u128 b)
{
#if CORK_U128_HAVE_U128
    return (a._.u128 == b._.u128);
#else
    return (a._.be64.hi == b._.be64.hi) && (a._.be64.lo == b._.be64.lo);
#endif
}

CORK_INLINE
bool
cork_u128_ne(cork_u128 a, cork_u128 b)
{
#if CORK_U128_HAVE_U128
    return (a._.u128 != b._.u128);
#else
    return (a._.be64.hi != b._.be64.hi) || (a._.be64.lo != b._.be64.lo);
#endif
}

CORK_INLINE
bool
cork_u128_lt(cork_u128 a, cork_u128 b)
{
#if CORK_U128_HAVE_U128
    return (a._.u128 < b._.u128);
#else
    if (a._.be64.hi == b._.be64.hi) {
        return a._.be64.lo < b._.be64.lo;
    } else {
        return a._.be64.hi < b._.be64.hi;
    }
#endif
}

CORK_INLINE
bool
cork_u128_le(cork_u128 a, cork_u128 b)
{
#if CORK_U128_HAVE_U128
    return (a._.u128 <= b._.u128);
#else
    if (a._.be64.hi == b._.be64.hi) {
        return a._.be64.lo <= b._.be64.lo;
    } else {
        return a._.be64.hi <= b._.be64.hi;
    }
#endif
}

CORK_INLINE
bool
cork_u128_gt(cork_u128 a, cork_u128 b)
{
#if CORK_U128_HAVE_U128
    return (a._.u128 > b._.u128);
#else
    if (a._.be64.hi == b._.be64.hi) {
        return a._.be64.lo > b._.be64.lo;
    } else {
        return a._.be64.hi > b._.be64.hi;
    }
#endif
}

CORK_INLINE
bool
cork_u128_ge(cork_u128 a, cork_u128 b)
{
#if CORK_U128_HAVE_U128
    return (a._.u128 >= b._.u128);
#else
    if (a._.be64.hi == b._.be64.hi) {
        return a._.be64.lo >= b._.be64.lo;
    } else {
        return a._.be64.hi >= b._.be64.hi;
    }
#endif
}


CORK_INLINE
cork_u128
cork_u128_shl(cork_u128 a, unsigned int b)
{
#if CORK_U128_HAVE_U128
    cork_u128  result;
    result._.u128 = a._.u128 << b;
    return result;
#else
    if (b == 0) {
        return a;
    }
    if (b == 64) {
        return cork_u128_from_64(a._.be64.lo, 0);
    }
    if (b >= 128) {
        /* This is undefined behavior */
        return cork_u128_zero();
    }
    if (b >= 64) {
        return cork_u128_from_64(a._.be64.lo << (b - 64), 0);
    }
    return cork_u128_from_64(
        (a._.be64.hi << b) + (a._.be64.lo >> (64 - b)),
        a._.be64.lo << b);
#endif
}

CORK_INLINE
cork_u128
cork_u128_shr(cork_u128 a, unsigned int b)
{
#if CORK_U128_HAVE_U128
    cork_u128  result;
    result._.u128 = a._.u128 >> b;
    return result;
#else
    if (b == 0) {
        return a;
    }
    if (b == 64) {
        return cork_u128_from_64(0, a._.be64.hi);
    }
    if (b >= 128) {
        /* This is undefined behavior */
        return cork_u128_zero();
    }
    if (b >= 64) {
        return cork_u128_from_64(0, a._.be64.hi >> (b - 64));
    }
    return cork_u128_from_64(
        a._.be64.hi >> b,
        (a._.be64.lo >> b) + (a._.be64.hi << (64 - b)));
#endif
}


CORK_INLINE
cork_u128
cork_u128_add(cork_u128 a, cork_u128 b)
{
    cork_u128  result;
#if CORK_U128_HAVE_U128
    result._.u128 = a._.u128 + b._.u128;
#else
    result._.be64.lo = a._.be64.lo + b._.be64.lo;
    result._.be64.hi =
        a._.be64.hi + b._.be64.hi + (result._.be64.lo < a._.be64.lo);
#endif
    return result;
}

CORK_INLINE
cork_u128
cork_u128_sub(cork_u128 a, cork_u128 b)
{
    cork_u128  result;
#if CORK_U128_HAVE_U128
    result._.u128 = a._.u128 - b._.u128;
#else
    result._.be64.lo = a._.be64.lo - b._.be64.lo;
    result._.be64.hi =
        a._.be64.hi - b._.be64.hi - (result._.be64.lo > a._.be64.lo);
#endif
    return result;
}

CORK_INLINE
cork_u128
cork_u128_mul(cork_u128 lhs, cork_u128 rhs)
{
    cork_u128  result;
#if CORK_U128_HAVE_U128
    result._.u128 = lhs._.u128 * rhs._.u128;
#else
    /* Multiplication table:  (each letter is 32 bits)
     *
     *         AaBb
     *       * CcDd
     *       ------
     *           Ee = b * d
     *          Ff  = B * d
     *         Gg   = a * d
     *        Hh    = A * d
     *          Ii  = b * D
     *         Jj   = B * D
     *        Kk    = a * D
     *       Ll     = A * D
     *         Mm   = b * c
     *        Nn    = B * c
     *       Oo     = a * c         x = e
     *      Pp      = A * c         X = E + f + i                 + [carry of x]
     *        Qq    = b * C         w = F + g + I + j + m         + [carry of X]
     *       Rr     = B * C         W = G + h + J + k + M + n + q + [carry of w]
     *      Ss      = a * C         v = H + K + l + N + o + Q + r + [carry of W]
     *     Tt       = A * C         V = L + O + p + R + s         + [carry of v]
     *     --------                 u = P + S + t                 + [carry of V]
     *     UuVvWwXx                 U = T                         + [carry of U]
     */
#endif
    uint64_t A = lhs._.u32[3];
    uint64_t a = lhs._.u32[2];
    uint64_t B = lhs._.u32[1];
    uint64_t b = lhs._.u32[0];
    uint64_t C = rhs._.u32[3];
    uint64_t c = rhs._.u32[2];
    uint64_t D = rhs._.u32[1];
    uint64_t d = rhs._.u32[0];
    uint64_t Ee = b * d, E = Ee >> 32, e = Ee & 0xffffffff;
    uint64_t Ff = B * d, F = Ff >> 32, f = Ff & 0xffffffff;
    uint64_t Gg = a * d, G = Gg >> 32, g = Gg & 0xffffffff;
    uint64_t Hh = A * d,               h = Hh & 0xffffffff;
    uint64_t Ii = b * D, I = Ii >> 32, i = Ii & 0xffffffff;
    uint64_t Jj = B * D, J = Jj >> 32, j = Jj & 0xffffffff;
    uint64_t Kk = a * D,               k = Kk & 0xffffffff;
    uint64_t Mm = b * c, M = Mm >> 32, m = Mm & 0xffffffff;
    uint64_t Nn = B * c,               n = Nn & 0xffffffff;
    uint64_t Qq = b * C,               q = Qq & 0xffffffff;
    uint64_t x = e;
    uint64_t X = E + f + i                 + (x >> 32);
    uint64_t w = F + g + I + j + m         + (X >> 32);
    uint64_t W = G + h + J + k + M + n + q + (w >> 32);
    result._.u32[3] = W;
    result._.u32[2] = w;
    result._.u32[1] = X;
    result._.u32[0] = x;
    return result;
}

struct cork_u128_divmod {
    cork_u128  div;
    cork_u128  mod;
};

CORK_INLINE
struct cork_u128_divmod
cork_u128_divmod(cork_u128 a, cork_u128 b)
{
    struct cork_u128_divmod  result;
#if CORK_U128_HAVE_U128
    result.div._.u128 = a._.u128 / b._.u128;
    result.mod._.u128 = a._.u128 % b._.u128;
#else
    cork_u128  divisor;
    cork_u128  multiple;

    if (cork_u128_eq(b, cork_u128_zero())) {
        // Division by 0
        abort();
    }

    divisor = b;
    result.mod = a;
    result.div = cork_u128_zero();
    multiple = cork_u128_from_64(0, 1);

    while (cork_u128_lt(divisor, a)) {
        divisor = cork_u128_shl(divisor, 1);
        multiple = cork_u128_shr(multiple, 1);
    }

    do {
        if (cork_u128_ge(result.mod, divisor)) {
            result.mod = cork_u128_sub(result.mod, divisor);
            result.div = cork_u128_add(result.div, multiple);
        }
        divisor = cork_u128_shr(divisor, 1);
        multiple = cork_u128_shr(multiple, 1);
    } while (!cork_u128_eq(multiple, cork_u128_zero()));
#endif
    return result;
}

CORK_INLINE
cork_u128
cork_u128_div(cork_u128 a, cork_u128 b)
{
#if CORK_U128_HAVE_U128
    cork_u128  result;
    result._.u128 = a._.u128 / b._.u128;
    return result;
#else
    return cork_u128_divmod(a, b).div;
#endif
}

CORK_INLINE
cork_u128
cork_u128_mod(cork_u128 a, cork_u128 b)
{
#if CORK_U128_HAVE_U128
    cork_u128  result;
    result._.u128 = a._.u128 % b._.u128;
    return result;
#else
    return cork_u128_divmod(a, b).div;
#endif
}


/* log10(x) = log2(x) / log2(10) ~= log2(x) / 3.322 */
#define CORK_U128_DECIMAL_LENGTH  44  /* ~= 128 / 3 + 1 + 1 */

CORK_API const char *
cork_u128_to_decimal(char *buf, cork_u128 val);


#define CORK_U128_HEX_LENGTH  33

CORK_API const char *
cork_u128_to_hex(char *buf, cork_u128 val);

CORK_API const char *
cork_u128_to_padded_hex(char *buf, cork_u128 val);


#endif /* LIBCORK_CORE_U128_H */
