.. SPDX-License-Identifier: MPL-2.0

=============
Update Brukit
=============

Updating this framework is done by merging brukit commits into consumer using
``scripts/merge.sh``. Brukit uses merge script to perduce merges. It's easy
to filter merges made by consumer. So don't do it by hand, otherwise you lose
consistent commit message format.

This script requires a ref, like ``brukit-v0.1`` or ``brukit/master``. But not
``54d92f60d0a0``. If you don't understand these, go learn git. Stable history
of brukit lives on its master branch. Don't merge other branches. They are WIP
and mess up your history. This script generates consistent commit message in
the following format::

	$ git log -1 HEAD
	commit 97c1f63d3ee61048b9c7c6910726be7228cbe3e4
	Merge: 2175472dec83 54d92f60d0a0
	Author: Jiamu Sun <barroit@linux.com>
	Date:   Tue Jul 15 20:22:11 2025 +0900

	    Merge brukit-v0.1 into master

	    This commit merges tags/brukit-v0.1 at 2175472dec83.

	    ======== CHANGELOG ========

	    strlist:
	      - switch length to size_t
	      - rework sl_read_line multi-byte port
	      - rework sl_read_line wide char port
	      - fix broken multi-byte item creation

	    unicode:
	      - rework API
	      - add fake utf-32 port

	    termas:
	      - rework message output
	      - add debug routines
	      - fix prefix format

	    iter:
	      - rename idx_for_each to for_each_idx

	    strbuf:
	      - drop sb_detach()
	      - rename sb_pth_legacy_* to sb_pth_*_dumb
	      - support empty string in sb_put*()
	      - switch length to size_t
	      - allow sb_puts_at to take length

	    printf:
	      - add pr_* functions

	    build:
	      - raise toolchain minimum version

	    i18n:
	      - drop CONFIG_CUSTOM_TEXT_LOCALE
	      - rename gettext API to i18n
	      - rework locale lookup

	    init:
	      - rework API

	    That-remote: NULL
	    This-remote: git@github.com:barroit/brukit.git (origin)
	    Signed-off-by: Jiamu Sun <barroit@linux.com>

``That-remote``
   Is remote ULR of ref passed to merge script. It's NULL if:

	* ref is tag

	* ref refers to remote branch

	* ref is local branch without remote tracking branch

``This-remote``
   Is remote ULR of local branch you're on. It's NULL only if it doesn't have
   remote tracking branch.

This script handles merge conflicts well most times. It ignores files dropped
at initial consumer stage, and files dropped by brukit that consumer modified
only for license correction.
