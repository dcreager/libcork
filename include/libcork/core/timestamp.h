/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CORE_TIMESTAMP_H
#define LIBCORK_CORE_TIMESTAMP_H


#include <libcork/core/error.h>
#include <libcork/core/types.h>


typedef uint64_t  cork_timestamp;


#define cork_timestamp_init_sec(ts, sec) \
    do { \
        *(ts) = (((uint64_t) (sec)) << 32); \
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


void
cork_timestamp_init_now(cork_timestamp *ts);


#define cork_timestamp_sec(ts)  ((uint32_t) ((ts) >> 32))
#define cork_timestamp_gsec(ts)  ((uint32_t) ((ts) & 0xFFFFFFFF))


bool
cork_timestamp_format_utc(const cork_timestamp ts, const char *format,
                          char *buf, size_t size);

bool
cork_timestamp_format_local(const cork_timestamp ts, const char *format,
                            char *buf, size_t size);


#endif /* LIBCORK_CORE_TIMESTAMP_H */
