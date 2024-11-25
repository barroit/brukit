/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_GETTEXT_H
#define NG39_GETTEXT_H

#ifdef HAVE_INTL
# ifdef CONFIG_DEBUG_INTL
#  define _INTL_REDIRECT_INLINE
# endif
# include <libintl.h>
# define _(masid)  gettext(masid)
#else
# define _(masid)  masid
#endif

#define N_(masid) masid

#endif /* NG39_GETTEXT_H */
