#include "sgc_extension/asset_deserializer.hpp"
#include <sgc/data/serialization.hpp>

file::MapDocumentInfo sgc::asset::AssetDeserializer<file::MapDocumentInfo>::Deserialize(const std::vector<uint8_t>& data) 
{
    sgc::data::BinaryReader reader{
        data.data(),
        0
    };
    file::MapDocumentInfo info;
    
    switch(packageVersion)
    {
        case 1:
            reader.Read<uint8_t>();
            break;

        default:
            reader.Read<file::EditorDataType>();
            break;
    }
    
    info.name = reader.ReadString();
    info.mapAssetId = reader.Read<sgc::data::AssetId>();
    info.symbolGroupsAssetId = reader.Read<sgc::data::AssetId>();

    return info;
}

file::TilesetDocumentInfo sgc::asset::AssetDeserializer<file::TilesetDocumentInfo>::Deserialize(const std::vector<uint8_t>& data)
{
    sgc::data::BinaryReader reader{
        data.data(),
        0
    };
    file::TilesetDocumentInfo info;

    switch(packageVersion)
    {
        case 1:
            reader.Read<uint8_t>();
            break;

        default:
            reader.Read<file::EditorDataType>();
            break;
    }

    info.name = reader.ReadString();    
    info.imageAssetId = reader.Read<sgc::data::AssetId>();
    info.tilesetAssetId = reader.Read<sgc::data::AssetId>();

    return info;
}

file::SymbolGroupsInfo sgc::asset::AssetDeserializer<file::SymbolGroupsInfo>::Deserialize(const std::vector<uint8_t>& data)
{
    sgc::data::BinaryReader reader{
        data.data(),
        0
    };
    file::SymbolGroupsInfo info;

    switch(packageVersion)
    {
        case 1:
            reader.Read<uint8_t>();
            break;

        default:
            reader.Read<file::EditorDataType>();
            break;
    }

    auto groupCount = reader.Read<size_t>();

    for(size_t i = 0; i < groupCount; ++i) 
    {
        file::SymbolGroupInfo groupInfo;
        groupInfo.groupName = reader.ReadString();
        groupInfo.symbolSize = reader.Read<sgc::tile::TilePosition2D>();

        auto symbolCount = reader.Read<size_t>();
        for(size_t j = 0; j < symbolCount; ++j) 
        {
            auto position = reader.Read<sgc::tile::TilePosition2D>();
            groupInfo.positions.push_back(position);
        }

        info.groups.push_back(groupInfo);
    }

    return info;
}

uint32_t sgc::asset::PackageVersionDependent::packageVersion = data::c_packageVersion;

void sgc::asset::PackageVersionDependent::SetDefaultVersion()
{
    packageVersion = data::c_packageVersion;
}