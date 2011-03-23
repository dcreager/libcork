/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CONFIG_GCC_H
#define LIBCORK_CONFIG_GCC_H

/**
 * @file
 * @brief GCC-specific configuration auto-detection
 */

#if !defined(CORK_DOCUMENTATION)

/* Figure out the GCC version */

#if defined(__GNUC_PATCHLEVEL__)
#define CORK_CONFIG_GCC_VERSION (__GNUC__ * 10000 \
                               + __GNUC_MINOR__ * 100 \
                               + __GNUC_PATCHLEVEL__)
#else
#define CORK_CONFIG_GCC_VERSION (__GNUC__ * 10000 \
                               + __GNUC_MINOR__ * 100)
#endif


/*-----------------------------------------------------------------------
 * Compiler attributes
 */

/*
 * The attributes we want to use are available as of GCC 2.96.
 */

#if CORK_CONFIG_GCC_VERSION >= 29600
#define CORK_CONFIG_HAVE_GCC_ATTRIBUTES  1
#else
#define CORK_CONFIG_HAVE_GCC_ATTRIBUTES  0
#endif


#endif /* !CORK_DOCUMENTATION */
#endif /* LIBCORK_CONFIG_GCC_H */
