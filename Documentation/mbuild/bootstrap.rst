.. SPDX-License-Identifier: GPL-3.0-or-later or MIT

================
Mbuild Bootstrap
================

Brukit controls mbuild command and flow using bootstrap. On Unix-like OS,
bootstrap uses Makefile. On Windows It uses PowerShell.

Bootstrap on Windows port is minimum mock of Makefile. It includes most
commands and mechanism on Unix port, and behaviors strictly align with
Makefile.

Some commands are only available to Unix, Windows port doesn't implement them.
The following list shows available commands of bootstrap:

   - menuconfig
   - configure
   - build
   - all (alias of ``make configure && make build``)
   - install
   - uninstall
   - clean
   - distclean
   - tests/all
   - tests/\*.t
   - scripts/\*.sh

To execute a command, run this on source tree::

	make [<command>]
	# or
	# .\make.ps1 [<command>]

You can't execute ``menuconfig``, ``configure``, and ``build`` together. Or you
fuck up :ref:`auto reconfigure <auto_reconf>`.
