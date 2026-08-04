#include "action/new_document_action.hpp"
#include "program/program.hpp"
#include <windows.h>
#include <commctrl.h>

action::NewDocumentAction::NewDocumentAction()
{
    m_hMenu = CreatePopupMenu();
    m_menuId = static_cast<int>(action::ActionType::NewFile);

    m_actionType = ActionType::NewFile;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = -1;
    m_actionDescription.toolbarOrder = -1;
    m_actionDescription.menuOrder = 100;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::File;
    m_actionDescription.menuId = MenuId::File;
    m_actionDescription.tooltipStringId = std::nullopt;
    m_actionDescription.nameStringId = locale::StringId::NameNewFile;
}