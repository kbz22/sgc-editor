#include "command/layer_move_command.hpp"
#include "program/program.hpp"
#include <stdexcept>

void command::LayerMoveCommand::Execute()
{
    auto& programContext = program::GetProgramContext();

    if(programContext.layerManager != nullptr) {
        try {            
            programContext.layerManager->MoveActiveLayer(m_movement);
        } catch ([[maybe_unused]]const std::out_of_range& e) {
            //! no need to note the out of range error
        }

        programContext.mapSection->Refresh(*programContext.layerManager);
        programContext.mapSection->Update();

        programContext.layersSection->SetSelectedLayer(programContext.layerManager->GetActiveLayerIndex());
        programContext.layersSection->Refresh(*programContext.layerManager);
        programContext.layersSection->Update();
    }
}

void command::LayerMoveCommand::Undo()
{
    auto& programContext = program::GetProgramContext();

    if(programContext.layerManager != nullptr) {
        try {            
            programContext.layerManager->MoveActiveLayer(-m_movement);
        } catch ([[maybe_unused]]const std::out_of_range& e) {
            //! no need to note the out of range error
        }

        programContext.mapSection->Refresh(*programContext.layerManager);
        programContext.mapSection->Update();

        programContext.layersSection->SetSelectedLayer(programContext.layerManager->GetActiveLayerIndex());
        programContext.layersSection->Refresh(*programContext.layerManager);
        programContext.layersSection->Update();
    }
}