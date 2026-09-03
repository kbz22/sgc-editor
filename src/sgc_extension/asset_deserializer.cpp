#include "sgc_extension/asset_deserializer.hpp"
#include <sgc/data/serialization.hpp>

file::MapDocumentInfo sgc::asset::AssetDeserializer<file::MapDocumentInfo>::Deserialize(const std::vector<uint8_t>& data) 
{
    sgc::data::BinaryReader reader{
        data.data(),
        0
    };
    file::MapDocumentInfo info;
    
    reader.Read<file::DocumentType>();
    info.name = reader.ReadString();
    info.mapAssetId = reader.Read<sgc::data::AssetId>();

    return info;
}

file::TilesetDocumentInfo sgc::asset::AssetDeserializer<file::TilesetDocumentInfo>::Deserialize(const std::vector<uint8_t>& data)
{
    sgc::data::BinaryReader reader{
        data.data(),
        0
    };
    file::TilesetDocumentInfo info;

    reader.Read<file::DocumentType>();
    info.name = reader.ReadString();
    info.tilesetAssetId = reader.Read<sgc::data::AssetId>();
    info.imageAssetId = reader.Read<sgc::data::AssetId>();

    return info;
}