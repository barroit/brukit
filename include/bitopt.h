/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_BITOPT_H
#define NG39_BITOPT_H

#define bit_is_set(flags, ...) (((flags) & (__VA_ARGS__)) == (__VA_ARGS__))

#endif /* NG39_BITOPT_H */
