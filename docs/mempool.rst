.. _mempool:

************
Memory pools
************

.. highlight:: c

::

  #include <libcork/core.h>

The functions in this section let you define *memory pools*, which allow
you to reduce the overhead of allocating and freeing large numbers of
small objects.  Instead of generating a ``malloc`` call for each
individual object, the memory pool allocates a large *block* of memory,
and then subdivides this block of memory into objects of the desired
size.  The free objects in the memory pool are linked together in a
singly-linked list, which means that allocation and deallocation is
usually a (very small) constant-time operation.

.. note::

   Memory pools are *not* thread safe; if you have multiple threads
   allocating objects of the same type, they'll need separate memory
   threads.


.. type:: struct cork_mempool

   A memory pool.  All of the objects created by the memory pool will be
   the same size; this size is provided when you initialize the memory
   pool.  You should not access any of the fields of this type directly.

.. function:: void cork_mempool_init_size(struct cork_mempool \*mp, size_t element_size)
              void cork_mempool_init(struct cork_mempool \*mp, TYPE type)

   Initialize a new memory pool.  The size of the objects allocated by
   the memory pool is given either as an explicit *element_size*, or by
   giving the *type* of the objects.  The blocks allocated by the memory
   pool will be of a default size (currently 4Kb).

.. function:: void cork_mempool_init_size_ex(struct cork_mempool \*mp, size_t element_size, size_t block_size)
              void cork_mempool_init_ex(struct cork_mempool \*mp, TYPE type, size_t block_size)

   Initialize a new memory pool.  The size of the objects allocated by
   the memory pool is given either as an explicit *element_size*, or by
   giving the *type* of the objects.  The blocks allocated by the memory
   pool will be *block_size* bytes large.

.. function:: void cork_mempool_done(struct cork_mempool \*mp)

   Finalize a memory pool.  You **must** have already freed all of the
   objects allocated by the pool; if you haven't, then this function
   will cause the current process to abort.

.. function:: void \*cork_mempool_new(struct cork_mempool \*mp)

   Allocate a new object from the memory pool.  Returns ``NULL`` if we
   cannot allocate a new object.

.. function:: void cork_mempool_free(struct cork_mempool \*mp, void \*ptr)

   Free an object that was allocated from the memory pool.
