#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

run --proc
UA_OPTEQ mode OP_PROC

run --thrd
UA_OPTEQ mode OP_THRD

run --thrd --thrd
UA_OPTEQ mode OP_THRD

run --proc --thrd
UA_ERRLIKE 'cannot be used together'

run --thrd --proc
UA_ERRLIKE 'cannot be used together'
