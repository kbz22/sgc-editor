#pragma once

#include "editor_tools/ieditor_tool.hpp"
#include "command/delete_chunk_command.hpp"
#include <memory>

namespace editor_tools
{
    class ChunkRemoverTool : public IEditorTool
    {
        private:
            std::unique_ptr<command::DeleteChunkCommand> m_deleteCommand;
            bool &m_needsRedraw;

        public:
            ChunkRemoverTool(bool &needsRedraw);

            void Execute(file::MapDocument& mapDocument, sgc::tile::TilePosition2D cursorPosition) override;
            void Commit(file::MapDocument& mapDocument) override;
    };
}