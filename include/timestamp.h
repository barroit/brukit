/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_TIMESTAMP_H
#define NG39_TIMESTAMP_H

#include <stdint.h>
#include <time.h>

typedef uint64_t timestamp_t;

timestamp_t ts_now(void);

void ts_mono(struct timespec *ts);

#endif /* NG39_TIMESTAMP_H */
