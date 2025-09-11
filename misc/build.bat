@echo off

set code_dir=%cd%\..\src
set build_dir=%cd%\..\build
set compiler=-nologo -EHa- -fp:fast -fp:except- -GR- -Oi -FC
set debug=-Z7 -Zo -MTd -WX -W4 -wd4505 -wd4100 -wd4189 -wd4456
set linker=/link -incremental:no -opt:ref
set libs=Advapi32.lib Ole32.lib OleAut32.lib

pushd %build_dir%

:: Release mode
cl %compiler% -O2 -DCMD_MODE=1        -FeQSetup       %code_dir%\win32_qsetup.cpp %linker% %libs%
cl %compiler% -O2 -DPOWERSHELL_MODE=1 -FeQSetup_ps    %code_dir%\win32_qsetup.cpp %linker% %libs%
cl %compiler% -O2 -DFILE_MODE=1       -FeQSetup_file  %code_dir%\win32_qsetup.cpp %linker% %libs%
cl %compiler% -O2 -DCOMMAND_MODE=1    -FeQCompile     %code_dir%\win32_qsetup.cpp %linker% %libs%

:: debug mode
:: cl %compiler% %debug% -Od -DCMD_MODE=1        -FeQSetup       %code_dir%\win32_qsetup.cpp %linker% %libs%
:: cl %compiler% %debug% -Od -DPOWERSHELL_MODE=1 -FeQSetup_ps    %code_dir%\win32_qsetup.cpp %linker% %libs%
:: cl %compiler% %debug% -Od -DFILE_MODE=1       -FeQSetup_file  %code_dir%\win32_qsetup.cpp %linker% %libs%
:: cl %compiler% %debug% -Od -DCOMMAND_MODE=1    -FeQCompile     %code_dir%\win32_qsetup.cpp %linker% %libs%

:: popd