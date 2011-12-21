.. _errors:

***************
Error reporting
***************

.. highlight:: c

::

  #include <libcork/core.h>

This section defines an API for reporting error conditions.  It's
loosely modeled on glib's *GError* API.

The standard POSIX approach for reporting errors is to return an integer
status code, and to store error codes into the ``errno`` global
variable.  This approach has a couple of drawbacks.  The first is that
you have to ensure that ``errno`` is placed in thread-local storage, so
that separate threads have their own error condition variables.  The
second, and in our mind more important, is that the set of error codes
is fixed and platform-dependent.  It's difficult to add new error codes
to represent application-level error conditions.

The libcork error API is a way around this.  Errors are represented by a
tuple of an *error class* and an *error code*, along with a pointer to a
*printer function* that can produce a human-readable string description
of the error.  Error classes represent broad classes of errors, and
usually correspond to a library or to an important group of related
functions within a library.  An error class is represented by a hash of
some string identifying the library or group of functions.  This “hash
of a string” approach makes it easy to define new error classes, without
needing any centralized mechanism for assigning IDs to the various
classes.  An error code is a simple integer, and only needs to be unique
within a particular error class.  This means that each error class is
free to define its error codes however it wishes (usually via an
``enum`` type), without having to worry about them clashing with the
codes of any other class.

Individual error classes are also able to associate “extra” data with an
error condition.  This can be used by the error's printer function to
customize the human-readable string.  (For instance, a parse error might
include a reference the location of a syntax error, or even a copy of
the malformed string.)

.. type:: struct cork_error

   An object representing a particular error condition.  This type
   should be considered opaque; you should use the various accessor
   functions described below to interrogate an error instance.

.. macro:: CORK_ERROR_NONE

   A special error class that signals that no error occurred.


Calling a function that can return an error
-------------------------------------------

There are two basic forms for a function that can produce an error.  The
first is if the function returns a single pointer as its result::

  TYPE *
  my_function(/* parameters */, struct cork_error *err);

The second is for any other function::

  int
  my_function(/* parameters */, struct cork_error *err);

If you only want to check whether an error occurred or not, you can pass
in ``NULL`` for the *err* parameter, and simply check the function's
return value.  If an error occurs, the function will return either
``NULL`` or ``-1``, depending on its return type.  Success will be
indicated by a non-\ ``NULL`` pointer or a ``0``.  (More complex return
value schemes are possible, if the function needs to signal more than a
simple “success” or “failure”; in that case, you'll need to check the
function's documentation for details.)

If you want to know specifics about the error, you need to create a
:c:type:`cork_error` instance to pass in for the *err* parameter.  The
easiest way to do this is to simply allocate one on the stack::

  struct cork_alloc  *alloc = /* obtained elsewhere */;
  struct cork_error  err = CORK_ERROR_INIT(alloc);

or::

  struct cork_alloc  *alloc = /* obtained elsewhere */;
  struct cork_error  err;
  cork_error_init(alloc, &err);

.. function:: bool cork_error_init(struct cork_alloc \*alloc, struct cork_error \*err)
              struct cork_error CORK_ERROR_INIT(struct cork_alloc \*alloc)

   Initializes an error instance that you've allocated on the stack or
   in some other storage.  The ``CORK_ERROR_INIT`` version can only be
   used as a static initializer.

When the function returns, there are several functions that you can use
to interrogate the error instance.

.. function:: bool cork_error_occurred(const struct cork_error \*err)

   After passing *err* into a function that might return an error
   condition, you can use this function to check whether an error
   actually occurred.

.. function:: cork_error_class cork_error_class(const struct cork_error \*err)
              cork_error_code cork_error_code(const struct cork_error \*err)

   Returns the class and code of an error condition.  If no error
   occurred, the error class will be :c:macro:`CORK_ERROR_NONE`, and the
   code will be ``0``.

.. function:: int cork_error_message(struct cork_alloc \*alloc, struct cork_error \*err, struct cork_buffer \*dest)

   Places the human-readable string description of *err* into *dest*.
   (You have to pass in a :ref:`resizable buffer <buffer>` since we
   can't know in advance how long the string description will be.) If no
   error occurred, the result of this function is undefined.

.. note::

   If you pass in a :c:type:`cork_error` instance to the function call,
   you don't actually have to check the function's return value to see
   if an error occurred; you can just call
   :c:func:`cork_error_occurred()`.

When you're done with your error instance, you should use
``cork_error_done`` to dispose of it.

.. function:: void cork_error_done(struct cork_alloc \*alloc, struct cork_error \*err)

   Finalizes an error condition instance.


Writing a function that can return an error
-------------------------------------------

When writing a function that might produce an error condition, your
function signature should follow one of the two standard patterns
described above::

  int
  my_function(/* parameters */, struct cork_error *err);

  TYPE *
  my_function(/* parameters */, struct cork_error *err);

You should return ``-1`` or ``NULL`` if an error occurs, and ``0`` or a
non-\ ``NULL`` pointer if it succeeds.  If ``NULL`` is a valid
“successful” result of the function, you should use the first form, and
define a ``TYPE **`` output parameter to return the actual pointer
value.  (If you're using the first form, you can use additional return
codes if there are other possible results besides a simple “success” and
“failure”.)

If your function results in an error, you need to fill in your
function's *err* parameter.  Each error class will define several public
functions that can be used to fill an error condition instance (e.g.,
:ref:`Network address error conditions <net-address-errors>`).  Note
that these setter functions work just fine if the caller passed in a
``NULL`` *err* parameter (signifying that they don't care about the
details of any error that occurs).  In that case, the setter function
behaves just like a no-op.


Propagating errors from nested function calls
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Things can be slightly more complicated if you're writing a function
that returns an error condition, which in turn calls a different
function that returns an error condition.

Most of the time, you can get away with passing in your own *err*
parameter when calling the nested function::

  int
  outer_function(/* parameters */, struct cork_error *err)
  {
      int  rc;

      rc = inner_function(/* more parameters */, err);
      if (rc == -1) {
          return rc;
      }

      /* do some more stuff */
      return 0;
  }

This works because you don't need to interrogate *err* to determine if
an error occurred; you can always check the inner function's result
(looking for ``-1`` or ``NULL``).

The complications show up if you need to check the error condition.  For
instance, your outer function might be able to recover from some error
conditions, but not others.  In that case, you **can't** pass your *err*
parameter into the inner function, since the caller is free to pass in a
``NULL`` :c:type:`cork_error` pointer.  And that wouldn't be good, since
then you wouldn't have an error condition to interrogate!  Instead, you
need to define your own ``cork_error`` instance, and then *clear* or
*propagate* that into the caller's *err* instance as appropriate::

  int
  outer_function(struct cork_alloc *alloc, /* params */, struct cork_error *err)
  {
      struct cork_error  suberr = CORK_ERROR_INIT(alloc);

      inner_function(/* more params */, &suberr);
      if (cork_error_occurred(&suberr)) {
          /* As an example, let's say that we can recover from a
           * CORK_NET_ADDRESS_PARSE_ERROR. */
          if ((cork_error_class(&suberr) == CORK_NET_ADDRESS_ERROR) &&
              (cork_error_code(&suberr) == CORK_NET_ADDRESS_PARSE_ERROR)) {
              /* Perform some kind of recovery, and then clean up the error */
              cork_error_done(alloc, &suberr);
          } else {
              /* We can't recover from this error, so propagate it on */
              cork_error_propagate(alloc, err, &suberr);
              return -1;
          }
      }

      /* etc */
      return 0;
  }

.. function:: void cork_error_propagate(struct cork_alloc \*alloc, struct cork_error \*err, struct cork_error \*suberr)

   Propagates an error condition from one instance to another.  In the
   most common case, *err* will be the error instance passed in from the
   current function's caller, while *suberr* will be an instance
   allocated in the current function.  In other words, *err* might be
   ``NULL``, while *suberr* never should be.

   If *err* is ``NULL``, indicating that your caller doesn't care about
   the details of the error, then we just finalize *suberr*.  If *err*
   is non-\ ``NULL``, then we move the contents of *suberr* into *err*.

   In both cases, *suberr* will be finalized when
   ``cork_error_propagate`` returns.  You **should not** call
   :c:func:`cork_error_done` on *suberr* afterwards.


Error-checking macros
---------------------

There can be a lot of repetitive code when calling functions that return
:c:type:`cork_error` error conditions.  We provide a collection of
helper macros that make it easier to write this code.

.. note::

   Unlike most libcork modules, these macros are **not** automatically
   defined when you include the ``libcork/core.h`` header file.  Since
   they're used so often, the macros don't include a ``cork_`` prefix,
   saving a handful of keystrokes.  Because of this, we don't want to
   pollute your namespace unless you ask for the macros.  To do so, you
   must explicitly include their header file::

     #include <libcork/core/checkers.h>

Returning a default error code
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you follow one of the standard function signature patterns described
above, then your function will either return an ``int`` or some pointer
type, and errors will be signalled by a return value of ``-1`` or
``NULL``.  If so, you can use the macros in this section to
automatically return the appropriate error return value if a nested
function call returns an error.

With these macros, you won't have a chance to inspect the error
condition when an error occurs, so you should pass in your own *err*
parameter when calling the nested function.

(The mnemonic for remembering these macro names is that they all start
with ``rXY_``.  The ``r`` indicates that they automatically “return”.
The second character indicates whether *your* function returns an
``int`` or a pointer.  The third character indicates whether the
function you're *calling* returns an ``int`` or a pointer.)

.. function:: void rii_check(call)

   Call a function that returns an ``int`` error indicator, when your
   function also returns an ``int``.  If the nested function call
   returns an error, we propagate that error on.

.. function:: void rip_check(call)

   Call a function that returns a pointer, when your function returns an
   ``int``.  If the nested function call returns an error, we propagate
   that error on.

.. function:: void rpi_check(call)

   Call a function that returns an ``int`` error indicator, when your
   function returns a pointer.  If the nested function call returns an
   error, we propagate that error on.

.. function:: void rpp_check(call)

   Call a function that returns a pointer, when your function also
   returns a pointer.  If the nested function call returns an error, we
   propagate that error on.

Returning a non-standard return value
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If your function doesn't have a standard signature, or it uses
additional return values besides ``0``, ``1``, ``NULL``, and valid
pointers, then you can use the macros in this section to return a custom
return value in case of an error.

With these macros, you won't have a chance to inspect the error
condition when an error occurs, so you should pass in your own *err*
parameter when calling the nested function.

(The mnemonic for remembering these macro names is that they all start
with ``xY_``.  The ``x`` doesn't standard for anything in particular.
The second character indicates whether the function you're *calling*
returns an ``int`` or a pointer.  We don't need separate macros for
*your* function's return type, since you provide a return value
explicitly.)

.. function:: void xi_check(retval, call)

   Call a function that returns an ``int`` error indicator.  If the
   nested function call raises an error, we propagate that error on, and
   return *retval* from the current function.

.. function:: void xp_check(retval, call)

   Call a function that returns a pointer.  If the nested function call
   raises an error, we propagate that error on, and return *retval* from
   the current function.

Post-processing when an error occurs
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you need to perform some post-processing when a nested function
returns an error, you can use the functions in this section.  They will
automatically jump to the current scope's ``error`` label whenever an
error occurs.

(The mnemonic for remembering these macro names is that they all start
with ``eY_``.  The ``e`` indicates that they'll jump to the ``error``
label.  The second character indicates whether the function you're
*calling* returns an ``int`` or a pointer.  We don't need separate
macros for *your* function's return type, since the macros won't
automatically return anything.)

.. function:: void ei_check(retval, call)

   Call a function that returns an ``int`` error indicator.  If the
   nested function call raises an error, we automatically jump to the
   current scope's ``error`` label.

.. function:: void ep_check(retval, call)

   Call a function that returns a pointer.  If the nested function call
   raises an error, we automatically jump to the current scope's
   ``error`` label.

Allocating new instances
~~~~~~~~~~~~~~~~~~~~~~~~

All of the previous macros are used to propagate errors from within
nested function calls.  Another common use case is to allocate a new
instance of some type (using either a custom allocator or a garbage
collection context), and to raise a new error if the allocation fails.
The macros in this section make it easier to write this kind of code.

.. function:: void e_check_new(type, var, desc)
              void x_check_new(retval, type, var, desc)
              void ri_check_new(type, var, desc)
              void rp_check_new(type, var, desc)

   Allocates a new instance using a custom allocator.  These macros
   assume that you have a parameter or variable named ``alloc`` that
   contains a custom allocator, an error condition parameter named
   ``err``, and that you've already declared a variable named *var*, of
   type *type*.  *desc* should be a human-readable name of the kind of
   object you're trying to allocate.  We'll automatically allocate a new
   instance, storing it into *var*.  If the allocation fails, we'll fill
   in *err* with a :c:macro:`CORK_ALLOC_CANNOT_ALLOCATE` error
   condition.

.. function:: void e_check_gc_new(type, var, desc)
              void x_check_gc_new(retval, type, var, desc)
              void ri_check_gc_new(type, var, desc)
              void rp_check_gc_new(type, var, desc)

   Allocates a new instance of a garbage-collected object.  These macros
   assume that you have a parameter or variable named ``alloc`` that
   contains a custom allocator, an error condition parameter named
   ``err``, and that you've already declared a variable named *var*, of
   type *type*.  They also assume that the garbage collection interface
   for *type* is named ``[type]_gc_iface``.  *desc* should be a
   human-readable name of the kind of object you're trying to allocate.
   We'll automatically allocate a new instance, storing it into *var*.
   If the allocation fails, we'll fill in *err* with a
   :c:macro:`CORK_ALLOC_CANNOT_ALLOCATE` error condition.

   .. note::

      Note that *type* should **not** contain the ``struct`` prefix of
      your garbage-collected type.  We add that to the type name
      automatically.  (This lets us construct the default garbage
      collection interface name.)

.. function:: void e_check_gc_inew(type, iface, var, desc)
              void x_check_gc_inew(retval, type, iface, var, desc)
              void ri_check_gc_inew(type, iface, var, desc)
              void rp_check_gc_inew(type, iface, var, desc)

   Allocates a new instance of a garbage-collected object.  These macros
   assume that you have a parameter or variable named ``alloc`` that
   contains a custom allocator, an error condition parameter named
   ``err``, and that you've already declared a variable named *var*, of
   type *type*.  They also assume that the garbage collection interface
   for *type* is named *iface*.  *desc* should be a human-readable name
   of the kind of object you're trying to allocate.  We'll automatically
   allocate a new instance, storing it into *var*.  If the allocation
   fails, we'll fill in *err* with a
   :c:macro:`CORK_ALLOC_CANNOT_ALLOCATE` error condition.

   .. note::

      Note that *type* should **not** contain the ``struct`` prefix of
      your garbage-collected type.  We add that to the type name
      automatically.

.. function:: void e_check_alloc(call, desc)
              void x_check_alloc(retval, call, desc)
              void ri_check_alloc(call, desc)
              void rp_check_alloc(call, desc)

   Checks the result of an arbitrary allocation.  *call* should be a
   statement that allocates some new memory.  These macros assume that
   you have a parameter or variable named ``alloc`` that contains a
   custom allocator, and an error condition parameter named ``err``.
   *desc* should be a human-readable name of the kind of object you're
   trying to allocate.    If the allocation fails, we'll fill in *err*
   with a :c:macro:`CORK_ALLOC_CANNOT_ALLOCATE` error condition.


Defining a new error class
--------------------------

If none of the built-in error classes and codes suffice for an error
condition that you need to report, you'll have to define our own error
class.

Error classes and codes
~~~~~~~~~~~~~~~~~~~~~~~

The first step is to decide on some string that will represent your
error class.  This string must be unique across all error classes, so it
should include (at least) some representation of the library name.  In
libcork itself, we always use the name of the header file that the error
class is defined in.  (This limits us to one error class per header, but
that's not a deal-breaker.)  Thus, the :c:macro:`CORK_NET_ADDRESS_ERROR`
error class is represented by the string
``"libcork/core/net-addresses.h"``.

Given this string, you can produce the error class's hash value using
the ``extras/hashstring.py`` script that's included in the libcork
source::

  $ python extras/hashstring.py "libcork/core/net-addresses.h"
  0x1f76fedf

The next step is to define the error codes within the class.  This is
best done by creating an ``enum`` class.  Taken together, we have the
following definitions for the error conditions in the
:ref:`net-addresses` module::

  /* hash of "libcork/core/net-addresses.h" */
  #define CORK_NET_ADDRESS_ERROR  0x1f76fedf

  enum cork_net_address_error {
      /* A parse error while parsing a network address. */
      CORK_NET_ADDRESS_PARSE_ERROR
  };

This gives us a constant for the error class, and a set of constants for
each error code within the class, all of which start with a standard
namespace prefix (``CORK_NET_ADDRESS_``).

.. type:: uint32_t  cork_error_class

   An identifier for a class of error conditions.  Should be the hash of
   a unique string describing the error class.

.. type:: unsigned int  cork_error_code

   An identifier for a particular type of error within an error class.
   The particular values within an error class should be defined using
   an ``enum`` type.

Printer functions
~~~~~~~~~~~~~~~~~

For each error code, you'll also need to define a *printer function*
that can produce the human-readable description of an error, and a
public *setter function* that can fill in a :c:type:`cork_error`
instance with a particular error class/code combination.  It's possible
to have multiple variations for each error code, if you want; for
instance, we have different versions of ``CORK_NET_ADDRESS_PARSE_ERROR``
depending on whether the caller tried to create an IPv4, IPv6, or
general IP address.

.. type:: int (\*cork_error_printer)(struct cork_alloc \*alloc, struct cork_error \*err, struct cork_buffer \*dest)

   Renders the human-readable description of *err* into *dest*.

In the simplest case, the human-readable description will be a fixed
string.  As an example::

  static int
  cork_basic_error(struct cork_alloc *alloc, struct cork_error *err,
                     struct cork_buffer *dest)
  {
      return cork_buffer_set_string
          (alloc, dest, "Unknown error!", NULL);
  }

  int
  cork_basic_error_set(struct cork_alloc *alloc, struct cork_error *err)
  {
      return cork_error_set(alloc, err,
                            CORK_BUILTIN_ERROR,
                            CORK_UNKNOWN_ERROR,
                            cork_basic_error);
  }

.. note::

   Within the printer function, we ignore any additional errors that
   occur, by passing in a ``NULL`` :c:type:`cork_error` parameter to
   :c:func:`cork_buffer_set_string`.

In this case, your printer function is just a wrapper around a call to
:c:func:`cork_buffer_set_string()`, and your setter function is just a
wrapper around a call to ``cork_error_set``:

.. function:: int cork_error_set(struct cork_alloc \*alloc, struct cork_error \*error, cork_error_class eclass, cork_error_code ecode, cork_error_printer printer)

   Fills in *err* with the given error condition.  The error condition
   is defined by the error class *eclass*, the error code *ecode*, and
   the printer function *printer*.


Including extra data
~~~~~~~~~~~~~~~~~~~~

In more complex cases, you might want to customize the human-readable
description of the error based on some additional data, such as the
parameters to the original function call.  You can do this via the
“extra” data pointer included in each :c:type:`cork_error` instance.
By using the ``cork_error_set_extra`` and ``cork_error_set_extra_raw``
functions, you can copy a fixed amount of additional data into the error
instance when filling it in with an error condition:

.. function:: int cork_error_set_extra_raw(struct cork_alloc \*alloc, struct cork_error \*error, cork_error_class eclass, cork_error_code ecode, cork_error_printer printer, void \*extra, size_t extra_size)
              int cork_error_set_extra(struct cork_alloc \*alloc, struct cork_error \*error, cork_error_class eclass, cork_error_code ecode, cork_error_printer printer, TYPE &extra)

   Fills in *err* just like :c:func:`cork_error_set()`.  It also copies
   additional data into the error instance's “extra” field.  For the
   ``_raw`` variant, you provide an explicit pointer and size to the
   extra data to copy.  For the non-\ ``raw`` variant, you just provide
   a reference to some lvalue visible in the current scope.  (It must be
   an lvalue so that we can take its address.)  We'll figure out the
   size of the extra data using the ``sizeof`` operator.

.. function:: void \*cork_error_extra(struct cork_error \*err)

   Returns a pointer to the extra data associated with the error
   condition.

A limited amount of space is reserved in the :c:type:`cork_error`
``struct`` itself to hold extra data.  If your extra data fits within
this bound (currently 64 bytes), then it will be copied into the
:c:type:`cork_error` without requiring any additional heap allocations.
If your extra data is larger than this bound, however,
:c:func:`cork_error_set_extra` will allocate space from the heap to
store the extra data.

In your printer function, you can then access this extra data to
customize the human-readable description.  The
``CORK_NET_ADDRESS_PARSE_ERROR`` error condition uses this mechanism to
include the malformed IP address in the error message::

  static int
  cork_ip_parse_error(struct cork_alloc *alloc, struct cork_error *err,
                      struct cork_buffer *dest)
  {
      const char  **str = cork_error_extra(err);
      return cork_buffer_printf
          (alloc, dest, NULL, "Invalid IP address: %s", *str);
  }

  int
  cork_ip_parse_error_set(struct cork_alloc *alloc, struct cork_error *err,
                          const char *invalid_str)
  {
      return cork_error_set_extra(alloc, err,
                                  CORK_NET_ADDRESS_ERROR,
                                  CORK_NET_ADDRESS_PARSE_ERROR,
                                  cork_ip_parse_error,
                                  invalid_str);
  }

Note that this printer function assumes that the malformed string will
still be a live object when the printer function is called.

Builtin errors
--------------

There are a few basic, builtin errors that you can use if no others are
applicable.  In almost all cases, you'll want to define a more specific
error class and code instead.

.. macro:: CORK_BUILTIN_ERROR
           CORK_UNKNOWN_ERROR

   The error class and codes used for the error conditions described in
   this section.

.. function:: int cork_unknown_error_set(struct cork_alloc \*alloc, struct cork_error \*err)

   Fills in *err* to indicate that there was some unknown error.  The
   error description will include the name of the current function.
