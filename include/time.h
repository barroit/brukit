/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_TIME_H
#define NG39_TIME_H

#include_next <time.h>

typedef uint64_t timestamp_t;

timestamp_t __timestamp(void);

#ifdef CONFIG_TS_SMALL_UNIT
# define timestamp (__timestamp() / (1000 * 1000))
#else
# define timestamp (__timestamp())
#endif

void monotime(struct timespec *ts);

#endif /* NG39_TIME_H */
