// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "pathwalk.h"

#include "path.h"

static const xchar *skip_drive(const xchar *name)
{
	if (!name[0] || !name[1])
		return name;

	xchar du = name[0] & 0xdf;
	xchar sep = name[1];

	if ('A' <= du && du <= 'Z' && sep == ':')
		name += 2;

	return name;
}

static const xchar *skip_drive_back(const xchar *name)
{
	if (!name[0] || !name[-1])
		return name;

	xchar sep = name[0];
	xchar du = name[-1] & 0xdf;

	if ('A' <= du && du <= 'Z' && sep == ':')
		name -= 2;

	return name;
}


static const xchar *skip_network_root(const xchar *name)
{
	const xchar *prev = pw_skip_nsep(name, 2);

	if (prev == name)
		return prev;

	const xchar *next = pw_skip_name(prev);

	if (next == prev)
		return name;
	return next;
}


static const xchar *skip_network_root_back(const xchar *name)
{
	const xchar *prev = pw_skip_name_back(name);

	if (prev == name)
		return prev;

	const xchar *next = pw_skip_nsep_back(prev, 2);

	if (next == prev)
		return name;
	return next;
}

const xchar *pw_skip_root(const xchar *name)
{
	const xchar *p = skip_drive(name);

	if (p != name)
		return p;

	return skip_network_root(name);
}

const xchar *pw_skip_root_back(const xchar *name)
{
	const xchar *p = skip_network_root_back(name);

	if (!*p)
		return p;

	return skip_drive_back(name);
}
