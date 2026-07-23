#include "action/file_menu_action.hpp"
#include "program/program.hpp"
#include <windows.h>
#include <commctrl.h>

action::FileMenuAction::FileMenuAction()
{
    m_hMenu = CreatePopupMenu();

    m_actionType = ActionType::MenuFile;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = -1;
    m_actionDescription.toolbarOrder = -1;
    m_actionDescription.menuOrder = 1;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::Default;
    m_actionDescription.menuId = MenuId::File;
    m_actionDescription.tooltipStringId = locale::StringId::TextMissing;    
    m_actionDescription.nameStringId = locale::StringId::NameFile;
}

void action::FileMenuAction::Execute(program::ProgramContext& context)
{
    RECT rc{};
    int intId = static_cast<int>(action::ActionType::MenuFile);

    SendMessage(
        context.menuSection->GetHwndToolbar(),
        TB_GETRECT,
        intId,
        reinterpret_cast<LPARAM>(&rc));

    MapWindowPoints(
        context.menuSection->GetHwndToolbar(),
        HWND_DESKTOP,
        reinterpret_cast<POINT*>(&rc),
        2);

    auto actionMenuFile = context.actionManager->Find(action::ActionType::MenuFile);

    if(actionMenuFile == nullptr || !actionMenuFile->IsPopup()) {
        return;
    }

    auto hMenu = dynamic_cast<action::FileMenuAction*>(actionMenuFile)->GetHMenu();

    TrackPopupMenu(
        hMenu,
        TPM_LEFTALIGN | TPM_TOPALIGN,
        rc.left,
        rc.bottom,
        0,
        context.menuSection->GetHwndToolbar(),
        nullptr
    );

    return;
}