#include "win32_program/windows_init.hpp"
#undef CreateWindow // avoid macro name conflict with sdl::CreateWindow
#include "win32_program/windows_controls.hpp"
#include "win32_program/layout_manager.hpp"
#include "program/program.hpp"
#include "program/except.hpp"
#include "program/layer_manager.hpp"
#include "program/editor_update.hpp"
#include <sgc/data/chunkedtilestorage.hpp>

#include "action/new_document_action.hpp"
#include "action/new_map_document_action.hpp"
#include "action/new_tileset_document_action.hpp"
#include "action/undo_action.hpp"
#include "action/redo_action.hpp"
#include "action/layer_add_action.hpp"
#include "action/layer_remove_action.hpp"
#include "action/layer_move_action.hpp"
#include "action/change_layer_mode_action.hpp"
#include "action/change_chunk_mode_action.hpp"
#include "action/file_menu_action.hpp"
#include "action/edit_menu_action.hpp"
#include "action/map_menu_action.hpp"
#include "action/view_menu_action.hpp"
#include "action/help_menu_action.hpp"
#include "action/close_file_action.hpp"
#include "action/save_file_action.hpp"
#include "action/open_file_action.hpp"
#include "action/change_brush_mode_action.hpp"
#include "action/zoom_action.hpp"
#include "action/zoom_reset_action.hpp"
#include "action/change_brush_erase_mode_action.hpp"
#include "action/zoom_select_action.hpp"
#include "action/change_grid_mode_action.hpp"
#include "action/change_selection_mode_action.hpp"
#include "action/selection_clear_action.hpp"
#include "action/selection_copy_action.hpp"
#include "action/selection_paste_action.hpp"
#include "action/selection_cut_action.hpp"
#include "action/selection_move_action.hpp"
#include "action/save_as_action.hpp"
#include "action/export_file_action.hpp"
#include "action/export_image_action.hpp"
#include "action/new_package_action.hpp"
#include "action/change_active_layer_action.hpp"
#include "action/settings_action.hpp"
#include "defaults.hpp"

#include "settings/settings.hpp"

#include "editor_graphics.h"
#include "win32_helpers/load_bitmap.hpp"

bool program::ProgramContext::m_win32Set = false;
auto program::ProgramContext::m_hInstance = HINSTANCE();
auto program::ProgramContext::m_mainWindowHandle = HWND();

program::ProgramContext::ProgramContext(HWND hMainWindow, HINSTANCE hInstance)
{
    if(m_win32Set) return;

    m_hInstance = hInstance;
    m_mainWindowHandle = hMainWindow;
    m_stringLookup = std::make_unique<locale::StringLookup>(); // for default text
    m_win32Set = true;
}

program::ProgramContext::ProgramContext()
{
    m_actionManager = std::make_unique<action::ActionManager>();
    m_shortcutManager = std::make_unique<win32_program::ShortcutManager>(
        m_hInstance,
        m_mainWindowHandle
    );
    m_stringLookup = std::make_unique<locale::StringLookup>();

    SetupImageLists();
    RegisterActions();
    RegisterDefaultShortcuts();    
}

program::ProgramContext& program::GetProgramContext()
{
    static ProgramContext context = {};
    return context;
}

std::vector<action::ActionType> g_activeEditorButtons {    
    action::ActionType::CloseFile,
    action::ActionType::Undo,
    action::ActionType::Redo,
    action::ActionType::AddLayer,
    action::ActionType::RemoveLayer,
    action::ActionType::MoveLayerUp,
    action::ActionType::MoveLayerDown,
    action::ActionType::LayerModeMultilayer,
    action::ActionType::LayerModeSingleLayer,
    action::ActionType::LayerModeSingleImage,
    action::ActionType::ChunkModeFixedSize,
    action::ActionType::ChunkModeFree,
    action::ActionType::PaintModeBrush,
    action::ActionType::PaintModeRectangle,
    action::ActionType::PaintModeFill,
    action::ActionType::PaintModeSelect,    
    action::ActionType::SelectZoom,
    action::ActionType::ResetZoom,
    action::ActionType::ZoomIn,
    action::ActionType::ZoomOut,
    action::ActionType::EraseModeClearTile,
    action::ActionType::EraseModeDeleteChunk,
    action::ActionType::GridModeTile,
    action::ActionType::GridModeChunk,
    action::ActionType::SelectSingleLayerMode,
    action::ActionType::SelectAllLayersMode,
    action::ActionType::SelectVisibleLayersMode,
    action::ActionType::SelectionClear,
    action::ActionType::SelectionCopy,
    action::ActionType::SelectionPaste,
    action::ActionType::SelectionCut,
    action::ActionType::SelectionMove,
    action::ActionType::ExportAsImage,
    action::ActionType::ExportFile    
};

void program::ProgramContext::RegisterDefaultSettings()
{
    m_settingsManager->RegisterSetting<settings::ShortcutsSetting>(
        static_cast<unsigned>(settings::Key::ShortcutsSetting),
        std::make_unique<settings::ShortcutsSetting>(*m_shortcutManager)
    );
    m_settingsManager->RegisterSetting<settings::AutoRestoreFilesSetting>(
        static_cast<unsigned>(settings::Key::AutoRestoreFilesSetting),
        std::make_unique<settings::AutoRestoreFilesSetting>(false)
    );
    m_settingsManager->RegisterSetting<settings::DefaultOpenFiletypeSetting>(
        static_cast<unsigned>(settings::Key::DefaultOpenFiletypeSetting),
        std::make_unique<settings::DefaultOpenFiletypeSetting>(file::FileType::Map)
    );
}

void program::ProgramContext::StartDefault()
{
    m_assetManager = std::make_unique<file::AssetManager>();
    m_fileManager = std::make_unique<file::FileManager>();
    m_settingsManager = std::make_unique<settings::SettingsManager>();
    m_stringLookup = std::make_unique<locale::StringLookup>();
    
    RegisterDefaultSettings();
    m_settingsManager->LoadValuesFromPreferences();    

    m_actionManager->ActionSetEnabled(g_activeEditorButtons, false);

    m_menuSection = std::make_unique<sections::MenuSection>(*this);
    m_sections.push_back(m_menuSection.get());
    m_menuSection->Refresh(*this);

    m_mapSection = std::make_unique<sections::MapSection>(*this);
    m_sections.push_back(m_mapSection.get());    

    m_toolbarSection = std::make_unique<sections::ToolbarSection>(*this);
    m_sections.push_back(m_toolbarSection.get());

    m_tilesetSection = std::make_unique<sections::TilesetSection>(*this);
    m_sections.push_back(m_tilesetSection.get());    

    m_layersSection = std::make_unique<sections::LayersSection>(*this);
    m_sections.push_back(m_layersSection.get());

    m_packageSection = std::make_unique<sections::PackageSection>(*this);
    m_sections.push_back(m_packageSection.get());

    m_statusSection = std::make_unique<sections::StatusSection>(*this);
    m_sections.push_back(m_statusSection.get());

    m_layersSection->RegisterSelectedLayerChangeCallback([this](size_t index) {
        auto document = m_fileManager->GetActiveDocument();
        if(document != nullptr) {
            auto layerManager = document->GetLayerManager();
            layerManager->SetActiveLayerIndex(index);

            if(m_editorLayerMode == EditorLayerMode::MultiLayer) {
                MultiLayerModeSetup(*this);
            }

            m_mapSection->Refresh(*this);
            m_mapSection->Update();
        }
    });

    m_layersSection->RegisterLayerVisibilityChangeCallback([this](size_t index, bool visible) {
        auto document = m_fileManager->GetActiveDocument();
        if(document != nullptr) {
            auto layerManager = document->GetLayerManager();
            layerManager->SetLayerVisibility(index, visible);

            m_mapSection->Refresh(*this);
            m_mapSection->Update();
        }
    });

    m_layersSection->RegisterLayerNameChangeCallback([this](size_t index, std::wstring newName) {
        auto document = m_fileManager->GetActiveDocument();
        if(document != nullptr) {
            auto layerManager = document->GetLayerManager();
            layerManager->SetLayerName(index, newName);
            document->SetDirty(true);

            m_mapSection->Refresh(*this);
            m_mapSection->Update();

            m_packageSection->UpdateTreeViewItems(*this);
        }
    });

    m_packageSection->RegisterFileActionCallback(sections::FileAction::ItemSelected, [this](file::IFile* file, size_t index) 
    {
        auto fileManager = m_fileManager.get();
        auto location = file::DocumentLocation{file, index};
        
        fileManager->SelectDocument(location);

        m_packageSection->UpdateTreeViewItems(*this);
        m_packageSection->Update();
    });

    m_packageSection->RegisterFileActionCallback(sections::FileAction::ItemDoubleClicked, [this](file::IFile* file, size_t index) 
    {
        auto fileManager = m_fileManager.get();
        auto location = file::DocumentLocation{file, index};
        
        if(index == 0 && !file->IsActivable())
        {
            return;
        }
        
        // I'm making this the Package's responsibility
        /* if(file->IsContainer())
        {
            location.index -= 1;
        } */

        fileManager->SetActiveDocument(location);

        m_mapSection->Refresh(*this);
        m_mapSection->Update();

        m_layersSection->Refresh(*this);
        m_layersSection->Update();

        m_tilesetSection->Refresh(*this);
        m_tilesetSection->Update();

        m_packageSection->UpdateTreeViewItems(*this);
        m_packageSection->Update();
    });

    m_fileManager->RegisterOnFileUpdatedCallback([this]([[maybe_unused]] file::IFile* file) 
    {
        // m_packageSection->UpdateTreeViewItems(*this);
        Refresh();
        m_packageSection->UpdateTreeViewItems(*this);
        m_packageSection->Update();
    });

    m_fileManager->RegisterOnActiveDocumentChangedCallback([this](file::DocumentLocation documentLocation) 
    {
        static const auto defaultWindowTitle = m_stringLookup->Get(locale::StringId::WindowTitle);        

        if(documentLocation.file == nullptr) 
        {
            win32_program::SetTitle(
                m_mainWindowHandle,
                defaultWindowTitle.value().c_str()
            );
        }
        else
        {
            auto fileName = documentLocation.file->GetFileName();
            auto newTitle = std::format(L"{} - {}", fileName, defaultWindowTitle.value());

            win32_program::SetTitle(
                m_mainWindowHandle,
                newTitle
            );
        }

        Refresh();
    });    

    m_fileManager->RegisterOnMapDirtyCallback([this]([[maybe_unused]]file::MapDocument* document, [[maybe_unused]]bool dirty) 
    {
        // EnableSaving(dirty);
        auto activeDocument = m_fileManager->GetActiveDocument();
        if(activeDocument != nullptr) 
        {
            if(activeDocument->IsDirty()) {
                EnableSaving(true);
            }
            else {
                EnableSaving(false);
            }
            
            m_packageSection->UpdateTreeViewItems(*this);
            m_packageSection->Update();
            m_toolbarSection->Refresh(*this);
            m_toolbarSection->Update();
        }        
    });

    if(m_settingsManager->GetSetting<settings::AutoRestoreFilesSetting>()->GetValue())
    {
        try
        {
            m_fileManager->RestoreLastSession(m_assetManager.get());
        }
        catch(const SessionRestoreException&)
        {
            auto errorName = m_stringLookup->Get(locale::StringId::ErrorName).value_or(L"ERROR NAME");
            auto errorMssg = m_stringLookup->Get(locale::StringId::EditorErrorFailedToRestoreSession).value_or(L"RESTORE FAILED MESSAGE");
            MessageBox(m_mainWindowHandle, errorMssg.c_str(), errorName.c_str(), MB_OK | MB_ICONERROR);
        }
    }

    Refresh();
}

void RedrawAllSections(program::ProgramContext& programContext)
{
    for(auto section : programContext.GetSections()) {
        section->HandleSectionResize();
        section->Update();
    }
}

void RefreshAllSection(program::ProgramContext& programContext)
{
    for(auto section : programContext.GetSections()) {
        section->Refresh(programContext);        
    }
}

void program::ProgramContext::RegisterDefaultShortcuts()
{
    auto& shortcutManager = m_shortcutManager;
    auto& actionManager = m_actionManager;

    for(auto &action : actionManager->GetActions()) {
        auto shortcuts = action->GetDefaultShortcuts();
        for(auto& shortcut : shortcuts) {
            shortcutManager->RegisterShortcut(
                shortcut,
                action->GetShortcutContext(),
                action->GetType()
            );
        }
    }
}

void program::ProgramContext::Refresh()
{
    auto currentDocument = m_fileManager->GetActiveDocument();

    if(currentDocument == nullptr || !currentDocument->IsEditable()) {
        m_actionManager->ActionSetChecked(g_activeEditorButtons, false);
        m_actionManager->ActionSetEnabled(g_activeEditorButtons, false);
    }
    else if(currentDocument->IsEditable()) {
        m_actionManager->ActionSetEnabled(g_activeEditorButtons, true);
        UpdateEditorLayerMode(m_editorLayerMode);
        UpdateEditorChunkMode(m_editorChunkMode);
        UpdateBrushMode(m_mapSection->GetPaintMode());
        UpdateEditorSelectionMode(m_mapSection->GetSelectionMode());
        UpdateEditorSelectionTools();
    }

    auto openFiles = m_fileManager->GetOpenFiles();

    EnableSaving(false);

    for(auto &file : openFiles) 
    {
        if(file->IsDirty()) {
            EnableSaving(true);
            break;
        }
    }

    RefreshAllSection(*this);
    RedrawAllSections(*this);

    return;
}

void program::ProgramContext::RegisterActions()
{
    if(m_actionManager == nullptr) {
        m_actionManager = std::make_unique<action::ActionManager>();
    }

    auto newDocAction = m_actionManager->Register(std::make_unique<action::NewDocumentAction>());
    auto newMapAction = m_actionManager->Register(std::make_unique<action::NewMapDocumentAction>());
    auto newTilesetAction = m_actionManager->Register(std::make_unique<action::NewTilesetDocumentAction>());
    auto newPackageAction = m_actionManager->Register(std::make_unique<action::NewPackageAction>());
    m_actionManager->Register(std::make_unique<action::OpenFileAction>());
    m_actionManager->Register(std::make_unique<action::SaveFileAction>());
    m_actionManager->Register(std::make_unique<action::CloseFileAction>());
    m_actionManager->Register(std::make_unique<action::UndoAction>());
    m_actionManager->Register(std::make_unique<action::RedoAction>());
    m_actionManager->Register(std::make_unique<action::LayerAddAction>());
    m_actionManager->Register(std::make_unique<action::LayerRemoveAction>());
    m_actionManager->Register(std::make_unique<action::LayerMoveAction>(-1));
    m_actionManager->Register(std::make_unique<action::LayerMoveAction>(1));
    m_actionManager->Register(std::make_unique<action::ChangeLayerModeAction>(EditorLayerMode::MultiLayer));
    m_actionManager->Register(std::make_unique<action::ChangeLayerModeAction>(EditorLayerMode::SingleLayer));
    m_actionManager->Register(std::make_unique<action::ChangeLayerModeAction>(EditorLayerMode::SingleImage));
    m_actionManager->Register(std::make_unique<action::ChangeChunkModeAction>(EditorChunkMode::FixedChunks));
    m_actionManager->Register(std::make_unique<action::ChangeChunkModeAction>(EditorChunkMode::DynamicChunks));
    m_actionManager->Register(std::make_unique<action::ChangeBrushModeAction>(editor_tools::PaintMode::Brush));
    m_actionManager->Register(std::make_unique<action::ChangeBrushModeAction>(editor_tools::PaintMode::Rectangle));
    m_actionManager->Register(std::make_unique<action::ChangeBrushModeAction>(editor_tools::PaintMode::Fill));
    m_actionManager->Register(std::make_unique<action::ChangeBrushModeAction>(editor_tools::PaintMode::Select));
    m_actionManager->Register(std::make_unique<action::ZoomResetAction>());
    m_actionManager->Register(std::make_unique<action::ZoomAction>(defaults::zoomFactor));
    m_actionManager->Register(std::make_unique<action::ZoomAction>(1.0f / defaults::zoomFactor));
    m_actionManager->Register(std::make_unique<action::ChangeBrushEraseModeAction>(editor_tools::EraserMode::ClearTile));
    m_actionManager->Register(std::make_unique<action::ChangeBrushEraseModeAction>(editor_tools::EraserMode::DeleteChunk));
    m_actionManager->Register(std::make_unique<action::ZoomSelectAction>());
    m_actionManager->Register(std::make_unique<action::ChangeGridModeAction>(EditorGridMode::TileGrid));
    m_actionManager->Register(std::make_unique<action::ChangeGridModeAction>(EditorGridMode::ChunkGrid));
    m_actionManager->Register(std::make_unique<action::ChangeSelectionModeAction>(editor_tools::SelectionMode::SingleLayer));
    m_actionManager->Register(std::make_unique<action::ChangeSelectionModeAction>(editor_tools::SelectionMode::AllLayers));
    m_actionManager->Register(std::make_unique<action::ChangeSelectionModeAction>(editor_tools::SelectionMode::VisibleLayers));
    m_actionManager->Register(std::make_unique<action::SelectionClearAction>());
    m_actionManager->Register(std::make_unique<action::SelectionCopyAction>());
    m_actionManager->Register(std::make_unique<action::SelectionPasteAction>());
    m_actionManager->Register(std::make_unique<action::SelectionCutAction>());
    m_actionManager->Register(std::make_unique<action::SelectionMoveAction>());    
    m_actionManager->Register(std::make_unique<action::SaveAsAction>());
    auto exportAction = m_actionManager->Register(std::make_unique<action::ExportFileAction>());
    auto exportImageAction = m_actionManager->Register(std::make_unique<action::ExportImageAction>());
    m_actionManager->Register(std::make_unique<action::ChangeActiveLayerAction>(action::ChangeActiveLayerDirection::Up));
    m_actionManager->Register(std::make_unique<action::ChangeActiveLayerAction>(action::ChangeActiveLayerDirection::Down));
    m_actionManager->Register(std::make_unique<action::ChangeActiveLayerAction>(action::ChangeActiveLayerDirection::Top));
    m_actionManager->Register(std::make_unique<action::ChangeActiveLayerAction>(action::ChangeActiveLayerDirection::Bottom));
    m_actionManager->Register(std::make_unique<action::SettingsAction>());

    reinterpret_cast<action::NewDocumentAction*>(newDocAction)->SetItems({
        newMapAction,
        newTilesetAction,
        newPackageAction
    });

    reinterpret_cast<action::ExportFileAction*>(exportAction)->SetItems({
        exportImageAction
    });

    m_actionManager->Register(std::make_unique<action::FileMenuAction>());
    m_actionManager->Register(std::make_unique<action::EditMenuAction>());
    m_actionManager->Register(std::make_unique<action::MapMenuAction>());
    m_actionManager->Register(std::make_unique<action::ViewMenuAction>());
    m_actionManager->Register(std::make_unique<action::HelpMenuAction>());
}

void program::ProgramContext::EnableSaving(bool enable)
{
    std::vector<action::ActionType> saveActions = {
        action::ActionType::SaveFile,
        action::ActionType::SaveAs
    };
    m_actionManager->ActionSetEnabled(saveActions, enable);
}

bool program::ProgramContext::ShortcutsEnabled() const
{
    if(std::find(m_sections.begin(), m_sections.end(), m_activeSection) != m_sections.end())
    {
        return true;
    }
    
    return false;
}

std::vector<sections::Section*> program::ProgramContext::GetSections() const
{
    return m_sections;
}

sections::Section* program::ProgramContext::GetActiveSection() const
{
    return m_activeSection;
}

void program::ProgramContext::SetActiveSection(sections::Section* section)
{
    m_activeSection = section;
}

template<>
sections::MapSection* program::ProgramContext::GetSection<sections::MapSection>() const
{
    return m_mapSection.get();
}

template<>
sections::TilesetSection* program::ProgramContext::GetSection<sections::TilesetSection>() const
{
    return m_tilesetSection.get();
}

template<>
sections::ToolbarSection* program::ProgramContext::GetSection<sections::ToolbarSection>() const
{
    return m_toolbarSection.get();
}

template<>
sections::MenuSection* program::ProgramContext::GetSection<sections::MenuSection>() const
{
    return m_menuSection.get();
}

template<>
sections::LayersSection* program::ProgramContext::GetSection<sections::LayersSection>() const
{
    return m_layersSection.get();
}

template<>
sections::PackageSection* program::ProgramContext::GetSection<sections::PackageSection>() const
{
    return m_packageSection.get();
}

template<>
sections::StatusSection* program::ProgramContext::GetSection<sections::StatusSection>() const
{
    return m_statusSection.get();
}

template<>
action::ActionManager* program::ProgramContext::GetManager<action::ActionManager>() const
{
    return m_actionManager.get();
}

template<>
file::FileManager* program::ProgramContext::GetManager<file::FileManager>() const
{
    return m_fileManager.get();
}

template<>
file::AssetManager* program::ProgramContext::GetManager<file::AssetManager>() const
{
    return m_assetManager.get();
}

template<>
win32_program::ShortcutManager* program::ProgramContext::GetManager<win32_program::ShortcutManager>() const
{
    return m_shortcutManager.get();
}

template<>
settings::SettingsManager* program::ProgramContext::GetManager<settings::SettingsManager>() const
{
    return m_settingsManager.get();
}

HWND program::ProgramContext::GetMainWindowHandle() const
{
    if(!m_win32Set)
        return HWND();

    return m_mainWindowHandle;
}

HINSTANCE program::ProgramContext::GetHInstance() const
{
    if(!m_win32Set)
        return HINSTANCE();

    return m_hInstance;
}

HIMAGELIST program::ProgramContext::GetImageList(ImageListType type) const
{
    return m_imageLists.at(type);
}

void program::ProgramContext::SetImageList(ImageListType type, HIMAGELIST imageList)
{
    m_imageLists[type] = imageList;
}

sgc::graphics::Rectangle& program::ProgramContext::GetSelectionRectangleOnTileset() const
{
    return *m_selectionRectangleOnTileset;
}

locale::StringLookup& program::ProgramContext::GetStringLookup() const
{
    return *m_stringLookup;
}

program::EditorGridMode program::ProgramContext::GetEditorGridMode() const
{
    return m_editorGridMode;
}

program::EditorLayerMode program::ProgramContext::GetEditorLayerMode() const
{
    return m_editorLayerMode;
}

program::EditorChunkMode program::ProgramContext::GetEditorChunkMode() const
{
    return m_editorChunkMode;
}

void program::ProgramContext::SetEditorChunkMode(program::EditorChunkMode newMode)
{
    m_editorChunkMode = newMode;
}

void program::ProgramContext::SetEditorLayerMode(program::EditorLayerMode newMode)
{
    m_editorLayerMode = newMode;
}

void program::ProgramContext::SetEditorGridMode(program::EditorGridMode newMode)
{
    m_editorGridMode = newMode;
}

void program::ProgramContext::SetupImageLists()
{
    m_imageLists[ImageListType::Toolbar] = ImageList_Create(24, 24, ILC_COLOR32, 10, 0);
    m_imageLists[ImageListType::ToolbarDisabled] = ImageList_Create(24, 24, ILC_COLOR32, 10, 0);
    m_imageLists[ImageListType::ListView] = ImageList_Create(16, 16, ILC_COLOR32, 10, 0);

    auto hInstance = m_hInstance;
    auto hBmp = win32_helpers::LoadBitmapFromResource(
        hInstance,
        IDB_TOOLBARICONS
    );
    auto hBmpDisabled = win32_helpers::LoadBitmapFromResource(
        hInstance,
        IDB_TOOLBARICONS_DISABLED
    );
    auto hBmpPackageIcons = win32_helpers::LoadBitmapFromResource(
        hInstance,
        // IDB_PACKAGEVIEWICONS
        IDB_LISTVIEWICONS
    );

    ImageList_Add(m_imageLists[ImageListType::Toolbar], hBmp, NULL);
    ImageList_Add(m_imageLists[ImageListType::ToolbarDisabled], hBmpDisabled, NULL);
    ImageList_Add(m_imageLists[ImageListType::ListView], hBmpPackageIcons, NULL);
}