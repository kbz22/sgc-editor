#include "command/create_chunk_command.hpp"
#include "program/program.hpp"

command::CreateChunkCommand::CreateChunkCommand(sgc::tile::TilePosition2D chunkPosition, size_t layerIndex, file::MapDocument &mapDocument, sgc::tile::TileId clearTileId) :
    m_chunkPosition{chunkPosition},
    m_layerIndex{layerIndex},
    m_mapDocument{mapDocument},
    m_clearTileId{clearTileId}
{}

command::CreateChunkCommand::CreateChunkCommand(const CreateChunkCommand& other) = default;

void command::CreateChunkCommand::Execute()
{
    auto layerManager = m_mapDocument.GetLayerManager();    
    auto layer = layerManager->GetLayers()[m_layerIndex];        
    
    auto storage = dynamic_cast<sgc::data::ChunkedTileStorage*>(
        layer.storage.get()
    );

    storage->SetChunkAt(m_chunkPosition, m_clearTileId);

    m_mapDocument.SetDirty(true);
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