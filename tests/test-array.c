/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <check.h>

#include "libcork/core/types.h"
#include "libcork/ds/array.h"

#include "helpers.h"


/*-----------------------------------------------------------------------
 * Doubly-linked arrays
 */

#define add_element(element, expected_new_size) \
    fail_if_error(cork_array_append(&array, element)); \
    fail_unless(cork_array_size(&array) == expected_new_size, \
                "Unexpected size of array: got %zu, expected %zu", \
                cork_array_size(&array), expected_new_size);

#define add_element0(element, expected_new_size, int_type) \
    do { \
        int_type  *__element; \
        fail_if_error(__element = cork_array_append_get(&array)); \
        *__element = element; \
        fail_unless(cork_array_size(&array) == expected_new_size, \
                    "Unexpected size of array: got %zu, expected %zu", \
                    cork_array_size(&array), expected_new_size); \
    } while (0)

#define test_sum(expected) \
    do { \
        int64_t  sum = 0; \
        size_t  i; \
        for (i = 0; i < cork_array_size(&array); i++) { \
            sum += cork_array_at(&array, i); \
        } \
        fail_unless(sum == expected, \
                    "Unexpected sum, got %ld, expected %ld", \
                    (long) sum, (long) expected); \
    } while (0)

#define test_int(int_type) \
START_TEST(test_array_##int_type) \
{ \
    DESCRIBE_TEST; \
    \
    cork_array(int_type)  array; \
    cork_array_init(&array); \
    \
    fail_unless(cork_array_size(&array) == 0, \
                "Unexpected size of array: got %zu, expected 0", \
                cork_array_size(&array)); \
    \
    /* Make sure to add enough elements to force the array into \
     * heap-allocated storage. */ \
    test_sum(0); \
    add_element ( 1,  1); \
    test_sum(1); \
    add_element0( 2,  2, int_type); \
    test_sum(3); \
    add_element ( 3,  3); \
    test_sum(6); \
    add_element0( 4,  4, int_type); \
    test_sum(10); \
    add_element0( 5,  5, int_type); \
    test_sum(15); \
    add_element ( 6,  6); \
    test_sum(21); \
    add_element ( 7,  7); \
    test_sum(28); \
    add_element0( 8,  8, int_type); \
    test_sum(36); \
    add_element ( 9,  9); \
    test_sum(45); \
    add_element0(10, 10, int_type); \
    test_sum(55); \
} \
END_TEST

test_int(int8_t)
test_int(int16_t)
test_int(int32_t)
test_int(int64_t)


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
test_suite()
{
    Suite  *s = suite_create("array");

    TCase  *tc_ds = tcase_create("array");
    tcase_add_test(tc_ds, test_array_int8_t);
    tcase_add_test(tc_ds, test_array_int16_t);
    tcase_add_test(tc_ds, test_array_int32_t);
    tcase_add_test(tc_ds, test_array_int64_t);
    suite_add_tcase(s, tc_ds);

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
