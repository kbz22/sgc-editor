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
    m_actionDescription.shortcutContext = win32_program::ShortcutContext::Global;
}

INT_PTR CALLBACK NewMapFileDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK NewTilesetFileDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK NewPackageFileDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

void action::NewMapDocumentAction::Execute(program::ProgramContext& context)
{     
    if(context.GetSection<sections::TilesetSection>() != nullptr) 
    {
        auto hMainWindow = context.GetMainWindowHandle();
        auto hInstance = context.GetHInstance();
        if(hMainWindow != nullptr)
        {
            DialogBox(
                hInstance,            
                MAKEINTRESOURCE(IDD_NEWMAP_DIALOG),
                hMainWindow,
                NewMapFileDialogProc
            );
        }
    }
}

INT_PTR CALLBACK NewMapFileDialogProc([[maybe_unused]] HWND hDlg, [[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam)
{
    auto refreshTilesetCombobox = [](HWND hDlg) {
        auto &programContext = program::GetProgramContext();
        auto tilesetAssetList = programContext.GetManager<file::FileManager>()->GetAllTilesetDocuments();        

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
        auto packagesList = programContext.GetManager<file::FileManager>()->GetAllPackages();

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
            using namespace locale;
            // Combo box for selecting the default open file type
            auto &programContext = program::GetProgramContext();
            auto stringLookup = programContext.GetStringLookup();
            auto createStr = stringLookup.Get(StringId::DialogCreate).value_or(L"CREATE NAME"); 
            auto cancelStr = stringLookup.Get(StringId::DialogCancel).value_or(L"CANCEL NAME");
            auto nameStr = stringLookup.Get(StringId::NewDialogNameName).value_or(L"NAME NAME");
            auto tilesetStr = stringLookup.Get(StringId::NewDialogTilesetName).value_or(L"TILESET NAME");
            auto newButtonStr = stringLookup.Get(StringId::NewDialogNewName).value_or(L"NEW NAME");
            auto packageStr = stringLookup.Get(StringId::NewDialogPackageName).value_or(L"PACKAGE NAME");
            auto includePackStr = stringLookup.Get(StringId::NewMapDialogIncludeInPackage).value_or(L"INCLUDE TEXT");            

            SetDlgItemTextW(hDlg, IDOK, createStr.c_str());
            SetDlgItemTextW(hDlg, IDCANCEL, cancelStr.c_str());
            SetDlgItemTextW(hDlg, IDC_MAP_LABEL_MAP_NAME, nameStr.c_str());
            SetDlgItemTextW(hDlg, IDC_MAP_LABEL_TILESET, tilesetStr.c_str());
            SetDlgItemTextW(hDlg, IDC_MAP_NEW_TILESET_BUTTON, newButtonStr.c_str());
            SetDlgItemTextW(hDlg, IDC_MAP_INCLUDE_PACKAGE_CHECKBOX, includePackStr.c_str());
            SetDlgItemTextW(hDlg, IDC_MAP_LABEL_PACKAGE, packageStr.c_str());
            SetDlgItemTextW(hDlg, IDC_MAP_NEW_PACKAGE_BUTTON, newButtonStr.c_str());            

            refreshTilesetCombobox(hDlg);
            refreshPackageCombobox(hDlg);
            return TRUE;
        }

        case WM_COMMAND:
        {
            auto commandId = LOWORD(wParam);
            auto &programContext = program::GetProgramContext();
            auto hInstance = programContext.GetHInstance();
            auto hMainWindow = programContext.GetMainWindowHandle();

            switch (commandId)
            {
                case IDC_MAP_NEW_TILESET_BUTTON:
                {
                    auto result = DialogBox(
                        hInstance,
                        MAKEINTRESOURCE(IDD_NEWTILESET_DIALOG),
                        hMainWindow,
                        NewTilesetFileDialogProc
                    );

                    if(result == IDOK) {
                        refreshTilesetCombobox(hDlg);
                    }

                    return TRUE;
                }

                case IDC_MAP_NEW_PACKAGE_BUTTON:
                {
                    auto result = DialogBox(
                        hInstance,
                        MAKEINTRESOURCE(IDD_NEW_PACKAGE_DIALOG),
                        hMainWindow,
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

                    auto stringLookup = programContext.GetStringLookup();
                    auto errorName = stringLookup.Get(locale::StringId::ErrorName).value_or(L"ERROR NAME");
                    auto noMapStr = stringLookup.Get(locale::StringId::UserErrorNoMapName).value_or(L"NO MAP NAME MESSAGE");
                    auto noTilesetStr = stringLookup.Get(locale::StringId::UserErrorNoTileset).value_or(L"NO TILESET MESSAGE");
                    auto noPackageStr = stringLookup.Get(locale::StringId::UserErrorNoPackage).value_or(L"NO PACKAGE MESSAGE");

                    if(mapName.empty()) {
                        MessageBox(hDlg, noMapStr.c_str(), errorName.c_str(), MB_OK | MB_ICONERROR);
                        return TRUE;
                    }

                    if (selectedTilesetIndex == CB_ERR) {
                        MessageBox(hDlg, noTilesetStr.c_str(), errorName.c_str(), MB_OK | MB_ICONERROR);
                        return TRUE;
                    }
                    
                    if (selectedPackageIndex == CB_ERR && includePackage) {
                        MessageBox(hDlg, noPackageStr.c_str(), errorName.c_str(), MB_OK | MB_ICONERROR);
                        return TRUE;
                    }

                    auto tilesetId = SendMessage(tilesetCombo, CB_GETITEMDATA, selectedTilesetIndex, 0);                    

                    // auto &programContext = program::GetProgramContext();
                    if(includePackage)
                    {
                        auto packagePtr = reinterpret_cast<file::PackageFile*>(SendMessage(packageCombo, CB_GETITEMDATA, selectedPackageIndex, 0));
                        auto mapDocument = std::make_unique<file::MapDocument>(mapName, static_cast<sgc::data::AssetId>(tilesetId));
                        packagePtr->AddMapDocument(std::move(mapDocument));
                        
                        // we skip file manager here so it needs a refresh
                        programContext.Refresh();
                    }
                    else 
                    {
                        auto fileManager = programContext.GetManager<file::FileManager>();
                        fileManager->NewMapFile(mapName, static_cast<sgc::data::AssetId>(tilesetId));
                    }

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