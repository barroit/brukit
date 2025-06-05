/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#pragma GCC diagnostic ignored "-Wformat-security"

#ifndef NG39_I18N_H
#define NG39_I18N_H
#ifndef INTL_PREP_MO

#include "compiler.h"

#ifdef CONFIG_DEBUG_INTL
# define _INTL_REDIRECT_INLINE
#endif

#ifdef HAVE_INTL
# include <libintl.h>
# define _(masid) __gettext(masid)
#else
# define _(masid) masid
#endif

#define N_(masid) masid

const char *__gettext(const char *id);

#endif /* INTL_PREP_MO */
#endif /* NG39_I18N_H */
