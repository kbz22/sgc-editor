#include "action/save_file_action.hpp"
#include "program/program.hpp"
#include "program/except.hpp"
#include "win32_helpers/file_helpers.hpp"
#include "locale/stringid.hpp"
#include "defaults.hpp"
#include <unordered_map>

action::SaveFileAction::SaveFileAction()
{
    m_actionType = ActionType::SaveFile;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = 2;
    m_actionDescription.toolbarOrder = 300;
    m_actionDescription.menuOrder = 300;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::File;
    m_actionDescription.menuId = MenuId::File;
    m_actionDescription.tooltipStringId = locale::StringId::TooltipFileSave;
    m_actionDescription.nameStringId = locale::StringId::NameSaveFile;
    m_actionDescription.shortcutContext = win32_program::ShortcutContext::Global;
    m_actionDescription.shortcuts = {
        {win32_program::ShortcutModifier::Ctrl, 'S'}
    };
}

void SaveAndUpdate(file::IFile* activeFile, std::filesystem::path filePath, program::ProgramContext& programContext);

void action::SaveFileAction::Execute(program::ProgramContext& programContext)
{       
    static std::unordered_map<file::FileType, locale::StringId> extensionToStringIdMap = {
        { file::FileType::Map, locale::StringId::NameMapFile },
        { file::FileType::Tileset, locale::StringId::NameTilesetFile },
        { file::FileType::Package, locale::StringId::NamePackageFile }
    };

    auto activeFile = programContext.fileManager->GetSelectedFile();
    
    if (activeFile != nullptr) {

        if(!activeFile->IsDirty()) {
            return;
        }
        
        std::optional<std::filesystem::path> filePath = activeFile->GetFilePath();

        if(filePath.has_value() && filePath.value().empty()) 
        {
            programContext.actionManager->Execute(
                action::ActionType::SaveAs, programContext
            );
        }
        else if(filePath.has_value())
        {
            SaveAndUpdate(activeFile, filePath.value(), programContext);
        }
    }
}
