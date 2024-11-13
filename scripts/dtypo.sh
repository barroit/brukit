# SPDX-License-Identifier: GPL-3.0-or-later or MIT

git ls-files | xargs -P$(nproc) -n1 codespell
