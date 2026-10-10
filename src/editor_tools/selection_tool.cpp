#include "editor_tools/selection_tool.hpp"

editor_tools::SelectionTool::SelectionTool(sgc_view::MapView &mapView, sections::TilesetSection &tilesetSection, const bool &allowChunkCreation, bool &needsRedraw) :
    Brush{mapView, tilesetSection, allowChunkCreation, needsRedraw}
{}

void editor_tools::SelectionTool::Execute(file::MapDocument& mapDocument, sgc::tile::TilePosition2D cursorPosition)
{
    if(m_cursorOrigin == nullptr)
    {
        m_cursorOrigin = std::make_unique<sgc::tile::TilePosition2D>(cursorPosition);
        m_lastPosition = std::make_unique<sgc::tile::TilePosition2D>(cursorPosition);
    }
    else if(*m_lastPosition == cursorPosition)
    {
        m_needsRedraw = false;
        return;
    }
    else
    {
        *m_lastPosition = cursorPosition;
    }

    auto tileSize = m_mapView.GetTileSize();

    m_mapView.SetSelectionPositionInPixels({
        static_cast<sgc::math::ival>(std::min(cursorPosition.x, m_cursorOrigin->x)) * tileSize.x,
        static_cast<sgc::math::ival>(std::min(cursorPosition.y, m_cursorOrigin->y)) * tileSize.y
    });
    m_mapView.SetSelectionSizeInPixels({
        static_cast<sgc::math::ival>(std::abs(cursorPosition.x - m_cursorOrigin->x) + 1) * tileSize.x,
        static_cast<sgc::math::ival>(std::abs(cursorPosition.y - m_cursorOrigin->y) + 1) * tileSize.y
    });
}