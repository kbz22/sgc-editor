#include "win32_program/windows_process.hpp"
#include "win32_program/windows_init.hpp"

#include "win32_program/windows_controls.hpp"
#include "win32_program/window_control.hpp"
#include "win32_program/controls_fun.hpp"

#undef CreateWindow // avoid macro name conflict with sdl::CreateWindow

#include "program/program.hpp"

LRESULT CALLBACK win32_program::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    using namespace win32_program;
    using namespace program;

    Win32Context& context = GetWin32Context();    
    ProgramContext& programContext = program::GetProgramContext();
    
    static bool capturedMouse = false;

    switch (msg)
    {
    case WM_CREATE:
    {
        context.hMainWindow = hwnd;       
        
        INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_BAR_CLASSES };    
        InitCommonControlsEx(&icc);

        WNDCLASSEX wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = DefWindowProc;
        wc.hInstance = context.hInstance;
        wc.lpszClassName = L"SectionWindow";
        RegisterClassEx(&wc);

        programContext.toolbarSection = std::make_unique<sections::ToolbarSection>(context);
        context.hRebarBottom = programContext.toolbarSection->GetHwnd();
        context.hToolbarFunctions = programContext.toolbarSection->GetHwndToolbar();

        programContext.menuSection = std::make_unique<sections::MenuSection>(context);
        context.hRebarTop = programContext.menuSection->GetHwnd();
        context.hToolbarMenu = programContext.menuSection->GetHwndToolbar();
        
        programContext.mapSection = std::make_unique<sections::MapSection>(context);
        context.hMapView = programContext.mapSection->GetHwnd();

        programContext.tilesetSection = std::make_unique<sections::TilesetSection>(context);
        context.hTilesetView = programContext.tilesetSection->GetHwnd();

        programContext.layersSection = std::make_unique<sections::LayersSection>(context);
        context.hLayerListView = programContext.layersSection->GetHwnd();

        programContext.packageSection = std::make_unique<sections::PackageSection>(context);
        context.hPackageView = programContext.packageSection->GetHwnd();

        CreateMainWindowContents(hwnd, context, programContext);
        
        break;
    }

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        // int code = HIWORD(wParam);
        // HWND src = (HWND)lParam;

        switch (id)
        {
            case static_cast<int>(CommandId::MenuFile):
                // OnMenuFileClicked();
                break;

            case static_cast<int>(CommandId::FileNew):
                win32_program::OnFileNewClicked();              
                break;

            case static_cast<int>(CommandId::FileSave):
                win32_program::OnFileSaveClicked();
                break;

            case static_cast<int>(CommandId::FileOpen):
                win32_program::OnFileOpenClicked();
                break;
        }

        break;
    }
    
    case WM_SIZE:
    {
        win32_program::HandleResize(hwnd, lParam, context);
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
        if (win32_program::CheckDragging(hwnd, lParam, context))
        {
            SetCapture(hwnd);
            capturedMouse = true;   
        }
        break;
    }

    case WM_ERASEBKGND:
        return 1;

    case WM_LBUTTONUP:
    {
        auto& state = GetSectionState();

        capturedMouse |= state.draggingLayerHorizontal || state.draggingTileset || state.draggingLayerVertical;
        
        state.draggingLayerHorizontal = false;
        state.draggingTileset = false;
        state.draggingLayerVertical = false;        

        if (capturedMouse)
        {
            ReleaseCapture();
            capturedMouse = false;
        }
        return 0;
    }

    case WM_MOUSEMOVE:
    {        
        win32_program::HandleDragging(hwnd, lParam, context);
        program::HandleResize();
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}