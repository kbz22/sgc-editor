#include "action/close_file_action.hpp"
#include "program/program.hpp"

action::CloseFileAction::CloseFileAction()
{
    m_actionType = ActionType::CloseFile;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = -1;
    m_actionDescription.toolbarOrder = -1;
    m_actionDescription.menuOrder = 400;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::File;
    m_actionDescription.menuId = MenuId::File;
    m_actionDescription.tooltipStringId = std::nullopt;
    m_actionDescription.nameStringId = locale::StringId::NameCloseFile;
}

void action::CloseFileAction::Execute(program::ProgramContext& context)
{
    auto fileManager = context.GetManager<file::FileManager>();
    auto currentFile = fileManager->GetSelectedFile();

    if(currentFile != nullptr) {
        fileManager->CloseFile(fileManager->GetSelectedFileIndex());
    }
}