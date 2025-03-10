/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

/*
 * This file provides only mapping functions that redirect calls to wchar/char
 * function variants but does not define them.
 */

#ifndef NG39_XCF_H
#define NG39_XCF_H

#define __39_freopen_mb   freopen
#define __39_getenv_mb    getenv
#define __39_ispunct_mb   ispunct
#define __39_isspace_mb   isspace
#define __39_isxdigit_mb  isxdigit
#define __39_mbslen_mb    __mbslen
#define __39_mbsws_mb     mbsws
#define __39_setenv_mb    setenv
#define __39_stat_mb      stat
#define __39_strchr_mb    strchr
#define __39_strchrnul_mb __strchrnul
#define __39_strcmp_mb    strcmp
#define __39_strdup_mb    strdup
#define __39_strlen_mb    strlen
#define __39_stpncpy_mb   strncpy
#define __39_strncmp_mb   strncmp
#define __39_strrchr_mb   strrchr
#define __39_tolower_mb   tolower
#define __39_vsnprintf_mb vsnprintf
#define __39_xstrdup_mb   xstrdup

#define __39_freopen_wc   _wfreopen
#define __39_getenv_wc    wgetenv
#define __39_ispunct_wc   iswpunct
#define __39_isspace_wc   iswspace
#define __39_isxdigit_wc  iswxdigit
#define __39_mbslen_wc    wcslen
#define __39_mbsws_wc     wcsws
#define __39_setenv_wc    wsetenv
#define __39_stat_wc      wstat
#define __39_strchr_wc    wcschr
#define __39_strchrnul_wc __strchrnul
#define __39_strcmp_wc    wcscmp
#define __39_strdup_wc    wcsdup
#define __39_strlen_wc    wcslen
#define __39_stpncpy_wc   wcsncpy
#define __39_strncmp_wc   wcsncmp
#define __39_strrchr_wc   wcsrchr
#define __39_tolower_wc   towlower
#define __39_vsnprintf_wc vswprintf
#define __39_xstrdup_wc   xwcsdup

#ifndef CONFIG_ENABLE_WCHAR
# define __xchar_type _mb
#else
# define __xchar_type _wc
#endif

#define __XCHAR_ALIAS(x)      ___XCHAR_ALIAS(x, __xchar_type)
#define ___XCHAR_ALIAS(...)   ____XCHAR_ALIAS(__VA_ARGS__)
#define ____XCHAR_ALIAS(x, t) x ## t

#define xc_freopen   __XCHAR_ALIAS(__39_freopen)
#define xc_getenv    __XCHAR_ALIAS(__39_getenv)
#define xc_ispunct   __XCHAR_ALIAS(__39_ispunct)
#define xc_isspace   __XCHAR_ALIAS(__39_isspace)
#define xc_isxdigit  __XCHAR_ALIAS(__39_isxdigit)
#define xc_mbslen    __XCHAR_ALIAS(__mbslen)
#define xc_mbsws     __XCHAR_ALIAS(__39_mbsws)
#define xc_setenv    __XCHAR_ALIAS(__39_setenv)
#define xc_stat      __XCHAR_ALIAS(__39_stat)
#define xc_strchr    __XCHAR_ALIAS(__39_strchr)
#define xc_strchrnul __XCHAR_ALIAS(__39_strchrnul)
#define xc_strcmp    __XCHAR_ALIAS(__39_strcmp)
#define xc_strdup    __XCHAR_ALIAS(__39_strdup)
#define xc_strlen    __XCHAR_ALIAS(__39_strlen)
#define xc_strncmp   __XCHAR_ALIAS(__39_strncmp)
#define xc_strncpy   __XCHAR_ALIAS(__39_strncpy)
#define xc_strrchr   __XCHAR_ALIAS(__39_strrchr)
#define xc_tolower   __XCHAR_ALIAS(__39_tolower)
#define xc_vsnprintf __XCHAR_ALIAS(__39_vsnprintf)
#define xc_xstrdup   __XCHAR_ALIAS(__39_xstrdup)

#endif /* NG39_XCF_H */
