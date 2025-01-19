/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_PROC_H
#define NG39_PROC_H

#include "types.h"

#define __rd_io_mask     (-1U << 30)
#define __rd_io_flags(f) ((f) & __rd_io_mask)

#if defined(__unix__)
typedef pid_t proc_t;
#elif defined(_WIN32)
# include <windows.h>
typedef PROCESS_INFORMATION proc_t;
#endif

#define PROC_RD_STDOUT (1U << 30)
#define PROC_RD_STDERR (1U << 31)

int proc_rd_io(const xchar *name, u32 flags);

int proc_exec(u32 flags, proc_t *proc, const xchar *file, ...);

int proc_wait(proc_t *proc, int *ret);

#endif /* NG39_PROC_H */
