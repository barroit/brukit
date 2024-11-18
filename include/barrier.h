/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 *
 * For details on how memory barriers work, take a look at the linux kernel's
 * Documentation/memory-barriers.txt. Although it's related to the kernel, the
 * essential mechanisms are interoperable.
 */

#ifndef NG39_BARRIER_H
#define NG39_BARRIER_H

#define READ_ONCE(x)     __atomic_load_n(x, __ATOMIC_RELAXED)
#define WRITE_ONCE(x, v) __atomic_store_n(x, v, __ATOMIC_RELAXED)

#define smp_load_acquire(x)     __atomic_load_n(x, __ATOMIC_ACQUIRE)
#define smp_store_release(x, v) __atomic_store_n(x, v, __ATOMIC_RELEASE)

#define smp_inc_return(x)    smp_add_return(x, 1)
#define smp_add_return(x, v) __atomic_add_fetch(x, v, __ATOMIC_ACQ_REL)

#define smp_dec_return(x)    smp_sub_return(x, 1)
#define smp_sub_return(x, v) __atomic_sub_fetch(x, v, __ATOMIC_ACQ_REL)

#define smp_test_and_set(x) __atomic_test_and_set(x, __ATOMIC_ACQ_REL)
#define smp_clear(x)        __atomic_clear(x, __ATOMIC_RELEASE)

#define smp_mb()  __atomic_thread_fence(__ATOMIC_ACQ_REL)
#define smp_rmb() __atomic_thread_fence(__ATOMIC_ACQUIRE)
#define smp_wmb() __atomic_thread_fence(__ATOMIC_RELEASE)

#endif /* NG39_BARRIER_H */
