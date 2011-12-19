/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_DS_STREAM_H
#define LIBCORK_DS_STREAM_H


#include <libcork/core/allocator.h>
#include <libcork/core/types.h>

#include <libcork/ds/slice.h>


struct cork_stream_consumer {
    int
    (*data)(struct cork_alloc *alloc, struct cork_stream_consumer *consumer,
            struct cork_slice *slice, bool is_first_chunk,
            struct cork_error *err);

    int
    (*eof)(struct cork_alloc *alloc, struct cork_stream_consumer *consumer,
           struct cork_error *err);

    void
    (*free)(struct cork_alloc *alloc, struct cork_stream_consumer *consumer);
};


#define cork_stream_consumer_data(alloc, consumer, slice, is_first, err) \
    ((consumer)->data((alloc), (consumer), (slice), (is_first), (err)))

#define cork_stream_consumer_eof(alloc, consumer, err) \
    ((consumer)->eof((alloc), (consumer), (err)))

#define cork_stream_consumer_free(alloc, consumer) \
    ((consumer)->free((alloc), (consumer)))


#endif /* LIBCORK_DS_STREAM_H */
