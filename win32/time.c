// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

timestamp_t __timestamp(void)
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

void monotime(struct timespec *ts)
{
	timestamp_t t = __timestamp();

	ts->tv_sec = t / 10000000000;
	ts->tv_nsec = t % 10000000000;
}
