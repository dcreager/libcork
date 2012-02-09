.. _array:

****************
Resizable arrays
****************

.. highlight:: c

::

  #include <libcork/ds.h>

This section defines a resizable array class, similar to C++'s
``std::vector`` or Java's ``ArrayList`` classes.  Our arrays can store
any fixed-size element.  The arrays automatically resize themselves as
necessary to store the elements that you add.  In addition, we allocate
a small amount of storage in the array itself, which prevents additional
heap allocations for arrays containing a small number of elements.  (The
array will automatically shift over to heap-allocated storage once this
internal storage has been exhausted.)

.. note::

   Most of the functions in this section are actually implemented as
   macros.


.. type:: cork_array(element_type)

   A resizable array that contains elements of type *element_type*.

.. function:: void cork_array_init(cork_array(T) \*array)

   Initializes a new array.  You should allocate *array* yourself,
   presumably on the stack or directly within some other data type.  The
   array will start empty.

.. function:: void cork_array_done(cork_array(T) \*array)

   Finalizes an array, freeing any storage that was allocated to hold
   the arrays elements.  We don't finalize the individual array
   elements; you're responsible for doing this before calling this
   function.

.. function:: size_t cork_array_size(cork_array(T) \*array)

   Returns the number of elements in *array*.

.. function:: bool cork_array_is_empty(cork_array(T) \*array)

   Returns whether *array* has any elements.

.. function:: T cork_array_at(cork_array(T) \*array, size_t index)

   Returns the element in *array* at the given *index*.  Like accessing
   a normal C array, we don't do any bounds checking.  The result is a
   valid lvalue, so it can be directly assigned to::

     cork_array(int64_t)  array;
     cork_array_append(array, 5, err);
     cork_array_at(array, 0) = 12;

.. function:: int cork_array_append(cork_array(T) \*array, T element)

   Appends *element* to the end of *array*, reallocating the array's
   storage if necessary.  We'll return an error condition if we need to
   resize the array but the reallocation fails.

.. function:: T \*cork_array_append_get(cork_array(T) \*array)

   Appends a new, uninitialized element to the end of *array*,
   reallocating the array's storage if necessary.  We'll return a
   pointer to the new element, or ``NULL`` if we need to resize the
   array but the reallocation fails.

.. function:: int cork_array_ensure_size(cork_array(T) \*array, size_t desired_count)
              int cork_array_ensure_size_(cork_array(T) \*array, size_t desired_count, size_t element_size)

   Ensures that *array* has enough allocated space to store
   *desired_count* elements.  We'll reallocate the array's storage if
   needed.  The actual size and contents of the array aren't changed.
   The second variant of this function requires you to pass in the
   array's element size explicitly; you'll usually never need to call
   this variant.  The first variant calculates the element size for you
   automatically.

.. function:: size_t cork_array_element_size(cork_array(T) \*array)

   Returns the size of the elements that are stored in *array*.  You
   won't normally need to call this, since you can just use
   ``sizeof(T)``.
