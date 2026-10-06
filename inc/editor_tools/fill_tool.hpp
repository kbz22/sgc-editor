#pragma once

#include "editor_tools/brush.hpp"

namespace editor_tools
{
    class FillTool : public Brush
    {
        public:
            FillTool(sgc_view::MapView &mapView, sections::TilesetSection &tilesetSection, const bool &allowChunkCreation, bool &needsRedraw);

            void Execute(file::MapDocument& mapDocument, sgc::tile::TilePosition2D cursorPosition) override;
    };
}