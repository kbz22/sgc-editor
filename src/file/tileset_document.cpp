#include "file/tileset_document.hpp"

file::TilesetDocument::TilesetDocument(std::wstring name, sgc::data::AssetId tilesetId) :
    name{name},
    m_tilesetId{tilesetId}
{}

sgc::data::AssetId file::TilesetDocument::GetTilesetAssetId() const
{
    return m_tilesetId;
}

const std::wstring& file::TilesetDocument::GetName() const
{
    return name;
}