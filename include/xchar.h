/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_XCHAR_H
#define NG39_XCHAR_H

#include <stdio.h>

#ifdef ANSI
# include <ctype.h>
# include <string.h>
#else
# include <wctype.h>
# include <wchar.h>
#endif

#define XC(x) __XC(x)

#ifdef ANSI
# define __XC(x) x
#else
# define __XC(x) L##x
#endif

#define __strlen_a    strlen
#define __strrchr_a   strrchr
#define __vsnprintf_a vsnprintf
#define __isspace_a   isspace
#define __strcmp_a    strcmp

#define __strlen_u    wcslen
#define __strrchr_u   wcsrchr
#define __vsnprintf_u vswprintf
#define __isspace_u   iswspace
#define __strcmp_u    wcscmp

#ifdef ANSI
# define __xchar_type _a
#else
# define __xchar_type _u
#endif

#define __XCHAR_ALIAS(x)      ___XCHAR_ALIAS(x, __xchar_type)
#define ___XCHAR_ALIAS(...)   ____XCHAR_ALIAS(__VA_ARGS__)
#define ____XCHAR_ALIAS(x, t) __##x##t

#define xc_strlen    __XCHAR_ALIAS(strlen)
#define xc_strrchr   __XCHAR_ALIAS(strrchr)
#define xc_vsnprintf __XCHAR_ALIAS(vsnprintf)
#define xc_isspace   __XCHAR_ALIAS(isspace)
#define xc_strcmp    __XCHAR_ALIAS(strcmp)

#endif /* NG39_XCHAR_H */
