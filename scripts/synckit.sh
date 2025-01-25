#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

set -e

git switch brukit
git pull

git rebase --onto brukit $(git merge-base master brukit) master
