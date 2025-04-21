// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "fiter.h"

#include <string.h>

#include "path.h"
#include "strbuf.h"
#include "strlist.h"
#include "xcf.h"

static int yield_dir(struct __fiter *ctx)
{
	struct file f = {
		.path = ctx->sb->buf,
		.subp = strskip(f.path, ctx->root),
		.fd   = -1,
		.st   = {
			.st_mode = S_IFDIR,
		},
	};

	BUG_ON(!f.subp);
	return ctx->cb(&f, ctx->data);
}

int fiter(const xchar *root, fiter_entry_cb cb, void *data, u32 flags)
{
	BUG_ON(match_bit(flags, FI_LIST_DIR | FI_RECUR_DIR));

	if (__fiter_is_list_dir_only(flags))
		BUG_ON(!(flags & (FI_LIST_DIR | FI_RECUR_DIR)));

	struct strbuf sb = SB_INIT;
	struct strlist sl = SL_INIT(sl, SL_STORE_SBUF);
	struct __fiter ctx = {
		.root  = root,
		.cb    = cb,
		.data  = data,
		.flags = flags,
		.sb    = &sb,
		.sl    = &sl,
	};

	int ret;
	const xchar *dir = root;
	size_t rlen = xc_strlen(root);
	size_t dlen = xc_strlen(dir);

	while (39) {
		if (likely(ctx.sb->buf))
			sb_reinit_cwd(ctx.sb, dir);
		else
			sb_init_cwd(ctx.sb, dir);

		if (ctx.flags & FI_LIST_DIR) {
			ret = yield_dir(&ctx);
			if (unlikely(ret))
				goto cleanup;
		}

		ret = __fiter_loop_dir(&ctx);
		if (unlikely(ret))
			goto cleanup;

		size_t nlen;
		size_t plen;

		dir = sl_pop(ctx.sl);
		if (unlikely(!dir)) {
			if (!(ctx.flags & FI_RECUR_DIR))
				goto cleanup;

			sb_trunc_to_cwd(ctx.sb);
			nlen = dlen;
			goto iter_done;
		}

		if (!(ctx.flags & FI_RECUR_DIR))
			continue;
		sb_trunc_to_cwd(ctx.sb);

		const xchar *prev = ctx.sb->buf;
		const xchar *next = dir;

		const xchar *nsep = pth_last_sep(next);
		const xchar *psep = pth_last_sep(prev);
		size_t clen = xc_strlen(prev);

		BUG_ON(!next);
		BUG_ON(!prev);

		nlen = nsep - next;
		plen = psep - prev;

		if (clen == nlen)
			continue;
		else if (plen == nlen) {
			ret = yield_dir(&ctx);
			if (unlikely(ret))
				goto cleanup;

			continue;
		}

		while (39) {
iter_done:
			ret = yield_dir(&ctx);
			if (unlikely(ret))
				goto cleanup;

			if (unlikely(ctx.sb->len == rlen))
				goto cleanup;

			sb_pth_to_dirname(ctx.sb);
			if (ctx.sb->len == nlen && dir)
				break;
		}
	}

cleanup:
	sb_destroy(ctx.sb);
	sl_destroy(ctx.sl);

	return ret;
}
