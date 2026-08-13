#include "win32_program/windows_init.hpp"
#undef CreateWindow // avoid macro name conflict with sdl::CreateWindow
#include "win32_program/windows_controls.hpp"
#include "win32_program/layout_manager.hpp"
#include "program/program.hpp"
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
#include "defaults.hpp"

program::ProgramContext& program::GetProgramContext()
{
    static ProgramContext context = {};
    return context;
}

std::vector<action::ActionType> g_activeEditorButtons {
    action::ActionType::SaveFile,
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
    action::ActionType::SelectVisibleLayersMode
};

void program::StartDefault()
{
    auto& programContext = GetProgramContext();    
    
    programContext.assetManager = std::make_unique<file::AssetManager>();
    programContext.fileManager = std::make_unique<file::FileManager>();
    programContext.shortcutManager = std::make_unique<win32_program::ShortcutManager>(
        programContext.mainWindowContext->hInstance,
        programContext.mainWindowContext->hMainWindow
    );

    programContext.actionManager->ActionSetEnabled(g_activeEditorButtons, false);

    programContext.menuSection = std::make_unique<sections::MenuSection>(programContext);
    programContext.sections.push_back(programContext.menuSection.get());
    programContext.menuSection->Refresh(programContext);

    programContext.mapSection = std::make_unique<sections::MapSection>(programContext);
    programContext.sections.push_back(programContext.mapSection.get());    

    programContext.toolbarSection = std::make_unique<sections::ToolbarSection>(programContext);
    programContext.sections.push_back(programContext.toolbarSection.get());

    programContext.tilesetSection = std::make_unique<sections::TilesetSection>(programContext);
    programContext.sections.push_back(programContext.tilesetSection.get());    

    programContext.layersSection = std::make_unique<sections::LayersSection>(programContext);
    programContext.sections.push_back(programContext.layersSection.get());

    programContext.packageSection = std::make_unique<sections::PackageSection>(programContext);
    programContext.sections.push_back(programContext.packageSection.get());

    programContext.statusSection = std::make_unique<sections::StatusSection>(programContext);
    programContext.sections.push_back(programContext.statusSection.get());

    programContext.layersSection->RegisterSelectedLayerChangeCallback([&programContext](size_t index) {
        auto document = programContext.fileManager->GetActiveDocument();
        if(document != nullptr) {
            auto layerManager = document->GetLayerManager();
            layerManager->SetActiveLayerIndex(index);

            if(programContext.editorLayerMode == EditorLayerMode::MultiLayer) {
                MultiLayerModeSetup(programContext);
            }

            programContext.mapSection->Refresh(programContext);
            programContext.mapSection->Update();
        }
    });

    programContext.layersSection->RegisterLayerVisibilityChangeCallback([&programContext](size_t index, bool visible) {
        auto document = programContext.fileManager->GetActiveDocument();
        if(document != nullptr) {
            auto layerManager = document->GetLayerManager();
            layerManager->SetLayerVisibility(index, visible);

            programContext.mapSection->Refresh(programContext);
            programContext.mapSection->Update();
        }
    });

    programContext.layersSection->RegisterLayerNameChangeCallback([&programContext](size_t index, std::wstring newName) {
        auto document = programContext.fileManager->GetActiveDocument();
        if(document != nullptr) {
            auto layerManager = document->GetLayerManager();
            layerManager->SetLayerName(index, newName);
            document->SetDirty(true);

            programContext.mapSection->Refresh(programContext);
            programContext.mapSection->Update();

            programContext.packageSection->UpdateTreeViewItems(programContext);
        }
    });

    programContext.packageSection->RegisterFileActionCallback(sections::FileAction::ItemSelected, [&programContext](file::IFile* file, size_t index) {
        auto fileManager = programContext.fileManager.get();
        auto location = file::DocumentLocation{file, index};
        
        fileManager->SelectDocument(location);

        programContext.mapSection->Refresh(programContext);
        programContext.mapSection->Update();

        programContext.layersSection->Refresh(programContext);
        programContext.layersSection->Update();

        programContext.tilesetSection->Refresh(programContext);
        programContext.tilesetSection->Update();

        programContext.packageSection->UpdateTreeViewItems(programContext);
        programContext.packageSection->Update();
    });

    programContext.packageSection->RegisterFileActionCallback(sections::FileAction::ItemDoubleClicked, [&programContext](file::IFile* file, size_t index) {
        auto fileManager = programContext.fileManager.get();
        auto location = file::DocumentLocation{file, index};
        
        fileManager->SetActiveDocument(location);

        programContext.mapSection->Refresh(programContext);
        programContext.mapSection->Update();

        programContext.layersSection->Refresh(programContext);
        programContext.layersSection->Update();

        programContext.tilesetSection->Refresh(programContext);
        programContext.tilesetSection->Update();

        programContext.packageSection->UpdateTreeViewItems(programContext);
        programContext.packageSection->Update();
    });
}

void RedrawAllSections(program::ProgramContext& programContext)
{
    for(auto section : programContext.sections) {
        section->HandleSectionResize();
        section->Update();
    }
}

void RefreshAllSection(program::ProgramContext& programContext)
{
    for(auto section : programContext.sections) {
        section->Refresh(programContext);        
    }
}

void program::RefreshEditor()
{
    auto &programContext = GetProgramContext();
    auto currentDocument = programContext.fileManager->GetActiveDocument();

    if(currentDocument == nullptr || !currentDocument->IsEditable()) {
        programContext.actionManager->ActionSetChecked(g_activeEditorButtons, false);
        programContext.actionManager->ActionSetEnabled(g_activeEditorButtons, false);
    }
    else if(currentDocument->IsEditable()) {
        programContext.actionManager->ActionSetEnabled(g_activeEditorButtons, true);
        program::UpdateEditorLayerMode(programContext.editorLayerMode, programContext);
        program::UpdateEditorChunkMode(programContext.editorChunkMode, programContext);
        program::UpdateBrushMode(programContext.mapSection->GetPaintMode(), programContext);
        program::UpdateEditorSelectionMode(programContext.mapSection->GetSelectionMode(), programContext);
    }    

    RefreshAllSection(programContext);
    RedrawAllSections(programContext);

    return;
}
    

void program::RegisterActions()
{
    auto& programContext = GetProgramContext();

    if(programContext.actionManager == nullptr) {
        programContext.actionManager = std::make_unique<action::ActionManager>();
    }

    auto newDocAction = programContext.actionManager->Register(std::make_unique<action::NewDocumentAction>());
    auto newMapAction = programContext.actionManager->Register(std::make_unique<action::NewMapDocumentAction>());
    auto newTilesetAction = programContext.actionManager->Register(std::make_unique<action::NewTilesetDocumentAction>());
    programContext.actionManager->Register(std::make_unique<action::OpenFileAction>());
    programContext.actionManager->Register(std::make_unique<action::SaveFileAction>());
    programContext.actionManager->Register(std::make_unique<action::CloseFileAction>());
    programContext.actionManager->Register(std::make_unique<action::UndoAction>());
    programContext.actionManager->Register(std::make_unique<action::RedoAction>());
    programContext.actionManager->Register(std::make_unique<action::LayerAddAction>());
    programContext.actionManager->Register(std::make_unique<action::LayerRemoveAction>());
    programContext.actionManager->Register(std::make_unique<action::LayerMoveAction>(-1));
    programContext.actionManager->Register(std::make_unique<action::LayerMoveAction>(1));
    programContext.actionManager->Register(std::make_unique<action::ChangeLayerModeAction>(EditorLayerMode::MultiLayer));
    programContext.actionManager->Register(std::make_unique<action::ChangeLayerModeAction>(EditorLayerMode::SingleLayer));
    programContext.actionManager->Register(std::make_unique<action::ChangeLayerModeAction>(EditorLayerMode::SingleImage));
    programContext.actionManager->Register(std::make_unique<action::ChangeChunkModeAction>(EditorChunkMode::FixedChunks));
    programContext.actionManager->Register(std::make_unique<action::ChangeChunkModeAction>(EditorChunkMode::DynamicChunks));
    programContext.actionManager->Register(std::make_unique<action::ChangeBrushModeAction>(editor_tools::PaintMode::Brush));
    programContext.actionManager->Register(std::make_unique<action::ChangeBrushModeAction>(editor_tools::PaintMode::Rectangle));
    programContext.actionManager->Register(std::make_unique<action::ChangeBrushModeAction>(editor_tools::PaintMode::Fill));
    programContext.actionManager->Register(std::make_unique<action::ChangeBrushModeAction>(editor_tools::PaintMode::Select));
    programContext.actionManager->Register(std::make_unique<action::ZoomResetAction>());
    programContext.actionManager->Register(std::make_unique<action::ZoomAction>(defaults::zoomFactor));
    programContext.actionManager->Register(std::make_unique<action::ZoomAction>(1.0f / defaults::zoomFactor));
    programContext.actionManager->Register(std::make_unique<action::ChangeBrushEraseModeAction>(editor_tools::EraserMode::ClearTile));
    programContext.actionManager->Register(std::make_unique<action::ChangeBrushEraseModeAction>(editor_tools::EraserMode::DeleteChunk));
    programContext.actionManager->Register(std::make_unique<action::ZoomSelectAction>());
    programContext.actionManager->Register(std::make_unique<action::ChangeGridModeAction>(EditorGridMode::TileGrid));
    programContext.actionManager->Register(std::make_unique<action::ChangeGridModeAction>(EditorGridMode::ChunkGrid));
    programContext.actionManager->Register(std::make_unique<action::ChangeSelectionModeAction>(editor_tools::SelectionMode::SingleLayer));
    programContext.actionManager->Register(std::make_unique<action::ChangeSelectionModeAction>(editor_tools::SelectionMode::AllLayers));
    programContext.actionManager->Register(std::make_unique<action::ChangeSelectionModeAction>(editor_tools::SelectionMode::VisibleLayers));

    reinterpret_cast<action::NewDocumentAction*>(newDocAction)->SetItems({
        newMapAction,
        newTilesetAction
    });

    programContext.actionManager->Register(std::make_unique<action::FileMenuAction>());
    programContext.actionManager->Register(std::make_unique<action::EditMenuAction>());
    programContext.actionManager->Register(std::make_unique<action::MapMenuAction>());
    programContext.actionManager->Register(std::make_unique<action::ViewMenuAction>());
    programContext.actionManager->Register(std::make_unique<action::HelpMenuAction>());
}