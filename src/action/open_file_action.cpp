#include "action/open_file_action.hpp"
#include "win32_helpers/file_helpers.hpp"
#include "program/program.hpp"
#include "defaults.hpp"

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
    auto allFilesString = programContext.stringLookup.Get(locale::StringId::NameAllFiles);
    auto mapFilesString = programContext.stringLookup.Get(locale::StringId::NameMapFile);
    auto tilesetFilesString = programContext.stringLookup.Get(locale::StringId::NameTilesetFile);
    auto packageFilesString = programContext.stringLookup.Get(locale::StringId::NamePackageFile);

    std::vector<win32_helpers::FileFilter> filters = {
        { mapFilesString.value_or(L"Map Files").c_str(), { defaults::MapFileExtension.data() } },
        { tilesetFilesString.value_or(L"Tileset Files").c_str(), { defaults::TilesetFileExtension.data() } },
        { packageFilesString.value_or(L"Package Files").c_str(), { defaults::PackageFileExtension.data() } },
        { allFilesString.value_or(L"All Files").c_str(), { L"*.*" } }
    };

    auto filePath = win32_helpers::ShowOpenDialog(
        programContext.mainWindowContext->hMainWindow,
        filters
    );

    if(filePath.has_value()) {
        programContext.fileManager->OpenFile(filePath.value(), programContext.assetManager.get());
        // program::RefreshEditor();
        // refresh is now handled by the file manager's on file updated callback
    }
   return;
}