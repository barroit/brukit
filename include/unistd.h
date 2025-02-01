/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_UNISTD_H
#define NG39_UNISTD_H

#ifdef __unix__

#include_next <unistd.h>

#else /* __unix__ */

#include <direct.h>
#include <io.h>
#include <process.h>

#define STDIN_FILENO  ({ fileno(stdin); })
#define STDOUT_FILENO ({ fileno(stdout); })
#define STDERR_FILENO ({ fileno(stderr); })

#define R_OK 04
#define W_OK 02
#define X_OK 01
#define F_OK 00

#endif /* __unix__ */

#endif /* NG39_UNISTD_H */
