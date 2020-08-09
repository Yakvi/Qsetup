# QSetup
#### Quick Setup of the Visual Studio Environment Variables

## Introduction

Tired of waiting several seconds for `vcvarsall` to run? Tired of searching and updating your Visual Studio paths each time there's a new update? This tool is for you. 

QSetup creates a frame around [Jonathan's Blow](https://en.wikipedia.org/wiki/Jonathan_Blow) solution to these problems. Simply boot up your shell (PowerShell or Command Prompt), run `qsetup` and start programming!

## Description

Version 1.1 comes with three executables doing largely the same thing. They only differ in the output.

* `qsetup`: Set up the environment variables and launch a command prompt shell
* `qsetup_ps`: Set up the environment variables and launch a PowerShell
* `qsetup_file`: Export the environment variables to `qsetup.bat`.

## Usage

### Qsetup/Qsetup_ps

`Qsetup`/`Qsetup_ps`: Place the executable anywhere in your hard drive. Run to set up your dev environment. 
`Qsetup_file`: Run each time your Visual Studio install changes. Run `qsetup.bat` to set up your dev environment.

You can setup the automated program/script execution by adding `/k [path to qsetup/qsetup_ps/qsetup.bat]` in your shell's launch arguments.

### Additional Environment Variables

All three executables can set up additional variables as specified by the user. Simply places them in `libs.ini` located in your working directory, with the syntax `VAR=VALUE`. You can also create a `GlobalLibs.ini` at the executables' location. Values from both files, if they exist, will be added alongside the default ones.

### Debug Mode

If you pass any argument to one of the executables, the default variables will be displayed. 

### Dependencies: 

* [ini.h](https://github.com/mattiasgustavsson/libs/blob/master/docs/ini.md)
* [microsoft_craziness.h]((https://pastebin.com/SX3mSC9n)) (modified to have include paths as well and included with the source)

### Limitations 

As mentioned in `microsoft_craziness.h`, if you have multiple versions of Visual Studio installed, we return the first one, rather than the newest.

Additionally, this dev environment is currently hardcoded to x64. This can be worked around in case of interest (add options for ARM/x86).


License: MIT / Public domain dual license