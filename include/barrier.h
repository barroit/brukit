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

#define READ_ONCE(x)       __atomic_load_n(&(x), __ATOMIC_RELAXED)
#define WRITE_ONCE(x, val) __atomic_store_n(&(x), val, __ATOMIC_RELAXED)

#endif /* NG39_BARRIER_H */
