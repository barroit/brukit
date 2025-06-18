.. SPDX-License-Identifier: GPL-3.0-or-later or MIT

=======================
Configure Build Options
=======================

This document describes Kconfig in mbuild, also ``menuconfig`` bootstrap
command.

Mbuild uses menuconfig [1]_ to detect and configure pre-cmake-configure things,
like C compiler, linker, CMake generator, and build options. This menuconfig
backed by Kconfiglib [2]_.

To start menuconfig, use::

	make menuconfig
	# or
	# .\make.ps1 menuconfig

Default compiler, linker, and generator are ``gcc``, ``ld.bfd``, and ``ninja``.
You can switch to ``clang``, ``ld.lld``, and ``make`` at this stage. Just pass
``LLVM=1`` and ``MAKEFILE=1`` to make, or define them in environment variable.

After menuconfig, mbuild records compiler, linker, and generator in
``build/tools``, and you can't change them. On Windows port, mbuild only
supports LLVM and Ninja build.

.. [1] https://docs.kernel.org/kbuild/kconfig-language.html

.. [2] https://github.com/ulfalizer/Kconfiglib
