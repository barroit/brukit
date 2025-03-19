// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "ussert.h"

#include <string.h>
#include <wchar.h>

#include "calc.h"
#include "strconv.h"

int __cold __ua_streq(const char *file, int line,
			     const char *func, const char *expr,
			     const void *__s1, const void *__s2, int is_ansi)
{
	if (is_ansi) {
		if (strcmp(__s1, __s2) == 0)
			return 0;
	} else {
		if (wcscmp(__s1, __s2) == 0)
			return 0;
	}

	char *s1 = (typeof(s1))__s1;
	char *s2 = (typeof(s2))__s2;

	if (!is_ansi) {
		mb_wcstombs_fb(&s1, (wchar_t *)__s1, "���");
		mb_wcstombs_fb(&s2, (wchar_t *)__s2, "���");
	}

	__termas(file, line, func,
		 TM_ERROR, NULL, MAS_SHOW_FILE | MAS_SHOW_FUNC,
		 "\nassertion `%s' failed\na: %s\nb: %s", expr, s1, s2);

	if (!is_ansi) {
		free(s1);
		free(s2);
	}

	return 1;
}
