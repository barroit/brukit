#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

run push
UA_OPTEQ cmd push

run pop
UA_OPTEQ cmd pop

run test
UA_ERRLIKE 'unknown command'

run
UA_OUTLIKE 'usage: run'
