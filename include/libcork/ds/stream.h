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


#include <libcork/core/types.h>


struct cork_stream_consumer {
    int
    (*data)(struct cork_stream_consumer *consumer,
            const void *buf, size_t size, bool is_first_chunk);

    int
    (*eof)(struct cork_stream_consumer *consumer);

    void
    (*free)(struct cork_stream_consumer *consumer);
};


#define cork_stream_consumer_data(consumer, buf, size, is_first) \
    ((consumer)->data((consumer), (buf), (size), (is_first)))

#define cork_stream_consumer_eof(consumer) \
    ((consumer)->eof((consumer)))

#define cork_stream_consumer_free(consumer) \
    ((consumer)->free((consumer)))


#endif /* LIBCORK_DS_STREAM_H */
