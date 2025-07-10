/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 *
 * These printf functions are for API logging. For user logging, use termas
 * instead.
 */

#ifndef NG39_PRINTF_H
#define NG39_PRINTF_H

#include <stdio.h>

#include "stringify.h"

/*
 * See include/linux/printk.h.
 */
#ifndef pr_fmt
# define pr_fmt(fmt) fmt
#endif

#define pr_out(fmt, ...) __pr(stdout, fmt, ##__VA_ARGS__)
#define pr_err(fmt, ...) __pr(stdout, fmt, ##__VA_ARGS__)

#define __pr(f, fmt, ...) fprintf(f, pr_fmt(fmt), ##__VA_ARGS__)

#endif /* NG39_PRINTF_H */
