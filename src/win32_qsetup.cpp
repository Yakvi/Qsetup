/* * * * * * * * * * * * * * * * * * * * * * *
 * 
 * Quick MSVC Setup, a helper tool for quick setup of the dev environment on Windows. 
 * By Ivan "Yakvi" Yakymchak
 * Version 1.0, Aug 12-Sep 8, 2019
 * This program implements modified version of microsoft_craziness.h, original version 2, by Jonathan Blow (MIT License). 
 * This code is also released under MIT License which you can find at: https://opensource.org/licenses/MIT
 * 
* * * * * * * * * * * * * * * * * * * * * * * */

#define INI_IMPLEMENTATION
#include <mattias_g/ini.h>
#include "microsoft_craziness.h" 

#define VERSION 100
#define MAX_VAR_LENGTH 32767
#define internal static

internal void
DebugPrintVars(Find_Result* Result)
{
    printf("VISUAL STUDIO PATH\n");
    printf("    MSVC executables:           %ls\n", Result->vs_exe_path);
    printf("    vcruntime Includes:         %ls\n", Result->vs_include_path);
    printf("    vcruntime Libraries:        %ls\n", Result->vs_library_path);

    printf("WINDOWS SDK PATH PATH\n");
    printf("    SDK Version:                %u\n", Result->windows_sdk_version); // Zero if no Windows SDK found.
    printf("    SDK Libs Root Folder:       %ls\n", Result->windows_sdk_lib_root);
    printf("    SDK Universal CRT Libs:     %ls\n", Result->windows_sdk_ucrt_library_path);
    printf("    SDK User Mode Libs:         %ls\n", Result->windows_sdk_um_library_path);

    printf("    SDK Includes Root Folder:   %ls\n", Result->windows_sdk_include_root);
    printf("    SDK Universal CRT Includes: %ls\n", Result->windows_sdk_ucrt_include_path);
    printf("    SDK User Mode Includes:     %ls\n", Result->windows_sdk_um_include_path);
    printf("    SDK Shared Includes:        %ls\n", Result->windows_sdk_shared_include_path);
}

internal bool
AddToEnvironmentVariable(wchar_t* Variable, wchar_t* ExtraParams)
{
    bool Result = false;

    if (ExtraParams)
    {
        wchar_t Buffer[MAX_VAR_LENGTH] = {};
        GetEnvironmentVariableW(Variable, Buffer, MAX_VAR_LENGTH);
        wcscat_s(Buffer, L";");
        wcscat_s(Buffer, ExtraParams);
        SetEnvironmentVariableW(Variable, Buffer);

        Result = true;
    }

    return (Result);
}

internal bool
IsAlreadyRunning()
{
    bool Result = false;
    char Buffer[2] = {};
    const char* Env = "QSETUP";

    GetEnvironmentVariableA(Env, Buffer, 2);
    if (*Buffer == '1')
    {
        Result = true;
    }
    else
    {
        SetEnvironmentVariableA(Env, "1");
    }

    return (Result);
}

internal void
RemoveFileName(char* Path, int MaxLength)
{
    int At = 0;
    int LastBreak = 0;
    while (At < MaxLength)
    {
        char* Pos = &Path[At++];
        if (*Pos)
        {
            if (*Pos == '\\')
            {
                LastBreak = At;
            }
        }
        else
        {
            break;
        }
    }

    if (LastBreak)
    {
        Path[LastBreak] = 0;
    }
}

internal char*
ReadEntireFile(char* Path)
{
    char* Data = 0;
    FILE* fp = fopen(Path, "r");
    if (fp)
    {
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

internal char*
GetFullPath(const char* Filename)
{
    char* Path = (char*)malloc(MAX_VAR_LENGTH);
    HMODULE Module = GetModuleHandleW(0);
    GetModuleFileNameA(Module, Path, MAX_VAR_LENGTH);
    RemoveFileName(Path, MAX_VAR_LENGTH);
    snprintf(Path, MAX_VAR_LENGTH, "%s%s", Path, Filename);

    return (Path);
}

internal void
AddVariablesFromConfigFile(char *Data)
{
    if (Data)
    {
        ini_t* IniData = ini_load(Data, 0);
        free(Data);

        int Section = INI_GLOBAL_SECTION;
        int PropertyCount = ini_property_count(IniData, Section);
        for (int PropertyIndex = 0;
             PropertyIndex < PropertyCount;
             ++PropertyIndex)
        {
            const char* Property = ini_property_name(IniData, Section, PropertyIndex);
            wchar_t PropertyW[MAX_VAR_LENGTH];
            mbstowcs(PropertyW, Property, MAX_VAR_LENGTH);

            const char* Value = ini_property_value(IniData, Section, PropertyIndex);
            wchar_t ValueW[MAX_VAR_LENGTH];
            mbstowcs(ValueW, Value, MAX_VAR_LENGTH);

            AddToEnvironmentVariable(PropertyW, ValueW);
        }

        ini_destroy(IniData);
    }
}

int
main(int argc, const char* argv)
{
    bool ShowDebug = argc > 1;
    if (!IsAlreadyRunning())
    {
        Find_Result Result = find_visual_studio_and_windows_sdk();

        AddToEnvironmentVariable(L"Path", Result.vs_exe_path);

        AddToEnvironmentVariable(L"LIB", Result.vs_library_path);
        AddToEnvironmentVariable(L"LIB", Result.windows_sdk_ucrt_library_path);
        AddToEnvironmentVariable(L"LIB", Result.windows_sdk_um_library_path);

        AddToEnvironmentVariable(L"LIBPATH", Result.windows_sdk_ucrt_library_path);
        AddToEnvironmentVariable(L"LIBPATH", Result.windows_sdk_um_library_path);

        AddToEnvironmentVariable(L"INCLUDE", Result.vs_include_path);
        AddToEnvironmentVariable(L"INCLUDE", Result.windows_sdk_ucrt_include_path);
        AddToEnvironmentVariable(L"INCLUDE", Result.windows_sdk_um_include_path);
        AddToEnvironmentVariable(L"INCLUDE", Result.windows_sdk_winrt_include_path);
        AddToEnvironmentVariable(L"INCLUDE", Result.windows_sdk_shared_include_path);

        char* GlobalConfigPath = GetFullPath("GlobalLibs.ini");
        char* Data = ReadEntireFile(GlobalConfigPath);
        free(GlobalConfigPath);
        AddVariablesFromConfigFile(Data); // Global settings
        Data = ReadEntireFile("Libs.ini");
        AddVariablesFromConfigFile(Data); // Local settings

        system("cls||clear");
        if (ShowDebug) DebugPrintVars(&Result);
        free_resources(&Result);

        printf("Environment variables set.\n");
        system("powershell -NoLogo -NoExit");
    }
    return (0);
}