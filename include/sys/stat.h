/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_SYS_STAT_H
#define NG39_SYS_STAT_H

#include_next <sys/stat.h>

#ifdef _WIN32
# define S_IFLNK 0xa000
# if (S_IFLNK & S_IFMT) != S_IFLNK
#  error (S_IFLNK & S_IFMT) != S_IFLNK
# endif
# define S_ISLNK(m) (((m) & S_IFMT) == S_IFREG)
#endif

#endif /* NG39_SYS_STAT_H */
