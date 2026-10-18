#include "action/help_repository_action.hpp"
#include "program/program.hpp"
#include "program/except.hpp"
#include <windows.h>
#include <shellapi.h>

action::HelpRepositoryAction::HelpRepositoryAction()
{
    m_actionType = ActionType::HelpGithubHyperlink;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = -1;
    m_actionDescription.toolbarOrder = -1;
    m_actionDescription.menuOrder = 200;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::Default;
    m_actionDescription.menuId = MenuId::Help;
    m_actionDescription.tooltipStringId = std::nullopt;
    m_actionDescription.nameStringId = locale::StringId::NameHelpGithub;
    m_actionDescription.shortcutStringId = std::nullopt;
}

bool action::HelpRepositoryAction::IsShortcutable() const
{
    return false;
}

void action::HelpRepositoryAction::Execute(program::ProgramContext& programContext)
{
    constexpr wchar_t repositoryUrl[] = L"https://github.com/kbz22/sgc-editor";

    auto hMainWindow = programContext.GetMainWindowHandle();
    auto result = reinterpret_cast<INT_PTR>(
        ShellExecuteW(
            hMainWindow,
            L"open",
            repositoryUrl,
            nullptr,
            nullptr,
            SW_SHOWNORMAL
        )
    );

    if (result <= 32)
    {
        throw program::OpenRepositoryFailure("HelpRepositoryAction could not open the repository");
    }
}