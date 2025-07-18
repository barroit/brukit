.. SPDX-License-Identifier: MPL-2.0

============
Build System
============

Brukit has its own highly customized build system. It doesn't use any external
toolchains. Everything here is built from scratch, with many practical tests.
It's robust, feature-rich without overengineering crap, and easy to use. It
also has excellent auto-reconfig handling.

It manages build system with a top-level Makefile. That gmake script defines
runtime environments, command targets, build workflows, source generation,
and auto-reconfig handling. Without it, you can't run build system or some
scripts. On Windows port, the equivalent script is make.ps1.

To get started, read this:

.. toctree::
   :maxdepth: 1

   configure
