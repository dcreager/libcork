.. _config:

*******************
Configuring libcork
*******************

.. highlight:: c

::

  #include <libcork/config.h>

Several libcork features have different implementations on different
platforms.  Since we want libcork to be easily embeddable into projects
with a wide range of build systems, we try to autodetect which
implementations to use, using only the C preprocessor and the predefined
macros that are available on the current system.

This module provides a layer of indirection, with all of the
preprocessor-based autodetection in one place.  This module's task is to
define a collection of libcork-specific configuration macros, which all
other libcork modules will use to select which implementation to use.

This design also lets you skip the autodetection, and provide values for
the configuration macros directly.  This is especially useful if you're
embedding libcork into another project, and already have a ``configure``
step in your build system that performs platform detection.  See
:c:macro:`CORK_CONFIG_SKIP_AUTODETECT` for details.

.. note::

   The autodetection logic is almost certainly incomplete.  If you need
   to port libcork to another platform, this is where an important chunk
   of edits will take place.  Patches are welcome!


.. _configuration-macros:

Configuration macros
====================

This section lists all of the macros that are defined by libcork's
autodetection logic.  Other libcork modules will use the values of these
macros to choose among the possible implementations.

.. macro:: CORK_CONFIG_IS_BIG_ENDIAN
           CORK_CONFIG_IS_LITTLE_ENDIAN

   Whether the current system is big-endian or little-endian.  Exactly
   one of these macros should be defined to ``1``; the other should be
   defined to ``0``.


.. macro:: CORK_CONFIG_HAVE_GCC_ATTRIBUTES

   Whether the GCC-style syntax for compiler attributes is available.
   (This doesn't imply that the compiler is specifically GCC.)  Should
   be defined to ``0`` or ``1``.


.. macro:: CORK_CONFIG_HAVE_REALLOCF

   Whether this platform defines a ``reallocf`` function in
   ``stdlib.h``.  ``reallocf`` is a BSD extension that frees the
   existing pointer if a reallocation fails.  If this function exists,
   we can use it to implement :func:`cork_realloc`.


.. _skipping-autodetection:

Skipping autodetection
======================


.. macro:: CORK_CONFIG_SKIP_AUTODETECT

   If you want to skip libcork's autodetection logic, then you are
   responsible for providing the appropriate values for all of the
   macros defined in :ref:`configuration-macros`.  To do this, have your
   build system define this macro, with a value of ``1``.  This will
   override the default value of ``0`` provided in the
   ``libcork/config/config.h`` header file.

   Then, create (or have your build system create) a
   ``libcork/config/custom.h`` header file.  You can place this file
   anywhere in your header search path.  We will load that file instead
   of libcork's autodetection logic.  Place the appropriate definitions
   for each of the configuration macros into this file.  If needed, you
   can generate this file as part of the ``configure`` step of your
   build system; the only requirement is that it's available once you
   start compiling the libcork source files.
