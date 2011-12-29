.. _hash-table:

***********
Hash tables
***********

.. highlight:: c

::

  #include <libcork/ds.h>

This section defines a hash table class.  Our hash table implementation
is based on the public domain hash table package written in the late
1980's by Peter Moore at UC Berkeley.

The keys and values of a libcork hash table are both represented by
``void *`` pointers.  You can also store integer keys or values, as long
as you use the :c:type:`intptr_t` or :c:type:`uintptr_t` integral types.
(These are the only integer types guaranteed by the C99 standard to fit
within the space used by a ``void *``.)  The keys of the hash table can
be any arbitrary type; you must provide two functions that control how
key pointers are used to identify entries in the table: the *hasher*
(:c:type:`cork_hash_table_hasher`) and the *comparator*
(:c:type:`cork_hash_table_comparator`).  It's your responsibility to
ensure that these two functions are consistent with each other — i.e.,
if two keys are equal according to your comparator, they must also map
to the same hash value.  (The inverse doesn't need to be true; it's fine
for two keys to have the same hash value but not be equal.)

The hash table does not take ownership of the keys or values in the
table.  It is your responsibility, as a hash table user, to ensure that
the keys and values in a hash table are disposed of correctly *before*
you dispose of the hash table.

.. type:: struct cork_hash_table

   A hash table instance.  A common use case is to embed this type
   directly into another type that needs to use a hash table internally.
   All of the fields of this type should be considered opaque; you
   should only you the functions described below to query or update the
   hash table.

.. function:: int cork_hash_table_init(struct cork_alloc \*alloc, struct cork_hash_table \*table, size_t initial_size, cork_hash_table_hasher hasher, cork_hash_table_comparator comparator, struct cork_error \*err)
              struct cork_hash_table \*cork_hash_table_new(struct cork_alloc \*alloc, size_t initial_size, cork_hash_table_hasher hasher, cork_hash_table_comparator comparator, struct cork_error \*err)

   Initializes a new hash table instance.  The ``_init`` variant should
   be used to initialize an instance that you've allocated yourself.
   The ``_new`` variant will allocate a new instance from the heap,
   returning ``NULL`` if the allocation fails.

   If you know roughly how many entries you're going to add to the hash
   table, you can pass this in as the *initial_size* parameter.  If you
   don't know how many entries there will be, you can use ``0`` for this
   parameter instead.  You must also provide the *hasher* and
   *comparator* functions that will be used for the keys of this hash
   table.

.. type:: cork_hash (\*cork_hash_table_hasher)(const void \*key)

   Translates a key pointer into a :c:type:`cork_hash` hash value.

.. type:: bool (\*cork_hash_table_comparator)(const void \*key1, const void \*key2)

   Compares two key pointers for equality.


.. function:: void cork_hash_table_done(struct cork_alloc \*alloc, struct cork_hash_table \*table)
              void cork_hash_table_free(struct cork_alloc \*alloc, struct cork_hash_table \*table)

   Finalizes a hash table.  The ``_done`` variant should be used to
   finalize an instance that you allocated yourself.  The ``_free``
   variant should be used on instances that were allocated from the heap
   using :c:func:`cork_hash_table_new()`.

   Nothing special is done to any remaining keys or values in the table;
   if they need to be finalized, you should do that yourself before
   calling this function.


.. type:: struct cork_hash_table_entry

   The contents of an entry in a hash table.

   .. member:: void  \*key

      The key for this entry.  There won't be any other entries in the
      hash table with the same key, as determined by the comparator
      function that you provide.

   .. member:: void  \*value

      The value for this entry.  The entry's value is completely opaque
      to the hash table; we'll never need to compare or interrogate the
      values in the table.

   .. member:: cork_hash  hash

      The hash value for this entry's key.  This field is strictly
      read-only.


Adding and retrieving entries
-----------------------------

There are several functions that can be used to add or retrieve entries
from a hash table.  Each one has slightly different semantics; you
should read through them all before deciding which one to use for a
particular use case.

.. function:: void \*cork_hash_table_get(struct cork_alloc \*alloc, const struct cork_hash_table \*table, const void \*key)

   Retrieves the value in *table* with the given *key*.  We return
   ``NULL`` if there's no corresponding entry in the table.  This means
   that, using this function, you can't tell the difference between a
   missing entry, and an entry that's explicitly mapped to ``NULL``.  If
   you need to distinguish those cases, you should use
   :c:func:`cork_hash_table_get_entry()` instead.

.. function:: struct cork_hash_table_entry \*cork_hash_table_get_entry(struct cork_alloc \*alloc, const struct cork_hash_table \*table, const void \*key)

   Retrieves the entry in *table* with the given *key*.  We return
   ``NULL`` if there's no corresponding entry in the table.

   You are free to update the :c:member:`key
   <cork_hash_table_entry.key>` and :c:member:`value
   <cork_hash_table_entry.value>` fields of the entry.  However, you
   must ensure that any new key is considered “equal” to the old key,
   according to the hasher and comparator functions that you provided
   for this hash table.

.. function:: struct cork_hash_table_entry \*cork_hash_table_get_or_create(struct cork_alloc \*alloc, struct cork_hash_table \*table, void \*key, bool \*is_new, struct cork_error \*err)

   Retrieves the entry in *table* with the given *key*.  If there is no
   entry with the given key, it will be created.  (If we can't create
   the new entry, we'll return ``NULL``.)  We'll fill in the *is_new*
   output parameter to indicate whether the entry is new or not.

   If a new entry is created, its value will initially be ``NULL``, but
   you can update this as necessary.  You can also update the entry's
   key, though you must ensure that any new key is considered “equal” to
   the old key, according to the hasher and comparator functions that
   you provided for this hash table.  This is necessary, for instance,
   if the *key* parameter that we search for was allocated on the stack.
   We can't save this stack key into the hash table, since it will
   disapppear as soon as the calling function finishes.  Instead, you
   must create a new key on the heap, which can be saved into the entry.
   For efficiency, you'll only want to allocate this new heap-stored key
   if the entry is actually new, especially if there will be a lot
   successful lookups of existing keys.

.. function:: int cork_hash_table_put(struct cork_alloc \*alloc, struct cork_hash_table \*table, void \*key, void \*value, bool \*is_new, void \*\*old_key, void \*\*old_value, struct cork_error \*err)

   Add an entry to a hash table.  If there is already an entry with the
   given key, we will overwrite its key and value with the *key* and
   *value* parameters.  If the *is_new* parameter is non-\ ``NULL``,
   we'll fill it in to indicate whether the entry is new or already
   existed in the table.  If the *old_key* and/or *old_value* parameters
   are non-\ ``NULL``, we'll fill them in with the existing key and
   value.  This can be used, for instance, to finalize an overwritten
   key or value object.

.. function:: bool cork_hash_table_delete(struct cork_alloc \*alloc, struct cork_hash_table \*table, const void \*key, void \*\*deleted_key, void \*\*deleted_value)

   Removes the entry with the given *key* from *table*.  If there isn't
   any entry with the given key, we'll return ``false``.  If the
   *deleted_key* and/or *deleted_value* parameters are non-\ ``NULL``,
   we'll fill them in with the deleted key and value.  This can be used,
   for instance, to finalize the key or value object that was stored in
   the hash table entry.


Other operations
----------------

.. function:: size_t cork_hash_table_size(const struct cork_hash_table \*table)

   Returns the number of entries in a hash table.

.. function:: void cork_hash_table_clear(struct cork_alloc \*alloc, struct cork_hash_table \*table)

   Removes all of the entries in a hash table, without finalizing the
   hash table itself.

   Nothing special is done to any remaining keys or values in the table;
   if they need to be finalized, you should do that yourself before
   calling this function.

.. function:: int cork_hash_table_ensure_size(struct cork_alloc \*alloc, struct cork_hash_table \*table, size_t desired_count, struct cork_error \*err)

   Ensures that *table* has enough space to efficiently store a certain
   number of entries.  This can be used to reduce (or eliminate) the
   number of resizing operations needed to add a large number of entries
   to the table, when you know in advance roughly how many entries there
   will be.


Iterating through a hash table
------------------------------

There are two strategies you can use to access all of the entries in a
hash table: *mapping* and *iterating*.  With mapping, you write a
mapping function, which will be applied to each entry in the table.  (In
this case, libcork controls the loop that steps through each entry.)

.. function:: void cork_hash_table_map(struct cork_alloc \*alloc, struct cork_hash_table \*table, cork_hash_table_mapper mapper, void \*user_data)

   Applies the *mapper* function to each entry in a hash table.  The
   mapper function's :c:type:`cork_hash_table_map_result` return value
   can be used to influence the iteration.

.. type:: enum cork_hash_table_map_result (\*cork_hash_table_mapper)(struct cork_alloc \*alloc, struct cork_hash_table_entry \*entry, void \*user_data)

   A function that can be applied to each entry in a hash table.  The
   function's return value can be used to influence the iteration:

.. type:: enum cork_hash_table_map_result

   .. var:: CORK_HASH_TABLE_CONTINUE

      Continue the current :c:func:`cork_hash_table_map()` operation.
      If there are remaining elements, the next one will be passed into
      another call of the mapping function.

   .. var:: CORK_HASH_TABLE_ABORT

      Stop the current :c:func:`cork_hash_table_map()` operation.  No
      more entries will be processed after this one.

   .. var:: CORK_HASH_TABLE_DELETE

      Continue the current :c:func:`cork_hash_table_map()` operation,
      but first delete the entry that was just processed.  If there are
      remaining elements, the next one will be passed into another call
      of the mapping function.

For instance, you can manually calculate the number of entries in a hash
table as follows (assuming you didn't want to use the built-in
:c:func:`cork_hash_table_size()` function, of course)::

  static enum cork_hash_table_map_result
  count_entries(struct cork_alloc *alloc,
                struct cork_hash_table_entry *entry, void *ud)
  {
      size_t  *count = ud;
      (*count)++;
      return CORK_HASH_TABLE_MAP_CONTINUE;
  }

  struct cork_alloc  *alloc = /* from somewhere */;
  struct cork_hash_table  *table = /* from somewhere */;
  size_t  count = 0;
  cork_hash_table_map(alloc, table, count_entries, &count);
  /* the number of entries is now in count */


The second strategy is to iterate through the entries yourself.  Since
the internal struture of the :c:type:`cork_hash_table` type is opaque
(and slightly more complex than a simple array), you have to use a
special “iterator” type to manage the manual iteration.  Note that
unlike when using a mapping function, it is **not** safe to delete
entries in a hash table as you manually iterate through them.

.. type:: struct cork_hash_table_iterator

   A helper type for manually iterating through the entries in a hash
   table.  All of the fields in this type are private.  You'll usually
   allocate this type on the stack.

.. function:: void cork_hash_table_iterator_init(struct cork_hash_table \*table, struct cork_hash_table_iterator \*iterator)

   Initializes a new iterator for the given hash table.

.. function:: struct cork_hash_table_entry \*cork_hash_table_iterator_next(struct cork_hash_table \*table, struct cork_hash_table_iterator \*iterator)

   Returns the next entry in *table*, according to the given iterator.
   If you've already iterated through all of the entries in *table*,
   we'll return ``NULL``.

With these functions, manually counting the hash table entries looks
like::

  struct cork_hash_table  *table = /* from somewhere */;
  struct cork_hash_table_iterator  iter;
  struct cork_hash_table_entry  *entry;
  size_t  count = 0;

  cork_hash_table_iterator_init(table, &iter);
  while ((entry = cork_hash_table_iterator_next(table, &iter)) != NULL) {
      count++;
  }
  /* the number of elements is now in count */
