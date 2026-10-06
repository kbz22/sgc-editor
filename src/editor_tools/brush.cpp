#include "editor_tools/paint_brush.hpp"
#include "editor_tools/helpers.hpp"

editor_tools::Brush::Brush(sgc_view::MapView &mapView, sections::TilesetSection &tilesetSection, const bool &allowChunkCreation, bool &needsRedraw) :
    m_paintCommand{nullptr},
    m_cursorOrigin{nullptr},
    m_mapView{mapView},
    m_tilesetSection{tilesetSection},
    m_allowChunkCreation{allowChunkCreation},
    m_needsRedraw{needsRedraw}
{}

void editor_tools::Brush::Commit(file::MapDocument& mapDocument)
{
    auto commandManager = mapDocument.GetCommandManager();

    if(m_bulkCommand != nullptr && !m_bulkCommand->Empty() && m_paintCommand != nullptr) 
    {
        m_bulkCommand->AddCommand(std::move(m_paintCommand));
        commandManager->Commit(std::move(m_bulkCommand));
    }
    else if(m_paintCommand != nullptr) 
    {
        commandManager->Commit(std::move(m_paintCommand));
    }    

    m_paintCommand.reset();
    m_cursorOrigin.reset();
}