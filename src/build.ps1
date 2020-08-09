# NOTE: Compiler params
$c = '-nologo','-EHa-'          #Disable exception handling
$c += '-fp:fast'                #Floating point behaviour. Precise behaviour usually unnecessary.
$c += '-fp:except-'             #Floating point behaviour. Precise behaviour usually unnecessary.
$c += '-Gm-'                    #Enables minimal rebuild. For faster compile.
$c += '-GR-'                    #Disables run-time type information. For faster compile.
$c += '-Oi'                     #Generates intrinsic functions. For faster runtime.
$c += '-FC'                     #Display full path of source code (for error jumping)
# NOTE: Debug mode
$debug = '-Z7', '-Zo'          # Generates C 7.0–compatible debugging information, enhance optimized debugging 
$debug += '-MTd'               # Create debug multithreaded executable
$debug += '-WX'                # Treats all warnings as errors (except the ones below).
$debug += '-W4'                # Displays all warnings (except the ones below).
# NOTE: Ignoring selected warnings:
$debug += '-wd4505'            # Unreferenced function
$debug += '-wd4100'            # Unreferenced variable
$debug += '-wd4189'            # Local variable initialized but not used
$debug += '-wd4456'            # Declaration hides previous local declaration

# NOTE: linker params, go after the source file
$linker = '/link', '-incremental:no'   #Passes linker parameters from here; Disables incremental linking of the linker
$linker += '-opt:ref'                  #Eliminates functions and data that are never referenced
# NOTE: Extra libraries for win32
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

# NOTE release mode
cl $c -O2 -DPOWERSHELL_MODE=1 -FeQSetup_ps   ..\$srcDir\$win32file  $linker
cl $c -O2 -DFILE_MODE=1       -FeQSetup_file ..\$srcDir\$win32file  $linker
cl $c -O2                     -FeQSetup      ..\$srcDir\$win32file $linker

# NOTE debug mode
# cl $c $debug -Od -DPOWERSHELL_MODE=1 -FeQSetup_ps   ..\$srcDir\$win32file  $linker
# cl $c $debug -Od -DFILE_MODE=1       -FeQSetup_file ..\$srcDir\$win32file  $linker
# cl $c $debug -Od                     -FeQSetup      ..\$srcDir\$win32file $linker

Write-Host ""
Write-Host "Compilation finished at $(Get-Date -DisplayHint Date)"

Set-Location ..\$srcDir
