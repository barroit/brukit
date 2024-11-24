// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "termas.h"
#include "tercol.h"
#include "scio.h"
#include "iter.h"
#include "udef.h"
#include "timestamp.h"

struct tm_tag {
	const char *name;
	const char *colored;
};

#ifdef CONFIG_TERMAS_SMALL_BUFFER
# define PUTF_BUF_CAP SZ_1K
#else
# define PUTF_BUF_CAP SZ_4K
#endif

#ifdef CONFIG_SPEC_ALT_CNTRL
# define ALT_CNTRL CONFIG_ALT_CNTRL
#else
# define ALT_CNTRL "?"
#endif

#define __test_add_buf_size(__nr, __size, __avail)	\
({							\
	int __pass = 1;					\
	if (__nr < *(__avail)) {			\
		*(__size) += __nr;			\
		*(__avail) -= __nr;			\
	} else {					\
		*(__size) += *(__avail);		\
		__pass = 0;				\
	}						\
	__pass;						\
})

static inline int char_is_good_cntrl(char c)
{
	return c == '\t' || c == '\n' || c == '\033';
}

static inline int char_is_bad_cntrl(char c)
{
	return isascii(c) && iscntrl(c) && !char_is_good_cntrl(c);
}

static size_t rm_bad_cntrl(char *buf, size_t size, size_t cap)
{
	size_t i;
	uint cnt = 0;

	idx_for_each(i, size)
		if (char_is_bad_cntrl(buf[i])) 
			cnt++;

	if (cnt == 0)
		return size;

	/*
	 * Minus 1 in here is because bad control characters have already
	 * reserved 1 char for us.
	 */
	size_t len = strlen(ALT_CNTRL);
	size_t unit = len - 1;
	int do_fast = len == 1;
	int no_room = unit * cnt > cap - size;
	char *alt = ALT_CNTRL;

	if (no_room)
		alt = "?";
	if (do_fast || no_room) {
		idx_for_each(i, size)
			if (char_is_bad_cntrl(buf[i]))
				buf[i] = alt[0];
		return size;
	}

	size_t off = cnt * unit;
	size_t last = size - 1;
	size_t ret = size + off;

	/*
	 * Assume the length of bad control character replacement is 3. The '?'
	 * is the bad control character and ... is the replacement.
	 *
	 * str  "?fun?ction?       "
	 *
	 * 1-1  "?fun?ction?       "  no move applied
	 * 1-2  "?fun?ction?   ... "  use negative offset to set replacement
	 *
	 * 2-1  "?fun?ctioction... "
	 * 2-2  "?fun?c...ction... "
	 *
	 * 3-1  "?fufun...ction... "
	 * 3-2  "...fun...ction... "
	 */
	idx_for_each_reverse(i, size - 1) {
		if (!char_is_bad_cntrl(buf[i]))
			continue;

		size_t good = i + 1;
		size_t new = good + off;
		if (likely(i != size - 1))
			memmove(&buf[new], &buf[good], last - i);

		memcpy(&buf[new - len], alt, len);
		off -= unit;
		last = i - 1; /* even if i == 0 it's fine */
	}

	return ret;
}

int __termas(const char *file, int line,
	     const char *func, enum tm_level level,
	     const char *hint, u32 flags, const char *fmt, ...)
{
	static struct tm_tag tags[] = {
		[TM_LOG]   = { NULL, NULL },
		[TM_WARN]  = { N_("warn:"),  HN_("warn:",  BOLD, MAGENTA) },
		[TM_ERROR] = { N_("error:"), HN_("error:", BOLD, RED) },
		[TM_FATAL] = { N_("fatal:"), HN_("fatal:", BOLD, RED) },
		[TM_BUG]   = { N_("BUG:"),   HN_("BUG:",   BOLD, RED,
							   BG_BLACK) },
	};
	struct tm_tag *tag = &tags[level];
	va_list ap;

	/*
	 * Pay attention that we use write(2) to put our message to file. So
	 * there's no need (and should not) to care about the null terminator
	 * as it's not required.
	 */
	char buf[PUTF_BUF_CAP];
	size_t size = 0;
	size_t cap = sizeof(buf) - 1;
	size_t avail = cap;
	ssize_t nr;

	if (cc_termas_with_ts || !tag->name) {
		struct timespec ts;
		ts_mono(&ts);

		u64 s = ts.tv_sec;
		u64 us = ts.tv_nsec / 1000;
		const char *mas = !cc_use_tercol ?
				  "[%" PRIu64 ".%" PRIu64 "] " :
				  H("[%" PRIu64 ".%" PRIu64 "] ", GREEN);

		nr = snprintf(&buf[size], avail + 1, mas, s, us);
		if (!__test_add_buf_size(nr, &size, &avail))
			goto out;
	}

	if (cc_termas_with_pid) {
		long pid = getpid();
		const char *mas = !cc_use_tercol ? ">%ld " :
						   H(">", BOLD) "%ld ";

		nr = snprintf(&buf[size], avail + 1, mas, pid);
		if (!__test_add_buf_size(nr, &size, &avail))
			goto out;
	}

	if (tag->name) {
		int show_pos = flags & TM_FLLN || (flags & TM_FUNC);
		const char *t = cc_use_tercol ? tag->colored : tag->name;
		size_t len = strlen(t) + !show_pos;

		if (len > avail)
			len = avail;

		memcpy(&buf[size], _(t), len);
		size += len;
		if (!show_pos)
			buf[size - 1] = ' ';

		if (size >= avail)
			goto out;
		avail -= size;
	}

	if (flags & TM_FLLN) {
		const char *mas = !cc_use_tercol ? "%s:%d:%s" :
						   H("%s:%d:%s", BOLD);

		nr = snprintf(&buf[size], avail + 1, mas,
			      file, line, flags & TM_FUNC ? "" : " ");
		if (!__test_add_buf_size(nr, &size, &avail))
			goto out;
	}

	if (flags & TM_FUNC) {
		const char *mas = !cc_use_tercol ? "%s " : H("%s ", BOLD);

		nr = snprintf(&buf[size], avail + 1, mas, func);
		if (!__test_add_buf_size(nr, &size, &avail))
			goto out;
	}

	va_start(ap, fmt);
	nr = vsnprintf(&buf[size], avail + 1, fmt, ap);
	va_end(ap);
	if (!__test_add_buf_size(nr, &size, &avail))
		goto out;

	if (hint) {
		if (avail <= 2)
			goto out;

		buf[size++] = ';';
		buf[size++] = ' ';
		avail -= 2;

		size_t len = strlen(hint);
		if (len > avail)
			len = avail;

		memcpy(&buf[size], hint, len);
		size += len;
	}

out:
	size = rm_bad_cntrl(buf, size, cap);

	int is_err = !(flags & TM_WROUT);
	int fd = STDERR_FILENO;
	int ret = -1;
	FILE *stream = stderr;

	if (!is_err) {
		fd = STDOUT_FILENO;
		stream = stdout;
		ret = 0;
	}

	buf[size] = '\n';
	size += 1;

	fflush(stream);
	xwrite(fd, buf, size);

	switch (level) {
	case TM_FATAL:
		exit(128);
	case TM_BUG:
		abort();
	default:
		return ret;
	}
}

void __die_overflow(const char *file, int line, const char *func,
		    uintmax_t a, uintmax_t b, char op, uint size)
{
	__termas(file, line, func, TM_FATAL, NULL, TM_FLLN | TM_FUNC,
		 "%" PRIuMAX " %c %" PRIuMAX " overflows in %u-byte",
		 a, op, b, size);
	unreachable();
}
