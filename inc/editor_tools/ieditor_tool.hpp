#pragma once

#include "file/map_document.hpp"
#include <sgc/graphics/tileset.hpp>

namespace editor_tools
{
    class IEditorTool
    {
        public:
            virtual ~IEditorTool() = default;

            virtual void Execute(file::MapDocument& mapDocument, sgc::tile::TilePosition2D cursorPosition) = 0;
            virtual void Commit(file::MapDocument& mapDocument) = 0;
    };
}