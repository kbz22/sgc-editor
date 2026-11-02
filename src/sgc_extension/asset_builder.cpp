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
        file::EditorDataType::Map,
        document.GetName(),
        document.GetMapAssetId()
    };

    return documentInfo;
}

file::TilesetDocumentInfo sgc::asset::AssetBuilder<file::TilesetDocumentInfo, file::TilesetDocument>::Build(const file::TilesetDocument& document)
{
    file::TilesetDocumentInfo documentInfo = {
        file::EditorDataType::Tileset,
        document.GetName(),
        document.GetImageAssetId(),
        document.GetTilesetAssetId()
    };

    return documentInfo;
}

file::SymbolGroupsInfo sgc::asset::AssetBuilder<file::SymbolGroupsInfo, file::MapDocument>::Build(const file::MapDocument& document)
{
    file::SymbolGroupsInfo documentInfo = {
        file::EditorDataType::SymbolGroups,
        {}
    };

    file::SymbolGroupInfo groupInfo;
    auto symbolGroups = document.GetSymbolManager()->GetSymbolGroups();

    for(auto &group : symbolGroups) 
    {
        groupInfo.groupName = group.groupName;
        groupInfo.symbolSize = group.group.GetSize();
        groupInfo.positions = group.group.GetPositions();

        documentInfo.groups.push_back(groupInfo);
    }

    return documentInfo;
}