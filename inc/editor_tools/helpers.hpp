#pragma once

#include <optional>
#include <sgc/tile/tile.hpp>
#include "program/sgc_tileset.hpp"

namespace editor_tools
{
    std::optional<sgc::tile::TileId> GetTileId(
        // sgc::graphics::Tileset& tileset,
        program::SgcTileset &tileset,
        sgc::tile::TilePosition2D cursorPositionOnTileset,
        sgc::tile::TileSize2D selectionSizeInTiles,
        sgc::tile::TilePosition2D cursorPositionOnMap,
        sgc::tile::TilePosition2D cursorOrginPosition
    );
}