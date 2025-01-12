// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

/*
 * See also: https://github.com/google/sanitizers/wiki/AddressSanitizerFlags
 */

#pragma GCC diagnostic ignored "-Wmissing-prototypes"

const char *__asan_default_options(void)
{
	return "abort_on_error=1:"
	       "disable_coredump=0";
}
