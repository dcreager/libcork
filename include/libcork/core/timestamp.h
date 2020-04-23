/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, libcork authors
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CORE_TIMESTAMP_H
#define LIBCORK_CORE_TIMESTAMP_H


#include <libcork/core/api.h>
#include <libcork/core/error.h>
#include <libcork/core/types.h>
#include <libcork/ds/buffer.h>


typedef uint64_t  cork_timestamp;


CORK_INLINE
void
cork_timestamp_init_sec(cork_timestamp* ts, uint64_t sec)
{
    *ts = sec << 32;
}

CORK_INLINE
void
cork_timestamp_init_gsec(cork_timestamp* ts, uint64_t sec, uint64_t gsec)
{
    *ts = (sec << 32) | (gsec & 0xffffffff);
}

CORK_INLINE
void
cork_timestamp_init_msec(cork_timestamp* ts, uint64_t sec, uint64_t msec)
{
    *ts = (sec << 32) | ((msec << 32) / 1000);
}

CORK_INLINE
void
cork_timestamp_init_usec(cork_timestamp* ts, uint64_t sec, uint64_t usec)
{
    *ts = (sec << 32) | ((usec << 32) / 1000000);
}

CORK_INLINE
void
cork_timestamp_init_nsec(cork_timestamp* ts, uint64_t sec, uint64_t nsec)
{
    *ts = (sec << 32) | ((nsec << 32) / 1000000000);
}


CORK_API void
cork_timestamp_init_now(cork_timestamp *ts);


CORK_INLINE
uint32_t
cork_timestamp_sec(const cork_timestamp ts)
{
    return (uint32_t) (ts >> 32);
}

CORK_INLINE
uint32_t
cork_timestamp_gsec(const cork_timestamp ts)
{
    return (uint32_t) (ts & 0xffffffff);
}

CORK_INLINE
uint64_t
cork_timestamp_gsec_to_units(const cork_timestamp ts, uint64_t denom)
{
    uint64_t  half = ((uint64_t) 1 << 31) / denom;
    uint64_t  gsec = cork_timestamp_gsec(ts);
    gsec += half;
    gsec *= denom;
    gsec >>= 32;
    return gsec;
}

CORK_INLINE
uint64_t
cork_timestamp_msec(const cork_timestamp ts)
{
    return cork_timestamp_gsec_to_units(ts, 1000);
}

CORK_INLINE
uint64_t
cork_timestamp_usec(const cork_timestamp ts)
{
    return cork_timestamp_gsec_to_units(ts, 1000000);
}

CORK_INLINE
uint64_t
cork_timestamp_nsec(const cork_timestamp ts)
{
    return cork_timestamp_gsec_to_units(ts, 1000000000);
}


CORK_API int
cork_timestamp_format_utc(const cork_timestamp ts, const char *format,
                          struct cork_buffer *dest);

CORK_API int
cork_timestamp_format_local(const cork_timestamp ts, const char *format,
                            struct cork_buffer *dest);


#endif /* LIBCORK_CORE_TIMESTAMP_H */
