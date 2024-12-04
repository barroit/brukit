/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_STRTOX_H
#define NG39_STRTOX_H

#include "types.h"

int stx_strtoull(const xchar *s, uint base, unsigned long long *res);
int stx_strtoul(const xchar *s, uint base, ulong *res);
int stx_strtouint(const xchar *s, uint base, uint *res);

int stx_strtoll(const xchar *s, uint base, long long *res);
int stx_strtol(const xchar *s, uint base, long *res);
int stx_strtoint(const xchar *s, uint base, int *res);

int stx_strtou64(const xchar *s, uint base, u64 *res);
int stx_strtou32(const xchar *s, uint base, u32 *res);
int stx_strtou16(const xchar *s, uint base, u16 *res);
int stx_strtou8(const xchar *s, uint base, u8 *res);

int stx_strtos64(const xchar *s, uint base, s64 *res);
int stx_strtos32(const xchar *s, uint base, s32 *res);
int stx_strtos16(const xchar *s, uint base, s16 *res);
int stx_strtos8(const xchar *s, uint base, s8 *res);

#endif /* NG39_STRTOX_H */
