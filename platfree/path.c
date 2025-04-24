// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "path.h"

#include "compiler.h"
#include "strbuf.h"
#include "xalloc.h"
#include "xc.h"

const xchar *pth_prefix(void)
{
	static xchar *name;

	if (unlikely(!name)) {
		const xchar *child = pth_executable();
		struct strbuf sb = SB_INIT;

		sb_puts(&sb, child);
		sb_pth_legacy_to_dirname(&sb);
		name = sb_detach(&sb);
	}

	return name;
}

const xchar *pth_locale(void)
{
	static xchar *name;

	if (unlikely(!name)) {
		const xchar *prefix = pth_prefix();
		struct strbuf sb = SB_INIT;

		sb_puts(&sb, prefix);
		sb_pth_legacy_append(&sb, XC("locale"));
		name = sb_detach(&sb);
	}

	return name;
}
