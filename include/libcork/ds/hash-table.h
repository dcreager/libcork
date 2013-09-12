/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011-2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_DS_HASH_TABLE_H
#define LIBCORK_DS_HASH_TABLE_H

#include <libcork/core/api.h>
#include <libcork/core/hash.h>
#include <libcork/core/mempool.h>
#include <libcork/core/types.h>
#include <libcork/ds/dllist.h>


/*-----------------------------------------------------------------------
 * Hash tables
 */

typedef cork_hash
(*cork_hash_table_hasher)(const void *key);

typedef bool
(*cork_hash_table_comparator)(const void *key1, const void *key2);

struct cork_hash_table_entry {
    /* The hash of this entry's key. */
    cork_hash  hash;
    /* This entry's key */
    void  *key;
    /* This entry's value */
    void  *value;
    /* A link to the other entries in the same hash bucket. */
    struct cork_dllist_item  siblings;
};


struct cork_hash_table {
    /* The current array of bins in the hash table. */
    struct cork_dllist  *bins;
    /* The number of bins in the hash table. */
    size_t  bin_count;
    /* The number of entries in the hash table. */
    size_t  entry_count;
    /* A hashing function. */
    cork_hash_table_hasher  hasher;
    /* A comparator function. */
    cork_hash_table_comparator  comparator;
    /* A memory pool for the hash table entries */
    struct cork_mempool  *entry_mempool;
};


CORK_API void
cork_hash_table_init(struct cork_hash_table *table,
                     size_t initial_size,
                     cork_hash_table_hasher hasher,
                     cork_hash_table_comparator comparator);

CORK_API struct cork_hash_table *
cork_hash_table_new(size_t initial_size,
                    cork_hash_table_hasher hasher,
                    cork_hash_table_comparator comparator);

CORK_API void
cork_hash_table_done(struct cork_hash_table *table);

CORK_API void
cork_hash_table_free(struct cork_hash_table *table);


CORK_API void
cork_hash_table_clear(struct cork_hash_table *table);


CORK_API void
cork_hash_table_ensure_size(struct cork_hash_table *table,
                            size_t desired_count);

#define cork_hash_table_size(table) ((table)->entry_count)


CORK_API void *
cork_hash_table_get(const struct cork_hash_table *table, const void *key);

CORK_API struct cork_hash_table_entry *
cork_hash_table_get_entry(const struct cork_hash_table *table,
                          const void *key);

CORK_API struct cork_hash_table_entry *
cork_hash_table_get_or_create(struct cork_hash_table *table,
                              void *key, bool *is_new);

CORK_API void
cork_hash_table_put(struct cork_hash_table *table,
                    void *key, void *value, bool *is_new,
                    void **old_key, void **old_value);

CORK_API void
cork_hash_table_delete_entry(struct cork_hash_table *table,
                             struct cork_hash_table_entry *entry);

CORK_API bool
cork_hash_table_delete(struct cork_hash_table *table, const void *key,
                       void **deleted_key, void **deleted_value);


enum cork_hash_table_map_result {
    /* Abort the current @ref cork_hash_table_map operation. */
    CORK_HASH_TABLE_MAP_ABORT = 0,
    /* Continue on to the next entry in the hash table. */
    CORK_HASH_TABLE_MAP_CONTINUE = 1,
    /* Delete the entry that was just processed, and then continue on to
     * the next entry in the hash table. */
    CORK_HASH_TABLE_MAP_DELETE = 2
};

typedef enum cork_hash_table_map_result
(*cork_hash_table_mapper)(struct cork_hash_table_entry *entry,
                          void *user_data);

CORK_API void
cork_hash_table_map(struct cork_hash_table *table,
                    cork_hash_table_mapper mapper, void *user_data);


struct cork_hash_table_iterator {
    struct cork_hash_table  *table;
    /* The index of the current bin */
    size_t  bin_index;
    /* The current element within the bin */
    struct cork_dllist_item  *curr;
};

CORK_API void
cork_hash_table_iterator_init(struct cork_hash_table *table,
                              struct cork_hash_table_iterator *iterator);

CORK_API struct cork_hash_table_entry *
cork_hash_table_iterator_next(struct cork_hash_table_iterator *iterator);


/*-----------------------------------------------------------------------
 * Built-in key types
 */

CORK_API void
cork_string_hash_table_init(struct cork_hash_table *table, size_t initial_size);

CORK_API struct cork_hash_table *
cork_string_hash_table_new(size_t initial_size);

CORK_API void
cork_pointer_hash_table_init(struct cork_hash_table *table,
                              size_t initial_size);

CORK_API struct cork_hash_table *
cork_pointer_hash_table_new(size_t initial_size);


#endif /* LIBCORK_DS_HASH_TABLE_H */
