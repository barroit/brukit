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
		const xchar *program = pth_executable();
		struct strbuf *sb;

		sb_pth_init(&sb, program);
		sb_pth_pop(sb);
		sb_pth_pop(sb);

		name = sb_pth_detach(&sb);
	}

	return name;
}

const xchar *pth_locale(void)
{
	static xchar *name;

	if (unlikely(!name)) {
		const xchar *prefix = pth_prefix();
		struct strbuf *sb;

		sb_pth_init(&sb, prefix);
		sb_pth_push(sb, XC("share/locale"));

		name = sb_pth_detach(&sb);
	}

	return name;
}
