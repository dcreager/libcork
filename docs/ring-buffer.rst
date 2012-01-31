.. _ring-buffer:

************
Ring buffers
************

.. highlight:: c

::

  #include <libcork/ds.h>


.. type:: struct cork_ring_buffer

   A ring buffer data structure that can be easily embedded into other
   types.  All of the fields in ``cork_ring_buffer`` are private; you
   should only access the contents of the ring buffer via the functions
   defined below.

   The elements of a ring buffer are ``void *`` pointers.  (You can also
   store integers via the :c:type:`intptr_t` and :c:type:`uintptr_t`
   types.)  Ring buffers have a fixed capacity, which must be specified
   when the ring buffer instance is initialized.  You cannot add extra
   space to an existing ring buffer.

   Ring buffers implement a FIFO queue structure; elements will be
   returned by :c:func:`cork_ring_buffer_pop()` in the same order that
   they're added by :c:func:`cork_ring_buffer_add()`.


.. function:: int cork_ring_buffer_init(struct cork_ring_buffer \*buf, size_t size)

   Initializes a ring buffer instance, having a capacity of *size*
   elements.  If we cannot initialize the ring buffer, we'll return
   ``1``.

.. function:: void cork_ring_buffer_done(struct cork_ring_buffer \*buf)

   Finalizes a ring buffer instance.  Nothing special is done to any
   remaining elements in the ring buffer; if they need to be finalized,
   you should do that yourself before calling this function.


.. function:: bool cork_ring_buffer_is_empty(struct cork_ring_buffer \*buf)
              bool cork_ring_buffer_is_full(struct cork_ring_buffer \*buf)

   Returns whether the ring buffer is empty or full.  (You cannot add
   elements to a full ring buffer, and you cannot pop elemenst from an
   empty one.)


.. function:: int cork_ring_buffer_add(struct cork_ring_buffer \*buf, void \*element)

   Adds *element* to a ring buffer.  If the ring buffer is full, we
   return ``-1``, and the ring buffer will be unchanged.  Otherwise we
   return ``0``.

.. function:: void \*cork_ring_buffer_pop(struct cork_ring_buffer \*buf)
              void \*cork_ring_buffer_peek(struct cork_ring_buffer \*buf)

   Returns the next element in the ring buffer.  If the ring buffer is
   empty, we return ``NULL``.  The ``_pop`` variant will remove the
   returned element from the ring buffer before returning it; the
   ``_peek`` variant will leave the element in the ring buffer.
