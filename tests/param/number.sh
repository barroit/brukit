# SPDX-License-Identifier: GPL-3.0-or-later or MIT

run --job 3939
PSSERT_OPTEQUAL job 3939

run --job 0x3939
PSSERT_OPTEQUAL job 14649

run --job 0101
PSSERT_OPTEQUAL job 65

run --job
PSSERT_ERRLIKE requires a value

run --job miku
PSSERT_ERRLIKE has an invalid integer value
