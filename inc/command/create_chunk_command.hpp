#pragma once

#include "command/icommand.hpp"
#include "file/map_document.hpp"
#include <sgc/data/chunkedtilestorage.hpp>

namespace command
{
    class CreateChunkCommand : public ICommand
    {
        private:            
            sgc::tile::TilePosition2D m_chunkPosition;
            file::MapDocument &m_mapDocument;
            size_t m_layerIndex = 0;
            sgc::tile::TileId m_clearTileId = 0;

        public:
            CreateChunkCommand(sgc::tile::TilePosition2D chunkPosition, size_t layerIndex, file::MapDocument &mapDocument, sgc::tile::TileId clearTileId);
            CreateChunkCommand(const CreateChunkCommand& other);

            void Execute() override;
            void Commit() override;
            void Undo() override;
    };
}