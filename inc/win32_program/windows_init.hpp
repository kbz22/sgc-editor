#pragma once

#include <windows.h>
#include <commctrl.h>

#include <map>

#include "types.hpp"

namespace win32_program
{
    struct Win32Context
    {
        HINSTANCE hInstance = nullptr;
        HWND hMainWindow = HWND();
        HWND hRebarTop = HWND();
        HWND hRebarBottom = HWND();
        HWND hToolbarMenu = HWND();
        HWND hToolbarFunctions = HWND();
    };

    Win32Context& GetWin32Context();

    void Init(HINSTANCE hInstance, Win32Context& context);        

    void Run();

}
