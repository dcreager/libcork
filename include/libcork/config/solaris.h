/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2019, libcork authors
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CONFIG_SOLARIS_H
#define LIBCORK_CONFIG_SOLARIS_H

/*-----------------------------------------------------------------------
 * Endianness
 */

#include <sys/byteorder.h>

#ifdef _BIG_ENDIAN
#define CORK_CONFIG_IS_BIG_ENDIAN      1
#define CORK_CONFIG_IS_LITTLE_ENDIAN   0
#else
#define CORK_CONFIG_IS_BIG_ENDIAN      0
#define CORK_CONFIG_IS_LITTLE_ENDIAN   1
#endif

#define CORK_HAVE_REALLOCF  0
#define CORK_HAVE_PTHREADS  1

#endif /* LIBCORK_CONFIG_SOLARIS_H */
