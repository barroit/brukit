// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#include "termas.h"

#include <ctype.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "gettext.h"
#include "iter.h"
#include "scio.h"
#include "size.h"
#include "tercol.h"
#include "timestamp.h"
#include "udef.h"

struct tag {
	const char *name;
	const char *colored;
};

static struct tag tags[] = {
	[TM_LOG]   = { NULL, NULL },
	[TM_HINT]  = { N_("hint:"),  HN_("hint:",  BOLD, YELLOW) },
	[TM_WARN]  = { N_("warn:"),  HN_("warn:",  BOLD, MAGENTA) },
	[TM_ERROR] = { N_("error:"), HN_("error:", BOLD, RED) },
	[TM_FATAL] = { N_("fatal:"), HN_("fatal:", BOLD, RED) },
	[TM_BUG]   = { N_("BUG:"),   HN_("BUG:",   BOLD, RED, BG_BLACK) },
};

static inline int good_cntrl(char c)
{
	return c == '\t' || c == '\n' || c == '\033';
}

static inline int bad_cntrl(char c)
{
	return isascii(c) && iscntrl(c) && !good_cntrl(c);
}

static size_t rm_bad_cntrl(char *buf, size_t len, size_t limit)
{
	size_t i;
	size_t dropped = 0;

	idx_for_each(i, len) {
		if (bad_cntrl(buf[i]))
			dropped++;
	}

	if (dropped == 0)
		return len;

	size_t sub_len = strlen(CONFIG_CNTRL_SUB);
	size_t sub_delta = sub_len - 1;
	size_t need = sub_delta * dropped;

	int overflow = need > limit - len;
	char *sub = CONFIG_CNTRL_SUB;

	if (overflow)
		sub = "?";

	if (sub_len == 1 || overflow) {
		idx_for_each(i, len) {
			if (bad_cntrl(buf[i]))
				buf[i] = sub[0];
		}

		return len;
	}

	size_t tail = len - 1;
	size_t ret = len + need;

	idx_for_each_reverse(i, len - 1) {
		if (!bad_cntrl(buf[i]))
			continue;

		size_t good = i + 1;
		size_t next = good + need;

		if (likely(i != len - 1))
			memmove(&buf[next], &buf[good], tail - i);

		memcpy(&buf[next - sub_len], sub, sub_len);
		need -= sub_delta;
		tail = i - 1;
	}

	return ret;
}

static int consume_room(size_t len, size_t *used, size_t *room)
{
	if (len < *room) {
		*used += len;
		*room -= len;
	} else {
		*used += *room;
		*room = 0;
	}

	return !!*room;
}

int __termas(const char *file, int line,
	     const char *func, enum tm_level level,
	     const char *hint, u32 flags, const char *fmt, ...)
{
	int debug = flags & MAS_OUT_DEBUG;

	if (!udef_verbose && debug)
		return 0;

	char buf[SZ_4K];
	size_t len = 0;
	size_t limit = sizeof(buf) - 1;

	struct tag *tag = &tags[level];
	size_t room = limit;
	int ret;

	if (udef_termas_ts || !tag->name || debug) {
		struct timespec ts;
		const char *ts_fmt;

		if (!debug) {
			ts_fmt = H("[%" PRIu64 ".%" PRIu64 "] ", GREEN);

			if (!udef_use_tercol)
				ts_fmt = "[%" PRIu64 ".%" PRIu64 "] ";

		} else {
			ts_fmt = H(" %" PRIu64 ".%" PRIu64 " + ", GREEN);

			if (!udef_use_tercol)
				ts_fmt = "%" PRIu64 ".%" PRIu64 " + ";
		}

		ts_mono(&ts);

		ret = snprintf(&buf[len], room + 1, ts_fmt,
			       ts.tv_sec, ts.tv_nsec / 1000);
		consume_room(ret, &len, &room);
	}

	if (udef_termas_pid) {
		long pid = getpid();
		const char *pid_fmt = H(">", BOLD) "%ld ";

		if (!udef_use_tercol)
			pid_fmt = ">%ld ";

		ret = snprintf(&buf[len], room + 1, pid_fmt, pid);
		consume_room(ret, &len, &room);
	}

	if (tag->name) {
		int no_pad = flags & (MAS_SHOW_FILE | MAS_SHOW_FUNC);
		const char *name = tag->colored;
		size_t n = 1;

		if (no_pad)
			n = 0;
		if (!udef_use_tercol)
			name = tag->name;

		name = _(name);
		n += strlen(name);

		if (n > room)
			n = room;

		memcpy(&buf[len], name, n);
		consume_room(n, &len, &room);

		if (!no_pad)
			buf[len - 1] = ' ';
	}

	if (flags & MAS_SHOW_FILE) {
		const char *pos_fmt = H("%s:%d:%s", BOLD);
		int no_pad = flags & MAS_SHOW_FUNC;
		const char *pad = " ";

		if (!udef_use_tercol)
			pos_fmt = "%s:%d:%s";
		if (no_pad)
			pad = "";

		ret = snprintf(&buf[len], room + 1, pos_fmt, pad);
		if (!consume_room(ret, &len, &room))
			goto out;
	}

	if (flags & MAS_SHOW_FUNC) {
		const char *pos_fmt = H("%s:%d:%s", BOLD);

		if (!udef_use_tercol)
			pos_fmt = "%s: ";

		ret = snprintf(&buf[len], room + 1, pos_fmt, func);
		if (!consume_room(ret, &len, &room))
			goto out;
	}

	/*
	 * Must have prefix if fmt is empty, or it segfaults.
	 */
	if (!fmt[0]) {
		const char *suffix = __fmtcol(RESET);
		size_t suffix_len = strlen(suffix);

		if (udef_use_tercol)
			len -= suffix_len;

		while (isspace(buf[len - 1]) || buf[len - 1] == ':')
			len--;

		if (udef_use_tercol) {
			memcpy(&buf[len], suffix, suffix_len);
			len += suffix_len;
		}

		goto out;
	}

	va_list ap;

	va_start(ap, fmt);
	ret = vsnprintf(&buf[len], room + 1, fmt, ap);
	va_end(ap);

	if (!consume_room(ret, &len, &room))
		goto out;

	if (hint) {
		if (room) {
			buf[len++] = ';';
			room--;
		}

		if (room) {
			buf[len++] = ' ';
			room--;
		}

		if (!room)
			goto out;

		size_t hint_len = strlen(hint);

		if (hint_len > room)
			hint_len = room;

		memcpy(&buf[len], hint, hint_len);
		if (!consume_room(hint_len, &len, &room))
			goto out;
	}

out:
	len = rm_bad_cntrl(buf, len, limit);

	int out = -1;
	int fd = STDERR_FILENO;
	FILE *stream = stderr;

	if (flags & MAS_TO_STDOUT) {
		fd = STDOUT_FILENO;
		stream = stdout;
		out = 0;
	}

	buf[len] = '\n';
	len += 1;

	fflush(stream);
	xwrite(fd, buf, len);

	switch (level) {
	case TM_BUG:
		abort();
	case TM_FATAL:
		if (!(flags & MAS_NO_EXIT))
			exit(128);
	default:
		return out;
	}
}
