/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <time.h>
#include <sys/time.h>

#include "libcork/core/timestamp.h"
#include "libcork/core/types.h"

void
cork_timestamp_init_now(cork_timestamp *ts)
{
    struct timeval  tp;
    gettimeofday(&tp, NULL);
    cork_timestamp_init_usec(ts, tp.tv_sec, tp.tv_usec);
}


bool
cork_timestamp_format_utc(const cork_timestamp ts,
                          const char *format,
                          char *buf, size_t size)
{
    time_t  clock;
    struct tm  tm;

    clock = cork_timestamp_sec(ts);
    gmtime_r(&clock, &tm);
    return strftime(buf, size, format, &tm) > 0;
}


bool
cork_timestamp_format_local(const cork_timestamp ts,
                            const char *format,
                            char *buf, size_t size)
{
    time_t  clock;
    struct tm  tm;

    clock = cork_timestamp_sec(ts);
    localtime_r(&clock, &tm);
    return strftime(buf, size, format, &tm) > 0;
}

bool
cork_timestamp_format_iso8601(const cork_timestamp ts,
                              char *buf, size_t size)
{
    time_t  clock;
    struct tm  tm;

    clock = cork_timestamp_sec(ts);
    gmtime_r(&clock, &tm);
    return strftime(buf, size, "%FT%H:%M:%SZ", &tm) > 0;
}
