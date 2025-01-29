// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include "strbuf.h"

TESTDECL_BEGIN();

TESTDECL_ROUTINE(sb_init_ws)
{
	struct strbuf __cleanup(sb_destroy) sb;

	sb_init_ws(&sb, XC("path/to/default/ws"));
	USSERT_STREQUAL(sb.buf, XC("path/to/default/ws"));

	sb_reinit_ws(&sb, XC("path/to/new/ws"));
	USSERT_STREQUAL(sb.buf, XC("path/to/new/ws"));
}

TESTDECL_ROUTINE(sb_puts)
{
	struct strbuf __cleanup(sb_destroy) sb = SB_INIT;
	uint __cap;

	sb_puts(&sb, XC("miku"));
	USSERT_EQ(xc_strlen(sb.buf), 4);
	__cap = sb.cap;

	sb_puts_at_ws(&sb, XC("miku39"));
	USSERT_EQ(xc_strlen(sb.buf), 6);
	USSERT_EQ(sb.cap, __cap);

	sb_puts_at(&sb, 2, XC("miku"));
	USSERT_EQ(xc_strlen(sb.buf), 6);
}

TESTDECL_ROUTINE(sb_putc)
{
	struct strbuf __cleanup(sb_destroy) sb = SB_INIT;
	uint __cap;

	sb_putc(&sb, XC('a'));
	USSERT_EQ(xc_strlen(sb.buf), 1);
	__cap = sb.cap;

	sb_putc_at_ws(&sb, XC('b'));
	USSERT_EQ(xc_strlen(sb.buf), 1);
	USSERT_STREQUAL(sb.buf, XC("b"));
	USSERT_EQ(sb.cap, __cap);

	sb_puts_at(&sb, 0, XC("miku.39"));
	sb_putc_at(&sb, 4, XC('_'));
	USSERT_STREQUAL(sb.buf, XC("miku_"));
}

TESTDECL_ROUTINE(sb_trunc)
{
	struct strbuf __cleanup(sb_destroy) sb = SB_INIT;

	sb_puts(&sb, XC("aliquam sit amet nibh commodo"));

	sb_trunc(&sb, sb.len);
	USSERT_ZERO(sb.len);
	USSERT_ZERO(sb.buf[0]);

	sb_reinit_ws(&sb, XC("/path/to/dir"));

	sb_puts(&sb, XC("/dummy/path"));
	sb_trunc_to_ws(&sb);
	USSERT_EQ(sb.len, xc_strlen(XC("/path/to/dir")));
	USSERT_STREQUAL(sb.buf, XC("/path/to/dir"));
}

TESTDECL_ROUTINE(sb_printf)
{
	struct strbuf __cleanup(sb_destroy) sb = SB_INIT;
	uint __cap;

	/* 39 Variable! */
	int int39 = 39;
	float fp39 = 39.3939f;
	uint hex39 = 0x3939;
	const xchar *str39 = XC("miku");

	sb_printf(&sb, XC("%d\t%.2f\t%x\t%s"), int39, fp39, hex39, str39);
	USSERT_STREQUAL(sb.buf, XC("39\t39.39\t3939\tmiku"));
	__cap = sb.cap;

	sb_printf_at_ws(&sb,
			XC("%d\t%.2f\t%x\t%s"), int39, fp39, hex39, str39);
	USSERT_STREQUAL(sb.buf, XC("39\t39.39\t3939\tmiku"));
	USSERT_EQ(sb.cap, __cap);

	sb_printf_at(&sb, 9,
		     XC("%d\t%.2f\t%x\t%s"), int39, fp39, hex39, str39);
	USSERT_STREQUAL(sb.buf, XC("39\t39.39\t39\t39.39\t3939\tmiku"));
}

TESTDECL_ROUTINE(sb_trim)
{
	struct strbuf __cleanup(sb_destroy) sb = SB_INIT;

	sb_puts(&sb, XC("aliquam sit amet nibh commodo\t         "));
	sb_trim(&sb);
	USSERT_STREQUAL(sb.buf, XC("aliquam sit amet nibh commodo"));

	sb_trunc(&sb, sb.len);
	sb_puts(&sb, XC("      \t\taliquam sit amet nibh commodo\t\t    "));
	sb_trim(&sb);
	USSERT_STREQUAL(sb.buf, XC("aliquam sit amet nibh commodo"));

	sb_trunc(&sb, sb.len);
	sb_puts(&sb, XC("      \t\taliquam sit amet nibh commodo"));
	sb_trim(&sb);
	USSERT_STREQUAL(sb.buf, XC("aliquam sit amet nibh commodo"));
}

TESTDECL_ROUTINE(sb_off_ws)
{
	struct strbuf __cleanup(sb_destroy) sb;

	/*
	 * This is just a test. Do not initialize a strbuf with a trailing
	 * slash and use path-related APIs if the strbuf is working with
	 * off.ws (ws stands for working space).
	 */
	sb_init_ws(&sb, XC("path/to/root/dir/"));

	USSERT_STREQUAL(sb.buf, XC("path/to/root/dir/"));
	USSERT_EQ(sb.off.ws, xc_strlen(XC("path/to/root/dir/")));

	sb_puts(&sb, XC("executable"));
	USSERT_STREQUAL(sb.buf, XC("path/to/root/dir/executable"));

	sb_puts_at_ws(&sb, XC("file"));
	USSERT_STREQUAL(sb.buf, XC("path/to/root/dir/file"));

	sb_printf_at_ws(&sb, XC(".%s%u"), XC("miku"), 39);
	USSERT_STREQUAL(sb.buf, XC("path/to/root/dir/.miku39"));
}

TESTDECL_ROUTINE(sb_pth_api)
{
	struct strbuf __cleanup(sb_destroy) sb = SB_INIT;

#if defined(__unix__)
	sb_init_ws(&sb, XC("path/to/root/dir"));

	sb_pth_append(&sb, XC("executable"));
	USSERT_STREQUAL(sb.buf, XC("path/to/root/dir/executable"));

	sb_pth_append_at_ws(&sb, XC("file"));
	USSERT_STREQUAL(sb.buf, XC("path/to/root/dir/file"));

#elif defined(_WIN32)
	sb_init_ws(&sb, XC("path\\to\\root\\dir"));

	sb_pth_append(&sb, XC("executable"));
	USSERT_STREQUAL(sb.buf, XC("path\\to\\root\\dir\\executable"));

	sb_pth_append_at_ws(&sb, XC("file"));
	USSERT_STREQUAL(sb.buf, XC("path\\to\\root\\dir\\file"));
#endif
}

TESTDECL_END();
