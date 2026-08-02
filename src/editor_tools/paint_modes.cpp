#include "editor_tools/brush.hpp"

void editor_tools::PaintStroke(
    Brush& brush,
    file::MapDocument& mapDocument,
    sgc::graphics::Tileset& tileset,
    sgc::tile::TilePosition2D tilePosition,
    sgc::tile::TilePosition2D cursorPositionOnTileset,
    sgc::tile::TileSize2D tileSize
)
{
    auto currentLayer = mapDocument.GetCurrentLayerStorage();

    if(currentLayer == nullptr) {
        return;
    }    

    auto absmod = [](sgc::math::ival value, sgc::math::ival mod) -> sgc::math::ival {
        return ((value % mod) + mod) % mod;
    };

    for(auto x = 0; x < tileSize.x; ++x){
        for(auto y = 0; y < tileSize.y; ++y)
        {
            auto tileMapX = tilePosition.x + x;
            auto tileMapY = tilePosition.y + y;

            auto deltaX = absmod(
                tileMapX - brush.m_selectionStart->x,
                tileSize.x
            );

            auto deltaY = absmod(
                tileMapY - brush.m_selectionStart->y,
                tileSize.y
            );

            auto tileId = tileset.ToTileId(
                cursorPositionOnTileset.x + deltaX,
                cursorPositionOnTileset.y + deltaY
            );

            if( !brush.m_checkTileBeforePainting || currentLayer->GetTileAt({ tileMapX, tileMapY }).has_value()) {

                command::TileChange change{
                    { tileMapX, tileMapY },
                    currentLayer->GetTileAt({ tileMapX, tileMapY }),
                    tileId
                };
                
                brush.m_paintCommand->ExecuteTileChange(change);

            } 
        }
    }
}

void editor_tools::PaintRectangle(
    Brush& brush,
    file::MapDocument& mapDocument,
    sgc::graphics::Tileset& tileset,
    sgc::tile::TilePosition2D tilePosition,
    sgc::tile::TilePosition2D cursorPositionOnTileset,
    sgc::tile::TileSize2D tileSize
)
{
    auto currentLayer = mapDocument.GetCurrentLayerStorage();

    if(currentLayer == nullptr) {
        return;
    }

    brush.m_paintCommand->UndoTileChanges();

    auto absmod = [](sgc::math::ival value, sgc::math::ival mod) -> sgc::math::ival {
        return ((value % mod) + mod) % mod;
    };

    auto abs = [](sgc::math::ival value) -> sgc::math::ival {
        return value < 0 ? -value : value;
    };

    auto rectStartX = std::min(brush.m_selectionStart->x, tilePosition.x);
    auto rectEndX = std::max(brush.m_selectionStart->x, tilePosition.x);
    auto rectStartY = std::min(brush.m_selectionStart->y, tilePosition.y);
    auto rectEndY = std::max(brush.m_selectionStart->y, tilePosition.y);

    for(auto x = rectStartX; x <= rectEndX; ++x){
        for(auto y = rectStartY; y <= rectEndY; ++y)
        {
            auto tileMapX = x;
            auto tileMapY = y;

            auto deltaX = absmod(
                tileMapX - brush.m_selectionStart->x,
                tileSize.x
            );

            auto deltaY = absmod(
                tileMapY - brush.m_selectionStart->y,
                tileSize.y
            );

            auto tileId = tileset.ToTileId(
                cursorPositionOnTileset.x + deltaX,
                cursorPositionOnTileset.y + deltaY
            );

            auto getTile = currentLayer->GetTileAt({ tileMapX, tileMapY });

            if( !brush.m_checkTileBeforePainting || getTile.has_value()) {

                command::TileChange change{
                    { tileMapX, tileMapY },
                    getTile,
                    tileId
                };
                
                brush.m_paintCommand->ExecuteTileChange(change);

            } 
        }
    }
}