/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, libcork authors
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_CONFIG_GCC_H
#define LIBCORK_CONFIG_GCC_H

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

/* The GCC assembly syntax has been available basically forever. */

#if !defined(CORK_CONFIG_HAVE_GCC_ASM)
#if defined(CORK_CONFIG_GCC_VERSION)
#define CORK_CONFIG_HAVE_GCC_ASM  1
#else
#define CORK_CONFIG_HAVE_GCC_ASM  0
#endif
#endif

/* The GCC atomic instrinsics are available as of GCC 4.1.0. */

#if !defined(CORK_CONFIG_HAVE_GCC_ATOMICS)
#if CORK_CONFIG_GCC_VERSION >= 40100
#define CORK_CONFIG_HAVE_GCC_ATOMICS  1
#else
#define CORK_CONFIG_HAVE_GCC_ATOMICS  0
#endif
#endif

/* The attributes we want to use are available as of GCC 2.96. */

#if !defined(CORK_CONFIG_HAVE_GCC_ATTRIBUTES)
#if CORK_CONFIG_GCC_VERSION >= 29600
#define CORK_CONFIG_HAVE_GCC_ATTRIBUTES  1
#else
#define CORK_CONFIG_HAVE_GCC_ATTRIBUTES  0
#endif
#endif

/* C99 inline is available if we're compiling in C99 mode. */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define CORK_CONFIG_HAVE_C99_INLINE  1
#else
#define CORK_CONFIG_HAVE_C99_INLINE  0
#endif

/* __int128 seems to be available on 64-bit platforms as of GCC 4.6.  The
 * attribute((mode(TI))) syntax seems to be available as of 4.1. */

#if !defined(CORK_CONFIG_HAVE_GCC_INT128)
#if CORK_CONFIG_ARCH_X64 && CORK_CONFIG_GCC_VERSION >= 40600
#define CORK_CONFIG_HAVE_GCC_INT128  1
#else
#define CORK_CONFIG_HAVE_GCC_INT128  0
#endif
#endif

#if !defined(CORK_CONFIG_HAVE_GCC_MODE_ATTRIBUTE)
#if CORK_CONFIG_ARCH_X64 && CORK_CONFIG_GCC_VERSION >= 40100
#define CORK_CONFIG_HAVE_GCC_MODE_ATTRIBUTE  1
#else
#define CORK_CONFIG_HAVE_GCC_MODE_ATTRIBUTE  0
#endif
#endif

/* Statement expressions have been available since GCC 3.1. */

#if !defined(CORK_CONFIG_HAVE_GCC_STATEMENT_EXPRS)
#if CORK_CONFIG_GCC_VERSION >= 30100
#define CORK_CONFIG_HAVE_GCC_STATEMENT_EXPRS  1
#else
#define CORK_CONFIG_HAVE_GCC_STATEMENT_EXPRS  0
#endif
#endif

/* Thread-local storage has been available since GCC 3.3, but not on Mac
 * OS X. */

#if !defined(CORK_CONFIG_HAVE_THREAD_STORAGE_CLASS)
#if !(defined(__APPLE__) && defined(__MACH__))
#if CORK_CONFIG_GCC_VERSION >= 30300
#define CORK_CONFIG_HAVE_THREAD_STORAGE_CLASS  1
#else
#define CORK_CONFIG_HAVE_THREAD_STORAGE_CLASS  0
#endif
#else
#define CORK_CONFIG_HAVE_THREAD_STORAGE_CLASS  0
#endif
#endif


#endif /* LIBCORK_CONFIG_GCC_H */
