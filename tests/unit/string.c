// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include "string.h"

#include <locale.h>

TESTDECL_BEGIN(setup)
{
	setlocale(LC_ALL, "en_US.UTF-8");
}

TESTDECL_ROUTINE(__mbslen)
{
	size_t len;
	
	len = __mbslen("ミクミク");
	USSERT_EQ(len, 4);

	len = __mbslen("mikumiku");
	USSERT_EQ(len, 8);

	len = __mbslen("");
	USSERT_EQ(len, 0);

	len = __mbslen("^");
	USSERT_EQ(len, 1);
}

TESTDECL_ROUTINE(__mbtowc)
{
	wchar_t str[2] = { 0 };

	str[0] = __mbtowc("M");
	USSERT_STREQUAL_WC(str, L"M");

	str[0] = __mbtowc("é");
	USSERT_STREQUAL_WC(str, L"é");

	str[0] = __mbtowc("ミ");
	USSERT_STREQUAL_WC(str, L"ミ");

	str[0] = __mbtowc((char []){ 0xF8, 0x80, 0x80, 0x80, 0 });
	USSERT_LT(str[0], 0xFF);
}

TESTDECL_ROUTINE(mbsws)
{
	const char *str = "ミク　３９";
	const char *rest = mbsws(str);

	USSERT_NONNULL(rest);
	USSERT_STREQUAL_MB(rest, "　３９");
}

TESTDECL_ROUTINE(wcsws)
{
	const wchar_t *str = L"ミク　３９";
	const wchar_t *rest = wcsws(str);

	USSERT_NONNULL(rest);
	USSERT_STREQUAL_WC(rest, L"　３９");
}

TESTDECL_END();
