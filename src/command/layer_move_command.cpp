#include "command/layer_move_command.hpp"
#include "program/program.hpp"
#include <stdexcept>

command::LayerMoveCommand::LayerMoveCommand(const LayerMoveCommand& other) = default;

void command::LayerMoveCommand::Execute()
{
    auto &programContext = program::GetProgramContext();
    auto mapDocument = programContext.GetManager<file::FileManager>()->GetActiveDocument();

    if(mapDocument != nullptr)
    {
        auto layerManager = mapDocument->GetLayerManager();

        try {            
            layerManager->MoveActiveLayer(m_movement);
            mapDocument->SetDirty(true);
        } catch ([[maybe_unused]]const std::out_of_range& e) {
            //! no need to note the out of range error
        }

        auto mapSection = programContext.GetSection<sections::MapSection>();
        mapSection->Refresh(programContext);
        mapSection->Update();

        auto layersSection = programContext.GetSection<sections::LayersSection>();
        layersSection->SetSelectedLayer(layerManager->GetActiveLayerIndex());
        layersSection->Refresh(programContext);
        layersSection->Update();
    }
}

void command::LayerMoveCommand::Commit()
{    
    return;
}

void command::LayerMoveCommand::Undo()
{
    auto &programContext = program::GetProgramContext();
    auto mapDocument = programContext.GetManager<file::FileManager>()->GetActiveDocument();

    if(mapDocument != nullptr)
    {
        auto layerManager = mapDocument->GetLayerManager();

        try {            
            layerManager->MoveActiveLayer(-m_movement);
            mapDocument->SetDirty(true);
        } catch ([[maybe_unused]]const std::out_of_range& e) {
            //! no need to note the out of range error
        }        

        auto mapSection = programContext.GetSection<sections::MapSection>();
        mapSection->Refresh(programContext);
        mapSection->Update();

        auto layersSection = programContext.GetSection<sections::LayersSection>();
        layersSection->SetSelectedLayer(layerManager->GetActiveLayerIndex());
        layersSection->Refresh(programContext);
        layersSection->Update();
    }
}