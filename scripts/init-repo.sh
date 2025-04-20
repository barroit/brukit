#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

TREE=$(dirname $(dirname $(readlink -f $BASH_SOURCE)))
if [[ $TREE != $PWD ]]; then
	>&2 echo you need to be inside the source tree
	exit 1
fi

source scripts/libutil.sh

if [[ ! -f .program.in ]]; then
	die 'missing .program.in'
fi

repo=$(st_section repo .program.in)
license=$(st_section license .program.in)
name=$(st_section name .program.in)

conf=$(section conf .program.in)
icon=$(section icon .program.in)

fhc_key='"fileHeaderComment.parameter"."*"'
fhc_license=\"$license\"
__license=$(jq -r ".$fhc_key.license" .vscode/settings.json)

jq --tab  ".$fhc_key.license = $fhc_license" .vscode/settings.json > $$.tmp
mv $$.tmp .vscode/settings.json

setting=$(jq -r ".$fhc_key" .vscode/settings.json)
year=$(jq -r .year <<< $setting)

no_arch=$(grep no_arch <<< $conf)

section readme .program.in > README

cat <<EOF > .program
name	$name
version	0.0
license	$license

license_old	$__license
EOF

cat <<EOF > $name.manifest.in
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
// SPDX-License-Identifier: $license
#include "resdecl.h"

RES_PROG_ICON	ICON	"$icon"
EOF

if [[ ! $no_arch ]]; then
	mkdir arch arch/x86 arch/arm64 arch/riscv

	cat <<EOF > arch/.map
riscv	riscv	riscv64
arm64	arm64	aarch64
x86	x86_64	amd64	x64
EOF
fi

perl -i -ne 'last if /^# みくみくにしてあげる♪$/; print' .gitignore
sed -i -e :a -e '/^\n*$/{$d;N;};/\n$/ba' .gitignore

cat <<EOF > .pickignore
.program.in.example
scripts/init.sh
EOF

rm .program.in*
(rm $0) &

printf 'brukit\t%s\n' $repo > .remote
printf 'this\t%s\n' $(git remote get-url origin) >> .remote

scripts/initloc.sh

git add .
scripts/fix-license.sh "$__license" "$license"

scripts/syncmo.sh
make distclean

git add .
git commit -sm 'INITIAL CONSUMER'
