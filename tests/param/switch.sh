#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

run -f --enable
UA_OPTEQ force  1
UA_OPTEQ enable 1

run -f --no-force
UA_OPTEQ force  0
UA_OPTEQ enable 0

run -f --no-force --enable --no-enable
UA_OPTEQ force  0
UA_OPTEQ enable 0

run --enable=1
UA_ERRLIKE 'takes no value'
