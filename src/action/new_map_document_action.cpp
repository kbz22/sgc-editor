#include "action/new_map_document_action.hpp"
#include "program/except.hpp"
#include "program/program.hpp"
#include "win32_helpers/file_helpers.hpp"
#include "file/new_file_dialogs.hpp"
#include "file/map_document.hpp"

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
    m_actionDescription.shortcutStringId = locale::StringId::ShortcutNameNewMap;
}

INT_PTR CALLBACK NewMapFileDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK NewTilesetFileDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK NewPackageFileDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

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
    auto refreshTilesetCombobox = [](HWND hDlg) {
        auto &programContext = program::GetProgramContext();
        auto tilesetAssetList = programContext.fileManager->GetAllTilesetDocuments();        

        auto tilesetCombo = GetDlgItem(hDlg, IDC_MAP_TILESET_COMBO);
        SendMessage(tilesetCombo, CB_RESETCONTENT, 0, 0);

        for (const auto& tilesetDoc : tilesetAssetList) {
            auto index = SendMessage(tilesetCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(tilesetDoc->GetName().c_str()));
            SendMessage(tilesetCombo, CB_SETITEMDATA, index, tilesetDoc->GetTilesetAssetId());
        }

        HWND hTilesetCombo = GetDlgItem(hDlg, IDC_MAP_TILESET_COMBO);
        bool hasTilesets = (SendMessage(hTilesetCombo, CB_GETCOUNT, 0, 0) > 0);
        auto selectedTilesetIndex = SendMessage(hTilesetCombo, CB_GETCURSEL, 0, 0);

        if(selectedTilesetIndex == -1){
            SendMessage(hTilesetCombo, CB_SETCURSEL, 0, 0);
        }
        EnableWindow(hTilesetCombo, hasTilesets);
    };

    auto refreshPackageCombobox = [](HWND hDlg) {
        auto &programContext = program::GetProgramContext();
        auto packagesList = programContext.fileManager->GetAllPackages();

        auto packageCombo = GetDlgItem(hDlg, IDC_MAP_PACKAGE_COMBO);
        SendMessage(packageCombo, CB_RESETCONTENT, 0, 0);

        for (const auto& package : packagesList) {
            auto index = SendMessage(packageCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(package->GetName().c_str()));
            SendMessage(packageCombo, CB_SETITEMDATA, index, reinterpret_cast<LPARAM>(package));
        }

        HWND hPackageCombo = GetDlgItem(hDlg, IDC_MAP_PACKAGE_COMBO);
        bool includePackage = (IsDlgButtonChecked(hDlg, IDC_MAP_INCLUDE_PACKAGE_CHECKBOX) == BST_CHECKED);
        bool hasPackages = (SendMessage(hPackageCombo, CB_GETCOUNT, 0, 0) > 0);
        auto selectedPackageIndex = SendMessage(hPackageCombo, CB_GETCURSEL, 0, 0);
        
        if(selectedPackageIndex == -1){
            SendMessage(hPackageCombo, CB_SETCURSEL, 0, 0);
        }

        EnableWindow(GetDlgItem(hDlg, IDC_MAP_NEW_PACKAGE_BUTTON), includePackage);        
        EnableWindow(hPackageCombo, includePackage && hasPackages);
    };

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            refreshTilesetCombobox(hDlg);
            refreshPackageCombobox(hDlg);
            return TRUE;
        }

        case WM_COMMAND:
        {
            auto commandId = LOWORD(wParam);

            switch (commandId)
            {
                case IDC_MAP_NEW_TILESET_BUTTON:
                {
                    auto &programContext = program::GetProgramContext();

                    auto result = DialogBox(
                        programContext.mainWindowContext->hInstance,
                        MAKEINTRESOURCE(IDD_NEWTILESET_DIALOG),
                        programContext.mainWindowContext->hMainWindow,
                        NewTilesetFileDialogProc
                    );

                    if(result == IDOK) {
                        refreshTilesetCombobox(hDlg);
                    }

                    return TRUE;
                }

                case IDC_MAP_NEW_PACKAGE_BUTTON:
                {
                    auto &programContext = program::GetProgramContext();

                    auto result = DialogBox(
                        programContext.mainWindowContext->hInstance,
                        MAKEINTRESOURCE(IDD_NEW_PACKAGE_DIALOG),
                        programContext.mainWindowContext->hMainWindow,
                        NewPackageFileDialogProc
                    );

                    if(result == IDOK) {
                        refreshPackageCombobox(hDlg);
                    }

                    return TRUE;
                }

                case IDC_MAP_INCLUDE_PACKAGE_CHECKBOX:
                {
                    if (HIWORD(wParam) == BN_CLICKED)
                    {
                        refreshPackageCombobox(hDlg);
                    }

                    return TRUE;
                }

                case IDOK:
                {
                    wchar_t buffer[1024] = {0};

                    GetDlgItemText(hDlg, IDC_MAP_NAME, buffer, 1024);
                    std::wstring mapName(buffer);

                    auto tilesetCombo = GetDlgItem(hDlg, IDC_MAP_TILESET_COMBO);
                    auto selectedTilesetIndex = SendMessage(tilesetCombo, CB_GETCURSEL, 0, 0);
                    auto packageCombo = GetDlgItem(hDlg, IDC_MAP_PACKAGE_COMBO);
                    auto selectedPackageIndex = SendMessage(packageCombo, CB_GETCURSEL, 0, 0);
                    bool includePackage = (IsDlgButtonChecked(hDlg, IDC_MAP_INCLUDE_PACKAGE_CHECKBOX) == BST_CHECKED);

                    if(mapName.empty()) {
                        MessageBox(hDlg, L"Please provide a name for the map.", L"Error", MB_OK | MB_ICONERROR);
                        return TRUE;
                    }

                    if (selectedTilesetIndex == CB_ERR) {
                        MessageBox(hDlg, L"Please select a tileset.", L"Error", MB_OK | MB_ICONERROR);
                        return TRUE;
                    }
                    
                    if (selectedPackageIndex == CB_ERR && includePackage) {
                        MessageBox(hDlg, L"Please select a package.", L"Error", MB_OK | MB_ICONERROR);
                        return TRUE;
                    }

                    auto tilesetId = SendMessage(tilesetCombo, CB_GETITEMDATA, selectedTilesetIndex, 0);                    

                    auto &programContext = program::GetProgramContext();
                    if(includePackage)
                    {
                        auto packagePtr = reinterpret_cast<file::PackageFile*>(SendMessage(packageCombo, CB_GETITEMDATA, selectedPackageIndex, 0));
                        auto mapDocument = std::make_unique<file::MapDocument>(mapName, static_cast<sgc::data::AssetId>(tilesetId));
                        packagePtr->AddMapDocument(std::move(mapDocument));
                        
                        // we skip file manager here so it needs a refresh
                        program::RefreshEditor();
                    }
                    else 
                    {
                        programContext.fileManager->NewMapFile(mapName, static_cast<sgc::data::AssetId>(tilesetId));
                    }                    

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