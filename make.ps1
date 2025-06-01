# SPDX-License-Identifier: GPL-3.0-or-later or MIT

# PowerShell is just cargo cult junk. They shoved every damn thing into some
# object-pissing pipeline. It's bloated, slow, and forgot it's supposed to be a
# shell. You can't pipe, can't grep, can't script without wading through layers
# of .NET trash.

#requires -version 6

param
(
	# Ugly, but no other way in dumbass pwsh.
	[ArgumentCompletions('menuconfig', 'depconfig', 'configure', `
			     'build', 'all', 'tests', 'unit', 'param', `
			     'clean', 'distclean', 'install', 'uninstall')]
	[string]$1 = 'build',
	[ArgumentCompletions('menuconfig', 'depconfig', 'configure', `
			     'build', 'all', 'tests', 'unit', 'param', `
			     'clean', 'distclean', 'install', 'uninstall')]
	[string]$2 = '',
	[ArgumentCompletions('menuconfig', 'depconfig', 'configure', `
			     'build', 'all', 'tests', 'unit', 'param', `
			     'clean', 'distclean', 'install', 'uninstall')]
	[string]$3 = '',
	[ArgumentCompletions('menuconfig', 'depconfig', 'configure', `
			     'build', 'all', 'tests', 'unit', 'param', `
			     'clean', 'distclean', 'install', 'uninstall')]
	[string]$4 = '',
	[ArgumentCompletions('menuconfig', 'depconfig', 'configure', `
			     'build', 'all', 'tests', 'unit', 'param', `
			     'clean', 'distclean', 'install', 'uninstall')]
	[string]$5 = '',
	[ArgumentCompletions('menuconfig', 'depconfig', 'configure', `
			     'build', 'all', 'tests', 'unit', 'param', `
			     'clean', 'distclean', 'install', 'uninstall')]
	[string]$6 = ''
)

$argv = @($1, $2, $3, $4, $5, $6) + $args | Where-Object { $_ }

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
$__test_all   = 1 -shl 9

$target = 0
$tests = @()

foreach ($arg in $argv) {
	$__target = 0

	switch -CaseSensitive ($arg) {
	'menuconfig' { $__target = $__menuconfig; break }
	'depconfig'  { $__target = $__depconfig;  break }
	'configure'  { $__target = $__configure;  break }
	'build'	     { $__target = $__build;      break }
	'all'	     { $__target = $__all;        break }
	'clean'	     { $__target = $__clean;      break }
	'distclean'  { $__target = $__distclean;  break }
	'install'    { $__target = $__install;    break }
	'uninstall'  { $__target = $__uninstall;  break }
	'tests'      { $__target = $__test_all;   break }
	}

	if ($target -band $__target) {
		error "overriding recipe for target '$arg'"
	}

	if (!$__target -and `
	    (($arg -like 'unit/*') -or ($arg -like 'param/*'))) {
		$file = "$objtree/tests/$arg.exe"

		if (Test-Path $file) {
			$__target = $__test

			$tests += $file
		}
	}

	if (!$__target) {
		error "no rule to make target '$arg'"
	}

	$target = $target -bor $__target
}

export KCONFIG_CONFIG=$DOTCONF
export PYTHONDONTWRITEBYTECODE=y

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

if ($target -band $__install) {
	cmake --install $objtree
}

if ($target -band $__uninstall) {
	Get-Content $objtree/install_manifest.txt | ForEach-Object {
		Remove-Item -Force -Verbose $_
	}
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

if ($target -band $__test_all) {
	ctest --test-dir $objtree/tests --parallel $env:NUMBER_OF_PROCESSORS
}

if ($target -band $__test) {
	foreach ($test in $tests) {
		& $test
	}
}
