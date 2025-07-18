.. SPDX-License-Identifier: MPL-2.0

========================
The Brukit Documentation
========================

This is the top level of brukit documentation.

User guide
==========

Brukit hacking
==============

This framework comes with some not so dumb maintenance tooling. So follow the
rules, or you'll scrawl it up. It depends on `core library`_ to maintain. These
two repositories must sit side by side. Otherwise, symlinks won't work right.

You must be familiar with git. Without it, you don't ever understand how to
maintain histories; you're also expected to read the code. This document never
explains how functions flow. It only tells what the functions do.

This is not tutorial. If something breaks and you don't know why, you probably
didn't follow the rules or skipped the code. The following documents explain
how to use and hack into this framework. Read them before you start.

.. toctree::
   :maxdepth: 2

   maintain/index

   buildsys/index

.. _`core library`: ../barroit
