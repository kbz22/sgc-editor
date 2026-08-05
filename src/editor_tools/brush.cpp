#include "editor_tools/brush.hpp"
#include "command/delete_chunk_command.hpp"
#include <sgc/data/chunkedtilestorage.hpp>

editor_tools::Brush::Brush(sgc::graphics::Rectangle& selectionRectangleOnTileset) :
    m_selectionRectangleOnTileset{selectionRectangleOnTileset}
{}

editor_tools::PaintMode editor_tools::Brush::GetPaintMode() const
{
    return m_paintMode;
}

editor_tools::EraserMode editor_tools::Brush::GetEraserMode() const
{
    return m_eraserMode;
}

bool editor_tools::Brush::NeedsRedraw() const
{
    return m_needsRedraw;
}

void editor_tools::Brush::SetPaintMode(PaintMode paintMode)
{
    m_paintMode = paintMode;
}

void editor_tools::Brush::SetEraserMode(EraserMode eraserMode)
{
    m_eraserMode = eraserMode;
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
    if(m_eraserMode == EraserMode::DeleteChunk)
    {
        auto layerManager = mapDocument.GetLayerManager();
        auto activeLayer = layerManager->GetActiveLayerIndex();

        auto tileCheck = layerManager->GetLayers()[activeLayer].storage->GetTileAt(tilePosition);

        if(tileCheck.has_value()) 
        {
            auto chunkPosition = sgc::data::ChunkedTileStorage::GetChunkCoordAt(tilePosition);
            auto deleteChunkCommand = std::make_unique<command::DeleteChunkCommand>(chunkPosition);

            auto commandManager = mapDocument.GetCommandManager();
            commandManager->Execute(std::move(deleteChunkCommand));

            m_needsRedraw = true;
        }
        
        return;
    }

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
            if(m_eraserMode == EraserMode::ClearTile) {
                EraseStroke(
                    *this,
                    mapDocument,
                    tileset,
                    tilePosition,
                    cursorPositionOnTileset,
                    tileSize
                );
            }
            else {
                PaintStroke(
                    *this,
                    mapDocument,
                    tileset,
                    tilePosition,
                    cursorPositionOnTileset,
                    tileSize
                );
            }
            m_needsRedraw = true;
            break;

        case PaintMode::Rectangle:
            if(m_eraserMode != EraserMode::None) {
                EraseRectangle(
                    *this,
                    mapDocument,
                    tileset,
                    tilePosition,
                    cursorPositionOnTileset,
                    tileSize
                );
            }
            else {
                PaintRectangle(
                    *this,
                    mapDocument,
                    tileset,
                    tilePosition,
                    cursorPositionOnTileset,
                    tileSize
                );
            }
            m_needsRedraw = true;
            break;

        case PaintMode::Fill:
            if(m_eraserMode != EraserMode::None) {
                EraseFill(
                    *this,
                    mapDocument,
                    tileset,
                    tilePosition,
                    cursorPositionOnTileset,
                    tileSize
                );
            }
            else {
                PaintFill(
                    *this,
                    mapDocument,
                    tileset,
                    tilePosition,
                    cursorPositionOnTileset,
                    tileSize
                );
            }
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
