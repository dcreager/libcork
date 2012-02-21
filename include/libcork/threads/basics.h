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


/*-----------------------------------------------------------------------
 * Executing something once
 */

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
        if (CORK_LIKELY(barrier == 2)) { \
            /* already initialized */ \
        } else { \
            /* Try to claim the ability to perform the initialization */ \
            int  prior_state = cork_int_cas(&barrier, 0, 1); \
            if (CORK_LIKELY(prior_state == 0)) { \
                /* we get to initialize */ \
                call; \
                assert(cork_int_cas(&barrier, 1, 2) == 1); \
            } else { \
                /* someone else is initializing, spin/wait until done */ \
                while (barrier != 2) { cork_pause(); } \
            } \
        } \
    } while (0)


/*-----------------------------------------------------------------------
 * Thread-local storage
 */

/* Prefer, in order:
 *
 * 1) __thread storage class
 * 2) pthread_key_t
 */

#if CORK_CONFIG_HAVE_THREAD_STORAGE_CLASS
#define cork_tls(TYPE, NAME) \
static __thread TYPE  NAME##__tls; \
\
static TYPE * \
NAME##_get(void) \
{ \
    return &NAME##__tls; \
}

#elif CORK_HAVE_PTHREADS
#include <pthread.h>

#define cork_tls(TYPE, NAME) \
static pthread_key_t  NAME##__tls_key; \
cork_once_barrier(NAME##__tls_barrier); \
\
static void \
NAME##__tls_destroy(void *vself) \
{ \
    free(vself); \
} \
\
static void \
NAME##__tls_init_key(void) \
{ \
    cork_once \
        (NAME##__tls_barrier, \
         assert(pthread_key_create(&NAME##__tls_key, &NAME##__tls_destroy) \
                == 0)); \
} \
\
static TYPE * \
NAME##_get(void) \
{ \
    TYPE  *self; \
    NAME##__tls_init_key(); \
    self = pthread_getspecific(NAME##__tls_key); \
    if (CORK_UNLIKELY(self == NULL)) { \
        self = cork_calloc(1, sizeof(TYPE)); \
        pthread_setspecific(NAME##__tls_key, self); \
    } \
    return self; \
}

#else
#error "No thread-local storage implementation!"
#endif


#endif /* LIBCORK_THREADS_BASICS_H */
