// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "fiter.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "gettext.h"
#include "path.h"
#include "strbuf.h"
#include "strlist.h"
#include "termas.h"

static int yield_lnk(struct __fiter *ctx, struct file *f)
{
	if (!(ctx->flags & FI_USE_STAT)) {
		int err = lstat(f->path, &f->st);

		if (unlikely(err))
			return warn_errno(_("failed to get status for link `%s'"),
					  f->path);
	}

	return ctx->cb(f, ctx->data);
}

static int yield_reg(struct __fiter *ctx, struct file *f)
{
	int ret;

	if (ctx->flags & FI_USE_FD) {
		f->fd = open(f->path, O_RDONLY);

		if (unlikely(f->fd == -1))
			return warn_errno(_("failed to open file `%s'"),
					  f->path);
	}

	if (ctx->flags & FI_USE_STAT) {
		if (ctx->flags & FI_USE_FD)
			ret = fstat(f->fd, &f->st);
		else
			ret = stat(f->path, &f->st);

		if (unlikely(ret)) {
			if (ctx->flags & FI_USE_FD) {
				int errnum = errno;

				close(f->fd);
				errno = errnum;
			}

			return warn_errno(_("failed to get status for file `%s'"),
					  f->path);
		}
	}

	ret = ctx->cb(f, ctx->data);
	if (ctx->flags & FI_USE_FD)
		close(f->fd);

	return ret;
}

static int yield_file(struct __fiter *ctx, struct dirent *ent)
{
	if (__fiter_is_list_dir_only(ctx->flags) && ent->d_type != DT_DIR)
		return 0;
	else if (ctx->flags & FI_NO_LNK && ent->d_type == DT_LNK)
		return 0;
	else if (ctx->flags & FI_NO_REG && ent->d_type == DT_REG)
		return 0;

	const char *name = ent->d_name;

	if (pth_is_dot(name))
		return 0;
	sb_pth_legacy_append(ctx->sb, name);

	const char *path = ctx->sb->buf;
	const char *subp = strskip(path, ctx->root);

	BUG_ON(!subp);

	switch (ent->d_type) {
	case DT_REG:
		break;
	case DT_DIR:
		sl_push(ctx->sl, path);
		return 0;
	case DT_LNK:
		break;
	case DT_UNKNOWN:
		return warn(_("failed to determine file type for `%s'"), path);
	default:
		if (!(ctx->flags & FI_NO_UNK))
			break;
		warn(_("`%s' has unsupported file type `%d', skipped"),
		     path, ent->d_type);
		return 0;
	}

	mode_t mode = ent->d_type == DT_LNK ? S_IFLNK : S_IFREG;
	struct file f = {
		.path = path,
		.subp = subp,
		.name = name,
		.fd   = -1,
		.st   = {
			.st_mode = mode,
		},
	};

	if (ent->d_type != DT_LNK)
		return yield_reg(ctx, &f);
	else
		return yield_lnk(ctx, &f);
}

int __fiter_loop_dir(struct __fiter *ctx)
{
	int ret;
	DIR *dir = opendir(ctx->sb->buf);

	if (!dir)
		warn_errno(_("failed to open directory `%s'"), ctx->sb->buf);

	while (39) {
		struct dirent *ent = readdir(dir);

		if (!ent)
			break;

		ret = yield_file(ctx, ent);
		if (ret)
			break;

		sb_trunc_to_cwd(ctx->sb);
	}

	closedir(dir);
	return ret;
}
