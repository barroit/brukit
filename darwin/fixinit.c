// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "init.h"

#include <mach-o/getsect.h>

#include "attr.h"
#include "iter.h"
#include "xalloc.h"

extern const struct mach_header_64 _mh_execute_header;

static int sort_data(const void *__a, const void *__b)
{
	const struct init_data *a = __a;
	const struct init_data *b = __b;

	return a->lv - b->lv;
}

static void __cold __attr((__constructor__)) run_init(void)
{
	ulong __size;
	u8 *start = getsectiondata(&_mh_execute_header,
				   "__DATA", "__miku_init", &__size);
	const struct section_64 *sec = getsectbyname("__DATA", "__miku_init");

	struct init_data *copy = xmalloc(sec->size);
	size_t size = sizeof(*copy);
	size_t nr = sec->size / size;

	memcpy(copy, start, sec->size);
	qsort(copy, nr, size, sort_data);

	uint i;

	idx_for_each(i, nr) {
		struct init_data *data = &copy[i];

		data->cb();
	}
}
