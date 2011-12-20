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
#include "libcork/core/gc.h"
#include "libcork/core/types.h"

#define DESCRIBE_TEST \
    fprintf(stderr, "--- %s\n", __func__);

/*-----------------------------------------------------------------------
 * Garbage collector
 */

struct tree {
    int  id;
    struct tree  *left;
    struct tree  *right;
};

static void
tree_recurse(struct cork_gc *gc, void *vself,
             cork_gc_recurser recurse, void *ud)
{
    struct tree  *self = vself;
    recurse(gc, self->left, ud);
    recurse(gc, self->right, ud);
}

static struct cork_gc_obj_iface TREE_IFACE = {
    NULL,
    tree_recurse
};

struct tree *
tree_new(struct cork_gc *gc, int id, struct tree *l, struct tree *r)
{
    struct tree  *self = cork_gc_new(gc, struct tree, &TREE_IFACE);
    self->id = id;
    self->left = cork_gc_incref(gc, l);
    self->right = cork_gc_incref(gc, r);
    return self;
}

START_TEST(test_gc_acyclic_01)
{
    DESCRIBE_TEST;
    struct cork_alloc  *alloc = cork_allocator_new_debug();
    struct cork_gc  gc;
    cork_gc_init(&gc, alloc);

    struct tree  *t1 = tree_new(&gc, 0, NULL, NULL);
    struct tree  *t2 = tree_new(&gc, 0, NULL, NULL);
    struct tree  *t0 = tree_new(&gc, 0, t1, t2);

    cork_gc_decref(&gc, t1);
    cork_gc_decref(&gc, t2);
    cork_gc_decref(&gc, t0);

    cork_gc_done(&gc);
    cork_allocator_free(alloc);
}
END_TEST

START_TEST(test_gc_cyclic_01)
{
    DESCRIBE_TEST;
    struct cork_alloc  *alloc = cork_allocator_new_debug();
    struct cork_gc  gc;
    cork_gc_init(&gc, alloc);

    struct tree  *t1 = tree_new(&gc, 0, NULL, NULL);
    struct tree  *t2 = tree_new(&gc, 0, NULL, NULL);
    struct tree  *t0 = tree_new(&gc, 0, t1, t2);

    t1->left = cork_gc_incref(&gc, t0);

    cork_gc_decref(&gc, t1);
    cork_gc_decref(&gc, t2);
    cork_gc_decref(&gc, t0);

    cork_gc_done(&gc);
    cork_allocator_free(alloc);
}
END_TEST

START_TEST(test_gc_cyclic_02)
{
    DESCRIBE_TEST;
    struct cork_alloc  *alloc = cork_allocator_new_debug();
    struct cork_gc  gc;
    cork_gc_init(&gc, alloc);

    struct tree  *t1 = tree_new(&gc, 0, NULL, NULL);
    struct tree  *t2 = tree_new(&gc, 0, NULL, NULL);
    struct tree  *t0 = tree_new(&gc, 0, t1, t2);

    t1->left = cork_gc_incref(&gc, t0);
    t2->left = cork_gc_incref(&gc, t2);
    t2->right = cork_gc_incref(&gc, t0);

    cork_gc_decref(&gc, t1);
    cork_gc_decref(&gc, t2);
    cork_gc_decref(&gc, t0);

    cork_gc_done(&gc);
    cork_allocator_free(alloc);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
test_suite()
{
    Suite  *s = suite_create("gc");

    TCase  *tc_gc = tcase_create("gc");
    tcase_add_test(tc_gc, test_gc_acyclic_01);
    tcase_add_test(tc_gc, test_gc_cyclic_01);
    tcase_add_test(tc_gc, test_gc_cyclic_02);
    suite_add_tcase(s, tc_gc);

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

