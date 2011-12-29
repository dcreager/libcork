.. _allocation:

*****************
Custom allocation
*****************

.. highlight:: c

::

  #include <libcork/core.h>

The types and functions in this section let you customize how libcork
allocates objects and memory regions from the heap.

There tend to be two common strategies prevelant in the wild for
allowing the users of a library to provide custom allocation functions:
the *global variable* approach, and the *context parameter* approach.
In the global variable approach, there's a function that lets you
register a custom allocator, which is then used for all allocations made
by any function in the library.  In the context parameter approach, the
custom allocator is provided as a parameter to every function that needs
to allocate memory.  The global variable approach is usually seen as
more convenient, since each function call is simpler, while the context
parameter approach is more flexible, since you can use different
allocators in different parts of a large program.

In libcork, we use the context parameter approach; most libcork
functions will take in a :c:type:`cork_alloc` pointer as their first
parameter.  This incurs a slight overhead in typing when you write code
that uses libcork, but we feel that the increased flexibility is worth
it.

This means that if you're writing a library that uses libcork, you
should write your functions to take in :c:type:`cork_alloc` pointers as
parameters, just like libcork's functions do.  This way, your library
isn't tied to any particular custom allocator.

If you're writing a program that uses libcork (or uses some other
library that uses libcork), then part of your program's initialization
will be to choose a particular allocator implementation to use, and then
to pass that allocator object into any libcork functions that you call
later on.

This gives you a nice separation of concerns: choosing an allocator
(whether :ref:`builtin <builtin-allocators>` or :ref:`custom
<custom-allocators>`) is completely orthogonal to :ref:`using
<using-allocators>` the allocator.


.. _using-allocators:

Using an allocator
==================

Given a custom allocator, you can use the functions in this section to
allocate, reallocate, or free objects and memory regions.

.. note::

   One major difference between libcork's allocation functions and the
   standard ``malloc``/``free`` function calls is that you need to
   remember how large a memory region is when you reallocate or free it.
   This tends not to be a problem, since in most cases, you're either
   working with a fixed-size type (so you know at compile time what size
   to pass into :c:func:`cork_free()`\ ), or you'll need to maintain the
   size of a variable-size buffer anyway.


Size-based functions
--------------------

.. function:: void \*cork_malloc(struct cork_alloc \*alloc, size_t size)

   Allocates a new memory region of the given *size*.  Returns ``NULL``
   if the region can't be allocated.

.. function:: void \*cork_realloc(struct cork_alloc \*alloc, void \*ptr, size_t old_size, size_t new_size)

   Reallocates a memory region to a different size.  If the memory
   region can't be resized, then we free the existing memory region and
   return ``NULL``.  Otherwise we return a pointer to the reallocated
   memory region.  We don't make any guarantees about whether the
   existing memory region can be reused, so the return value may or may
   not be equal to *ptr*.

.. function:: void cork_free(struct cork_alloc \*alloc, void \*ptr, size_t old_size)

   Frees a memory region of the given size.  It is your responsibility
   to ensure that *old_size* matches the size that was used to allocate
   (or most recently reallocate) *ptr*.


Type-based functions
--------------------

.. function:: type \*cork_new(struct cork_alloc \*alloc, TYPE type)

   Allocates a new instance of *type*.  The size of the memory region to
   allocate is calculated using the ``sizeof`` operator, and the result
   will be automatically cast to ``type *``.  Returns ``NULL`` if the
   region can't be allocated.

.. function:: void cork_delete(struct cork_alloc \*alloc, TYPE type, type \*instance)

   Frees an instance of *type*.  The size of the memory region is
   calculated using the ``sizeof`` operator.


String-related functions
------------------------

.. function:: const char \*cork_strdup(struct cork_alloc \*alloc, const char \*str)

   Creates a copy of the given C string.  You shouldn't modify the
   contents of the copied string.  You must use :c:func:`cork_strfree()`
   to free the string when you're done with it.

.. function:: void cork_strfree(struct cork_alloc \*alloc, const char \*str)

   Frees *str*, which must have been created using
   :c:func:`cork_strdup()`.


Allocation errors
-----------------

.. macro:: CORK_ALLOC_ERROR
           CORK_ALLOC_CANNOT_ALLOCATE

   An error class and code that can be used to indicate allocation
   failures.

.. function:: int cork_cannot_allocate_set(struct cork_alloc \*alloc, struct cork_error \*err, const char \*what)

   Fills in *err* to indicate that an allocation attempt failed.  *what*
   should be the human-readable name of the object that you tried to
   allocate.


.. _builtin-allocators:

Built-in allocators
===================

When writing an application, you'll often just want to use the standard
``malloc`` allocator provided by your C library.  The functions in this
section provide libcork custom allocator wrappers for this use case.

.. note::

   You'll still need to free your custom allocator object using
   :c:func:`cork_allocator_free` as part of your program's cleanup.


.. function:: struct cork_alloc \*cork_allocator_new_malloc(void)

   Creates a new allocator object that uses the standard ``malloc``,
   ``realloc``, and ``free`` functions.

.. function:: struct cork_alloc \*cork_allocator_new_debug(void)

   Creates a new allocator object that uses the standard ``malloc``,
   ``realloc``, and ``free`` functions, and also performs the following
   tests on the memory regions that are created by allocator:

   * When an object is freed, we verify that the size of the object
     that's passed into :c:func:`cork_free()` matches the size that was
     used to allocate the object.

   .. note::

      This function is useful for test cases, but probably shouldn't be
      used in production code.


.. _custom-allocators:

Writing a custom allocator
==========================

.. note::

   You can safely ignore the contents of this section if you only plan
   on using one of the :ref:`builtin-allocators`.


No extra state
--------------

In the simplest case, your custom allocator is simply a wrapper around
an existing allocation library.  In this case, you just need to provide
a function whose signature matches :c:type:`cork_alloc_func`, and then
use :c:func:`cork_allocator_new()` to create your custom allocator
object.

This approach works great if you can call global functions in your
wrapper function.  (This is how the standard ``malloc``/``free`` wrapper
is written, for instance.)  If you need to maintain additional state,
you'll have to write your own :c:type:`cork_alloc` :ref:`subclass
<allocator-subclass>`.

.. type:: void \* (\*cork_alloc_func)(struct cork_alloc \*alloc, void \*ptr, size_t osize, size_t nsize)

   A function that can allocate, reallocate, or free a memory buffer.
   *ptr* is a pointer to an existing memory location, *osize* is the
   allocated size of this existing memory location, and *nsize* is the
   desired new size of the allocation.  (The caller will guarantee that
   *ptr* is ``NULL`` iff *osize* is 0, and that *osize* and *nsize* will
   not both be 0.)

   This single function definition encompasses the standard ``malloc``,
   ``realloc``, and ``free`` calls.  If *osize* is 0, then the function
   should allocate a new heap object, similar to ``malloc``.  If neither
   *osize* nor *nsize* are 0, then the function should reallocate an
   existing heap object, similar to ``realloc``.  If *osize* is not 0,
   but *nsize* is 0, then the function should free an existing heap
   object, similar to ``free``.

   If the function is asked to allocate or reallocate a heap object, it
   should return ``NULL`` if the allocation fails.  If a reallocation
   fails, it is the function's responsibility to free the existing heap
   object, to prevent memory leaks.

   If the function is asked to free an existing heap object, the
   function must always returns ``NULL``.

.. function:: struct cork_alloc \*cork_allocator_new(cork_alloc_func alloc_func)

   Creates a new custom allocator object from the given allocation
   function.  You can use this function if you don't need to maintain
   any additional state in your custom allocator object.  In this case,
   the ``struct cork_alloc`` type is completely sufficient for your
   needs.

   We'll allocate the ``struct cork_alloc`` instance using the
   allocation function.  You don't need to provide a :c:member:`free
   <cork_alloc.free>` function; since we know how big the allocation
   object will be, we can provide the correct ``free`` function for you.

.. function:: void cork_allocator_free(struct cork_alloc \*alloc)

   Finalizes and frees an allocator object.  You should call this
   function on any custom allocator object when you're done with it;
   regardless of whether it's a custom implementation you've written
   yourself, or one of the :ref:`builtin-allocators`.

   .. note::

      Don't confuse this function with :c:func:`cork_free()`.  This
      function is used to free the *allocator itself* when you're done
      with it, whereas :c:func:`cork_free()` uses the custom allocator
      to free some other memory region.


.. _allocator-subclass:

Allocator subclasses
--------------------

If you need to maintain any additional state, you should embed a
``struct cork_alloc`` instance inside of some other type::

  struct my_custom_alloc {
      struct cork_alloc  parent;
      /* additional fields */
  };

  static void *
  my_custom_alloc_func(struct cork_alloc *valloc, void *ptr, size_t os, size_t ns)
  {
      struct my_custom_alloc  *alloc =
          cork_container_of(valloc, struct my_custom_alloc, parent);
      /* work some magic */
  }

Note how your :c:member:`allocation function <cork_alloc.alloc>` will
always be given a pointer to a ``struct cork_alloc``.  You can use the
:c:func:`cork_container_of()` macro to obtain a pointer to your “real”
allocator instance.

You'll also need to provide functions for creating and destroying your
custom allocator object.  Notice how the finalizer function is
``static``, since you'll still use the public
:c:func:`cork_allocator_free()` function to free the allocator object.

::

  static void
  my_custom_alloc_free(struct cork_alloc *valloc)
  {
      struct my_custom_alloc  *alloc =
          cork_container_of(valloc, struct my_custom_alloc, parent);
      /* do any necessary cleanup */

      /* Use the allocator to free itself */
      cork_delete(valloc, struct my_custom_alloc, valloc);
  }

  struct cork_alloc *
  my_custom_alloc_new(void)
  {
      /* Mimic a call to cork_new to create the allocator */
      struct my_custom_alloc  *alloc =
          my_custom_alloc_func(NULL, NULL, 0, sizeof(struct my_custom_alloc));
      if (alloc == NULL) {
          return NULL;
      }

      /* Perform any additional initialization */

      /* We need this trick since the alloc field is const in struct
       * cork_alloc.  (And by using a union, we don't violate any strict
       * aliasing rules.) */
      union {
          struct cork_alloc  *original;
          cork_alloc_func  *alloc;
      } mutable;
      mutable.original = &alloc->parent;
      *mutable.alloc = my_custom_alloc_func;

      alloc->parent.free = my_custom_alloc_free;
      return &alloc->parent;
  }


.. type:: struct cork_alloc

   .. member:: const cork_alloc_func  alloc

      The allocation function that we'll call to allocate and deallocate
      memory regions.

   .. member:: void (\*free)(struct cork_alloc \*alloc)

      A function that will be used to free the ``struct cork_alloc``
      instance when it's no longer needed.  (We need a customizable
      function here because we don't know how big the overall type is if
      you embed the ``struct cork_alloc`` within some other type.)
