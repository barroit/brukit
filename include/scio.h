/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 *
 * SC stands for syscall
 */

#ifndef NG39_SCIO_H
#define NG39_SCIO_H

/*
 * The functions in this file are mainly for handling retries on errors like
 * EINTR. The prefix 'x' here does not stand for "die on failure"; it's just
 * a character used to distinguish the syscall version of the functions.
 */

ssize_t xwrite(int fd, const void *buf, size_t count) __read_only(2, 3);

ssize_t xread(int fd, void *buf, size_t count);

#endif /* NG39_SCIO_H */
