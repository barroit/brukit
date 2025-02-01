/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_XCHAR_H
#define NG39_XCHAR_H

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#include "xalloc.h"

#define XC(x) __XC(x)

#ifdef ANSI
# define __XC(x) x
#else
# define __XC(x) L##x
#endif

#define __mbslen_a    __mbslen
#define __strchrnul_a __strchrnul
#define __strcmp_a    strcmp
#define __strdup_a    xstrdup
#define __strlen_a    strlen
#define __stpncpy_a   strncpy
#define __strncmp_a   strncmp
#define __strrchr_a   strrchr
#define __vsnprintf_a vsnprintf
#define __ispunct_a   ispunct
#define __isspace_a   isspace
#define __isxdigit_a  isxdigit
#define __tolower_a   tolower
#define __mbsws_a     mbsws

#define __mbslen_u    wcslen
#define __strchrnul_u __strchrnul
#define __strcmp_u    wcscmp
#define __strdup_u    xwcsdup
#define __strlen_u    wcslen
#define __stpncpy_u   wcsncpy
#define __strncmp_u   wcsncmp
#define __strrchr_u   wcsrchr
#define __vsnprintf_u vswprintf
#define __ispunct_u   iswpunct
#define __isspace_u   iswspace
#define __isxdigit_u  iswxdigit
#define __tolower_u   towlower
#define __mbsws_u     wcsws

#ifdef ANSI
# define __xchar_type _a
#else
# define __xchar_type _u
#endif

#define __XCHAR_ALIAS(x)      ___XCHAR_ALIAS(x, __xchar_type)
#define ___XCHAR_ALIAS(...)   ____XCHAR_ALIAS(__VA_ARGS__)
#define ____XCHAR_ALIAS(x, t) x ## t

/*
 * Returns the exact length of a UTF-8 string in characters, independent of
 * the locale.
 */
#define xc_mbslen    __XCHAR_ALIAS(__mbslen)

#define xc_strchrnul __XCHAR_ALIAS(__strchrnul)
#define xc_strcmp    __XCHAR_ALIAS(__strcmp)
#define xc_strdup    __XCHAR_ALIAS(__strdup)
#define xc_strlen    __XCHAR_ALIAS(__strlen)
#define xc_strncpy   __XCHAR_ALIAS(__strncpy)
#define xc_strncmp   __XCHAR_ALIAS(__strncmp)
#define xc_strrchr   __XCHAR_ALIAS(__strrchr)
#define xc_vsnprintf __XCHAR_ALIAS(__vsnprintf)
#define xc_ispunct   __XCHAR_ALIAS(__ispunct)
#define xc_isspace   __XCHAR_ALIAS(__isspace)
#define xc_isxdigit  __XCHAR_ALIAS(__isxdigit)
#define xc_tolower   __XCHAR_ALIAS(__tolower)
#define xc_mbsws     __XCHAR_ALIAS(__mbsws)

#endif /* NG39_XCHAR_H */
