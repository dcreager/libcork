/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009-2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdio.h>

#include <check.h>

#include "libcork/core/allocator.h"
#include "libcork/core/checkers.h"
#include "libcork/core/types.h"
#include "libcork/ds/managed-buffer.h"
#include "libcork/ds/slice.h"

#include "helpers.h"


/*-----------------------------------------------------------------------
 * Helper functions
 */

struct flag_buffer {
    struct cork_managed_buffer  parent;
    bool  *flag;
};

static void
set_flag_on_free(struct cork_alloc *alloc, struct cork_managed_buffer *mbuf)
{
    struct flag_buffer  *fbuf =
        cork_container_of(mbuf, struct flag_buffer, parent);
    *fbuf->flag = true;
    cork_delete(alloc, struct flag_buffer, fbuf);
}

static struct cork_managed_buffer_iface  FLAG__MANAGED_BUFFER = {
    set_flag_on_free
};

static struct cork_managed_buffer *
flag_buffer_new(struct cork_alloc *alloc,
                const void *buf, size_t size,
                bool *flag, struct cork_error *err)
{
    struct flag_buffer  *fbuf;
    rp_check_new(struct flag_buffer, fbuf, "flag buffer");
    fbuf->parent.buf = buf;
    fbuf->parent.size = size;
    fbuf->parent.ref_count = 1;
    fbuf->parent.iface = &FLAG__MANAGED_BUFFER;
    fbuf->flag = flag;
    return &fbuf->parent;
}



/*-----------------------------------------------------------------------
 * Buffer reference counting
 */

START_TEST(test_managed_buffer_refcount)
{
    struct cork_alloc  *alloc = cork_allocator_new_debug();

    bool  flag = false;

    /*
     * Make a bunch of references, unreference them all, and then
     * verify that the free function got called.
     */

    struct cork_managed_buffer  *pb0;
    fail_if_error(pb0 = flag_buffer_new(alloc, NULL, 0, &flag, &err));
    struct cork_managed_buffer  *pb1 = cork_managed_buffer_ref(alloc, pb0);
    struct cork_managed_buffer  *pb2 = cork_managed_buffer_ref(alloc, pb0);
    struct cork_managed_buffer  *pb3 = cork_managed_buffer_ref(alloc, pb2);

    cork_managed_buffer_unref(alloc, pb0);
    cork_managed_buffer_unref(alloc, pb1);
    cork_managed_buffer_unref(alloc, pb2);
    cork_managed_buffer_unref(alloc, pb3);

    fail_unless(flag,
                "Managed buffer free function never called.");

    cork_allocator_free(alloc);
}
END_TEST


START_TEST(test_managed_buffer_bad_refcount)
{
    struct cork_alloc  *alloc = cork_allocator_new_debug();

    bool  flag = false;

    /*
     * Make a bunch of references, forget to unreference one of them,
     * and then verify that the free function didn't called.
     */

    struct cork_managed_buffer  *pb0;
    fail_if_error(pb0 = flag_buffer_new(alloc, NULL, 0, &flag, &err));
    struct cork_managed_buffer  *pb1 = cork_managed_buffer_ref(alloc, pb0);
    struct cork_managed_buffer  *pb2 = cork_managed_buffer_ref(alloc, pb0);
    struct cork_managed_buffer  *pb3 = cork_managed_buffer_ref(alloc, pb2);

    cork_managed_buffer_unref(alloc, pb0);
    cork_managed_buffer_unref(alloc, pb1);
    cork_managed_buffer_unref(alloc, pb2);
    /* cork_managed_buffer_unref(pb3);   OH NO! */
    (void) pb3;

    fail_if(flag,
            "Managed buffer free function was called unexpectedly.");

    /* free the buffer here to quiet valgrind */
    cork_managed_buffer_unref(alloc, pb3);
    cork_allocator_free(alloc);
}
END_TEST


/*-----------------------------------------------------------------------
 * Slicing
 */

START_TEST(test_slice)
{
    struct cork_alloc  *alloc = cork_allocator_new_debug();

    /* Try to slice a NULL buffer. */
    struct cork_slice  ps1;

    fail_unless_error(cork_managed_buffer_slice
                      (alloc, &ps1, NULL, 0, 0, &err),
                      "Shouldn't be able to slice a NULL buffer");
    fail_unless_error(cork_managed_buffer_slice_offset
                      (alloc, &ps1, NULL, 0, &err),
                      "Shouldn't be able to slice a NULL buffer");

    fail_unless_error(cork_slice_copy
                      (alloc, &ps1, NULL, 0, 0, &err),
                      "Shouldn't be able to slice a NULL slice");
    fail_unless_error(cork_slice_copy_offset
                      (alloc, &ps1, NULL, 0, &err),
                      "Shouldn't be able to slice a NULL slice");

    cork_allocator_free(alloc);
}
END_TEST


/*-----------------------------------------------------------------------
 * Slice reference counting
 */

START_TEST(test_slice_refcount)
{
    struct cork_alloc  *alloc = cork_allocator_new_debug();

    bool  flag = false;

    /*
     * Make a bunch of slices, finish them all, and then verify that
     * the free function got called.
     */

    static char  *BUF =
        "abcdefg";
    static size_t  LEN = 7;

    struct cork_managed_buffer  *pb;
    fail_if_error(pb = flag_buffer_new(alloc, BUF, LEN, &flag, &err));

    struct cork_slice  ps1;
    struct cork_slice  ps2;
    struct cork_slice  ps3;

    fail_if_error(cork_managed_buffer_slice(alloc, &ps1, pb, 0, 7, &err));
    fail_if_error(cork_managed_buffer_slice(alloc, &ps2, pb, 1, 1, &err));
    fail_if_error(cork_managed_buffer_slice(alloc, &ps3, pb, 4, 3, &err));

    cork_managed_buffer_unref(alloc, pb);
    cork_slice_finish(alloc, &ps1);
    cork_slice_finish(alloc, &ps2);
    cork_slice_finish(alloc, &ps3);

    fail_unless(flag,
                "Managed buffer free function never called.");

    cork_allocator_free(alloc);
}
END_TEST


START_TEST(test_slice_bad_refcount)
{
    struct cork_alloc  *alloc = cork_allocator_new_debug();

    bool  flag = false;

    /*
     * Make a bunch of slices, forget to finish one of them, and then
     * verify that the free function didn't called.
     */

    static char  *BUF =
        "abcdefg";
    static size_t  LEN = 7;

    struct cork_managed_buffer  *pb;
    fail_if_error(pb = flag_buffer_new(alloc, BUF, LEN, &flag, &err));

    struct cork_slice  ps1;
    struct cork_slice  ps2;
    struct cork_slice  ps3;

    fail_if_error(cork_managed_buffer_slice(alloc, &ps1, pb, 0, 7, &err));
    fail_if_error(cork_managed_buffer_slice(alloc, &ps2, pb, 1, 1, &err));
    fail_if_error(cork_managed_buffer_slice(alloc, &ps3, pb, 4, 3, &err));

    cork_managed_buffer_unref(alloc, pb);
    cork_slice_finish(alloc, &ps1);
    cork_slice_finish(alloc, &ps2);
    /* cork_slice_finish(&ps3);   OH NO! */

    fail_if(flag,
            "Managed buffer free function was called unexpectedly.");

    /* free the slice here to quiet valgrind */
    cork_slice_finish(alloc, &ps3);
    cork_allocator_free(alloc);
}
END_TEST


/*-----------------------------------------------------------------------
 * Slice equality
 */

START_TEST(test_slice_equals_01)
{
    struct cork_alloc  *alloc = cork_allocator_new_debug();

    /*
     * Make a bunch of slices, finish them all, and then verify that
     * the free function got called.
     */

    static char  *BUF =
        "abcdefg";
    static size_t  LEN = 7;

    struct cork_managed_buffer  *pb;
    fail_if_error(pb = cork_managed_buffer_new_copy(alloc, BUF, LEN, &err));

    struct cork_slice  ps1;
    struct cork_slice  ps2;

    fail_if_error(cork_managed_buffer_slice_offset(alloc, &ps1, pb, 0, &err));
    fail_if_error(cork_managed_buffer_slice(alloc, &ps2, pb, 0, LEN, &err));

    fail_unless(cork_slice_equal(&ps1, &ps2),
                "Slices aren't equal");

    cork_managed_buffer_unref(alloc, pb);
    cork_slice_finish(alloc, &ps1);
    cork_slice_finish(alloc, &ps2);

    cork_allocator_free(alloc);
}
END_TEST


START_TEST(test_slice_equals_02)
{
    struct cork_alloc  *alloc = cork_allocator_new_debug();

    /*
     * Make a bunch of slices, finish them all, and then verify that
     * the free function got called.
     */

    static char  *BUF =
        "abcdefg";
    static size_t  LEN = 7;

    struct cork_managed_buffer  *pb;
    fail_if_error(pb = cork_managed_buffer_new_copy(alloc, BUF, LEN, &err));

    struct cork_slice  ps1;
    struct cork_slice  ps2;
    struct cork_slice  ps3;

    fail_if_error(cork_managed_buffer_slice(alloc, &ps1, pb, 3, 3, &err));

    fail_if_error(cork_managed_buffer_slice_offset(alloc, &ps2, pb, 1, &err));
    fail_if_error(cork_slice_copy(alloc, &ps3, &ps2, 2, 3, &err));
    fail_if_error(cork_slice_slice(alloc, &ps2, 2, 3, &err));

    fail_unless(cork_slice_equal(&ps1, &ps2),
                "Slices aren't equal");
    fail_unless(cork_slice_equal(&ps1, &ps3),
                "Slices aren't equal");

    cork_managed_buffer_unref(alloc, pb);
    cork_slice_finish(alloc, &ps1);
    cork_slice_finish(alloc, &ps2);
    cork_slice_finish(alloc, &ps3);

    cork_allocator_free(alloc);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
test_suite()
{
    Suite  *s = suite_create("managed-buffer");

    TCase  *tc_buffer_refcount = tcase_create("managed-buffer-refcount");
    tcase_add_test(tc_buffer_refcount, test_managed_buffer_refcount);
    tcase_add_test(tc_buffer_refcount, test_managed_buffer_bad_refcount);
    suite_add_tcase(s, tc_buffer_refcount);

    TCase  *tc_slice = tcase_create("slice");
    tcase_add_test(tc_slice, test_slice);
    suite_add_tcase(s, tc_slice);

    TCase  *tc_slice_refcount = tcase_create("slice-refcount");
    tcase_add_test(tc_slice_refcount, test_slice_refcount);
    tcase_add_test(tc_slice_refcount, test_slice_bad_refcount);
    suite_add_tcase(s, tc_slice_refcount);

    TCase  *tc_slice_equality = tcase_create("slice-equality");
    tcase_add_test(tc_slice_equality, test_slice_equals_01);
    tcase_add_test(tc_slice_equality, test_slice_equals_02);
    suite_add_tcase(s, tc_slice_equality);

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
