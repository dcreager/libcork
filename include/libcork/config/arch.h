/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CONFIG_ARCH_H
#define LIBCORK_CONFIG_ARCH_H


/*-----------------------------------------------------------------------
 * Platform
 */

#if defined(__i386__) || defined(_M_IX86)
#define CORK_CONFIG_ARCH_X86  1
#else
#define CORK_CONFIG_ARCH_X86  0
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define CORK_CONFIG_ARCH_X64  1
#else
#define CORK_CONFIG_ARCH_X64  0
#endif

#if defined(__ppc__) || defined(_M_PPC)
#define CORK_CONFIG_ARCH_PPC  1
#else
#define CORK_CONFIG_ARCH_PPC  0
#endif


#endif /* LIBCORK_CONFIG_ARCH_H */
