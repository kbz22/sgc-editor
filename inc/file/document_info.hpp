#pragma once

#include <string>
#include <sgc/data/asset.hpp>
#include <sgc/tile/tile.hpp>

namespace file {

    enum class EditorDataType : uint8_t
    {
        Map,
        Tileset,
        SymbolGroups,
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