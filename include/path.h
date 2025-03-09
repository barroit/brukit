/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 *
 * All path retrieval functions may or may not be MT-safe on the first run.
 * However, subsequent calls to these functions are guaranteed to be MT-safe.
 */

#ifndef NG39_PATH_H
#define NG39_PATH_H

#include "attr.h"
#include "types.h"

#define PTH_SEP_UNI '/'
#define PTH_SEP_WIN '\\'

#if defined(__unix__)
# define PTH_SEP PTH_SEP_UNI
#elif defined(_WIN32)
# define PTH_SEP PTH_SEP_WIN
#endif

int chr_is_sep(xchar c);

int pth_is_abs(const xchar *name) __pure;

xchar *pth_next_sep(const xchar *s) __pure;

xchar *pth_last_sep(const xchar *s) __pure;

static inline int pth_is_dot(const xchar *name)
{
	return name[0] == '.' &&
	       (name[1] == 0 || (name[1] == '.' && name[2] == 0));
}

char *delink(const char *name);

const xchar *pth_home(void);

const xchar *pth_executable(void);

const xchar *pth_prefix(void);

const xchar *pth_locale(void);

const xchar *pth_cwd(void);

#endif /* NG39_PATH_H */
