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

#include "libcork/ds/slice.h"

#include "helpers.h"


/*-----------------------------------------------------------------------
 * Static slices
 */

START_TEST(test_static_slice)
{
    static char  SRC[] = "Here is some text.";
    size_t  SRC_LEN = sizeof(SRC) - 1;

    struct cork_slice  slice1;
    struct cork_slice  slice2;
    cork_slice_init_static(&slice1, SRC, SRC_LEN);
    fail_if_error(cork_slice_copy(&slice2, &slice1, 8, 4));
    fail_if_error(cork_slice_slice(&slice1, 8, 4));
    fail_unless(cork_slice_equal(&slice1, &slice2), "Slices should be equal");
    cork_slice_finish(&slice1);
    cork_slice_finish(&slice2);
}
END_TEST


/*-----------------------------------------------------------------------
 * Copy-once slices
 */

START_TEST(test_copy_once_slice)
{
    static char  SRC[] = "Here is some text.";
    size_t  SRC_LEN = sizeof(SRC) - 1;

    struct cork_slice  slice1;
    struct cork_slice  slice2;
    struct cork_slice  slice3;

    cork_slice_init_copy_once(&slice1, SRC, SRC_LEN);
    fail_unless(slice1.buf == SRC, "Unexpected slice buffer");

    fail_if_error(cork_slice_copy(&slice2, &slice1, 8, 4));
    fail_if_error(cork_slice_slice(&slice1, 8, 4));
    fail_unless(slice1.buf != SRC, "Unexpected slice buffer");
    fail_unless(slice1.buf == slice2.buf, "Unexpected slice buffer");
    fail_unless(cork_slice_equal(&slice1, &slice2), "Slices should be equal");

    fail_if_error(cork_slice_copy(&slice3, &slice1, 0, 4));
    fail_unless(slice1.buf == slice3.buf, "Unexpected slice buffer");
    fail_unless(slice2.buf == slice3.buf, "Unexpected slice buffer");
    fail_unless(cork_slice_equal(&slice1, &slice3), "Slices should be equal");
    fail_unless(cork_slice_equal(&slice2, &slice3), "Slices should be equal");

    cork_slice_finish(&slice1);
    cork_slice_finish(&slice2);
    cork_slice_finish(&slice3);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
test_suite()
{
    Suite  *s = suite_create("slice");

    TCase  *tc_slice = tcase_create("slice");
    tcase_add_test(tc_slice, test_static_slice);
    tcase_add_test(tc_slice, test_copy_once_slice);
    suite_add_tcase(s, tc_slice);

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
