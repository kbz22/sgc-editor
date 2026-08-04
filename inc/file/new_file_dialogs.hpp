#pragma once

#include "new_map_dialog.h"
#include "new_tileset_dialog.h"
#include <sgc/data/asset.hpp>
#include <string>
#include <filesystem>

namespace file {

    struct NewMapDialogResult
    {
        std::wstring mapName;
        sgc::data::AssetId tilesetId;
    };

    struct NewTilesetDialogResult
    {
        std::wstring tilesetName;
        std::filesystem::path imagePath;
        int tileWidth;
        int tileHeight;
    };

}