#include "editor_tools/paint_brush.hpp"
#include "editor_tools/helpers.hpp"

editor_tools::Brush::Brush(sgc_view::MapView &mapView, sections::TilesetSection &tilesetSection, const bool &allowChunkCreation) :
    m_paintCommand{nullptr},
    m_cursorOrigin{nullptr},
    m_mapView{mapView},
    m_tilesetSection{tilesetSection},
    m_allowChunkCreation{allowChunkCreation}
{}

void editor_tools::Brush::Commit(file::MapDocument& mapDocument)
{
    auto commandManager = mapDocument.GetCommandManager();

    if(m_paintCommand != nullptr) {
        commandManager->Commit(std::move(m_paintCommand));
    }

    m_paintCommand.reset();
    m_cursorOrigin.reset();
}