#include "editor_tools/editor_tool_manager.hpp"
#include "command/delete_chunk_command.hpp"
#include "sections/map_section.hpp"
#include <sgc/data/chunkedtilestorage.hpp>

editor_tools::EditorToolManager::EditorToolManager(sgc_view::MapView &mapView, sections::MapSection &mapSection, sections::TilesetSection &tilesetSection) :
    m_tilesetSection{
        tilesetSection
    },
    m_paintBrush{
        mapView,
        tilesetSection,
        m_allowChunkCreation,
        m_needsRedraw,
        m_clearTile
    },
    m_rectangularBrush{
        mapView,
        tilesetSection,
        m_allowChunkCreation,
        m_needsRedraw,
    },
    m_fillTool{
        mapView,
        tilesetSection,
        m_allowChunkCreation,
        m_needsRedraw,
    },
    m_chunkRemover{
        m_needsRedraw
    },
    m_tilePickerTool{
        tilesetSection
    },
    m_selectionTool{
        mapView,
        tilesetSection,
        m_allowChunkCreation,
        m_needsRedraw
    },
    m_selectionMoveTool{
        mapView,
        mapSection,
        m_selectionMode,
        m_clearTile,
        m_allowChunkCreation
    },
    m_scatterBrush{
        mapView,
        tilesetSection,
        m_allowChunkCreation,
        m_needsRedraw,
        m_clearTile,
        4
    }
{
    SetActiveTool();
}

editor_tools::PaintMode editor_tools::EditorToolManager::GetPaintMode() const
{
    return m_paintMode;
}

editor_tools::SelectionMode editor_tools::EditorToolManager::GetSelectionMode() const
{
    return m_selectionMode;
}

bool editor_tools::EditorToolManager::NeedsRedraw() const
{
    return m_needsRedraw;
}

bool editor_tools::EditorToolManager::NeedsUpdatesPeriodically() const
{
    return m_activeTool->GetUpdateRestriction() == EditorToolUpdateRestriction::Time;
}

void editor_tools::EditorToolManager::SetPaintMode(PaintMode paintMode)
{
    m_paintMode = paintMode;
    SetActiveTool();
}

void editor_tools::EditorToolManager::SetSelectionMode(SelectionMode selectionMode)
{
    m_selectionMode = selectionMode;
    SetActiveTool();
}

void editor_tools::EditorToolManager::SetAllowChunkCreation(bool check)
{
    m_allowChunkCreation = check;
}

void editor_tools::EditorToolManager::Execute(file::MapDocument& mapDocument, sgc::tile::TilePosition2D cursorPosition)
{
    if(m_activeTool == nullptr)
        return;

    switch(m_activeTool->GetUpdateRestriction())
    {
        case EditorToolUpdateRestriction::Position:
        {
            if(m_lastPosition == nullptr)
            {
                m_lastPosition = std::make_unique<sgc::tile::TilePosition2D>(cursorPosition);                
            }
            else if(cursorPosition == *m_lastPosition)
            {
                m_needsRedraw = false;
                return;
            }

            *m_lastPosition = cursorPosition;
            break;
        }

        case EditorToolUpdateRestriction::Time:
        {
            if(!m_toolTimeIntervals.contains(m_activeTool))
            {
                m_toolTimeIntervals[m_activeTool] = std::chrono::milliseconds{1};
            }

            auto elapsedTime = duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - m_lastCheckTimestamp
            );

            if(elapsedTime < m_toolTimeIntervals[m_activeTool])
            {                
                m_needsRedraw = false;
                return;
            }

            m_lastCheckTimestamp = std::chrono::steady_clock::now();
            break;
        }

        default:
            return;
    }
    
    m_clearTile = m_tilesetSection.GetClearTileId();
    m_activeTool->Execute(mapDocument, cursorPosition);
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

        case PaintMode::Rectangle:
        {
            m_activeTool = &m_rectangularBrush;
            return;
        }

        case PaintMode::Fill:
        {
            m_activeTool = &m_fillTool;
            return;
        }

        case PaintMode::ChunkRemover:
        {
            m_activeTool = &m_chunkRemover;
            return;
        }

        case PaintMode::TilePicker:
        {
            m_activeTool = &m_tilePickerTool;
            return;
        }

        case PaintMode::Select:
        {
            m_activeTool = &m_selectionTool;
            break;
        }

        case PaintMode::MoveSelection:
        {
            m_activeTool = &m_selectionMoveTool;
            break;
        }

        case PaintMode::Scatter:
        {
            m_activeTool = &m_scatterBrush;
            break;
        }

        default:
        {
            m_activeTool = nullptr;
            return;
        }
    }    
}

void editor_tools::EditorToolManager::SetModeDelay(PaintMode paintMode, std::chrono::milliseconds interval)
{
    switch(paintMode)
    {
        case PaintMode::Scatter:
        {
            m_toolTimeIntervals[&m_scatterBrush] = interval;
            return;
        }

        default:
            return;
    }
}