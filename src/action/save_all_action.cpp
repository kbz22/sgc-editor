#include "action/save_all_action.hpp"
#include "program/program.hpp"
#include "program/except.hpp"
#include "win32_helpers/file_helpers.hpp"
#include "locale/stringid.hpp"
#include "file/ifile.hpp"

action::SaveAllAction::SaveAllAction()
{
    m_actionType = ActionType::SaveAll;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = 37;
    m_actionDescription.toolbarOrder = 320;
    m_actionDescription.menuOrder = 320;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::File;
    m_actionDescription.menuId = MenuId::File;
    m_actionDescription.tooltipStringId = std::nullopt;
    m_actionDescription.nameStringId = locale::StringId::NameSaveAll;
    m_actionDescription.shortcutContext = win32_program::ShortcutContext::Global;
}

void SaveAndUpdate(file::IFile* file, std::filesystem::path filePath, program::ProgramContext& programContext);
std::optional<std::filesystem::path> GetFilePathFromDialog(file::IFile* activeFile, const locale::StringLookup &stringLookup, HWND windowHwnd);

void action::SaveAllAction::Execute(program::ProgramContext& programContext)
{       
    static std::unordered_map<file::FileType, locale::StringId> extensionToStringIdMap = {
        { file::FileType::Map, locale::StringId::NameMapFile },
        { file::FileType::Tileset, locale::StringId::NameTilesetFile },
        { file::FileType::Package, locale::StringId::NamePackageFile }
    };

    auto fileManager = programContext.GetManager<file::FileManager>();
    auto stringLookup = programContext.GetStringLookup();
    auto files = fileManager->GetOpenFiles();

    for(auto &file : files)
    {
        if(file->IsDirty())
        {
            std::optional<std::filesystem::path> filePath = file->GetFilePath();

            if(filePath.value().empty()) 
            {
                filePath = GetFilePathFromDialog(
                    file,
                    programContext.GetStringLookup(),
                    programContext.GetMainWindowHandle()
                );
            }

            if(filePath.has_value())
            {
                SaveAndUpdate(file, filePath.value(), programContext);
            }
        }
    }
}
