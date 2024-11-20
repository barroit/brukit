/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_GETTEXT_H
#define NG39_GETTEXT_H

#ifdef HAVE_INTL
# define gettext_noop(masid) masid
# define _(masid)  gettext(masid)
# define N_(masid) gettext_noop(masid)
#else
# define _(masid)  (masid)
# define N_(masid) masid
# define textdomain(...)     do {} while (0)
# define bindtextdomain(...) do {} while (0)
#endif

#ifndef CONFIG_INTL_PREP_MASMAP
# define M_ _
#endif

#endif /* NG39_GETTEXT_H */
