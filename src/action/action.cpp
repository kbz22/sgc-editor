#include "action/action.hpp"
#include "locale/string_lookup.hpp"
#include "program/program.hpp"

action::ActionType action::Action::GetType() const
{
    return m_actionType;
}

bool action::Action::IsEnabled() const
{
    return m_enabled;
}

bool action::Action::IsChecked() const
{
    return m_checked;
}

bool action::Action::IsCheckGroupItem() const
{
    return m_actionDescription.checkGroupItem;
}

bool action::Action::IsToolbarItem() const
{
    return (m_actionDescription.toolbarOrder >= 0);
}

bool action::Action::IsPopup() const
{
    return false;
}

bool action::Action::IsWidget() const
{
    return false;
}

void action::Action::SetEnabled(bool enabled)
{
    m_enabled = enabled;
}

void action::Action::SetChecked(bool checked)
{
    m_checked = checked;
}

int action::Action::GetToolbarImageIndex() const
{
    return m_actionDescription.imageIndex;
}

int action::Action::GetToolbarOrder() const
{
    return m_actionDescription.toolbarOrder;
}

int action::Action::GetMenuIndex() const
{
    return m_actionDescription.menuOrder;
}

action::GroupId action::Action::GetGroupId() const
{
    return m_actionDescription.groupId;
}

action::MenuId action::Action::GetMenuId() const
{
    return m_actionDescription.menuId;
}

std::optional<locale::StringId> action::Action::GetTooltipStringId() const
{
    return m_actionDescription.tooltipStringId;
}

std::optional<locale::StringId> action::Action::GetNameStringId() const
{
    return m_actionDescription.nameStringId;
}

program::ShortcutContext action::Action::GetShortcutContext() const
{
    return m_actionDescription.shortcutContext;
}

std::vector<program::Shortcut> action::Action::GetDefaultShortcuts() const
{
    return m_actionDescription.shortcuts;
}

std::optional<std::vector<action::Action*>> action::Action::GetPopupMenuItems() const
{
    return std::nullopt;
}