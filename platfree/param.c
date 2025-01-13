// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "param.h"

#include "bitopt.h"
#include "iter.h"
#include "list.h"
#include "strbuf.h"
#include "strconv.h"
#include "string.h"
#include "strlist.h"
#include "strtox.h"
#include "termas.h"
#include "xalloc.h"

#define opt_for_each(pos, opts) \
	for (pos = opts; (pos)->type != OPTION__END; (pos)++)

#define OPT_SHORT_OPT (1 << 0)
#define OPT_LONG_OPT  (1 << 1)
#define OPT_UNSET (1 << 2)

#define __setopt(name) opt_set_ ## name

#define NO_NO_ARG (-1U << 31)

struct param {
	int argc;
	const xchar **argv;

	int outc;
	const xchar **outv;

	struct option *opts;
	const char **usage;

	u32 flags;

	struct list_head mode;
};

struct abbrev {
	struct option *opt;
	u32 flags;
};

struct cmdmode {
	int val;
	int *ptr;

	struct option *opt;
	u32 flags;

	struct list_head list;
};

static void __noreturn show_help(const char **usage, struct option *opts);

static struct strbuf __cmdname = SB_INIT;
static char *cmdname;

static void cmdname_append(const xchar *name)
{
	if (__cmdname.len != 0)
		sb_putc(&__cmdname, XC(' '));

	sb_puts(&__cmdname, name);

	if (IS_ENABLED(CONFIG_WIDE_CHAR)) {
		if (cmdname)
			free(cmdname);
		cmdname = sb_mb_str_fb(&__cmdname, "����");
	} else {
		cmdname = (char *)__cmdname.buf;
	}
}

static void cmdmode_init(struct list_head *mode, struct option *opts)
{
	struct cmdmode *cm;
	struct option *opt;

	opt_for_each(opt, opts) {
		if (opt->type != OPTION_CMDMODE)
			continue;

		cm = xcalloc(1, sizeof(*cm));
		list_add(&cm->list, mode);

		cm->ptr = opt->ptr;
		cm->val = *cm->ptr;
	}
}

static void cmdmode_destroy(struct list_head *mode)
{
	struct cmdmode *cm, *tmp;

	list_for_each_entry_safe(cm, tmp, mode, list) {
		list_del(&cm->list);
		free(cm);
	}
}

static char *pretty_opt_name(struct option *opt, u32 flags)
{
	struct strbuf sb = SB_INIT;

	if (flags & OPT_SHORT_OPT)
		sb_printf(&sb, XC("-%c"), opt->snam);
	else
		sb_printf(&sb, XC("--%s%s"),
			  flags & OPT_UNSET ? XC("no-") : XC(""), opt->lnam);
	
	char *ret = sb_mb_str_fb(&sb, "���");

	sb_destroy(&sb);
	return ret;
}

static char *pretty_arg_name(const xchar *arg, const char *fb)
{
	char *ret = (char *)arg;

	if (IS_ENABLED(CONFIG_WIDE_CHAR))
		mb_wcstombs_fb(&ret, (wchar_t *)arg, fb);

	return ret;
}

static void cmdmode_record(struct list_head *mode,
			   struct option *opt, u32 flags)
{
	struct cmdmode *cm;

	list_for_each_entry(cm, mode, list) {
		if (*cm->ptr == cm->val || cm->opt == opt)
			continue;

		if (cm->opt)
			break;

		cm->opt = opt;
		cm->flags = flags;
	}

	if (list_entry_is_head(cm, mode, list))
		return;

	char *n1 = pretty_opt_name(cm->opt, cm->flags);
	char *n2 = pretty_opt_name(opt, flags);

	die(_("options %s and %s cannot be used together"), n1, n2);
}

static void parse_command(struct option *opts, const xchar *cmd)
{
	struct option *opt;

	opt_for_each(opt, opts) {
		if (opt->type != OPTION__COMMAND)
			continue;

		if (xc_strcmp(opt->lnam, cmd) != 0)
			continue;

		*(command_callback_t *)opt->ptr = opt->cmd;
		return;
	}

	char *name = pretty_arg_name(cmd, "����");

	die(_("unknown command `%s', see '%s -h'"), name, cmdname);
}

static void __setopt(switch)(struct option *opt, const xchar *arg, u32 flags)
{
	int *p = opt->ptr;

	*p = flags & OPT_UNSET ? 0 : 1;
}

static void __setopt(number)(struct option *opt, const xchar *arg, u32 flags)
{
	if (flags & OPT_UNSET) {
		*(u32 *)opt->ptr = 0;
		return;
	}

	int err;

	err = stx_strtou32(arg, 0, opt->ptr);
	if (!err)
		return;

	char *name = pretty_opt_name(opt, flags);
	char *val = pretty_arg_name(arg, "���");

	die(_("option %s has an invalid integer value `%s'"), name, val);
}

static void __setopt(string)(struct option *opt, const xchar *arg, u32 flags)
{
	const xchar **p = opt->ptr;

	if (flags & OPT_UNSET)
		*p = NULL;
	else if (arg)
		*p = arg;
	else if (opt->flags & OPT_OPT_ARG)
		*p = (const xchar *)opt->val;
}

static void __setopt(cmdmode)(struct option *opt, const xchar *arg, u32 flags)
{
	int *p = opt->ptr;

	*p = opt->val;
}

static void set_opt_val(struct param *ctx,
			struct option *opt, const xchar *arg, u32 flags)
{
	static typeof(__setopt(switch)) *map[__OPTION__SIZE] = {
		[OPTION_SWITCH]  = __setopt(switch),
		[OPTION_NUMBER]  = __setopt(number),
		[OPTION_STRING]  = __setopt(string),
		[OPTION_CMDMODE] = __setopt(cmdmode),
	};

	map[opt->type](opt, arg, flags);

	if (opt->type == OPTION_CMDMODE)
		cmdmode_record(&ctx->mode, opt, flags);
}

static const xchar *parse_shrt_opt(struct param *ctx, const xchar *args)
{
	struct option *opt;
	const xchar *arg = NULL;

	xchar snam = args[0];
	const xchar *ret = &args[1];

	opt_for_each(opt, ctx->opts) {
		if (opt->snam != snam)
			continue;

		if (opt->flags & OPT_NO_ARG) {
			arg = NULL;
		} else if (args[1]) {
			arg = &args[1];
			ret = XC("");
		} else if (ctx->argc > 1) {
			arg = ctx->argv[0];
			ctx->argc--;
			ctx->argv++;
		} else if (!(opt->flags & OPT_OPT_ARG)) {
			die(_("option -%c requires a value"), opt->snam);
		}

		set_opt_val(ctx, opt, arg, OPT_SHORT_OPT);
		return ret;
	}

	die(_("unknown option -%c"), (char)snam);
}

static void opt_abbrev(struct option *opt, u32 flags,
		       struct abbrev *abbrev, struct abbrev *ambig)
{
	if (abbrev->opt && abbrev->flags != flags) {
		ambig->opt = abbrev->opt;
		ambig->flags = abbrev->flags;
	}

	abbrev->opt = opt;
	abbrev->flags = flags;
}

static int __parse_long_opt(struct param *ctx, struct option *opt,
			    const xchar *arg, u32 argf, u32 optf)
{
	int unset = argf & OPT_UNSET;

	if (arg[0] == '=') {
		if (opt->flags & OPT_NO_ARG || unset)
			die(_("option `%s%s' takes no value"),
			    unset ? "no-" : "", opt->__lnam);
		arg += 1;
	} else if (arg[0]) {
		return -1;
	} else if ((opt->flags & OPT_NO_ARG) || unset) {
		arg = NULL;
	} else if (ctx->argc > 1) {
		arg = ctx->argv[1];
		ctx->argc--;
		ctx->argv++;
	} else if (!(opt->flags & OPT_OPT_ARG)) {
		die(_("option `%s%s' requires a value"),
		    unset ? "no-" : "", opt->__lnam);
	}

	set_opt_val(ctx, opt, arg, argf ^ optf);
	return 0;
}

static void parse_long_opt(struct param *ctx, const xchar *arg)
{
	int err;
	struct option *opt;

	const xchar *anam = arg;
	const xchar *asep = xc_strchrnul(arg, '=');
	u32 aflg = OPT_LONG_OPT;

	struct abbrev abbrev = { 0 };
	struct abbrev ambig = { 0 };

	err = strskip2(anam, XC("no-"), &anam);
	if (!err) {
		err = strskip2(anam, XC("no-"), &anam);

		if (err)
			aflg |= OPT_UNSET;
		else
			aflg |= NO_NO_ARG;
	}

	opt_for_each(opt, ctx->opts) {
		if (!opt->lnam || opt->type == OPTION__COMMAND)
			continue;

		const xchar *onam = opt->lnam;
		u32 oflg = OPT_LONG_OPT;
		int no_neg = opt->flags & OPT_NO_NEG;
		const xchar *rest = NULL;

		err = strskip2(onam, XC("no-"), &onam);
		if (!err)
			oflg |= OPT_UNSET;
		else if (aflg & NO_NO_ARG)
			continue;

		if ((aflg ^ oflg) & OPT_UNSET && no_neg)
			continue;

		err = strskip2(anam, onam, &rest);
		if (!err) {
			err = __parse_long_opt(ctx, opt, rest, aflg, oflg);
			if (err)
				continue;
			return;
		}

		if (xc_strncmp(onam, anam, asep - anam) == 0)
			opt_abbrev(opt, aflg ^ oflg, &abbrev, &ambig);

		if (!no_neg && strskip(XC("no-"), arg) != NULL)
			opt_abbrev(opt, OPT_UNSET ^ oflg, &abbrev, &ambig);
	}

	if (ambig.opt) {
		char *nam = pretty_arg_name(arg, "���");
		char *nam1 = pretty_opt_name(ambig.opt, ambig.flags);
		char *nam2 = pretty_opt_name(abbrev.opt, abbrev.flags);

		die(_("ambiguous option --%s, could be %s or %s"),
		    nam, &nam1[2], &nam2[2]);
	} else if (abbrev.opt) {
		arg = &asep[1];
		err = __parse_long_opt(ctx, abbrev.opt,
				       arg, aflg, abbrev.flags);
		BUG_ON(err);
	} else {
		char *nam = pretty_arg_name(arg, "���");

		die(_("unknown option --%s"), nam);
	}
}

static int parse_cmd_arg(struct param *ctx)
{
	const xchar *str = ctx->argv[0];

	if (str[0] != '-') {
		if (ctx->flags & PRM_STOP_AT_NON_OPT)
			return 39;

		if (ctx->flags & PRM_PARSE_COMMAND)
			parse_command(ctx->opts, str);

		ctx->outv[ctx->outc] = str;
		ctx->outc += 1;
	} else if (str[1] != '-') {
		str += 1;

		if (!(ctx->flags & PRM_NO_BUILTIN_HELP) &&
		    str[0] == 'h' && str[1] == 0)
			show_help(ctx->usage, ctx->opts);
		else if (str[0] == 0)
			die(_("unknown option -"));

		while (*str)
			str = parse_shrt_opt(ctx, str);
	} else {
		str += 2;

		/* end of options ("--") */
		if (str[0] == 0) {
			ctx->argc--;
			ctx->argv++;
			return 39;
		}

		if (!(ctx->flags & PRM_NO_BUILTIN_HELP) &&
		    xc_strcmp(str, XC("help")) == 0)
			show_help(ctx->usage, ctx->opts);

		parse_long_opt(ctx, str);
	}

	return 0;
}

int parse_param(int argc, const xchar **argv,
		const char **usage, struct option *opts, u32 flags)
{
	BUG_ON(match_bit(flags, PRM_STOP_AT_NON_OPT | PRM_PARSE_COMMAND));

	struct param ctx = {
		.argc  = argc - 1,
		.argv  = argv + 1,

		.outc  = argc,
		.outv  = argv,

		.opts  = opts,
		.usage = usage,

		.flags = flags,

		.mode  = LIST_HEAD_INIT(ctx.mode),
	};

	cmdname_append(argv[0]);
	cmdmode_init(&ctx.mode, ctx.opts);

	while (ctx.argc) {
		int ret = parse_cmd_arg(&ctx);

		if (unlikely(ret == 39))
			break;

		ctx.argc--;
		ctx.argv++;
	}

	if (!list_is_empty(&ctx.mode))
		cmdmode_destroy(&ctx.mode);

	if (ctx.argc)
		memmove(&ctx.outv[ctx.outc],
			ctx.argv, ctx.argc * sizeof(*ctx.argv));

	argc = ctx.outc + ctx.argc;
	ctx.outv[argc] = 0;
	return argc;
}

static void show_cmd_usage(const char **usage)
{
	const char *pref = "usage: ";
	size_t __len = strlen("usage: ");

	STRLIST(sl, SL_STORE__CHR);

	while (*usage) {
		const char *line = *usage;
		const char *rest = line;

		while (isalpha(*rest) || isspace(*rest))
			rest++;

		size_t len = rest - line;
		size_t pad = __len + len;
		size_t wrap = CONFIG_LINE_WRAP - __len;
		char *str;

		if ((CONFIG_LINE_WRAP >> 1) > pad)
			wrap -= len;

		sl_read_line_chr(&sl, rest, wrap);

		printf("%*s%.*s", (int)__len, pref, (int)len, line);
		str = sl_pop_chr(&sl);
		puts(str);
		free(str);

		while ((str = sl_pop_chr(&sl))) {
			printf("%*s%s\n", (int)pad, "", str);
			free(str);
		}

		usage++;
		pref = "or: ";
	}

	sl_destroy(&sl);
}

static void show_opt_usage(struct option *opts)
{
	struct option *opt;

	STRLIST(sl, SL_STORE__CHR);

	opt_for_each(opt, opts) {
		switch (opt->type) {
		case OPTION__GROUP:
			putchar('\n');
			puts(opt->__lnam);
		case OPTION__COMMAND:
			continue;
		}

		const char *fmt;
		size_t len = CONFIG_HELP_OPT_INDENT;

		printf("%*s", (int)len, "");

		if (opt->__snam)
			len += printf("-%c, ", opt->__snam);

		int no_neg = opt->flags & OPT_NO_NEG;

		fmt = no_neg ? "--%s" : "--[no-]%s";
		len += printf(fmt, opt->__lnam);

		if (opt->argh) {
			char *occur = strpbrk(opt->argh, "()<>[]|");
			int no_brac = occur != NULL;

			if (opt->flags & OPT_OPT_ARG)
				fmt = no_brac ? "[=%s]" : "[=<%s>]";
			else
				fmt = no_brac ? " %s" : " <%s>";
			len += printf(fmt, opt->argh);
		}

		if (len >= CONFIG_HELP_OPT_WRAP) {
			putchar('\n');
			len = 0;
		}

		size_t __pad = CONFIG_HELP_OPT_WRAP + 2;
		size_t pad = __pad - len;
		size_t wrap = CONFIG_LINE_WRAP - __pad;
		char *str;

		sl_read_line(&sl, _(opt->usage), wrap);

		str = sl_pop_chr(&sl);
		printf("%*s%s\n", (int)pad, "", str);
		free(str);

		while ((str = sl_pop_chr(&sl))) {
			printf("%*s%s\n", (int)__pad, "", str);
			free(str);
		}
	}

	sl_destroy(&sl);
}

static void show_help(const char **usage, struct option *opts)
{
	show_cmd_usage(usage);
	putchar('\n');

	show_opt_usage(opts);
	putchar('\n');

	exit(0);
}
