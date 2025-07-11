// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include <string.h>

#include "path.h"
#include "strbuf.h"
#include "xcf.h"

#define SEP XC(PTH_SEP_STR)

UT_BEGIN();

UT_ROUTINE(sb_puts)
{
	struct strbuf __cleanup(sb_destroy) sb = SB_INIT;
	uint __cap;

	sb_puts(&sb, XC("miku"));
	UA_EQ(xc_strlen(sb.buf), 4);
	UA_EQ(sb.len, 4);
	__cap = sb.cap;

	sb_puts_at_cwd(&sb, XC("miku39"));
	UA_EQ(xc_strlen(sb.buf), 6);
	UA_EQ(sb.len, 6);
	UA_EQ(sb.cap, __cap);

	sb_puts_at(&sb, 2, XC("miku"));
	UA_EQ(xc_strlen(sb.buf), 6);
	UA_EQ(sb.len, 6);

	sb_puts_at(&sb, 0, XC(""));
	UA_EQ(xc_strlen(sb.buf), 0);
	UA_EQ(sb.len, 0);
}

UT_ROUTINE(sb_putc)
{
	struct strbuf __cleanup(sb_destroy) sb = SB_INIT;
	uint __cap;

	sb_putc(&sb, XC('a'));
	UA_EQ(xc_strlen(sb.buf), 1);
	__cap = sb.cap;

	sb_putc_at_cwd(&sb, XC('b'));
	UA_EQ(xc_strlen(sb.buf), 1);
	UA_STREQ(sb.buf, XC("b"));
	UA_EQ(sb.cap, __cap);

	sb_puts_at(&sb, 0, XC("miku.39"));
	sb_putc_at(&sb, 4, XC('_'));
	UA_STREQ(sb.buf, XC("miku_"));

	sb_putc_at(&sb, 0, 0);
	UA_EQ(xc_strlen(sb.buf), 0);
	UA_EQ(sb.len, 0);
}

UT_ROUTINE(sb_trunc)
{
	struct strbuf __cleanup(sb_destroy) sb = SB_INIT;

	sb_puts(&sb, XC("aliquam sit amet nibh commodo"));

	sb_trunc(&sb, sb.len);
	UA_ZERO(sb.len);
	UA_ZERO(sb.buf[0]);

	sb_pth_reinit_cwd_dumb(&sb, XC("/path/to/dir"));

	sb_puts(&sb, XC("/dummy/path"));
	sb_trunc_to_cwd(&sb);
	UA_EQ(sb.len, xc_strlen(XC("/path/to/dir")));
	UA_STREQ(sb.buf, XC("/path/to/dir"));
}

UT_ROUTINE(sb_printf)
{
	struct strbuf __cleanup(sb_destroy) sb = SB_INIT;
	uint __cap;

	/* 39 Variable! */
	int int39 = 39;
	float fp39 = 39.3939f;
	uint hex39 = 0x3939;
	const xchar *str39 = XC("miku");

	sb_printf(&sb, XC("%d\t%.2f\t%x\t%s"), int39, fp39, hex39, str39);
	UA_STREQ(sb.buf, XC("39\t39.39\t3939\tmiku"));
	__cap = sb.cap;

	sb_printf_at_cwd(&sb,
			XC("%d\t%.2f\t%x\t%s"), int39, fp39, hex39, str39);
	UA_STREQ(sb.buf, XC("39\t39.39\t3939\tmiku"));
	UA_EQ(sb.cap, __cap);

	sb_printf_at(&sb, 9,
		     XC("%d\t%.2f\t%x\t%s"), int39, fp39, hex39, str39);
	UA_STREQ(sb.buf, XC("39\t39.39\t39\t39.39\t3939\tmiku"));
}

UT_ROUTINE(sb_trim)
{
	struct strbuf __cleanup(sb_destroy) sb = SB_INIT;

	sb_puts(&sb, XC("aliquam sit amet nibh commodo\t         "));
	sb_trim(&sb);
	UA_STREQ(sb.buf, XC("aliquam sit amet nibh commodo"));

	sb_trunc(&sb, sb.len);
	sb_puts(&sb, XC("      \t\taliquam sit amet nibh commodo\t\t    "));
	sb_trim(&sb);
	UA_STREQ(sb.buf, XC("aliquam sit amet nibh commodo"));

	sb_trunc(&sb, sb.len);
	sb_puts(&sb, XC("      \t\taliquam sit amet nibh commodo"));
	sb_trim(&sb);
	UA_STREQ(sb.buf, XC("aliquam sit amet nibh commodo"));
}

UT_ROUTINE(sb_off_ws)
{
	struct strbuf __cleanup(sb_destroy) sb;

	sb_pth_init_cwd_dumb(&sb, XC("path/to/root/dir/"));

	UA_STREQ(sb.buf, XC("path/to/root/dir/"));
	UA_EQ(sb.off.cwd, xc_strlen(XC("path/to/root/dir/")));

	/*
	 * Don't copy this, use path-legacy-related APIs in your code.
	 */
	sb_puts(&sb, XC("executable"));
	UA_STREQ(sb.buf, XC("path/to/root/dir/executable"));

	sb_puts_at_cwd(&sb, XC("file"));
	UA_STREQ(sb.buf, XC("path/to/root/dir/file"));

	sb_printf_at_cwd(&sb, XC(".%s%u"), XC("miku"), 39);
	UA_STREQ(sb.buf, XC("path/to/root/dir/.miku39"));
}

UT_ROUTINE(sb_pth_init_cwd_dumb)
{
	struct strbuf __cleanup(sb_destroy) sb;

	sb_pth_init_cwd_dumb(&sb, XC("path/to/default/cwd"));
	UA_STREQ(sb.buf, XC("path/to/default/cwd"));

	sb_pth_reinit_cwd_dumb(&sb, XC("path/to/new/cwd"));
	UA_STREQ(sb.buf, XC("path/to/new/cwd"));
}

UT_ROUTINE(sb_pth_legacy_api)
{
	struct strbuf __cleanup(sb_destroy) sb;

	sb_pth_init_cwd_dumb(&sb, XC("path/to/root/dir"));

	sb_pth_append_dumb(&sb, XC("executable"));
	UA_STREQ(sb.buf, XC("path/to/root/dir") SEP XC("executable"));

	sb_pth_append_at_cwd_dumb(&sb, XC("file"));
	UA_STREQ(sb.buf, XC("path/to/root/dir") SEP XC("file"));
}

UT_END();
