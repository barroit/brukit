// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "scio.h"

#include <errno.h>
#include <unistd.h>

#include "compiler.h"

ssize_t xwrite(int fd, const void *buf, size_t count)
{
	int nr;

retry:
	nr = write(fd, buf, count);

	if (unlikely(nr == -1 && errno == EINTR))
		goto retry;

	return nr;
}

ssize_t xread(int fd, void *buf, size_t count)
{
	int nr;

retry:
	nr = read(fd, buf, count);

	if (unlikely(nr == -1 && errno == EINTR))
		goto retry;

	return nr;
}
