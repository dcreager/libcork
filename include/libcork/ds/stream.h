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

/**
 * @file
 * @brief Implementation of the @ref stream submodule
 */

#include <libcork/core/allocator.h>
#include <libcork/core/types.h>

#include <libcork/ds/managed-buffer.h>

/**
 * @addtogroup stream
 *
 * <tt>#%include \<libcork/ds/stream.h\></tt>
 *
 * This section defines an interface for consuming streams of binary
 * data.
 *
 * @{
 */


/**
 * @brief An interface for consuming a stream of binary data.
 *
 * The producer of the stream will call the @ref
 * cork_stream_consumer_data method repeatedly once for each successive
 * chunk of data in the stream.  Once the stream has been exhausted, the
 * producer will call the @ref cork_stream_consumer_eof
 * method.  Both methods can interrupt the producer if appropriate.
 *
 * @since 0.1-dev
 */

typedef struct cork_stream_consumer_t  cork_stream_consumer_t;

struct cork_stream_consumer_t
{
    /**
     * @brief Implementation of the @ref cork_stream_consumer_data
     * method
     * @since 0.1-dev
     * @private
     */

    bool
    (*data)(cork_stream_consumer_t *consumer,
            cork_slice_t *slice, bool is_first_chunk);

    /**
     * @brief Implementation of the @ref cork_stream_consumer_eof method
     * @since 0.1-dev
     * @private
     */

    bool
    (*eof)(cork_stream_consumer_t *consumer);

    /**
     * @brief Frees the stream consumer instance
     * @since 0.1-dev
     * @private
     */

    void
    (*free)(cork_stream_consumer_t *consumer);
};

/* end of stream group */
/**
 * @}
 */


/**
 * @brief Process the next chunk of data in the stream.
 *
 * The @a slice is only guaranteed to be valid for the duration of this
 * method.  If you need access to it later, you must save it somewhere
 * yourself.
 *
 * @param [in] consumer  A stream consumer
 * @param [in] slice  The current chunk of binary data
 * @param [in] is_first_chunk  Whether this is the first chunk of the
 * stream
 * @returns @c true if this chunk was processed successfully, @c false
 * otherwise.
 * @public @memberof cork_stream_consumer_t
 * @since 0.1-dev
 */

#if defined(CORK_DOCUMENTATION)
bool
cork_stream_consumer_data(cork_stream_consumer_t *consumer,
                          cork_slice_t *slice, bool is_first_chunk);
#else
#define cork_stream_consumer_data(consumer, slice, is_first) \
    ((consumer)->data((consumer), (slice), (is_first)))
#endif

/**
 * @brief Handle the end of the data stream.
 *
 * This method will be called after the last chunk of data in the stream
 * has been processed.  This lets the consumer raise an error condition
 * if needed — for instance, because of truncated data.
 *
 * @param [in] consumer  A stream consumer
 * @returns @c true if this stream was processed successfully, @c false
 * otherwise.
 * @public @memberof cork_stream_consumer_t
 * @since 0.1-dev
 */

#if defined(CORK_DOCUMENTATION)
bool
cork_stream_consumer_eof(cork_stream_consumer_t *consumer);
#else
#define cork_stream_consumer_eof(consumer) ((consumer)->eof((consumer)))
#endif

/**
 * @brief Finalize and deallocate a stream consumer.
 * @param [in] consumer  A stream consumer
 * @public @memberof cork_stream_consumer_t
 * @since 0.1-dev
 */

#if defined(CORK_DOCUMENTATION)
void
cork_stream_consumer_free(cork_stream_consumer_t *consumer);
#else
#define cork_stream_consumer_free(consumer) ((consumer)->free((consumer)))
#endif


#endif /* LIBCORK_DS_STREAM_H */
