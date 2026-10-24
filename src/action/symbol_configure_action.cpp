#include "action/symbol_configure_action.hpp"
#include "program/program.hpp"
#include "symbol_config_dialog.h"
#include <windows.h>
#include <commdlg.h>

action::SymbolConfigureAction::SymbolConfigureAction()
{
    m_actionType = ActionType::SymbolConfiguration;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = 39;
    m_actionDescription.toolbarOrder = 3000;
    m_actionDescription.menuOrder = 1000;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::SymbolTools;
    m_actionDescription.menuId = MenuId::Edit;
    m_actionDescription.tooltipStringId = locale::StringId::TooltipSymbolConfiguration;
    m_actionDescription.nameStringId = locale::StringId::NameSymbolConfiguration;
}

INT_PTR CALLBACK SymbolConfigureDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

void action::SymbolConfigureAction::Execute(program::ProgramContext& context)
{     
    if(context.GetSection<sections::TilesetSection>() != nullptr) 
    {
        auto hMainWindow = context.GetMainWindowHandle();
        auto hInstance = context.GetHInstance();
        if(hMainWindow != nullptr)
        {
            DialogBox(
                hInstance,            
                MAKEINTRESOURCE(IDD_SYMBOL_CONFIG),
                hMainWindow,
                SymbolConfigureDialogProc
            );
        }
    }
}

INT_PTR CALLBACK SymbolConfigureDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_INITDIALOG:
        {
            using namespace locale;

            return TRUE;
        }

        case WM_COMMAND:
        {
            auto commandId = LOWORD(wParam);

            switch (commandId)
            {
                case IDOK:
                {
                    EndDialog(hDlg, IDCANCEL);
                    return TRUE;
                }

                case IDCANCEL:
                {
                    EndDialog(hDlg, IDCANCEL);
                    return TRUE;
                }

                default:
                {
                    return TRUE;
                }
            }            
        }
    }

    return FALSE;
}
