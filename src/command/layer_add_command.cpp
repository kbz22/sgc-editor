#include "command/layer_add_command.hpp"
#include "program/program.hpp"
#include "program/layer_manager.hpp"
#include <sgc/data/chunkedtilestorage.hpp>
#include <memory>

void command::LayerAddCommand::Execute()
{
    auto &programContext = program::GetProgramContext();
    auto mapDocument = programContext.fileManager->GetSelectedDocument();

    if(mapDocument != nullptr) {
        auto layerManager = mapDocument->GetLayerManager();
        auto storage = std::make_shared<sgc::data::ChunkedTileStorage>();

        //! This should probably be removed. The user should create chunks manually.
        // I leave it for now because it is convienient for testing purposes.
        // If you added empty tiles to tileset probably best to remove this.
        storage->SetTileAt({0, 0}, 0);

        m_addedLayerIndex = layerManager->AddLayer({
            storage,
            L"Layer " + std::to_wstring(layerManager->GetSize())
        });

        programContext.layersSection->Refresh(programContext);
        programContext.layersSection->Update();

        programContext.mapSection->Refresh(programContext);
        programContext.mapSection->Update();
    }
}

void command::LayerAddCommand::Undo()
{
    auto& programContext = program::GetProgramContext();

    if(programContext.fileManager->GetSelectedDocument() != nullptr) {
        auto layerManager = programContext.fileManager->GetSelectedDocument()->GetLayerManager();
        layerManager->RemoveLayer(m_addedLayerIndex);

        programContext.layersSection->Refresh(programContext);
        programContext.layersSection->Update();

        programContext.mapSection->Refresh(programContext);
        programContext.mapSection->Update();
    }
}