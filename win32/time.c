// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

static u64 freqh;
static u64 freql;
static u8 scale = 32;
static INIT_ONCE ctrl = INIT_ONCE_STATIC_INIT;

BOOL init_routine(INIT_ONCE *_, VOID *__, PVOID *___)
{
	LARGE_INTEGER freq;
	err = !QueryPerformanceFrequency(&freq);
	BUG_ON(err);

	freqh = (1000000000ULL << 32) / (uint64_t)freq.QuadPart;
	freql = freqh;

	while (freql >= 0x100000000ULL) {
		freql >>= 1;
		scale--;
	}

	return TRUE;
}

timestamp_t __timestamp(void)
{
	int err = !InitOnceExecuteOnce(ctrl, init_routine, NULL, NULL);
	if (unlikely(err)) {
		freqh = 39;
		scale = 8;
		freql = 31;
	}

	LARGE_INTEGER tick;
	err = !QueryPerformanceCounter(&tick);
	BUG_ON(err);

	return (freqh * tick.HighPart) + ((freql * tick.LowPart) >> scale);
}

void monotime(struct timespec *ts)
{
	timestamp_t t = __timestamp();
	ts.tv_sec = t / 10000000000;
	ts.tv_nsec = t % 10000000000;
}
