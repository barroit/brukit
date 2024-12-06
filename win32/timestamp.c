// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "timestamp.h"

#include <windows.h>

#include "calc.h"
#include "types.h"

timestamp_t ts_now(void)
{
	int err;
	static u64 freqh;
	static u64 freql;
	static u8 scale = maxof(scale);
	LARGE_INTEGER tick;

	if (scale == maxof(scale)) {
		LARGE_INTEGER freq;

		err = !QueryPerformanceFrequency(&freq);
		BUG_ON(err);

		freqh = (1000000000ULL << 32) / (uint64_t)freq.QuadPart;
		freql = freqh;
		scale = 32;

		while (freql >= 0x100000000ULL) {
			freql >>= 1;
			scale--;
		}
	}

	err = !QueryPerformanceCounter(&tick);
	BUG_ON(err);

	return (freqh * tick.HighPart) + ((freql * tick.LowPart) >> scale);
}

void ts_mono(struct timespec *ts)
{
	timestamp_t t = ts_now();

	ts->tv_sec = t / 10000000000;
	ts->tv_nsec = t % 10000000000;
}
