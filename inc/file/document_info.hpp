#pragma once

#include <string>
#include <sgc/data/asset.hpp>

namespace file {

    enum class DocumentType : uint8_t
    {
        Map,
        Tileset
    };

    struct MapDocumentInfo
    {
        const DocumentType type = DocumentType::Map;
        std::wstring name;
        sgc::data::AssetId mapAssetId;
    };

    struct TilesetDocumentInfo
    {
        const DocumentType type = DocumentType::Tileset;
        std::wstring name;
        sgc::data::AssetId imageAssetId;
        sgc::data::AssetId tilesetAssetId;
    };

}