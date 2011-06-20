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
#include "libcork/ds/dllist.h"


/*-----------------------------------------------------------------------
 * Doubly-linked lists
 */

struct int64_item {
    int64_t  value;
    cork_dllist_item_t  element;
};

static void
int64_sum(cork_dllist_item_t *element, void *user_data)
{
    int64_t  *sum = user_data;
    struct int64_item  *item =
        cork_container_of(element, struct int64_item, element);
    *sum += item->value;
}

START_TEST(test_dllist)
{
    cork_dllist_t  list;
    cork_dllist_init(&list);

    fail_unless(cork_dllist_size(&list) == 0,
                "Unexpected size of list: got %zu, expected 0",
                cork_dllist_size(&list));

    struct int64_item  item1;
    struct int64_item  item2;
    struct int64_item  item3;

    item1.value = 1;
    cork_dllist_add(&list, &item1.element);
    fail_unless(cork_dllist_size(&list) == 1,
                "Unexpected size of list: got %zu, expected 1",
                cork_dllist_size(&list));

    item2.value = 2;
    cork_dllist_add(&list, &item2.element);
    fail_unless(cork_dllist_size(&list) == 2,
                "Unexpected size of list: got %zu, expected 2",
                cork_dllist_size(&list));

    item3.value = 3;
    cork_dllist_add(&list, &item3.element);
    fail_unless(cork_dllist_size(&list) == 3,
                "Unexpected size of list: got %zu, expected 3",
                cork_dllist_size(&list));

    int64_t  sum = 0;
    cork_dllist_map(&list, int64_sum, &sum);
    fail_unless(sum == 6,
                "Unexpected sum, got %ld, expected 6",
                (long) sum);

    cork_dllist_remove(&item2.element);
    fail_unless(cork_dllist_size(&list) == 2,
                "Unexpected size of list: got %zu, expected 2",
                cork_dllist_size(&list));
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
test_suite()
{
    Suite  *s = suite_create("ds");

    TCase  *tc_ds = tcase_create("ds");
    tcase_add_test(tc_ds, test_dllist);
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
