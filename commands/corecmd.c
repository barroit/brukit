// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#include "corecmd.h"

#include "param.h"
#include "proc.h"
#include "termas.h"
#include "udef.h"

static const char *usage[] = {
	PROGRAM_NAME " [--termas <dest> | --no-termas] [--no-tercol]"
		     " [--show-ts] [--show-pid] [--verbose]"
		     " <command> [<args>]",
	NULL,
	NULL,
};

static cmd_cb subcmd;

static struct opt opts[] = {
	OPT_GROUP(N_("Format options:")),
	OPT_SWITCH(0, "tercol", &udef_use_tercol,
		   N_("enable colorized terminal output")),
	OPT_FLAG(0, "show-ts", &udef_termas_ts,
		N_("prefix messages with timestamp")),
	OPT_FLAG(0, "show-pid", &udef_termas_pid,
		N_("prefix messages with current PID")),

	OPT_GROUP(N_("Output options:")),
	__OPT_STRING(0, "termas", &udef_termas_dest,
		     N_("specify output destination"), 0),
	OPT_FLAG(0, "verbose", &udef_verbose,
		N_("verbose output")),
	OPT_COMMAND("version", &subcmd, cmd_version),
	OPT_END(),
};

int cmd_main(int argc, const xchar **argv)
{
	argc = param_parse(argc, argv, usage, opts, PRM_PARSE_CMD);

	if (udef_termas_dest != UDEF_DEF_PTR) {
		const xchar *out = udef_termas_dest ? : XC(CONFIG_NULL_DEVICE);
		int err = proc_redir_std(out, PROC_REDIR_OUT | PROC_REDIR_ERR);

		if (err)
			warn(_("failed to run process in silent mode"));
	}

	return subcmd(argc, argv);
}
