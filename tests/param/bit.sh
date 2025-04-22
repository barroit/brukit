#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

run --all
UA_OPTEQ show 0f

run --all --no-lib
UA_OPTEQ show 07

run --all --no-lib --no-cflag
UA_OPTEQ show 03

run --all --no-lib --no-cflag --no-prefix --no-build
UA_OPTEQ show 00

run --all --no-lib --no-cflag --prefix --build
UA_OPTEQ show 03

run --prefix --build
UA_OPTEQ show 03
