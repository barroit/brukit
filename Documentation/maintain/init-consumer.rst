.. SPDX-License-Identifier: MPL-2.0

==============
Consumer Setup
==============

``scripts/init-consumer.sh`` turns provider to consumer in irreversible way. It
drops junk, generates files from .consumer.in, and commits changes with message
``INITIAL CONSUMER``.

The .consumer.in is an ini-like file. It stores sections. Each section begins
with ``[<field>]`` and ends with ``[end<field>]``. Indentations are preserved.
Don't use whitespaces unless the field is ``license`` or ``readme``. This
document uses ``$<field>`` to refer to fields.

::

	# Example of .consumer.in

	[repo]
	git@github.com:barroit/brukit.git
	[endrepo]

	[license]
	GPL-3.0-or-later
	[endlicense]

	[name]
	brukit
	[endname]

	[conf]
	no_arch
	[endconf]

	[icon]
	cherry.ico
	[endicon]

	[readme]
	Negi
	====

	Miku is justice!
	[endreadme]

This script sets ``origin`` remote URL to ``$repo``. Replaces README contents
with ``$readme``. It records ``$name`` in NAME, but uses lowercase when
generating files with ``$name`` prefix.

``$license`` is new license to use in consumer. It only affects source code
works, not documents, GitHub workflows, or artworks. Check brukit README for
available licenses. As of writing this document, brukit source code works are
dual-licensed under GPL-3.0-or-later and MIT. You can choose whichever one you
want, but not others.

This script generates .pickignore and .licensefix, which are read by
`merge tool`_. ``.pickignore`` tells which files to ignore when merging
upstream to current branch, to avoid upstream changes on dropped files causing
merge conflicts. ``.licensefix`` enables automatic replacement of old licenses
with new ones.

``$conf`` is some tuning knobs for consumer. Don't use it in new consumer. It's
an overengineering crap, non-portable, broken, and planned to be dropped in a
future version; ``icon`` specifies executable icon file. It's only available on
Windows, noop on Unix-like OS.

You are free to amend the initial commit of consumer. For example, you never
port to Windows and drop all Windows stuff ;)

.. _`merge tool`: https://github.com/
		  barroit/barroit/blob/master/scripts/merge-upstream.sh
