/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_CHARCONV_H
#define NG39_CHARCONV_H

#include <stddef.h>

enum mbstatus {
	MB_INVAL = -1,	/* unrecognized character */
	MB_ASCII = 0,	/* character is ascii */
	MB_DATA,	/* in the middle of a character */
	MB_LBT2,	/* character is represented by 2 bytes */
	MB_LBT3,
	MB_LBT4,
};

enum mbstatus mbstatus(char c);

size_t cc_wcstombs(char **__dest, const wchar_t *__src);

/*
 * If the length returned by this function is maxof(size_t), then the dest
 * buffer must not be passed to free(3).
 */
size_t cc_wcstombs_fb(char **dest, const wchar_t *src, const char *fb);

#endif /* NG39_CHARCONV_H */
