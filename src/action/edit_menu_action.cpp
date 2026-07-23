#include "action/edit_menu_action.hpp"

action::EditMenuAction::EditMenuAction()
{
    m_hMenu = CreatePopupMenu();
    m_menuId = static_cast<int>(action::ActionType::MenuEdit);

    m_actionType = ActionType::MenuEdit;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = -1;
    m_actionDescription.toolbarOrder = -1;
    m_actionDescription.menuOrder = 1;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::Default;
    m_actionDescription.menuId = MenuId::Edit;
    m_actionDescription.tooltipStringId = std::nullopt;
    m_actionDescription.nameStringId = locale::StringId::NameEdit;
}