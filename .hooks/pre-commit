#!/usr/bin/bash
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

RED='\033[0;31m'
RESET='\033[0m'

inv=$(git diff-index --cached --name-only \
      --diff-filter=A -z HEAD | LC_ALL=C tr '[ -~]\0' '\0\n')

if [[ -n $inv ]]; then
	>&2 printf "non-ascii name: ${RED}%s${RESET}\n" $inv
	exit 1
fi

mod=$(git diff-index --cached --name-only HEAD)

if [[ -z $mod ]]; then
	exit 0
fi

echo $mod | xargs -P$(nproc) -n1 codespell
if [[ $? -ne 0 ]]; then
	exit 1
fi
