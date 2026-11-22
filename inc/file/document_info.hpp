#pragma once

#include <string>
#include <sgc/data/asset.hpp>
#include <sgc/tile/tile.hpp>

namespace file {

    enum class EditorDataType : uint16_t
    {
        Map = 1,
        Tileset = 2,
        SymbolGroups = 3,
    };

    struct MapDocumentInfo
    {
        const EditorDataType type = EditorDataType::Map;
        std::wstring name;
        sgc::data::AssetId mapAssetId;
        sgc::data::AssetId symbolGroupsAssetId = 0;
    };

    struct TilesetDocumentInfo
    {
        const EditorDataType type = EditorDataType::Tileset;
        std::wstring name;
        sgc::data::AssetId imageAssetId;
        sgc::data::AssetId tilesetAssetId;
    };

    struct SymbolGroupInfo
    {
        std::wstring groupName;
        sgc::tile::TileSize2D symbolSize;
        std::vector<sgc::tile::TilePosition2D> positions;
    };

    struct SymbolGroupsInfo
    {
        const EditorDataType type = EditorDataType::SymbolGroups;        
        std::vector<SymbolGroupInfo> groups;
    };

}