/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_TERMAS_H
#define NG39_TERMAS_H

#include <errno.h>

#include "attr.h"
#include "compiler.h"
#include "types.h"

#undef NULL
#define NULL ((void *)0)

enum tm_level {
	TM_LOG,
	TM_HINT,
	TM_WARN,
	TM_ERROR,
	TM_FATAL,
	TM_BUG,
};

#define MAS_SHOW_FILE (1 << 0)	/* show file name and line number */
#define MAS_SHOW_FUNC (1 << 1)	/* show function name */
#define MAS_TO_STDOUT (1 << 2)	/* output to stdout */
#define MAS_NO_EXIT   (1 << 3)	/* do not exit */
#define MAS_OUT_DEBUG (1 << 4)	/* output is in debug level */

/*
 * For 'hint' parameter in __warn(), __error(), and __die(). This argument
 * provides additional information for the message. The output will look like
 * '<tag>: <message>; <hint>'. It can also be NULL, in which case the message
 * will be '<tag>: <message>'.
 */

#define ___termas(...) __termas(__FILE__, __LINE__, __func__, ##__VA_ARGS__)

int __termas(const char *file, int line,
	     const char *func, enum tm_level level,
	     const char *hint, u32 flags, const char *fmt, ...) __printf(7, 8);

#define __tm_mas(hint, flags, fmt, ...) \
	___termas(TM_LOG, hint, flags | MAS_TO_STDOUT, fmt, ##__VA_ARGS__)

#define __tm_hint(hint, flags, fmt, ...) \
	___termas(TM_HINT, hint, flags, fmt, ##__VA_ARGS__)

#define __tm_warn(hint, flags, fmt, ...) \
	___termas(TM_WARN, hint, flags, fmt, ##__VA_ARGS__)

#define __tm_error(hint, flags, fmt, ...) \
	___termas(TM_ERROR, hint, flags, fmt, ##__VA_ARGS__)

#define ___tm_die(hint, flags, fmt, ...) \
	___termas(TM_FATAL, hint, flags, fmt, ##__VA_ARGS__);

#define __tm_die(hint, flags, fmt, ...)			\
({							\
	___tm_die(hint, flags, fmt, ##__VA_ARGS__);	\
	unreachable();					\
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

#ifdef __unix__
extern char *strerror(int errnum);
#endif

#define __strerrno strerror(errno)

#define mas(fmt, ...)       __tm_mas(NULL, 0, fmt, ##__VA_ARGS__)
#define mas_debug(fmt, ...) __tm_mas(NULL, MAS_OUT_DEBUG, fmt, ##__VA_ARGS__)

#define hint(fmt, ...)       __tm_hint(NULL, 0, fmt, ##__VA_ARGS__)
#define hint_debug(fmt, ...) __tm_hint(NULL, MAS_OUT_DEBUG, fmt, ##__VA_ARGS__)

#define warn(fmt, ...)       __tm_warn(NULL, 0, fmt, ##__VA_ARGS__)
#define warn_errno(fmt, ...) __tm_warn(__strerrno, 0, fmt, ##__VA_ARGS__)
#define warn_debug(fmt, ...) __tm_warn(NULL, MAS_OUT_DEBUG, fmt, ##__VA_ARGS__)

#define error(fmt, ...)       __tm_error(NULL, 0, fmt, ##__VA_ARGS__)
#define error_errno(fmt, ...) __tm_error(__strerrno, 0, fmt, ##__VA_ARGS__)
#define error_debug(fmt, ...) \
	__tm_error(NULL, MAS_OUT_DEBUG, fmt, ##__VA_ARGS__)

#define die(fmt, ...)       __tm_die(NULL, 0, fmt, ##__VA_ARGS__)
#define die_errno(fmt, ...) __tm_die(__strerrno, 0, fmt, ##__VA_ARGS__)

#define bug(fmt, ...) __tm_bug(NULL, 0, fmt, ##__VA_ARGS__)

#ifdef _WIN32

const char *strwinerr(void);

#define warn_winerr(fmt, ...) __tm_warn(strwinerr(), 0, fmt, ##__VA_ARGS__)

#define error_winerr(fmt, ...) __tm_error(strwinerr(), 0, fmt, ##__VA_ARGS__)

#define die_winerr(fmt, ...) __tm_die(strwinerr(), 0, fmt, ##__VA_ARGS__)

#endif /* _WIN32 */

void __termas_output_mode(int mode);

#define TM_O_CHAR  1
#define TM_O_WCHAR 2

#ifdef CONFIG_ENABLE_WCHAR
# define termas_output_mode __termas_output_mode
#else
# define termas_output_mode NOOP
#endif

#endif /* NG39_TERMAS_H */
