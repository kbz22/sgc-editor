#include "action/action.hpp"
#include "locale/command_lookup.hpp"
#include "locale/string_lookup.hpp"
#include "program/program.hpp"

action::ActionType action::Action::GetType() const
{
    return m_actionType;
}

std::wstring action::Action::GetTooltip(program::ProgramContext const& context) const
{
    auto cmdInfo = context.commandLookup.Get(m_commandId);
    return context.stringLookup.Get(cmdInfo.tooltip);
}

bool action::Action::IsEnabled() const
{
    return m_enabled;
}

bool action::Action::IsChecked() const
{
    return m_checked;
}

void action::Action::SetEnabled(bool enabled)
{
    m_enabled = enabled;
}

void action::Action::SetChecked(bool checked)
{
    m_checked = checked;
}

win32_program::CommandId action::Action::GetCommandId() const
{
    return m_commandId;
}

int action::Action::GetToolbarImageIndex() const
{
    return m_actionDescription.imageIndex;
}

action::GroupId action::Action::GetGroupId() const
{
    return m_actionDescription.groupId;
}

action::MenuId action::Action::GetMenuId() const
{
    return m_actionDescription.menuId;
}
