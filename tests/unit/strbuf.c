// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include "strbuf.h"

TESTDECL_BEGIN();

TESTDECL_ROUTINE(sb_puts)
{
	struct strbuf __cleanup(sb_destroy) sb = SB_INIT;
	uint __cap;

	sb_puts(&sb, XC("miku"));
	USSERT_EQUAL(xc_strlen(sb.buf), 4);
	__cap = sb.cap;

	sb_puts_at_ws(&sb, XC("miku39"));
	USSERT_EQUAL(xc_strlen(sb.buf), 6);
	USSERT_EQUAL(sb.cap, __cap);

	sb_puts_at(&sb, 2, XC("miku"));
	USSERT_EQUAL(xc_strlen(sb.buf), 6);
}

TESTDECL_ROUTINE(sb_trunc)
{
	struct strbuf __cleanup(sb_destroy) sb = SB_INIT;

	sb_puts(&sb, XC("aliquam sit amet nibh commodo"));

	sb_trunc(&sb, sb.len);
	USSERT_ZERO(sb.len);
	USSERT_ZERO(sb.buf[0]);
}

TESTDECL_ROUTINE(sb_printf)
{
	struct strbuf __cleanup(sb_destroy) sb = SB_INIT;
	uint __cap;

	sb_printf(&sb, XC("%d\t%.2f\t%p\t%s"), 5, 39.3939f, NULL, XC("foo"));
	USSERT_STREQUAL(sb.buf, XC("5\t39.39\t(nil)\tfoo"));
	__cap = sb.cap;

	sb_printf_at_ws(&sb,
			XC("%d\t%.2f\t%p\t%s"), 5, 39.3939f, NULL, XC("foo"));
	USSERT_STREQUAL(sb.buf, XC("5\t39.39\t(nil)\tfoo"));
	USSERT_EQUAL(sb.cap, __cap);

	sb_printf_at(&sb, 9,
		     XC("%d\t%.2f\t%p\t%s"), 5, 39.3939f, NULL, XC("foo"));
	USSERT_STREQUAL(sb.buf, XC("5\t39.39\t(5\t39.39\t(nil)\tfoo"));
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

TESTDECL_END();
