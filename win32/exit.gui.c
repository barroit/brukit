// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shlwapi.h>

#include "compiler.h"
#include "console.h"
#include "gettext.h"
#include "udef.h"
#include "xalloc.h"
#include "xc.h"
#include "xcf.h"

#ifdef exit
# undef exit
#endif

#ifdef CONFIG_ENABLE_WCHAR
# undef __argv
# define __argv __wargv
# define getenv _wgetenv
#endif

static int is_testing(void)
{
	if (!IS_ENABLED(CONFIG_ENABLE_TEST))
		return 0;

	const xchar *exe = getenv(XC("TEST3939"));

	if (!exe)
		return 0;

	const xchar *n1 = PathFindFileName(exe);
	const xchar *n2 = PathFindFileName(__argv[0]);

	if (xc_strcmp(n1, n2) == 0)
		return 1;

	xchar *n3 = xc_xstrdup(n2);
	int in_test = 0;

	PathRemoveExtension(n3);
	if (xc_strcmp(n1, n3) == 0)
		in_test = 1;

	free(n3);
	return in_test;
}

void __exit_show_confirm(void)
{
	if (is_testing())
		return;

	puts(_("\nPress any key to continue..."));
	_getch();
}

void __exit(int status)
{
	if (!udef_no_console && status) {
		console_show();
		__exit_show_confirm();
	}

	if (status == EXIT_CONOUT)
		status = 0;

	exit(status);
}
