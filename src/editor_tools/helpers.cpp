#include "editor_tools/helpers.hpp"

std::optional<sgc::tile::TileId> editor_tools::GetTileId(
    sgc::graphics::Tileset& tileset,
    sgc::tile::TilePosition2D cursorPositionOnTileset,
    sgc::tile::TileSize2D selectionSizeInTiles,
    sgc::tile::TilePosition2D cursorPositionOnMap,
    sgc::tile::TilePosition2D cursorOrginPosition
)
{    
    auto deltaX = sgc::math::AbsMod(
        cursorPositionOnMap.x - cursorOrginPosition.x,
        selectionSizeInTiles.x
    );

    auto deltaY = sgc::math::AbsMod(
        cursorPositionOnMap.y - cursorOrginPosition.y,
        selectionSizeInTiles.y
    );

    return tileset.ToTileId(
        cursorPositionOnTileset.x + deltaX,
        cursorPositionOnTileset.y + deltaY
    );
}