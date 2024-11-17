# SPDX-License-Identifier: GPL-3.0-or-later or MIT
# Using PowerShell feels like shoving a cactus up your ass. This is what
# you get when you let a bunch of clueless idiots design a language. It's
# a fucking nightmare dressed as a tool.

#requires -version 6

param
(
	[ArgumentCompletions('configure', 'build', `
			     'all', 'clean', 'distclean', `
			     'menuconfig', 'install', 'uninstall')]
	[string]$first
)

function rmrf
{
	param($name)
	Remove-Item -Recurse -Force -ErrorAction SilentlyContinue -Path $name
}

$tree = $PSScriptRoot
Set-Alias error Write-Error

if ($tree -ne $PWD) {
	error "you need to be inside the source tree ($tree)"
}

$TREE = $tree.Replace('\','/')
$env:TREE = $TREE

$targets = @('configure', 'build', 'all', 'clean', `
	     'distclean', 'menuconfig', 'install', 'uninstall')

if (! $first) {
	# make
	$target = 'build'
} elseif ($first -in $targets) {
	# make all
	$target = $first
} else {
	# make LLVM=1
	# or
	# make LLVM=1 configure
	# or
	# make xxxx
	$args = @($first) + $args
	$target = 'build'
	if ($args[-1] -in $targets) {
		$target = $args[-1]
	}
}

if ($args | Where-Object { $_ -like 'GMAKE=*' }) {
	$GMAKE = 1
}

$env:CC = 'clang'
# Append an extension to make python correctly find that executable
$env:LD = 'ld.lld.exe'

if ($target -eq 'configure' -or $target -eq 'all') {
	if ($GMAKE) {
		$generator = 'Unix Makefiles'
	} else {
		$generator = 'Ninja'
	}
	cmake -G $generator -S $TREE -B $TREE/build
}

if ($target -eq 'build' -or $target -eq 'all') {
	cmake --build $TREE/build --parallel
}

if ($target -eq 'clean') {
	cmake --build $TREE/build --target clean
} elseif ($target -eq 'distclean') {
	rmrf $TREE/include/generated
	rmrf $TREE/.config*
	rmrf $TREE/*.manifest
	rmrf (git ls-files --directory -o 'build')
} elseif ($target -eq 'menuconfig') {
	python $TREE/scripts/kconfig.py menuconfig
}
