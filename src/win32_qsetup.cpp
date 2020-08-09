/* * * * * * * * * * * * * * * * * * * * * * *
 * 
 * Quick MSVC Setup, a helper tool for quick setup of the dev environment on Windows. 
 * By Ivan "Yakvi" Yakymchak
 * Version 1.1.0, 2020
 * This program implements modified version of microsoft_craziness.h, version 2, by Jonathan Blow (MIT License)
 * This program implements ini.h, by Mattias Gustavsson (MIT / PD)
 * This code is also released under dual license, details can be found at the end of the file.
 * 
* * * * * * * * * * * * * * * * * * * * * * * */

#define INI_IMPLEMENTATION
#include <mattias_g/ini.h>
#include "microsoft_craziness.h"

#define VERSION 110
#define DEBUG_STRING_LENGTH 32767
#define MAX_VAR_LENGTH 32767
#define local static

local void
DebugPrintVars(Find_Result* Result, wchar_t* DebugInfo, int DebugInfoLength)
{
    if (Result->windows_sdk_version) {
        const int BufferCount = 200;
        wchar_t   Buffer[BufferCount];
        _snwprintf(Buffer, BufferCount, L"VISUAL STUDIO PATH\n");
        wcscat_s(DebugInfo, DebugInfoLength, Buffer);

        _snwprintf(Buffer, BufferCount, L"    MSVC executables:           %ls\n", Result->vs_exe_path);
        wcscat_s(DebugInfo, DebugInfoLength, Buffer);
        _snwprintf(Buffer, BufferCount, L"    vcruntime Includes:         %ls\n", Result->vs_include_path);
        wcscat_s(DebugInfo, DebugInfoLength, Buffer);
        _snwprintf(Buffer, BufferCount, L"    vcruntime Libraries:        %ls\n\n", Result->vs_library_path);
        wcscat_s(DebugInfo, DebugInfoLength, Buffer);

        _snwprintf(Buffer, BufferCount, L"WINDOWS SDK PATH PATH\n");
        wcscat_s(DebugInfo, DebugInfoLength, Buffer);
        _snwprintf(Buffer, BufferCount, L"    SDK Libs Root Folder:       %ls\n", Result->windows_sdk_lib_root);
        wcscat_s(DebugInfo, DebugInfoLength, Buffer);
        _snwprintf(Buffer, BufferCount, L"    SDK Universal CRT Libs:     %ls\n", Result->windows_sdk_ucrt_library_path);
        wcscat_s(DebugInfo, DebugInfoLength, Buffer);
        _snwprintf(Buffer, BufferCount, L"    SDK User Mode Libs:         %ls\n", Result->windows_sdk_um_library_path);
        wcscat_s(DebugInfo, DebugInfoLength, Buffer);

        _snwprintf(Buffer, BufferCount, L"    SDK Includes Root Folder:   %ls\n", Result->windows_sdk_include_root);
        wcscat_s(DebugInfo, DebugInfoLength, Buffer);
        _snwprintf(Buffer, BufferCount, L"    SDK Universal CRT Includes: %ls\n", Result->windows_sdk_ucrt_include_path);
        wcscat_s(DebugInfo, DebugInfoLength, Buffer);
        _snwprintf(Buffer, BufferCount, L"    SDK User Mode Includes:     %ls\n", Result->windows_sdk_um_include_path);
        wcscat_s(DebugInfo, DebugInfoLength, Buffer);
        _snwprintf(Buffer, BufferCount, L"    SDK WinRT Includes:         %ls\n", Result->windows_sdk_winrt_include_path);
        wcscat_s(DebugInfo, DebugInfoLength, Buffer);
        _snwprintf(Buffer, BufferCount, L"    SDK Shared Includes:        %ls\n", Result->windows_sdk_shared_include_path);
        wcscat_s(DebugInfo, DebugInfoLength, Buffer);
    }
}

local bool
AddToEnvironmentVariable(wchar_t* Variable, wchar_t* ExtraParams, wchar_t* BatBuffer)
{
    bool Result = false;

    if (ExtraParams) {
#if FILE_MODE
        wchar_t Buffer[MAX_VAR_LENGTH] = {};
        swprintf_s(Buffer, L"set %s=\"%s\";%%%s%%\n", Variable, ExtraParams, Variable);
        wcscat_s(BatBuffer, MAX_VAR_LENGTH, Buffer);
#else
        wchar_t Buffer[MAX_VAR_LENGTH] = {};
        GetEnvironmentVariableW(Variable, Buffer, MAX_VAR_LENGTH);
        wcscat_s(Buffer, L";");
        wcscat_s(Buffer, ExtraParams);
        SetEnvironmentVariableW(Variable, Buffer);
#endif
        Result = true;
    }

    return (Result);
}

local bool
IsAlreadyRunning()
{
    bool        Result    = false;
    char        Buffer[2] = {};
    const char* Env       = "QSETUP";

    GetEnvironmentVariableA(Env, Buffer, 2);
    if (*Buffer == '1') {
        Result = true;
    }
    else {
        SetEnvironmentVariableA(Env, "1");
    }

    return (Result);
}

local void
RemoveFileName(char* Path, int MaxLength)
{
    int At        = 0;
    int LastBreak = 0;
    while (At < MaxLength) {
        char* Pos = &Path[At++];
        if (*Pos) {
            if (*Pos == '\\') {
                LastBreak = At;
            }
        }
        else {
            break;
        }
    }

    if (LastBreak) {
        Path[LastBreak] = 0;
    }
}

local void
WriteEntireFile(wchar_t* Buffer, wchar_t* Path)
{
    size_t StrSize = wcslen(Buffer);
    if (StrSize > 0) {
        FILE* fp = _wfopen(Path, L"wt+,ccs=UTF-8");
        if (fp) {
            if (fwrite(Buffer, sizeof(wchar_t), StrSize, fp) != StrSize) {
                printf("fwrite failed!\n");
            }

            fclose(fp);
        }
    }
}

local char*
ReadEntireFile(char* Path)
{
    char* Data = 0;
    FILE* fp   = fopen(Path, "r");
    if (fp) {
        fseek(fp, 0, SEEK_END);
        int Size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        Data = (char*)malloc(Size + 1);
        fread(Data, 1, Size, fp);
        Data[Size] = '\0';
        fclose(fp);
    }

    return (Data);
}

local char*
GetFullPath(const char* Filename)
{
    char*   Path   = (char*)malloc(MAX_VAR_LENGTH);
    HMODULE Module = GetModuleHandleW(0);
    GetModuleFileNameA(Module, Path, MAX_VAR_LENGTH);
    RemoveFileName(Path, MAX_VAR_LENGTH);
    snprintf(Path, MAX_VAR_LENGTH, "%s%s", Path, Filename);

    return (Path);
}

local void
AddVariablesFromConfigFile(char* Data, wchar_t* BatBuffer)
{
    if (Data) {
        ini_t* IniData = ini_load(Data, 0);
        free(Data);

        int Section       = INI_GLOBAL_SECTION;
        int PropertyCount = ini_property_count(IniData, Section);
        for (int PropertyIndex = 0;
             PropertyIndex < PropertyCount;
             ++PropertyIndex) {
            const char* Property = ini_property_name(IniData, Section, PropertyIndex);
            wchar_t     PropertyW[MAX_VAR_LENGTH];
            mbstowcs(PropertyW, Property, MAX_VAR_LENGTH);

            const char* Value = ini_property_value(IniData, Section, PropertyIndex);
            wchar_t     ValueW[MAX_VAR_LENGTH];
            mbstowcs(ValueW, Value, MAX_VAR_LENGTH);

            AddToEnvironmentVariable(PropertyW, ValueW, BatBuffer);
        }

        ini_destroy(IniData);
    }
}

int
main(int argc, const char* argv)
{
    bool ShowDebug = argc > 1; // any argument enables debug mode

#if FILE_MODE
    wchar_t BatBuffer[MAX_VAR_LENGTH] = {};
    wcscat_s(BatBuffer, L"@echo off\n");
#else
    wchar_t BatBuffer[1];
#endif

    if (!IsAlreadyRunning()) {
        Find_Result WinSDK = find_visual_studio_and_windows_sdk();

        if (WinSDK.windows_sdk_version) {
            AddToEnvironmentVariable(L"Path", WinSDK.vs_exe_path, BatBuffer);

            AddToEnvironmentVariable(L"LIB", WinSDK.vs_library_path, BatBuffer);
            AddToEnvironmentVariable(L"LIB", WinSDK.windows_sdk_ucrt_library_path, BatBuffer);
            AddToEnvironmentVariable(L"LIB", WinSDK.windows_sdk_um_library_path, BatBuffer);

            AddToEnvironmentVariable(L"LIBPATH", WinSDK.windows_sdk_ucrt_library_path, BatBuffer);
            AddToEnvironmentVariable(L"LIBPATH", WinSDK.windows_sdk_um_library_path, BatBuffer);

            AddToEnvironmentVariable(L"INCLUDE", WinSDK.vs_include_path, BatBuffer);
            AddToEnvironmentVariable(L"INCLUDE", WinSDK.windows_sdk_ucrt_include_path, BatBuffer);
            AddToEnvironmentVariable(L"INCLUDE", WinSDK.windows_sdk_um_include_path, BatBuffer);
            AddToEnvironmentVariable(L"INCLUDE", WinSDK.windows_sdk_winrt_include_path, BatBuffer);
            AddToEnvironmentVariable(L"INCLUDE", WinSDK.windows_sdk_shared_include_path, BatBuffer);
        }

        char* GlobalConfigPath = GetFullPath("GlobalLibs.ini");
        char* Data             = ReadEntireFile(GlobalConfigPath);
        free(GlobalConfigPath);
        AddVariablesFromConfigFile(Data, BatBuffer); // Global settings
        Data = ReadEntireFile("Libs.ini");
        AddVariablesFromConfigFile(Data, BatBuffer); // Local settings

        wchar_t DebugOutput[DEBUG_STRING_LENGTH];
        DebugOutput[0] = L'\n';
        if (ShowDebug) DebugPrintVars(&WinSDK, DebugOutput, DEBUG_STRING_LENGTH);

        int SDK_version = WinSDK.windows_sdk_version;
        free_resources(&WinSDK); // Cleanup

        // Create work environment / output file
        wchar_t Message[200];
        if (SDK_version) {
            _snwprintf_s(Message, 200, L"Environment variables set. Windows SDK v. %d\n", SDK_version);
        }
        else {
            _snwprintf_s(Message, 200, L"Environment variables set. Windows SDK not found\n");
        }

#if POWERSHELL_MODE
        system("clear");
        wprintf(Message);
        system("powershell -NoLogo -NoExit");
#elif FILE_MODE
        wcscat_s(BatBuffer, L"echo ");
        wcscat_s(BatBuffer, Message);
        WriteEntireFile(BatBuffer, L"qsetup.bat");
        system("echo Qsetup.bat generated in %cd%.");
#else // CMD mode
        system("cls");
        wchar_t Command[100];
        wcscat_s(Command, L"cmd /k \"cls & echo ");
        wcscat_s(Command, Message);
        wcscat_s(Command, L"\"");

        _wsystem(Command);
#endif
        if (ShowDebug) wprintf(DebugOutput);
    }
    else {
        printf("This program is already running! Please exit the shell and try again.");
    }
    return (0);
}

/*

revision history:
    1.1     added CMD and bat export modes, cleanup
    1.0     first publicly released version (Powershell mode)

*/

/*
------------------------------------------------------------------------------

This software is available under 2 licenses - you may choose the one you like.

------------------------------------------------------------------------------

ALTERNATIVE A - MIT License

Copyright (c) 2020 Ivan Yakymchak

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.

------------------------------------------------------------------------------

ALTERNATIVE B - Public Domain (www.unlicense.org)

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this 
software, either in source code form or as a compiled binary, for any purpose, 
commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this 
software dedicate any and all copyright interest in the software to the public 
domain. We make this dedication for the benefit of the public at large and to 
the detriment of our heirs and successors. We intend this dedication to be an 
overt act of relinquishment in perpetuity of all present and future rights to 
this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

------------------------------------------------------------------------------
*/
