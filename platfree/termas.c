// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "termas.h"
#include "tercol.h"
#include "scio.h"
#include "iter.h"
#include "time.h"
#include "udef.h"

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

static void fdputs(int fd, const char *tag,
		   const char *hint, const char *fmt, va_list ap)
{
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

	if (cc_termas_with_ts || !tag) {
		struct timespec ts;
		monotime(&ts);

		u64 s = ts.tv_sec;
		u64 us = ts.tv_nsec / 1000;
		const char *mas = "[%" PRIu64 ".%" PRIu64 "] ";
		if (cc_use_tercol)
			mas = H("[%" PRIu64 ".%" PRIu64 "] ", GREEN);

		nr = snprintf(&buf[size], avail + 1, mas, s, us);
		BUG_ON(nr < 0);

		if (!__test_add_buf_size(nr, &size, &avail))
			goto out;
	}

	if (cc_termas_with_pid) {
		long pid = getpid();
		char *mas = ">%ld ";
		if (cc_use_tercol)
			mas = H(">", BOLD) "%ld ";

		nr = snprintf(&buf[size], avail + 1, mas, pid);
		BUG_ON(nr < 0);

		if (!__test_add_buf_size(nr, &size, &avail))
			goto out;
	}

	if (tag) {
		size_t len = strlen(tag);
		if (len > avail)
			len = avail;

		memcpy(&buf[size], tag, len);
		size += len;

		if (size >= avail)
			goto out;
		avail -= size;
	}

	nr = vsnprintf(&buf[size], avail + 1, fmt, ap);
	BUG_ON(nr < 0);

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

	if (fd == STDERR_FILENO)
		fflush(stderr);
	else if (fd == STDOUT_FILENO)
		fflush(stdout);

	buf[size] = '\n';
	size += 1;

	nr = xwrite(fd, buf, size);
	BUG_ON(nr < 0);
}

static inline void puterr(const char *tag,
			  const char *hint, const char *fmt, va_list ap)
{
	return fdputs(STDERR_FILENO, tag, hint, fmt, ap);
}

void __log(const char *hint, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);

	fdputs(STDOUT_FILENO, NULL, hint, fmt, ap);
	va_end(ap);
}

void __note(const char *hint, const char *fmt, ...)
{
	va_list ap;
	const char *tag = H_("note: ", BOLD, CYAN);

	va_start(ap, fmt);
	if (!cc_use_tercol)
		tag = "note: ";

	puterr(tag, hint, fmt, ap);
	va_end(ap);
}

int __warn(const char *hint, const char *fmt, ...)
{
	va_list ap;
	const char *tag = H_("warning: ", BOLD, MAGENTA);

	va_start(ap, fmt);
	if (!cc_use_tercol)
		tag = "warning: ";

	puterr(tag, hint, fmt, ap);
	va_end(ap);
	return -1;
}

int __cold __error(const char *hint, const char *fmt, ...)
{
	va_list ap;
	const char *tag = H_("error: ", BOLD, RED);

	va_start(ap, fmt);
	if (!cc_use_tercol)
		tag = "error: ";

	puterr(tag, hint, fmt, ap);
	va_end(ap);
	return -1;
}

void __cold __die(const char *hint, const char *fmt, ...)
{
	va_list ap;
	const char *tag = H_("fatal: ", BOLD, RED);

	va_start(ap, fmt);
	if (!cc_use_tercol)
		tag = "fatal: ";

	puterr(tag, hint, fmt, ap);
	exit(128);
}

void __cold __bug(const char *file, int line, const char *fmt, ...)
{
	char tag[SZ_1K];
	va_list ap;

	va_start(ap, fmt);
	snprintf(tag, sizeof(tag), "BUG!!! %s:%d: ", file, line);

	puterr(tag, NULL, fmt, ap);
	exit(128);
}
