// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#ifdef CONFIG_ENABLE_SANITIZER
# include <sanitizer/asan_interface.h>
#else
# include "compiler.h"
# define __sanitizer_set_death_callback NOOP
#endif

#include <stdlib.h>

#include "init.h"

INITCALL(death_cb)
{
	__sanitizer_set_death_callback(__exit_show_confirm);
}
