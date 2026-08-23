#pragma once

#include "sgc_view/tileset_view.hpp"
#include "sgc_view/map_view.hpp"

#include "sections/tileset_section.hpp"
#include "sections/map_section.hpp"
#include "sections/toolbar_section.hpp"
#include "sections/menu_section.hpp"
#include "sections/layers_section.hpp"
#include "sections/package_section.hpp"
#include "sections/status_section.hpp"

#include "locale/string_lookup.hpp"

#include "command/command_manager.hpp"
#include "file/file_manager.hpp"
#include "action/action_manager.hpp"

#include "win32_program/win32_context.hpp"
#include "win32_program/shortcut_manager.hpp"
#include "file/map_document.hpp"
#include "file/asset_manager.hpp"
#include "program/editor_mode.hpp"

#include <sgc/graphics/rectangle.hpp>
#include <commctrl.h>
#include <string>

namespace program {

    struct ProgramContext
    {
        std::unique_ptr<sections::TilesetSection> tilesetSection;
        std::unique_ptr<sections::MapSection> mapSection;
        std::unique_ptr<sections::ToolbarSection> toolbarSection;
        std::unique_ptr<sections::MenuSection> menuSection;
        std::unique_ptr<sections::LayersSection> layersSection;
        std::unique_ptr<sections::PackageSection> packageSection;
        std::unique_ptr<sections::StatusSection> statusSection;

        std::vector<sections::Section*> sections;
        sections::Section* activeSection = nullptr;

        std::unique_ptr<sgc::graphics::Rectangle> selectionRectangleOnTileset;
        
        std::wstring currentProjectName;        

        std::unique_ptr<win32_program::MainWindowContext> mainWindowContext;
        
        std::unique_ptr<file::FileManager> fileManager{};
        std::unique_ptr<action::ActionManager> actionManager{};
        std::unique_ptr<file::AssetManager> assetManager{};
        std::unique_ptr<win32_program::ShortcutManager> shortcutManager{};

        HIMAGELIST toolbarIcons;
        HIMAGELIST toolbarIconsDisabled;
        HIMAGELIST packageViewFileIcons;
        
        locale::StringLookup stringLookup{};

        program::EditorLayerMode editorLayerMode = program::EditorLayerMode::MultiLayer;
        program::EditorChunkMode editorChunkMode = program::EditorChunkMode::FixedChunks;
        program::EditorGridMode editorGridMode = program::EditorGridMode::NoGrid;
    };

    ProgramContext& GetProgramContext();

    void RegisterActions();
    void RegisterDefaultShortcuts(program::ProgramContext& programContext);
    void StartDefault();    
    
    void RefreshEditor();

}