// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "timestamp.h"

#include <windows.h>

#include <stdio.h>

#include "calc.h"
#include "types.h"

timestamp_t ts_now(void)
{
	int err;
	static u64 tps[2];
	static u8 scale = maxof(scale);
	LARGE_INTEGER tick;

	if (scale == maxof(scale)) {
		LARGE_INTEGER __tps;

		err = !QueryPerformanceFrequency(&__tps);
		BUG_ON(err);

		tps[0] = (1000000000ULL << 32) / (u64)__tps.QuadPart;
		tps[1] = tps[0];
		scale = 32;

		while (tps[1] >= 0x100000000ULL) {
			tps[1] >>= 1;
			scale--;
		}
	}

	err = !QueryPerformanceCounter(&tick);
	BUG_ON(err);

	timestamp_t t1 = tps[0] * tick.HighPart;
	timestamp_t t2 = (tps[1] * tick.LowPart) >> scale;

	return t1 + t2;
}

void ts_mono(struct timespec *ts)
{
	timestamp_t t = ts_now();

	ts->tv_sec = t / 1000000000LL;
	ts->tv_nsec = t % 1000000000LL;
}
