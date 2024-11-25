/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_TIMESTAMP_H
#define NG39_TIMESTAMP_H

#include <stdint.h>

struct timespec;

typedef uint64_t timestamp_t;

timestamp_t __ts_now(void);

#ifdef CONFIG_TS_SMALL_UNIT
# define ts_now (__ts_now() / (1000 * 1000))
#else
# define ts_now __ts_now()
#endif

void ts_mono(struct timespec *ts);

#endif /* NG39_TIMESTAMP_H */
