/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
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

#include "libcork/core/types.h"
#include "libcork/threads/atomics.h"
#include "libcork/threads/basics.h"

#include "helpers.h"


/*-----------------------------------------------------------------------
 * Atomics
 */

#define test_atomic_op(name, type, fmt, op, expected) \
    do { \
        type  actual = cork_##name##_atomic_##op(&val, 1); \
        fail_unless_equal(#name, fmt, expected, actual); \
    } while (0)

#define test_cas(name, type, fmt, ov, nv) \
    do { \
        type  actual = cork_##name##_cas(&val, ov, nv); \
        fail_unless_equal(#name, fmt, ov, actual); \
    } while (0)

#define test_atomic(name, type, fmt) \
START_TEST(test_atomic_##name) \
{ \
    volatile type  val = 0; \
    test_atomic_op(name, type, fmt, add, 1); \
    test_atomic_op(name, type, fmt, pre_add, 1); \
    test_atomic_op(name, type, fmt, add, 3); \
    test_atomic_op(name, type, fmt, pre_add, 3); \
    fail_unless_equal(#name, fmt, 4, val); \
    test_atomic_op(name, type, fmt, sub, 3); \
    test_atomic_op(name, type, fmt, pre_sub, 3); \
    test_atomic_op(name, type, fmt, sub, 1); \
    test_atomic_op(name, type, fmt, pre_sub, 1); \
    fail_unless_equal(#name, fmt, 0, val); \
    \
    test_cas(name, type, fmt, 0, 1); \
    test_cas(name, type, fmt, 1, 10); \
    test_cas(name, type, fmt, 10, 2); \
    test_cas(name, type, fmt, 2, 0); \
    fail_unless_equal(#name, fmt, 0, val); \
} \
END_TEST

test_atomic(int8,   int8_t,   "%" PRId8);
test_atomic(int16,  int16_t,  "%" PRId16);
test_atomic(int32,  int32_t,  "%" PRId32);
test_atomic(int64,  int64_t,  "%" PRId64);
test_atomic(uint8,  uint8_t,  "%" PRIu8);
test_atomic(uint16, uint16_t, "%" PRIu16);
test_atomic(uint32, uint32_t, "%" PRIu32);
test_atomic(uint64, uint64_t, "%" PRIu64);

test_atomic(short, short, "%hd");
test_atomic(int,   int,   "%d");
test_atomic(long,  long,  "%ld");

test_atomic(ushort, unsigned short, "%hu");
test_atomic(uint,   unsigned int,   "%u");
test_atomic(ulong,  unsigned long,  "%lu");

START_TEST(test_atomic_ptr)
{
    uint64_t  v0 = 0;
    uint64_t  v1 = 0;
    uint64_t  v2 = 0;
    uint64_t  v3 = 0;
    uint64_t * volatile  val = &v0;

    test_cas(ptr, uint64_t *, "%p", &v0, &v1);
    test_cas(ptr, uint64_t *, "%p", &v1, &v2);
    test_cas(ptr, uint64_t *, "%p", &v2, &v3);
    test_cas(ptr, uint64_t *, "%p", &v3, &v0);
    fail_unless_equal("ptr", "%p", &v0, val);
}
END_TEST


/*-----------------------------------------------------------------------
 * Once
 */

START_TEST(test_once)
{
    cork_once_barrier(once);
    static size_t  call_count = 0;
    static int  value = 0;

#define go \
    do { \
        call_count++; \
        value = 1; \
    } while (0)

    cork_once(once, go);
    fail_unless_equal("Value", "%d", 1, value);
    cork_once(once, go);
    fail_unless_equal("Value", "%d", 1, value);
    cork_once(once, go);
    fail_unless_equal("Value", "%d", 1, value);
    cork_once(once, go);
    fail_unless_equal("Value", "%d", 1, value);

    fail_unless_equal("Call count", "%zu", 1, call_count);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
test_suite()
{
    Suite  *s = suite_create("threads");

    TCase  *tc_atomic = tcase_create("atomic");
    tcase_add_test(tc_atomic, test_atomic_int8);
    tcase_add_test(tc_atomic, test_atomic_int16);
    tcase_add_test(tc_atomic, test_atomic_int32);
    tcase_add_test(tc_atomic, test_atomic_int64);
    tcase_add_test(tc_atomic, test_atomic_uint8);
    tcase_add_test(tc_atomic, test_atomic_uint16);
    tcase_add_test(tc_atomic, test_atomic_uint32);
    tcase_add_test(tc_atomic, test_atomic_uint64);
    tcase_add_test(tc_atomic, test_atomic_short);
    tcase_add_test(tc_atomic, test_atomic_int);
    tcase_add_test(tc_atomic, test_atomic_long);
    tcase_add_test(tc_atomic, test_atomic_ushort);
    tcase_add_test(tc_atomic, test_atomic_uint);
    tcase_add_test(tc_atomic, test_atomic_ulong);
    tcase_add_test(tc_atomic, test_atomic_ptr);
    suite_add_tcase(s, tc_atomic);

    TCase  *tc_basics = tcase_create("basics");
    tcase_add_test(tc_basics, test_once);
    suite_add_tcase(s, tc_basics);

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
