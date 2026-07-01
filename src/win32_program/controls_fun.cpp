#include "win32_program/controls_fun.hpp"

#include "sgc_view/sgc_view.hpp"
#include "win32_program/windows_init.hpp"
#include "win32_helpers/file_helpers.hpp"
#include "program/program.hpp"
#include "program/except.hpp"
#include "defaults.hpp"

#include <sgc/data/serialization.hpp> //! temp, testing serialization
#include <fstream>
#include <vector>

#include "new_file_dialog.h"
#include <commdlg.h>

void win32_program::OnMenuFileClicked()
{
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
                program::StartEditor(
                    buffer,
                    GetDlgItemInt(hDlg, IDC_MAP_WIDTH, nullptr, FALSE),
                    GetDlgItemInt(hDlg, IDC_MAP_HEIGHT, nullptr, FALSE),
                    1,
                    1
                );
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


void win32_program::OnFileNewClicked()
{
    auto& context = GetWin32Context();

    if (context.hTilesetView != nullptr) {

        if(context.hMainWindow != nullptr)
        DialogBox(
            context.hInstance,            
            MAKEINTRESOURCE(IDD_NEWFILE_DIALOG),
            context.hMainWindow,
            NewFileDialogProc
        );
    }
}

void win32_program::OnFileSaveClicked()
{
    /* using namespace sgc::data;

    auto& contextWin32 = GetWin32Context();
    auto& contextProgram = program::GetProgramContext();

    auto path = win32_helpers::ShowSaveDialog(contextWin32.hMainWindow);
    if (!path) return;

    BinaryWriter writer;
    MapSerializer::Serialize(
        contextProgram.mapView->GetStorage(),
        writer
    );

    const auto& data = writer.Write()

    std::ofstream file(*path, std::ios::binary);
    file.write(reinterpret_cast<const char*>(data.data()), data.size()); */
}

void win32_program::OnFileOpenClicked()
{
    /* using namespace sgc::data;

    auto& contextWin32 = GetWin32Context();
    auto& contextProgram = program::GetProgramContext();

    auto path = win32_helpers::ShowOpenDialog(contextWin32.hMainWindow);
    if (!path) return;

    std::ifstream file(*path, std::ios::binary);

    std::vector<uint8_t> data(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );

    m_map.Deserialize(reader); */
}