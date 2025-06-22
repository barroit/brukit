// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 *
 * This file is compiled only when CONFIG_ENABLE_I18N is enabled.
 */

#include "i18n.h"

#include <errno.h>
#include <locale.h>
#include <stdarg.h>
#include <stdlib.h>

#include "compiler.h"
#include "init.h"
#include "iter.h"
#include "path.h"
#include "termas.h"
#include "xalloc.h"

#define try_set_lc(lc, locale, ...) \
	__try_set_lc(lc, locale, ##__VA_ARGS__, NULL)

const char *__gettext(const char *masid)
{
	int errnum = errno;
	const char *ret = gettext(masid);

	errno = errnum;
	return ret;
}

static const char *i18n_locale(void)
{
	const char *names[] = {
		"LANGUAGE",
		"LC_ALL",
		"LC_MESSAGES",
		"LANG",
	};
	uint i;

	for_each_idx(i, sizeof_array(names)) {
		const char *locale = getenv(names[i]);

		if (locale)
			return locale;
	}

	return "C.UTF-8";
}

static const char *__try_set_lc(int lc, const char *locale, ...)
{
	const char *ret;
	va_list ap;

	ret = setlocale(lc, locale);
	va_start(ap, locale);

	while (!ret) {
		const char *fb = va_arg(ap, const char *);

		if (!fb)
			break;

		ret = setlocale(lc, fb);
	}

	return ret;
}

static void gettext_init(void)
{
	const char *dir = pth_locale();
	const char *locale = i18n_locale();
	const char *ret;

	ret = try_set_lc(LC_COLLATE, "C");
	setenv("LC_COLLATE", ret ?: "C", 1);

	ret = try_set_lc(LC_CTYPE, "C.UTF-8");
	setenv("LC_CTYPE", ret ?: "C.UTF-8", 1);
	
	ret = try_set_lc(LC_MESSAGES, locale, "en_us.UTF-8");
	setenv("LC_MESSAGES", ret ?: "en_us.UTF-8", 1);

	ret = try_set_lc(LC_MONETARY, locale, "en_us.UTF-8", "C.UTF-8", "C");
	setenv("LC_MONETARY", ret ?: "C", 1);

	ret = try_set_lc(LC_NUMERIC, locale, "en_us.UTF-8", "C.UTF-8", "C");
	setenv("LC_NUMERIC", ret ?: "C", 1);

	ret = try_set_lc(LC_TIME, locale, "en_us.UTF-8", "C.UTF-8", "C");
	setenv("LC_TIME", ret ?: "C", 1);

	textdomain(CONFIG_TEXT_DOMAIN_NAME);
	bindtextdomain(CONFIG_TEXT_DOMAIN_NAME, dir);
	bind_textdomain_codeset(CONFIG_TEXT_DOMAIN_NAME, "UTF-8");
}

INITCALL(gettext)
{
#ifdef HAVE_INTL
	gettext_init();
#else
	NOOP(gettext_init);
#endif
}
