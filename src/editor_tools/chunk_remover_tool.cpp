#include "editor_tools/chunk_remover_tool.hpp"

editor_tools::ChunkRemoverTool::ChunkRemoverTool(bool &needsRedraw) :
    m_needsRedraw{needsRedraw}
{}

void editor_tools::ChunkRemoverTool::Execute(file::MapDocument& mapDocument, sgc::tile::TilePosition2D cursorPosition)
{
    auto layerManager = mapDocument.GetLayerManager();
    auto activeLayer = layerManager->GetActiveLayerIndex();

    auto tileCheck = layerManager->GetLayers()[activeLayer].storage->GetTileAt(cursorPosition);

    if(tileCheck.has_value()) 
    {
        auto chunkPosition = sgc::data::ChunkedTileStorage::GetChunkCoordAt(cursorPosition);
        m_deleteCommand = std::make_unique<command::DeleteChunkCommand>(chunkPosition);

        auto commandManager = mapDocument.GetCommandManager();
        commandManager->Execute(std::move(m_deleteCommand));
    }
}

void editor_tools::ChunkRemoverTool::Commit(file::MapDocument& mapDocument)
{
    return;
}