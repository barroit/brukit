#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

run --job 3939
UA_OPTEQ job 3939

run --job 0x3939
UA_OPTEQ job 14649

run --job 0101
UA_OPTEQ job 65

run --job
UA_ERRLIKE 'requires a value'

run --job miku
UA_ERRLIKE 'has an invalid integer value'
