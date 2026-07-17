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

        programContext.commandManager = std::make_unique<command::CommandManager>();
        
        INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_BAR_CLASSES };    
        InitCommonControlsEx(&icc);        

        StartDefault();
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
                win32_program::OnEditUndoClicked();
                break;

            case static_cast<int>(CommandId::EditRedo):
                win32_program::OnEditRedoClicked();
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

            case static_cast<int>(CommandId::EditorLayerModeNonActiveTransparent):
                UpdateEditorLayerMode(EditorLayerMode::MultiLayer);
                break;

            case static_cast<int>(CommandId::EditorLayerModeSingleLayer):
                UpdateEditorLayerMode(EditorLayerMode::SingleLayer);
                break;

            case static_cast<int>(CommandId::EditorLayerModeSingleImage):
                UpdateEditorLayerMode(EditorLayerMode::SingleImage);
                break;                

            case static_cast<int>(CommandId::EditorChunkModeFixedSize):
                UpdateEditorChunkMode(EditorChunkMode::FixedChunks);
                break;

            case static_cast<int>(CommandId::EditorChunkModeFree):
                UpdateEditorChunkMode(EditorChunkMode::DynamicChunks);
                break;            
        }

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

            CommandId command = static_cast<CommandId>(info->hdr.idFrom);

            const auto& commandInfo = programContext.commandLookup.Get(command);

            const auto& text = programContext.stringLookup.Get(commandInfo.tooltip);

            wcscpy_s(
                info->szText,
                text.c_str()
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