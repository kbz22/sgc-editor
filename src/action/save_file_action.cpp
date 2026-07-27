#include "action/save_file_action.hpp"
#include "program/program.hpp"
#include "program/except.hpp"
#include "win32_helpers/file_helpers.hpp"
#include "locale/stringid.hpp"
#include "defaults.hpp"

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
}

void action::SaveFileAction::Execute(program::ProgramContext& programContext)
{       
    static std::unordered_map<wchar_t, locale::StringId> extensionToStringIdMap = {
        { L't', locale::StringId::NameTilesetFile },
        { L'm', locale::StringId::NameMapFile },
        { L'p', locale::StringId::NamePackageFile }
    };

    auto activeFile = programContext.fileManager->GetSelectedFile();
    
    if (activeFile != nullptr) {

        if(!activeFile->IsDirty()) {
            return;
        }
        
        std::optional<std::filesystem::path> filePath = activeFile->GetFilePath();

        if(filePath.has_value() && filePath.value().empty()) {

            auto allFilesString = programContext.stringLookup.Get(locale::StringId::NameAllFiles);
            auto thisTypeString = programContext.stringLookup.Get(
                extensionToStringIdMap[activeFile->GetExtension().at(4)]
            );

            if(!thisTypeString.has_value() || !allFilesString.has_value()) {
                throw program::StringNotFoundException("Missing string for file type or all files");
            }

            std::wstring thisTypeStr = std::format(
                L"{} ({})",
                thisTypeString.value(),
                activeFile->GetExtension()
            );

            filePath = win32_helpers::ShowSaveDialog(
                programContext.mainWindowContext->hMainWindow,
                {
                    { thisTypeStr.c_str(), { activeFile->GetExtension().c_str() } },
                    { allFilesString.value().c_str(), { L"*" } }
                }
            );
        }

        if(filePath.has_value())
        {
            activeFile->SetFilePath(filePath.value());

            programContext.fileManager->SaveFile(
                programContext.fileManager->GetSelectedFileIndex()
            );
        }

    }
}
