// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

/*
 * Win32 file flags: a masterclass in turning something simple into an
 * unreadable pile of shit. Seriously, whoever designed this must have
 * had their skull smashed by a Windows-running PC.
 */

#include "fiter.h"

#include <stdlib.h>
#include <windows.h>

#include "gettext.h"
#include "path.h"
#include "strbuf.h"
#include "strlist.h"
#include "tercol.h"
#include "termas.h"
#include "xc.h"

#define FILE_ATTRIBUTE_REGULAR \
	(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | \
	FILE_ATTRIBUTE_SYSTEM    | FILE_ATTRIBUTE_ARCHIVE | \
	FILE_ATTRIBUTE_TEMPORARY | FILE_ATTRIBUTE_COMPRESSED | \
	FILE_ATTRIBUTE_ENCRYPTED | FILE_ATTRIBUTE_SPARSE_FILE | \
	FILE_ATTRIBUTE_OFFLINE)

static int yield_file(struct __fiter *ctx, WIN32_FIND_DATA *ent)
{
	int is_dir = ent->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
	int is_lnk = ent->dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT;
	int is_reg = ent->dwFileAttributes & FILE_ATTRIBUTE_REGULAR;
	const xchar *name = ent->cFileName;

	if (pth_is_dot(name))
		return 0;
	sb_pth_append(ctx->sb, name);

	if (!is_lnk) {
		if (__fiter_is_list_dir_only(ctx->flags) && !is_dir)
			return 0;

		if (is_reg) {
			if (ctx->flags & FI_NO_REG)
				return 0;
		} else if (ctx->flags & FI_NO_UNK && !is_dir) {
			char *path = __sb_mb_str_fb(ctx->sb, "���");

			warn(_("`%s' has unsupported file type `%lu', skipped"),
		     	     path, ent->dwFileAttributes);
			if (IS_ENABLED(CONFIG_ENABLE_WCHAR))
				free(path);

			return 0;
		}
	} else if (ctx->flags & FI_NO_LNK) {
		return 0;
	}

	const xchar *path = ctx->sb->buf;
	const xchar *subp = strskip(path, ctx->root);

	BUG_ON(!subp);

	struct file f = {
		.path = path,
		.subp = subp,
		.name = name,
		.fd   = -1,
		.st   = {
			.st_mode = S_IFREG,
		}
	};

	if (is_lnk) {
		f.st.st_mode = S_IFLNK;
		return ctx->cb(&f, ctx->data);
	} else if (is_dir) {
		sl_push(ctx->sl, path);
		return 0;
	}

	if (ctx->flags & FI_USE_STAT) {
		int err;

		if (IS_ENABLED(CONFIG_ENABLE_WCHAR))
			err = _wstat((wchar_t *)path, &f.st);
		else
			err = _stat((char *)path, &f.st);

		if (unlikely(err)) {
			char *p = __sb_mb_str_fb(ctx->sb, "���");

			warn_errno(_("failed to get status for `%s'"), p);
			if (IS_ENABLED(CONFIG_ENABLE_WCHAR))
				free(p);

			return -1;
		}
	}

	f.st.st_mode = S_IFREG;
	return ctx->cb(&f, ctx->data);
}

int __fiter_loop_dir(struct __fiter *ctx)
{
	sb_pth_append(ctx->sb, XC("*"));

	int ret;
	WIN32_FIND_DATA ent;
	HANDLE dir = FindFirstFile(ctx->sb->buf, &ent);

	if (unlikely(dir == INVALID_HANDLE_VALUE)) {
		char *p = __sb_mb_str_fb(ctx->sb, "���");

		warn_winerr(_("failed to find first file from pattern `%s'"),
			    p);
		if (IS_ENABLED(CONFIG_ENABLE_WCHAR))
			free(p);

		return -1;
	}

	do {
		sb_trunc_to_cwd(ctx->sb);

		ret = yield_file(ctx, &ent);
		if (ret)
			break;

		ret = !FindNextFile(dir, &ent);
		if (unlikely(ret)) {
			DWORD errnum = GetLastError();

			if (errnum == ERROR_NO_MORE_FILES) {
				ret = 0;
				break;
			}

			char *p = __sb_mb_str_fb(ctx->sb, "���");

			warn_winerr(_("failed to find next file after `%s'"),
				    p);
			if (IS_ENABLED(CONFIG_ENABLE_WCHAR))
				free(p);

			break;
		}
	} while (39);

	FindClose(dir);
	return ret;
}
