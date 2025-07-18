.. SPDX-License-Identifier: MPL-2.0

=================
Configuring Build
=================

Before building the final executable, you need to generate build scripts. Run
``menuconfig`` and ``configure`` to do this. ``menuconfig`` outputs configs for
``configure``. ``configure`` generates build scripts.

Menuconfig
==========

This target opens menuconfig TUI. After saving changes, it dumps config options
to .config.unix or .config.win32. This file defines constants control build and
program behavior. It's read by the ``configure`` target. Linux documents usage
and specification of the Kconfig language. Check it in `Kconfig Language`_.

You can change build tools by adding LLVM=1 or MAKEFILE=Ninja when running this
target, but you can't change them after menuconfig. gmake script dumps these
tool names to build/tools.

Configure
=========

This target generates build scripts for generator. Available generators are GNU
Make and Ninja. The output directory is build.unix/ or build.win32/, depends on
which OS you're on. It also has a build/, it's symblink to output directory.
Get it by running ``scripts/setup-repo.sh`` or ``scripts/setup-repo.ps1``.

Running ``configure`` without running ``menuconfig`` is fine. At that point, it
generates build scripts based on default config options. These defaults work
well for installation, but a pain if you're hacking into brukit. They lack
sanitizers, debug symbols, and have a messy optimization level.

Never touch files under output directory. Build scripts and gmake script
sometimes read their last modification timestamps. If you fuck them up, build
system goes strange and may produce garbage.

Build script pipes CMAKEFLAGS passed to it into cmake at configure phase. It
records CMAKEFLAGS to build/flags. To change it, re-run this target.

.. _`Kconfig Language`: https://docs.kernel.org/kbuild/kconfig-language.html
