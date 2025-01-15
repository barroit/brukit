# SPDX-License-Identifier: GPL-3.0-or-later or MIT

run --name barroit
PSSERT_OPTEQUAL name barroit

run --name=初音ミク
PSSERT_OPTEQUAL name 初音ミク

run --name 'Hatsune Miku'
PSSERT_OPTEQUAL name Hatsune Miku

run --name
PSSERT_ERRLIKE requires a value
