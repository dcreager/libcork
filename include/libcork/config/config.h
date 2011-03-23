/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CONFIG_CONFIG_H
#define LIBCORK_CONFIG_CONFIG_H

/**
 * @file
 * @brief Implementation of the @ref config module
 */


/**
 * @addtogroup config
 *
 * <tt>#%include \<libcork/config.h\></tt>
 *
 * Several libcork features have different implementations on different
 * platforms.  Since we want libcork to be easily embeddable into
 * projects with a wide range of build systems, we try to autodetect
 * which implementations to use, strictly using the C preprocessor, and
 * the predefined macros that are available on different systems.
 *
 * This module provides a layer of indirection, encapsulating all of the
 * preprocessor-based autodetection into one place.  The goal of this
 * module is to define a collection of libcork-specific configuration
 * macros, which all other libcork modules will use to select which
 * implementation to use.
 *
 * If you want to skip libcork's autodetection, you can; see @ref
 * CORK_CONFIG_SKIP_AUTODETECT for details.
 *
 * @{
 */


/*-----------------------------------------------------------------------
 * Skipping autodetection
 */

/**
 * @brief Whether to skip libcork's autodetection logic
 *
 * If you want to skip libcork's autodetection logic, then you are
 * responsible for providing the appropriate values for all of the
 * macros defined in this section.  To do this, have your build system
 * define this macro, with a value of 1.  This will override the default
 * value of 0 provided in the libcork/config/config.h header file.
 *
 * Then, create a <tt>libcork/config/custom.h</tt> header file.  You can
 * place this file anywhere in your header search path.  We will load
 * that file instead of libcork's autodetection logic.  Place the
 * appropriate definitions for each of the configuration macros into
 * this file.  If needed, you can generate this file as part of the
 * “configure” step of your build system; the only requirement is that
 * it's available once you start compiling the libcork source files.
 *
 * @showinitializer
 * @since 0.0-dev
 */

#if defined(CORK_DOCUMENTATION)
#define CORK_CONFIG_SKIP_AUTODETECT  0
#else
#if !defined(CORK_CONFIG_SKIP_AUTODETECT)
#define CORK_CONFIG_SKIP_AUTODETECT  0
#endif
#endif


/*-----------------------------------------------------------------------
 * Endianness
 */

/**
 * @brief Whether this is a big-endian system.
 * @since 0.0-dev
 */

#if defined(CORK_DOCUMENTATION)
#define CORK_CONFIG_IS_BIG_ENDIAN  0
#endif

/**
 * @brief Whether this is a little-endian system.
 * @since 0.0-dev
 */

#if defined(CORK_DOCUMENTATION)
#define CORK_CONFIG_IS_LITTLE_ENDIAN  0
#endif


/*-----------------------------------------------------------------------
 * Compiler attributes
 */

/**
 * @brief Whether the GCC compiler attributes are available.
 * @since 0.0-dev
 */

#if defined(CORK_DOCUMENTATION)
#define CORK_CONFIG_HAVE_GCC_ATTRIBUTES  0
#endif


/* end of config group */
/**
 * @}
 */

/*-----------------------------------------------------------------------
 * Auto-detection
 */

/* don't bother autodetecting if we're just building the docs */
#if !defined(CORK_DOCUMENTATION)

#if CORK_CONFIG_SKIP_AUTODETECT
/* The user has promised that they'll define everything themselves. */
#include <libcork/config/custom.h>

#else
/* Otherwise autodetect! */


/**** PLATFORMS ****/

#if defined(__linux)
/* Do some Linux-specific autodetection. */
#include <libcork/config/linux.h>

#elif defined(__APPLE__) && defined(__MACH__)
/* Do some Mac OS X-specific autodetection. */
#include <libcork/config/macosx.h>

#endif  /* platforms */


/**** COMPILERS ****/

#if defined(__GNUC__)
/* Do some GCC-specific autodetection. */
#include <libcork/config/gcc.h>

#endif  /* compilers */


#endif  /* autodetect or not */
#endif  /* documentation check */


#endif /* LIBCORK_CONFIG_CONFIG_H */
