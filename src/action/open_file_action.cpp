#include "action/open_file_action.hpp"
#include "win32_helpers/file_helpers.hpp"
#include "program/program.hpp"
#include "settings/default_open_filetype_setting.hpp"
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
    auto stringLookup = programContext.GetStringLookup();
    auto allFilesString = stringLookup.Get(locale::StringId::NameAllFiles);
    auto mapFilesString = stringLookup.Get(locale::StringId::NameMapFile);
    auto tilesetFilesString = stringLookup.Get(locale::StringId::NameTilesetFile);
    auto packageFilesString = stringLookup.Get(locale::StringId::NamePackageFile);

    auto indexSetting = programContext.GetManager<settings::SettingsManager>()->GetSetting<settings::DefaultOpenFiletypeSetting>();

    std::vector<win32_helpers::FileFilter> filters = {
        { mapFilesString.value_or(L"Map Files").c_str(), { defaults::MapFileExtension.data() } },
        { tilesetFilesString.value_or(L"Tileset Files").c_str(), { defaults::TilesetFileExtension.data() } },
        { packageFilesString.value_or(L"Package Files").c_str(), { defaults::PackageFileExtension.data() } },
        { allFilesString.value_or(L"All Files").c_str(), { L"*.*" } }
    };

    auto hMainWindow = programContext.GetMainWindowHandle();
    auto filePath = win32_helpers::ShowOpenDialog(
        hMainWindow,
        filters,
        static_cast<int>(indexSetting->GetValue()) + 1 // one indexed
    );

    auto fileManager = programContext.GetManager<file::FileManager>();
    auto assetManager = programContext.GetManager<file::AssetManager>();
    if(filePath.has_value()) {
        fileManager->OpenFile(filePath.value(), assetManager);
        // program::RefreshEditor();
        // refresh is now handled by the file manager's on file updated callback
    }
   return;
}