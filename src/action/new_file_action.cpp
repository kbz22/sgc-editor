#include "action/new_file_action.hpp"
#include "program/except.hpp"
#include "program/program.hpp"
#include "new_file_dialog.h"
#include <sgc/asset/imageasset.hpp>
#include <sgc/asset/tilesetasset.hpp>

#include <windows.h>
#include <commdlg.h>

action::NewFileAction::NewFileAction()
{
    m_actionType = ActionType::NewFile;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = 0;
    m_actionDescription.toolbarOrder = 100;
    m_actionDescription.menuOrder = 100;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::File;
    m_actionDescription.menuId = MenuId::File;
    m_actionDescription.tooltipStringId = locale::StringId::TooltipFileNew;
    m_actionDescription.nameStringId = locale::StringId::NameNewFile;
}

INT_PTR NewFileDialogCommandHandler(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    (void)msg;
    (void)lParam;

    switch (LOWORD(wParam))
    {

        case IDC_BROWSE_BUTTON:
        {
            wchar_t filePath[MAX_PATH] = {0};

            OPENFILENAME ofn = {};
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hDlg;
            ofn.lpstrFilter = L"Image Files\0*.png;*.bmp;*.jpg;*.jpeg\0All Files\0*.*\0";
            ofn.lpstrFile = filePath;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

            if (GetOpenFileName(&ofn))
            {
                // Put the selected path into the text field
                SetDlgItemText(hDlg, IDC_PATH_EDIT, filePath);
            }
            return TRUE;
        }

        case IDOK:
        {
            wchar_t buffer[MAX_PATH];
            GetDlgItemText(hDlg, IDC_PATH_EDIT, buffer, MAX_PATH);
            
            try
            {
                auto tile_width = GetDlgItemInt(hDlg, IDC_MAP_WIDTH, nullptr, FALSE);
                auto tile_height = GetDlgItemInt(hDlg, IDC_MAP_HEIGHT, nullptr, FALSE);

                auto &programContext = program::GetProgramContext();

                programContext.fileManager->NewMapFile(std::filesystem::path(buffer), tile_width, tile_height);
                
                program::RefreshEditor();
            }
            catch (const program::TileSizeException& e)
            {
                MessageBoxA(hDlg, e.what(), "Error", MB_OK | MB_ICONERROR);
                return TRUE;
            }

            EndDialog(hDlg, IDOK);
            return TRUE;
        }


        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
    }
    
    return FALSE;
}

INT_PTR CALLBACK NewFileDialogProc([[maybe_unused]] HWND hDlg, [[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam)
{
    switch (msg)
    {
        case WM_INITDIALOG:
        {
            // Set the dialog's default values for width and height
            SetDlgItemInt(hDlg, IDC_MAP_WIDTH, defaults::tileSize, FALSE);
            SetDlgItemInt(hDlg, IDC_MAP_HEIGHT, defaults::tileSize, FALSE);
            return TRUE;
        }
        
        case WM_COMMAND:
        {
            return NewFileDialogCommandHandler(hDlg, msg, wParam, lParam);        
        }
    }

    return FALSE;
}

void action::NewFileAction::Execute(program::ProgramContext& context)
{ 
    if(context.tilesetSection != nullptr) {

        if(context.mainWindowContext->hMainWindow != nullptr)
        DialogBox(
            context.mainWindowContext->hInstance,            
            MAKEINTRESOURCE(IDD_NEWFILE_DIALOG),
            context .mainWindowContext->hMainWindow,
            NewFileDialogProc
        );
    }
}