#include "win32_program/windows_process.hpp"
#include "win32_program/windows_init.hpp"

#include "win32_program/windows_controls.hpp"
#include "win32_program/toolbar_functions.hpp"

#undef CreateWindow // avoid macro name conflict with sdl::CreateWindow

#include "program/program.hpp"
#include "win32_program/layout_manager.hpp"

#include "win32_helpers/load_bitmap.hpp"

#include <windows.h>
#include <CommCtrl.h>

#include "defaults.hpp"

win32_program::LayoutManager& GetLayoutManager()
{
    static win32_program::LayoutManager layoutManager(        
        program::GetProgramContext()
    );

    return layoutManager;
}

void HandleResize(HWND hwnd, LPARAM lParam)
{
    auto& layoutManager = GetLayoutManager();
    auto& programContext = program::GetProgramContext();

    layoutManager.HandleResize(
        hwnd,
        lParam
    );

    for(auto section : programContext.sections) {
        section->HandleSectionResize();        
    }

    for(auto section : programContext.sections) {
        section->Update();
    }
}

void UpdateAllSections()
{
    auto& programContext = program::GetProgramContext();

    for(auto section : programContext.sections) {
        section->Update();
    }
}

void LoadToolbarIcons()
{
    auto& programContext = program::GetProgramContext();

    programContext.toolbarIcons = ImageList_Create(24, 24, ILC_COLOR32, 10, 0);
    programContext.toolbarIconsDisabled = ImageList_Create(24, 24, ILC_COLOR32, 10, 0);

    HBITMAP hBmp = win32_helpers::LoadPngWIC(defaults::IconsPath.data());
    HBITMAP hBmpDisabled = win32_helpers::LoadPngWIC(defaults::DisabledIconsPath.data());

    ImageList_Add(programContext.toolbarIcons, hBmp, NULL);
    ImageList_Add(programContext.toolbarIconsDisabled, hBmpDisabled, NULL);
}

LRESULT CALLBACK win32_program::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{    
    using namespace program;

    ProgramContext& programContext = GetProgramContext();    
    
    static bool capturedMouse = false;

    switch (msg)
    {
    case WM_CREATE:
    {        
        LoadToolbarIcons();

        programContext.mainWindowContext->hMainWindow = hwnd;

        auto &context = *programContext.mainWindowContext;
        
        INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_BAR_CLASSES };    
        InitCommonControlsEx(&icc);

        WNDCLASSEX wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = DefWindowProc;
        wc.hInstance = context.hInstance;
        wc.lpszClassName = L"SectionWindow";
        RegisterClassEx(&wc);

        programContext.toolbarSection = std::make_unique<sections::ToolbarSection>(programContext);
        programContext.sections.push_back(programContext.toolbarSection.get());

        programContext.menuSection = std::make_unique<sections::MenuSection>(context);
        programContext.sections.push_back(programContext.menuSection.get());
        
        programContext.mapSection = std::make_unique<sections::MapSection>(context);
        programContext.sections.push_back(programContext.mapSection.get());        

        programContext.tilesetSection = std::make_unique<sections::TilesetSection>(context);
        programContext.sections.push_back(programContext.tilesetSection.get());

        programContext.layersSection = std::make_unique<sections::LayersSection>(programContext);
        programContext.sections.push_back(programContext.layersSection.get());

        programContext.packageSection = std::make_unique<sections::PackageSection>(context);
        programContext.sections.push_back(programContext.packageSection.get());   
        
        HandleResize(hwnd, lParam);

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

            case static_cast<int>(CommandId::EditUndo):
                // win32_program::OnEditUndoClicked();
                break;

            case static_cast<int>(CommandId::EditRedo):
                // win32_program::OnEditRedoClicked();
                break;

            case static_cast<int>(CommandId::LayerAdd):
                win32_program::OnLayerAddClicked();
                break;

            case static_cast<int>(CommandId::LayerRemove):  
                win32_program::OnLayerRemoveClicked();
                break;

            case static_cast<int>(CommandId::LayerMoveUp):
                win32_program::OnLayerMoveUpClicked();
                break;
            
            case static_cast<int>(CommandId::LayerMoveDown):
                win32_program::OnLayerMoveDownClicked();
                break;
        }

        break;
    }
    
    case WM_SIZE:
    {        
        // auto& layoutManager = GetLayoutManager();
        // layoutManager.HandleResize(hwnd, lParam);
        HandleResize(hwnd, lParam);
        break;
    }

    case WM_NOTIFY:
    {
        LPNMHDR nm = (LPNMHDR)lParam;        

        if (nm->code == TTN_GETDISPINFO)
        {
            NMTTDISPINFO* info = (NMTTDISPINFO*)nm;

            // Convert command ID → button index
            int index = static_cast<int>(
                SendMessage(
                    programContext.toolbarSection->GetHwndToolbar(),
                    TB_COMMANDTOINDEX,
                    info->hdr.idFrom,
                    0
                ));

            if (index >= 0)
            {
                TBBUTTON btn{};
                SendMessage(
                    programContext.toolbarSection->GetHwndToolbar(),
                    TB_GETBUTTON,
                    index,
                    (LPARAM)&btn
                );

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
        auto& layoutManager = GetLayoutManager();
        if (layoutManager.GetDraggedSplitter(hwnd, lParam) != DraggedSplitter::None)
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
        auto& layoutManager = GetLayoutManager();
        layoutManager.ResetDraggedSplitter();

        if (capturedMouse)
        {
            ReleaseCapture();
            capturedMouse = false;
        }
        return 0;
    }

    case WM_MOUSEMOVE:
    {        
        auto& layoutManager = GetLayoutManager();
        auto draggedSplitter = layoutManager.GetDraggedSplitter(hwnd, lParam);

        if (draggedSplitter != DraggedSplitter::None && capturedMouse)
        {
            layoutManager.HandleDragging(hwnd, lParam);
            // layoutManager.HandleResize(hwnd, lParam);
            HandleResize(hwnd, lParam);
        }

        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}