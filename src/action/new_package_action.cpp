#include "action/new_package_action.hpp"
#include "program/program.hpp"

action::NewPackageAction::NewPackageAction()
{
    m_actionType = ActionType::NewPackage;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = -1;
    m_actionDescription.toolbarOrder = -1;
    m_actionDescription.menuOrder = 130;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::File;
    m_actionDescription.menuId = MenuId::None;
    m_actionDescription.tooltipStringId = std::nullopt;
    m_actionDescription.nameStringId = locale::StringId::NameNewPackage;
}

void action::NewPackageAction::Execute(program::ProgramContext &programContext)
{
    programContext.fileManager->NewPackageFile(
        L"New Package",
        {},
        {}
    );

    program::RefreshEditor();
}