#include "file/tileset_document.hpp"

file::TilesetDocument::TilesetDocument(std::wstring name, sgc::data::AssetId tilesetId, sgc::data::AssetId imageId) :
    name{name},
    m_tilesetId{tilesetId},
    m_imageId{imageId}
{}

sgc::data::AssetId file::TilesetDocument::GetTilesetAssetId() const
{
    return m_tilesetId;
}

const std::wstring& file::TilesetDocument::GetName() const
{
    return name;
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

bool file::TilesetDocument::IsActivable() const
{
    return false;
}