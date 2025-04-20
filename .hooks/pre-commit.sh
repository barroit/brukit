#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

invalid=$(git diff-index --cached --name-only \
	  --diff-filter=A -z HEAD | LC_ALL=C tr '[ -~]\0' '\0\n')

if [ -n "$invalid" ]; then
	lines=$(printf '\n  %s' $invalid)

	printf "error: non-ascii file name%s\n" "$lines" >&2
	exit 1
fi

files=$(git diff-index --cached --name-only HEAD)

if [ -z "$files" ]; then
	exit 0
fi

printf '%s\n' "$files" | xargs -P$(nproc) -n1 codespell

if [ $? -ne 0 ]; then
	exit 1
fi
