#include "program/editor_update.hpp"
#include "program/program.hpp"

void program::UpdateEditorLayerMode(program::EditorLayerMode newMode)
{
    auto& programContext = program::GetProgramContext();

    programContext.editorLayerMode = newMode;
    
    switch(newMode) {

        case program::EditorLayerMode::SingleLayer:
        {
            programContext.layerManager->SetSingleLayerMode(true);            
            break;
        }

        case program::EditorLayerMode::MultiLayer:
        {
            programContext.layerManager->SetSingleLayerMode(false);
            program::MultiLayerModeSetup(programContext);
            break;
        }

        case program::EditorLayerMode::SingleImage:
        {
            programContext.layerManager->SetSingleLayerMode(false);
            program::SingleImageModeSetup(programContext);
            break;
        }
    }

    programContext.mapSection->Refresh(*programContext.layerManager);
    programContext.mapSection->Update();
}

void program::UpdateEditorChunkMode(program::EditorChunkMode newMode)
{
    auto& programContext = program::GetProgramContext();

    programContext.editorChunkMode = newMode;

    programContext.mapSection->SetCheckTileBeforePainting(newMode == program::EditorChunkMode::FixedChunks);
}