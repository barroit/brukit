/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_HPRELUDE_H
#define NG39_HPRELUDE_H

#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#if defined(__unix__)
# include <unistd.h>
# include <pwd.h>
# include <libgen.h>
# include <dirent.h>
# include <sys/resource.h>
# include <sys/utsname.h>
# include <sys/syscall.h>
# include <sys/mman.h>
# include <sys/wait.h>
# include <sys/syscall.h>
#elif defined(_WIN32)
# include <sdkddkver.h>
# include <io.h>
# include <conio.h>
# include <malloc.h>
# include <winsock2.h>
# include <windows.h>
# include <windowsx.h>
# include <versionhelpers.h>
# include <commctrl.h>
# include <direct.h>
# include <basetsd.h>
# include <shlwapi.h>
# include <process.h>
# include <userenv.h>
# ifdef CONFIG_REP_HOOK
#  include <dbghelp.h>
#  include <crtdbg.h>
# endif
#endif

#ifdef HAVE_INTL
# ifdef CONFIG_DEBUG_INTL
#  define _INTL_REDIRECT_INLINE
# endif
# include <libintl.h>
# include <locale.h>
#endif

#endif /* NG39_HPRELUDE_H */
