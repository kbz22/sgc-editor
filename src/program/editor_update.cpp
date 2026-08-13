#include "program/editor_update.hpp"
#include "program/program.hpp"

void program::UpdateEditorLayerMode(program::EditorLayerMode newMode, program::ProgramContext& programContext)
{
    auto mapDocument = programContext.fileManager->GetActiveDocument();

    if(mapDocument == nullptr) {
        return;
    }

    auto layerManager = mapDocument->GetLayerManager();

    programContext.editorLayerMode = newMode;

    programContext.actionManager->ActionSetChecked(action::ActionType::LayerModeMultilayer, newMode == program::EditorLayerMode::MultiLayer);
    programContext.actionManager->ActionSetChecked(action::ActionType::LayerModeSingleLayer, newMode == program::EditorLayerMode::SingleLayer);
    programContext.actionManager->ActionSetChecked(action::ActionType::LayerModeSingleImage, newMode == program::EditorLayerMode::SingleImage);
    
    switch(newMode) {

        case program::EditorLayerMode::SingleLayer:
        {
            layerManager->SetSingleLayerMode(true);                      
            break;
        }

        case program::EditorLayerMode::MultiLayer:
        {
            layerManager->SetSingleLayerMode(false);
            program::MultiLayerModeSetup(programContext);
            break;
        }

        case program::EditorLayerMode::SingleImage:
        {
            layerManager->SetSingleLayerMode(false);
            program::SingleImageModeSetup(programContext);
            break;
        }
    }

    programContext.mapSection->Refresh(programContext);
    programContext.mapSection->Update();

    programContext.toolbarSection->Refresh(programContext);
    programContext.menuSection->Refresh(programContext);
}

void program::UpdateEditorChunkMode(program::EditorChunkMode newMode, program::ProgramContext& programContext)
{
    programContext.editorChunkMode = newMode;

    programContext.actionManager->ActionSetChecked(action::ActionType::ChunkModeFixedSize, newMode == program::EditorChunkMode::FixedChunks);
    programContext.actionManager->ActionSetChecked(action::ActionType::ChunkModeFree, newMode == program::EditorChunkMode::DynamicChunks);

    programContext.mapSection->SetCheckTileBeforePainting(newMode == program::EditorChunkMode::FixedChunks);

    programContext.toolbarSection->Refresh(programContext);
    programContext.menuSection->Refresh(programContext);
}

void program::UpdateBrushMode(editor_tools::PaintMode newMode, program::ProgramContext& programContext)
{
    programContext.mapSection->SetPaintMode(newMode);

    programContext.actionManager->ActionSetChecked(action::ActionType::PaintModeBrush, newMode == editor_tools::PaintMode::Brush);
    programContext.actionManager->ActionSetChecked(action::ActionType::PaintModeRectangle, newMode == editor_tools::PaintMode::Rectangle);
    programContext.actionManager->ActionSetChecked(action::ActionType::PaintModeFill, newMode == editor_tools::PaintMode::Fill);
    programContext.actionManager->ActionSetChecked(action::ActionType::PaintModeSelect, newMode == editor_tools::PaintMode::Select);

    programContext.toolbarSection->Refresh(programContext);
    programContext.menuSection->Refresh(programContext);
}

void program::UpdateBrushEraseMode(editor_tools::EraserMode newMode, program::ProgramContext& programContext)
{
    programContext.mapSection->SetEraseMode(newMode);

    programContext.actionManager->ActionSetChecked(
        action::ActionType::EraseModeClearTile,
        newMode == editor_tools::EraserMode::ClearTile
    );
    programContext.actionManager->ActionSetChecked(
        action::ActionType::EraseModeDeleteChunk,
        newMode == editor_tools::EraserMode::DeleteChunk
    );

    programContext.toolbarSection->Refresh(programContext);
    programContext.menuSection->Refresh(programContext);
}

void program::UpdateEditorGridMode(program::EditorGridMode newMode, program::ProgramContext& programContext)
{
    programContext.editorGridMode = static_cast<program::EditorGridMode>(
        static_cast<uint8_t>(programContext.editorGridMode) ^
        static_cast<uint8_t>(newMode)
    );

    programContext.actionManager->ActionSetChecked(action::ActionType::GridModeTile, HasFlag(programContext.editorGridMode, program::EditorGridMode::TileGrid));
    programContext.actionManager->ActionSetChecked(action::ActionType::GridModeChunk, HasFlag(programContext.editorGridMode, program::EditorGridMode::ChunkGrid));

    programContext.toolbarSection->Refresh(programContext);
    programContext.menuSection->Refresh(programContext);
    programContext.mapSection->Refresh(programContext);
    programContext.mapSection->Update();
}

void program::UpdateEditorSelectionMode(editor_tools::SelectionMode newMode, program::ProgramContext& programContext)
{
    bool selectionActive = programContext.mapSection->GetPaintMode() == editor_tools::PaintMode::Select;
    constexpr int selectionModeCount = 3;
    editor_tools::SelectionMode selectionOptions[selectionModeCount] = {
        editor_tools::SelectionMode::SingleLayer,
        editor_tools::SelectionMode::AllLayers,
        editor_tools::SelectionMode::VisibleLayers
    };
    action::ActionType selectionActionTypes[selectionModeCount] = {
        action::ActionType::SelectSingleLayerMode,
        action::ActionType::SelectAllLayersMode,
        action::ActionType::SelectVisibleLayersMode
    };

    if(selectionActive) 
    {
        for(int i=0; i<selectionModeCount; ++i) 
        {
            programContext.actionManager->ActionSetChecked(
                selectionActionTypes[i],
                newMode == selectionOptions[i]
            );
            programContext.actionManager->ActionSetEnabled(
                selectionActionTypes[i],
                true
            );
        }
        
        programContext.mapSection->SetSelectionMode(newMode);
    }
    else 
    {
        for(int i=0; i<selectionModeCount; ++i) 
        {
            programContext.actionManager->ActionSetChecked(
                selectionActionTypes[i],
                false
            );
            programContext.actionManager->ActionSetEnabled(
                selectionActionTypes[i],
                false
            );
        }
    }

    programContext.toolbarSection->Refresh(programContext);
    programContext.menuSection->Refresh(programContext);
}