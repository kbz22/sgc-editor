#include "program/editor_update.hpp"
#include "program/program.hpp"

void program::UpdateEditorLayerMode(program::EditorLayerMode newMode)
{
    auto &programContext = program::GetProgramContext();
    // auto &fileManager = programContext.fileManager;
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

void program::UpdateEditorChunkMode(program::EditorChunkMode newMode)
{
    auto& programContext = program::GetProgramContext();

    programContext.editorChunkMode = newMode;

    programContext.actionManager->ActionSetChecked(action::ActionType::ChunkModeFixedSize, newMode == program::EditorChunkMode::FixedChunks);
    programContext.actionManager->ActionSetChecked(action::ActionType::ChunkModeFree, newMode == program::EditorChunkMode::DynamicChunks);

    programContext.mapSection->SetCheckTileBeforePainting(newMode == program::EditorChunkMode::FixedChunks);

    programContext.toolbarSection->Refresh(programContext);
    programContext.menuSection->Refresh(programContext);
}

void program::UpdateBrushMode(editor_tools::PaintMode newMode)
{
    auto& programContext = program::GetProgramContext();

    programContext.mapSection->SetPaintMode(newMode);

    programContext.actionManager->ActionSetChecked(action::ActionType::PaintModeBrush, newMode == editor_tools::PaintMode::Brush);
    programContext.actionManager->ActionSetChecked(action::ActionType::PaintModeRectangle, newMode == editor_tools::PaintMode::Rectangle);
    programContext.actionManager->ActionSetChecked(action::ActionType::PaintModeFill, newMode == editor_tools::PaintMode::Fill);
    programContext.actionManager->ActionSetChecked(action::ActionType::PaintModeSelect, newMode == editor_tools::PaintMode::Select);

    programContext.toolbarSection->Refresh(programContext);
    programContext.menuSection->Refresh(programContext);
}

void program::UpdateBrushEraseMode(editor_tools::EraserMode newMode)
{
    auto& programContext = program::GetProgramContext();

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