#pragma once

#include <sgc/asset/assetdeserializer.hpp>
#include "file/document_info.hpp"

namespace sgc::asset {

    template<>
    struct AssetDeserializer<file::MapDocumentInfo> {
        static file::MapDocumentInfo Deserialize(const std::vector<uint8_t>& data);
    };
}