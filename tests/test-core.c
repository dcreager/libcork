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

#include "libcork/core/allocator.h"
#include "libcork/core/byte-order.h"
#include "libcork/core/error.h"
#include "libcork/core/hash.h"
#include "libcork/core/net-addresses.h"
#include "libcork/core/timestamp.h"
#include "libcork/core/types.h"


/*-----------------------------------------------------------------------
 * Core types
 */

START_TEST(test_bool)
{
    bool  value;

    value = true;
    fail_unless(value, "Unexpected true value");

    value = false;
    fail_if(value, "Unexpected false value");
}
END_TEST

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


START_TEST(test_int_sizeof)
{
    /*
     * Test that our CORK_SIZEOF_FOO preprocessor macros match the
     * results of the builtin sizeof operator.
     */

#define TEST_SIZEOF(TYPE, type) \
    { \
        fail_unless(CORK_SIZEOF_##TYPE == sizeof(type), \
                    "Incorrect size for " #type ": got %zu, expected %zu", \
                    (size_t) CORK_SIZEOF_##TYPE, \
                    (size_t) sizeof(type)); \
    }

    TEST_SIZEOF(SHORT, short)
    TEST_SIZEOF(SHORT, unsigned short)
    TEST_SIZEOF(INT, int)
    TEST_SIZEOF(INT, unsigned int)
    TEST_SIZEOF(LONG, long)
    TEST_SIZEOF(LONG, unsigned long)
    TEST_SIZEOF(POINTER, void *)
    TEST_SIZEOF(POINTER, int *)
    TEST_SIZEOF(POINTER, void (*)(void))

#undef TEST_SIZEOF
}
END_TEST


/*-----------------------------------------------------------------------
 * Endianness
 */

START_TEST(test_endianness)
{
#define TEST_ENDIAN(TYPE, type, sz, expected, ...) \
    { \
        union { uint8_t octets[sz]; type val; }  u = \
            { { __VA_ARGS__ } }; \
        \
        type  from_big = CORK_##TYPE##_BIG_TO_HOST(u.val); \
        fail_unless(from_big == expected, \
                    "Unexpected big-to-host " #type " value"); \
        \
        type  from_big_in_place = u.val; \
        CORK_##TYPE##_BIG_TO_HOST_IN_PLACE(from_big_in_place); \
        fail_unless(from_big_in_place == expected, \
                    "Unexpected in-place big-to-host " #type " value"); \
        \
        type  to_big = CORK_##TYPE##_HOST_TO_BIG(expected); \
        fail_unless(to_big == u.val, \
                    "Unexpected host-to-big " #type " value"); \
        \
        type  to_big_in_place = expected; \
        CORK_##TYPE##_HOST_TO_BIG_IN_PLACE(to_big_in_place); \
        fail_unless(to_big_in_place == u.val, \
                    "Unexpected in-place host-to-big " #type " value"); \
        \
        int  i; \
        for (i = 0; i < sz/2; i++) { \
            uint8_t  tmp = u.octets[i]; \
            u.octets[i] = u.octets[sz-i-1]; \
            u.octets[sz-i-1] = tmp; \
        } \
        \
        type  from_little = CORK_##TYPE##_LITTLE_TO_HOST(u.val); \
        fail_unless(from_little == expected, \
                    "Unexpected little-to-host " #type " value"); \
        \
        type  from_little_in_place = u.val; \
        CORK_##TYPE##_LITTLE_TO_HOST_IN_PLACE(from_little_in_place); \
        fail_unless(from_little_in_place == expected, \
                    "Unexpected in-place little-to-host " #type " value"); \
        \
        type  to_little = CORK_##TYPE##_HOST_TO_LITTLE(expected); \
        fail_unless(to_little == u.val, \
                    "Unexpected host-to-little " #type " value"); \
        \
        type  to_little_in_place = expected; \
        CORK_##TYPE##_HOST_TO_LITTLE_IN_PLACE(to_little_in_place); \
        fail_unless(to_little_in_place == u.val, \
                    "Unexpected in-place host-to-little " #type " value"); \
    }

    TEST_ENDIAN(UINT16, uint16_t, 2, 0x0102, 1, 2);
    TEST_ENDIAN(UINT32, uint32_t, 4, 0x01020304, 1, 2, 3, 4);
    TEST_ENDIAN(UINT64, uint64_t, 8, UINT64_C(0x0102030405060708),
                1, 2, 3, 4, 5, 6, 7, 8);

#undef TEST_ENDIAN
}
END_TEST


/*-----------------------------------------------------------------------
 * Hash values
 */

START_TEST(test_hash)
{
    static const char  BUF[] = "test";
    static size_t  LEN = sizeof(BUF);

    /* without the NUL terminator */
    fail_unless(cork_hash_buffer(0, BUF, LEN-1) == 0xba6bd213,
                "Unexpected hash value 0x%08lx",
                (unsigned long) cork_hash_buffer(0, BUF, LEN-1));
    /* with the NUL terminator */
    fail_unless(cork_hash_buffer(0, BUF, LEN)   == 0x586fce33,
                "Unexpected hash value 0x%08lx",
                (unsigned long) cork_hash_buffer(0, BUF, LEN));

    uint32_t  val32 = 1234;
    fail_unless(cork_hash_variable(0, val32) == 0x6bb65380,
                "Unexpected hash value: 0x%08lx",
                (unsigned long) cork_hash_variable(0, val32));

    uint64_t  val64 = 1234;
    fail_unless(cork_hash_variable(0, val64) == 0x4d5c4063,
                "Unexpected hash value: 0x%08lx",
                (unsigned long) cork_hash_variable(0, val64));
}
END_TEST


/*-----------------------------------------------------------------------
 * Allocation
 */

static void
allocator_tests(struct cork_alloc *alloc)
{
    void  *buf = cork_malloc(alloc, 100);
    fail_if(buf == NULL, "Couldn't allocate buffer");
    buf = cork_realloc(alloc, buf, 100, 200);
    fail_if(buf == NULL, "Couldn't reallocate buffer");
    cork_free(alloc, buf, 200);

    double  *d = cork_new(alloc, double);
    fail_if(d == NULL, "Couldn't allocate new double");
    cork_delete(alloc, double, d);

    const char  *str_src = "hello there";
    const char  *str_copied = cork_strdup(alloc, str_src);
    fail_unless(strcmp(str_src, str_copied) == 0,
                "Strings not equal");
    cork_strfree(alloc, str_copied);
}

START_TEST(test_default_allocator)
{
    struct cork_alloc  *alloc = cork_allocator_new_malloc();
    allocator_tests(alloc);
    cork_allocator_free(alloc);
}
END_TEST


START_TEST(test_debug_allocator)
{
    struct cork_alloc  *alloc = cork_allocator_new_debug();
    allocator_tests(alloc);
    cork_allocator_free(alloc);
}
END_TEST


/*-----------------------------------------------------------------------
 * Hierarchical allocator
 */

struct halloc_value {
    size_t  *count;
};

static void
decrement_count(cork_halloc *ptr)
{
    struct halloc_value  *u = ptr;
    (*u->count)--;
}

START_TEST(test_halloc)
{
    struct cork_alloc  *alloc = cork_allocator_new_debug();
    cork_halloc  *root = cork_halloc_new_root(alloc);

    size_t  count = 4;
    struct halloc_value  *val1;
    struct halloc_value  *val2;
    struct halloc_value  *val3;
    struct halloc_value  *val4;

    val1 = cork_halloc_new(root, struct halloc_value);
    cork_halloc_set_destructor(val1, decrement_count);
    val1->count = &count;

    val2 = cork_halloc_new(val1, struct halloc_value);
    cork_halloc_set_destructor(val2, decrement_count);
    val2->count = &count;

    val3 = cork_halloc_new(val1, struct halloc_value);
    cork_halloc_set_destructor(val3, decrement_count);
    val3->count = &count;

    val4 = cork_halloc_new(val3, struct halloc_value);
    cork_halloc_set_destructor(val4, decrement_count);
    val4->count = &count;

    /* Reallocate one of the pointers to make sure we can update the
     * tree state correctly. */
    val1 = cork_halloc_realloc(val1, sizeof(struct halloc_value) * 2);

    /* Free the root and make sure that all destructors were called. */
    cork_halloc_free(root);
    fail_unless(count == 0,
                "Unexpected final count: got %zu, expected 0",
                count);

    cork_allocator_free(alloc);
}
END_TEST


/*-----------------------------------------------------------------------
 * IP addresses
 */

START_TEST(test_ipv4_address)
{
#define ROUND_TRIP(str) \
    { \
        struct cork_ipv4  addr; \
        struct cork_error  error = CORK_ERROR_INIT(alloc); \
        bool result = cork_ipv4_init(&addr, str, &error); \
        fail_unless(result, \
                    "Could not initialize IPv4 address from %s: %s", \
                    str, cork_error_message(&error)); \
        char  actual[CORK_IPV4_STRING_LENGTH]; \
        cork_ipv4_to_raw_string(&addr, actual); \
        fail_unless(strcmp(actual, str) == 0, \
                    "Unexpected string representation: " \
                    "got %s, expected %s", \
                    actual, str); \
        \
        struct cork_ipv4  addr2; \
        cork_ipv4_init(&addr2, str, NULL); \
        fail_unless(cork_ipv4_equal(&addr, &addr2), \
                    "IPv4 cork_eq_t instances should be equal"); \
    }

#define BAD(str) \
    { \
        struct cork_ipv4  addr; \
        struct cork_error  error = CORK_ERROR_INIT(alloc); \
        fail_if(cork_ipv4_init(&addr, str, &error), \
                "Shouldn't be able to initialize IPv4 address from %s", \
                str); \
        cork_error_done(&error); \
    }

    struct cork_alloc  *alloc = cork_allocator_new_debug();

    ROUND_TRIP("192.168.1.100");
    BAD("192.168.0.");
    BAD("fe80::1");
    BAD("::ffff:192.168.1.100");
    BAD("abcd");

    cork_allocator_free(alloc);

#undef ROUND_TRIP
#undef BAD
}
END_TEST


START_TEST(test_ipv6_address)
{
#define ROUND_TRIP(str) \
    { \
        struct cork_ipv6  addr; \
        struct cork_error  error = CORK_ERROR_INIT(alloc); \
        bool result = cork_ipv6_init(&addr, str, &error); \
        fail_unless(result, \
                    "Could not initialize IPv6 address from %s: %s", \
                    str, cork_error_message(&error)); \
        char  actual[CORK_IPV6_STRING_LENGTH]; \
        cork_ipv6_to_raw_string(&addr, actual); \
        fail_unless(strcmp(actual, str) == 0, \
                    "Unexpected string representation: " \
                    "got %s, expected %s", \
                    actual, str); \
        \
        struct cork_ipv6  addr2; \
        cork_ipv6_init(&addr2, str, NULL); \
        fail_unless(cork_ipv6_equal(&addr, &addr2), \
                    "IPv6 cork_eq_t instances should be equal"); \
    }

#define BAD(str) \
    { \
        struct cork_ipv6  addr; \
        struct cork_error  error = CORK_ERROR_INIT(alloc); \
        fail_if(cork_ipv6_init(&addr, str, &error), \
                "Shouldn't be able to initialize IPv6 address from %s", \
                str); \
        cork_error_done(&error); \
    }

    struct cork_alloc  *alloc = cork_allocator_new_debug();

    ROUND_TRIP("fe80::1");
    ROUND_TRIP("::ffff:192.168.1.100");
    BAD("fe80:");
    BAD("fe80::1::2");
    BAD("192.168.1.100");
    BAD("abcd");

    cork_allocator_free(alloc);

#undef ROUND_TRIP
#undef BAD
}
END_TEST


START_TEST(test_ip_address)
{
    struct cork_ip  addr;

#define ROUND_TRIP(str) \
    { \
        struct cork_ip  addr; \
        struct cork_error  error = CORK_ERROR_INIT(alloc); \
        bool result = cork_ip_init(&addr, str, &error); \
        fail_unless(result, \
                    "Could not initialize IP address from %s: %s", \
                    str, cork_error_message(&error)); \
        char  actual[CORK_IP_STRING_LENGTH]; \
        cork_ip_to_raw_string(&addr, actual); \
        fail_unless(strcmp(actual, str) == 0, \
                    "Unexpected string representation: " \
                    "got %s, expected %s", \
                    actual, str); \
        \
        struct cork_ip  addr2; \
        cork_ip_init(&addr2, str, NULL); \
        fail_unless(cork_ip_equal(&addr, &addr2), \
                    "IP cork_eq_t instances should be equal"); \
    }

#define BAD(str) \
    { \
        struct cork_ip  addr; \
        struct cork_error  error = CORK_ERROR_INIT(alloc); \
        fail_if(cork_ip_init(&addr, str, &error), \
                "Shouldn't be able to initialize IP address from %s", \
                str); \
        cork_error_done(&error); \
    }

    struct cork_alloc  *alloc = cork_allocator_new_debug();

    ROUND_TRIP("192.168.1.100");
    ROUND_TRIP("fe80::1");
    ROUND_TRIP("::ffff:192.168.1.100");
    BAD("192.168.0.");
    BAD("fe80:");
    BAD("fe80::1::2");
    BAD("abcd");

#undef ROUND_TRIP
#undef BAD

    struct cork_ipv4  addr4;
    struct cork_ipv6  addr6;

    cork_ip_init(&addr, "192.168.1.1", NULL);
    cork_ipv4_init(&addr4, "192.168.1.1", NULL);
    fail_unless(addr.version == 4,
                "Unexpected IP address version (expected 4, got %u)",
                addr.version);
    fail_unless(cork_ipv4_equal(&addr.ip.v4, &addr4),
                "IP addresses should be equal");

    cork_ip_init(&addr, "fe80::1", NULL);
    cork_ipv6_init(&addr6, "fe80::1", NULL);
    fail_unless(addr.version == 6,
                "Unexpected IP address version (expected 6, got %u)",
                addr.version);
    fail_unless(cork_ipv6_equal(&addr.ip.v6, &addr6),
                "IP addresses should be equal");

    cork_allocator_free(alloc);
}
END_TEST


/*-----------------------------------------------------------------------
 * Timestamps
 */

START_TEST(test_timestamp)
{
    static char  buf[4096];
    static size_t  size = sizeof(buf);

    static const uint32_t  TEST_TIME_1 = 700000000;
    static const char  *FORMATTED_TIME_1 = "1992-03-07 20:26:40";

    static const uint32_t  TEST_TIME_2 = 1200000000;
    static const char  *FORMATTED_TIME_2 = "2008-01-10 21:20:00";

    static const uint32_t  TEST_TIME_3 = 1305180745;
    static const char  *FORMATTED_TIME_3 = "2011-05-12 06:12:25";

    cork_timestamp  ts;

#define test(unit, expected) \
    fail_unless(cork_timestamp_##unit(ts) == expected, \
                "Unexpected " #unit " portion of timestamp " \
                "(got %lu, expected %lu)", \
                (unsigned long) cork_timestamp_##unit(ts), \
                (unsigned long) expected);

#define test_format(expected) \
    fail_unless(cork_timestamp_format_utc(ts, "%Y-%m-%d %H:%M:%S", buf, size), \
                "Cannot format timestamp"); \
    fail_unless(strcmp(buf, expected) == 0, \
                "Unexpected formatted time (got %s, expected %s)", \
                buf, expected);

    cork_timestamp_init_sec(&ts, TEST_TIME_1);
    test(sec, TEST_TIME_1);
    test(gsec, 0);
    test_format(FORMATTED_TIME_1);

    cork_timestamp_init_sec(&ts, TEST_TIME_2);
    test(sec, TEST_TIME_2);
    test(gsec, 0);
    test_format(FORMATTED_TIME_2);

    cork_timestamp_init_sec(&ts, TEST_TIME_3);
    test(sec, TEST_TIME_3);
    test(gsec, 0);
    test_format(FORMATTED_TIME_3);

    cork_timestamp_init_msec(&ts, TEST_TIME_1, 500);
    test(sec, TEST_TIME_1);
    test(gsec, 1 << 31);

    cork_timestamp_init_usec(&ts, TEST_TIME_1, 500000);
    test(sec, TEST_TIME_1);
    test(gsec, 1 << 31);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
test_suite()
{
    Suite  *s = suite_create("core");

    TCase  *tc_types = tcase_create("types");
    tcase_add_test(tc_types, test_bool);
    tcase_add_test(tc_types, test_int_types);
    tcase_add_test(tc_types, test_int_sizeof);
    suite_add_tcase(s, tc_types);

    TCase  *tc_endianness = tcase_create("endianness");
    tcase_add_test(tc_endianness, test_endianness);
    suite_add_tcase(s, tc_endianness);

    TCase  *tc_hash = tcase_create("hash");
    tcase_add_test(tc_hash, test_hash);
    suite_add_tcase(s, tc_hash);

    TCase  *tc_allocation = tcase_create("allocator");
    tcase_add_test(tc_allocation, test_default_allocator);
    tcase_add_test(tc_allocation, test_debug_allocator);
    tcase_add_test(tc_allocation, test_halloc);
    suite_add_tcase(s, tc_allocation);

    TCase  *tc_addresses = tcase_create("net-addresses");
    tcase_add_test(tc_addresses, test_ipv4_address);
    tcase_add_test(tc_addresses, test_ipv6_address);
    tcase_add_test(tc_addresses, test_ip_address);
    suite_add_tcase(s, tc_addresses);

    TCase  *tc_timestamp = tcase_create("timestamp");
    tcase_add_test(tc_timestamp, test_timestamp);
    suite_add_tcase(s, tc_timestamp);

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
