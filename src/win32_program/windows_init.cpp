#include "win32_program/windows_init.hpp"
#include "win32_program/control_setup.hpp"
#include "win32_helpers/create_helpers.hpp"
#include "defaults.hpp"

void InitializeAllControls(win32_program::Win32Context& context)
{
    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_BAR_CLASSES };    
    InitCommonControlsEx(&icc);

    win32_program::SetupMenuBar(context);
    win32_program::SetupToolbar(context);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    using namespace win32_program;

    Win32Context& context = GetWin32Context();

    switch (msg)
    {
    case WM_CREATE:
    {
        context.hMainWindow = hwnd;       
        
        InitializeAllControls(context);
        
        break;
    }
    
    case WM_SIZE:
    {
        RECT rc;
        GetClientRect(hwnd, &rc);

        // --- First rebar ---
        int hTop = (int)SendMessage(context.hRebarTop, RB_GETBARHEIGHT, 0, 0);
        SetWindowPos(context.hRebarTop, nullptr,
            0, 0,
            rc.right, hTop,
            SWP_NOZORDER);

        // --- Second rebar ---
        int hBottom = (int)SendMessage(context.hRebarBottom, RB_GETBARHEIGHT, 0, 0);
        SetWindowPos(context.hRebarBottom, nullptr,
            0, hTop,
            rc.right, hBottom,
            SWP_NOZORDER);

        break;
    }

    case WM_NOTIFY:
    {
        LPNMHDR nm = (LPNMHDR)lParam;

        if (nm->code == TTN_GETDISPINFO)
        {
            NMTTDISPINFO* info = (NMTTDISPINFO*)nm;

            // Convert command ID → button index
            int index = (int)SendMessage(context.hToolbarFunctions, TB_COMMANDTOINDEX, info->hdr.idFrom, 0);

            if (index >= 0)
            {
                TBBUTTON btn{};
                SendMessage(context.hToolbarFunctions, TB_GETBUTTON, index, (LPARAM)&btn);

                info->lpszText = (LPWSTR)btn.dwData;
            }

            return TRUE;
        }
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

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
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        defaults::WindowWidth,
        defaults::WindowHeight,
        nullptr, nullptr, hInstance, nullptr
    );

    return;
}