#pragma once

#include "file/map_document.hpp"
#include "file/tileset_document.hpp"
#include "file/document_info.hpp"
#include <sgc/asset/mapasset.hpp>
#include <sgc/asset/assetbuilder.hpp>
#include <vector>

namespace sgc::asset {

    template<>
    struct AssetBuilder<MapAsset, file::MapDocument> 
    {
        static MapAsset Build(const file::MapDocument& document);
    };

    template<>
    struct AssetBuilder<file::MapDocumentInfo, file::MapDocument>
    {
        static file::MapDocumentInfo Build(const file::MapDocument& document);        
    };

    template<>
    struct AssetBuilder<file::TilesetDocumentInfo, file::TilesetDocument>
    {
        static file::TilesetDocumentInfo Build(const file::TilesetDocument& document);        
    };
}