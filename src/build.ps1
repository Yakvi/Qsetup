# NOTE: Compiler params
$c = '-nologo','-EHa-'          #Disable exception handling
# NOTE: Faster compile/runtime
$c += '-fp:fast'                #Floating point behaviour. Precise behaviour usually unnecessary.
$c += '-fp:except-'             #Floating point behaviour. Precise behaviour usually unnecessary.
$c += '-Gm-'                    #Enables minimal rebuild. For faster compile.
$c += '-GR-'                    #Disables run-time type information. For faster compile.
$c += '-Oi'                     #Generates intrinsic functions. For faster runtime.
# NOTE: Ignoring selected warnings:
# $c += '-wd4201'                 #Nonstandard extension used: nameless struct/union
$c += '-wd4100'                 #Unreferenced variable
$c += '-wd4189'                 #Local variable initialized but not used
$c += '-wd4456'                 #Declaration hides previous local declaration
# $c += '-wd4505'                 #Unreferenced local function
# $c += '-wd4127'                 #
# $c += '-wd4302'                 #pointer truncation
# $c += '-wd4311'                 #truncation
# $c += '-wd4838'                 #Conversion from type 1 to type 2 requires a narrowing conversion
# NOTE: Preprocessor directives
$c += '-DQCOMPILE_DEV=1'   #For debug stuff
$c += '-DQCOMPILE_WIN32=1'      #Compiles for Win32
# NOTE: other
$c += '-FC'                     #Display full path of source code
# NOTE: Debug mode
$debug += '-Z7'                     #Generates C 7.0–compatible debugging information.
$debug += '-MTd'                    #Creates debug multithreaded executable
$debug += '-Zo'                     #Enhance Optimized Debugging
$debug += '-WX'                     #Treats all warnings as errors (except the ones below).
$debug += '-W4'                     #Displays all warnings (except the ones below).

# NOTE: linker params, go after the source file
$linker = '/link', '-incremental:no'   #Passes linker parameters from here; Disables incremental linking of the linker
$linker += '-opt:ref'                  #Eliminates functions and data that are never referenced
# NOTE: Extra libraries for win32
$linker = 'user32.lib','gdi32.lib'   #Creates and manipulates the standard elements of the Windows GUI. #Graphics Device Interface, used for primitive drawing functions.
$linker += 'Advapi32.lib'             
$linker += 'Ole32.lib'
$linker += 'OleAut32.lib'

#timeout /t 1

$srcDir = "src"
$buildDir = "build"
$win32file = "win32_qsetup.cpp"

Clear
If (-Not($((get-item $pwd).Name) -match $srcDir)) { cd $srcDir } 
Set-Location ..\$buildDir
Write-Host "Compilation started at $(Get-Date -DisplayHint Date)"
H:\C\vcvars.ps1
Write-Host ""

cl $c -O2 ..\$srcDir\$win32file $linker

Write-Host ""
Write-Host "Compilation finished at $(Get-Date -DisplayHint Date)"

Set-Location ..\$srcDir
