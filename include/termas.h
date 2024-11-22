/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_TERMAS_H
#define NG39_TERMAS_H

/*
 * For 'hint' parameter in __warn(), __error(), and __die(). This argument
 * provides additional information for the message. The output will look like
 * '<tag>: <message>; <hint>'. It can also be NULL, in which case the message
 * will be '<tag>: <message>'.
 */

void __log(const char *hint, const char *fmt, ...) __printf(2, 3);

void __note(const char *hint, const char *fmt, ...) __printf(2, 3);

int __warn(const char *hint, const char *fmt, ...) __printf(2, 3);

int __error(const char *hint, const char *fmt, ...) __printf(2, 3);

void __noreturn __die(const char *hint, const char *fmt, ...) __printf(2, 3);

void __noreturn __bug(const char *file,
		      int line, const char *fmt, ...) __printf(3, 4);

/*
 * For warn(), error(), and die(), the output format is '<tag>: <message>'.
 * For the *_errro() version of these functions, the output format will be
 * '<tag>: <message>; <strerror>'
 */

#define log(fmt, ...)       __log(NULL, fmt, ##__VA_ARGS__)

#define note(fmt, ...)       __note(NULL, fmt, ##__VA_ARGS__)
#define note_errno(fmt, ...) __note(strerror(errno), fmt, ##__VA_ARGS__)

#define warn(fmt, ...)       __warn(NULL, fmt, ##__VA_ARGS__)
#define warn_errno(fmt, ...) __warn(strerror(errno), fmt, ##__VA_ARGS__)

#define error(fmt, ...)       __error(NULL, fmt, ##__VA_ARGS__)
#define error_errno(fmt, ...) __error(strerror(errno), fmt, ##__VA_ARGS__)

#define die(fmt, ...)       __die(NULL, fmt, ##__VA_ARGS__)
#define die_errno(fmt, ...) __die(strerror(errno), fmt, ##__VA_ARGS__)

#define bug(fmt, ...) __bug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

#ifdef _WIN32

const char *strwinerr(void);

#define warn_winerr(...) __warn(strwinerr(), __VA_ARGS__)

#define error_winerr(...) __error(strwinerr(), __VA_ARGS__)

#define die_winerr(...) __die(strwinerr(), __VA_ARGS__)

#endif /* _WIN32 */

#endif /* NG39_TERMAS_H */
