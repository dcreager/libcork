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

/**
 * @file
 * @brief Implementation of the @ref timestamp submodule
 */

#include <libcork/core/error.h>
#include <libcork/core/types.h>

/**
 * @defgroup timestamp High-precision timestamps
 * @ingroup basic_types
 *
 * <tt>#%include \<libcork/core/timestamp.h\></tt>
 *
 * Defines a high-precision timestamp type.  A timestamp is represented
 * by a 64-bit integer, whose unit is the <i>gammasecond</i> (γsec),
 * where 1&nbsp;γsec&nbsp;=&nbsp;1/2<sup>32</sup>&nbsp;sec.  With this
 * representation, the upper 32 bits of a timestamp value represent the
 * timestamp truncated (towards zero) to seconds.
 *
 * For the basic @ref cork_timestamp type, we don't concern ourselves
 * with any higher-level issues of clock synchronization.  Timestamps
 * can be used to represent any time quantity, regardless of which time
 * standard (UTC, GMT, TAI) you use, or whether it takes into account
 * the local time zone.
 *
 * @{
 */

/**
 * @brief An unsigned high-precision timestamp.
 * @since 0.2
 */

typedef uint64_t  cork_timestamp;

/* end of timestamp group */
/**
 * @}
 */


/**
 * @brief Initialize a timestamp from separate a single second quantity.
 *
 * @param [out] ts  A timestamp
 * @param [in] sec  The seconds portion of the timestamp
 *
 * @public @memberof cork_timestamp
 * @since 0.2
 */

#if defined(CORK_DOCUMENTATION)
void
cork_timestamp_init_sec(cork_timestamp *ts, uint32_t sec);
#else
#define cork_timestamp_init_sec(ts, sec) \
    do { \
        *(ts) = (((uint64_t) (sec)) << 32); \
    } while (0)
#endif


/**
 * @brief Initialize a timestamp from separate second and millisecond
 * quantities.
 *
 * @param [out] ts  A timestamp
 * @param [in] sec  The seconds portion of the timestamp
 * @param [in] msec  The milliseconds portion of the timestamp
 *
 * @public @memberof cork_timestamp
 * @since 0.2
 */

#if defined(CORK_DOCUMENTATION)
void
cork_timestamp_init_msec(cork_timestamp *ts, uint32_t sec, uint32_t msec);
#else
#define cork_timestamp_init_msec(ts, sec, msec) \
    do { \
        *(ts) = (((uint64_t) (sec)) << 32) | \
                ((((uint64_t) (msec)) << 32) / 1000); \
    } while (0)
#endif


/**
 * @brief Initialize a timestamp from separate second and microsecond
 * quantities.
 *
 * @param [out] ts  A timestamp
 * @param [in] sec  The seconds portion of the timestamp
 * @param [in] usec  The microseconds portion of the timestamp
 *
 * @public @memberof cork_timestamp
 * @since 0.2
 */

#if defined(CORK_DOCUMENTATION)
void
cork_timestamp_init_usec(cork_timestamp *ts, uint32_t sec, uint32_t usec);
#else
#define cork_timestamp_init_usec(ts, sec, usec) \
    do { \
        *(ts) = (((uint64_t) (sec)) << 32) | \
                ((((uint64_t) (usec)) << 32) / 1000000); \
    } while (0)
#endif


/**
 * @brief Initialize a timestamp with the current UTC time of day.
 *
 * @note The resolution of this function is system-dependent.
 *
 * @param [out] ts  A timestamp
 *
 * @public @memberof cork_timestamp
 * @since 0.2
 */

void
cork_timestamp_init_now(cork_timestamp *ts);


/**
 * @brief Extract the seconds portion of a timestamp.
 * @param [in] ts  A timestamp
 * @returns The timestamp truncated to an integral number of seconds.
 * @public @memberof cork_timestamp
 * @since 0.2
 */

#if defined(CORK_DOCUMENTATION)
uint32_t
cork_timestamp_sec(const cork_timestamp ts);
#else
#define cork_timestamp_sec(ts)  ((uint32_t) ((ts) >> 32))
#endif


/**
 * @brief Extract the fractional portion of a timestamp.
 * @param [in] ts  A timestamp
 * @returns The fractional portion of a timestamp, in gammaseconds.
 * @public @memberof cork_timestamp
 * @since 0.2
 */

#if defined(CORK_DOCUMENTATION)
uint32_t
cork_timestamp_gsec(const cork_timestamp ts);
#else
#define cork_timestamp_gsec(ts)  ((uint32_t) ((ts) & 0xFFFFFFFF))
#endif


/**
 * @brief Format a timestamp that represents a UTC time.
 *
 * @param [in] ts  A timestamp
 * @param [in] format  A strftime format string
 * @param [out] buf  A buffer to place the formatted time into.
 * @param [in] size  The size of @a buf
 * @returns Whether we successfully formatted the timestamp.
 *
 * @public @memberof cork_timestamp
 * @since 0.2
 */

bool
cork_timestamp_format_utc(const cork_timestamp ts,
                          const char *format,
                          char *buf, size_t size);


/**
 * @brief Format a timestamp that represents a local time.
 *
 * @param [in] ts  A timestamp
 * @param [in] format  A strftime format string
 * @param [out] buf  A buffer to place the formatted time into.
 * @param [in] size  The size of @a buf
 * @returns Whether we successfully formatted the timestamp.
 *
 * @public @memberof cork_timestamp
 * @since 0.2
 */

bool
cork_timestamp_format_local(const cork_timestamp ts,
                            const char *format,
                            char *buf, size_t size);


#endif /* LIBCORK_CORE_TIMESTAMP_H */
