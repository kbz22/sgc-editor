#include "action/open_file_action.hpp"

action::OpenFileAction::OpenFileAction()
{
    m_actionType = ActionType::OpenFile;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = 1;
    m_actionDescription.toolbarOrder = 200;
    m_actionDescription.menuOrder = 200;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::File;
    m_actionDescription.menuId = MenuId::File;
    m_actionDescription.tooltipStringId = locale::StringId::TooltipFileOpen;
    m_actionDescription.nameStringId = locale::StringId::NameOpenFile;
}

void action::OpenFileAction::Execute(program::ProgramContext& programContext)
{
    /* auto filePath = win32_helpers::ShowOpenDialog(
        programContext.mainWindowContext->hMainWindow,
        {
            { L"Map Files", defaults::MapFileExtension.data() },
            { L"Tileset Files", defaults::TilesetFileExtension.data() },
            { L"All Files", L"*.*" }
        }
    );

    if(filePath.has_value()) {
        programContext.fileManager->OpenFile(filePath.value());
    } */
   return;
}