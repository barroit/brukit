# SPDX-License-Identifier: GPL-3.0-or-later or MIT

run push
PSSERT_OPTEQUAL cmd push

run pop
PSSERT_OPTEQUAL cmd pop

run test
PSSERT_ERRLIKE unknown command

run
PSSERT_OUTLIKE usage: run
