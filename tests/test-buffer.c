/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009-2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <check.h>

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
    static char  SRC[] =
        "Here is some text.";
    size_t  SRC_LEN = strlen(SRC);

    struct cork_buffer  buffer1;
    struct cork_buffer  *buffer2;
    struct cork_buffer  *buffer3;
    struct cork_buffer  buffer4;

    cork_buffer_init(&buffer1);
    fail_if_error(cork_buffer_set(&buffer1, SRC, SRC_LEN));

    fail_unless(cork_buffer_char(&buffer1, 0) == 'H',
                "Unexpected character at position 0: got %c, expected %c",
                (int) cork_buffer_char(&buffer1, 0), (int) 'H');

    fail_unless(cork_buffer_byte(&buffer1, 1) == (uint8_t) 'e',
                "Unexpected character at position 1: got %c, expected %c",
                (int) cork_buffer_byte(&buffer1, 1), (int) 'e');

    fail_if_error(buffer2 = cork_buffer_new());
    fail_if_error(cork_buffer_set_string(buffer2, SRC));

    fail_unless(cork_buffer_equal(&buffer1, buffer2),
                "Buffers should be equal: got %zu:%s, expected %zu:%s",
                buffer1.size, buffer1.buf, buffer2->size, buffer2->buf);

    fail_if_error(buffer3 = cork_buffer_new());
    fail_if_error(cork_buffer_printf
                  (buffer3, "Here is %s text.", "some"));

    fail_unless(cork_buffer_equal(&buffer1, buffer3),
                "Buffers should be equal: got %zu:%s, expected %zu:%s",
                buffer1.size, buffer1.buf, buffer3->size, buffer3->buf);

    cork_buffer_init(&buffer4);
    cork_buffer_copy(&buffer4, &buffer1);

    fail_unless(cork_buffer_equal(&buffer1, &buffer4),
                "Buffers should be equal: got %zu:%s, expected %zu:%s",
                buffer1.size, buffer1.buf, buffer4.size, buffer4.buf);

    cork_buffer_done(&buffer1);
    cork_buffer_free(buffer2);
    cork_buffer_free(buffer3);
    cork_buffer_done(&buffer4);
}
END_TEST


START_TEST(test_buffer_append)
{
    static char  SRC1[] = "abcd";
    size_t  SRC1_LEN = 4;
    static char  SRC2[] = "efg";
    size_t  SRC2_LEN = 3;
    static char  SRC3[] = "hij";
    static char  SRC4[] = "kl";

    struct cork_buffer  buffer1;
    cork_buffer_init(&buffer1);

    /*
     * Let's try setting some data, then clearing it, before we do our
     * appends.
     */

    fail_if_error(cork_buffer_set(&buffer1, SRC2, SRC2_LEN));
    cork_buffer_clear(&buffer1);

    /*
     * Okay now do the appends.
     */

    fail_if_error(cork_buffer_append(&buffer1, SRC1, SRC1_LEN));
    fail_if_error(cork_buffer_append(&buffer1, SRC2, SRC2_LEN));
    fail_if_error(cork_buffer_append_string(&buffer1, SRC3));
    fail_if_error(cork_buffer_append_string(&buffer1, SRC4));

    static char  EXPECTED[] = "abcdefghijkl";

    struct cork_buffer  buffer2;
    cork_buffer_init(&buffer2);
    fail_if_error(cork_buffer_set_string(&buffer2, EXPECTED));

    fail_unless(cork_buffer_equal(&buffer1, &buffer2),
                "Buffers should be equal: got %zu:%s, expected %zu:%s",
                buffer1.size, buffer1.buf, buffer2.size, buffer2.buf);

    struct cork_buffer  *buffer3;
    fail_if_error(buffer3 = cork_buffer_new());
    fail_if_error(cork_buffer_set(buffer3, SRC1, SRC1_LEN));
    fail_if_error(cork_buffer_append_printf
                  (buffer3, "%s%s%s", SRC2, SRC3, SRC4));

    fail_unless(cork_buffer_equal(&buffer1, buffer3),
                "Buffers should be equal: got %zu:%s, expected %zu:%s",
                buffer1.size, buffer1.buf, buffer3->size, buffer3->buf);

    cork_buffer_done(&buffer1);
    cork_buffer_done(&buffer2);
    cork_buffer_free(buffer3);
}
END_TEST


START_TEST(test_buffer_slicing)
{
    static char  SRC[] =
        "Here is some text.";

    struct cork_buffer  *buffer;
    fail_if_error(buffer = cork_buffer_new());
    fail_if_error(cork_buffer_set_string(buffer, SRC));

    struct cork_managed_buffer  *managed;
    fail_if_error(managed = cork_buffer_to_managed_buffer
                  (buffer));
    cork_managed_buffer_unref(managed);

    fail_if_error(buffer = cork_buffer_new());
    fail_if_error(cork_buffer_set_string(buffer, SRC));

    struct cork_slice  slice1;
    struct cork_slice  slice2;

    fail_if_error(cork_buffer_to_slice(buffer, &slice1));

    fail_if_error(cork_slice_copy_offset(&slice2, &slice1, 2));
    cork_slice_finish(&slice2);

    fail_if_error(cork_slice_copy_offset(&slice2, &slice1, buffer->size));
    cork_slice_finish(&slice2);

    fail_if_error(cork_slice_copy_fast(&slice2, &slice1, 2, 2));
    cork_slice_finish(&slice2);

    fail_if_error(cork_slice_copy_offset_fast(&slice2, &slice1, 2));
    cork_slice_finish(&slice2);

    fail_if_error(cork_slice_copy_offset(&slice2, &slice1, 0));
    fail_if_error(cork_slice_slice_offset_fast(&slice2, 2));
    fail_if_error(cork_slice_slice_fast(&slice2, 0, 2));
    fail_if_error(cork_slice_slice(&slice1, 2, 2));
    fail_unless(cork_slice_equal(&slice1, &slice2), "Slices should be equal");
    cork_slice_finish(&slice2);

    cork_slice_finish(&slice1);
}
END_TEST


START_TEST(test_buffer_stream)
{
    static char  SRC1[] = "abcd";
    size_t  SRC1_LEN = 4;
    static char  SRC2[] = "efg";
    size_t  SRC2_LEN = 3;

    struct cork_buffer  buffer1;
    cork_buffer_init(&buffer1);
    struct cork_stream_consumer  *consumer;
    fail_if_error(consumer =
                  cork_buffer_to_stream_consumer(&buffer1));

    /* chunk #1 */
    fail_if_error(cork_stream_consumer_data(consumer, SRC1, SRC1_LEN, true));

    /* chunk #2 */
    fail_if_error(cork_stream_consumer_data(consumer, SRC2, SRC2_LEN, false));

    /* eof */
    fail_if_error(cork_stream_consumer_eof(consumer));

    /* check the result */

    static char  EXPECTED[] = "abcdefg";
    static size_t  EXPECTED_SIZE = 7;

    struct cork_buffer  buffer2;
    cork_buffer_init(&buffer2);
    fail_if_error(cork_buffer_set
                  (&buffer2, EXPECTED, EXPECTED_SIZE));

    fail_unless(cork_buffer_equal(&buffer1, &buffer2),
                "Buffers should be equal: got %zu:%s, expected %zu:%s",
                buffer1.size, buffer1.buf, buffer2.size, buffer2.buf);

    cork_stream_consumer_free(consumer);
    cork_buffer_done(&buffer1);
    cork_buffer_done(&buffer2);
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
