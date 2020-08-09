@echo off

set code=%cd%
set compiler=-nologo -EHa- -fp:fast -fp:except- -GR- -Oi -FC
set debug=-Z7 -Zo -MTd -WX -W4 -wd4505 -wd4100 -wd4189 -wd4456
set linker=/link -incremental:no -opt:ref
set libs=Advapi32.lib Ole32.lib OleAut32.lib

REM pushd ../build

REM Release mode
cl %compiler% -O2                     -FeQSetup      %code%\win32_qsetup.cpp %linker% %libs%
cl %compiler% -O2 -DPOWERSHELL_MODE=1 -FeQSetup_ps   %code%\win32_qsetup.cpp %linker% %libs%
cl %compiler% -O2 -DFILE_MODE=1       -FeQSetup_file %code%\win32_qsetup.cpp %linker% %libs%

REM debug mode
REM cl %compiler% %debug% -Od                     -FeQSetup      %code%\win32_qsetup.cpp %linker% %libs%
REM cl %compiler% %debug% -Od -DPOWERSHELL_MODE=1 -FeQSetup_ps   %code%\win32_qsetup.cpp %linker% %libs%
REM cl %compiler% %debug% -Od -DFILE_MODE=1       -FeQSetup_file %code%\win32_qsetup.cpp %linker% %libs%

REM popd