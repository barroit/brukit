/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_TERMAS_H
#define NG39_TERMAS_H

#include "attr.h"
#include "compiler.h"
#include "types.h"

#define NULL ((void *)0)

enum tm_level {
	TM_LOG,
	TM_WARN,
	TM_ERROR,
	TM_FATAL,
	TM_BUG,
};

#define TM_FLLN  (1 << 0)	/* show file name and line number */
#define TM_FUNC  (1 << 1)	/* show function name */
#define TM_WROUT (1 << 2)	/* output to stdout */

/*
 * For 'hint' parameter in __warn(), __error(), and __die(). This argument
 * provides additional information for the message. The output will look like
 * '<tag>: <message>; <hint>'. It can also be NULL, in which case the message
 * will be '<tag>: <message>'.
 */

#define ___termas(...) __termas(__FILE__, __LINE__, __func__, ##__VA_ARGS__)

int __termas(const char *file, int line,
	     const char *func, enum tm_level level,
	     const char *hint, u32 flags, const char *fmt, ...);

#define __tm_mas(hint, flags, fmt, ...) \
	___termas(TM_LOG, hint, flags | TM_WROUT, fmt, ##__VA_ARGS__)

#define __tm_warn(hint, flags, fmt, ...) \
	___termas(TM_WARN, hint, flags, fmt, ##__VA_ARGS__)

#define __tm_error(hint, flags, fmt, ...) \
	___termas(TM_ERROR, hint, flags, fmt, ##__VA_ARGS__)

#define __tm_die(hint, flags, fmt, ...)				\
({								\
	___termas(TM_FATAL, hint, flags, fmt, ##__VA_ARGS__);	\
	unreachable();						\
})

#define __tm_bug(hint, flags, fmt, ...)				\
({								\
	___termas(TM_BUG, hint, flags, fmt, ##__VA_ARGS__);	\
	unreachable();						\
})

/*
 * For warn(), error(), and die(), the output format is '<tag>: <message>'.
 * For the *_errro() version of these functions, the output format will be
 * '<tag>: <message>; <strerror>'
 */

#define __strerrno strerror(errno)

#define mas(fmt, ...)       __tm_mas(NULL, 0, fmt, ##__VA_ARGS__)

#define note(fmt, ...)       __tm_note(NULL, 0, fmt, ##__VA_ARGS__)
#define note_errno(fmt, ...) __tm_note(__strerrno, 0, fmt, ##__VA_ARGS__)

#define warn(fmt, ...)       __tm_warn(NULL, 0, fmt, ##__VA_ARGS__)
#define warn_errno(fmt, ...) __tm_warn(__strerrno, 0, fmt, ##__VA_ARGS__)

#define error(fmt, ...)       __tm_error(NULL, 0, fmt, ##__VA_ARGS__)
#define error_errno(fmt, ...) __tm_error(__strerrno, 0, fmt, ##__VA_ARGS__)

#define die(fmt, ...)       __tm_die(NULL, 0, fmt, ##__VA_ARGS__)
#define die_errno(fmt, ...) __tm_die(__strerrno, 0, fmt, ##__VA_ARGS__)

#define bug(fmt, ...) __tm_bug(NULL, 0, fmt, ##__VA_ARGS__)

#ifdef _WIN32

const char *strwinerr(void);

#define warn_winerr(fmt, ...) __tm_warn(strwinerr(), 0, fmt, ##__VA_ARGS__)

#define error_winerr(fmt, ...) __tm_error(strwinerr(), 0, fmt, ##__VA_ARGS__)

#define die_winerr(fmt, ...) __tm_die(strwinerr(), 0, fmt, ##__VA_ARGS__)

#endif /* _WIN32 */

#endif /* NG39_TERMAS_H */
