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

#include "win32_program/shortcut_manager.hpp"
#include "settings/settings_manager.hpp"
#include "file/map_document.hpp"
#include "file/asset_manager.hpp"
#include "program/editor_mode.hpp"

#include <sgc/graphics/rectangle.hpp>
#include <commctrl.h>
#include <string>

namespace program {

    enum class ImageListType
    {
        Toolbar,
        ToolbarDisabled,
        ListView
    };

    class ProgramContext    
    {
        private:
            std::unique_ptr<sections::TilesetSection> m_tilesetSection;
            std::unique_ptr<sections::MapSection> m_mapSection;
            std::unique_ptr<sections::ToolbarSection> m_toolbarSection;
            std::unique_ptr<sections::MenuSection> m_menuSection;
            std::unique_ptr<sections::LayersSection> m_layersSection;
            std::unique_ptr<sections::PackageSection> m_packageSection;
            std::unique_ptr<sections::StatusSection> m_statusSection;

            std::vector<sections::Section*> m_sections;
            sections::Section* m_activeSection = nullptr;

            std::unique_ptr<sgc::graphics::Rectangle> m_selectionRectangleOnTileset;
        
            std::unique_ptr<file::FileManager> m_fileManager{};
            std::unique_ptr<action::ActionManager> m_actionManager{};
            std::unique_ptr<file::AssetManager> m_assetManager{};
            std::unique_ptr<win32_program::ShortcutManager> m_shortcutManager{};
            std::unique_ptr<settings::SettingsManager> m_settingsManager{};            

            static bool m_win32Set;
            static HINSTANCE m_hInstance;
            static HWND m_mainWindowHandle;

            std::unordered_map<ImageListType, HIMAGELIST> m_imageLists;
        
            std::unique_ptr<locale::StringLookup> m_stringLookup{};

            program::EditorLayerMode m_editorLayerMode = program::EditorLayerMode::MultiLayer;
            program::EditorChunkMode m_editorChunkMode = program::EditorChunkMode::FixedChunks;
            program::EditorGridMode m_editorGridMode = program::EditorGridMode::NoGrid;

            void RegisterActions();
            void RegisterDefaultSettings();
            void RegisterDefaultShortcuts();            
            void SetupImageLists();

        public:
            ProgramContext();
            ProgramContext(HWND hMainWindow, HINSTANCE hInstance);

            void StartDefault();

            void Refresh();
            void UpdateEditorLayerMode(program::EditorLayerMode newMode);
            void UpdateEditorChunkMode(program::EditorChunkMode newMode);
            void UpdateBrushMode(editor_tools::PaintMode newMode);
            void UpdateBrushEraseMode(editor_tools::EraserMode newMode);
            void UpdateEditorGridMode(program::EditorGridMode newMode);
            void UpdateEditorSelectionMode(editor_tools::SelectionMode newMode);
            void UpdateEditorSelectionTools();

            void EnableSaving(bool enable);
            
            bool ShortcutsEnabled() const;

            std::vector<sections::Section*> GetSections() const;
            sections::Section* GetActiveSection() const;
            void SetActiveSection(sections::Section* section);
            template<typename T>
            T* GetSection() const;
            template<typename T>
            T* GetManager() const;

            locale::StringLookup& GetStringLookup() const;
            
            HIMAGELIST GetImageList(ImageListType type) const;
            void SetImageList(ImageListType type, HIMAGELIST imageList);

            sgc::graphics::Rectangle& GetSelectionRectangleOnTileset() const;

            program::EditorGridMode GetEditorGridMode() const;
            program::EditorLayerMode GetEditorLayerMode() const;
            program::EditorChunkMode GetEditorChunkMode() const;

            void SetEditorChunkMode(program::EditorChunkMode newMode);
            void SetEditorLayerMode(program::EditorLayerMode newMode);
            void SetEditorGridMode(program::EditorGridMode newMode);

            HWND GetMainWindowHandle() const;
            HINSTANCE GetHInstance() const;
    };

    ProgramContext& GetProgramContext();
    

}