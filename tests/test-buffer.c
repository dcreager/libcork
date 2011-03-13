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


/*-----------------------------------------------------------------------
 * Buffers
 */

START_TEST(test_buffer)
{
    cork_allocator_t  *alloc = cork_allocator_new_debug();

    static char  SRC[] =
        "Here is some text.";
    size_t  SRC_LEN = strlen(SRC);

    cork_buffer_t  buffer1;
    cork_buffer_init(alloc, &buffer1);
    fail_unless(cork_buffer_set(&buffer1, SRC, SRC_LEN+1),
                "Could not set contents of buffer");

    cork_buffer_t  *buffer2 = cork_buffer_new(alloc);
    fail_if(buffer2 == NULL,
            "Could not allocate buffer");
    fail_unless(cork_buffer_set_string(buffer2, SRC),
                "Could not set string contents of buffer");

    fail_unless(cork_buffer_equal(&buffer1, buffer2),
                "Buffers should be equal");

    cork_buffer_done(&buffer1);
    cork_buffer_free(buffer2);
    cork_allocator_free(alloc);
}
END_TEST


START_TEST(test_buffer_slicing)
{
    cork_allocator_t  *alloc = cork_allocator_new_debug();

    static char  SRC[] =
        "Here is some text.";

    cork_buffer_t  *buffer = cork_buffer_new(alloc);
    fail_unless(cork_buffer_set_string(buffer, SRC),
                "Could not set string contents of buffer");

    cork_managed_buffer_t  *managed =
        cork_buffer_to_managed_buffer(buffer);
    fail_if(managed == NULL,
            "Cannot manage buffer");
    cork_managed_buffer_unref(managed);

    buffer = cork_buffer_new(alloc);
    fail_unless(cork_buffer_set_string(buffer, SRC),
                "Could not set string contents of buffer");

    cork_slice_t  slice;
    fail_unless(cork_buffer_to_slice(buffer, &slice),
                "Cannot slice buffer");
    cork_slice_finish(&slice);

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
    tcase_add_test(tc_buffer, test_buffer_slicing);
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
