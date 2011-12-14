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
tuple of an *error class* and an *error code*, along with a
human-readable string description of the error.  Error classes represent
broad classes of errors, and usually correspond to a library or to an
important group of related functions within a library.  An error class
is represented by a hash of some string identifying the library or group
of functions.  This “hash of a string” approach makes it easy to define
new error classes, without needing any centralized mechanism for
assigning IDs to the various classes.  An error code is a simple
integer, and only needs to be unique within a particular error class.
This means that each error class is free to define its error codes
however it wishes (usually via an ``enum`` type), without having to
worry about them clashing with the codes of any other class.

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

.. function:: const char \*cork_error_message(const struct cork_error \*err)

   Returns the human-readable string description of the error.  If no
   error occurred, the result of this function is undefined.

.. note::

   If you pass in a :c:type:`cork_error` instance to the function call,
   you don't actually have to check the function's return value to see
   if an error occurred; you can just call
   :c:func:`cork_error_occurred()`.

When you're done with your error instance, you should use
``cork_error_done`` to dispose of it.

.. function:: void cork_error_done(struct cork_error \*err)

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

If your function results in an error, you should use the
:c:func:`cork_error_set()` function to fill in the *err* parameter with
details about the error, in addition to returning a ``NULL`` or ``-1``
error code.  Note that ``cork_error_set`` works just fine if the caller
passed in a ``NULL`` *err* parameter (signifying that they don't care
about the details of any error that occurs).  In that case,
``cork_error_set`` behaves just like a no-op.

.. function:: void cork_error_set(struct cork_error \*err, cork_error_class cls, cork_error_code code, const char \*format, ...)

   Fill in a :c:type:`cork_error` with information about an error
   condition.  If *err* is ``NULL``, then nothing occurs.  Otherwise,
   *err* will be filled in with the given error class and code, and with
   a string description that's constructed from the given ``printf``
   format and values.


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
              cork_error_done(&suberr);
          } else {
              /* We can't recover from this error, so propagate it on */
              cork_error_propagate(err, &suberr);
              return -1;
          }
      }

      /* etc */
      return 0;
  }

.. function:: void cork_error_clear(struct cork_error \*err)

   Clears an error condition instance.  If *err* is ``NULL``, then we
   don't do anything, since there's no error to clear.

.. function:: void cork_error_propagate(struct cork_error \*err, struct cork_error \*suberr)

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


Defining a new error class
--------------------------

If none of the built-in error classes and codes suffice for an error
condition that you need to report, you'll have to define our own error
class.  The first step is to decide on some string that will represent
your error class.  This string must be unique across all error classes,
so it should include (at least) some representation of the library name.
In libcork itself, we always use the name of the header file that the
error class is defined in.  (This limits us to one error class per
header, but that's not a deal-breaker.)  Thus, the
:c:macro:`CORK_NET_ADDRESS_ERROR` error class is represented by the
string ``"libcork/core/net-addresses.h"``.

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
      /* An unknown error while parsing a network address. */
      CORK_NET_ADDRESS_UNKNOWN_ERROR,
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
