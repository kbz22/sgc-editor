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
}

INT_PTR CALLBACK NewPackageFileDialogProc([[maybe_unused]] HWND hDlg, [[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam);

void action::NewPackageAction::Execute(program::ProgramContext &programContext)
{
    if(programContext.packageSection != nullptr) {
        if(programContext.mainWindowContext->hMainWindow != nullptr){
            DialogBox(
                programContext.mainWindowContext->hInstance,            
                MAKEINTRESOURCE(IDD_NEW_PACKAGE_DIALOG),
                programContext.mainWindowContext->hMainWindow,
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

                    programContext.fileManager->NewPackageFile(
                        packageName,
                        {},
                        {},
                        *programContext.assetManager
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