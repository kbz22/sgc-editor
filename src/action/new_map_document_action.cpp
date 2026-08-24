#include "action/new_map_document_action.hpp"
#include "program/except.hpp"
#include "program/program.hpp"
#include "win32_helpers/file_helpers.hpp"
#include "file/new_file_dialogs.hpp"

#include <sgc/asset/imageasset.hpp>
#include <sgc/asset/tilesetasset.hpp>
#include <windows.h>
#include <commdlg.h>

action::NewMapDocumentAction::NewMapDocumentAction()
{
    m_actionType = ActionType::NewMapDocument;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = 0;
    m_actionDescription.toolbarOrder = 100;
    m_actionDescription.menuOrder = 110;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::File;
    m_actionDescription.menuId = MenuId::None;    
    m_actionDescription.tooltipStringId = locale::StringId::TooltipNewMapDocument;
    m_actionDescription.nameStringId = locale::StringId::NameNewMapDocument;
}

INT_PTR CALLBACK NewMapFileDialogProc([[maybe_unused]] HWND hDlg, [[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam);
INT_PTR CALLBACK NewTilesetFileDialogProc([[maybe_unused]] HWND hDlg, [[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam);

void action::NewMapDocumentAction::Execute(program::ProgramContext& context)
{ 
    if(context.tilesetSection != nullptr) {
        if(context.mainWindowContext->hMainWindow != nullptr){
            DialogBox(
                context.mainWindowContext->hInstance,            
                MAKEINTRESOURCE(IDD_NEWMAP_DIALOG),
                context .mainWindowContext->hMainWindow,
                NewMapFileDialogProc
            );
        }
    }
}

INT_PTR CALLBACK NewMapFileDialogProc([[maybe_unused]] HWND hDlg, [[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam)
{
    auto refreshCombobox = [](HWND hDlg) {
        auto &programContext = program::GetProgramContext();
        auto tilesetAssetList = programContext.fileManager->GetAllTilesetDocuments();

        auto tilesetCombo = GetDlgItem(hDlg, IDC_TILESET_COMBO);
        SendMessage(tilesetCombo, CB_RESETCONTENT, 0, 0);
        for (const auto& tilesetDoc : tilesetAssetList) {
            auto index = SendMessage(tilesetCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(tilesetDoc->GetName().c_str()));
            SendMessage(tilesetCombo, CB_SETITEMDATA, index, tilesetDoc->GetTilesetAssetId());
        }

        HWND hCombo = GetDlgItem(hDlg, IDC_TILESET_COMBO);

        if (SendMessage(hCombo, CB_GETCOUNT, 0, 0) > 0)
        {
            SendMessage(hCombo, CB_SETCURSEL, 0, 0);
            EnableWindow(hCombo, TRUE);
        }
        else
        {
            EnableWindow(hCombo, FALSE);
        }
    };

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            refreshCombobox(hDlg);
            return TRUE;
        }

        case WM_COMMAND:
        {
            auto commandId = LOWORD(wParam);

            switch (commandId)
            {
                case IDC_NEW_TILESET_BUTTON:
                {
                    auto &programContext = program::GetProgramContext();

                    auto result = DialogBox(
                        programContext.mainWindowContext->hInstance,
                        MAKEINTRESOURCE(IDD_NEWTILESET_DIALOG),
                        programContext.mainWindowContext->hMainWindow,
                        NewTilesetFileDialogProc
                    );

                    if(result == IDOK) {
                        refreshCombobox(hDlg);
                    }

                    return TRUE;
                }

                case IDOK:
                {
                    wchar_t buffer[1024] = {0};

                    GetDlgItemText(hDlg, IDC_MAP_NAME, buffer, 1024);
                    std::wstring mapName(buffer);

                    auto tilesetCombo = GetDlgItem(hDlg, IDC_TILESET_COMBO);
                    auto selectedIndex = SendMessage(tilesetCombo, CB_GETCURSEL, 0, 0);

                    if(mapName.empty()) {
                        MessageBox(hDlg, L"Please provide a name for the map.", L"Error", MB_OK | MB_ICONERROR);
                        return TRUE;
                    }

                    if (selectedIndex == CB_ERR) {
                        MessageBox(hDlg, L"Please select a tileset.", L"Error", MB_OK | MB_ICONERROR);
                        return TRUE;
                    }

                    auto tilesetId = SendMessage(tilesetCombo, CB_GETITEMDATA, selectedIndex, 0);

                    auto &programContext = program::GetProgramContext();
                    programContext.fileManager->NewMapFile(mapName, static_cast<sgc::data::AssetId>(tilesetId));

                    // program::RefreshEditor();
                    // refresh is now handled by the file manager's on file updated callback

                    EndDialog(hDlg, IDOK);
                    return TRUE;
                }

                case IDCANCEL:
                    EndDialog(hDlg, IDCANCEL);
                    return TRUE;
            }
        }
    }

    return FALSE;
}