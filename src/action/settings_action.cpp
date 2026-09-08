#include "action/settings_action.hpp"
#include "program/program.hpp"
#include "settings_dialog.h"

action::SettingsAction::SettingsAction()
{
    m_actionType = ActionType::Settings;
    m_enabled = true;
    m_checked = false;
    
    m_actionDescription.imageIndex = 3;
    m_actionDescription.toolbarOrder = -1; //!idk
    m_actionDescription.menuOrder = 900;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::Preferences;
    m_actionDescription.menuId = MenuId::File;
    m_actionDescription.tooltipStringId = locale::StringId::TooltipSettings;
    m_actionDescription.nameStringId = locale::StringId::NameSettings;
}

INT_PTR CALLBACK SettingsDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

void action::SettingsAction::Execute(program::ProgramContext& context)
{
    if(context.mainWindowContext->hMainWindow != nullptr){
        DialogBox(
            context.mainWindowContext->hInstance,            
            MAKEINTRESOURCE(IDD_SETTINGS),
            context.mainWindowContext->hMainWindow,
            SettingsDialogProc
        );
    }
}

INT_PTR CALLBACK SettingsDialogProc(HWND hDlg, UINT msg, WPARAM wParam, [[maybe_unused]] LPARAM lParam)
{
    switch (msg)
    {
        case WM_INITDIALOG:
        {
            return TRUE;
        }

        case WM_COMMAND:
        {
            auto commandId = LOWORD(wParam);

            switch (commandId)
            {
                case IDOK:
                {
                    EndDialog(hDlg, IDOK);
                    return TRUE;
                }

                case IDCANCEL:
                {
                    EndDialog(hDlg, IDCANCEL);
                    return TRUE;
                }
            }
            break;
        }
    }

    return FALSE;
}