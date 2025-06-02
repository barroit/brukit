/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#pragma GCC diagnostic ignored "-Wformat-security"

#ifndef NG39_I18N_H
#define NG39_I18N_H
#ifndef INTL_PREP_MO

#include "compiler.h"

const char *__gettext(const char *id);

void __gettext_init(void);

#ifdef HAVE_INTL
# ifdef CONFIG_DEBUG_INTL
#  define _INTL_REDIRECT_INLINE
# endif
# include <libintl.h>
# define _(masid) __gettext(masid)
# define gettext_init __gettext_init
#else
# define _(masid) masid
# define gettext_init NOOP
#endif

#define N_(masid) masid

#endif /* INTL_PREP_MO */
#endif /* NG39_I18N_H */
