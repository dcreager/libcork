.. _multithreading:

**********************
Multithreading support
**********************

.. highlight:: c

::

  #include <libcork/threads.h>

libcork provides several functions for handling threads and writing
thread-aware code in a portable way.


.. _threads:

Thread information
==================

.. type:: unsigned int cork_thread_id

   An identifier for a thread in the current process.  This is a portable type;
   it is not based on the "raw" thread ID used by the underlying thread
   implementation.  This type will always be equivalent to ``unsigned int``, on
   all platforms.  Furthermore, :c:data:`CORK_THREAD_NONE` will always refer to
   an instance of this type that we guarantee will not be used by any thread.

.. var:: cork_thread_id CORK_THREAD_NONE

   A :c:type:`cork_thread_id` value that will not be used as the ID of any
   thread.  You can use this value to represent "no thread" in any data
   structures you create.  Moreover, we guarantee that ``CORK_THREAD_NONE`` will
   have the value ``0``, which lets you zero-initialize a data structure
   containing a :c:type:`cork_thread_id`, and have its initial state
   automatically represent "no thread".

.. function:: cork_thread_id cork_thread_get_id(void)

   Returns the identifier of the currently executing thread.


.. _atomics:

Atomic operations
=================

We provide several platform-agnostic macros for implementing common
atomic operations.


Addition
~~~~~~~~

.. function:: void cork_int_atomic_add(volatile int_t \*var, int_t delta)
              void cork_uint_atomic_add(volatile uint_t \*var, uint_t delta)

   Atomically add *delta* to the variable pointed to by *var*, returning
   the result of the addition.

.. function:: void cork_int_atomic_pre_add(volatile int_t \*var, int_t delta)
              void cork_uint_atomic_pre_add(volatile uint_t \*var, uint_t delta)

   Atomically add *delta* to the variable pointed to by *var*, returning
   the value from before the addition.


Subtraction
~~~~~~~~~~~

.. function:: void cork_int_atomic_sub(volatile int_t \*var, int_t delta)
              void cork_uint_atomic_sub(volatile uint_t \*var, uint_t delta)

   Atomically subtract *delta* from the variable pointed to by *var*,
   returning the result of the subtraction.

.. function:: void cork_int_atomic_pre_sub(volatile int_t \*var, int_t delta)
              void cork_uint_atomic_pre_sub(volatile uint_t \*var, uint_t delta)

   Atomically subtract *delta* from the variable pointed to by *var*,
   returning the value from before the subtraction.


Compare-and-swap
~~~~~~~~~~~~~~~~

.. function:: int_t cork_int_cas(volatile int_t \*var, int_t old_value, int_t new_value)
              uint_t cork_uint_cas(volatile uint_t \*var, uint_t old_value, uint_t new_value)
              TYPE \*cork_ptr_cas(TYPE \* volatile \*var, TYPE \*old_value, TYPE \*new_value)

   Atomically check whether the variable pointed to by *var* contains
   the value *old_value*, and if so, update it to contain the value
   *new_value*.  We return the value of *var* before the
   compare-and-swap.  (If this value is equal to *old_value*, then the
   compare-and-swap was successful.)


.. _once:

Executing something once
========================

The functions in this section let you ensure that a particular piece of
code is executed exactly once, even if multiple threads attempt the
execution at roughly the same time.

.. macro:: cork_once_barrier(name)

   Declares a barrier that can be used with the :c:func:`cork_once`
   macro.

.. macro:: cork_once(barrier, call)

   Ensure that *call* (which can be an arbitrary statement) is executed
   exactly once, regardless of how many times control reaches the call
   to ``cork_once``.  If control reaches the ``cork_once`` call at
   roughly the same time in multiple threads, exactly one of them will
   be allowed to execute the code.  The call to ``cork_once`` won't
   return until *call* has been executed.

   If you have multiple calls to ``cork_once`` that use the same
   *barrier*, then exactly one *call* will succeed.  If the *call*
   statements are different in those ``cork_once`` invocations, then
   it's undefined which one gets executed.

   It's fine if the function that contains the ``cork_once`` call is
   recursive; if the same thread tries to obtain the underlying lock
   multiple times, the second and later calls will silently succeed.

These macros are usually used to initialize a static variable that will
be shared across multiple threads::

    static struct my_type  shared_value;

    static void
    expensive_initialization(void)
    {
        /* do something to initialize shared_value */
    }

    cork_once_barrier(shared_value_once);

    struct my_type *
    get_shared_value(void)
    {
        cork_once(shared_value_once, expensive_initialization());
        return &shared_value;
    }

Each thread can then call ``get_shared_value`` to retrieve a properly
initialized instance of ``struct my_type``.  Regardless of how many
threads call this function, and how often they call it, the value will
be initialized exactly once, and will be guaranteed to be initialized
before any thread tries to use it.


.. _tls:

Thread-local storage
====================

The macro in this section can be used to create thread-local storage in
a platform-agnostic manner.

.. macro:: cork_tls(TYPE type, SYMBOL name)

   Creates a static function called :samp:`{[name]}_get`, which will
   return a pointer to a thread-local instance of *type*.  This is a
   static function, so it won't be visible outside of the current
   compilation unit.

   When a particular thread's instance is created for the first time, it
   will be filled with ``0`` bytes.  If the actual type needs more
   complex initialization before it can be used, you can create a
   wrapper struct that contains a boolean indiciating whether that
   initialization has happened::

       struct wrapper {
           bool  initialized;
           struct real_type  val;
       };

       cork_tls(struct wrapper, wrapper);

       static struct real_type *
       real_type_get(void)
       {
           struct wrapper * wrapper = wrapper_get();
           struct real_type * real_val = &wrapper->val;
           if (CORK_UNLIKELY(!wrapper->initialized)) {
               expensive_initialization(real_val);
           }
           return real_val;
       }

   It's also not possible to provide a finalization function; if your
   thread-local variable acquires any resources or memory that needs to
   be freed when the thread finishes, you must make a “thread cleanup”
   function that you explicitly call at the end of each thread.

   .. note::

      On some platforms, the number of thread-local values that can be
      created by any given process is limited (i.e., on the order of 128
      or 256 values).  This means that you should limit the number of
      thread-local values you create, especially in a library.
