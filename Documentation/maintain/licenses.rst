.. SPDX-License-Identifier: MPL-2.0

.. _licenses-def:

========
Licenses
========

Brukit has three parts. Each part uses a different license. LICENSES/ stores
license text.

The code works are the main part of this repository. They are dual-licensed
under GPL-3.0-or-later and MIT. When promoted to consumer, you can choose
whichever one you want, or leave it unchanged. But you can't use other
licenses.

GitHub workflow configs are licensed under 0BSD. This is because configs can't
be symlinks, and only copy works here. But configs come from core library, and
its license is incompatible with MIT. So 0BSD wins for permissive.

Documentation works are licensed under MPL-2.0 for maximum flexibility while
still keeping a bit of control.
