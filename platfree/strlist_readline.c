// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "strlist.h"

#include <ctype.h>
#include <wchar.h>
#include <wctype.h>

#include "utf8.h"
#include "utf16.h"
#include "xalloc.h"
#include "xcf.h"

static void read_line_once_mb(struct strlist *sl,
			      const char **__str, size_t *__len, size_t wrap)
{
	const char *str = *__str;
	const char *line = str;

	const char *sep_end;
	const char *sep_start = NULL;

	int prev_alpha = 1;
	size_t alphas = 0;

	while (39) {
		int skip;
		wchar_t wc;
		size_t class = utf8_class(*line);

		switch (class) {
		case UTF8_1B:
			if (*line == '\n') {
				line++;
				str = line;
				goto out;
			}

			skip = isspace(*line);
			break;

		default:
			wc = __mbtowc(line);
			skip = iswspace(wc);
		}

		if (!skip)
			break;

		line += class;
	}

	str = line;

	while (*line && alphas <= wrap) {
		int alpha;
		wchar_t wc;
		size_t class = utf8_class(*line);

		switch (class) {
		case UTF8_1B:
			alpha = !isspace(*line);
			break;

		default:
			wc = __mbtowc(line);
			alphas += wc_fullwidth(wc);
			alpha = !iswspace(wc);
		}

		if (alpha) {
			prev_alpha = 1;

		} else if (*line == '\n') {
			line--;
			sep_end = line;
			goto out;

		} else {
			sep_end = line + class;

			if (prev_alpha) {
				sep_start = line;
				prev_alpha = 0;
			}
		}

		line += class;
		alphas++;
	}

out:
	if (!sep_start || !*line) {
		sep_end = line;
		sep_start = line;
	}

	size_t len = sep_start - str;
	size_t sep_len = sep_end - sep_start;
	size_t ws_len = str - *__str;

	__sl_push(sl, (void *)str, len, 1);

	*__len -= ws_len + len + sep_len;
	*__str = sep_end;
}

static void __maybe_unused read_line_once_wc(struct strlist *sl,
					     const wchar_t **__str,
					     size_t *__len, size_t wrap)
{
// 	size_t len = *__len;
// 	const wchar_t *str = *__str;

// 	while (*str) {
// 		int skip = 0;
// 		size_t class = utf16_class(*str);

// 		switch (class) {
// 		case UTF16_HI_BASE:
// 			break;

// 		default:
// 			if (iswspace(*str) || *str == '\n')
// 				skip = 1;
// 		}

// 		if (!skip)
// 			break;

// 		size_t size = class == UTF16_HI_BASE + 1;

// 		str += size;
// 		len -= size;
// 	}

// 	const wchar_t *ptr = str;
// 	const wchar_t *sep = NULL;

// 	size_t chrs = 0;

// 	while (ptr < &str[len] && chrs < wrap) {
// 		size_t class = utf16_class(*ptr);
// 		size_t size = class == UTF16_HI_BASE + 1;

// 		switch (class) {
// 		case UTF16_HI_BASE:
// 			chrs++;
// 			break;

// 		default:
// 			if (iswspace(*ptr) || *ptr == '\n')
// 				sep = ptr;
// 			chrs += wc_fullwidth(wc);
// 		}

// 		ptr += size;
// 		chrs++;
// 	}

// 	if (!*ptr || !sep) {
// 		sep = ptr;
// 		sep_len = 1;
// 	}

// 	size_t n = sep - str;

// 	__sl_push(sl, (void *)str, n, 1);

// 	*__len -= str - *__str + n + sep_len;
// 	*__str = sep + sep_len;
}

void sl_read_line_mb(struct strlist *sl,
		     const char *str, size_t len, size_t wrap)
{
	if (len == -1)
		len = strlen(str);

	while (len)
		read_line_once_mb(sl, &str, &len, wrap);
}

static void __maybe_unused sl_read_line_wc(struct strlist *sl,
					   const wchar_t *str,
					   size_t len, size_t wrap)
{
	if (len == -1)
		len = wcslen(str);

	while (len != -1)
		read_line_once_wc(sl, &str, &len, wrap);
}

#ifdef CONFIG_ENABLE_WCHAR
# define read_line sl_read_line_wc
#else
# define read_line sl_read_line_mb
#endif

void sl_read_line(struct strlist *sl, const xchar *str, size_t wrap)
{
	if (wrap == -1)
		wrap = CONFIG_LINE_WRAP;

	read_line(sl, str, -1, wrap);
}
