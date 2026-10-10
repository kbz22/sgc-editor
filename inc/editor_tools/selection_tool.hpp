#pragma once

#include "editor_tools/brush.hpp"
#include "editor_tools/paint_modes.hpp"

namespace editor_tools
{
    class SelectionTool : public Brush
    {
        public:
            SelectionTool(sgc_view::MapView &mapView, sections::TilesetSection &tilesetSection, const bool &allowChunkCreation, bool &needsRedraw);

            void Execute(file::MapDocument& mapDocument, sgc::tile::TilePosition2D cursorPosition) override;
    };
}