/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, libcork authors
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef TESTS_HELPERS_H
#define TESTS_HELPERS_H

#include "libcork/core/allocator.h"
#include "libcork/core/error.h"


/*-----------------------------------------------------------------------
 * Allocators
 */

/* For the "embedded" tests, use a custom allocator that debugs every
 * allocation.  For the "shared" tests, use the default allocator. */

#if CORK_EMBEDDED_TEST

static void
setup_allocator(void)
{
    struct cork_alloc  *debug = cork_debug_alloc_new(cork_allocator);
    cork_set_allocator(debug);
}

#else /* !CORK_EMBEDDED_TEST */

static void
setup_allocator(void)
{
    /* do nothing */
}

#endif


/*-----------------------------------------------------------------------
 * Error reporting
 */

#if !defined(PRINT_EXPECTED_FAILURES)
#define PRINT_EXPECTED_FAILURES  1
#endif

#if PRINT_EXPECTED_FAILURES
#define print_expected_failure() \
    printf("[expected: %s]\n", cork_error_message());
#else
#define print_expected_failure()  /* do nothing */
#endif


#define DESCRIBE_TEST \
    fprintf(stderr, "--- %s\n", __func__);


#define fail_if_error(call) \
    do { \
        call; \
        if (cork_error_occurred()) { \
            ck_abort_msg(cork_error_message()); \
        } \
    } while (0)

#define fail_unless_error(call, ...) \
    do { \
        call; \
        if (!cork_error_occurred()) { \
            ck_abort_msg(__VA_ARGS__); \
        } else { \
            print_expected_failure(); \
        } \
        cork_error_clear(); \
    } while (0)

#define fail_unless_equal(what, format, expected, actual) \
    (ck_assert_msg((expected) == (actual), \
                 "%s not equal (expected " format \
                 ", got " format ")", \
                 (what), (expected), (actual)))

#define fail_unless_streq(what, expected, actual) \
    (ck_assert_msg(strcmp((expected), (actual)) == 0, \
                 "%s not equal (expected \"%s\", got \"%s\")", \
                 (char *) (what), (char *) (expected), (char *) (actual)))

#define ck_assert_false_msg(expr, ...) ck_assert_msg(!(expr), ## __VA_ARGS__)
#endif /* TESTS_HELPERS_H */
