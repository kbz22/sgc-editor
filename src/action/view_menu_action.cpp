#include "action/view_menu_action.hpp"

action::ViewMenuAction::ViewMenuAction()
{
    m_hMenu = CreatePopupMenu();
    m_menuId = static_cast<int>(action::ActionType::MenuView);

    m_actionType = ActionType::MenuView;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = -1;
    m_actionDescription.toolbarOrder = -1;
    m_actionDescription.menuOrder = 1;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::Default;
    m_actionDescription.menuId = MenuId::View;
    m_actionDescription.tooltipStringId = locale::StringId::TextMissing;    
    m_actionDescription.nameStringId = locale::StringId::NameView;
}