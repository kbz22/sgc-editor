#include "command/create_chunk_command.hpp"
#include "program/program.hpp"

command::CreateChunkCommand::CreateChunkCommand(sgc::tile::TilePosition2D chunkPosition) :
    m_chunkPosition(chunkPosition)
{}

command::CreateChunkCommand::CreateChunkCommand(const CreateChunkCommand& other) = default;

void command::CreateChunkCommand::Execute()
{
    auto& programContext = program::GetProgramContext();
    auto mapDocument = programContext.GetManager<file::FileManager>()->GetActiveDocument();

    if(mapDocument != nullptr) {
        auto layerManager = mapDocument->GetLayerManager();
        m_layerIndex = layerManager->GetActiveLayerIndex();
        auto currentLayer = layerManager->GetLayers()[m_layerIndex];
        auto tilesetSection = programContext.GetSection<sections::TilesetSection>();
        
        auto storage = dynamic_cast<sgc::data::ChunkedTileStorage*>(
            currentLayer.storage.get()
        );

        storage->SetChunkAt(m_chunkPosition, tilesetSection->GetClearTileId());

        mapDocument->SetDirty(true);
    }
}

void command::CreateChunkCommand::Commit()
{
    return;
}

void command::CreateChunkCommand::Undo()
{
    auto& programContext = program::GetProgramContext();
    auto mapDocument = programContext.GetManager<file::FileManager>()->GetActiveDocument();

    if(mapDocument != nullptr) {
        auto layerManager = mapDocument->GetLayerManager();
        auto editedLayer = layerManager->GetLayers()[m_layerIndex];
        
        auto storage = dynamic_cast<sgc::data::ChunkedTileStorage*>(
            editedLayer.storage.get()
        );

        storage->RemoveChunkAt(m_chunkPosition);

        mapDocument->SetDirty(true);
    }
}