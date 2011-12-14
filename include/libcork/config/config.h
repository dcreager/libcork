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


/* If you want to skip autodetection, define this to 1, and provide a
 * libcork/config/custom.h header file. */

#if !defined(CORK_CONFIG_SKIP_AUTODETECT)
#define CORK_CONFIG_SKIP_AUTODETECT  0
#endif


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


#endif /* LIBCORK_CONFIG_CONFIG_H */
