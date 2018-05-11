/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2017, libcork authors
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#include <check.h>

#include "libcork/config.h"
#include "libcork/core/u128.h"
#include "libcork/os/subprocess.h"

#include "helpers.h"


/*-----------------------------------------------------------------------
 * 128-bit integers
 */

static void
test_one_u128_decimal(cork_u128 value, const char *expected)
{
    char  buf[CORK_U128_DECIMAL_LENGTH];
    const char  *actual = cork_u128_to_decimal(buf, value);
    fail_unless_streq("Integers", expected, actual);
}

static void
test_one_u128_hex(cork_u128 value, const char *expected)
{
    char  buf[CORK_U128_HEX_LENGTH];
    const char  *actual = cork_u128_to_hex(buf, value);
    fail_unless_streq("Integers", expected, actual);
}

static void
test_one_u128_padded_hex(cork_u128 value, const char *expected)
{
    char  buf[CORK_U128_HEX_LENGTH];
    const char  *actual = cork_u128_to_padded_hex(buf, value);
    fail_unless_streq("Integers", expected, actual);
}

static void
test_one_u128_print_from_32(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3,
                            const char *expected_decimal,
                            const char *expected_hex,
                            const char *expected_padded_hex)
{
    cork_u128  value = cork_u128_from_32(i0, i1, i2, i3);
    test_one_u128_decimal(value, expected_decimal);
    test_one_u128_hex(value, expected_hex);
    test_one_u128_padded_hex(value, expected_padded_hex);
}

static void
test_one_u128_print_from_64(uint64_t i0, uint64_t i1,
                            const char *expected_decimal,
                            const char *expected_hex,
                            const char *expected_padded_hex)
{
    cork_u128  value = cork_u128_from_64(i0, i1);
    test_one_u128_decimal(value, expected_decimal);
    test_one_u128_hex(value, expected_hex);
    test_one_u128_padded_hex(value, expected_padded_hex);
}

START_TEST(test_u128_print)
{
    DESCRIBE_TEST;
    test_one_u128_print_from_32(
        0, 0, 0, 0,
        "0",
        "0",
        "00000000000000000000000000000000"
    );
    test_one_u128_print_from_32(
        0, 0, 0, 2,
        "2",
        "2",
        "00000000000000000000000000000002"
    );
    test_one_u128_print_from_32(
        0, 0, 0, 20,
        "20",
        "14",
        "00000000000000000000000000000014"
    );
    test_one_u128_print_from_32(
        0, 0, 0, 0xffffffff,
        "4294967295",
        "ffffffff",
        "000000000000000000000000ffffffff"
    );
    test_one_u128_print_from_32(
        0, 0, 1, 0,
        "4294967296",
        "100000000",
        "00000000000000000000000100000000"
    );
    test_one_u128_print_from_32(
        0, 0, 0xffffffff, 0xffffffff,
        "18446744073709551615",
        "ffffffffffffffff",
        "0000000000000000ffffffffffffffff"
    );
    test_one_u128_print_from_32(
        0, 1, 0, 0,
        "18446744073709551616",
        "10000000000000000",
        "00000000000000010000000000000000"
    );
    test_one_u128_print_from_64(
        0, 0,
        "0",
        "0",
        "00000000000000000000000000000000"
    );
    test_one_u128_print_from_64(
        0, 2,
        "2",
        "2",
        "00000000000000000000000000000002"
    );
    test_one_u128_print_from_64(
        0, 20,
        "20",
        "14",
        "00000000000000000000000000000014"
    );
    test_one_u128_print_from_64(
        0, UINT64_C(0xffffffffffffffff),
        "18446744073709551615",
        "ffffffffffffffff",
        "0000000000000000ffffffffffffffff"
    );
    test_one_u128_print_from_64(
        1, 0,
        "18446744073709551616",
        "10000000000000000",
        "00000000000000010000000000000000"
    );
}
END_TEST


struct arithmetic_test {
    uint64_t i0;
    uint64_t i1;
    uint64_t j0;
    uint64_t j1;
    uint64_t res0;
    uint64_t res1;
};

static void
check_arithmetic_test(cork_u128(op)(cork_u128, cork_u128), const char *op_str,
                      const struct arithmetic_test *test)
{
    cork_u128  value1 = cork_u128_from_64(test->i0, test->i1);
    cork_u128  value2 = cork_u128_from_64(test->j0, test->j1);
    cork_u128  expected = cork_u128_from_64(test->res0, test->res1);
    cork_u128  sum = op(value1, value2);
    if (!cork_u128_eq(sum, expected)) {
        char  buf1[CORK_U128_HEX_LENGTH];
        char  buf2[CORK_U128_HEX_LENGTH];
        char  buf3[CORK_U128_HEX_LENGTH];
        char  buf4[CORK_U128_HEX_LENGTH];
        const char  *value1_str = cork_u128_to_hex(buf1, value1);
        const char  *value2_str = cork_u128_to_hex(buf2, value2);
        const char  *expected_str = cork_u128_to_hex(buf3, expected);
        const char  *sum_str = cork_u128_to_hex(buf4, sum);
        fprintf(stderr, "#     %40s\n", value1_str);
        fprintf(stderr, "#   %s %40s\n", op_str, value2_str);
        fprintf(stderr, "#   = %40s\n", expected_str);
        fprintf(stderr, "# got %40s\n", sum_str);
        fail("Unexpected arithmetic error");
    }
}

static void
check_arithmetic_tests_(cork_u128(op)(cork_u128, cork_u128), const char *op_str,
                        const struct arithmetic_test *test, size_t count)
{
    size_t  i;
    for (i = 0; i < count; i++) {
        check_arithmetic_test(op, op_str, test + i);
    }
}

#define check_arithmetic_tests(op, op_str, tests) \
    check_arithmetic_tests_(op, op_str, \
            (tests), sizeof(tests) / sizeof(tests[0]))


static const struct arithmetic_test ADD_TESTS[] = {
    {0, 0, 0, 0, 0, 0},
    {0, 1, 0, 1, 0, 2},
    {0, 1, 0, UINT64_C(0xffffffffffffffff), 1, 0},
    {0, 1, UINT64_C(0xffffffffffffffff), UINT64_C(0xffffffffffffffff), 0, 0},
#include "u128-tests-add.c.in"
};

START_TEST(test_u128_add)
{
    DESCRIBE_TEST;
    check_arithmetic_tests(cork_u128_add, "+", ADD_TESTS);
}
END_TEST


static const struct arithmetic_test SUB_TESTS[] = {
    {0, 0, 0, 0, 0, 0},
    {0, 1, 0, 1, 0, 0},
    {0, 2, 0, 1, 0, 1},
    {1, 0, 0, 1, 0, UINT64_C(0xffffffffffffffff)},
    {0, 1, 0, 2, UINT64_C(0xffffffffffffffff), UINT64_C(0xffffffffffffffff)},
#include "u128-tests-sub.c.in"
};

START_TEST(test_u128_sub)
{
    DESCRIBE_TEST;
    check_arithmetic_tests(cork_u128_sub, "-", SUB_TESTS);
}
END_TEST


struct comparison_test {
    uint64_t i0;
    uint64_t i1;
    uint64_t j0;
    uint64_t j1;
    bool expected;
};

static void
check_comparison_test(bool(op)(cork_u128, cork_u128), const char *op_str,
                      const struct comparison_test *test)
{
    cork_u128  value1 = cork_u128_from_64(test->i0, test->i1);
    cork_u128  value2 = cork_u128_from_64(test->j0, test->j1);
    bool  actual = op(value1, value2);
    if (actual != test->expected) {
        char  buf1[CORK_U128_HEX_LENGTH];
        char  buf2[CORK_U128_HEX_LENGTH];
        const char  *value1_str = cork_u128_to_hex(buf1, value1);
        const char  *value2_str = cork_u128_to_hex(buf2, value2);
        fprintf(stderr, "#     %40s\n", value1_str);
        fprintf(stderr, "#  %s %40s\n", op_str, value2_str);
        fprintf(stderr, "#   = %s\n", test->expected? "true": "false");
        fprintf(stderr, "# got %s\n", actual? "true": "false");
        fail("Unexpected comparison error");
    }
}

static void
check_comparison_tests_(bool(op)(cork_u128, cork_u128), const char *op_str,
                        const struct comparison_test *test, size_t count)
{
    size_t  i;
    for (i = 0; i < count; i++) {
        check_comparison_test(op, op_str, test + i);
    }
}

#define check_comparison_tests(op, op_str, tests) \
    check_comparison_tests_(op, op_str, \
            (tests), sizeof(tests) / sizeof(tests[0]))

static const struct comparison_test EQ_TESTS[] = {
    {0, 0, 0, 0, true},
    {0, 0, 0, 1, false},
    {0, 2, 0, 1, false},
    {0, 1, 0, UINT64_C(0x100000000), false},
    {0, UINT64_C(0x100000000), 0, UINT64_C(0x100000000), true},
#include "u128-tests-eq.c.in"
};

START_TEST(test_u128_eq)
{
    DESCRIBE_TEST;
    check_comparison_tests(cork_u128_eq, "==", EQ_TESTS);
}
END_TEST

static const struct comparison_test NE_TESTS[] = {
    {0, 0, 0, 0, false},
    {0, 0, 0, 1, true},
    {0, 2, 0, 1, true},
    {0, 1, 0, UINT64_C(0x100000000), true},
    {0, UINT64_C(0x100000000), 0, UINT64_C(0x100000000), false},
#include "u128-tests-ne.c.in"
};

START_TEST(test_u128_ne)
{
    DESCRIBE_TEST;
    check_comparison_tests(cork_u128_ne, "!=", NE_TESTS);
}
END_TEST

static const struct comparison_test LT_TESTS[] = {
    {0, 0, 0, 0, false},
    {0, 0, 0, 1, true},
    {0, 2, 0, 1, false},
    {0, 1, 0, UINT64_C(0x100000000), true},
#include "u128-tests-lt.c.in"
};

START_TEST(test_u128_lt)
{
    DESCRIBE_TEST;
    check_comparison_tests(cork_u128_lt, "< ", LT_TESTS);
}
END_TEST

static const struct comparison_test LE_TESTS[] = {
    {0, 0, 0, 0, true},
    {0, 1, 0, 0, false},
    {0, 1, 0, 2, true},
    {0, UINT64_C(0x100000000), 0, 1, false},
#include "u128-tests-le.c.in"
};

START_TEST(test_u128_le)
{
    DESCRIBE_TEST;
    check_comparison_tests(cork_u128_le, "<=", LE_TESTS);
}
END_TEST

static const struct comparison_test GT_TESTS[] = {
    {0, 0, 0, 0, false},
    {0, 1, 0, 0, true},
    {0, 1, 0, 2, false},
    {0, UINT64_C(0x100000000), 0, 1, true},
#include "u128-tests-gt.c.in"
};

START_TEST(test_u128_gt)
{
    DESCRIBE_TEST;
    check_comparison_tests(cork_u128_gt, "> ", GT_TESTS);
}
END_TEST

static const struct comparison_test GE_TESTS[] = {
    {0, 0, 0, 0, true},
    {0, 0, 0, 1, false},
    {0, 2, 0, 1, true},
    {0, 1, 0, UINT64_C(0x100000000), false},
#include "u128-tests-ge.c.in"
};

START_TEST(test_u128_ge)
{
    DESCRIBE_TEST;
    check_comparison_tests(cork_u128_ge, ">=", GE_TESTS);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
test_suite()
{
    Suite  *s = suite_create("core");

    TCase  *tc_u128 = tcase_create("u128");
    tcase_add_test(tc_u128, test_u128_print);
    tcase_add_test(tc_u128, test_u128_add);
    tcase_add_test(tc_u128, test_u128_sub);
    tcase_add_test(tc_u128, test_u128_eq);
    tcase_add_test(tc_u128, test_u128_ne);
    tcase_add_test(tc_u128, test_u128_lt);
    tcase_add_test(tc_u128, test_u128_le);
    tcase_add_test(tc_u128, test_u128_gt);
    tcase_add_test(tc_u128, test_u128_ge);
    suite_add_tcase(s, tc_u128);

    return s;
}


int
main(int argc, const char **argv)
{
    int  number_failed;
    Suite  *suite = test_suite();
    SRunner  *runner = srunner_create(suite);

    setup_allocator();
    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (number_failed == 0)? EXIT_SUCCESS: EXIT_FAILURE;
}
