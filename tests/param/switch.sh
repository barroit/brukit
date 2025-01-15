# SPDX-License-Identifier: GPL-3.0-or-later or MIT

run -f --enable
PSSERT_OPTEQUAL force  1
PSSERT_OPTEQUAL enable 1

run -f --no-force
PSSERT_OPTEQUAL force  0
PSSERT_OPTEQUAL enable 0

run -f --no-force --enable --no-enable
PSSERT_OPTEQUAL force  0
PSSERT_OPTEQUAL enable 0

run --enable=1
PSSERT_ERRLIKE takes no value
