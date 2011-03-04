/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <check.h>

#include "libcork/core/types.h"


/*-----------------------------------------------------------------------
 * Core types
 */

START_TEST(test_int_types)
{
    /*
     * Make sure we have all of the C99 fixed-size integer types
     * available.
     */

#define TEST_INT_TYPE(type) \
    { \
        type  i = 0; \
        fail_unless(i == 0, "Unexpected value for " #type); \
    }

    TEST_INT_TYPE(int8_t);
    TEST_INT_TYPE(int16_t);
    TEST_INT_TYPE(int32_t);
    TEST_INT_TYPE(int64_t);
    TEST_INT_TYPE(uint8_t);
    TEST_INT_TYPE(uint16_t);
    TEST_INT_TYPE(uint32_t);
    TEST_INT_TYPE(uint64_t);
    TEST_INT_TYPE(size_t);
    TEST_INT_TYPE(ptrdiff_t);
    TEST_INT_TYPE(intptr_t);
    TEST_INT_TYPE(uintptr_t);

#undef TEST_INT_TYPE
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
test_suite()
{
    Suite  *s = suite_create("core");

    TCase  *tc_core = tcase_create("core");
    tcase_add_test(tc_core, test_int_types);
    suite_add_tcase(s, tc_core);

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
