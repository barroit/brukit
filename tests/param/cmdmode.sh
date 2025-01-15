# SPDX-License-Identifier: GPL-3.0-or-later or MIT

run --proc
PSSERT_OPTEQUAL mode OP_PROC

run --thrd
PSSERT_OPTEQUAL mode OP_THRD

run --thrd --thrd
PSSERT_OPTEQUAL mode OP_THRD

run --proc --thrd
PSSERT_ERRLIKE cannot be used together

run --thrd --proc
PSSERT_ERRLIKE cannot be used together
