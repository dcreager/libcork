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
#include "libcork/core/hash.h"
#include "libcork/core/types.h"
#include "libcork/ds/hash-table.h"

/*-----------------------------------------------------------------------
 * Hash tables
 */

static bool
uint64_equals(const void *va, const void *vb)
{
    const uint64_t  *a = va;
    const uint64_t  *b = vb;
#if 0
    printf("Testing %p (%" PRIu64 ") and %p (%" PRIu64 ")\n",
           a, *a, b, *b);
#endif
    return *a == *b;
}

static cork_hash_t
uint64_hash(const void *velement)
{
    const uint64_t  *element = velement;
#if 0
    printf("Hashing %p (%" PRIu64 ")\n", element, *element);
#endif
    return (cork_hash_t) *element;
}

static cork_hash_table_map_result_t
uint64_sum(cork_hash_table_entry_t *entry, void *vsum)
{
    uint64_t  *sum = vsum;
    uint64_t  *value = entry->value;
    *sum += *value;
    return CORK_HASH_TABLE_MAP_CONTINUE;
}

static cork_hash_table_map_result_t
uint64_map_free(cork_hash_table_entry_t *entry, void *valloc)
{
    cork_allocator_t  *alloc = valloc;
    cork_delete(alloc, uint64_t, entry->key);
    cork_delete(alloc, uint64_t, entry->value);
    return CORK_HASH_TABLE_MAP_DELETE;
}

START_TEST(test_hash_table)
{
    cork_allocator_t  *alloc = cork_allocator_new_debug();

    cork_hash_table_t  *table =
        cork_hash_table_new(alloc, 0, uint64_hash, uint64_equals);
    fail_if(table == NULL,
            "Couldn't allocate new hash table");

    uint64_t  key, *key_ptr, *old_key;
    uint64_t  *value_ptr, *old_value;
    bool  is_new;
    cork_hash_table_entry_t  *entry;

    fail_unless(cork_hash_table_size(table) == 0,
                "Hash table should start empty");

    key = 0;
    fail_unless(cork_hash_table_get(table, &key) == NULL,
                "Shouldn't get value pointer from empty hash table");

    key_ptr = cork_new(alloc, uint64_t);
    *key_ptr = 0;
    value_ptr = cork_new(alloc, uint64_t);
    *value_ptr = 32;
    fail_unless(cork_hash_table_put(table, key_ptr, value_ptr,
                                    (void **) &old_key, (void **) &old_value),
                "Couldn't append {0=>32} to hash table");

    fail_unless(old_key == NULL,
                "Unexpected previous key");
    fail_unless(old_value == NULL,
                "Unexpected previous value");

    fail_unless(cork_hash_table_size(table) == 1,
                "Unexpected size after adding {0->32}");

    entry = cork_hash_table_get_or_create(table, &key, &is_new);
    fail_if(is_new,
            "Shouldn't create new {0=>X} entry");
    value_ptr = entry->value;
    fail_unless(*value_ptr == 32,
                "Unexpected value for {0=>X} entry");

    fail_unless(cork_hash_table_size(table) == 1,
                "Unexpected size after retrieving {0->32}");

    key = 1;
    entry = cork_hash_table_get_or_create(table, &key, &is_new);
    fail_unless(is_new,
                "Should create new {1=>X} entry");
    key_ptr = cork_new(alloc, uint64_t);
    *key_ptr = key;
    entry->key = key_ptr;
    value_ptr = cork_new(alloc, uint64_t);
    *value_ptr = 2;
    entry->value = value_ptr;

    fail_unless(cork_hash_table_size(table) == 2,
                "Unexpected size after adding {1=>2}");

    uint64_t  sum = 0;
    cork_hash_table_map(table, uint64_sum, &sum);
    fail_unless(sum == 34,
                "Unexpected sum, got %lu, expected %lu",
                (unsigned long) sum, (unsigned long) 34);

    key = 0;
    fail_unless(cork_hash_table_delete
                (table, &key, (void **) &old_key, (void **) &old_value),
                "Couldn't delete {0=>32}");
    cork_delete(alloc, uint64_t, old_key);
    cork_delete(alloc, uint64_t, old_value);

    fail_unless(cork_hash_table_size(table) == 1,
                "Unexpected size after deleting entry");

    key = 3;
    fail_if(cork_hash_table_delete(table, &key, NULL, NULL),
            "Shouldn't be able to delete nonexistent {3=>X}");

    key = 1;
    fail_unless(cork_hash_table_delete
                (table, &key, (void **) &old_key, (void **) &old_value),
                "Couldn't delete {1=>2}");
    cork_delete(alloc, uint64_t, old_key);
    cork_delete(alloc, uint64_t, old_value);

    fail_unless(cork_hash_table_size(table) == 0,
                "Unexpected size after deleting last entry");

    /*
     * Add the entries back so that we can try deleting them using
     * cork_hash_table_map.
     */

    key_ptr = cork_new(alloc, uint64_t);
    *key_ptr = 0;
    value_ptr = cork_new(alloc, uint64_t);
    *value_ptr = 32;
    fail_unless(cork_hash_table_put(table, key_ptr, value_ptr,
                                    (void **) &old_key, (void **) &old_value),
                "Couldn't append {0=>32} to hash table");

    key_ptr = cork_new(alloc, uint64_t);
    *key_ptr = 1;
    value_ptr = cork_new(alloc, uint64_t);
    *value_ptr = 2;
    fail_unless(cork_hash_table_put(table, key_ptr, value_ptr,
                                    (void **) &old_key, (void **) &old_value),
                "Couldn't append {1=>2} to hash table");

    cork_hash_table_map(table, uint64_map_free, alloc);
    fail_unless(cork_hash_table_size(table) == 0,
                "Unexpected size after deleting entries using map");

    /*
     * And we're done, so let's free everything.
     */

    cork_hash_table_free(table);
    cork_allocator_free(alloc);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
test_suite()
{
    Suite  *s = suite_create("hash_table");

    TCase  *tc_ds = tcase_create("hash_table");
    tcase_add_test(tc_ds, test_hash_table);
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
