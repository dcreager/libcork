/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CONFIG_MACOSX_H
#define LIBCORK_CONFIG_MACOSX_H

/**
 * @file
 * @brief Mac OS X-specific configuration auto-detection
 */

#if !defined(CORK_DOCUMENTATION)


/*-----------------------------------------------------------------------
 * Endianness
 */

#include <machine/endian.h>

#if BYTE_ORDER == BIG_ENDIAN
#define CORK_CONFIG_IS_BIG_ENDIAN      1
#define CORK_CONFIG_IS_LITTLE_ENDIAN   0
#elif BYTE_ORDER == LITTLE_ENDIAN
#define CORK_CONFIG_IS_BIG_ENDIAN      0
#define CORK_CONFIG_IS_LITTLE_ENDIAN   1
#else
#error "Cannot determine system endianness"
#endif


#endif /* !CORK_DOCUMENTATION */
#endif /* LIBCORK_CONFIG_MACOSX_H */
