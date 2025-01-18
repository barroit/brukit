// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "rio.h"

#include <errno.h>
#include <unistd.h>

#ifdef _WIN32
# include <io.h>
# define read  _read
# define write _write
#endif

#include "compiler.h"

ssize_t rread(int fd, void *buf, size_t count)
{
	ssize_t nr;

retry:
	nr = read(fd, buf, count);

	if (unlikely(nr < 0 && errno == EINTR))
		goto retry;

	return nr;
}


ssize_t rwrite(int fd, const void *buf, size_t count)
{
	ssize_t nr;

retry:
	nr = write(fd, buf, count);

	if (unlikely(nr < 0 && errno == EINTR))
		goto retry;

	return nr;
}
