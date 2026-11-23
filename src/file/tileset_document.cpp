#include "file/tileset_document.hpp"

file::TilesetDocument::TilesetDocument(std::wstring name, sgc::data::AssetId tilesetId, sgc::data::AssetId imageId) :
    m_name{name},
    m_tilesetId{tilesetId},
    m_imageId{imageId}
{
    m_tilesetDocumentId = sgc::data::HashAsset(L"tileset.doc."+m_name);
    m_metadataAssetId = sgc::data::HashAsset(L"tileset.meta."+m_name);
}

file::TilesetDocument::TilesetDocument(std::wstring name, sgc::data::AssetId tilesetId, sgc::data::AssetId imageId, sgc::data::AssetId metadataId) :
    m_name{name},
    m_tilesetId{tilesetId},
    m_imageId{imageId},
    m_metadataAssetId{metadataId}
{
    m_tilesetDocumentId = sgc::data::HashAsset(L"tileset.doc."+m_name);
}


sgc::data::AssetId file::TilesetDocument::GetTilesetAssetId() const
{
    return m_tilesetId;
}

const std::wstring& file::TilesetDocument::GetName() const
{
    return m_name;
}

bool file::TilesetDocument::IsContainer() const
{
    return false;
}

bool file::TilesetDocument::IsDirty() const
{
    return false;
}

sgc::data::AssetId file::TilesetDocument::GetImageAssetId() const
{
    return m_imageId;
}

sgc::data::AssetId file::TilesetDocument::GetTilesetDocumentAssetId() const
{
    return m_tilesetDocumentId;
}

sgc::data::AssetId file::TilesetDocument::GetMetadataAssetId() const
{
    return m_metadataAssetId;
}

bool file::TilesetDocument::IsActivable() const
{
    return false;
}

file::ListableType file::TilesetDocument::GetListableType() const
{
    return ListableType::TilesetDocument;
}

sgc::asset::AssetMetadata file::TilesetDocument::GenerateMetadataAsset() const
{
    using sgc::asset::AssetMetadata, sgc::asset::MetadataType;

    AssetMetadata asset;

    asset.data[m_tilesetId][MetadataType::Name] = m_name + L".tileset";
    asset.data[m_tilesetDocumentId][MetadataType::Name] = m_name + L".doc";
    asset.data[m_imageId][MetadataType::Name]  = m_name + L".image";
    asset.data[m_metadataAssetId][MetadataType::Name] = m_name + L".meta";

    return asset;
}