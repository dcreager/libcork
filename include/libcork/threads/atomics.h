/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef LIBCORK_THREADS_ATOMICS_H
#define LIBCORK_THREADS_ATOMICS_H

#include <libcork/config.h>
#include <libcork/core/types.h>

/*-----------------------------------------------------------------------
 * GCC intrinsics
 */

/* Ideally we can use GCC's intrinsics to define everything */
#if defined(CORK_CONFIG_HAVE_GCC_ATOMICS)

#define cork_int8_atomic_add    __sync_add_and_fetch
#define cork_int16_atomic_add   __sync_add_and_fetch
#define cork_int32_atomic_add   __sync_add_and_fetch
#define cork_int64_atomic_add   __sync_add_and_fetch
#define cork_uint8_atomic_add   __sync_add_and_fetch
#define cork_uint16_atomic_add  __sync_add_and_fetch
#define cork_uint32_atomic_add  __sync_add_and_fetch
#define cork_uint64_atomic_add  __sync_add_and_fetch

#define cork_int8_atomic_pre_add    __sync_fetch_and_add
#define cork_int16_atomic_pre_add   __sync_fetch_and_add
#define cork_int32_atomic_pre_add   __sync_fetch_and_add
#define cork_int64_atomic_pre_add   __sync_fetch_and_add
#define cork_uint8_atomic_pre_add   __sync_fetch_and_add
#define cork_uint16_atomic_pre_add  __sync_fetch_and_add
#define cork_uint32_atomic_pre_add  __sync_fetch_and_add
#define cork_uint64_atomic_pre_add  __sync_fetch_and_add

#define cork_int8_atomic_sub    __sync_sub_and_fetch
#define cork_int16_atomic_sub   __sync_sub_and_fetch
#define cork_int32_atomic_sub   __sync_sub_and_fetch
#define cork_int64_atomic_sub   __sync_sub_and_fetch
#define cork_uint8_atomic_sub   __sync_sub_and_fetch
#define cork_uint16_atomic_sub  __sync_sub_and_fetch
#define cork_uint32_atomic_sub  __sync_sub_and_fetch
#define cork_uint64_atomic_sub  __sync_sub_and_fetch

#define cork_int8_atomic_pre_sub    __sync_fetch_and_sub
#define cork_int16_atomic_pre_sub   __sync_fetch_and_sub
#define cork_int32_atomic_pre_sub   __sync_fetch_and_sub
#define cork_int64_atomic_pre_sub   __sync_fetch_and_sub
#define cork_uint8_atomic_pre_sub   __sync_fetch_and_sub
#define cork_uint16_atomic_pre_sub  __sync_fetch_and_sub
#define cork_uint32_atomic_pre_sub  __sync_fetch_and_sub
#define cork_uint64_atomic_pre_sub  __sync_fetch_and_sub

#define cork_int8_cas    __sync_val_compare_and_swap
#define cork_int16_cas   __sync_val_compare_and_swap
#define cork_int32_cas   __sync_val_compare_and_swap
#define cork_int64_cas   __sync_val_compare_and_swap
#define cork_uint8_cas   __sync_val_compare_and_swap
#define cork_uint16_cas  __sync_val_compare_and_swap
#define cork_uint32_cas  __sync_val_compare_and_swap
#define cork_uint64_cas  __sync_val_compare_and_swap
#define cork_ptr_cas     __sync_val_compare_and_swap


/*-----------------------------------------------------------------------
 * End of atomic implementations
 */
#else
#error "No atomics implementation!"
#endif


/*-----------------------------------------------------------------------
 * Implementation-agnostic atomics
 */

/* The following atomic definitions can be defined in terms of one of
 * the existing implementations. */

#if CORK_SIZEOF_SHORT == 1
#define cork_short_atomic_add       cork_int8_atomic_add
#define cork_short_atomic_pre_add   cork_int8_atomic_pre_add
#define cork_short_atomic_sub       cork_int8_atomic_sub
#define cork_short_atomic_pre_sub   cork_int8_atomic_pre_sub
#define cork_short_cas              cork_int8_cas
#define cork_ushort_atomic_add      cork_uint8_atomic_add
#define cork_ushort_atomic_pre_add  cork_uint8_atomic_pre_add
#define cork_ushort_atomic_sub      cork_uint8_atomic_sub
#define cork_ushort_atomic_pre_sub  cork_uint8_atomic_pre_sub
#define cork_ushort_cas             cork_uint8_cas
#elif CORK_SIZEOF_SHORT == 2
#define cork_short_atomic_add       cork_int16_atomic_add
#define cork_short_atomic_pre_add   cork_int16_atomic_pre_add
#define cork_short_atomic_sub       cork_int16_atomic_sub
#define cork_short_atomic_pre_sub   cork_int16_atomic_pre_sub
#define cork_short_cas              cork_int16_cas
#define cork_ushort_atomic_add      cork_uint16_atomic_add
#define cork_ushort_atomic_pre_add  cork_uint16_atomic_pre_add
#define cork_ushort_atomic_sub      cork_uint16_atomic_sub
#define cork_ushort_atomic_pre_sub  cork_uint16_atomic_pre_sub
#define cork_ushort_cas             cork_uint16_cas
#elif CORK_SIZEOF_SHORT == 4
#define cork_short_atomic_add       cork_int32_atomic_add
#define cork_short_atomic_pre_add   cork_int32_atomic_pre_add
#define cork_short_atomic_sub       cork_int32_atomic_sub
#define cork_short_atomic_pre_sub   cork_int32_atomic_pre_sub
#define cork_short_cas              cork_int32_cas
#define cork_ushort_atomic_add      cork_uint32_atomic_add
#define cork_ushort_atomic_pre_add  cork_uint32_atomic_pre_add
#define cork_ushort_atomic_sub      cork_uint32_atomic_sub
#define cork_ushort_atomic_pre_sub  cork_uint32_atomic_pre_sub
#define cork_ushort_cas             cork_uint32_cas
#elif CORK_SIZEOF_SHORT == 8
#define cork_short_atomic_add       cork_int64_atomic_add
#define cork_short_atomic_pre_add   cork_int64_atomic_pre_add
#define cork_short_atomic_sub       cork_int64_atomic_sub
#define cork_short_atomic_pre_sub   cork_int64_atomic_pre_sub
#define cork_short_cas              cork_int64_cas
#define cork_ushort_atomic_add      cork_uint64_atomic_add
#define cork_ushort_atomic_pre_add  cork_uint64_atomic_pre_add
#define cork_ushort_atomic_sub      cork_uint64_atomic_sub
#define cork_ushort_atomic_pre_sub  cork_uint64_atomic_pre_sub
#define cork_ushort_cas             cork_uint64_cas
#endif

#if CORK_SIZEOF_INT == 1
#define cork_int_atomic_add       cork_int8_atomic_add
#define cork_int_atomic_pre_add   cork_int8_atomic_pre_add
#define cork_int_atomic_sub       cork_int8_atomic_sub
#define cork_int_atomic_pre_sub   cork_int8_atomic_pre_sub
#define cork_int_cas              cork_int8_cas
#define cork_uint_atomic_add      cork_uint8_atomic_add
#define cork_uint_atomic_pre_add  cork_uint8_atomic_pre_add
#define cork_uint_atomic_sub      cork_uint8_atomic_sub
#define cork_uint_atomic_pre_sub  cork_uint8_atomic_pre_sub
#define cork_uint_cas             cork_uint8_cas
#elif CORK_SIZEOF_INT == 2
#define cork_int_atomic_add       cork_int16_atomic_add
#define cork_int_atomic_pre_add   cork_int16_atomic_pre_add
#define cork_int_atomic_sub       cork_int16_atomic_sub
#define cork_int_atomic_pre_sub   cork_int16_atomic_pre_sub
#define cork_int_cas              cork_int16_cas
#define cork_uint_atomic_add      cork_uint16_atomic_add
#define cork_uint_atomic_pre_add  cork_uint16_atomic_pre_add
#define cork_uint_atomic_sub      cork_uint16_atomic_sub
#define cork_uint_atomic_pre_sub  cork_uint16_atomic_pre_sub
#define cork_uint_cas             cork_uint16_cas
#elif CORK_SIZEOF_INT == 4
#define cork_int_atomic_add       cork_int32_atomic_add
#define cork_int_atomic_pre_add   cork_int32_atomic_pre_add
#define cork_int_atomic_sub       cork_int32_atomic_sub
#define cork_int_atomic_pre_sub   cork_int32_atomic_pre_sub
#define cork_int_cas              cork_int32_cas
#define cork_uint_atomic_add      cork_uint32_atomic_add
#define cork_uint_atomic_pre_add  cork_uint32_atomic_pre_add
#define cork_uint_atomic_sub      cork_uint32_atomic_sub
#define cork_uint_atomic_pre_sub  cork_uint32_atomic_pre_sub
#define cork_uint_cas             cork_uint32_cas
#elif CORK_SIZEOF_INT == 8
#define cork_int_atomic_add       cork_int64_atomic_add
#define cork_int_atomic_pre_add   cork_int64_atomic_pre_add
#define cork_int_atomic_sub       cork_int64_atomic_sub
#define cork_int_atomic_pre_sub   cork_int64_atomic_pre_sub
#define cork_int_cas              cork_int64_cas
#define cork_uint_atomic_add      cork_uint64_atomic_add
#define cork_uint_atomic_pre_add  cork_uint64_atomic_pre_add
#define cork_uint_atomic_sub      cork_uint64_atomic_sub
#define cork_uint_atomic_pre_sub  cork_uint64_atomic_pre_sub
#define cork_uint_cas             cork_uint64_cas
#endif

#if CORK_SIZEOF_LONG == 1
#define cork_long_atomic_add       cork_int8_atomic_add
#define cork_long_atomic_pre_add   cork_int8_atomic_pre_add
#define cork_long_atomic_sub       cork_int8_atomic_sub
#define cork_long_atomic_pre_sub   cork_int8_atomic_pre_sub
#define cork_long_cas              cork_int8_cas
#define cork_ulong_atomic_add      cork_uint8_atomic_add
#define cork_ulong_atomic_pre_add  cork_uint8_atomic_pre_add
#define cork_ulong_atomic_sub      cork_uint8_atomic_sub
#define cork_ulong_atomic_pre_sub  cork_uint8_atomic_pre_sub
#define cork_ulong_cas             cork_uint8_cas
#elif CORK_SIZEOF_LONG == 2
#define cork_long_atomic_add       cork_int16_atomic_add
#define cork_long_atomic_pre_add   cork_int16_atomic_pre_add
#define cork_long_atomic_sub       cork_int16_atomic_sub
#define cork_long_atomic_pre_sub   cork_int16_atomic_pre_sub
#define cork_long_cas              cork_int16_cas
#define cork_ulong_atomic_add      cork_uint16_atomic_add
#define cork_ulong_atomic_pre_add  cork_uint16_atomic_pre_add
#define cork_ulong_atomic_sub      cork_uint16_atomic_sub
#define cork_ulong_atomic_pre_sub  cork_uint16_atomic_pre_sub
#define cork_ulong_cas             cork_uint16_cas
#elif CORK_SIZEOF_LONG == 4
#define cork_long_atomic_add       cork_int32_atomic_add
#define cork_long_atomic_pre_add   cork_int32_atomic_pre_add
#define cork_long_atomic_sub       cork_int32_atomic_sub
#define cork_long_atomic_pre_sub   cork_int32_atomic_pre_sub
#define cork_long_cas              cork_int32_cas
#define cork_ulong_atomic_add      cork_uint32_atomic_add
#define cork_ulong_atomic_pre_add  cork_uint32_atomic_pre_add
#define cork_ulong_atomic_sub      cork_uint32_atomic_sub
#define cork_ulong_atomic_pre_sub  cork_uint32_atomic_pre_sub
#define cork_ulong_cas             cork_uint32_cas
#elif CORK_SIZEOF_LONG == 8
#define cork_long_atomic_add       cork_int64_atomic_add
#define cork_long_atomic_pre_add   cork_int64_atomic_pre_add
#define cork_long_atomic_sub       cork_int64_atomic_sub
#define cork_long_atomic_pre_sub   cork_int64_atomic_pre_sub
#define cork_long_cas              cork_int64_cas
#define cork_ulong_atomic_add      cork_uint64_atomic_add
#define cork_ulong_atomic_pre_add  cork_uint64_atomic_pre_add
#define cork_ulong_atomic_sub      cork_uint64_atomic_sub
#define cork_ulong_atomic_pre_sub  cork_uint64_atomic_pre_sub
#define cork_ulong_cas             cork_uint64_cas
#endif


#endif /* LIBCORK_THREADS_ATOMICS_H */
