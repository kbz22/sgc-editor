#include "command/layer_add_command.hpp"
#include "program/program.hpp"
#include "program/layer_manager.hpp"
#include <sgc/data/chunkedtilestorage.hpp>
#include <memory>

command::LayerAddCommand::LayerAddCommand(const LayerAddCommand& other) = default;

void command::LayerAddCommand::Execute()
{
    auto &programContext = program::GetProgramContext();
    auto fileManager = programContext.GetManager<file::FileManager>();
    auto mapDocument = fileManager->GetActiveDocument();

    if(mapDocument != nullptr) {
        auto layerManager = mapDocument->GetLayerManager();
        auto storage = std::make_shared<sgc::data::ChunkedTileStorage>();

        m_addedLayerIndex = layerManager->AddLayer({
            storage,
            L"Layer " + std::to_wstring(layerManager->GetSize())
        });

        mapDocument->SetDirty(true);

        auto layersSection = programContext.GetSection<sections::LayersSection>();
        layersSection->Refresh(programContext);
        layersSection->Update();
    }
}

void command::LayerAddCommand::Commit()
{
    // No additional commit logic needed for adding a layer
    return;
}

void command::LayerAddCommand::Undo()
{
    auto& programContext = program::GetProgramContext();
    auto mapDocument = programContext.GetManager<file::FileManager>()->GetActiveDocument();

    if(mapDocument != nullptr) {
        auto layerManager = mapDocument->GetLayerManager();
        layerManager->RemoveLayer(m_addedLayerIndex);

        mapDocument->SetDirty(true);

        auto layersSection = programContext.GetSection<sections::LayersSection>();
        layersSection->Refresh(programContext);
        layersSection->Update();

        auto mapSection = programContext.GetSection<sections::MapSection>();
        mapSection->Refresh(programContext);
        mapSection->Update();
    }
}