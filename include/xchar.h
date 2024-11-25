/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_XCHAR_H
#define NG39_XCHAR_H

#define XCHAR(x) __XCHAR(x)

#ifdef ANSI
# define __XCHAR(x) x
#else
# define __XCHAR(x) L##x
#endif

#endif /* NG39_XCHAR_H */
