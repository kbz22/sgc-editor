#include "win32_program/windows_process.hpp"
#include "win32_program/windows_init.hpp"

#include "win32_program/windows_controls.hpp"

#undef CreateWindow // avoid macro name conflict with sdl::CreateWindow

#include "program/program.hpp"
#include "win32_program/layout_manager.hpp"
#include "win32_helpers/load_bitmap.hpp"

#include <windows.h>
#include <CommCtrl.h>

#include "editor_graphics.h"

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

void SetupImageLists()
{
    auto& programContext = program::GetProgramContext();

    programContext.toolbarIcons = ImageList_Create(24, 24, ILC_COLOR32, 10, 0);
    programContext.toolbarIconsDisabled = ImageList_Create(24, 24, ILC_COLOR32, 10, 0);
    programContext.packageViewFileIcons = ImageList_Create(16, 16, ILC_COLOR32, 10, 0);

    auto hInstance = programContext.mainWindowContext->hInstance;
    auto hBmp = win32_helpers::LoadBitmapFromResource(
        hInstance,
        IDB_TOOLBARICONS
    );
    auto hBmpDisabled = win32_helpers::LoadBitmapFromResource(
        hInstance,
        IDB_TOOLBARICONS_DISABLED
    );
    auto hBmpPackageIcons = win32_helpers::LoadBitmapFromResource(
        hInstance,
        IDB_PACKAGEVIEWICONS
    );

    ImageList_Add(programContext.toolbarIcons, hBmp, NULL);
    ImageList_Add(programContext.toolbarIconsDisabled, hBmpDisabled, NULL);
    ImageList_Add(programContext.packageViewFileIcons, hBmpPackageIcons, NULL);
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
        SetupImageLists();

        programContext.mainWindowContext->hMainWindow = hwnd;
        
        programContext.actionManager = std::make_unique<action::ActionManager>();

        RegisterActions();
        
        INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_BAR_CLASSES };    
        InitCommonControlsEx(&icc);

        StartDefault();
        RefreshEditor();
        HandleResize(hwnd, lParam);        

        break;
    }

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);

        programContext.actionManager->Execute(
            static_cast<action::ActionType>(id), programContext
        );

        break;
    }
    
    case WM_SIZE:
    {    
        HandleResize(hwnd, lParam);
        return TRUE;
    }

    case WM_NOTIFY:
    {
        auto *nm = reinterpret_cast<LPNMHDR>(lParam);        

        if (nm->code == TTN_GETDISPINFO)
        {
            auto* info = reinterpret_cast<NMTTDISPINFO*>(lParam);

            auto action = programContext.actionManager->Find(static_cast<action::ActionType>(info->hdr.idFrom));

            if(!action) {
                break;
            }

            auto tooltipStringId = action->GetTooltipStringId();

            if(tooltipStringId == std::nullopt) {
                break;
            }
            
            auto text = programContext.stringLookup.Get(*tooltipStringId);

            if(!text.has_value()) {
                break;
            }

            wcscpy_s(
                info->szText,
                text.value().c_str()
            );

            info->lpszText = info->szText;

            return TRUE;
        }

        break;
    }   

    case WM_CTLCOLORSTATIC:
    {
        HDC hdc = (HDC)wParam;
        SetBkMode(hdc, TRANSPARENT);
        return (LRESULT)GetStockObject(WHITE_BRUSH);
    }

    case WM_LBUTTONDOWN:
    {
        SetFocus(hwnd);
        auto& layoutManager = GetLayoutManager();
        if (layoutManager.GetDraggedSplitter(hwnd, lParam) != DraggedSplitter::None)
        {
            SetCapture(hwnd);            
            capturedMouse = true;
        }
        break;
    }

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
    
    case WM_RBUTTONDOWN:
    {
        SetFocus(hwnd);
        break;
    }

    case WM_MOUSEMOVE:
    {        
        auto& layoutManager = GetLayoutManager();
        auto draggedSplitter = layoutManager.GetDraggedSplitter(hwnd, lParam);

        if (draggedSplitter != DraggedSplitter::None && capturedMouse)
        {
            layoutManager.HandleDragging(hwnd, lParam);            
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