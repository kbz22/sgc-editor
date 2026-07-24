#include "program/editor_update.hpp"
#include "program/program.hpp"

void program::UpdateEditorLayerMode(program::EditorLayerMode newMode)
{
    auto &programContext = program::GetProgramContext();
    auto &mapDocument = programContext.mapDocument;
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