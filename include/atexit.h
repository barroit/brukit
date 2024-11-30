/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_ATEXIT_H
#define NG39_ATEXIT_H

typedef void (*atexit_callback_t)(void);

void atexit_push(atexit_callback_t func);

atexit_callback_t atexit_pop(void);

#endif /* NG39_ATEXIT_H */
