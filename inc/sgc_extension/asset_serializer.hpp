#pragma once

#include <sgc/asset/assetserializer.hpp>
#include "file/document_info.hpp"

namespace sgc::asset {

    template<>
    struct AssetSerializer<file::MapDocumentInfo>
    {
        static std::vector<uint8_t> Serialize(const file::MapDocumentInfo& documentInfo);
    };

    template<>
    struct AssetSerializer<file::TilesetDocumentInfo>
    {
        static std::vector<uint8_t> Serialize(const file::TilesetDocumentInfo& document);
    };
}