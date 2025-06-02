// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 *
 * This file is compiled only when CONFIG_ENABLE_I18N is enabled.
 */

#include "gettext.h"

#include <errno.h>
#include <locale.h>
#include <stdlib.h>

#include "compiler.h"
#include "init.h"
#include "path.h"

const char *__gettext(const char *masid)
{
	int errnum = errno;
	const char *ret = gettext(masid);

	errno = errnum;
	return ret;
}

static const char *user_locale(void)
{
	const char *lang;

	lang = getenv("LANGUAGE");
	if (lang)
		return lang;

	lang = getenv("LANG");
	if (lang)
		return lang;

	return "C.UTF-8";
}

void __gettext_init(void)
{
	const char *dir = pth_locale();
	const char *locale = user_locale();

	textdomain(CONFIG_TEXT_DOMAIN_NAME);
	bindtextdomain(CONFIG_TEXT_DOMAIN_NAME, dir);
	bind_textdomain_codeset(CONFIG_TEXT_DOMAIN_NAME, "UTF-8");

	setenv("LANGUAGE", locale, 1);

	setlocale(LC_CTYPE, "C.UTF-8");
	setlocale(LC_TIME, locale);
	setlocale(LC_MONETARY, locale);
	setlocale(LC_MESSAGES, locale);
}

INIT_ROUTINE(gettext)
{
	gettext_init();
}
