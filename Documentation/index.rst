.. SPDX-License-Identifier: MPL-2.0

========================
The Brukit Documentation
========================

This is the top level of brukit documentation. Stuff here is for players, not
end users.

Brukit comes with some not so dumb maintenance tooling. So follow the rules, or
you'll scrawl it up. It depends on `core library`_ to maintain and build. These
two repositories must sit side by side. Otherwise, symlinks won't work right.

You must be familiar with git. Without it, you don't ever understand how to
maintain histories; you're also expected to read the code. This document never
explains how functions flow. It only tells what the functions do; this is not
a tutorial. If something breaks and you do not know why, you probably skipped
the code.

.. toctree::
   :maxdepth: 1

   Licenses <maintain/licenses>

   Maintenance workflow <maintain/index>

.. _`core library`: ../barroit
