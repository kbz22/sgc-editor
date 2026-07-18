#include "action/action.hpp"
#include "locale/command_lookup.hpp"
#include "locale/string_lookup.hpp"

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

bool action::Action::IsSeperator() const
{
    return m_actionType == ActionType::Seperator;
}

bool action::Action::IsGrouped() const
{
    return m_grouped;
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
