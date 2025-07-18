.. SPDX-License-Identifier: MPL-2.0

=============
Build Program
=============

Build system offers two targets to build final executable. One is ``build``,
you must have build scripts to run this. Another is ``all``, which is an alias
for ``configure`` and ``build``.

The executable is written to build/<name>. This name is lowercased value read
from NAME.
