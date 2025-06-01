# SPDX-License-Identifier: GPL-3.0-or-later or MIT

$ErrorActionPreference = 'Stop'

try {
	New-Item -ItemType SymbolicLink -Force -Target .39 .miku >NUL
	Remove-Item .miku

	$symlink_enabled = $true

} catch {
	$symlink_enabled = $false
}

if (!(Test-Path scripts/libkit.py)) {
	if ($symlink_enabled) {
		New-Item -ItemType SymbolicLink -Force `
			 -Target ../../barroit/current/libkit.py `
			 scripts/libkit.py
	} else {
		git apply fixes/scripts-add-libkit-py.patch
	}
}

if ($symlink_enabled) {
	New-Item -ItemType SymbolicLink -Force `
		 -Target build.win32/tests/Testing/Temporary/LastTest.log `
		 tres.win32
	New-Item -ItemType SymbolicLink -Force -Target tres.win32 tres
	New-Item -ItemType SymbolicLink -Force -Target build.win32 build
}

if (!(Test-Path brukit)) {
	exit
}

if ($symlink_enabled) {
	New-Item -ItemType SymbolicLink -Force -Target VERSION.1 VERSION
} else {
	Copy-Item -Force VERSION.1 VERSION
}

if (!(Test-Path brukit.manifest.in)) {
	git apply fixes/build-add-brukit-manifest-in.patch
}

if (!(Test-Path brukit.rc)) {
	git apply fixes/build-add-brukit-rc.patch
}

if (!(Test-Path cherry.ico)) {
	git apply fixes/build-add-cherry-ico.patch
}
