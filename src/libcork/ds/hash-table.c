/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <string.h>

#include <libcork/core/allocator.h>
#include <libcork/core/hash.h>
#include <libcork/core/types.h>
#include <libcork/ds/dllist.h>
#include <libcork/ds/hash-table.h>

#ifndef CORK_HASH_TABLE_DEBUG
#define CORK_HASH_TABLE_DEBUG 0
#endif

#if CORK_HASH_TABLE_DEBUG
#include <stdio.h>
#define DEBUG(...) \
    do { \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n"); \
    } while (0)
#else
#define DEBUG(...) /* nothing */
#endif


/**
 * @brief The default initial number of bins to allocate in a new table.
 */

#define CORK_HASH_TABLE_DEFAULT_INITIAL_SIZE  11

/**
 * @brief The default number of entries per bin to allow before
 * increasing the number of bins.
 */

#define CORK_HASH_TABLE_MAX_DENSITY  5

/**
 * @brief A table of prime numbers, each greater than a power of two.
 * @details @$ 2^n + a, 2 <= n <= 30 @$.
 */

static size_t  CORK_HASH_TABLE_PRIMES[] =
{
    8 + 3,
    16 + 3,
    32 + 5,
    64 + 3,
    128 + 3,
    256 + 27,
    512 + 9,
    1024 + 9,
    2048 + 5,
    4096 + 3,
    8192 + 27,
    16384 + 43,
    32768 + 3,
    65536 + 45,
    131072 + 29,
    262144 + 3,
    524288 + 21,
    1048576 + 7,
    2097152 + 17,
    4194304 + 15,
    8388608 + 9,
    16777216 + 43,
    33554432 + 35,
    67108864 + 15,
    134217728 + 29,
    268435456 + 3,
    536870912 + 11,
    1073741824 + 85,
    0
};

#define CORK_HASH_TABLE_PRIME_COUNT \
    (sizeof(CORK_HASH_TABLE_PRIMES) / sizeof(CORK_HASH_TABLE_PRIMES[0]))

/**
 * @brief Return a prime number bin count that's at least as big as the
 * given requested size.
 */

static size_t
cork_hash_table_new_size(size_t desired_count)
{
    size_t  new_size = CORK_HASH_TABLE_DEFAULT_INITIAL_SIZE;
    unsigned int  i;
    for (i = 0; i < CORK_HASH_TABLE_PRIME_COUNT; i++, new_size <<= 1) {
        if (new_size > desired_count) {
            return CORK_HASH_TABLE_PRIMES[i];
        }
    }

    return 0;
}

/**
 * @brief Allocates a new bins array in a hash table.
 *
 * We overwrite the old array, so make sure to stash it away somewhere
 * safe first.
 */

static bool
cork_hash_table_allocate_bins(cork_hash_table_t *table,
                              size_t desired_count)
{
    size_t  allocated_size;
    size_t  i;

    table->bin_count = cork_hash_table_new_size(desired_count);
    DEBUG("      Allocating %zu bins", table->bin_count);
    allocated_size = table->bin_count * sizeof(cork_dllist_t);
    table->bins = cork_malloc(table->alloc, allocated_size);

    if (table->bins == NULL) {
        return false;
    }

    for (i = 0; i < table->bin_count; i++) {
        cork_dllist_init(&table->bins[i]);
    }

    return true;
}


bool
cork_hash_table_init(cork_allocator_t *alloc,
                     cork_hash_table_t *table,
                     size_t initial_size,
                     cork_hash_table_hasher_t hasher,
                     cork_hash_table_comparator_t comparator)
{
    table->alloc = alloc;
    table->entry_count = 0;
    table->hasher = hasher;
    table->comparator = comparator;
    return cork_hash_table_allocate_bins(table, initial_size);
}


cork_hash_table_t *
cork_hash_table_new(cork_allocator_t *alloc,
                    size_t initial_size,
                    cork_hash_table_hasher_t hasher,
                    cork_hash_table_comparator_t comparator)
{
    cork_hash_table_t  *table = cork_new(alloc, cork_hash_table_t);
    if (table == NULL) {
        return NULL;
    }

    cork_hash_table_init(alloc, table, initial_size, hasher, comparator);
    return table;
}


void
cork_hash_table_clear(cork_hash_table_t *table)
{
    DEBUG("(clear) Removing all entries");

    size_t  i;
    for (i = 0; i < table->bin_count; i++) {
        DEBUG("  Bin %zu", i);
        cork_dllist_t  *bin = &table->bins[i];
        cork_dllist_item_t  *curr = bin->head.next;
        while (curr != &bin->head) {
            cork_hash_table_entry_t  *entry =
                cork_container_of(curr, cork_hash_table_entry_t, siblings);
            cork_dllist_item_t  *next = curr->next;

            DEBUG("    Freeing entry %p", entry);
            cork_delete(table->alloc, cork_hash_table_entry_t, entry);

            curr = next;
        }
        cork_dllist_init(&table->bins[i]);
    }

    table->entry_count = 0;
}


void
cork_hash_table_done(cork_hash_table_t *table)
{
    cork_hash_table_clear(table);
    cork_free(table->alloc, table->bins,
              table->bin_count * sizeof(cork_dllist_t));
}


void
cork_hash_table_free(cork_hash_table_t *table)
{
    cork_hash_table_done(table);
    cork_delete(table->alloc, cork_hash_table_t, table);
}


bool
cork_hash_table_ensure_size(cork_hash_table_t *table,
                            size_t desired_count)
{
    if (desired_count > table->bin_count) {
        cork_dllist_t  *old_bins = table->bins;
        size_t  old_bin_count = table->bin_count;

        if (!cork_hash_table_allocate_bins(table, desired_count)) {
            return false;
        }

        size_t  i;
        for (i = 0; i < old_bin_count; i++) {
            cork_dllist_t  *bin = &old_bins[i];
            cork_dllist_item_t  *curr = bin->head.next;
            while (curr != &bin->head) {
                cork_hash_table_entry_t  *entry =
                    cork_container_of(curr, cork_hash_table_entry_t, siblings);
                cork_dllist_item_t  *next = curr->next;

                size_t  bin_index = entry->hash % table->bin_count;
                DEBUG("      Rehashing %p from bin %zu to bin %zu",
                      entry, i, bin_index);
                cork_dllist_add(&table->bins[bin_index], curr);

                curr = next;
            }
        }

        cork_free(table->alloc, old_bins,
                  old_bin_count * sizeof(cork_dllist_t));
    }

    return true;
}


static bool
cork_hash_table_rehash(cork_hash_table_t *table)
{
    DEBUG("    Reaching maximum density; rehashing");
    return cork_hash_table_ensure_size(table, table->bin_count + 1);
}


cork_hash_table_entry_t *
cork_hash_table_get_entry(const cork_hash_table_t *table,
                          const void *key)
{
    cork_hash_t  hash_value = table->hasher(key);
    size_t  bin_index = hash_value % table->bin_count;
    DEBUG("(get) Searching for key %p (hash 0x%lx, bin %zu)",
          key, (unsigned long) hash_value, bin_index);

    cork_dllist_t  *bin = &table->bins[bin_index];
    cork_dllist_item_t  *curr = bin->head.next;
    while (curr != &bin->head) {
        cork_hash_table_entry_t  *entry =
            cork_container_of(curr, cork_hash_table_entry_t, siblings);

        DEBUG("  Checking entry %p", entry);
        if (table->comparator(key, entry->key)) {
            DEBUG("  Match");
            return entry;
        }

        curr = curr->next;
    }

    DEBUG("  Entry not found");
    return NULL;
}

void *
cork_hash_table_get(const cork_hash_table_t *table,
                    const void *key)
{
    cork_hash_table_entry_t  *entry = cork_hash_table_get_entry(table, key);
    if (entry == NULL) {
        return NULL;
    } else {
        DEBUG("  Extracting value pointer %p", entry->value);
        return entry->value;
    }
}


cork_hash_table_entry_t *
cork_hash_table_get_or_create(cork_hash_table_t *table,
                              void *key, bool *is_new)
{
    cork_hash_t  hash_value = table->hasher(key);
    size_t  bin_index = hash_value % table->bin_count;
    DEBUG("(get_or_create) Searching for key %p (hash 0x%lx, bin %zu)",
          key, (unsigned long) hash_value, bin_index);

    cork_dllist_t  *bin = &table->bins[bin_index];
    cork_dllist_item_t  *curr = bin->head.next;
    while (curr != &bin->head) {
        cork_hash_table_entry_t  *entry =
            cork_container_of(curr, cork_hash_table_entry_t, siblings);

        DEBUG("  Checking entry %p", entry);
        if (table->comparator(key, entry->key)) {
            DEBUG("    Match");
            DEBUG("    Returning value pointer %p", entry->value);
            *is_new = false;
            return entry;
        }

        curr = curr->next;
    }

    /* create a new entry */

    DEBUG("  Entry not found");

    if ((table->entry_count / table->bin_count) > CORK_HASH_TABLE_MAX_DENSITY) {
        if (!cork_hash_table_rehash(table)) {
            return NULL;
        }
        bin_index = hash_value % table->bin_count;
    }

    DEBUG("    Allocating new entry");
    cork_hash_table_entry_t  *entry =
        cork_new(table->alloc, cork_hash_table_entry_t);
    if (entry == NULL) {
        return NULL;
    }

    DEBUG("    Created new entry %p", entry);
    entry->hash = hash_value;
    entry->key = key;
    entry->value = NULL;

    DEBUG("    Adding entry into bin %zu", bin_index);
    cork_dllist_add(&table->bins[bin_index], &entry->siblings);

    table->entry_count++;
    *is_new = true;
    return entry;
}


bool
cork_hash_table_put(cork_hash_table_t *table,
                    void *key, void *value,
                    void **old_key, void **old_value)
{
    cork_hash_t  hash_value = table->hasher(key);
    size_t  bin_index = hash_value % table->bin_count;
    DEBUG("(put) Searching for key %p (hash 0x%lx, bin %zu)",
          key, (unsigned long) hash_value, bin_index);

    cork_dllist_t  *bin = &table->bins[bin_index];
    cork_dllist_item_t  *curr = bin->head.next;
    while (curr != &bin->head) {
        cork_hash_table_entry_t  *entry =
            cork_container_of(curr, cork_hash_table_entry_t, siblings);

        DEBUG("  Checking entry %p", entry);
        if (table->comparator(key, entry->key)) {
            DEBUG("    Found existing entry; overwriting");
            DEBUG("    Returning old key %p", entry->key);
            if (old_key != NULL) {
                *old_key = entry->key;
            }
            DEBUG("    Returning old value %p", entry->value);
            if (old_value != NULL) {
                *old_value = entry->value;
            }
            DEBUG("    Copying key %p into entry", key);
            entry->key = key;
            DEBUG("    Copying value %p into entry", value);
            entry->value = value;
            return true;
        }

        curr = curr->next;
    }

    /* create a new entry */

    DEBUG("  Entry not found");

    if ((table->entry_count / table->bin_count) >
        CORK_HASH_TABLE_MAX_DENSITY) {
        if (!cork_hash_table_rehash(table)) {
            return false;
        }
        bin_index = hash_value % table->bin_count;
    }

    DEBUG("    Allocating new entry");
    cork_hash_table_entry_t  *entry =
        cork_new(table->alloc, cork_hash_table_entry_t);
    if (entry == NULL) {
        return false;
    }

    DEBUG("    Created new entry %p", entry);
    entry->hash = hash_value;
    entry->key = key;
    entry->value = value;

    DEBUG("    Adding entry into bin %zu", bin_index);
    cork_dllist_add(&table->bins[bin_index], &entry->siblings);

    table->entry_count++;
    if (old_key != NULL) {
        *old_key = NULL;
    }
    if (old_value != NULL) {
        *old_value = NULL;
    }
    return true;
}


bool
cork_hash_table_delete(cork_hash_table_t *table, const void *key,
                       void **deleted_key, void **deleted_value)
{
    cork_hash_t  hash_value = table->hasher(key);
    size_t  bin_index = hash_value % table->bin_count;
    DEBUG("(delete) Searching for key %p (hash 0x%lx, bin %zu)",
          key, (unsigned long) hash_value, bin_index);


    cork_dllist_t  *bin = &table->bins[bin_index];
    cork_dllist_item_t  *curr = bin->head.next;
    while (curr != &bin->head) {
        cork_hash_table_entry_t  *entry =
            cork_container_of(curr, cork_hash_table_entry_t, siblings);

        DEBUG("  Checking entry %p", entry);
        if (table->comparator(key, entry->key)) {
            DEBUG("    Match");
            if (deleted_key != NULL) {
                *deleted_key = entry->key;
            }
            if (deleted_value != NULL) {
                *deleted_value = entry->value;
            }

            DEBUG("    Removing entry from hash bin %zu", bin_index);
            cork_dllist_remove(curr);
            table->entry_count--;

            DEBUG("    Freeing entry %p", entry);
            cork_delete(table->alloc, cork_hash_table_entry_t, entry);
            return true;
        }

        curr = curr->next;
    }

    DEBUG("  Entry not found");
    return false;
}


void
cork_hash_table_map(cork_hash_table_t *table,
                    cork_hash_table_mapper_t mapper, void *user_data)
{
    DEBUG("Mapping across hash table");

    size_t  i;
    for (i = 0; i < table->bin_count; i++) {
        DEBUG("  Bin %zu", i);
        cork_dllist_t  *bin = &table->bins[i];
        cork_dllist_item_t  *curr = bin->head.next;
        while (curr != &bin->head) {
            cork_hash_table_entry_t  *entry =
                cork_container_of(curr, cork_hash_table_entry_t, siblings);
            cork_dllist_item_t  *next = curr->next;

            DEBUG("    Applying function to entry %p", entry);
            cork_hash_table_map_result_t  result = mapper(entry, user_data);

            if (result == CORK_HASH_TABLE_MAP_ABORT) {
                return;
            } else if (result == CORK_HASH_TABLE_MAP_DELETE) {
                DEBUG("      Delete requested");
                cork_dllist_remove(curr);
                table->entry_count--;
            }

            curr = next;
        }
    }
}
