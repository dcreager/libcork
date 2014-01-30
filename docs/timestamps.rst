.. _timestamps:

*************************
High-precision timestamps
*************************

.. highlight:: c

::

  #include <libcork/core.h>


.. type:: uint64_t  cork_timestamp

   A high-precision timestamp type.  A timestamp is represented by a
   64-bit integer, whose unit is the *gammasecond* (γsec), where
   :math:`1~\textrm{γsec} = \frac{1}{2^{32}} \textrm{sec}`.  With this
   representation, the upper 32 bits of a timestamp value represent the
   timestamp truncated (towards zero) to seconds.

   For this type, we don't concern ourselves with any higher-level
   issues of clock synchronization or time zones.  ``cork_timestamp``
   values can be used to represent any time quantity, regardless of
   which time standard (UTC, GMT, TAI) you use, or whether it takes into
   account the local time zone.


.. function:: void cork_timestamp_init_sec(cork_timestamp \*ts, uint32_t sec)
              void cork_timestamp_init_gsec(cork_timestamp \*ts, uint32_t sec, uint32_t gsec)
              void cork_timestamp_init_msec(cork_timestamp \*ts, uint32_t sec, uint32_t msec)
              void cork_timestamp_init_usec(cork_timestamp \*ts, uint32_t sec, uint32_t usec)
              void cork_timestamp_init_nsec(cork_timestamp \*ts, uint32_t sec, uint32_t nsec)

   Initializes a timestamp from a separate seconds part and fractional
   part.  For the ``_sec`` variant, the fractional part will be set to
   ``0``.  For the ``_gsec`` variant, you provide the fractional part in
   gammaseconds.  For the ``_msec``, ``_usec``, and ``_nsec`` variants, the
   fractional part will be translated into gammaseconds from milliseconds,
   microseconds, or nanoseconds, respectively.


.. function:: void cork_timestamp_init_now(cork_timestamp \*ts)

   Initializes a timestamp with the current UTC time of day.

   .. note::

      The resolution of this function is system-dependent.


.. function:: uint32_t cork_timestamp_sec(const cork_timestamp ts)
              uint32_t cork_timestamp_round_sec(const cork_timestamp ts)

   Returns the seconds portion of a timestamp.  If the timestamp includes a
   subsecond fractional component, the ``round`` variant will round the result
   towards the nearest second.  The non-``round`` variant will truncate the
   result towards zero.

.. function:: uint32_t cork_timestamp_gsec(const cork_timestamp ts)

   Returns the fractional portion of a timestamp, in gammaseconds.

.. function:: void cork_timestamp_round_msec(const cork_timestamp ts, uint32_t \*sec, uint32_t \*msec)
              void cork_timestamp_round_usec(const cork_timestamp ts, uint32_t \*sec, uint32_t \*usec)
              void cork_timestamp_round_nsec(const cork_timestamp ts, uint32_t \*sec, uint32_t \*nsec)

   Returns the seconds and fractional portions of a timestamp, with the
   fractional portion rounded to, respectively, milliseconds, microseconds, or
   nanoseconds.  (You must extract the seconds and fractional portions together,
   because rounding the fractional portion might involve adding a carry bit to
   the seconds portion.)


.. function:: int cork_timestamp_format_utc(const cork_timestamp ts, const char \*format, struct cork_buffer \*buf)
              int cork_timestamp_format_local(const cork_timestamp ts, const char \*format, struct cork_buffer \*buf)

   Create the string representation of the given timestamp according to
   *format*, appending the result to the current contents of *buf*.

   The ``_utc`` variant assumes that *ts* represents a UTC time, whereas the
   ``_local`` variant assumes that it represents a time in the local time zone.

   *format* is a format string whose syntax is similar to that of the POSIX
   ``strftime`` function.  *format* must contain arbitrary text interspersed
   with ``%`` specifiers, which will be replaced with portions of the timestamp.
   The following specifiers are recognized (note that this list does **not**
   include all of the specifiers supported by ``strftime``):

   ============== ====================================================
   Specifier      Replacement
   ============== ====================================================
   ``%%``         A literal ``%`` character
   ``%d``         Day of month (``01``-``31``)
   ``%H``         Hour in current day (``00``-``23``)
   ``%m``         Month (``01``-``12``)
   ``%M``         Minute in current hour (``00``-``59``)
   ``%[.width]s`` Number of seconds (and possibly fractional seconds)
                  since Unix epoch
   ``%[.width]S`` Second (and possibly fractional second) in current
                  minute (``00``-``60``)
   ``%Y``         Four-digit year (including century)
   ============== ====================================================

   For the ``%s`` and ``%S`` specifiers, you can also provide a ``width``.  If
   you do, then we will also include the subsecond fractional portion of the
   timestamp, rounded to the nearnest number of fractional digits.  For
   instance, if you want the number of milliseconds since the Unix epoch, you
   would use ``%.3s`` as your format specifier.  If you want the 24-hour time
   within the day, including microseconds, you would use `%H:%M:%.6S`.

   If the format string is invalid, we will return an :ref:`error condition
   <errors>`.
