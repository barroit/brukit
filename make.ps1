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

$BUILD_NAME = 'build.win32'

export SRCTREE=$($PSScriptRoot.Replace('\','/'))
export GENDIR=$SRCTREE/include/generated
export OBJTREE=$SRCTREE/$BUILD_NAME

export CC=clang.exe
export LD=ld.lld.exe

export DOTPLAT=$SRCTREE/.platform

export USRCONF=$SRCTREE/.config.win32
export DEFCONF=$USRCONF.1

if (Test-Path $USRCONF) {
	$DOTCONF    = $USRCONF
} else {
	$DOTCONF    = $DEFCONF
	$GEN_DEFCONF = 1
}

export DOTCONF=$DOTCONF
export KCONFIG_CONFIG=$DOTCONF

if (Test-Path $USRCONF) {
	if (Test-Path $DEFCONF) {
		$RECONFIGURE  = 1
		$RM_DEFCONF = 1
	} else {
		$RERECONFDEP  = 1
	}
}

export RECONFDEP=$OBJTREE/reconfdep

$__menuconfig = 1 -shl 0
$__configure  = 1 -shl 1
$__build      = 1 -shl 2
$__all	      = $__configure -bor $__build
$__clean      = 1 -shl 3
$__distclean  = 1 -shl 4
$__install    = 1 -shl 5
$__uninstall  = 1 -shl 6
$__test       = 1 -shl 7
$__reconfdep  = 1 -shl 8

switch -CaseSensitive ($__target) {
'menuconfig' { $target = $__menuconfig; break }
'reconfdep'  { $target = $__reconfdep;  break }
'configure'  { $target = $__configure;  break }
'build'	     { $target = $__build;      break }
'all'	     { $target = $__all;        break }
'clean'	     { $target = $__clean;      break }
'distclean'  { $target = $__distclean;  break }
'install'    { $target = $__install;    break }
'uninstall'  { $target = $__uninstall;  break }
}

if (!$target) {
	switch -Wildcard ($__target) {
	't/*'       { $target = $__test; break}
	}

	if (!$target) {
		error "unknown target '$__target'"
	}
}

if ($target -band $__menuconfig) {
	python scripts/kconfig.py menuconfig
}

if ($target -band $__reconfdep) {
	if ($GEN_DEFCONF) {
		python scripts/kconfig.py alldefconfig
	}

	if ($RM_DEFCONF) {
		Remove-Item -Force $DEFCONF
	}

	python scripts/reconfdep.py $DOTCONF $RECONFDEP
}

if ($target -band $__configure) {
	if (!(Test-Path $OBJTREE/features.cmake)) {
		if (!(Test-Path $GENDIR)) {
			mkdir $GENDIR
		}

		python scripts/cc-feature.py cmake
	}

	& $0 reconfdep
	cmake -G Ninja -S . -B $OBJTREE
}

if ($target -band $__build) {
	if (!(Test-Path $DOTPLAT) -or (cat $DOTPLAT) -ne 'win32') {
		echo win32 > $DOTPLAT
	}

	if ($RECONFIGURE) {
		& $0 configure
	}

	if ($RERECONFDEP) {
		& $0 reconfdep
	}

	cmake --build $OBJTREE --parallel
}

if ($target -band $__clean) {
	cmake --build $OBJTREE --target clean
}

if ($target -band $__distclean) {
	$dotconfs = Get-ChildItem -Force $env:KCONFIG_CONFIG*
	$buildgens = git ls-files --directory -o $BUILD_NAME

	if (Test-Path include/generated) {
		Remove-Item -Force -Recurse include/generated
	}

	if ($dotconfs) {
		Remove-Item -Force $dotconfs
	}

	if (Test-Path $DOTPLAT) {
		Remove-Item -Force $DOTPLAT
	}

	if (Test-Path *.manifest) {
		Remove-Item -Force *.manifest
	}

	if ($buildgens) {
		Remove-Item -Force -Recurse $buildgens
	}
}

$cpus = $env:NUMBER_OF_PROCESSORS

if ($target -band $__test) {
	if ($__target -eq 't/all') {
		ctest --test-dir $OBJTREE/tests --parallel $cpus
	} else {
		$t = "$OBJTREE/$__target.exe"

		if (!(Test-Path $t)) {
			error "not a test '$t'"
		}

		& $t
	}
}
