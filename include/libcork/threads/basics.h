/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_THREADS_BASICS_H
#define LIBCORK_THREADS_BASICS_H

#include <assert.h>

#include <libcork/core/attributes.h>
#include <libcork/threads/atomics.h>


#if CORK_CONFIG_HAVE_GCC_ASM && (CORK_CONFIG_ARCH_X86 || CORK_CONFIG_ARCH_X64)
#define cork_pause() \
    do { \
        __asm__ __volatile__ ("pause"); \
    } while (0)
#else
#define cork_pause()  do { /* do nothing */ } while (0)
#endif


#define cork_once_barrier(name) \
    static volatile int  name = 0;

#define cork_once(barrier, call) \
    do { \
        /* Try to claim the ability to perform the initialization */ \
        int  prior_state = cork_int_cas(&barrier, 0, 1); \
        if (CORK_LIKELY(prior_state == 2)) { \
            /* already initialized */ \
        } else if (CORK_UNLIKELY(prior_state == 1)) { \
            /* someone else is initializing, spin/wait until done */ \
            while (barrier != 2) { cork_pause(); } \
        } else { \
            /* we get to initialize */ \
            call; \
            assert(cork_int_cas(&barrier, 1, 2) == 1); \
        } \
    } while (0)


#endif /* LIBCORK_THREADS_BASICS_H */
