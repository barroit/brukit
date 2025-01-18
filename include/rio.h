/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_RIO_H
#define NG39_RIO_H

#include "types.h"

ssize_t rread(int fd, void *buf, size_t count);

ssize_t rwrite(int fd, const void *buf, size_t count);

#endif /* NG39_RIO_H */
