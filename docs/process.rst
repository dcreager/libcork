.. _processes:

*********
Processes
*********

.. highlight:: c

::

  #include <libcork/os.h>

The functions in this section let you interact with the current running process.


Cleanup functions
~~~~~~~~~~~~~~~~~

Often you will need to perform some cleanup tasks whenever the current process
terminates normally.  The functions in this section let you do that.

.. function:: void cork_cleanup_at_exit(int priority, cork_cleanup_function function)
              void cork_cleanup_at_exit_named(const char \*name, int priority, cork_cleanup_function function)

   Register a *function* that should be called when the current process
   terminates.  When multiple functions are registered, the order in which they
   are called is determined by their *priority* values --- functions with lower
   priorities will be called first.  If any functions have the same priority
   value, there is no guarantee about the order in which they will be called.

   All cleanup functions must conform to the following signature:

   .. type:: void (\*cork_cleanup_function)(void)

   The ``_named`` variant lets you provide an explicit name for the cleanup
   function, which currently is only used when printing out debug messages.  The
   plain variant automatically detects the name of *function*, so that you don't
   have to provide it explicitly.
