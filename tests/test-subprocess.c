/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011-2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <check.h>

#include "libcork/config.h"
#include "libcork/core.h"
#include "libcork/ds.h"
#include "libcork/os.h"

#include "helpers.h"


/*-----------------------------------------------------------------------
 * Verifying stream consumer
 */

struct verify_consumer {
    struct cork_stream_consumer  parent;
    struct cork_buffer  buf;
    const char  *name;
    const char  *expected;
};

static int
verify_consumer__data(struct cork_stream_consumer *vself,
                      const void *buf, size_t size, bool is_first)
{
    struct verify_consumer  *self =
        cork_container_of(vself, struct verify_consumer, parent);
    if (is_first) {
        cork_buffer_clear(&self->buf);
    }
    cork_buffer_append(&self->buf, buf, size);
    return 0;
}

static int
verify_consumer__eof(struct cork_stream_consumer *vself)
{
    struct verify_consumer  *self =
        cork_container_of(vself, struct verify_consumer, parent);
    const char  *actual = self->buf.buf;
    if (actual == NULL) {
        actual = "";
    }
    fail_unless(strcmp(actual, self->expected) == 0,
                "Unexpected %s: got\n%s\nexpected\n%s\n", self->name,
                actual, self->expected);
    return 0;
}

static void
verify_consumer__free(struct cork_stream_consumer *vself)
{
    struct verify_consumer  *self =
        cork_container_of(vself, struct verify_consumer, parent);
    cork_buffer_done(&self->buf);
    cork_strfree(self->name);
    cork_strfree(self->expected);
    free(self);
}

struct cork_stream_consumer *
verify_consumer_new(const char *name, const char *expected)
{
    struct verify_consumer  *self = cork_new(struct verify_consumer);
    self->parent.data = verify_consumer__data;
    self->parent.eof = verify_consumer__eof;
    self->parent.free = verify_consumer__free;
    cork_buffer_init(&self->buf);
    self->name = cork_strdup(name);
    self->expected = cork_strdup(expected);
    return &self->parent;
}


/*-----------------------------------------------------------------------
 * Helpers
 */

struct spec {
    char  *program;
    char * const  *params;
    const char  *expected_stdout;
    const char  *expected_stderr;
    struct cork_stream_consumer  *verify_stdout;
    struct cork_stream_consumer  *verify_stderr;
};

static void
test_subprocesses_(size_t spec_count, struct spec **specs)
{
    size_t  i;
    struct cork_subprocess  **subs =
        cork_calloc(spec_count, sizeof(struct cork_subprocess *));

    for (i = 0; i < spec_count; i++) {
        struct spec  *spec = specs[i];
        spec->verify_stdout = verify_consumer_new("stdout", spec->expected_stdout);
        spec->verify_stderr = verify_consumer_new("stderr", spec->expected_stderr);
        fail_if_error(subs[i] = cork_subprocess_new_exec
                      (spec->program, spec->params,
                       spec->verify_stdout, spec->verify_stderr, 0));
    }

    fail_if_error(cork_subprocess_start_and_wait(spec_count, subs));

    for (i = 0; i < spec_count; i++) {
        struct spec  *spec = specs[i];
        cork_stream_consumer_free(spec->verify_stdout);
        cork_stream_consumer_free(spec->verify_stderr);
        cork_subprocess_free(subs[i]);
    }

    free(subs);
}

#define test_subprocesses(specs) \
    test_subprocesses_(sizeof(specs) / sizeof(specs[0]), specs)


/*-----------------------------------------------------------------------
 * Subprocesses
 */

static char  *echo_01_params[] = { "echo", "hello", "world", NULL };
static struct spec  echo_01 = {
    "echo", echo_01_params, "hello world\n", ""
};

static char  *echo_02_params[] = { "echo", "foo", "bar", "baz", NULL };
static struct spec  echo_02 = {
    "echo", echo_02_params, "foo bar baz\n", ""
};


START_TEST(test_subprocess_01)
{
    DESCRIBE_TEST;
    struct spec  *specs[] = { &echo_01 };
    test_subprocesses(specs);
}
END_TEST


START_TEST(test_subprocess_02)
{
    DESCRIBE_TEST;
    struct spec  *specs[] = { &echo_02 };
    test_subprocesses(specs);
}
END_TEST


START_TEST(test_subprocess_03)
{
    DESCRIBE_TEST;
    struct spec  *specs[] = { &echo_01, &echo_02 };
    test_subprocesses(specs);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
test_suite()
{
    Suite  *s = suite_create("subprocess");

    TCase  *tc_subprocess = tcase_create("subprocess");
    tcase_add_test(tc_subprocess, test_subprocess_01);
    tcase_add_test(tc_subprocess, test_subprocess_02);
    tcase_add_test(tc_subprocess, test_subprocess_03);
    suite_add_tcase(s, tc_subprocess);

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
