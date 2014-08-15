.. _allocation:

*****************
Memory allocation
*****************

.. highlight:: c

::

  #include <libcork/core.h>

The macros in this section provide nicer interfaces for certain memory
allocation tasks.  We provide two strategies for handling memory
allocation failures.  The default is to abort the program, since there's
usually not going to be much actual recovery that can be done.  There
are also macros that will return a ``NULL`` pointer (just like the
standard ``malloc`` and friends), allowing you to try to perform some
recovery.  (If you just want to print out a friendly error message and
quit the program, use the default abort-on-failure macros; they'll do
both of those things for you, and your code will be much simpler.)

.. note::

   All of the internal memory allocation performed by libcork is done
   using the abort-on-failure macros.  The recover-on-failure macros
   only affect memory allocation that you perform yourself.


Aborting on failure
===================

The macros in this section abort the program if any memory allocation
fails.

.. function:: void \*cork_malloc(size_t size)
              void \*cork_calloc(size_t count, size_t size)
              void \*cork_realloc(void \*ptr, size_t new_size)

   Uses the standard ``malloc``, ``calloc``, and ``realloc`` functions
   to allocate (or reallocate) some memory.  These functions behave
   exactly like the underlying standard functions; however, if the
   allocation fails, they'll print out an error message and abort the
   program using ``abort()``.

   .. note::

      Note that the possible memory leak in the standard ``realloc``
      function doesn't apply here, since we're going to abort the whole
      program if the reallocation fails.

.. function:: type \*cork_new(TYPE type)

   Allocate a new instance of the given type.  This is exactly
   equivalent to::

       cork_malloc(sizeof(type))

Since we use the standard ``malloc`` and friends to allocate the memory,
you should use the standard ``free`` function to free it.


Recovering from failures
========================

The macros in this section will return a ``NULL`` pointer if any memory
allocation fails, allowing you to (possibly? maybe?) recover from the
error condition.

.. function:: void \*cork_xmalloc(size_t size)
              void \*cork_xcalloc(size_t count, size_t size)
              void \*cork_xrealloc(void \*ptr, size_t new_size)
              type \*cork_xnew(TYPE type)

   These macros do the same thing as the non-\ ``x`` variants, but they
   don't abort the program if the allocation fails.  Instead, you'll get
   a ``NULL`` pointer back.  You should still use the standard ``free``
   function to free the memory when you're done with it.

   Note that ``cork_xrealloc`` is more safe than the standard A common
   idiom when calling the standard ``realloc`` function is::

       void  *ptr = /* from somewhere */;
       /* UNSAFE!  Do not do this! */
       ptr = realloc(ptr, new_size);

   This is unsafe, however.  The ``realloc`` function returns a ``NULL``
   pointer if the reallocation fails.  By assigning directly into *ptr*,
   you'll get a memory leak in these situations.  The ``cork_xrealloc``
   function, on the other hand, will automatically free the existing
   pointer if the reallocation fails, eliminating the memory leak::

       void  *ptr = /* from somewhere */;
       /* This is safe.  Do this. */
       ptr = cork_realloc(ptr, new_size);


String-related functions
========================

.. function:: const char \*cork_strdup(const char \*str)
              const char \*cork_xstrdup(const char \*str)

   Creates a copy of the given C string.  You shouldn't modify the
   contents of the copied string.  You must use :c:func:`cork_strfree()`
   to free the string when you're done with it.  The ``x`` variant
   returns a ``NULL`` pointer if the allocation fails; the non-\ ``x``
   variant aborts the program.

.. function:: void cork_strfree(const char \*str)

   Frees *str*, which must have been created using
   :c:func:`cork_strdup()` or :c:func:`cork_xstrdup()`.
