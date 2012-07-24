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

   Returns the seconds portion of a timestamp.

.. function:: uint32_t cork_timestamp_gsec(const cork_timestamp ts)
              uint32_t cork_timestamp_msec(const cork_timestamp ts)
              uint32_t cork_timestamp_usec(const cork_timestamp ts)
              uint32_t cork_timestamp_nsec(const cork_timestamp ts)

   Returns the fractional portion of a timestamp.  The variants return the
   fractional portion in, respectively, gammaseconds, milliseconds,
   microseconds, or nanoseconds.


.. function:: bool cork_timestamp_format_utc(const cork_timestamp ts, const char \*format, char \*buf, size_t size)
              bool cork_timestamp_format_local(const cork_timestamp ts, const char \*format, char \*buf, size_t size)

   Fills in *buf* with the string representation of the given timestamp,
   according to *fmt*, which should be a format string compatible with
   the POSIX ``strftime`` function.  *size* must be the size (in bytes)
   of *buf*.  If we can't format the timestamp for any reason, we return
   ``false``.  We assume *ts* represents a UTC time in both functions.


.. function:: bool cork_timestamp_format_iso8601_utc(const cork timestamp ts, char \*buf, size_t size)
              bool cork_timestamp_format_iso8601_local(const cork timestamp ts, char \*buf, size_t size)

   Fills in *buf* with the string representation of the given timestamp,
   according to the ISO 8601 compatible format ``YYYY-MM-DDThh:mm:ssZ``
   for UTC time and ``YYYY-MM-DDThh:mm:zz+/-hhmm`` for times in the local
   time zone. *size* must be the size (in bytes) of *buf*.  If we can't
   format the timestamp for any reason, we return ``false``.  We assume
   that *ts* represents a UTC time in both functions.
