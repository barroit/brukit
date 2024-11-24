/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 *
 * All path retrieval functions may or may not be MT-safe on the first run.
 * However, subsequent calls to these functions are guaranteed to be MT-safe.
 */

#ifndef NG39_PATH_H
#define NG39_PATH_H

#define PTH_SEP_UNIX  C('/')
#define PTH_SEP_WIN32 C('\\')

#if defined(__unix__)
# define PTH_SEP PTH_SEP_UNIX
#elif defined(_WIN32)
# define PTH_SEP PTH_SEP_WIN32
#endif

static inline int pth_is_abs(const xchar *name)
{
#if defined(__unix__)
	return name[0] == PTH_SEP;
#elif defined(_WIN32)
	return !PathIsRelative(name);
#endif
}

static inline xchar *pth_last_sep(const xchar *s)
{
#if defined(__unix__)
	return strrchr(s, PTH_SEP);
#elif defined(_WIN32)
	return xstrrchr(s, PTH_SEP_WIN32) ? : xstrrchr(s, PTH_SEP_UNIX);
#endif
}

static inline int pth_is_dot(const xchar *name)
{
	return name[0] == C('.') &&
	       (name[1] == 0 || (name[1] == C('.') && name[2] == 0));
}

#ifdef __unix__
char *delink(const char *name);
#endif

const xchar *pth_home(void);

const xchar *pth_executable(void);

const xchar *pth_prefix(void);

const xchar *pth_locale(void);

#endif /* NG39_PATH_H */
