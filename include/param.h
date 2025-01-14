/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_PARAM_H
#define NG39_PARAM_H

#include "compiler.h"
#include "gettext.h"
#include "types.h"
#include "xchar.h"

enum option_type {
	OPTION_SWITCH,
	OPTION_NUMBER,
	OPTION_STRING,
	OPTION_CMDMODE,

	__OPTION__SIZE,

	OPTION__END,
	OPTION__GROUP,
	OPTION__COMMAND,
};

#define OPT_NO_ARG  (1 << 0)
#define OPT_OPT_ARG (1 << 1)
#define OPT_NO_NEG  (1 << 2)

typedef int (*command_callback_t)(int, const char **);

struct option {
	enum option_type type;

	xchar snam;
	const xchar *lnam;

	char __snam;
	const char *__lnam;

	void *ptr;
	union {
		intptr_t val;
		command_callback_t cmd;
	};

	const char *argh;
	const char *usage;

	u32 flags;
};

#define PRM_STOP_AT_NON_OPT (1 << 0)
#define __PRM_PARSE_COMMAND (1 << 1)
#define PRM_NO_BUILTIN_HELP (1 << 2)

int parse_param(int argc, const xchar **argv,
		const char **usage, struct option *opts, u32 flags);

#define __opt_snam(s) __st_isascii(s) + XC(s)
#define __opt_lnam(l) XC(l)
#define __opt_ptr(p, t) __st_same_type(p, t *) + p

#define OPT_END()    { .type = OPTION__END }
#define OPT_GROUP(l) { .type = OPTION__GROUP, .__lnam = l }

#define OPT_SWITCH(s, l, p, h) __OPT_SWITCH(s, l, p, h, 0)
#define __OPT_SWITCH(s, l, p, h, f) {	\
	.type   = OPTION_SWITCH,	\
	.snam   = __opt_snam(s),	\
	.lnam   = __opt_lnam(l),	\
	.__snam = s,			\
	.__lnam = l,			\
	.ptr    = __opt_ptr(p, int),	\
	.usage  = h,			\
	.flags  = OPT_NO_ARG | (f),	\
}

#define OPT_NUMBER(s, l, p, h) __OPT_NUMBER(s, l, p, h, 0)
#define __OPT_NUMBER(s, l, p, h, f) {	\
	.type   = OPTION_NUMBER,	\
	.snam   = __opt_snam(s),	\
	.lnam   = __opt_lnam(l),	\
	.__snam = s,			\
	.__lnam = l,			\
	.ptr    = __opt_ptr(p, u32),	\
	.argh   = "N",			\
	.usage  = h,			\
	.flags  = f,			\
}

#define OPT_STRING(s, l, p, h) __OPT_STRING(s, l, p, h, OPT_NO_NEG)
#define __OPT_STRING(s, l, p, h, f) ___OPT_STRING(s, l, p, 0, h, f)
#define ___OPT_STRING(s, l, p, a, h, f) ____OPT_STRING(s, l, p, 0, a, h, f)
#define ____OPT_STRING(s, l, p, v, a, h, f) {	\
	.type   = OPTION_STRING,		\
	.snam   = __opt_snam(s),		\
	.lnam   = __opt_lnam(l),		\
	.__snam = s,				\
	.__lnam = l,				\
	.ptr    = __opt_ptr(p, const xchar *),	\
	.val    = v,				\
	.argh   = a,				\
	.usage  = h,				\
	.flags  = f,				\
}

#define OPT_FILENAME(s, l, p, h) __OPT_FILENAME(s, l, p, h, OPT_NO_NEG)
#define __OPT_FILENAME(s, l, p, h, f) ___OPT_STRING(s, l, p, N_("path"), h, f)

#define OPT_COMMAND(l, p, c) __OPT_COMMAND(l, p, c, 0)
#define __OPT_COMMAND(l, p, c, f) {			\
	.type   = OPTION__COMMAND,			\
	.lnam   = __opt_lnam(l),			\
	.__snam = s,					\
	.__lnam = l,					\
	.ptr    = __opt_ptr(p, command_callback_t),	\
	.cmd    = c,					\
	.flags  = f,					\
}

#define OPT_CMDMODE(s, l, p, v, h) __OPT_CMDMODE(s, l, p, v, h, OPT_NO_NEG)
#define __OPT_CMDMODE(s, l, p, v, h, f) {	\
	.type   = OPTION_CMDMODE,		\
	.snam   = __opt_snam(s),		\
	.lnam   = __opt_lnam(l),		\
	.__snam = s,				\
	.__lnam = l,				\
	.ptr    = p,				\
	.val    = v,				\
	.usage  = h,				\
	.flags  = OPT_NO_ARG | (f),		\
}

#endif /* NG39_PARAM_H */
