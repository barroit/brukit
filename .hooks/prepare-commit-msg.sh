#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

sed -i '' '/^. Please enter the commit message/,/^#$/d' $1
