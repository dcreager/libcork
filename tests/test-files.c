/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2013, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <check.h>

#include "libcork/core/types.h"
#include "libcork/os/files.h"

#include "helpers.h"


/*-----------------------------------------------------------------------
 * Paths
 */

void
verify_path_content(struct cork_path *path, const char *expected)
{
    fail_if(cork_path_get(path) == NULL, "Path should not have NULL content");
    fail_unless_streq("Paths", expected, cork_path_get(path));
}

void
test_path(const char *p, const char *expected)
{
    struct cork_path  *path;
    struct cork_path  *cloned;

    fprintf(stderr, "path(\"%s\") ?= \"%s\"\n",
            (p == NULL)? "": p,
            expected);

    path = cork_path_new(p);
    verify_path_content(path, expected);
    cloned = cork_path_clone(path);
    verify_path_content(cloned, expected);
    cork_path_free(path);
    cork_path_free(cloned);
}

START_TEST(test_path_01)
{
    DESCRIBE_TEST;
    test_path(NULL, "");
    test_path("a", "a");
    test_path("a/b", "a/b");
}
END_TEST


void
test_join(const char *p1, const char *p2, const char *expected)
{
    struct cork_path  *path1;
    struct cork_path  *path2;
    struct cork_path  *actual;

    fprintf(stderr, "join(\"%s\", \"%s\") ?= \"%s\"\n",
            (p1 == NULL)? "": p1,
            (p2 == NULL)? "": p2,
            expected);

    /* Try cork_path_join */
    path1 = cork_path_new(p1);
    actual = cork_path_join(path1, p2);
    verify_path_content(actual, expected);
    cork_path_free(path1);
    cork_path_free(actual);

    /* Try cork_path_join_path */
    path1 = cork_path_new(p1);
    path2 = cork_path_new(p2);
    actual = cork_path_join_path(path1, path2);
    verify_path_content(actual, expected);
    cork_path_free(path1);
    cork_path_free(path2);
    cork_path_free(actual);

    /* Try cork_path_append */
    actual = cork_path_new(p1);
    cork_path_append(actual, p2);
    verify_path_content(actual, expected);
    cork_path_free(actual);

    /* Try cork_path_append_path */
    actual = cork_path_new(p1);
    path2 = cork_path_new(p2);
    cork_path_append_path(actual, path2);
    verify_path_content(actual, expected);
    cork_path_free(path2);
    cork_path_free(actual);
}

START_TEST(test_path_join_01)
{
    DESCRIBE_TEST;
    test_join("a", "b",    "a/b");
    test_join("a/", "b",   "a/b");
    test_join("", "a/b",   "a/b");
    test_join("a/b", "",   "a/b");
    test_join(NULL, "a/b", "a/b");
    test_join("a/b", NULL, "a/b");
}
END_TEST

START_TEST(test_path_join_02)
{
    DESCRIBE_TEST;
    test_join("", "/b",   "/b");
    test_join(NULL, "/b", "/b");
    test_join("a", "/b",  "/b");
    test_join("a/", "/b", "/b");
}
END_TEST


void
test_basename(const char *p, const char *expected)
{
    struct cork_path  *path;
    struct cork_path  *actual;

    fprintf(stderr, "basename(\"%s\") ?= \"%s\"\n",
            (p == NULL)? "": p,
            expected);

    /* Try cork_path_basename */
    path = cork_path_new(p);
    actual = cork_path_basename(path);
    verify_path_content(actual, expected);
    cork_path_free(path);
    cork_path_free(actual);

    /* Try cork_path_set_basename */
    actual = cork_path_new(p);
    cork_path_set_basename(actual);
    verify_path_content(actual, expected);
    cork_path_free(actual);
}

START_TEST(test_path_basename_01)
{
    DESCRIBE_TEST;
    test_basename("", "");
    test_basename(NULL, "");
    test_basename("a", "a");
    test_basename("a/", "");
    test_basename("a/b", "b");
    test_basename("a/b/", "");
    test_basename("a/b/c", "c");
}
END_TEST


void
test_dirname(const char *p, const char *expected)
{
    struct cork_path  *path;
    struct cork_path  *actual;

    fprintf(stderr, "dirname(\"%s\") ?= \"%s\"\n",
            (p == NULL)? "": p,
            expected);

    /* Try cork_path_dirname */
    path = cork_path_new(p);
    actual = cork_path_dirname(path);
    verify_path_content(actual, expected);
    cork_path_free(path);
    cork_path_free(actual);

    /* Try cork_path_set_dirname */
    actual = cork_path_new(p);
    cork_path_set_dirname(actual);
    verify_path_content(actual, expected);
    cork_path_free(actual);
}

START_TEST(test_path_dirname_01)
{
    DESCRIBE_TEST;
    test_dirname("", "");
    test_dirname(NULL, "");
    test_dirname("a", "a");
    test_dirname("a/", "a");
    test_dirname("a/b", "a");
    test_dirname("a/b/", "a/b");
    test_dirname("a/b/c", "a/b");
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
test_suite()
{
    Suite  *s = suite_create("path");

    TCase  *tc_path = tcase_create("path");
    tcase_add_test(tc_path, test_path_01);
    tcase_add_test(tc_path, test_path_join_01);
    tcase_add_test(tc_path, test_path_join_02);
    tcase_add_test(tc_path, test_path_basename_01);
    tcase_add_test(tc_path, test_path_dirname_01);
    suite_add_tcase(s, tc_path);

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
