// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

void monotime(struct timespec *ts)
{
	int err = clock_gettime(CLOCK_MONOTONIC, ts);
	BUG_ON(err);
}

timestamp_t __timestamp(void)
{
	struct timespec ts;

	monotime(&ts);
	return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}
