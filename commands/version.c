// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include <stdio.h>
#include <stdlib.h>

#include "corecmd.h"
#include "param.h"
#include "path.h"
#include "strconv.h"
#include "strlist.h"

#define VERSION(x)     __VERSION(x)
#define __VERSION(...) ___VERSION(, ##__VA_ARGS__, no)
#define ___VERSION(_, x, ...) #x

#define SHOW_BUILD  (1U << 0)
#define SHOW_PREFIX (1U << 1)
#define SHOW_CFLAGS (1U << 2)
#define SHOW_LIBVER (1U << 3)

#define __show_mask (-1U >> (32 - 4))

#define __bias(m) ((m) << 16)
#define __mode(m) ((m) | __bias(m))

static const char *usage[] = {
	"version [-a | --all] [<options>]",
	NULL,
	NULL,
};

static int show_all;
static u32 show_mode = __show_mask;

static struct opt opts[] = {
	OPT_FLAG('a', "all", &show_all, N_("display all info")),
	OPT_BIT(0, "build", &show_mode,
		__mode(SHOW_BUILD), N_("show build platform")),
	OPT_BIT(0, "prefix", &show_mode,
		__mode(SHOW_PREFIX), N_("show install location")),
	OPT_BIT(0, "cflag", &show_mode,
		__mode(SHOW_CFLAGS), N_("list compile flags")),
	OPT_BIT(0, "lib", &show_mode,
		__mode(SHOW_LIBVER), N_("list library versions")),
	OPT_END(),
};

static void show_build(void)
{
	/*
	 * TRANSLATORS
	 * Keep the two leading space characters, and make sure the area before
	 * the first %s is 12 half-width characters wide.
	 */
	printf(_("  build     %s-%s\n"), PROGRAM_BUILD, PROGRAM_ARCH);
}

static void print_line(const char *fmt, const char *line)
{
	static STRLIST(sl, SL__STORE_CHR);
	char *str;

	sl_read_line_chr(&sl, line, CONFIG_LINE_WRAP - 12);

	str = sl_pop_chr(&sl);
	printf(fmt, str);
	free(str);

	while ((str = sl_pop_chr(&sl))) {
		printf("            %s\n", str);
		free(str);
	}
}

static void show_prefix(void)
{
	const xchar *__name = pth_prefix();
	char *name = (char *)__name;

	if (IS_ENABLED(CONFIG_ENABLE_WCHAR))
		mb_wcstombs_fb(&name, (wchar_t *)__name, "���");

	/*
	 * TRANSLATORS
	 * Keep the two leading space characters, and make sure the area before
	 * %s is 12 half-width characters wide.
	 */
	print_line(_("  prefix    %s\n"), name);

	if (IS_ENABLED(CONFIG_ENABLE_WCHAR))
		free(name);
}

static void show_cflags(void)
{
	print_line("  CFLAGS    %s\n", COMPILE_FLAGS);
}

static void show_libvar(void)
{
	if (show_mode & ~SHOW_LIBVER)
		putchar('\n');

	/*
	 * TRANSLATORS
	 * Keep the two leading space characters.
	 */
	puts(_("  libraries"));

	printf("            gettext-%s\n", VERSION(GETTEXT_VERSION));
}

int cmd_version(int argc, const xchar **argv)
{
	param_parse(argc, argv, usage, opts, PRM_NO_ARG);

	printf(_("%s version %s\n"), PROGRAM_NAME, PROGRAM_VERSION);

	if (!show_all)
		show_mode >>= 16;
	else
		show_mode &= __show_mask;

	if (show_mode)
		putchar('\n');

	if (show_mode & SHOW_BUILD)
		show_build();

	if (show_mode & SHOW_PREFIX)
		show_prefix();

	if (show_mode & SHOW_CFLAGS)
		show_cflags();

	if (show_mode & SHOW_LIBVER)
		show_libvar();

	exit(EXIT_CONOUT);
}
