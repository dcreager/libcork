.. _multithreading:

**********************
Multithreading support
**********************

.. highlight:: c

::

  #include <libcork/threads.h>

libcork provides several functions for handling threads and writing
thread-aware code in a portable way.


.. _atomics:

Atomic operations
=================

We provide several platform-agnostic macros for implementing common
atomic operations.


Addition
~~~~~~~~

.. function:: void cork_int8_atomic_add(volatile int8_t \*var, int8_t delta)
              void cork_int16_atomic_add(volatile int16_t \*var, int16_t delta)
              void cork_int32_atomic_add(volatile int32_t \*var, int32_t delta)
              void cork_int64_atomic_add(volatile int64_t \*var, int64_t delta)
              void cork_uint8_atomic_add(volatile uint8_t \*var, uint8_t delta)
              void cork_uint16_atomic_add(volatile uint16_t \*var, uint16_t delta)
              void cork_uint32_atomic_add(volatile uint32_t \*var, uint32_t delta)
              void cork_uint64_atomic_add(volatile uint64_t \*var, uint64_t delta)
              void cork_short_atomic_add(volatile short \*var, short delta)
              void cork_int_atomic_add(volatile int \*var, int delta)
              void cork_long_atomic_add(volatile long \*var, long delta)
              void cork_ushort_atomic_add(volatile unsigned short \*var, unsigned short delta)
              void cork_uint_atomic_add(volatile unsigned int \*var, unsigned int delta)
              void cork_ulong_atomic_add(volatile unsigned long \*var, unsigned long delta)

   Atomically add *delta* to the variable pointed to by *var*, returning
   the result of the addition.

.. function:: void cork_int8_atomic_pre_add(volatile int8_t \*var, int8_t delta)
              void cork_int16_atomic_pre_add(volatile int16_t \*var, int16_t delta)
              void cork_int32_atomic_pre_add(volatile int32_t \*var, int32_t delta)
              void cork_int64_atomic_pre_add(volatile int64_t \*var, int64_t delta)
              void cork_uint8_atomic_pre_add(volatile uint8_t \*var, uint8_t delta)
              void cork_uint16_atomic_pre_add(volatile uint16_t \*var, uint16_t delta)
              void cork_uint32_atomic_pre_add(volatile uint32_t \*var, uint32_t delta)
              void cork_uint64_atomic_pre_add(volatile uint64_t \*var, uint64_t delta)
              void cork_short_atomic_pre_add(volatile short \*var, short delta)
              void cork_int_atomic_pre_add(volatile int \*var, int delta)
              void cork_long_atomic_pre_add(volatile long \*var, long delta)
              void cork_ushort_atomic_pre_add(volatile unsigned short \*var, unsigned short delta)
              void cork_uint_atomic_pre_add(volatile unsigned int \*var, unsigned int delta)
              void cork_ulong_atomic_pre_add(volatile unsigned long \*var, unsigned long delta)

   Atomically add *delta* to the variable pointed to by *var*, returning
   the value from before the addition.


Subtraction
~~~~~~~~~~~

.. function:: void cork_int8_atomic_sub(volatile int8_t \*var, int8_t delta)
              void cork_int16_atomic_sub(volatile int16_t \*var, int16_t delta)
              void cork_int32_atomic_sub(volatile int32_t \*var, int32_t delta)
              void cork_int64_atomic_sub(volatile int64_t \*var, int64_t delta)
              void cork_uint8_atomic_sub(volatile uint8_t \*var, uint8_t delta)
              void cork_uint16_atomic_sub(volatile uint16_t \*var, uint16_t delta)
              void cork_uint32_atomic_sub(volatile uint32_t \*var, uint32_t delta)
              void cork_uint64_atomic_sub(volatile uint64_t \*var, uint64_t delta)
              void cork_short_atomic_sub(volatile short \*var, short delta)
              void cork_int_atomic_sub(volatile int \*var, int delta)
              void cork_long_atomic_sub(volatile long \*var, long delta)
              void cork_ushort_atomic_sub(volatile unsigned short \*var, unsigned short delta)
              void cork_uint_atomic_sub(volatile unsigned int \*var, unsigned int delta)
              void cork_ulong_atomic_sub(volatile unsigned long \*var, unsigned long delta)

   Atomically subtract *delta* from the variable pointed to by *var*,
   returning the result of the subtraction.

.. function:: void cork_int8_atomic_pre_sub(volatile int8_t \*var, int8_t delta)
              void cork_int16_atomic_pre_sub(volatile int16_t \*var, int16_t delta)
              void cork_int32_atomic_pre_sub(volatile int32_t \*var, int32_t delta)
              void cork_int64_atomic_pre_sub(volatile int64_t \*var, int64_t delta)
              void cork_uint8_atomic_pre_sub(volatile uint8_t \*var, uint8_t delta)
              void cork_uint16_atomic_pre_sub(volatile uint16_t \*var, uint16_t delta)
              void cork_uint32_atomic_pre_sub(volatile uint32_t \*var, uint32_t delta)
              void cork_uint64_atomic_pre_sub(volatile uint64_t \*var, uint64_t delta)
              void cork_short_atomic_pre_sub(volatile short \*var, short delta)
              void cork_int_atomic_pre_sub(volatile int \*var, int delta)
              void cork_long_atomic_pre_sub(volatile long \*var, long delta)
              void cork_ushort_atomic_pre_sub(volatile unsigned short \*var, unsigned short delta)
              void cork_uint_atomic_pre_sub(volatile unsigned int \*var, unsigned int delta)
              void cork_ulong_atomic_pre_sub(volatile unsigned long \*var, unsigned long delta)

   Atomically subtract *delta* from the variable pointed to by *var*,
   returning the value from before the subtraction.


Compare-and-swap
~~~~~~~~~~~~~~~~

.. function:: int8_t cork_int8_cas(volatile int8_t \*var, int8_t old_value, int8_t new_value)
              int16_t cork_int16_cas(volatile int16_t \*var, int16_t old_value, int16_t new_value)
              int32_t cork_int32_cas(volatile int32_t \*var, int32_t old_value, int32_t new_value)
              int64_t cork_int64_cas(volatile int64_t \*var, int64_t old_value, int64_t new_value)
              uint8_t cork_uint8_cas(volatile uint8_t \*var, uint8_t old_value, uint8_t new_value)
              uint16_t cork_uint16_cas(volatile uint16_t \*var, uint16_t old_value, uint16_t new_value)
              uint32_t cork_uint32_cas(volatile uint32_t \*var, uint32_t old_value, uint32_t new_value)
              uint64_t cork_uint64_cas(volatile uint64_t \*var, uint64_t old_value, uint64_t new_value)
              short cork_short_cas(volatile short \*var, short old_value, short new_value)
              int cork_int_cas(volatile int \*var, int old_value, int new_value)
              long cork_long_cas(volatile long \*var, long old_value, long new_value)
              unsigned short cork_ushort_cas(volatile unsigned short \*var, unsigned short old_value, unsigned short new_value)
              unsigned int cork_uint_cas(volatile unsigned int \*var, unsigned int old_value, unsigned int new_value)
              unsigned long cork_ulong_cas(volatile unsigned long \*var, unsigned long old_value, unsigned long new_value)
              TYPE \*cork_ptr_cas(TYPE \* volatile \*var, TYPE \*old_value, TYPE \*new_value)

   Atomically check whether the variable pointed to by *var* contains
   the value *old_value*, and if so, update it to contain the value
   *new_value*.  We return the value of *var* before the
   compare-and-swap.  (If this value is equal to *old_value*, then the
   compare-and-swap was successful.)


.. _threads:

Thread information
==================

.. type:: cork_thread_id

   An identifier for a thread in the current process.

.. function:: cork_thread_id cork_thread_get_id(void)

   Returns the identifier of the currently executing thread.


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
