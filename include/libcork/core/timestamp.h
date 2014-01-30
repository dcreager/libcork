/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CORE_TIMESTAMP_H
#define LIBCORK_CORE_TIMESTAMP_H


#include <libcork/core/api.h>
#include <libcork/core/error.h>
#include <libcork/core/types.h>
#include <libcork/ds/buffer.h>


typedef uint64_t  cork_timestamp;


#define cork_timestamp_init_sec(ts, sec) \
    do { \
        *(ts) = (((uint64_t) (sec)) << 32); \
    } while (0)

#define cork_timestamp_init_gsec(ts, sec, gsec) \
    do { \
        *(ts) = (((uint64_t) (sec)) << 32) | \
                (((uint64_t) (gsec)) & 0xffffffff); \
    } while (0)

#define cork_timestamp_init_msec(ts, sec, msec) \
    do { \
        *(ts) = (((uint64_t) (sec)) << 32) | \
                ((((uint64_t) (msec)) << 32) / 1000); \
    } while (0)

#define cork_timestamp_init_usec(ts, sec, usec) \
    do { \
        *(ts) = (((uint64_t) (sec)) << 32) | \
                ((((uint64_t) (usec)) << 32) / 1000000); \
    } while (0)

#define cork_timestamp_init_nsec(ts, sec, nsec) \
    do { \
        *(ts) = (((uint64_t) (sec)) << 32) | \
                ((((uint64_t) (nsec)) << 32) / 1000000000); \
    } while (0)


CORK_API void
cork_timestamp_init_now(cork_timestamp *ts);


#define cork_timestamp_sec(ts)  ((uint32_t) ((ts) >> 32))
#define cork_timestamp_gsec(ts)  ((uint32_t) ((ts) & 0xffffffff))

CORK_ATTR_UNUSED
static inline uint32_t
cork_timestamp_round_sec(const cork_timestamp ts)
{
    uint32_t  sec = cork_timestamp_sec(ts);
    uint32_t  gsec = cork_timestamp_gsec(ts);
    sec = cork_timestamp_sec(ts);
    if (gsec >= 0x80000000) {
        sec++;
    }
    return sec;
}

CORK_ATTR_UNUSED
static inline void
cork_timestamp_round(uint32_t *sec, uint32_t *frac, uint64_t denom)
{
    if (denom == 0) {
        if (*frac >= 0x80000000) {
            (*sec)++;
        }
    } else {
        uint64_t  gsec = *frac;
        uint64_t  half = ((uint64_t) 1 << 31) / denom;
        gsec += half;
        gsec *= denom;
        gsec >>= 32;
        while (gsec >= denom) {
            gsec -= denom;
            (*sec)++;
        }
        *frac = gsec;
    }
}

CORK_ATTR_UNUSED
static inline void
cork_timestamp_round_msec(const cork_timestamp ts,
                          uint32_t *sec, uint32_t *msec)
{
    *sec = cork_timestamp_sec(ts);
    *msec = cork_timestamp_gsec(ts);
    cork_timestamp_round(sec, msec, 1000);
}

CORK_ATTR_UNUSED
static inline void
cork_timestamp_round_usec(const cork_timestamp ts,
                          uint32_t *sec, uint32_t *usec)
{
    *sec = cork_timestamp_sec(ts);
    *usec = cork_timestamp_gsec(ts);
    cork_timestamp_round(sec, usec, 1000000);
}

CORK_ATTR_UNUSED
static inline void
cork_timestamp_round_nsec(const cork_timestamp ts,
                          uint32_t *sec, uint32_t *nsec)
{
    *sec = cork_timestamp_sec(ts);
    *nsec = cork_timestamp_gsec(ts);
    cork_timestamp_round(sec, nsec, 1000000000);
}


CORK_API int
cork_timestamp_format_utc(const cork_timestamp ts, const char *format,
                          struct cork_buffer *dest);

CORK_API int
cork_timestamp_format_local(const cork_timestamp ts, const char *format,
                            struct cork_buffer *dest);


#endif /* LIBCORK_CORE_TIMESTAMP_H */
