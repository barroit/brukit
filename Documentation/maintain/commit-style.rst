.. SPDX-License-Identifier: MPL-2.0

====================
Commit Message Style
====================

Brukit uses kernel commit style. This document is not tutorial about how to
write canonical commit messages. Kernel docs already cover that. Check it in
`submitting patches`_. All scripts in this framework and core library expect
commits written in that form, especially the first line. You must follow it
if you use merge tool or other history-related scripts.

The first line must be concise summary, prefixed with tag(s) followed by colon.
No punctuation at the end. This line is used heavily by internal tools. For
example, `group-history.py` reads the first line of each commit, extracts tags,
and groups commits to generate pretty changelog.

Some good first lines are::

	strlist: don't choke on zero-length input in sl_push_at()
	strlist: add sl_read_line()
	docs: strlist: document failure case of sl_to_argv()

And bad ones are::

	Fixed the strlist API that brokens on 0 length input and implemented
	the read line function
	Document the error case of the sl_to_argv function in strlist API

The bad ones break grouping in `group-history.py`, and you get dumb changelog
in merge commit. Even worse, they are hard to distinguish since they have no
visual cues.

.. _`submitting patches`: https://docs.kernel.org/
			  process/submitting-patches.html

.. _`group-history.py`: https://github.com/barroit/
			barroit/blob/master/scripts/merge.sh
