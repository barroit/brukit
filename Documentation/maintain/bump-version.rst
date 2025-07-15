.. SPDX-License-Identifier: MPL-2.0

=====================
Bump Consumer Version
=====================

VERSION records current consumer version. There's a VERSION.1, don't touch it.
It records brukit version. Modifying it will definitely cause merge conflict if
the next brukit update includes new version tags.

Use ``scripts/bump-version.sh`` to commit new content in VERSION and create
tags. You must have a valid GnuPG key. See `bump-version.sh usage`_ for
detailed explanation.

.. _`bump-version.sh usage`: https://docs.barroit.sh/
			     barroit/dev-tools/bump-version.html
