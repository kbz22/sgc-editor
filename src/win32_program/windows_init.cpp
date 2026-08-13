#include "win32_program/windows_init.hpp"
#include "win32_program/windows_process.hpp"
#include "win32_program/shortcut_manager.hpp"
#include "program/program.hpp"
#include "defaults.hpp"
#include "locale/string_lookup.hpp"
#include "editor_graphics.h"

#undef CreateWindow // avoid macro name conflict with sdl::CreateWindow

#include <windowsx.h>
#include <algorithm>
#include <stdexcept>

void win32_program::Run()
{
    using namespace program;

    ProgramContext& programContext = GetProgramContext();
    ShortcutManager& shortcutManager = *programContext.shortcutManager;

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        // direct capture of keyboard shortcuts for actions
        if(msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN) {
            ShortcutModifier modifier = ShortcutManager::GetShortcutModifierFromKeyState();
            Shortcut shortcut{ modifier, static_cast<uint32_t>(msg.wParam) };
            action::ActionType actionId = action::ActionType::Default;

            if(programContext.activeSection != nullptr)
            {
                actionId = shortcutManager.GetActionForShortcut(
                    shortcut,
                    ShortcutContext::MapEditor
                );
            }
            else
            {
                actionId = shortcutManager.GetActionForShortcut(
                    shortcut,
                    ShortcutContext::Global
                );
            }            

            if(actionId != action::ActionType::Default) {
                programContext.actionManager->Execute(
                    actionId, programContext
                );
                continue;
            }
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

win32_program::MainWindowContext& win32_program::GetMainWindowContext()
{
    static MainWindowContext context = {};
    return context;
}

void CenterWindow(HWND hwnd)
{
    RECT rc;
    GetWindowRect(hwnd, &rc);

    int windowWidth = rc.right - rc.left;
    int windowHeight = rc.bottom - rc.top;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int xPos = (screenWidth - windowWidth) / 2;
    int yPos = (screenHeight - windowHeight) / 2;

    SetWindowPos(hwnd, nullptr, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
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
    wc.hIcon = (HICON)LoadImage(
        hInstance,
        MAKEINTRESOURCE(IDI_APPICON),
        IMAGE_ICON,
        32, 32,
        LR_DEFAULTCOLOR
    );
    wc.hIconSm = (HICON)LoadImage(
        hInstance,
        MAKEINTRESOURCE(IDI_APPICON),
        IMAGE_ICON,
        GetSystemMetrics(SM_CXSMICON),
        GetSystemMetrics(SM_CYSMICON),
        0
    );

    RegisterClassEx(&wc);

    auto windowTitle = programContext.stringLookup.Get(locale::StringId::WindowTitle);

    if(windowTitle == std::nullopt) {
        throw std::runtime_error("Missing window title string.");
    }

    HWND hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        windowTitle->c_str(),
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        defaults::WindowWidth,
        defaults::WindowHeight,
        nullptr, nullptr, hInstance, nullptr
    );

    CenterWindow(hwnd);

    if (hwnd == nullptr)
    {
        MessageBox(nullptr, L"Failed to create main window.", L"Error", MB_OK | MB_ICONERROR);        
    }

    return;
}
