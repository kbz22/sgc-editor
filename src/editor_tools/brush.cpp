#include "editor_tools/brush.hpp"

editor_tools::Brush::Brush(sgc::graphics::Rectangle& selectionRectangleOnTileset) :
    m_selectionRectangleOnTileset{selectionRectangleOnTileset}
{}

editor_tools::PaintMode editor_tools::Brush::GetPaintMode() const
{
    return m_paintMode;
}

bool editor_tools::Brush::NeedsRedraw() const
{
    return m_needsRedraw;
}

void editor_tools::Brush::SetPaintMode(PaintMode paintMode)
{
    m_paintMode = paintMode;
}

void editor_tools::Brush::SetCheckTileBeforePainting(bool check)
{
    m_checkTileBeforePainting = check;
}

void editor_tools::Brush::PaintExecuteChange(
    file::MapDocument& mapDocument,
    sgc::graphics::Tileset& tileset,
    sgc::tile::TilePosition2D tilePosition,
    sgc::tile::TilePosition2D cursorPositionOnTileset,
    sgc::tile::TileSize2D tileSize
)
{
    if(m_paintCommand == nullptr) {
        auto layerManager = mapDocument.GetLayerManager();
        m_paintCommand = std::make_unique<command::PaintCommand>(
            &mapDocument,
            layerManager->GetActiveLayerIndex()
        );
    }

    if(m_selectionStart == nullptr) {
        m_selectionStart = std::make_unique<sgc::tile::TilePosition2D>(tilePosition);
        m_lastSelection = std::make_unique<sgc::tile::TilePosition2D>(tilePosition);
    }
    else if(m_lastSelection->x == tilePosition.x && m_lastSelection->y == tilePosition.y) {
        m_needsRedraw = false;
        return;
    }
    else {
        m_lastSelection->x = tilePosition.x;
        m_lastSelection->y = tilePosition.y;
    }

    m_clearTileId = tileset.TileIdCount();

    switch(m_paintMode)
    {
        case PaintMode::Brush:
            PaintStroke(
                *this,
                mapDocument,
                tileset,
                tilePosition,
                cursorPositionOnTileset,
                tileSize
            );
            m_needsRedraw = true;
            break;

        case PaintMode::Rectangle:
            PaintRectangle(
                *this,
                mapDocument,
                tileset,
                tilePosition,
                cursorPositionOnTileset,
                tileSize
            );
            m_needsRedraw = true;
            break;

        case PaintMode::Fill:
            PaintFill(
                *this,
                mapDocument,
                tileset,
                tilePosition,
                cursorPositionOnTileset,
                tileSize
            );
            m_needsRedraw = true;
            break;

        default:
            m_needsRedraw = false;
            break;
    }
    
}

void editor_tools::Brush::PaintCommitChanges(file::MapDocument& mapDocument)
{
    auto commandManager = mapDocument.GetCommandManager();

    if(m_paintCommand != nullptr) {
        commandManager->Commit(std::move(m_paintCommand));
    }

    m_paintCommand.reset();
    m_selectionStart.reset();
}
