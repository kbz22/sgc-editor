#include "action/new_package_action.hpp"
#include "program/program.hpp"
#include "file/new_file_dialogs.hpp"

#include <windows.h>
#include <commdlg.h>

action::NewPackageAction::NewPackageAction()
{
    m_actionType = ActionType::NewPackage;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = -1;
    m_actionDescription.toolbarOrder = -1;
    m_actionDescription.menuOrder = 130;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::File;
    m_actionDescription.menuId = MenuId::None;
    m_actionDescription.tooltipStringId = std::nullopt;
    m_actionDescription.nameStringId = locale::StringId::NameNewPackage;
    m_actionDescription.shortcutStringId = locale::StringId::ShortcutNameNewPackage;
}

INT_PTR CALLBACK NewPackageFileDialogProc([[maybe_unused]] HWND hDlg, [[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam);

void action::NewPackageAction::Execute(program::ProgramContext &programContext)
{
    if(programContext.GetSection<sections::PackageSection>() != nullptr) 
    {
        auto hMainWindow = programContext.GetMainWindowHandle();
        auto hInstance = programContext.GetHInstance();
        if(hMainWindow != nullptr)
        {
            DialogBox(
                hInstance,
                MAKEINTRESOURCE(IDD_NEW_PACKAGE_DIALOG),
                hMainWindow,
                NewPackageFileDialogProc
            );
        }
    }
}

INT_PTR CALLBACK NewPackageFileDialogProc([[maybe_unused]] HWND hDlg, [[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam)
{
    auto &programContext = program::GetProgramContext();

    switch(msg)
    {
        case WM_INITDIALOG:
        {
            using namespace locale;
            
            auto stringLookup = programContext.GetStringLookup();
            auto createStr = stringLookup.Get(StringId::DialogCreate).value_or(L"CREATE NAME"); 
            auto cancelStr = stringLookup.Get(StringId::DialogCancel).value_or(L"CANCEL NAME");
            auto nameStr = stringLookup.Get(StringId::NewDialogNameName).value_or(L"NAME NAME");

            SetDlgItemTextW(hDlg, IDOK, createStr.c_str());
            SetDlgItemTextW(hDlg, IDCANCEL, cancelStr.c_str());
            SetDlgItemTextW(hDlg, IDC_PACKAGE_LABEL_NAME, nameStr.c_str());

            return TRUE;
        }

        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case IDOK:
                {
                    wchar_t buffer[1024] = {0};

                    GetDlgItemText(hDlg, IDC_PACKAGE_NAME, buffer, 1024);
                    std::wstring packageName{buffer};

                    if(packageName.empty()) {
                        MessageBox(hDlg, L"Please provide a name and select an image.", L"Error", MB_OK | MB_ICONERROR);
                        return TRUE;
                    }

                    programContext.GetManager<file::FileManager>()->NewPackageFile(
                        packageName,
                        {},
                        {},
                        *programContext.GetManager<file::AssetManager>()
                    );

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