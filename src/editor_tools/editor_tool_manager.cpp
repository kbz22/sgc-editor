#include "editor_tools/editor_tool_manager.hpp"
#include "command/delete_chunk_command.hpp"
#include "sections/map_section.hpp"
#include <sgc/data/chunkedtilestorage.hpp>

editor_tools::EditorToolManager::EditorToolManager(sgc_view::MapView &mapView, sections::TilesetSection &tilesetSection) :
    m_paintBrush{
        mapView,
        tilesetSection,
        m_allowChunkCreation,
    }
{
    SetActiveTool();
}

editor_tools::PaintMode editor_tools::EditorToolManager::GetPaintMode() const
{
    return m_paintMode;
}

editor_tools::EraserMode editor_tools::EditorToolManager::GetEraserMode() const
{
    return m_eraserMode;
}

editor_tools::SelectionMode editor_tools::EditorToolManager::GetSelectionMode() const
{
    return m_selectionMode;
}

bool editor_tools::EditorToolManager::NeedsRedraw() const
{
    return m_needsRedraw;
}

void editor_tools::EditorToolManager::SetPaintMode(PaintMode paintMode)
{
    m_paintMode = paintMode;
}

void editor_tools::EditorToolManager::SetEraserMode(EraserMode eraserMode)
{
    m_eraserMode = eraserMode;
}

void editor_tools::EditorToolManager::SetSelectionMode(SelectionMode selectionMode)
{
    m_selectionMode = selectionMode;
}

void editor_tools::EditorToolManager::SetCheckTileBeforePainting(bool check)
{
    m_allowChunkCreation = check;
}

void editor_tools::EditorToolManager::Execute(file::MapDocument& mapDocument, sgc::tile::TilePosition2D cursorPosition)
{
    if(m_activeTool == nullptr)
        return;

    if(m_lastPosition == nullptr){
        m_lastPosition = std::make_unique<sgc::tile::TilePosition2D>(cursorPosition);
    }
    else if(cursorPosition == *m_lastPosition){
        m_needsRedraw = false;
        return;
    }

    m_activeTool->Execute(mapDocument, cursorPosition);
    m_needsRedraw = true;
}

void editor_tools::EditorToolManager::Commit(file::MapDocument &mapDocument)
{
    if(m_activeTool == nullptr)
        return;

    m_lastPosition.reset();

    m_activeTool->Commit(mapDocument);    
}

void editor_tools::EditorToolManager::SetActiveTool()
{
    m_lastPosition.reset();

    switch(m_paintMode)
    {
        case PaintMode::Brush:
        {
            m_activeTool = &m_paintBrush;
            return;
        }

        default:
        {
            m_activeTool = nullptr;
            return;
        }
    }    
}