/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009-2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <check.h>

#include "libcork/core/allocator.h"
#include "libcork/core/types.h"
#include "libcork/ds/buffer.h"
#include "libcork/ds/managed-buffer.h"
#include "libcork/ds/stream.h"

#include "helpers.h"


/*-----------------------------------------------------------------------
 * Buffers
 */

START_TEST(test_buffer)
{
    struct cork_alloc  *alloc = cork_allocator_new_debug();

    static char  SRC[] =
        "Here is some text.";
    size_t  SRC_LEN = strlen(SRC);

    struct cork_buffer  buffer1;
    cork_buffer_init(alloc, &buffer1);
    fail_if_error(cork_buffer_set(alloc, &buffer1, SRC, SRC_LEN+1, &err));

    struct cork_buffer  *buffer2;
    fail_if_error(buffer2 = cork_buffer_new(alloc, &err));
    fail_if_error(cork_buffer_set_string(alloc, buffer2, SRC, &err));

    fail_unless(cork_buffer_equal(&buffer1, buffer2),
                "Buffers should be equal: got %zu:%s, expected %zu:%s",
                buffer1.size, buffer1.buf, buffer2->size, buffer2->buf);

    struct cork_buffer  *buffer3;
    fail_if_error(buffer3 = cork_buffer_new(alloc, &err));
    fail_if_error(cork_buffer_printf
                  (alloc, buffer3, &err, "Here is %s text.", "some"));

    fail_unless(cork_buffer_equal(&buffer1, buffer3),
                "Buffers should be equal: got %zu:%s, expected %zu:%s",
                buffer1.size, buffer1.buf, buffer3->size, buffer3->buf);

    cork_buffer_done(alloc, &buffer1);
    cork_buffer_free(alloc, buffer2);
    cork_buffer_free(alloc, buffer3);
    cork_allocator_free(alloc);
}
END_TEST


START_TEST(test_buffer_append)
{
    struct cork_alloc  *alloc = cork_allocator_new_debug();

    static char  SRC1[] = "abcd";
    size_t  SRC1_LEN = 4;
    static char  SRC2[] = "efg";
    size_t  SRC2_LEN = 3;
    static char  SRC3[] = "hij";
    static char  SRC4[] = "kl";

    struct cork_buffer  buffer1;
    cork_buffer_init(alloc, &buffer1);

    /*
     * Let's try setting some data, then clearing it, before we do our
     * appends.
     */

    fail_if_error(cork_buffer_set(alloc, &buffer1, SRC2, SRC2_LEN, &err));
    cork_buffer_clear(alloc, &buffer1);

    /*
     * Okay now do the appends.
     */

    fail_if_error(cork_buffer_append(alloc, &buffer1, SRC1, SRC1_LEN, &err));
    fail_if_error(cork_buffer_append(alloc, &buffer1, SRC2, SRC2_LEN, &err));
    fail_if_error(cork_buffer_append_string(alloc, &buffer1, SRC3, &err));
    fail_if_error(cork_buffer_append_string(alloc, &buffer1, SRC4, &err));

    static char  EXPECTED[] = "abcdefghijkl";

    struct cork_buffer  buffer2;
    cork_buffer_init(alloc, &buffer2);
    fail_if_error(cork_buffer_set_string(alloc, &buffer2, EXPECTED, &err));

    fail_unless(cork_buffer_equal(&buffer1, &buffer2),
                "Buffers should be equal: got %zu:%s, expected %zu:%s",
                buffer1.size, buffer1.buf, buffer2.size, buffer2.buf);

    struct cork_buffer  *buffer3;
    fail_if_error(buffer3 = cork_buffer_new(alloc, &err));
    fail_if_error(cork_buffer_set(alloc, buffer3, SRC1, SRC1_LEN, &err));
    fail_if_error(cork_buffer_append_printf
                  (alloc, buffer3, &err, "%s%s%s", SRC2, SRC3, SRC4));

    fail_unless(cork_buffer_equal(&buffer1, buffer3),
                "Buffers should be equal: got %zu:%s, expected %zu:%s",
                buffer1.size, buffer1.buf, buffer3->size, buffer3->buf);

    cork_buffer_done(alloc, &buffer1);
    cork_buffer_done(alloc, &buffer2);
    cork_buffer_free(alloc, buffer3);
    cork_allocator_free(alloc);
}
END_TEST


START_TEST(test_buffer_slicing)
{
    struct cork_alloc  *alloc = cork_allocator_new_debug();

    static char  SRC[] =
        "Here is some text.";

    struct cork_buffer  *buffer;
    fail_if_error(buffer = cork_buffer_new(alloc, &err));
    fail_if_error(cork_buffer_set_string(alloc, buffer, SRC, &err));

    struct cork_managed_buffer  *managed;
    fail_if_error(managed = cork_buffer_to_managed_buffer
                  (alloc, buffer, &err));
    cork_managed_buffer_unref(alloc, managed);

    fail_if_error(buffer = cork_buffer_new(alloc, &err));
    fail_if_error(cork_buffer_set_string(alloc, buffer, SRC, &err));

    struct cork_slice  slice;
    fail_if_error(cork_buffer_to_slice(alloc, buffer, &slice, &err));
    cork_slice_finish(alloc, &slice);

    cork_allocator_free(alloc);
}
END_TEST


START_TEST(test_buffer_stream)
{
    struct cork_alloc  *alloc = cork_allocator_new_debug();

    static char  SRC1[] = "abcd";
    size_t  SRC1_LEN = 4;
    static char  SRC2[] = "efg";
    size_t  SRC2_LEN = 3;

    struct cork_buffer  buffer1;
    cork_buffer_init(alloc, &buffer1);
    struct cork_stream_consumer  *consumer;
    fail_if_error(consumer =
                  cork_buffer_to_stream_consumer(alloc, &buffer1, &err));

    struct cork_managed_buffer  *src;
    struct cork_slice  slice;

    /* chunk #1 */

    fail_if_error(src = cork_managed_buffer_new_copy
                  (alloc, SRC1, SRC1_LEN, &err));
    fail_if_error(cork_managed_buffer_slice_offset
                  (alloc, &slice, src, 0, &err));
    fail_if_error(cork_stream_consumer_data
                  (alloc, consumer, &slice, true, &err));
    cork_slice_finish(alloc, &slice);
    cork_managed_buffer_unref(alloc, src);

    /* chunk #2 */

    fail_if_error(src = cork_managed_buffer_new_copy
                  (alloc, SRC2, SRC2_LEN, &err));
    fail_if_error(cork_managed_buffer_slice_offset
                  (alloc, &slice, src, 0, &err));
    fail_if_error(cork_stream_consumer_data
                  (alloc, consumer, &slice, false, &err));
    cork_slice_finish(alloc, &slice);
    cork_managed_buffer_unref(alloc, src);

    /* eof */

    fail_if_error(cork_stream_consumer_eof(alloc, consumer, &err));

    /* check the result */

    static char  EXPECTED[] = "abcdefg";
    static size_t  EXPECTED_SIZE = 7;

    struct cork_buffer  buffer2;
    cork_buffer_init(alloc, &buffer2);
    fail_if_error(cork_buffer_set
                  (alloc, &buffer2, EXPECTED, EXPECTED_SIZE, &err));

    fail_unless(cork_buffer_equal(&buffer1, &buffer2),
                "Buffers should be equal: got %zu:%s, expected %zu:%s",
                buffer1.size, buffer1.buf, buffer2.size, buffer2.buf);

    cork_stream_consumer_free(alloc, consumer);
    cork_buffer_done(alloc, &buffer1);
    cork_buffer_done(alloc, &buffer2);
    cork_allocator_free(alloc);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
test_suite()
{
    Suite  *s = suite_create("buffer");

    TCase  *tc_buffer = tcase_create("buffer");
    tcase_add_test(tc_buffer, test_buffer);
    tcase_add_test(tc_buffer, test_buffer_append);
    tcase_add_test(tc_buffer, test_buffer_slicing);
    tcase_add_test(tc_buffer, test_buffer_stream);
    suite_add_tcase(s, tc_buffer);

    return s;
}


int
main(int argc, const char **argv)
{
    int  number_failed;
    Suite  *suite = test_suite();
    SRunner  *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (number_failed == 0)? EXIT_SUCCESS: EXIT_FAILURE;
}
