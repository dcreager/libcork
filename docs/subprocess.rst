.. _subprocesses:

************
Subprocesses
************

.. highlight:: c

::

  #include <libcork/os.h>

The functions in this section let you fork child processes, run arbitrary
commands in them, and collect any output that they produce.


Subprocess objects
~~~~~~~~~~~~~~~~~~

.. type:: struct cork_subprocess

   Represents a child process.  There are several functions for creating child
   processes, described below.


.. function:: void cork_subprocess_free(struct cork_subprocess \*sub)

   Free a subprocess.  The subprocess must not currently be executing.


Creating subprocesses
~~~~~~~~~~~~~~~~~~~~~

There are several functions that you can use to create child processes.

.. function:: struct cork_subprocess \*cork_subprocess_new_exec(const char \*program, char \* const \*params, struct cork_stream_consumer \*stdout, struct cork_stream_consumer \*stderr, unsigned int flags)

   Create a new subprocess that will execute another program.  *program* should
   either be an absolute path to an executable on the local filesystem, or the
   name of an executable that should be found in the current ``PATH``.  *params*
   should be a parameter array suitable to pass into the program's ``main``
   function.  (It must be ``NULL``\ -terminated, and its first element must be
   the *program*.)

   If you want to collect the data that the subprocess writes to its stdout and
   stderr streams, you should pass in :ref:`stream consumer <stream-consumers>`
   instances for the *stdout* and/or *stderr* parameters.  If either (or both)
   of these parameters is ``NULL``, then the child process will inherit the
   corresponding output stream from the current process.  (Usually, this means
   that the child's stdout or stderr will be interleaved with the parent's.)

   *flags* should be a bitwise-OR (``|``) of any of the following flags (or 0 if
   you don't want any flags):

   .. macro:: CORK_SUBPROCESS_PROPAGATE_TERMINATION

      While the subprocess is executing, propagate any termination signals that
      the parent process receives in to the children.


Executing subprocesses
~~~~~~~~~~~~~~~~~~~~~~

.. function:: int cork_subprocess_start_and_wait(size_t sub_count, struct cork_subprocess \*\*subs)

   Execute several subprocesses, and wait for them all to terminate.  If there
   are any errors starting the subprocesses, we will return ``-1`` and set an
   :ref:`error condition <errors>`.
