/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <check.h>

#include "libcork/core/mempool.h"
#include "libcork/core/types.h"

#define DESCRIBE_TEST \
    fprintf(stderr, "--- %s\n", __func__);


/*-----------------------------------------------------------------------
 * Memory pools
 */


START_TEST(test_mempool_01)
{
#define OBJECT_COUNT  16
    DESCRIBE_TEST;
    struct cork_mempool  mp;
    /* Small enough that we'll have to allocate a couple of blocks */
    cork_mempool_init_ex(&mp, int64_t, 64);

    size_t  i;
    int64_t  *objects[OBJECT_COUNT];
    for (i = 0; i < OBJECT_COUNT; i++) {
        fail_if((objects[i] = cork_mempool_new(&mp)) == NULL,
                "Cannot allocate object #%zu", i);
    }

    for (i = 0; i < OBJECT_COUNT; i++) {
        cork_mempool_free(&mp, objects[i]);
    }

    for (i = 0; i < OBJECT_COUNT; i++) {
        fail_if((objects[i] = cork_mempool_new(&mp)) == NULL,
                "Cannot reallocate object #%zu", i);
    }

    for (i = 0; i < OBJECT_COUNT; i++) {
        cork_mempool_free(&mp, objects[i]);
    }

    cork_mempool_done(&mp);
}
END_TEST

START_TEST(test_mempool_fail_01)
{
    DESCRIBE_TEST;
    struct cork_mempool  mp;
    cork_mempool_init(&mp, int64_t);

    int64_t  *obj;
    fail_if((obj = cork_mempool_new(&mp)) == NULL,
            "Cannot allocate object");

    /* This should raise an assertion since we never freed obj. */
    cork_mempool_done(&mp);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
test_suite()
{
    Suite  *s = suite_create("mempool");

    TCase  *tc_mempool = tcase_create("mempool");
    tcase_add_test(tc_mempool, test_mempool_01);
    tcase_add_test_raise_signal(tc_mempool, test_mempool_fail_01, SIGABRT);
    suite_add_tcase(s, tc_mempool);

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

