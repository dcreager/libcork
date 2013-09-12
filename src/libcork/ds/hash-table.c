/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011-2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license
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


/*-----------------------------------------------------------------------
 * Hash tables
 */

/* The default initial number of bins to allocate in a new table. */
#define CORK_HASH_TABLE_DEFAULT_INITIAL_SIZE  8

/* The default number of entries per bin to allow before increasing the
 * number of bins. */
#define CORK_HASH_TABLE_MAX_DENSITY  5

/* Return a power-of-2 bin count that's at least as big as the given requested
 * size. */
static inline size_t
cork_hash_table_new_size(size_t desired_count)
{
    size_t  v = desired_count;
    size_t  r = 1;
    while (v >>= 1) {
        r <<= 1;
    }
    if (r != desired_count) {
        r <<= 1;
    }
    return r;
}

#define bin_index(table, hash) \
    ((hash) & ((table)->bin_count - 1))

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
    table->entry_count = 0;
    table->hasher = hasher;
    table->comparator = comparator;
    table->entry_mempool = cork_mempool_new(struct cork_hash_table_entry);
    if (initial_size < CORK_HASH_TABLE_DEFAULT_INITIAL_SIZE) {
        initial_size = CORK_HASH_TABLE_DEFAULT_INITIAL_SIZE;
    }
    cork_hash_table_allocate_bins(table, initial_size);
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
            cork_mempool_free_object(table->entry_mempool, entry);

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
    cork_mempool_free(table->entry_mempool);
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

                    size_t  bin_index = bin_index(table, entry->hash);
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

    size_t  bin_index = bin_index(table, hash_value);
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
        bin_index = bin_index(table, hash_value);
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
            bin_index = bin_index(table, hash_value);
        }
    } else {
        DEBUG("(get_or_create) Searching for key %p (hash 0x%lx)",
              key, (unsigned long) hash_value);
        DEBUG("  Empty table");
        cork_hash_table_rehash(table);
        bin_index = bin_index(table, hash_value);
    }

    DEBUG("    Allocating new entry");
    struct cork_hash_table_entry  *entry =
        cork_mempool_new_object(table->entry_mempool);

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
        bin_index = bin_index(table, hash_value);
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
            bin_index = bin_index(table, hash_value);
        }
    } else {
        DEBUG("(put) Searching for key %p (hash 0x%lx)",
              key, (unsigned long) hash_value);
        DEBUG("  Empty table");
        cork_hash_table_rehash(table);
        bin_index = bin_index(table, hash_value);
    }

    DEBUG("    Allocating new entry");
    struct cork_hash_table_entry  *entry =
        cork_mempool_new_object(table->entry_mempool);

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


void
cork_hash_table_delete_entry(struct cork_hash_table *table,
                             struct cork_hash_table_entry *entry)
{
    cork_dllist_remove(&entry->siblings);
    table->entry_count--;
    cork_mempool_free_object(table->entry_mempool, entry);
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

    size_t  bin_index = bin_index(table, hash_value);
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
            cork_mempool_free_object(table->entry_mempool, entry);
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
                cork_mempool_free_object(table->entry_mempool, entry);
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


/*-----------------------------------------------------------------------
 * Built-in key types
 */

static cork_hash
string_hasher(const void *vk)
{
    const char  *k = vk;
    size_t  len = strlen(k);
    return cork_hash_buffer(0, k, len);
}

static bool
string_comparator(const void *vk1, const void *vk2)
{
    const char  *k1 = vk1;
    const char  *k2 = vk2;
    return strcmp(k1, k2) == 0;
}

void
cork_string_hash_table_init(struct cork_hash_table *table, size_t initial_size)
{
    cork_hash_table_init(table, initial_size, string_hasher, string_comparator);
}

struct cork_hash_table *
cork_string_hash_table_new(size_t initial_size)
{
    return cork_hash_table_new(initial_size, string_hasher, string_comparator);
}

static cork_hash
pointer_hasher(const void *vk)
{
    return (cork_hash) (uintptr_t) vk;
}

static bool
pointer_comparator(const void *vk1, const void *vk2)
{
    return vk1 == vk2;
}

void
cork_pointer_hash_table_init(struct cork_hash_table *table,
                              size_t initial_size)
{
    cork_hash_table_init
        (table, initial_size, pointer_hasher, pointer_comparator);
}

struct cork_hash_table *
cork_pointer_hash_table_new(size_t initial_size)
{
    return cork_hash_table_new
        (initial_size, pointer_hasher, pointer_comparator);
}
