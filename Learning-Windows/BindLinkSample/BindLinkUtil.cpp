/*
Copyright (c) Microsoft Corporation

This file contains a sample app showing how to Create and Remove Bind Links.
The assocated README file shows how to use this app to demonstrate the
features of Bind Links.

*/

#include <wil\resource.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>

#if !__has_include(<bindlink.h>)
#error This sample requires the Windows SDK version 10.0.25314.0 or higher.
#endif
#include <bindlink.h>

void usage(FILE* fp)
{
    fprintf(fp, "Usage: BindLink command command-parameters\n");
    fprintf(fp, "Commands:\n");
    fprintf(fp, "   CREATE virtPath targetPath [exceptionPath1 exceptionPath2 ...]\n");
    fprintf(fp, "   REMOVE virtPath\n");
}

void printErrorDetails(PCWSTR command, HRESULT hr)
{
    std::wcout << command << " failed with HRESULT 0x" << std::hex << std::setw(8) << std::setfill(L'0') << hr << "\n";
    wchar_t buffer[32768];
    if (FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, hr,
        0, buffer, ARRAYSIZE(buffer), nullptr))
    {
        std::wcout << buffer << "\n";
    }
}

int handleCreateCommand(int argc, wchar_t* argv[])
{
    // argv[0] = program name
    // argv[1] = "CREATE"
    // argv[2] = virtPath
    // argv[3] = backingPath
    // argv[4...] = exceptions...

    if (argc < 4)
    {
        usage(stderr);
        return 1;
    }

    PCWSTR virtPath = argv[2];
    PCWSTR backingPath = argv[3];

    CREATE_BIND_LINK_FLAGS bindLinkFlags = CREATE_BIND_LINK_FLAG_NONE;

    std::vector<std::wstring> exceptionPathsStr;
    for (int index = 4; index < argc; ++index)
    {
        exceptionPathsStr.push_back(std::wstring(argv[index]));
    }

    std::vector<PCWSTR> exceptionPaths;
    for (const std::wstring& path : exceptionPathsStr)
    {
        exceptionPaths.push_back(path.c_str());
    }

    HRESULT hr;
    if (exceptionPaths.size() > 0)
    {
        hr = CreateBindLink(virtPath, backingPath, bindLinkFlags, exceptionPaths.size(), exceptionPaths.data());
    }
    else
    {
        hr = CreateBindLink(virtPath, backingPath, bindLinkFlags, 0, nullptr);
    }

    if (FAILED(hr))
    {
        printErrorDetails(L"CreateBindLink", hr);
        return hr;
    }

    std::wcout << "Bind Link Created.\n";
    std::wcout << "\"" << virtPath << "\" draws content from \"" << backingPath << "\"\n";

    return 0;
}

int handleRemoveCommand(int argc, wchar_t* argv[])
{
    // argv[0] = program name
    // argv[1] = "REMOVE"
    // argv[2] = virtPath

    if (argc != 3)
    {
        usage(stderr);
        return 1;
    }

    PCWSTR virtPath = argv[2];

    HRESULT hr = RemoveBindLink(virtPath);

    if (FAILED(hr))
    {
        printErrorDetails(L"RemoveBindLink", hr);
        return hr;
    }

    std::wcout << "Bind Link for \"" << virtPath << "\" removed.\n";

    return 0;
}

int wmain(int argc, wchar_t* argv[])
{
    if (argc < 2) {
        usage(stderr);
        return 1;
    }

    if (!_wcsicmp(argv[1], L"CREATE"))
    {
        return handleCreateCommand(argc, argv);
    }
    else if (!_wcsicmp(argv[1], L"REMOVE"))
    {
        return handleRemoveCommand(argc, argv);
    }
    else
    {
        usage(stderr);
        return 1;
    }

    return 0;
}
