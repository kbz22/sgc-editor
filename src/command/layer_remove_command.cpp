#include "command/layer_remove_command.hpp"
#include "program/program.hpp"
#include <string>

command::LayerRemoveCommand::LayerRemoveCommand(size_t removedLayerIndex) :
    m_removedLayerIndex{removedLayerIndex}
{

}

void command::LayerRemoveCommand::Execute()
{
    auto& programContext = program::GetProgramContext();
    auto mapDocument = programContext.fileManager->GetActiveDocument();

    if(mapDocument != nullptr) {
        auto layerManager = mapDocument->GetLayerManager();
        // auto activeIndex = layerManager->GetActiveLayerIndex();

        // m_removedLayerIndex = activeIndex;

        m_removedLayerItem = layerManager->RemoveLayer(m_removedLayerIndex);

        mapDocument->SetDirty(true);

        programContext.mapSection->Refresh(programContext);
        programContext.mapSection->Update();

        programContext.layersSection->Refresh(programContext);
        programContext.layersSection->Update();
    }
}

void command::LayerRemoveCommand::Commit()
{
    return;
}

void command::LayerRemoveCommand::Undo()
{
    auto& programContext = program::GetProgramContext();
    auto mapDocument = programContext.fileManager->GetActiveDocument();

    if(mapDocument != nullptr) {
        auto layerManager = mapDocument->GetLayerManager();
        layerManager->InsertLayer(m_removedLayerItem, m_removedLayerIndex);

        mapDocument->SetDirty(true);

        programContext.mapSection->Refresh(programContext);
        programContext.mapSection->Update();

        programContext.layersSection->Refresh(programContext);
        programContext.layersSection->Update();
    }
}