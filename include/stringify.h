/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_STRINGIFY_H
#define NG39_STRINGIFY_H

#define __stringify(s)  ___stringify(s)
#define ___stringify(s) #s

#define FILE_LINE __FILE__ ":" __stringify(__LINE__)

#endif /* NG39_STRINGIFY_H */
