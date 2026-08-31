#include "command/delete_chunk_command.hpp"
#include "program/program.hpp"

command::DeleteChunkCommand::DeleteChunkCommand(const DeleteChunkCommand& other) = default;

void command::DeleteChunkCommand::Execute()
{
    auto& programContext = program::GetProgramContext();
    auto mapDocument = programContext.fileManager->GetActiveDocument();

    if(mapDocument != nullptr) {
        auto layerManager = mapDocument->GetLayerManager();
        m_layerIndex = layerManager->GetActiveLayerIndex();
        auto currentLayer = layerManager->GetLayers()[m_layerIndex];
        
        auto storage = dynamic_cast<sgc::data::ChunkedTileStorage*>(
            currentLayer.storage.get()
        );

        m_deletedChunk = storage->RemoveChunkAt(m_chunkPosition);

        if(m_deletedChunk.has_value()) {
            mapDocument->SetDirty(true);

            programContext.mapSection->Refresh(programContext);
            programContext.mapSection->Update();
        }        
    }
}

void command::DeleteChunkCommand::Commit()
{
    return;
}

void command::DeleteChunkCommand::Undo()
{
    if(m_deletedChunk.has_value()) {
        auto& programContext = program::GetProgramContext();
        auto mapDocument = programContext.fileManager->GetActiveDocument();

        if(mapDocument != nullptr) {
            auto layerManager = mapDocument->GetLayerManager();
            auto currentLayer = layerManager->GetLayers()[m_layerIndex];
            
            auto storage = dynamic_cast<sgc::data::ChunkedTileStorage*>(
                currentLayer.storage.get()
            );

            storage->SetChunkAt(m_chunkPosition, m_deletedChunk.value());

            mapDocument->SetDirty(true);

            programContext.mapSection->Refresh(programContext);
            programContext.mapSection->Update();
        }
    }
}