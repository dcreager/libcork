/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011-2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>
#include <string.h>

#include "libcork/core/hash.h"
#include "libcork/core/mempool.h"
#include "libcork/core/types.h"
#include "libcork/ds/dllist.h"
#include "libcork/ds/hash-table.h"
#include "libcork/helpers/errors.h"

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


/* The default initial number of bins to allocate in a new table. */
#define CORK_HASH_TABLE_DEFAULT_INITIAL_SIZE  8

/* The default number of entries per bin to allow before increasing the
 * number of bins. */
#define CORK_HASH_TABLE_MAX_DENSITY  5

/* A table of prime numbers, each greater than a power of two.  @details
 * @$ 2^n + a, 2 <= n <= 30 @$. */
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

/* Return a prime number bin count that's at least as big as the given
 * requested size. */
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

/* Allocates a new bins array in a hash table.  We overwrite the old
 * array, so make sure to stash it away somewhere safe first. */
static void
cork_hash_table_allocate_bins(struct cork_hash_table *table,
                              size_t desired_count)
{
    size_t  i;

    table->bin_count = cork_hash_table_new_size(desired_count);
    DEBUG("      Allocating %zu bins", table->bin_count);
    table->bins = cork_calloc(table->bin_count, sizeof(struct cork_dllist));
    for (i = 0; i < table->bin_count; i++) {
        cork_dllist_init(&table->bins[i]);
    }
}


void
cork_hash_table_init(struct cork_hash_table *table,
                     size_t initial_size,
                     cork_hash_table_hasher hasher,
                     cork_hash_table_comparator comparator)
{
    table->bins = NULL;
    table->bin_count = 0;
    table->entry_count = 0;
    table->hasher = hasher;
    table->comparator = comparator;
    cork_mempool_init(&table->entry_mempool, struct cork_hash_table_entry);
}


struct cork_hash_table *
cork_hash_table_new(size_t initial_size,
                    cork_hash_table_hasher hasher,
                    cork_hash_table_comparator comparator)
{
    struct cork_hash_table  *table = cork_new(struct cork_hash_table);
    cork_hash_table_init(table, initial_size, hasher, comparator);
    return table;
}


void
cork_hash_table_clear(struct cork_hash_table *table)
{
    DEBUG("(clear) Removing all entries");

    size_t  i;
    for (i = 0; i < table->bin_count; i++) {
        DEBUG("  Bin %zu", i);
        struct cork_dllist  *bin = &table->bins[i];
        struct cork_dllist_item  *curr = bin->head.next;
        while (curr != &bin->head) {
            struct cork_hash_table_entry  *entry =
                cork_container_of(curr, struct cork_hash_table_entry, siblings);
            struct cork_dllist_item  *next = curr->next;

            DEBUG("    Freeing entry %p", entry);
            cork_mempool_free(&table->entry_mempool, entry);

            curr = next;
        }
        cork_dllist_init(&table->bins[i]);
    }

    table->entry_count = 0;
}


void
cork_hash_table_done(struct cork_hash_table *table)
{
    cork_hash_table_clear(table);
    cork_mempool_done(&table->entry_mempool);
    free(table->bins);
}


void
cork_hash_table_free(struct cork_hash_table *table)
{
    cork_hash_table_done(table);
    free(table);
}


void
cork_hash_table_ensure_size(struct cork_hash_table *table,
                            size_t desired_count)
{
    if (desired_count > table->bin_count) {
        struct cork_dllist  *old_bins = table->bins;
        size_t  old_bin_count = table->bin_count;

        cork_hash_table_allocate_bins(table, desired_count);

        if (old_bins != NULL) {
            size_t  i;
            for (i = 0; i < old_bin_count; i++) {
                struct cork_dllist  *bin = &old_bins[i];
                struct cork_dllist_item  *curr = bin->head.next;
                while (curr != &bin->head) {
                    struct cork_hash_table_entry  *entry =
                        cork_container_of
                        (curr, struct cork_hash_table_entry, siblings);
                    struct cork_dllist_item  *next = curr->next;

                    size_t  bin_index = entry->hash % table->bin_count;
                    DEBUG("      Rehashing %p from bin %zu to bin %zu",
                          entry, i, bin_index);
                    cork_dllist_add(&table->bins[bin_index], curr);

                    curr = next;
                }
            }

            free(old_bins);
        }
    }
}


static void
cork_hash_table_rehash(struct cork_hash_table *table)
{
    DEBUG("    Reaching maximum density; rehashing");
    cork_hash_table_ensure_size(table, table->bin_count + 1);
}


struct cork_hash_table_entry *
cork_hash_table_get_entry(const struct cork_hash_table *table, const void *key)
{
    cork_hash  hash_value = table->hasher(key);

    if (table->bin_count == 0) {
        DEBUG("(get) Empty table when searching for key %p (hash 0x%lx)",
              key, (unsigned long) hash_value);
        return NULL;
    }

    size_t  bin_index = hash_value % table->bin_count;
    DEBUG("(get) Searching for key %p (hash 0x%lx, bin %zu)",
          key, (unsigned long) hash_value, bin_index);

    struct cork_dllist  *bin = &table->bins[bin_index];
    struct cork_dllist_item  *curr = bin->head.next;
    while (curr != &bin->head) {
        struct cork_hash_table_entry  *entry =
            cork_container_of(curr, struct cork_hash_table_entry, siblings);

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
cork_hash_table_get(const struct cork_hash_table *table, const void *key)
{
    struct cork_hash_table_entry  *entry =
        cork_hash_table_get_entry(table, key);
    if (entry == NULL) {
        return NULL;
    } else {
        DEBUG("  Extracting value pointer %p", entry->value);
        return entry->value;
    }
}


struct cork_hash_table_entry *
cork_hash_table_get_or_create(struct cork_hash_table *table,
                              void *key, bool *is_new)
{
    cork_hash  hash_value = table->hasher(key);
    size_t  bin_index;

    if (table->bin_count > 0) {
        bin_index = hash_value % table->bin_count;
        DEBUG("(get_or_create) Searching for key %p (hash 0x%lx, bin %zu)",
              key, (unsigned long) hash_value, bin_index);

        struct cork_dllist  *bin = &table->bins[bin_index];
        struct cork_dllist_item  *curr = bin->head.next;
        while (curr != &bin->head) {
            struct cork_hash_table_entry  *entry =
                cork_container_of(curr, struct cork_hash_table_entry, siblings);

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

        if ((table->entry_count / table->bin_count) >
            CORK_HASH_TABLE_MAX_DENSITY) {
            cork_hash_table_rehash(table);
            bin_index = hash_value % table->bin_count;
        }
    } else {
        DEBUG("(get_or_create) Searching for key %p (hash 0x%lx)",
              key, (unsigned long) hash_value);
        DEBUG("  Empty table");
        cork_hash_table_rehash(table);
        bin_index = hash_value % table->bin_count;
    }

    DEBUG("    Allocating new entry");
    struct cork_hash_table_entry  *entry =
        cork_mempool_new(&table->entry_mempool);

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


void
cork_hash_table_put(struct cork_hash_table *table,
                    void *key, void *value, bool *is_new,
                    void **old_key, void **old_value)
{
    cork_hash  hash_value = table->hasher(key);
    size_t  bin_index;

    if (table->bin_count > 0) {
        bin_index = hash_value % table->bin_count;
        DEBUG("(put) Searching for key %p (hash 0x%lx, bin %zu)",
              key, (unsigned long) hash_value, bin_index);

        struct cork_dllist  *bin = &table->bins[bin_index];
        struct cork_dllist_item  *curr = bin->head.next;
        while (curr != &bin->head) {
            struct cork_hash_table_entry  *entry =
                cork_container_of(curr, struct cork_hash_table_entry, siblings);

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
                if (is_new != NULL) {
                    *is_new = false;
                }
                return;
            }

            curr = curr->next;
        }

        /* create a new entry */

        DEBUG("  Entry not found");

        if ((table->entry_count / table->bin_count) >
            CORK_HASH_TABLE_MAX_DENSITY) {
            cork_hash_table_rehash(table);
            bin_index = hash_value % table->bin_count;
        }
    } else {
        DEBUG("(put) Searching for key %p (hash 0x%lx)",
              key, (unsigned long) hash_value);
        DEBUG("  Empty table");
        cork_hash_table_rehash(table);
        bin_index = hash_value % table->bin_count;
    }

    DEBUG("    Allocating new entry");
    struct cork_hash_table_entry  *entry =
        cork_mempool_new(&table->entry_mempool);

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
    if (is_new != NULL) {
        *is_new = true;
    }
}


bool
cork_hash_table_delete(struct cork_hash_table *table, const void *key,
                       void **deleted_key, void **deleted_value)
{
    cork_hash  hash_value = table->hasher(key);

    if (table->bin_count == 0) {
        DEBUG("(delete) Empty table when searching for key %p (hash 0x%lx)",
              key, (unsigned long) hash_value);
        return false;
    }

    size_t  bin_index = hash_value % table->bin_count;
    DEBUG("(delete) Searching for key %p (hash 0x%lx, bin %zu)",
          key, (unsigned long) hash_value, bin_index);

    struct cork_dllist  *bin = &table->bins[bin_index];
    struct cork_dllist_item  *curr = bin->head.next;
    while (curr != &bin->head) {
        struct cork_hash_table_entry  *entry =
            cork_container_of(curr, struct cork_hash_table_entry, siblings);

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
            cork_mempool_free(&table->entry_mempool, entry);
            return true;
        }

        curr = curr->next;
    }

    DEBUG("  Entry not found");
    return false;
}


void
cork_hash_table_map(struct cork_hash_table *table,
                    cork_hash_table_mapper mapper, void *user_data)
{
    DEBUG("Mapping across hash table");

    size_t  i;
    for (i = 0; i < table->bin_count; i++) {
        DEBUG("  Bin %zu", i);
        struct cork_dllist  *bin = &table->bins[i];
        struct cork_dllist_item  *curr = bin->head.next;
        while (curr != &bin->head) {
            struct cork_hash_table_entry  *entry =
                cork_container_of(curr, struct cork_hash_table_entry, siblings);
            struct cork_dllist_item  *next = curr->next;

            DEBUG("    Applying function to entry %p", entry);
            enum cork_hash_table_map_result  result =
                mapper(entry, user_data);

            if (result == CORK_HASH_TABLE_MAP_ABORT) {
                return;
            } else if (result == CORK_HASH_TABLE_MAP_DELETE) {
                struct cork_hash_table_entry  *entry =
                    cork_container_of
                    (curr, struct cork_hash_table_entry, siblings);
                DEBUG("      Delete requested");
                cork_dllist_remove(curr);
                cork_mempool_free(&table->entry_mempool, entry);
                table->entry_count--;
            }

            curr = next;
        }
    }
}


void
cork_hash_table_iterator_init(struct cork_hash_table *table,
                              struct cork_hash_table_iterator *iterator)
{
    iterator->table = table;
    iterator->bin_index = 0;
    if (table->bin_count > 0) {
        DEBUG("Iterating through hash table");
        DEBUG("  Bin %zu", (size_t) 0);
        struct cork_dllist  *bin = &table->bins[0];
        iterator->curr = bin->head.next;
    } else {
        DEBUG("Iterating through hash table");
        DEBUG("  Empty table");
        iterator->curr = NULL;
    }
}


struct cork_hash_table_entry *
cork_hash_table_iterator_next(struct cork_hash_table_iterator *iterator)
{
    if (iterator->curr == NULL) {
        return NULL;
    }

    struct cork_hash_table  *table = iterator->table;
    struct cork_dllist  *bin = &table->bins[iterator->bin_index];
    while (iterator->curr == &bin->head) {
        /*
         * We've made it to the end of this bin.  Move to the next bin
         * and try it.  If we run out of bins, then there aren't any
         * more elements.
         */

        iterator->bin_index++;
        if (iterator->bin_index >= table->bin_count) {
            DEBUG("  Iteration finished");
            iterator->curr = NULL;
            return NULL;
        }

        DEBUG("  Bin %zu", iterator->bin_index);

        bin = &table->bins[iterator->bin_index];
        iterator->curr = bin->head.next;
    }

    struct cork_hash_table_entry  *result =
        cork_container_of(iterator->curr, struct cork_hash_table_entry, siblings);
    DEBUG("    Returning entry %p", result);
    iterator->curr = iterator->curr->next;
    return result;
}
