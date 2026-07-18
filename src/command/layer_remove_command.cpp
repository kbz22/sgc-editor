#include "command/layer_remove_command.hpp"
#include "program/program.hpp"
#include <string>

void command::LayerRemoveCommand::Execute()
{
    auto& programContext = program::GetProgramContext();

    if(programContext.layerManager != nullptr) {
        auto activeIndex = programContext.layerManager->GetActiveLayerIndex();
        m_removedLayerIndex = activeIndex;
        m_removedLayerStorage = programContext.layerManager->RemoveLayer(m_removedLayerIndex);

        programContext.mapSection->Refresh(*programContext.layerManager);
        programContext.mapSection->Update();

        programContext.layersSection->Refresh(*programContext.layerManager);
        programContext.layersSection->Update();
    }
}

void command::LayerRemoveCommand::Undo()
{
    auto& programContext = program::GetProgramContext();

    if(programContext.layerManager != nullptr) {
        programContext.layerManager->InsertLayer({
                m_removedLayerStorage,
                L"Layer " + std::to_wstring(m_removedLayerIndex)
            }, m_removedLayerIndex
        );

        programContext.mapSection->Refresh(*programContext.layerManager);
        programContext.mapSection->Update();

        programContext.layersSection->Refresh(*programContext.layerManager);
        programContext.layersSection->Update();
    }
}