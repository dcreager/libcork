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

.. note::

   We correctly maintain a separate error condition for each thread in
   the current process.  This is all hidden by the functions in this
   section; it's safe to call them from multiple threads simultaneously.

.. macro:: CORK_ERROR_NONE

   A special error class that signals that no error occurred.


Calling a function that can return an error
-------------------------------------------

There are two basic forms for a function that can produce an error.  The
first is if the function returns a single pointer as its result::

  TYPE *
  my_function(/* parameters */);

The second is for any other function::

  int
  my_function(/* parameters */);

If an error occurs, the function will return either ``NULL`` or ``-1``,
depending on its return type.  Success will be indicated by a non-\
``NULL`` pointer or a ``0``.  (More complex return value schemes are
possible, if the function needs to signal more than a simple “success”
or “failure”; in that case, you'll need to check the function's
documentation for details.)

If you want to know specifics about the error, we provide several
accessor functions:

.. function:: bool cork_error_occurred(void)
              cork_error_class cork_error_get_class(void)
              cork_error_code cork_error_get_code(void)
              const char \*cork_error_message(void)

   Returns information about the current error condition.  This
   information is maintained in thread-local storage, so it is safe
   to call these functions from multiple threads simultaneously.  Note
   that you often won't need to call ``cork_error_occurred``, since
   you'll usually be able to detect error conditions by checking a
   function's return value.

If you want to know specifics about the error, there are several
functions that you can use to interrogate the current error condition.

.. function:: bool cork_error_occurred(void)

   Returns whether an error has occurred.

.. function:: cork_error_class cork_error_get_class(void)
              cork_error_code cork_error_get_code(void)

   Returns the class and code of the current error condition.  If no
   error has occurred, the error class will be
   :c:macro:`CORK_ERROR_NONE`, and the code will be ``0``.

.. function:: const char \*cork_error_message(void)

   Returns the human-readable string description the current error
   condition.  If no error occurred, the result of this function is
   undefined.

You can use the :c:func:`cork_error_prefix` function to add additional context
to the beginning of an error message.

.. function:: void cork_error_prefix(const char \*format, ...)

   Prepends some additional text to the current error condition.

When you're done checking the current error condition, you clear it so
that later calls to :c:func:`cork_error_occurred` and friends don't
re-report this error.

.. function:: void cork_error_clear(void)

   Clears the current error condition.


Writing a function that can return an error
-------------------------------------------

When writing a function that might produce an error condition, your
function signature should follow one of the two standard patterns
described above::

  int
  my_function(/* parameters */);

  TYPE *
  my_function(/* parameters */);

You should return ``-1`` or ``NULL`` if an error occurs, and ``0`` or a
non-\ ``NULL`` pointer if it succeeds.  If ``NULL`` is a valid
“successful” result of the function, you should use the first form, and
define a ``TYPE **`` output parameter to return the actual pointer
value.  (If you're using the first form, you can use additional return
codes if there are other possible results besides a simple “success” and
“failure”.)

If your function results in an error, you need to fill in the current
error condition using the ``cork_error_set`` function:

.. function:: void cork_error_set(cork_error_class eclass, cork_error_code ecode, const char \*format, ...)

   Fills in the current error condition.  The error condition is defined
   by the error class *eclass*, the error code *ecode*.  The
   human-readable description is constructed from *format* and any
   additional parameters.

As an example, the :ref:`IP address <net-addresses>` parsing functions
fill in ``CORK_NET_ADDRESS_PARSE_ERROR`` error conditions when you try
to parse a malformed address::

  const char  *str = /* the string that's being parsed */;
  cork_error_set
      (CORK_NET_ADDRESS_ERROR, CORK_NET_ADDRESS_PARSE_ERROR,
       "Invalid IP address: %s", str);

If a particular kind of error can be raised in several places
throughout your code, it can be useful to define a helper function for
filling in the current error condition::

  static void
  cork_ip_address_parse_error(const char *version, const char *str)
  {
      cork_error_set
          (CORK_NET_ADDRESS_ERROR, CORK_NET_ADDRESS_PARSE_ERROR,
           "Invalid %s address: %s", version, str);
  }


Error-checking macros
---------------------

There can be a lot of repetitive code when calling functions that return
error conditions.  We provide a collection of helper macros that make it
easier to write this code.

.. note::

   Unlike most libcork modules, these macros are **not** automatically
   defined when you include the ``libcork/core.h`` header file, since
   they don't include a ``cork_`` prefix.  Because of this, we don't
   want to pollute your namespace unless you ask for the macros.  To do
   so, you must explicitly include their header file::

     #include <libcork/helpers/errors.h>

Additional debugging output
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. macro:: CORK_PRINT_ERRORS

   If you define this macro to ``1`` before including
   :file:`libcork/helpers/errors.h`, then we'll output the current
   function name, file, and line number, along with the description of
   the error, to stderr whenever an error is detected by one of the
   macros described in this section.

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

.. function:: void rie_check(call)

   Call a function whose return value isn't enough to check for an error, when
   your function returns an ``int``.  We'll use :c:func:`cork_error_occurred` to
   check for an error.  If the nested function call returns an error, we
   propagate that error on.

.. function:: void rii_check(call)

   Call a function that returns an ``int`` error indicator, when your
   function also returns an ``int``.  If the nested function call
   returns an error, we propagate that error on.

.. function:: void rip_check(call)

   Call a function that returns a pointer, when your function returns an
   ``int``.  If the nested function call returns an error, we propagate
   that error on.

.. function:: void rpe_check(call)

   Call a function whose return value isn't enough to check for an error, when
   your function returns a pointer.  We'll use :c:func:`cork_error_occurred` to
   check for an error.  If the nested function call returns an error, we
   propagate that error on.

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

.. function:: void xe_check(retval, call)

   Call a function whose return value isn't enough to check for an error.  If
   the nested function call raises an error, we propagate that error on, and
   return *retval* from the current function.

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

.. function:: void ei_check(call)

   Call a function whose return value isn't enough to check for an error.  If
   the nested function call raises an error, we automatically jump to the
   current scope's ``error`` label.

.. function:: void ei_check(call)

   Call a function that returns an ``int`` error indicator.  If the
   nested function call raises an error, we automatically jump to the
   current scope's ``error`` label.

.. function:: void ep_check(call)

   Call a function that returns a pointer.  If the nested function call
   raises an error, we automatically jump to the current scope's
   ``error`` label.


Calling POSIX functions
~~~~~~~~~~~~~~~~~~~~~~~

The :c:func:`cork_system_error_set` function automatically translates a POSIX
error (specified in the standard ``errno`` variable) into a libcork error
condition.  We also define several helper macros for calling a POSIX function
and automatically checking its result.

::

   #include <libcork/helpers/posix.h>

.. note::

   For all of these macros, the ``EINTR`` POSIX error is handled specially.
   This error indicates that a system call was interrupted by a signal, and that
   the call should be retried.  The macros do not translate ``EINTR`` errors
   into libcork errors; instead, they will retry the ``call`` until the
   statement succeeds or returns a non-``EINTR`` error.

.. function:: void rii_check_posix(call)

   Call a function that returns an ``int`` error indicator, when your function
   also returns an ``int``.  If the nested function call returns a POSIX error,
   we translate it into a libcork error and return a libcork error code.

.. function:: void rip_check_posix(call)

   Call a function that returns a pointer, when your function returns an
   ``int``.  If the nested function call returns a POSIX error, we translate it
   into a libcork error and return a libcork error code.

.. function:: void rpi_check_posix(call)

   Call a function that returns an ``int`` error indicator, when your function
   returns a pointer.  If the nested function call returns a POSIX error, we
   translate it into a libcork error and return a libcork error code.

.. function:: void rpp_check_posix(call)

   Call a function that returns a pointer, when your function also returns a
   pointer.  If the nested function call returns a POSIX error, we translate it
   into a libcork error and return a libcork error code.

.. function:: void ei_check_posix(call)

   Call a function that returns an ``int`` error indicator.  If the nested
   function call raises a POSIX error, we translate it into a libcork error and
   automatically jump to the current scope's ``error`` label.

.. function:: void ep_check_posix(call)

   Call a function that returns a pointer.  If the nested function call raises a
   POSIX error, we translate it into a libcork error and automatically jump to
   the current scope's ``error`` label.


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
the :ref:`cork-hash <cork-hash>` command that's installed with libcork::

  $ cork-hash "libcork/core/net-addresses.h"
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

With your error class and code defined, you can fill in error instances
using :c:func:`cork_error_set()`.


Builtin errors
--------------

There are a few basic, builtin errors that you can use if no others are
applicable.  In almost all cases, you'll want to define a more specific
error class and code instead.

.. macro:: CORK_BUILTIN_ERROR
           CORK_SYSTEM_ERROR
           CORK_UNKNOWN_ERROR

   The error class and codes used for the error conditions described in
   this section.

.. function:: void cork_system_error_set(void)

   Fills in the current error condition with information from the C
   library's ``errno`` variable.  The human-readable description of the
   error will be obtained from the standard ``strerror`` function.

.. function:: void cork_unknown_error_set(void)

   Fills in the current error condition to indicate that there was some
   unknown error.  The error description will include the name of the
   current function.

.. function:: void cork_abort(const char \*fmt, ...)

   Aborts the current program with an error message given by *fmt* and any
   additional parameters.

.. function:: void cork_unreachable(void)

   Aborts the current program with a message indicating that the code path
   should be unreachable.  This can be useful in the ``default`` clause of a
   ``switch`` statement if you can ensure that one of the non-``default``
   branches will always be selected.
