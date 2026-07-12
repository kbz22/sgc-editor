#include "win32_program/windows_init.hpp"
#include "win32_program/controls_fun.hpp"
#include "win32_program/windows_process.hpp"
#include "program/program.hpp"
#include "defaults.hpp"

#undef CreateWindow // avoid macro name conflict with sdl::CreateWindow

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

win32_program::MainWindowContext& win32_program::GetMainWindowContext()
{
    static MainWindowContext context = {};
    return context;
}

void win32_program::Init(HINSTANCE hInstance)
{    
    program::ProgramContext& programContext = program::GetProgramContext();
    programContext.mainWindowContext = std::make_unique<win32_program::MainWindowContext>();
    programContext.mainWindowContext->hInstance = hInstance;

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
