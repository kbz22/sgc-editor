#include "file/document_serializer.hpp"
#include "file/map_document.hpp"
#include "file/tileset_document.hpp"
#include <sgc/asset/mapasset.hpp>
#include <sgc/asset/mapassetserializer.hpp>
#include <sgc/asset/mapassetdeserializer.hpp>
#include <sgc/asset/tilestorageassetbuilder.hpp>
#include <sgc/asset/chunkedtilestoragebuilder.hpp>

std::vector<uint8_t> file::DocumentSerializer<file::MapDocument>::Serialize(file::MapDocument* document)
{
    auto layers = document->GetLayerManager()->GetLayers();

    std::vector<sgc::asset::MapLayerAsset> layerAssets;
    layerAssets.reserve(layers.size());

    for(auto &layer : layers) {
        sgc::asset::MapLayerAsset layerAsset = {
            layer.name,
            {},
            sgc::asset::AssetBuilder<sgc::asset::TileStorageAsset>::Build(*layer.storage)
        };

        layerAssets.push_back(layerAsset);
    }

    sgc::asset::MapAsset mapAsset = {
        document->GetTilesetAssetId(),
        layerAssets
    };

    auto bytes = sgc::asset::AssetSerializer<sgc::asset::MapAsset>::Serialize(
        mapAsset
    );
    
    return std::move(bytes);
}

std::unique_ptr<file::MapDocument> file::DocumentSerializer<file::MapDocument>::Deserialize(const std::vector<uint8_t>& bytes)
{
    std::unique_ptr<file::MapDocument> document = nullptr;

    
    
    return document;
}

std::vector<uint8_t> file::DocumentSerializer<file::TilesetDocument>::Serialize(file::TilesetDocument* document)
{
    std::vector<uint8_t> serializedData;

    return serializedData;
}

std::unique_ptr<file::TilesetDocument> file::DocumentSerializer<file::TilesetDocument>::Deserialize(const std::vector<uint8_t>& bytes)
{
    std::unique_ptr<file::TilesetDocument> document = nullptr;

    //! to do

    return document;
}