#include "win32_program/control_setup.hpp"
#include "win32_program/window_control.hpp"
#include "win32_program/controls_fun.hpp"
#include "win32_program/windows_process.hpp"
#include "defaults.hpp"

#include <windowsx.h>
#include <algorithm>

void win32_program::Run()
{
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

win32_program::Win32Context& win32_program::GetWin32Context()
{
    static Win32Context context = {};
    return context;
}

void win32_program::Init(HINSTANCE hInstance, Win32Context &context)
{    
    context.hInstance = hInstance;

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"SGCEditorMainWindow";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        L"SGC Editor",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT,
        defaults::WindowWidth,
        defaults::WindowHeight,
        nullptr, nullptr, hInstance, nullptr
    );

    return;
}