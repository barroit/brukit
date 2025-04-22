#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

run --name barroit
UA_OPTEQ name barroit

run --name=初音ミク
UA_OPTEQ name 初音ミク

run --name 'Hatsune Miku'
UA_OPTEQ name 'Hatsune Miku'

run --name
UA_ERRLIKE 'requires a value'
