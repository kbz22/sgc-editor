#include <windows.h>

#include "win32_program/windows_init.hpp"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    using namespace win32_program;

    Win32Context& context = GetWin32Context();
    Init(hInstance, context);

    ShowWindow(context.hMainWindow, nCmdShow);

    Run();
    
    return 0;
}
