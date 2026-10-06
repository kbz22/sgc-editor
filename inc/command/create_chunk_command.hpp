#pragma once

#include "command/icommand.hpp"
#include <sgc/data/chunkedtilestorage.hpp>

namespace command
{
    class CreateChunkCommand : public ICommand
    {
        private:            
            sgc::tile::TilePosition2D m_chunkPosition;
            size_t m_layerIndex = 0;

        public:
            CreateChunkCommand(sgc::tile::TilePosition2D chunkPosition);
            CreateChunkCommand(const CreateChunkCommand& other);

            void Execute() override;
            void Commit() override;
            void Undo() override;
    };
}