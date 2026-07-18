#pragma once

#include <sgc/math/vector.hpp>
#include <sgc/tile/tile.hpp>
#include <optional>

namespace command {

    struct TileChange {
        sgc::math::vec2 position;
        std::optional<sgc::tile::TileId> previousTileId;
        std::optional<sgc::tile::TileId> newTileId;
    };

}