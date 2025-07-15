#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

set -e

if [ ! -f .licensefix ]; then
	exit
fi

base=$(git merge-base HEAD MERGE_HEAD)
spdx_theirs=$(grep old .licensefix | cut -f2)
spdx_ours=$(grep new .licensefix | cut -f2)

trap 'rm theirs ours' EXIT

git status --short | grep UD |  cut -d' ' -f2 | while read file; do
	git show $base:$file | sed "/$spdx_theirs/d" >theirs
	git show HEAD:$file | sed "/$spdx_ours/d" >ours

	if [ -n "$(diff theirs ours)" ]; then
		continue
	fi

	git rm $file
done
