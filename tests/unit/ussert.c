// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "ussert.h"

#include "calc.h"
#include "cconv.h"

int __cold __ussert_strequal(const char *file, int line,
			     const char *func, const char *expr,
			     const xchar *__s1, const xchar *__s2)
{
	if (xc_strcmp(__s1, __s2) == 0)
		return 0;

	char *s1 = (typeof(s1))__s1;
	char *s2 = (typeof(s2))__s2;

	if (IS_ENABLED(CONFIG_WIDE_CHAR)) {
		size_t len;

		len = conv_wcstombs(&s1, (wchar_t *)__s1);
		if (len == maxof(len))
			goto err_conv_s1;

		len = conv_wcstombs(&s2, (wchar_t *)__s2);
		if (len == maxof(len))
			goto err_conv_s2;
	}

	__termas(file, line, func, TM_ERROR, NULL, TM_FLLN | TM_FUNC,
		 "\nassertion `%s' failed\na: %s\nb: %s", expr, s1, s2);

	if (IS_ENABLED(CONFIG_WIDE_CHAR)) {
		free(s1);
		free(s2);
	}

	__test_failure_count += 1;
	return 1;

err_conv_s2:
	free(s1);
err_conv_s1:
	__termas(file, line, func, TM_ERROR, NULL,
		 TM_FLLN | TM_FUNC, "\nassertion `%s' failed", expr);

	__test_failure_count += 1;
	return -1;
}
