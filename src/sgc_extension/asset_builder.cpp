#include "sgc_extension/asset_builder.hpp"
#include <sgc/asset/tilestorageasset.hpp>
#include <sgc/asset/chunkedtilestorageasset.hpp>

sgc::asset::MapAsset sgc::asset::AssetBuilder<sgc::asset::MapAsset, file::MapDocument>::Build(const file::MapDocument& document) 
{
    auto layers = document.GetLayerManager()->GetLayers();

    std::vector<sgc::asset::MapLayerAsset> layerAssets;
    layerAssets.reserve(layers.size());

    for(auto &layer : layers) {
        sgc::asset::MapLayerAsset layerAsset = {
            layer.name,
            {},
            sgc::asset::AssetBuilder<sgc::asset::TileStorageAsset, sgc::data::ITileStorage>::Build(*layer.storage)
        };

        layerAssets.push_back(layerAsset);
    }

    sgc::asset::MapAsset mapAsset = {
        document.GetTilesetAssetId(),
        layerAssets
    };

    return mapAsset;
}

file::MapDocumentInfo sgc::asset::AssetBuilder<file::MapDocumentInfo, file::MapDocument>::Build(const file::MapDocument& document)
{
    file::MapDocumentInfo documentInfo = {
        file::DocumentType::Map,
        document.GetName(),
        document.GetMapAssetId()
    };

    return documentInfo;
}

file::TilesetDocumentInfo sgc::asset::AssetBuilder<file::TilesetDocumentInfo, file::TilesetDocument>::Build(const file::TilesetDocument& document)
{
    file::TilesetDocumentInfo documentInfo = {
        file::DocumentType::Tileset,
        document.GetName(),
        document.GetImageAssetId(),
        document.GetTilesetAssetId()
    };

    return documentInfo;
}