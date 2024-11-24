/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_WCMBS_H
#define NG39_WCMBS_H

enum mbstatus {
	MB_INVAL = -1,	/* unrecognized character */
	MB_ASCII = 0,	/* character is ascii */
	MB_DATA,	/* in the middle of a character */
	MB_LBT2,	/* character is represented by 2 bytes */
	MB_LBT3,
	MB_LBT4,
};

enum mbstatus mbstatus(char c);

size_t wcs_to_mbs(const wchar_t *__src, char **__dest);

#ifdef ANSI
#define xstrlen strlen
#define xstrrchr strrchr
#define xvsnprintf vsnprintf
#define xisspace isspace
#else
#define xstrlen wcslen
#define xstrrchr wcsrchr
#define xvsnprintf vswprintf
#define xisspace iswspace
#endif

#endif /* NG39_WCMBS_H */
