#pragma once

#include "editor_tools/brush.hpp"
#include <vector>

namespace editor_tools
{
    class ScatterBrush : public Brush
    {
        private:
            sgc::math::ival m_radius;
            sgc::tile::TileId &m_clearTileId;
            std::vector<sgc::tile::TilePosition2D> m_candidateOffsets;

            void GenerateCandidateOffsets();

        public:
            ScatterBrush(sgc_view::MapView &mapView, sections::TilesetSection &tilesetSection, const bool &allowChunkCreation, bool &needsRedraw, sgc::tile::TileId &clearTileId, sgc::math::ival radiusValue);

            EditorToolUpdateRestriction GetUpdateRestriction() override;

            void Execute(file::MapDocument& mapDocument, sgc::tile::TilePosition2D cursorPosition) override;
    };
}