# SPDX-License-Identifier: GPL-3.0-or-later or MIT
# Using PowerShell feels like shoving a cactus up your ass. This is what
# you get when you let a bunch of clueless idiots design a language. It's
# a fucking nightmare dressed as a tool.

#requires -version 6

param
(
	[ArgumentCompletions('menuconfig', 'configure', 'build', 'all',`
			     'clean', 'distclean', 'install', 'uninstall')]
	[string]$__target = 'build'
)

$ErrorActionPreference = 'Stop'

Set-Alias error Write-Error
Set-Alias rm_f  Remove-Item -Force -ErrorAction SilentlyContinue

$0 = $MyInvocation.MyCommand.Path

function export
{
	param([string]$str)

	$pair  = $str -split '=', 2
	$name  = $pair[0]
	$value = $pair[1]

	[System.Environment]::SetEnvironmentVariable($name, $value, 'Process')
	Set-Variable -Scope Script $name $value
}

$srctree = ($PSScriptRoot.Replace('\','/'))
$gendir  = "$srctree/include/generated"
$objtree = "$srctree/build.win32"

export SRCTREE=$srctree
export GENDIR=$gendir
export OBJTREE=$objtree

export CC=clang.exe
export LD=ld.lld.exe

export DOTPLAT=$srctree/.platform

export USRCONF=$srctree/.config.win32
export DEFCONF=$USRCONF.1

if (Test-Path $USRCONF) {
	export DOTCONF=$USRCONF
} else {
	export DOTCONF=$DEFCONF
}

if (Test-Path $USRCONF) {
	if (Test-Path $DEFCONF) {
		$build_prereq  = 'configure'
		$depconfig_prereq = 'rm-defconf'

	} else {
		$build_prereq  = 'depconfig'
	}

} else {
	$depconfig_prereq = 'gen-defconf'
}

export DEPCONF=$objtree/depconf

$__menuconfig = 1 -shl 0
$__configure  = 1 -shl 1
$__build      = 1 -shl 2
$__all	      = $__configure -bor $__build
$__clean      = 1 -shl 3
$__distclean  = 1 -shl 4
$__install    = 1 -shl 5
$__uninstall  = 1 -shl 6
$__test       = 1 -shl 7
$__depconfig  = 1 -shl 8

switch -CaseSensitive ($__target) {
'menuconfig' { $target = $__menuconfig; break }
'depconfig'  { $target = $__depconfig;  break }
'configure'  { $target = $__configure;  break }
'build'	     { $target = $__build;      break }
'all'	     { $target = $__all;        break }
'clean'	     { $target = $__clean;      break }
'distclean'  { $target = $__distclean;  break }
'install'    { $target = $__install;    break }
'uninstall'  { $target = $__uninstall;  break }
}

export KCONFIG_CONFIG=$DOTCONF
export PYTHONDONTWRITEBYTECODE=y

if (!$target) {
	switch -Wildcard ($__target) {
	't/*' { $target = $__test; break}
	}

	if (!$target) {
		error "unknown target '$__target'"
	}
}

if ($target -band $__menuconfig) {
	python scripts/kconfig.py menuconfig
}

if ($target -band $__depconfig) {
	if ($depconfig_prereq -eq 'gen-defconf') {
		python scripts/kconfig.py alldefconfig
	} elseif ($depconfig_prereq -eq 'rm-defconf') {
		if (Test-Path $DEFCONF) {
			Remove-Item -Force $DEFCONF
		}
	}

	if (!(Test-Path $DEPCONF)) {
		if (!(Test-Path $objtree)) {
			New-Item -ItemType Directory $objtree >NUL
		}
		New-Item $DEPCONF >NUL
	}

	python scripts/depconf.py
}

if ($target -band $__configure) {
	if (!(Test-Path $objtree/features.cmake)) {
		if (!(Test-Path $gendir)) {
			New-Item -ItemType Directory $gendir >NUL
		}

		if (!(Test-Path $objtree)) {
			New-Item -ItemType Directory $objtree >NUL
		}

		python scripts/cc-feature.py cmake
	}

	& $0 depconfig
	cmake -G Ninja -S . -B $objtree
}

if ($target -band $__build) {
	if (!(Test-Path $DOTPLAT) -or (cat $DOTPLAT) -ne 'win32') {
		echo win32 > $DOTPLAT
	}

	switch -CaseSensitive ($build_prereq) {
	'configure' { & $0 configure; break }
	'depconfig' { & $0 depconfig; break }
	}

	cmake --build $objtree --parallel
}

if ($target -band $__clean) {
	cmake --build $objtree --target clean
}

if ($target -band $__distclean) {
	$ErrorActionPreference = 'SilentlyContinue'

	Remove-Item -Force -Recurse include/generated
	Remove-Item -Force $USRCONF*
	Remove-Item -Force $DOTPLAT
	Remove-Item -Force *.manifest
	Remove-Item -Force -Recurse $objtree
}

$cpus = $env:NUMBER_OF_PROCESSORS

if ($target -band $__test) {
	if ($__target -eq 't/all') {
		ctest --test-dir $objtree/tests --parallel $cpus

	} else {
		$t = "$objtree/$__target.exe"

		if (!(Test-Path $t)) {
			error "not a test '$t'"
		}

		& $t
	}
}
