// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include <locale.h>

#include "string.h"

UT_BEGIN(setup)
{
	setlocale(LC_ALL, "C.UTF-8");
}

UT_ROUTINE(__mbslen)
{
	size_t len;
	
	len = __mbslen("ミクミク");
	UA_EQ(len, 4);

	len = __mbslen("mikumiku");
	UA_EQ(len, 8);

	len = __mbslen("");
	UA_EQ(len, 0);

	len = __mbslen("^");
	UA_EQ(len, 1);
}

UT_ROUTINE(__mbtowc)
{
	wchar_t str[2] = { 0 };

	str[0] = __mbtowc("M");
	UA_STREQ_WC(str, L"M");

	str[0] = __mbtowc("é");
	UA_STREQ_WC(str, L"é");

	str[0] = __mbtowc("ミ");
	UA_STREQ_WC(str, L"ミ");

	str[0] = __mbtowc((char []){ 0xf8, 0x80, 0x80, 0x80, 0 });
	UA_LT(str[0], 0xff);
}

UT_ROUTINE(mbsws)
{
	const char *str = "ミク　３９";
	const char *rest = mbsws(str);

	UA_NONNULL(rest);
	UA_STREQ_MB(rest, "　３９");
}

UT_ROUTINE(wcsws)
{
	const wchar_t *str = L"ミク　３９";
	const wchar_t *rest = wcsws(str);

	UA_NONNULL(rest);
	UA_STREQ_WC(rest, L"　３９");
}

UT_END();
