// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include "strbuf.h"
#include "path.h"

#define SEP XC(PTH_SEP_STR)

UT_BEGIN();

UT_ROUTINE(sb_pth_init_reinit)
{
	struct strbuf __cleanup(sb_pth_destroy) *sb;

	sb_pth_init(&sb, XC("/"));
	UA_STREQ(sb->buf, XC("/"));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("/"));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("/"));

	sb_pth_push(sb, XC("dev"));
	UA_STREQ(sb->buf, XC("/dev"));

	sb_pth_reinit(&sb, XC("."));
	UA_STREQ(sb->buf, XC("."));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("."));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("."));

	sb_pth_push(sb, XC("git"));
	UA_STREQ(sb->buf, XC(".") SEP XC("git"));
}

UT_ROUTINE(sb_pth_abs_push_pop)
{
	struct strbuf __cleanup(sb_pth_destroy) *sb;

	sb_pth_init(&sb, XC("/path/to/file"));
	UA_STREQ(sb->buf, XC("/path/to/file"));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("/path/to"));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("/path"));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("/"));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("/"));

	sb_pth_join(sb, XC("path"), XC("to"), XC("this"),
		    XC("fucking"), XC("dumbass"), XC("file"));
	UA_STREQ(sb->buf, XC("/path") SEP XC("to") SEP XC("this") SEP
		 XC("fucking") SEP XC("dumbass") SEP XC("file"));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("/path") SEP XC("to") SEP XC("this") SEP
		 XC("fucking") SEP XC("dumbass"));
}

UT_ROUTINE(sb_pth_rel_push_pop)
{
	struct strbuf __cleanup(sb_pth_destroy) *sb;

	sb_pth_init(&sb, XC("path/to/file"));
	UA_STREQ(sb->buf, XC("path/to/file"));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("path/to"));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("path"));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("."));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("."));

	sb_pth_join(sb, XC("path"), XC("to"), XC("this"),
		    XC("fucking"), XC("dumbass"), XC("file"));
	UA_STREQ(sb->buf, XC(".") SEP
		 XC("path") SEP XC("to") SEP XC("this") SEP
		 XC("fucking") SEP XC("dumbass") SEP XC("file"));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC(".") SEP XC("path") SEP XC("to") SEP
		 XC("this") SEP XC("fucking") SEP XC("dumbass"));
}

UT_ROUTINE(sb_pth_abs_cwd)
{
	struct strbuf __cleanup(sb_pth_destroy) *sb;

	sb_pth_init(&sb, XC("/home/barroit"));

	sb_pth_sync_cwd(sb);

	sb_pth_push_cwd(sb, XC("git"));
	UA_STREQ(sb->buf, XC("/home/barroit") SEP XC("git"));

	sb_pth_push_cwd(sb, XC("wasabi"));
	UA_STREQ(sb->buf, XC("/home/barroit") SEP XC("wasabi"));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("/home/barroit"));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("/home"));

	sb_pth_push(sb, XC("miku"));
	UA_STREQ(sb->buf, XC("/home") SEP XC("miku"));

	sb_pth_sync_cwd(sb);

	sb_pth_push_cwd(sb, XC("git"));
	UA_STREQ(sb->buf, XC("/home") SEP XC("miku") SEP XC("git"));

	sb_pth_push_cwd(sb, XC("wasabi"));
	UA_STREQ(sb->buf, XC("/home") SEP XC("miku") SEP XC("wasabi"));

	/*
	 * Don't do this in your code
	 */
	sb_trunc(sb, sb->len);
	sb_pth_sync_cwd(sb);

	sb_pth_push_cwd(sb, XC("sandbox"));
	UA_STREQ(sb->buf, XC("sandbox"));
}

UT_ROUTINE(sb_pth_rel_cwd)
{
	struct strbuf __cleanup(sb_pth_destroy) *sb;

	sb_pth_init(&sb, XC("./git/brukit"));

	sb_pth_sync_cwd(sb);

	sb_pth_push_cwd(sb, XC("linux"));
	UA_STREQ(sb->buf, XC("./git/brukit") SEP XC("linux"));

	sb_pth_push_cwd(sb, XC("darwin"));
	UA_STREQ(sb->buf, XC("./git/brukit") SEP XC("darwin"));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("./git/brukit"));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("./git"));

	sb_pth_sync_cwd(sb);

	sb_pth_push(sb, XC("barroit/posix"));
	UA_STREQ(sb->buf, XC("./git") SEP XC("barroit/posix"));

	sb_pth_push_cwd(sb, XC("cred"));
	UA_STREQ(sb->buf, XC("./git") SEP XC("cred"));

	sb_pth_push_cwd(sb, XC("itsconnner"));
	UA_STREQ(sb->buf, XC("./git") SEP XC("itsconnner"));

	sb_trunc(sb, sb->len);
	sb_pth_sync_cwd(sb);

	sb_pth_push_cwd(sb, XC("sandbox"));
	UA_STREQ(sb->buf, XC("sandbox"));
}

UT_ROUTINE_WIN32(sb_pth_push_pop)
{
	struct strbuf __cleanup(sb_pth_destroy) *sb;

	sb_pth_init(&sb, XC("C:\\path\\to\\file"));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("C:\\path\\to"));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("C:\\path"));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("C:\\"));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("C:\\"));

	sb_pth_reinit(&sb, XC("\\\\127.0.0.1\\c$\\file"));
	UA_STREQ(sb->buf, XC("\\\\127.0.0.1\\c$\\file"));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("\\\\127.0.0.1\\c$"));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("\\\\127.0.0.1\\"));

	sb_pth_pop(sb);
	UA_STREQ(sb->buf, XC("\\\\127.0.0.1\\"));

	sb_pth_push(sb, XC("clumsy"));
	UA_STREQ(sb->buf, XC("\\\\127.0.0.1\\clumsy"));

	sb_pth_push(sb, XC("win32"));
	UA_STREQ(sb->buf, XC("\\\\127.0.0.1\\clumsy\\win32"));

	sb_pth_push(sb, XC("API"));
	UA_STREQ(sb->buf, XC("\\\\127.0.0.1\\clumsy\\win32\\API"));
}

UT_END();
