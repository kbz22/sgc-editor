#pragma once

#include <sgc/asset/assetdeserializer.hpp>
#include <sgc/data/package.hpp>
#include "file/document_info.hpp"

namespace sgc::asset {

    struct PackageVersionDependent {
        static uint32_t packageVersion;

        static void SetDefaultVersion();
    };

    template<>
    struct AssetDeserializer<file::MapDocumentInfo> : public PackageVersionDependent {
        static file::MapDocumentInfo Deserialize(const std::vector<uint8_t>& data);
    };

    template<>
    struct AssetDeserializer<file::TilesetDocumentInfo> : public PackageVersionDependent {
        static file::TilesetDocumentInfo Deserialize(const std::vector<uint8_t>& data);
    };

    template<>
    struct AssetDeserializer<file::SymbolGroupsInfo> : public PackageVersionDependent {
        static file::SymbolGroupsInfo Deserialize(const std::vector<uint8_t>& data);
    };
}