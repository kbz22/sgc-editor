#include "win32_program/controls_fun.hpp"

#include "sgc/sgc_view.hpp"
#include "win32_program/windows_init.hpp"
#include "program/program.hpp"

#include "new_file_dialog.h"
#include <commdlg.h>

void win32_program::OnMenuFileClicked()
{
}

INT_PTR CALLBACK NewFileDialogProc([[maybe_unused]] HWND hDlg, [[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
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
            
            program::StartEditor(buffer);

            EndDialog(hDlg, IDOK);
            return TRUE;
        }


        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}


void win32_program::OnFileNewClicked()
{
    auto& context = GetWin32Context();

    if (context.hTilesetView != nullptr) {
        // sgc::SgcView view(context.hTilesetView);
        // // view.LoadTileset("test_icon.png",24,24);

        if(context.hMainWindow != nullptr)
        DialogBox(
            context.hInstance,            
            MAKEINTRESOURCE(IDD_NEWFILE_DIALOG),
            context.hMainWindow,
            NewFileDialogProc
        );
    }
}