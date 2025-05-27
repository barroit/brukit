#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

set -e

. scripts/use-posix-libkit.sh

if [ ! -f .program.in ]; then
	die 'missing .program.in'
fi

repo=$(ini_section_st repo .program.in)
license_new=$(ini_section_st license .program.in)
name=$(ini_section_st name .program.in)

conf=$(ini_section conf .program.in)
icon=$(ini_section icon .program.in)

fhc_key='"fileHeaderComment.parameter"."*"'
license_old=$(jq -r ".$fhc_key.license" .vscode/settings.json)

jq --tab  ".$fhc_key.license = \"$license_new\"" .vscode/settings.json >$$.tmp
mv $$.tmp .vscode/settings.json

setting=$(jq -r ".$fhc_key" .vscode/settings.json)
year=$(printf '%s\n' "$setting" | jq -r .year)

no_arch=$(printf '%s\n' "$conf" | grep no_arch)

perl -i -ne 'last if /^# みくみくにしてあげる♪$/; print' .gitignore

content=$(cat .gitignore)

printf '%s\n' "$content" >.gitignore

ini_section readme .program.in >README

cat <<EOF >$name.manifest.in
<?xml version="1.0" encoding="UTF-8"?>
<!-- SPDX-License-Identifier: $license -->
<assembly
  xmlns="urn:schemas-microsoft-com:asm.v1"
  xmlns:asmv3="urn:schemas-microsoft-com:asm.v3"
  manifestVersion="1.0">
  <assemblyIdentity
    type="win32"
    name="@prog_name_manifest@"
    version="@prog_version_manifest@"/>
  <compatibility xmlns="urn:schemas-microsoft-com:compatibility.v1">
    <maxversiontested Id="10.0.22621.1778"/>
    <!-- Windows 10 and Windows 11 -->
    <supportedOS Id="{8e0f7a12-bfb3-4fe8-b9a5-48fd50a15a9a}"/>
    <!-- Windows 8.1 -->
    <supportedOS Id="{1f676c76-80e1-4239-95bb-83d0f6d0da78}"/>
    <!-- Windows 8 -->
    <supportedOS Id="{4a2f28e3-53b9-4441-ba9c-d69d4a4a6e38}"/>
    <!-- Windows 7 -->
    <supportedOS Id="{35138b9a-5d96-4fbd-8e2d-a2440225f93a}"/>
    <!-- windows Vista -->
    <supportedOS Id="{e2011457-1546-43c5-a5fe-008deee3d3f0}" />
  </compatibility>
  <dependency>
    <dependentAssembly>
      <assemblyIdentity
        type="win32"
        name="Microsoft.Windows.Common-Controls"
        version="6.0.0.0"
        processorArchitecture="*"
        publicKeyToken="6595b64144ccf1df"
        language="*"/>
    </dependentAssembly>
  </dependency>
  <asmv3:application>
    <asmv3:windowsSettings
      xmlns="http://schemas.microsoft.com/SMI/2005/WindowsSettings">
      <dpiAware>true</dpiAware>
    </asmv3:windowsSettings>
    <asmv3:windowsSettings
      xmlns:ws2="http://schemas.microsoft.com/SMI/2016/WindowsSettings">
      <ws2:longPathAware>true</ws2:longPathAware>
    </asmv3:windowsSettings>
    <asmv3:windowsSettings
      xmlns="http://schemas.microsoft.com/SMI/2019/WindowsSettings">
      <activeCodePage>UTF-8</activeCodePage>
    </asmv3:windowsSettings>
    <asmv3:windowsSettings
      xmlns="http://schemas.microsoft.com/SMI/2020/WindowsSettings">
      <heapType>SegmentHeap</heapType>
    </asmv3:windowsSettings>
  </asmv3:application>
</assembly>
EOF

cat <<EOF > $name.rc
// SPDX-License-Identifier: $license_new
#include "resdecl.h"

RES_PROG_ICON	ICON	"$icon"
EOF

if [ ! $no_arch ]; then
	mkdir arch arch/x86 arch/arm64 arch/riscv

	cat <<-EOF >arch/.map
	riscv	riscv	riscv64
	arm64	arm64	aarch64
	x86	x86_64	amd64	x64
	EOF
fi

cat <<EOF >.pickignore
.program.in.example
scripts/init-consumer.sh
EOF

if [ "$license_new" != "$license_old" ]; then
	cat <<-EOF >FIXLICENSE
	new	$license_new
	old	$license_old
	EOF

	git add .
	scripts/fix-license.sh "$license_old" "$license_new"
fi

rm .program.in*
(rm $0) &

scripts/setup-repo.sh
scripts/syncmo.sh

make distclean

git add .
git commit -sm 'INITIAL CONSUMER'
