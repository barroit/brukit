// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include "string.h"

#include <locale.h>

TESTDECL_BEGIN();

TESTDECL_ROUTINE(__mbslen)
{
	size_t len;
	
	len = __mbslen("ミクミク");
	USSERT_EQUAL(len, 4);

	len = __mbslen("mikumiku");
	USSERT_EQUAL(len, 8);

	len = __mbslen("");
	USSERT_EQUAL(len, 0);

	len = __mbslen("^");
	USSERT_EQUAL(len, 1);
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

	str[0] = __mbtowc("𐍈");
	USSERT_STREQUAL_WC(str, L"𐍈");

	str[0] = __mbtowc((char []){ 0xF8, 0x80, 0x80, 0x80, 0 });
	USSERT_LT(str[0], 0xFF);
}

TESTDECL_END();
