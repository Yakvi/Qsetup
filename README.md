# Quick Setup
#### Quick configuration of Visual Studio environment variables

Usage: Place qsetup executable anywhere in your hard drive and add it to the path. Run with qsetup. 

Optionally, create _GlobalLibs.ini_ in the same folder of the executable with extra INCLUDE, LIB, and LIBPATH paths. This will be read alongside _Libs.ini_ which you should store in your working directory. 

Qsetup will run a new PowerShell/CMD shell with the specified environment variables set up; these will not persist upon exit. Alternatively, it will output results in a .bat file. Depends on the compile mode.

Dependencies: 

* [ini.h](https://github.com/mattiasgustavsson/libs/blob/master/docs/ini.md)
* microsoft_craziness.h (modified to have include paths as well and included with the source)

License: MIT