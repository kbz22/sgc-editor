#include "sgc_extension/asset_serializer.hpp"
#include <sgc/data/serialization.hpp>
#include <memory>

std::vector<uint8_t> sgc::asset::AssetSerializer<file::MapDocumentInfo>::Serialize(const file::MapDocumentInfo& documentInfo)
{
    sgc::data::BinaryWriter writer;

    writer.Write(documentInfo.type);
    writer.WriteString(documentInfo.name);
    writer.Write(documentInfo.mapAssetId);

    return std::move(writer.buffer);
}

std::vector<uint8_t> sgc::asset::AssetSerializer<file::TilesetDocumentInfo>::Serialize(const file::TilesetDocumentInfo& documentInfo)
{
    sgc::data::BinaryWriter writer;

    writer.Write(documentInfo.type);
    writer.WriteString(documentInfo.name);
    writer.Write(documentInfo.imageAssetId);
    writer.Write(documentInfo.tilesetAssetId);

    return std::move(writer.buffer);
}