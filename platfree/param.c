// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#include "param.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "iter.h"
#include "list.h"
#include "noleak.h"
#include "strbuf.h"
#include "strconv.h"
#include "strtox.h"
#include "termas.h"
#include "xalloc.h"
#include "xcf.h"

#define PARSE_DONE 39
#define PARSE_CMD  -39

#define OPT_SHORT_OPT (1 << 0)
#define OPT_LONG_OPT  (1 << 1)
#define OPT_UNSET     (1 << 2)

#define __setopt(name) opt_set_ ## name

#define NO_NO_ARG (-1U << 31)

struct cname {
	const xchar *name;
	struct list_head list;
};

struct param {
	int argc;
	const xchar **argv;

	int outc;
	const xchar **outv;

	struct opt *opts;
	const char **usage;

	u32 flags;

	struct list_head mode;
};

struct abbrev {
	struct opt *opt;
	u32 flags;
};

struct cmdmode {
	struct opt *opt;
	u32 flags;

	struct list_head list;
};

static intptr_t ex[32];

static void ex_init(u32 flags, va_list ap)
{
	u32 flag;

	for (flag = PRM_EX_BEGIN; flag != 0; flag <<= 1) {
		if (flags & flag)
			ex[ctz(flag)] = va_arg(ap, intptr_t);
	}
}

static intptr_t __ex_get(u32 flag)
{
	return ex[ctz(flag)];
}
#define ex_get(flag, type) ((type)__ex_get(flag))

static int has_command(struct opt *opts)
{
	struct opt *opt;

	opt_for_each(opt, opts) {
		if (opt->mode == OPTION__COMMAND)
			return 1;
	}

	return 0;
}

static LIST_HEAD(cnames);

static int cmdpath_is_prev(const xchar *name)
{
	if (list_is_empty(&cnames))
		return 0;

	struct cname *cn = list_last_entry(&cnames, typeof(*cn), list);

	return cn->name == name;
}

static void cmdpath_add(const xchar *name)
{
	struct cname *cn = xmalloc(sizeof(*cn));

	cn->name = name;
	list_add_tail(&cn->list, &cnames);
}

const xchar *cmdpath(void)
{
	static struct strbuf sb = SB_INIT;
	struct cname *cn;

	if (sb.len)
		sb_trunc(&sb, sb.len);

	list_for_each_entry(cn, &cnames, list) {
		sb_puts(&sb, cn->name);
		sb_putc(&sb, XC(' '));
	}

	sb_trunc(&sb, 1);
	return sb.buf;
}

void __cmdpath_reset(void)
{
	if (list_is_empty(&cnames))
		return;

	struct cname *cn;
	struct cname *tmp;

	list_for_each_entry_safe(cn, tmp, &cnames, list) {
		list_del(&cn->list);
		free(cn);
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

static char *pretty_opt_name(struct opt *opt, u32 flags)
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

	if (IS_ENABLED(CONFIG_ENABLE_WCHAR))
		mb_wcstombs_fb(&ret, (wchar_t *)arg, fb);

	return ret;
}

static void cmdmode_record(struct list_head *mode,
			   struct opt *opt, u32 flags)
{
	struct cmdmode *cm;

	list_for_each_entry(cm, mode, list) {
		if (cm->opt == opt)
			continue;

		char *n1 = pretty_opt_name(cm->opt, cm->flags);
		char *n2 = pretty_opt_name(opt, flags);

		die(_("options %s and %s cannot be used together"), n1, n2);
	}

	cm = xcalloc(1, sizeof(*cm));
	list_add(&cm->list, mode);

	cm->opt = opt;
	cm->flags = flags;
}

static int parse_command(struct param *ctx, const xchar *cmd)
{
	struct opt *opt;

	opt_for_each(opt, ctx->opts) {
		if (opt->mode != OPTION__COMMAND)
			continue;

		if (xc_strcmp(opt->lnam, cmd) != 0)
			continue;

		*(cmd_cb *)opt->ptr = opt->cmd;
		return PARSE_CMD;
	}

	if (ctx->flags & PRM_STOP_AT_UNKNOWN)
		return PARSE_DONE;

	char *name = pretty_arg_name(cmd, "����");

	die(_("unknown command `%s', see '%s -h'"), name, cmdpath());
}

static void __setopt(bit)(struct opt *opt, const xchar *arg, u32 flags)
{
	u32 *p = opt->ptr;

	if (flags & OPT_UNSET)
		*p &= ~opt->val;
	else
		*p |= opt->val;
}

static void __setopt(switch)(struct opt *opt, const xchar *arg, u32 flags)
{
	int *p = opt->ptr;

	*p = flags & OPT_UNSET ? 0 : 1;
}

static void __setopt(number)(struct opt *opt, const xchar *arg, u32 flags)
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

static void __setopt(string)(struct opt *opt, const xchar *arg, u32 flags)
{
	const xchar **p = opt->ptr;

	if (flags & OPT_UNSET)
		*p = NULL;
	else if (arg)
		*p = arg;
	else if (opt->flags & OPT_OPT_ARG)
		*p = (const xchar *)opt->val;
}

static void __setopt(cmdmode)(struct opt *opt, const xchar *arg, u32 flags)
{
	int *p = opt->ptr;

	*p = opt->val;
}

static void __setopt(choice)(struct opt *opt, const xchar *arg, u32 flags)
{
	const xchar **p = opt->ptr;

	if (flags & OPT_UNSET)
		*p = NULL;
	else
		*p = arg;

	const xchar *str = *p;
	const xchar **choice = (const xchar **)opt->val;

	if (!str)
		return;

	while (*choice) {
		if (xc_strcmp(str, *choice++) == 0)
			return;
	}

	struct strbuf sb = SB_INIT;

	choice = (const xchar **)opt->val;
	while (*choice)
		sb_printf(&sb, XC("  %s\n"), *choice++);

	sb_trunc(&sb, 1);
	die(_("unknown choice `%s', valid choices are:\n%s"), str, sb.buf);
}

static void set_opt_val(struct param *ctx,
			struct opt *opt, const xchar *arg, u32 flags)
{
	static typeof(__setopt(switch)) *map[__OPTION__SIZE] = {
		[OPTION_BIT]     = __setopt(bit),
		[OPTION_SWITCH]  = __setopt(switch),
		[OPTION_NUMBER]  = __setopt(number),
		[OPTION_STRING]  = __setopt(string),
		[OPTION_CMDMODE] = __setopt(cmdmode),
		[OPTION_CHOICE]  = __setopt(choice),
	};

	map[opt->mode](opt, arg, flags);

	if (opt->mode == OPTION_CMDMODE)
		cmdmode_record(&ctx->mode, opt, flags);
}

static int parse_shrt_opt(struct param *ctx, const xchar **__args)
{
	struct opt *opt;
	const xchar *arg = NULL;
	const xchar *args = *__args;

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
		*__args = ret;
		return 0;
	}

	if (ctx->flags & PRM_STOP_AT_UNKNOWN)
		return PARSE_DONE;

	die(_("unknown option -%c"), (char)snam);
}

static void opt_abbrev(struct opt *opt, u32 flags,
		       struct abbrev *abbrev, struct abbrev *ambig)
{
	if (abbrev->opt && abbrev->flags != flags) {
		ambig->opt = abbrev->opt;
		ambig->flags = abbrev->flags;
	}

	abbrev->opt = opt;
	abbrev->flags = flags;
}

static int __parse_long_opt(struct param *ctx, struct opt *opt,
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

static int parse_long_opt(struct param *ctx, const xchar *arg)
{
	int err;
	struct opt *opt;

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
		if (!opt->lnam || opt->mode == OPTION__COMMAND)
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
			return 0;
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
		arg = XC("");
		if (*asep)
			arg = asep;

		err = __parse_long_opt(ctx, abbrev.opt,
				       arg, aflg, abbrev.flags);
		BUG_ON(err);
		return 0;
	} else {
		if (ctx->flags & PRM_STOP_AT_UNKNOWN)
			return PARSE_DONE;

		char *nam = pretty_arg_name(arg, "���");

		die(_("unknown option --%s"), nam);
	}
}

static int parse_cmd_arg(struct param *ctx)
{
	int ret = 0;
	const xchar *str = ctx->argv[0];

	if (str[0] != '-') {
		if (ctx->flags & PRM_STOP_AT_ARG)
			return PARSE_DONE;
		else if (ctx->flags & PRM_PARSE_CMD)
			return parse_command(ctx, str);
		else if (ctx->flags & PRM_NO_ARG)
			die(_("'%s' takes no arguments, but got `%s'"),
			    cmdpath(), str);

		ctx->outv[ctx->outc] = str;
		ctx->outc += 1;
	} else if (str[1] != '-') {
		str += 1;

		if (!(ctx->flags & PRM_NO_DEF_HELP) &&
		    str[0] == 'h' && str[1] == 0)
			param_show_help(ctx->usage, ctx->opts, 0);
		else if (str[0] == 0)
			die(_("unknown option -"));

		while (*str && ret == 0)
			ret = parse_shrt_opt(ctx, &str);
	} else {
		str += 2;

		/* end of options ("--") */
		if (str[0] == 0) {
			ctx->argc--;
			ctx->argv++;
			return PARSE_DONE;
		}

		if (!(ctx->flags & PRM_NO_DEF_HELP) &&
		    xc_strcmp(str, XC("help")) == 0)
			param_show_help(ctx->usage, ctx->opts, 0);

		ret = parse_long_opt(ctx, str);
	}

	return ret;
}

static void err_huge_arg(int argc, const xchar **argv)
{
	int i;
	const char *fmt = N_("'%s' takes no extra arguments:\n%s\n");
	struct strbuf sb = SB_INIT;

	for_each_idx(i, argc)
		sb_printf(&sb, XC("  %s\n"), argv[i]);
	sb_trunc(&sb, 1);

	if (argc == 1)
		fmt = N_("'%s' takes no extra argument:\n%s\n");

	error(_(fmt), cmdpath(), sb.buf);
	noleak(sb);
}

static const xchar *def_cmd_name(struct opt *opts)
{
	struct opt *opt;

	opt_for_each(opt, opts) {
		if (opt->mode != OPTION__COMMAND)
			continue;
		if (*(cmd_cb *)opt->ptr == opt->cmd)
			return opt->lnam;
	}

	trap();
}

int param_parse(int argc, const xchar **argv,
		const char **usage, struct opt *opts, u32 flags, ...)
{
	if (flags & PRM_OPT_CMD)
		flags |= PRM_STOP_AT_UNKNOWN;

	if (has_command(opts))
		flags |= PRM_PARSE_CMD;

	BUG_ON(argc < 1);
	BUG_ON(flags & PRM_PARSE_CMD && flags & PRM_STOP_AT_ARG);
	BUG_ON(flags & PRM_OPT_CMD && flags & PRM_KEEP_ARG0);

	if (flags & PRM_EX_MASK) {
		va_list ap;

		va_start(ap, flags);
		ex_init(flags, ap);
		va_end(ap);
	}

	if (!cmdpath_is_prev(argv[0]))
		cmdpath_add(argv[0]);

	struct param ctx = {
		.argc  = argc - 1,
		.argv  = argv + 1,

		.outc  = 0,
		.outv  = argv,

		.opts  = opts,
		.usage = usage,

		.flags = flags,

		.mode  = LIST_HEAD_INIT(ctx.mode),
	};
	int argv_has_cmd = 0;

	if (flags & PRM_KEEP_ARG0)
		ctx.outc = 1;

	while (ctx.argc) {
		int ret = parse_cmd_arg(&ctx);

		switch (ret) {
		case PARSE_CMD:
			argv_has_cmd = 1;
		case PARSE_DONE:
			goto out;
		}

		ctx.argc--;
		ctx.argv++;
	}

out:
	if (!list_is_empty(&ctx.mode))
		cmdmode_destroy(&ctx.mode);

	if (flags & PRM_OPT_CMD && !argv_has_cmd) {
		ctx.outv[0] = def_cmd_name(opts);
		ctx.outc += 1;
	}

	int ret = ctx.outc + ctx.argc;

	if (flags & PRM_PARSE_CMD && !(flags & PRM_OPT_CMD) && ret == 0) {
		int err = argc - 1;

		if (err)
			error(_("'%s' requires a subcommand\n"), cmdpath());
		param_show_help(ctx.usage, ctx.opts, err);
	}

	if (ctx.argc)
		memmove(&ctx.outv[ctx.outc],
			ctx.argv, ctx.argc * sizeof(*ctx.argv));

	if (ctx.flags & PRM_LIM_ARG) {
		uint limit = ex_get(PRM_LIM_ARG, uint);

		if (ret > limit) {
			err_huge_arg(ret - limit, &ctx.outv[limit]);
			param_show_help(usage, opts, 1);
		}
	}

	ctx.outv[ret] = NULL;
	return ret;
}
