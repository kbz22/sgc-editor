#pragma once

#include "editor_tools/brush.hpp"

namespace editor_tools
{
    class PaintBrush : public Brush
    {
        private:
            sgc::tile::TileId &m_clearTileId;
            
        public:
            PaintBrush(sgc_view::MapView &mapView, sections::TilesetSection &tilesetSection, const bool &allowChunkCreation, bool &needsRedraw, sgc::tile::TileId &clearTileId);

            void Execute(file::MapDocument& mapDocument, sgc::tile::TilePosition2D cursorPosition) override;
            // void Commit(file::MapDocument& mapDocument) override;
    };
}