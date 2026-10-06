#include "command/layer_remove_command.hpp"
#include "program/program.hpp"
#include <string>

command::LayerRemoveCommand::LayerRemoveCommand(const LayerRemoveCommand& other) = default;

command::LayerRemoveCommand::LayerRemoveCommand(size_t removedLayerIndex) :
    m_removedLayerIndex{removedLayerIndex}
{

}

void command::LayerRemoveCommand::Execute()
{
    auto& programContext = program::GetProgramContext();
    auto fileManager = programContext.GetManager<file::FileManager>();
    auto mapDocument = fileManager->GetActiveDocument();

    if(mapDocument != nullptr) {
        auto layerManager = mapDocument->GetLayerManager();
        // auto activeIndex = layerManager->GetActiveLayerIndex();

        // m_removedLayerIndex = activeIndex;

        m_removedLayerItem = layerManager->RemoveLayer(m_removedLayerIndex);

        mapDocument->SetDirty(true);

        auto layersSection = programContext.GetSection<sections::LayersSection>();
        layersSection->Refresh(programContext);
        layersSection->Update();
    }
}

void command::LayerRemoveCommand::Commit()
{
    return;
}

void command::LayerRemoveCommand::Undo()
{
    auto& programContext = program::GetProgramContext();
    auto fileManager = programContext.GetManager<file::FileManager>();
    auto mapDocument = fileManager->GetActiveDocument();

    if(mapDocument != nullptr) {
        auto layerManager = mapDocument->GetLayerManager();
        layerManager->InsertLayer(m_removedLayerItem, m_removedLayerIndex);

        mapDocument->SetDirty(true);

        auto mapSection = programContext.GetSection<sections::MapSection>();
        mapSection->Refresh(programContext);
        mapSection->Update();

        auto layersSection = programContext.GetSection<sections::LayersSection>();
        layersSection->Refresh(programContext);
        layersSection->Update();
    }
}