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
 * Resizable arrays
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
    \
    cork_array_done(&array); \
} \
END_TEST

test_int(int8_t)
test_int(int16_t)
test_int(int32_t)
test_int(int64_t)


/*-----------------------------------------------------------------------
 * String arrays
 */

#define add_string(element, expected_new_size) \
    fail_if_error(cork_string_array_append(&array, element)); \
    fail_unless(cork_array_size(&array) == expected_new_size, \
                "Unexpected size of array: got %zu, expected %zu", \
                cork_array_size(&array), (size_t) expected_new_size);

#define test_string(index, expected) \
    do { \
        const char  *actual = cork_array_at(&array, index); \
        fail_unless_streq("Array elements", expected, actual); \
    } while (0)

START_TEST(test_array_string)
{
    DESCRIBE_TEST;
    struct cork_string_array  array;
    cork_string_array_init(&array);
    add_string("hello", 1);
    add_string("there", 2);
    add_string("world", 3);
    test_string(0, "hello");
    test_string(1, "there");
    test_string(2, "world");
    cork_array_clear(&array);
    add_string("reusing", 1);
    add_string("entries", 2);
    test_string(0, "reusing");
    test_string(1, "entries");
    cork_array_done(&array);
}
END_TEST


/*-----------------------------------------------------------------------
 * Array callbacks
 */

struct callback_counts {
    size_t  init;
    size_t  done;
    size_t  reuse;
    size_t  remove;
};

static void
test_array__init(void *user_data, void *vvalue)
{
    struct callback_counts  *counts = user_data;
    counts->init++;
}

static void
test_array__done(void *user_data, void *vvalue)
{
    struct callback_counts  *counts = user_data;
    counts->done++;
}

static void
test_array__reuse(void *user_data, void *vvalue)
{
    struct callback_counts  *counts = user_data;
    counts->reuse++;
}

static void
test_array__remove(void *user_data, void *vvalue)
{
    struct callback_counts  *counts = user_data;
    counts->remove++;
}

static void
check_counts(struct callback_counts *counts,
             size_t expected_init, size_t expected_done,
             size_t expected_reuse, size_t expected_remove)
{
    fail_unless_equal("init counts", "%zu", expected_init, counts->init);
    fail_unless_equal("done counts", "%zu", expected_done, counts->done);
    fail_unless_equal("reuse counts", "%zu", expected_reuse, counts->reuse);
    fail_unless_equal("remove counts", "%zu", expected_remove, counts->remove);
}

START_TEST(test_array_callbacks)
{
    DESCRIBE_TEST;
    struct callback_counts  counts = { 0, 0, 0, 0 };
    cork_array(unsigned int)  array;
    cork_array_init(&array);
    cork_array_set_callback_data(&array, &counts, NULL);
    cork_array_set_init(&array, test_array__init);
    cork_array_set_done(&array, test_array__done);
    cork_array_set_reuse(&array, test_array__reuse);
    cork_array_set_remove(&array, test_array__remove);

    check_counts(&counts, 0, 0, 0, 0);
    cork_array_append(&array, 0);
    cork_array_append(&array, 1);
    check_counts(&counts, 2, 0, 0, 0);
    cork_array_append(&array, 2);
    cork_array_append(&array, 3);
    check_counts(&counts, 4, 0, 0, 0);
    cork_array_clear(&array);
    check_counts(&counts, 4, 0, 0, 4);
    cork_array_append(&array, 0);
    cork_array_append(&array, 1);
    check_counts(&counts, 4, 0, 2, 4);
    cork_array_append(&array, 2);
    cork_array_append(&array, 3);
    check_counts(&counts, 4, 0, 4, 4);
    cork_array_append(&array, 4);
    check_counts(&counts, 5, 0, 4, 4);
    cork_array_done(&array);
    check_counts(&counts, 5, 5, 4, 4);
}
END_TEST


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
    tcase_add_test(tc_ds, test_array_string);
    tcase_add_test(tc_ds, test_array_callbacks);
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
