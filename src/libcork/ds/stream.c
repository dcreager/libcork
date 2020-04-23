/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2020, libcork authors
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include "libcork/ds/stream.h"

/*-----------------------------------------------------------------------
 * Inline declarations
 */

int
cork_stream_consumer_data(struct cork_stream_consumer* consumer,
                          const void *buf, size_t size, bool is_first_chunk);

int
cork_stream_consumer_eof(struct cork_stream_consumer *consumer);

void
cork_stream_consumer_free(struct cork_stream_consumer *consumer);
