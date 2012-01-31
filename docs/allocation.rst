.. _allocation:

*****************
Memory allocation
*****************

.. highlight:: c

::

  #include <libcork/core.h>

The macros in this section provide nicer interfaces for certain memory
allocation tasks.


Safe reallocation
-----------------

.. function:: void \*cork_realloc(void \*ptr, size_t old_size, size_t new_size)

   A common idiom when calling the standard ``realloc`` function is::

       void  *ptr = /* from somewhere */;
       /* UNSAFE!  Do not do this! */
       ptr = realloc(ptr, new_size);

   This is unsafe, however.  The ``realloc`` function returns a ``NULL``
   pointer if the reallocation fails.  By assigning directly into *ptr*,
   you'll get a memory leak in these situations.  The ``cork_realloc``
   function will automatically free the existing pointer if the
   reallocation fails, eliminating the memory leak::

       void  *ptr = /* from somewhere */;
       /* This is safe.  Do this. */
       ptr = cork_realloc(ptr, new_size);


Type-based functions
--------------------

.. function:: type \*cork_new(TYPE type)

   Allocates a new instance of *type*.  The size of the memory region to
   allocate is calculated using the ``sizeof`` operator, and the result
   will be automatically cast to ``type *``.  Returns ``NULL`` if the
   region can't be allocated.

.. function:: void cork_delete(TYPE type, type \*instance)

   Frees an instance of *type*.  The size of the memory region is
   calculated using the ``sizeof`` operator.


String-related functions
------------------------

.. function:: const char \*cork_strdup(const char \*str)

   Creates a copy of the given C string.  You shouldn't modify the
   contents of the copied string.  You must use :c:func:`cork_strfree()`
   to free the string when you're done with it.

.. function:: void cork_strfree(const char \*str)

   Frees *str*, which must have been created using
   :c:func:`cork_strdup()`.


Allocation errors
-----------------

.. macro:: CORK_ALLOC_ERROR
           CORK_CANNOT_ALLOCATE

   An error class and code that can be used to indicate allocation
   failures.

.. function:: int cork_cannot_allocate_set(struct cork_error \*err, const char \*what)

   Fills in *err* to indicate that an allocation attempt failed.  *what*
   should be the human-readable name of the object that you tried to
   allocate.
