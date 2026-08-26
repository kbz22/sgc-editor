#include "action/export_file_action.hpp"

action::ExportFileAction::ExportFileAction() 
{
    m_hMenu = CreatePopupMenu();
    m_menuId = static_cast<int>(action::ActionType::ExportFile);

    m_actionType = ActionType::ExportFile;    
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = -1;
    m_actionDescription.toolbarOrder = -1;
    m_actionDescription.menuOrder = 500;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::Export;
    m_actionDescription.menuId = MenuId::File;
    m_actionDescription.tooltipStringId = std::nullopt;
    m_actionDescription.nameStringId = locale::StringId::NameExportFile;
}