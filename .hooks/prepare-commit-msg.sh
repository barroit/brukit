#!/usr/bin/bash
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

sed -i.bak '/^. Please enter the commit message/,/^#$/d' $1
