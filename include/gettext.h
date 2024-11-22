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
# define textdomain     NOOP
# define bindtextdomain	NOOP
#endif

#ifndef __INTL_PREP_MASMAP_NOT_IMPLEMENTED
# define M_  _
# define MN_ N_
#endif

#endif /* NG39_GETTEXT_H */
