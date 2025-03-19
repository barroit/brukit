// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include "calc.h"
#include "param.h"

#ifdef __argc
# undef __argc
#endif

#ifdef __argv
# undef __argv
#endif

static const char *usage[] = {
	NULL,
	NULL,
};

#define DEFINE_ARGS(...)				\
	const xchar *__argv[] = { __VA_ARGS__ NULL };	\
	int __argc = sizeof_array(__argv) - 1;		\
	int argc = __argc;				\
	const xchar **argv = __argv;

static int cmd_fake(int argc, const xchar **argv)
{
	return 0;
}

UT_BEGIN();

UT_ROUTINE(PRM_STOP_AT_ARG)
{
	DEFINE_ARGS(
		XC("cmd"),
		XC("--a"),
		XC("3"),
		XC("arg"),
		XC("--b"),
		XC("9"),
	);

	u32 _;
	struct opt option[] = {
		OPT_NUMBER(0, "a", &_, ""),
		OPT_NUMBER(0, "b", &_, ""),
		OPT_NUMBER(0, "c", &_, ""),
		OPT_NUMBER(0, "d", &_, ""),
		OPT_END(),
	};

	argc = param_parse(argc, argv, usage, option, PRM_STOP_AT_ARG);
	UA_EQ(argc, 3);
	UA_STREQ(argv[0], XC("arg"));
	UA_STREQ(argv[1], XC("--b"));
	UA_STREQ(argv[2], XC("9"));

	argc = param_parse(argc, argv, usage, option, PRM_STOP_AT_ARG);
	UA_EQ(argc, 0);
}

UT_ROUTINE(PRM_PARSE_CMD)
{
	DEFINE_ARGS(
		XC("program"),
		XC("cmd"),
		XC("arg"),
	);

	cmd_cb cmd;
	struct opt option[] = {
		OPT_COMMAND("cmd", &cmd, cmd_fake),
		OPT_COMMAND("xxx", &cmd, NULL),
		OPT_END(),
	};

	argc = param_parse(argc, argv, usage, option, PRM_PARSE_CMD);
	UA_EQ(argc, 2);
	UA_STREQ(argv[0], XC("cmd"));
	UA_STREQ(argv[1], XC("arg"));
	UA_PTREQ(cmd, cmd_fake);
}

UT_ROUTINE(PRM_OPT_CMD)
{
	DEFINE_ARGS(
		XC("program"),
		XC("--test"),
		XC("arg"),
	);

	cmd_cb cmd = cmd_fake;
	struct opt option[] = {
		OPT_COMMAND("cmd", &cmd, cmd_fake),
		OPT_END(),
	};

	__cmdpath_reset();

	argc = param_parse(argc, argv, usage, option, PRM_OPT_CMD);
	UA_EQ(argc, 3);
	UA_STREQ(argv[0], XC("cmd"));
	UA_STREQ(argv[1], XC("--test"));
	UA_STREQ(argv[2], XC("arg"));

	const xchar *cmdpath_old = cmdpath();

	argc = param_parse(argc, argv, usage, option,
			   PRM_KEEP_ARG0 | PRM_STOP_AT_UNKNOWN);
	UA_EQ(argc, 3);

	const xchar *cmdpath_new = cmdpath();

	UA_STREQ(cmdpath_old, cmdpath_new);
}

UT_ROUTINE(PRM_KEEP_ARG0)
{
	DEFINE_ARGS(
		XC("program"),
		XC("--test"),
		XC("1"),
		XC("arg1"),
		XC("arg2"),
	);

	u32 test;
	struct opt option[] = {
		OPT_NUMBER(0, "test", &test, ""),
		OPT_END(),
	};

	argc = param_parse(argc, argv, usage, option, PRM_KEEP_ARG0);
	UA_EQ(argc, 3);
	UA_EQ(test, 1);
	UA_STREQ(argv[0], XC("program"));
	UA_STREQ(argv[1], XC("arg1"));
	UA_STREQ(argv[2], XC("arg2"));
	UA_ZERO(argv[3]);
}

UT_ROUTINE(PRM_STOP_AT_UNKNOWN)
{
	DEFINE_ARGS(
		XC("program"),
		XC("--test"),
		XC("1"),
		XC("arg"),
	);

	struct opt option[] = {
		OPT_END(),
	};

	argc = param_parse(argc, argv, usage, option, PRM_STOP_AT_UNKNOWN);
	UA_EQ(argc, 3);
	UA_STREQ(argv[0], XC("--test"));
	UA_STREQ(argv[1], XC("1"));
	UA_STREQ(argv[2], XC("arg"));
	UA_ZERO(argv[3]);
}

UT_END();
