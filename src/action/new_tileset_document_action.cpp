#include "action/new_tileset_document_action.hpp"
#include "program/program.hpp"
#include "program/except.hpp"
#include "win32_helpers/file_helpers.hpp"
#include "file/new_file_dialogs.hpp"

#include <windows.h>
#include <commdlg.h>

action::NewTilesetDocumentAction::NewTilesetDocumentAction()
{
    m_actionType = ActionType::NewTilesetDocument;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = -1;
    m_actionDescription.toolbarOrder = -1;
    m_actionDescription.menuOrder = 120;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::File;
    m_actionDescription.menuId = MenuId::None;    
    m_actionDescription.tooltipStringId = std::nullopt;
    m_actionDescription.nameStringId = locale::StringId::NameNewTilesetDocument;
}

INT_PTR CALLBACK NewTilesetFileDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK NewPackageFileDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

void action::NewTilesetDocumentAction::Execute(program::ProgramContext& context)
{
    if(context.tilesetSection != nullptr) {
        if(context.mainWindowContext->hMainWindow != nullptr){
            DialogBox(
                context.mainWindowContext->hInstance,            
                MAKEINTRESOURCE(IDD_NEWTILESET_DIALOG),
                context .mainWindowContext->hMainWindow,
                NewTilesetFileDialogProc
            );
        }
    }
}

INT_PTR CALLBACK NewTilesetFileDialogProc([[maybe_unused]] HWND hDlg, [[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam)
{
    auto refreshPackageCombobox = [](HWND hDlg) {
        auto &programContext = program::GetProgramContext();
        auto packagesList = programContext.fileManager->GetAllPackages();

        auto packageCombo = GetDlgItem(hDlg, IDC_TILESET_PACKAGE_COMBO);
        SendMessage(packageCombo, CB_RESETCONTENT, 0, 0);

        for (const auto& package : packagesList) {
            auto index = SendMessage(packageCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(package->GetName().c_str()));
            SendMessage(packageCombo, CB_SETITEMDATA, index, reinterpret_cast<LPARAM>(package));
        }

        HWND hPackageCombo = GetDlgItem(hDlg, IDC_TILESET_PACKAGE_COMBO);
        bool includePackage = (IsDlgButtonChecked(hDlg, IDC_TILESET_INCLUDE_PACKAGE_CHECKBOX) == BST_CHECKED);
        bool hasPackages = (SendMessage(hPackageCombo, CB_GETCOUNT, 0, 0) > 0);
        auto selectedPackageIndex = SendMessage(hPackageCombo, CB_GETCURSEL, 0, 0);
        
        if(selectedPackageIndex == -1){
            SendMessage(hPackageCombo, CB_SETCURSEL, 0, 0);
        }

        EnableWindow(GetDlgItem(hDlg, IDC_TILESET_NEW_PACKAGE_BUTTON), includePackage);
        EnableWindow(hPackageCombo, includePackage && hasPackages);
    };

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            SetDlgItemInt(hDlg, IDC_TILESET_TILE_WIDTH, defaults::tileSize, FALSE);
            SetDlgItemInt(hDlg, IDC_TILESET_TILE_HEIGHT, defaults::tileSize, FALSE);
            refreshPackageCombobox(hDlg);
            return TRUE;
        }

        case WM_COMMAND:
        {
            auto commandId = LOWORD(wParam);

            switch (commandId)
            {
                case IDC_TILESET_BROWSE_IMAGE_BUTTON:
                {
                    auto &programContext = program::GetProgramContext();

                    auto allFilesString = programContext.stringLookup.Get(locale::StringId::NameAllFiles);
                    auto imageFilesString = programContext.stringLookup.Get(locale::StringId::NameImageFile);

                    if(allFilesString == std::nullopt || imageFilesString == std::nullopt) {
                        throw program::StringNotFoundException("String not found for AllFiles or NameImageFile.");
                    }

                    auto fullImageString =  (imageFilesString.value() + L" (.png; .bmp; .jpg; .jpeg)");
                    
                    auto result = win32_helpers::ShowOpenDialog(hDlg, {
                        { fullImageString.c_str(), { L".png", L".bmp", L".jpg", L".jpeg" } },
                        { allFilesString.value().c_str(), { L"*.*" } }
                    });

                    if(result.has_value()) {
                        SetDlgItemText(hDlg, IDC_TILESET_IMAGE_PATH, result->c_str());
                    }

                    return TRUE;
                }

                case IDC_TILESET_NEW_PACKAGE_BUTTON:
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

                case IDC_TILESET_INCLUDE_PACKAGE_CHECKBOX:
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

                    GetDlgItemText(hDlg, IDC_TILESET_NAME, buffer, 1024);
                    std::wstring tilesetName(buffer);

                    GetDlgItemText(hDlg, IDC_TILESET_IMAGE_PATH, buffer, 1024);
                    std::filesystem::path imagePath(buffer);

                    auto tileWidth = GetDlgItemInt(hDlg, IDC_TILESET_TILE_WIDTH, nullptr, FALSE);
                    auto tileHeight = GetDlgItemInt(hDlg, IDC_TILESET_TILE_HEIGHT, nullptr, FALSE);                    
                    bool includePackage = (IsDlgButtonChecked(hDlg, IDC_TILESET_INCLUDE_PACKAGE_CHECKBOX) == BST_CHECKED);
                    auto packageCombo = GetDlgItem(hDlg, IDC_TILESET_PACKAGE_COMBO);
                    auto selectedPackageIndex = SendMessage(packageCombo, CB_GETCURSEL, 0, 0);

                    if(tilesetName.empty() || imagePath.empty()) {
                        MessageBox(hDlg, L"Please provide a name and select an image.", L"Error", MB_OK | MB_ICONERROR);
                        return TRUE;
                    }
                    
                    if(tileWidth <= 0 || tileHeight <= 0) {
                        MessageBox(hDlg, L"Tile width and height must be greater than zero.", L"Error", MB_OK | MB_ICONERROR);
                        return TRUE;
                    }
                    
                    if (selectedPackageIndex == CB_ERR && includePackage) {
                        MessageBox(hDlg, L"Please select a package.", L"Error", MB_OK | MB_ICONERROR);
                        return TRUE;
                    }

                    auto &programContext = program::GetProgramContext();
                    try {
                        if(includePackage && selectedPackageIndex != CB_ERR) 
                        {
                            auto packagePtr = reinterpret_cast<file::PackageFile*>(SendMessage(packageCombo, CB_GETITEMDATA, selectedPackageIndex, 0));
                            if(packagePtr) {
                                auto newTilesetDoc = std::make_unique<file::TilesetDocument>(
                                    programContext.fileManager->NewTilesetDocument(tilesetName, imagePath, tileWidth, tileHeight)
                                );
                                packagePtr->AddTilesetDocument(std::move(newTilesetDoc));
                            }
                        }
                        else 
                        {
                            programContext.fileManager->NewTilesetFile(tilesetName, imagePath, tileWidth, tileHeight, *programContext.assetManager);
                        }
                        
                    }
                    catch (const program::AssetCacheException) {
                        MessageBox(hDlg, L"An asset with the same identifier already exists.", L"Error", MB_OK | MB_ICONERROR);
                        return TRUE;
                    }
                    catch (const std::exception& e) {
                        MessageBoxA(hDlg, e.what(), "Error", MB_OK | MB_ICONERROR);
                        return TRUE;
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