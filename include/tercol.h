/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_TERCOL_H
#define NG39_TERCOL_H

#include "i18n.h"

/*
 * \033[<attributes>;<foreground>;<background>m
 *
 * Foreground		Background		Attribute
 * 30    Black		40	Black		0	Reset
 * 31    Red		41	Red		1	Bold/bright
 * 32    Green		42	Green		2	Faint
 * 33    Yellow		43	Yellow		3	Italic
 * 34    Blue		44	Blue		4	Underline
 * 35    Magenta	45	Magenta		5	Blink
 * 36    Cyan		46	Cyan		7	Inverse
 * 37    White		47	White		22	Normal intensity
 * 90    Bright Black	100	Bright Black	24	No underline
 * 91    Bright Red	101	Bright Red	25	No blink
 * 92    Bright Green	102	Bright Green
 * 93    Bright Yellow	103	Bright Yellow
 * 94    Bright Blue	104	Bright Blue
 * 95    Bright Magenta	105	Bright Magenta
 * 96    Bright Cyan	106	Bright Cyan
 * 97    Bright White	107	Bright White
 */

#define __tc_beg "\033["
#define __tc_end "m"
#define __tc_sep ";"

#define __fmtcol(...) ADAP_CALL(__fmtcol_, __VA_ARGS__)
#define __fmtcol_1(x)       __tc_beg #x __tc_end
#define __fmtcol_2(x, y)    __tc_beg #x __tc_sep #y __tc_end
#define __fmtcol_3(x, y, z) __tc_beg #x __tc_sep #y __tc_sep #z __tc_end

#define RESET 0
#define BOLD  1

#define BLACK   30
#define RED     31
#define GREEN   32
#define YELLOW  33
#define BLUE    34
#define MAGENTA 35
#define CYAN    36
#define WHITE   37

#define BRIGHT_BLACK   90
#define BRIGHT_RED     91
#define BRIGHT_GREEN   92
#define BRIGHT_YELLOW  93
#define BRIGHT_BLUE    94
#define BRIGHT_MAGENTA 95
#define BRIGHT_CYAN    96
#define BRIGHT_WHITE   97

#define BG_BLACK   40
#define BG_RED     41
#define BG_GREEN   42
#define BG_YELLOW  43
#define BG_BLUE    44
#define BG_MAGENTA 45
#define BG_CYAN    46
#define BG_WHITE   47

#define H(x, ...) __fmtcol(__VA_ARGS__) x __fmtcol(RESET)

#ifndef INTL_PREP_MO
# define __H_  _
# define __HN_ N_
#endif

/*
 * These are xgettext keywords. Do not wrap text with N_() or _()
 */
#define H_(x, ...)  __H_(H(x, __VA_ARGS__))
#define HN_(x, ...) __HN_(H(x, __VA_ARGS__))

#endif /* NG39_TERCOL_H */
