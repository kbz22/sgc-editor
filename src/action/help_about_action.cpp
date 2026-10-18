#include "action/help_about_action.hpp"
#include "program/program.hpp"
#include "about_dialog.h"
#include "editor_graphics.h"
#include "version.hpp"
#include <windows.h>

action::HelpAboutAction::HelpAboutAction()
{
    m_actionType = ActionType::HelpAbout;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = -1;
    m_actionDescription.toolbarOrder = -1;
    m_actionDescription.menuOrder = 100;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::Default;
    m_actionDescription.menuId = MenuId::Help;
    m_actionDescription.tooltipStringId = std::nullopt;
    m_actionDescription.nameStringId = locale::StringId::NameHelpAbout;
    m_actionDescription.shortcutStringId = locale::StringId::ShortcutNameNewPackage;
    m_actionDescription.shortcutContext = win32_program::ShortcutContext::Global;
}

bool action::HelpAboutAction::IsShortcutable() const
{
    return false;
}

INT_PTR CALLBACK HelpAboutDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

void action::HelpAboutAction::Execute(program::ProgramContext& programContext)
{
    auto hMainWindow = programContext.GetMainWindowHandle();
    auto hInstance = programContext.GetHInstance();
    if(hMainWindow != nullptr)
    {
        DialogBox(
            hInstance,
            MAKEINTRESOURCE(IDD_HELP_ABOUT),
            hMainWindow,
            HelpAboutDialogProc
        );
    }
}

INT_PTR CALLBACK HelpAboutDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto &programContext = program::GetProgramContext();

    switch(msg)
    {
        case WM_INITDIALOG:
        {
            auto &stringLookup = programContext.GetStringLookup();

            auto title = stringLookup.Get(locale::StringId::DialogTitleAbout).value_or(L"DIALOG TITLE");
            auto description = stringLookup.Get(locale::StringId::AboutAppDescription).value_or(L"DESCRIPTION");
            auto appName = stringLookup.Get(locale::StringId::ApplicationName).value_or(L"APP NAME");            
            auto versionString = std::wstring{SGC_EDITOR_VERSION_WSTRING};

            auto nameAndVersionString = std::format(L"{} v.{}",
                appName,
                versionString
            );

            SetDlgItemTextW(hDlg, IDC_HELP_ABOUT_APP_NAME_VERSION, nameAndVersionString.c_str());
            SetDlgItemTextW(hDlg, IDC_HELP_ABOUT_APP_DESCRIPTION, description.c_str());

            SetWindowTextW(hDlg, title.c_str());

            HBITMAP bitmap = static_cast<HBITMAP>(
            LoadImageW(
                programContext.GetHInstance(),
                MAKEINTRESOURCEW(IDB_HELP_ABOUT_LOGO),
                IMAGE_BITMAP,
                0,
                0,
                LR_DEFAULTCOLOR
            ));

            SendDlgItemMessageW(
                hDlg,
                IDC_HELP_ABOUT_LOGO,
                STM_SETIMAGE,
                IMAGE_BITMAP,
                reinterpret_cast<LPARAM>(bitmap)
            );

            return TRUE;
        }

        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case IDCANCEL:
                case IDOK:
                {
                    EndDialog(hDlg, IDOK);
                    return TRUE;
                }                
            }
        }
    }

    return FALSE;
}