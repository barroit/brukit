.. SPDX-License-Identifier: MPL-2.0

====================
Maintenance Overview
====================

This document describes how maintenance works. Unlike most frameworks, which
run through package managers or get used like libraries, brukit lives with git.
It's history-driven. All major updates come from master branch, with uniform
merge commits. It has two states:

Provider
   History before running ``scripts/init-consumer.sh``. Brukit lives on this
   side. In this document, provider only refers to brukit. For tight phrasing
   and relaxed tone, it just says brukit.

Consumer
   History after running ``scripts/init-consumer.sh``. Programs using brukit
   live on this side.

This repository promotes itself to consumer with a special commit. Commit
message is ``INITIAL CONSUMER``. After that, all changes are treated as
consumer updates. Hacking on brukit only happens on provider. Don't touch any
part of NG39 headers or sources on consumer side. NG39 part is non-portable.
Modifying it risks merge conflicts when updating brukit in consumer.

Due to scripts and file types brukit uses, maintaining consumer on non
Unix-like OS is basically impossible. You must follow commit message style.
Otherwise, consumer history gets bloated and messy, and fucks merge tool up.

Consumer workflow
=================

.. toctree::
   :maxdepth: 1

   Initialize repository <init-consumer>

   Update framework <update-brukit>
