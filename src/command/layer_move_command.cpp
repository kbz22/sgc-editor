#include "command/layer_move_command.hpp"
#include "program/program.hpp"
#include <stdexcept>

void command::LayerMoveCommand::Execute()
{
    auto &programContext = program::GetProgramContext();
    auto mapDocument = programContext.fileManager->GetSelectedDocument();

    if(mapDocument != nullptr)
    {
        auto layerManager = mapDocument->GetLayerManager();

        try {            
            layerManager->MoveActiveLayer(m_movement);
        } catch ([[maybe_unused]]const std::out_of_range& e) {
            //! no need to note the out of range error
        }

        programContext.mapSection->Refresh(programContext);
        programContext.mapSection->Update();

        programContext.layersSection->SetSelectedLayer(layerManager->GetActiveLayerIndex());
        programContext.layersSection->Refresh(programContext);
        programContext.layersSection->Update();
    }
}

void command::LayerMoveCommand::Undo()
{
    auto &programContext = program::GetProgramContext();
    auto mapDocument = programContext.fileManager->GetSelectedDocument();

    if(mapDocument != nullptr)
    {
        auto layerManager = mapDocument->GetLayerManager();

        try {            
            layerManager->MoveActiveLayer(-m_movement);
            mapDocument->SetDirty(true);
        } catch ([[maybe_unused]]const std::out_of_range& e) {
            //! no need to note the out of range error
        }        

        programContext.mapSection->Refresh(programContext);
        programContext.mapSection->Update();

        programContext.layersSection->SetSelectedLayer(layerManager->GetActiveLayerIndex());
        programContext.layersSection->Refresh(programContext);
        programContext.layersSection->Update();
    }
}