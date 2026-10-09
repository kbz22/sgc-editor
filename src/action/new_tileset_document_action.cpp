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
    m_actionDescription.shortcutStringId = locale::StringId::ShortcutNameNewTileset;
    m_actionDescription.shortcutContext = win32_program::ShortcutContext::Global;
}

INT_PTR CALLBACK NewTilesetFileDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK NewPackageFileDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

void action::NewTilesetDocumentAction::Execute(program::ProgramContext& context)
{    
    if(context.GetSection<sections::TilesetSection>() != nullptr) 
    {
        auto hMainWindow = context.GetMainWindowHandle();
        auto hInstance = context.GetHInstance();

        if(hMainWindow != nullptr)
        {
            DialogBox(
                hInstance,            
                MAKEINTRESOURCE(IDD_NEWTILESET_DIALOG),
                hMainWindow,
                NewTilesetFileDialogProc
            );
        }
    }
}

INT_PTR CALLBACK NewTilesetFileDialogProc([[maybe_unused]] HWND hDlg, [[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam)
{
    auto refreshPackageCombobox = [](HWND hDlg) {
        auto &programContext = program::GetProgramContext();
        auto packagesList = programContext.GetManager<file::FileManager>()->GetAllPackages();

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
            using namespace locale;

            auto &programContext = program::GetProgramContext();
            auto stringLookup = programContext.GetStringLookup();
            auto createStr = stringLookup.Get(StringId::DialogCreate).value_or(L"CREATE NAME"); 
            auto cancelStr = stringLookup.Get(StringId::DialogCancel).value_or(L"CANCEL NAME");
            auto nameStr = stringLookup.Get(StringId::NewDialogNameName).value_or(L"NAME NAME");
            auto imageStr = stringLookup.Get(StringId::NewTilesetDialogImageName).value_or(L"IMAGE NAME");
            auto tileWidthStr = stringLookup.Get(StringId::NewTilesetDialogTileWidthName).value_or(L"TILE WIDTH NAME");
            auto tileHeightStr = stringLookup.Get(StringId::NewTilesetDialogTileHeightName).value_or(L"TILE HEIGHT NAME");
            auto newButtonStr = stringLookup.Get(StringId::NewDialogNewName).value_or(L"NEW NAME");
            auto packageStr = stringLookup.Get(StringId::NewDialogPackageName).value_or(L"PACKAGE NAME");
            auto includePackStr = stringLookup.Get(StringId::NewTilesetDialogIncludeInPackage).value_or(L"INCLUDE TEXT");            

            SetDlgItemTextW(hDlg, IDOK, createStr.c_str());
            SetDlgItemTextW(hDlg, IDCANCEL, cancelStr.c_str());
            SetDlgItemTextW(hDlg, IDC_TILESET_LABEL_TILESET_NAME, nameStr.c_str());
            SetDlgItemTextW(hDlg, IDC_TILESET_LABEL_IMAGE_PATH, imageStr.c_str());
            SetDlgItemTextW(hDlg, IDC_TILESET_LABEL_TILE_WIDTH, tileWidthStr.c_str());
            SetDlgItemTextW(hDlg, IDC_TILESET_LABEL_TILE_HEIGHT, tileHeightStr.c_str());
            SetDlgItemTextW(hDlg, IDC_TILESET_INCLUDE_PACKAGE_CHECKBOX, includePackStr.c_str());
            SetDlgItemTextW(hDlg, IDC_TILESET_LABEL_PACKAGE, packageStr.c_str());
            SetDlgItemTextW(hDlg, IDC_TILESET_NEW_PACKAGE_BUTTON, newButtonStr.c_str());

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

                    auto stringLookup = programContext.GetStringLookup();
                    auto allFilesString = stringLookup.Get(locale::StringId::NameAllFiles);
                    auto imageFilesString = stringLookup.Get(locale::StringId::NameImageFile);

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
                    auto hMainWindow = programContext.GetMainWindowHandle();
                    auto hInstance = programContext.GetHInstance();

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

                    auto stringLookup = program::GetProgramContext().GetStringLookup();
                    auto errorName = stringLookup.Get(locale::StringId::ErrorName).value_or(L"ERROR NAME");
                    auto noTilesetName = stringLookup.Get(locale::StringId::UserErrorNoTilesetName).value_or(L"NO TILESET NAME");
                    auto noImage = stringLookup.Get(locale::StringId::UserErrorNoImage).value_or(L"NO IMAGE");
                    auto badTile = stringLookup.Get(locale::StringId::UserErrorWrongTileSize).value_or(L"WRONG TILE SIZE MESSAGE");
                    auto noPackage = stringLookup.Get(locale::StringId::UserErrorNoPackage).value_or(L"NO PACKAGE");

                    if(tilesetName.empty()){
                        MessageBox(hDlg, noTilesetName.c_str(), errorName.c_str(), MB_OK | MB_ICONERROR);
                        return TRUE;
                    }

                    if(imagePath.empty()) {
                        MessageBox(hDlg, noImage.c_str(), errorName.c_str(), MB_OK | MB_ICONERROR);
                        return TRUE;
                    }
                    
                    if(tileWidth <= 0 || tileHeight <= 0) {
                        MessageBox(hDlg, badTile.c_str(), errorName.c_str(), MB_OK | MB_ICONERROR);
                        return TRUE;
                    }
                    
                    if (selectedPackageIndex == CB_ERR && includePackage) {
                        MessageBox(hDlg, noPackage.c_str(), errorName.c_str(), MB_OK | MB_ICONERROR);
                        return TRUE;
                    }

                    auto &programContext = program::GetProgramContext();
                    auto fileManager = programContext.GetManager<file::FileManager>(); 
                    try {
                        if(includePackage && selectedPackageIndex != CB_ERR) 
                        {
                            auto packagePtr = reinterpret_cast<file::PackageFile*>(SendMessage(packageCombo, CB_GETITEMDATA, selectedPackageIndex, 0));
                            if(packagePtr) {
                                auto newTilesetDoc = std::make_unique<file::TilesetDocument>(
                                    fileManager->NewTilesetDocument(tilesetName, imagePath, tileWidth, tileHeight)
                                );
                                packagePtr->AddTilesetDocument(std::move(newTilesetDoc));
                            }
                        }
                        else 
                        {
                            auto assetManager = programContext.GetManager<file::AssetManager>();
                            fileManager->NewTilesetFile(tilesetName, imagePath, tileWidth, tileHeight, *assetManager);
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