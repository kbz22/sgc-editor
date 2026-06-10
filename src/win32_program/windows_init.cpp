#include "win32_program/windows_init.hpp"
#include "win32_program/control_setup.hpp"
#include "win32_program/window_control.hpp"
#include "win32_program/controls_fun.hpp"
#include "win32_helpers/create_helpers.hpp"
#include "defaults.hpp"
#include <windowsx.h>
#include <algorithm>

int toolbarOffset = 0;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    using namespace win32_program;

    Win32Context& context = GetWin32Context();

    switch (msg)
    {
    case WM_CREATE:
    {
        context.hMainWindow = hwnd;       
        
        win32_program::InitializeMenuControls(context);
        win32_program::CreateMainWindowContents(hwnd, context);
        
        break;
    }

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        int code = HIWORD(wParam);
        HWND src = (HWND)lParam;

        switch (id)
        {
            case static_cast<int>(CommandId::MenuFile):
                // OnMenuFileClicked();
                break;

            case static_cast<int>(CommandId::FileNew):
                win32_program::OnFileNewClicked();
                // SetWindowText(context.hTilesetView,   L"Hi :)");
                break;
        }

        break;
    }
    
    case WM_SIZE:
    {
        win32_program::HandleResize(hwnd, lParam, context);

        SetWindowText(context.hLayerListView, L"Layer List");
        SetWindowText(context.hPackageView,   L"Package View");
        SetWindowText(context.hMapView,       L"Map View");
        SetWindowText(context.hTilesetView,   L"Tileset");

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

    case WM_CTLCOLORSTATIC:
    {
        HDC hdc = (HDC)wParam;
        SetBkMode(hdc, TRANSPARENT);
        return (LRESULT)GetStockObject(WHITE_BRUSH);
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        // do NOT fill the background
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_LBUTTONDOWN:
    {
        win32_program::CheckDragging(hwnd, lParam, context);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1;

    case WM_LBUTTONUP:
    {
        auto& state = GetSectionState();
        
        state.draggingLayerHorizontal = false;
        state.draggingTileset = false;
        state.draggingLayerVertical = false;

        ReleaseCapture();
        return 0;
    }

    case WM_MOUSEMOVE:
    {        
        win32_program::HandleDragging(hwnd, lParam, context);

        return 0;
    }

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
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT,
        defaults::WindowWidth,
        defaults::WindowHeight,
        nullptr, nullptr, hInstance, nullptr
    );

    return;
}