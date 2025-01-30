/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_FITER_H
#define NG39_FITER_H

#include <sys/stat.h>

#include "bitopt.h"
#include "types.h"

#ifdef _WIN32
# define stat _stat
#endif

struct strbuf;
struct strlist;

struct file {
	const xchar *path;
	const xchar *subp;
	const xchar *name;

	int fd;
	struct stat st;
};

/*
 * NB: If you break the rules, such as using FI_USE_STAT with
 *     FI__RECUR_DIR_ONLY, FI_USE_STAT will simply be a no-op.
 *
 * FI_USE_STAT
 *	Make the st member available. Only works for regular files and
 *	unsupported files. This impacts performance on Windows, avoid using
 *	it there unless absolutely necessary.
 *
 * FI_USE_FD
 *	Make the fd member available. This is not available on Windows, as it
 *	makes performance down significantly.
 *
 * FI_LIST_DIR
 *	List directories.
 *
 * FI_RECUR_DIR
 *	List directories recursively, useful for implementing recursive
 *	directory deletion.
 *
 * FI_NO_UNK
 *	Do not list unsupported files.
 *
 * FI_NO_LNK
 *	Do not list symbolic links.
 *
 * FI_NO_REG
 *	Do not list regular files.
 */
#define FI_USE_STAT  (1U << 0)
#define FI_USE_FD    (1U << 1)

#define FI_LIST_DIR  (1U << 2)
#define FI_RECUR_DIR (1U << 3)

#define FI_NO_UNK    (1U << 4)
#define FI_NO_LNK    (1U << 5)
#define FI_NO_REG    (1U << 6)

#define __FI_DIR_ONLY      (FI_NO_UNK | FI_NO_LNK | FI_NO_REG)
#define FI__LIST_DIR_ONLY  (FI_LIST_DIR | __FI_DIR_ONLY)
#define FI__RECUR_DIR_ONLY (FI_RECUR_DIR | __FI_DIR_ONLY)

typedef int (*fiter_entry_cb)(struct file *f, void *data);

struct __fiter {
	const xchar *root;

	fiter_entry_cb cb;
	void *data;

	u32 flags;

	struct strbuf *sb;
	struct strlist *sl;
};

static inline int __fiter_is_list_dir_only(u32 flags)
{
	return match_bit(flags, FI_NO_UNK | FI_NO_LNK | FI_NO_REG);
}

int fiter(const xchar *root, fiter_entry_cb cb, void *data, u32 flags);

int __fiter_loop_dir(struct __fiter *ctx);

#endif /* NG39_FITER_H */
