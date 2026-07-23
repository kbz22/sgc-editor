#include "win32_program/windows_init.hpp"
#undef CreateWindow // avoid macro name conflict with sdl::CreateWindow
#include "win32_program/windows_controls.hpp"
#include "win32_program/layout_manager.hpp"
#include "program/program.hpp"
#include "program/layer_manager.hpp"
#include "program/editor_update.hpp"
#include "win32_program/toolbar_functions.hpp"
#include <sgc/data/chunkedtilestorage.hpp>

#include "action/new_file_action.hpp"
#include "action/undo_action.hpp"
#include "action/redo_action.hpp"
#include "action/layer_add_action.hpp"
#include "action/layer_remove_action.hpp"
#include "action/layer_move_action.hpp"
#include "action/change_layer_mode_action.hpp"
#include "action/change_chunk_mode_action.hpp"
#include "action/file_menu_action.hpp"

program::ProgramContext& program::GetProgramContext()
{
    static ProgramContext context = {};
    return context;
}

std::vector<action::ActionType> g_activeEditorButtons {
    action::ActionType::Undo,
    action::ActionType::Redo,
    action::ActionType::AddLayer,
    action::ActionType::RemoveLayer,
    action::ActionType::MoveLayer,
    action::ActionType::LayerModeMultilayer,
    action::ActionType::LayerModeSingleLayer,
    action::ActionType::LayerModeSingleImage,
    action::ActionType::ChunkModeFixedSize,
    action::ActionType::ChunkModeFree
};

void program::StartDefault()
{
    auto& programContext = GetProgramContext();    

    programContext.actionManager->ActionSetEnabled(g_activeEditorButtons, false);
    programContext.actionManager->ActionSetChecked(action::ActionType::LayerModeMultilayer, false);
    programContext.actionManager->ActionSetChecked(action::ActionType::ChunkModeFixedSize, false);    

    programContext.menuSection = std::make_unique<sections::MenuSection>(programContext);
    programContext.sections.push_back(programContext.menuSection.get());

    programContext.toolbarSection = std::make_unique<sections::ToolbarSection>(programContext);
    programContext.sections.push_back(programContext.toolbarSection.get());    

    programContext.mapSection = std::make_unique<sections::MapSection>(programContext);
    programContext.sections.push_back(programContext.mapSection.get());        

    programContext.tilesetSection = std::make_unique<sections::TilesetSection>(programContext);
    programContext.sections.push_back(programContext.tilesetSection.get());

    programContext.layersSection = std::make_unique<sections::LayersSection>(programContext);
    programContext.sections.push_back(programContext.layersSection.get());

    programContext.packageSection = std::make_unique<sections::PackageSection>(programContext);
    programContext.sections.push_back(programContext.packageSection.get()); 
}

void program::StartEditor(std::wstring tilesetPath, int tileWidth, int tileHeight, int chunksSizeX, int chunksSizeY)
{
    (void)chunksSizeX;
    (void)chunksSizeY;
    
    auto& programContext = GetProgramContext();

    programContext.actionManager->ActionSetEnabled(g_activeEditorButtons, true);
    programContext.actionManager->ActionSetChecked(action::ActionType::LayerModeMultilayer, true);
    programContext.actionManager->ActionSetChecked(action::ActionType::ChunkModeFixedSize, true);

    programContext.toolbarSection->Refresh();    

    if(programContext.selectionRectangleOnTileset != nullptr) {
        programContext.selectionRectangleOnTileset.reset();
    }

    programContext.selectionRectangleOnTileset = std::make_unique<sgc::graphics::Rectangle>(
        sgc::math::vec2{ 0, 0 },
        sgc::math::vec2{ tileWidth, tileHeight }
    );
    programContext.selectionRectangleOnTileset->SetColor({ 0, 128, 255, 128 });

    programContext.tilesetSection->LoadTileset(tilesetPath, tileWidth, tileHeight);

    programContext.mapSection->LoadTileset(tilesetPath, tileWidth, tileHeight);

    programContext.mapSection->HandleSectionResize();
    programContext.tilesetSection->HandleSectionResize();
    programContext.mapSection->Update();
    programContext.tilesetSection->Update();

    programContext.layerManager = std::make_unique<program::LayerManager>();

    programContext.layerManager->AddLayer({
        std::make_shared<sgc::data::ChunkedTileStorage>(),
        L"Layer 0"
    });
    programContext.layerManager->SetActiveLayerIndex(0);
    programContext.layerManager->SetBaseLayerIndex(0);

    for(int x=0;x<32;++x) {
        for(int y=0;y<32;++y) {
            programContext.layerManager->GetLayers()[0]
                .storage->SetTileAt({ x, y }, 1);
        }
    }

    programContext.mapSection->Refresh(*programContext.layerManager);
    programContext.mapSection->Update();

    programContext.layersSection->RegisterSelectedLayerChangeCallback([&programContext](size_t index) {
        if(programContext.layerManager != nullptr) {
            programContext.layerManager->SetActiveLayerIndex(index);

            if(programContext.editorLayerMode == EditorLayerMode::MultiLayer) {
                MultiLayerModeSetup(programContext);
            }

            programContext.mapSection->Refresh(*programContext.layerManager);
            programContext.mapSection->Update();
        }
    });

    programContext.layersSection->RegisterLayerVisibilityChangeCallback([&programContext](size_t index, bool visible) {
        if(programContext.layerManager != nullptr) {
            programContext.layerManager->SetLayerVisibility(index, visible);

            programContext.mapSection->Refresh(*programContext.layerManager);
            programContext.mapSection->Update();
        }
    });

    programContext.layersSection->Refresh(*programContext.layerManager);
    programContext.layersSection->Update();

    program::UpdateEditorLayerMode(programContext.editorLayerMode);
    program::UpdateEditorChunkMode(programContext.editorChunkMode);
}

void program::RegisterActions()
{
    auto& programContext = GetProgramContext();

    if(programContext.actionManager == nullptr) {
        programContext.actionManager = std::make_unique<action::ActionManager>();
    }

    programContext.actionManager->Register(std::make_unique<action::NewFileAction>());
    // programContext.actionManager->Register(std::make_unique<action::OpenFileAction>());
    // programContext.actionManager->Register(std::make_unique<action::SaveFileAction>());
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

    programContext.actionManager->Register(std::make_unique<action::FileMenuAction>());

}