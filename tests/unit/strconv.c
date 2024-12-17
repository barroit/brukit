// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include <locale.h>

#include "calc.h"
#include "strconv.h"

TESTDECL_BEGIN(setup)
{
	setlocale(LC_CTYPE, "en_US.UTF-8");
}

TESTDECL_ROUTINE(wcstombs)
{
	char __cleanup(__free) *__res;
	size_t len = mb_wcstombs(&__res, L"ミク");

	USSERT_EQ(len, strlen("ミク"));
	USSERT_STREQUAL_MB(__res, "ミク");

	char *res;
	wchar_t eseq[] = { L'ミ', 0xD800, L'ク', 0 };

	len = mb_wcstombs(&res, eseq);
	USSERT_EQ(len, maxof(len));

	len = mb_wcstombs_fb(&res, eseq, "���");
	USSERT_EQ(len, strlen("���"));
	USSERT_STREQUAL_MB(res, "���");
}

TESTDECL_ROUTINE(mbstowcs)
{
	wchar_t __cleanup(__free) *__res;
	size_t len = wc_mbstowcs(&__res, "ミク");

	USSERT_EQ(len, wcslen(L"ミク"));
	USSERT_STREQUAL_WC(__res, L"ミク");

	wchar_t *res;
	char eseq[] = {
		0xE3, 0x83, 0x9F,
		0xED, 0xA0, 0x80,
		0xE3, 0x82, 0xAF,
		0x00,
	};

	len = wc_mbstowcs(&res, eseq);
	USSERT_EQ(len, maxof(len));

	len = wc_mbstowcs_fb(&res, eseq, L"���");
	USSERT_EQ(len, wcslen(L"���"));
	USSERT_STREQUAL_WC(res, L"���");
}

TESTDECL_END();
