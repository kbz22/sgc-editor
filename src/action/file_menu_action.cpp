#include "action/file_menu_action.hpp"
#include "program/program.hpp"
#include <windows.h>
#include <commctrl.h>

action::FileMenuAction::FileMenuAction()
{
    m_hMenu = CreatePopupMenu();
    m_menuId = static_cast<int>(action::ActionType::MenuFile);

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

/* void action::FileMenuAction::Execute(program::ProgramContext& context)
{    
    PopupMenuAction::Execute(context);
    return;
} */