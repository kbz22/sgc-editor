#pragma once

#include "command/icommand.hpp"
#include <sgc/data/chunkedtilestorage.hpp>
#include <vector>

namespace command {

    class DeleteChunkCommand : public ICommand
    {
        private:            
            sgc::tile::TilePosition2D m_chunkPosition;
            std::optional<sgc::data::TileChunk> m_deletedChunk;
            size_t m_layerIndex = 0;

        public:
            DeleteChunkCommand(sgc::tile::TilePosition2D chunkPosition) : m_chunkPosition(chunkPosition) {}

            void Execute() override;
            void Commit() override;
            void Undo() override;
    };

}