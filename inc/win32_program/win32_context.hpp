#pragma once

#include <windows.h>

namespace win32_program {

    struct MainWindowContext
    {
        HINSTANCE hInstance = nullptr;
        HWND hMainWindow = HWND();
    };

    MainWindowContext& GetMainWindowContext();

}