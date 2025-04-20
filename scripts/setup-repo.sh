#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

set -e

ln -sf ../../../barroit/hooks/pre-commit.sh \
       .git/hooks/pre-commit

ln -sf ../../../barroit/hooks/prepare-commit-msg.sh \
       .git/hooks/prepare-commit-msg

if ! git remote | grep -q brukit; then
	cmd=add
else
	cmd=set-url
fi
git remote $cmd brukit $(grep ^brukit .remote | cut -f2)

git remote set-url origin $(grep ^this .remote | cut -f2)

git branch --set-upstream-to origin/master

git fetch brukit master

if ! git for-each-ref --format='%(refname)' refs/heads | grep -q brukit; then
	git branch --track brukit brukit/master
else
	git branch --force brukit brukit/master
fi
