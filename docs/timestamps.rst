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
              void cork_timestamp_init_msec(cork_timestamp \*ts, uint32_t sec, uint32_t msec)
              void cork_timestamp_init_usec(cork_timestamp \*ts, uint32_t sec, uint32_t usec)

   Initializes a timestamp from a separate seconds part and fractional
   part.  For the ``_sec`` variant, the fractional part will be set to
   ``0``.  For the ``_msec`` and ``_usec`` variants, the fractional part
   will be translated into gammaseconds from milliseconds or
   microseconds, respectively.


.. function:: void cork_timestamp_init_now(cork_timestamp \*ts)

   Initializes a timestamp with the current UTC time of day.

   .. note::

      The resolution of this function is system-dependent.


.. function:: uint32_t cork_timestamp_sec(const cork_timestamp ts)
              uint32_t cork_timestamp_gsec(const cork_timestamp ts)

   Returns the seconds or fractional portion, respectively, of a
   timestamp.  The fractional portion is represented in gammaseconds.


.. function:: bool cork_timestamp_format_utc(const cork_timestamp ts, const char \*format, char \*buf, size_t size)
              bool cork_timestamp_format_local(const cork_timestamp ts, const char \*format, char \*buf, size_t size)

   Fills in *buf* with the string representation of the given timestamp,
   according to *fmt*, which should be a format string compatible with
   the POSIX ``strftime`` function.  *size* must be the size (in bytes)
   of *buf*.  If we can't format the timestamp for any reason, we return
   ``false``.  The ``_utc`` variant assumes that *ts* represents a UTC
   time, whereas teh ``_local`` variant assumes that it represents a
   time in the local time zone.
