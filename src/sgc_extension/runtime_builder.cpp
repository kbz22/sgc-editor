#include "sgc_extension/runtime_builder.hpp"
#include <sgc/asset/chunkedtilestorageasset.hpp>
#include <sgc/data/chunkedtilestorage.hpp>

std::unique_ptr<file::MapDocument> sgc::asset::RuntimeBuilder<file::MapDocument>::Build(const file::MapDocumentInfo& info, const sgc::asset::MapAsset& mapAsset)
{
    auto document = std::make_unique<file::MapDocument>(
        info.name,
        mapAsset.tilesetId,
        info.mapAssetId
    );    

    auto layerManager = document->GetLayerManager();

    for(const auto &layerAsset : mapAsset.layers) {
        std::shared_ptr<sgc::data::ITileStorage> tileStorage;

        if(std::holds_alternative<sgc::asset::ChunkedTileStorageAsset>(layerAsset.tileStorage)) {
            auto &chunkedStorage = std::get<sgc::asset::ChunkedTileStorageAsset>(layerAsset.tileStorage);
            tileStorage = sgc::asset::RuntimeBuilder<sgc::data::ChunkedTileStorage>::Build(chunkedStorage);
        } else {
            throw std::runtime_error("Unknown tile storage type");
        }

        program::LayerItem layerItem = {
            tileStorage,
            layerAsset.name,
            true,
            255
        };

        layerManager->AddLayer(layerItem);
    }

    return std::move(document);
}